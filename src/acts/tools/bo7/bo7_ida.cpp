#include <includes.hpp>
#include <deps/idc_builder.hpp>
#include <hook/module_mapper.hpp>
#include <games/cod/asset_names.hpp>
#include <game_data.hpp>

namespace {

	struct GscFunction {
		uint64_t name;
		void(__fastcall* func)(void* vm);
		bool devFunc;
		byte minArgs;
		byte maxArgs;
	};

	struct GscFuncContainerData {
		GscFunction* functions;
		uint64_t count;
	};
	struct GscFuncContainerDataType {
		GscFuncContainerData data;
		bool isMethod;
	};
	static struct IWGscData {
		std::vector<std::vector<GscFuncContainerDataType>> pool{};
		std::vector<GscFuncContainerDataType>* functions{};
	} iwGscData;

	template<bool isMethod>
	void IWLoadFuncs(void* container, GscFuncContainerData* cfg) {
		iwGscData.functions->emplace_back(*cfg, isMethod);
	}


    int bo7_ida(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

        std::filesystem::path exe{ argv[2] };
        std::filesystem::path idcFile{ argv[3] };

        hook::module_mapper::Module mod{ true };

        LOG_INFO("Loading module {}", exe.string());
        if (!mod.Load(exe) || !mod) {
            LOG_ERROR("Can't load module");
            return tool::BASIC_ERROR;
        }


		deps::idc_builder::IdcBuilder idcBuilder{};

		hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
		acts::game_data::GameData game{ "bo7" };
		game.SetScanContainer(&scan);
		game.AddTypesToIdc(idcBuilder);
		game.ScanAllToIdc(idcBuilder);

		iwGscData.pool.clear();
		LOG_INFO("Loading data...");

		void* Scr_RegisterGscFunction{ game.GetPointer("Scr_RegisterGscFunction") };
		void* Scr_RegisterGscMethod{ game.GetPointer("Scr_RegisterGscMethod") };
		void* Scr_RegisterGscIgnored{ game.GetPointer("$Scr_RegisterGscIgnored") };
		void* Scr_RegisterGscSortIgnored{ game.GetPointer("$Scr_RegisterGscSortIgnored") };
		void* Load_AssetType{ game.GetPointer("Load_AssetType") };
		void* Load_AssetHeader{ game.GetPointer("Load_AssetHeader") };
		std::vector<void(*)(byte*)> Scr_RegisterGens{ game.GetPointerArray<void(*)(byte*)>("$Scr_RegisterGens") };


		if (scan.foundMissing) {
			LOG_ERROR("Missing pattern when searching functions");
			return tool::BASIC_ERROR;
		}

		if (!hook::library::ScanMatch(Load_AssetHeader, "41 8b 01 85 c0 0f 84 ?? ?? ?? ??")) {
			throw std::runtime_error("Load_AssetHeader has an invalid structure for code data generation");
		}

		hashutils::ReadDefaultFile();

		hook::memory::Nulled(Scr_RegisterGscIgnored);
		hook::memory::Nulled(Scr_RegisterGscSortIgnored);
		hook::memory::RedirectJmp(Scr_RegisterGscFunction, IWLoadFuncs<false>);
		hook::memory::RedirectJmp(Scr_RegisterGscMethod, IWLoadFuncs<true>);

		constexpr size_t len = 0x100000;

		std::unique_ptr<byte[]> tmpData{ std::make_unique<byte[]>(len) };

		for (auto Scr_RegisterGen : Scr_RegisterGens) {
			LOG_INFO("Loading gsc registry function: {}", hook::library::CodePointer{ Scr_RegisterGen });

			iwGscData.pool.emplace_back();
			iwGscData.functions = &iwGscData.pool[iwGscData.pool.size() - 1];

			Scr_RegisterGen(tmpData.get());
		}

		for (size_t i = 0; i < iwGscData.pool.size(); i++) {
			std::vector<GscFuncContainerDataType>& pool{ iwGscData.pool[i] };
			for (size_t j = 0; j < pool.size(); j++) {
				GscFuncContainerDataType& container{ pool[j] };
				for (size_t k = 0; k < container.data.count; k++) {
					GscFunction& func{ container.data.functions[k] };
					idcBuilder.AddAddressEx(func.func, utils::va("GScr_%s", hashutils::ExtractTmp(container.isMethod ? "function" : "method", func.name)), "SN_CHECK | SN_NOWARN");
				}
			}
		}

		static games::cod::asset_names::AssetNames<size_t, size_t> assets{ "physicssfxeventasset", "string" };
		LOG_INFO("Loading asset names");
		assets.InitMap(mod);

		size_t count{ assets.TypesCount() };
		deps::idc_builder::IdcEnumId assetTypeId{ idcBuilder.AddEnum("AssetType", true) };
		for (size_t i = 0; i < count; i++) {
			idcBuilder.AddEnumMember(assetTypeId, utils::va("ASSET_TYPE_%s", assets.GetCppName(i)), (int64_t)i);
		}
		idcBuilder.AddEnumMember(assetTypeId, "ASSET_TYPE_COUNT", (int64_t)count);


		LOG_INFO("Loading db assets load");
		std::unordered_map<size_t, byte*> loadPtrFuncs{};
		byte* assetLoader{ (byte*)Load_AssetHeader + 3 };
		while (true) {
			if (*assetLoader == 0xc3) {
				break; // ret
			}

			size_t assetId;
			if (assetLoader[0] == 0x85 && assetLoader[1] == 0xc0) { // test eax,eax
				assetId = 0;
				assetLoader += 2;
			}
			else if (assetLoader[0] == 0x83 && assetLoader[1] == 0xf8) { // cmp eax,0xXX
				assetId = (size_t)assetLoader[2];
				assetLoader += 3;
			}
			else if (assetLoader[0] == 0x3d) { // cmp eax,0xXXXXXXXX
				assetId = *(uint32_t*)&assetLoader[1];
				assetLoader += 5;
			}
			else {
				throw std::runtime_error(std::format("Unknown cmp asm 0x{:x} {}", (int)*assetLoader, hook::library::CodePointer{ assetLoader }));
			}

			if (assetLoader[0] != 0x0f || assetLoader[1] != 0x84) {
				throw std::runtime_error(std::format("Unknown jmp asm 0x{:x} {}", (int)*assetLoader, hook::library::CodePointer{ assetLoader }));
			}

			loadPtrFuncs[assetId] = hook::library::GetRelative<int32_t>(assetLoader, 2);
			assetLoader += 6;


			if (assetId == std::string::npos) {
				break;
			}
		}

		for (auto& [assetId, func] : loadPtrFuncs) {
			const char* name{ assets.GetTypeName(assetId) };
			char* typeName{ utils::CloneString(name) };
			*typeName = toupper(*typeName); // make the first char upper for the type

			idcBuilder.AddStruct(typeName);
			idcBuilder.AddAddress(func,
				utils::va("Load_%sPtr", typeName),
				utils::va("void Load_%sPtr(DBLoadCtx* ctx, bool atStreamStart, %s** handle)", typeName, typeName)
			);

		}

		idcBuilder.WriteIdcFile(idcFile);
		LOG_INFO("Created {}", idcFile.string());

        return tool::OK;
    }
    ADD_TOOL(bo7_ida, "bo7", " [exe] [dump]", "Create idc file from an exe dump", bo7_ida);
}