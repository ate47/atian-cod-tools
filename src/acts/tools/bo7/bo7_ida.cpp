#include <includes.hpp>
#include <deps/idc_builder.hpp>
#include <hook/module_mapper.hpp>
#include <games/cod/asset_names.hpp>

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

		iwGscData.pool.clear();
		LOG_INFO("loading data...");

		void* Scr_RegisterGscFunction{ scan.ScanSingle("4C 8B 4A 08 4D 85 C9 74 35", "Scr_RegisterGscFunction").location };
		void* Scr_RegisterGscMethod{ scan.ScanSingle("4C 8B 4A 08 4D 85 C9 74 3F", "Scr_RegisterGscMethod").location };
		void* Scr_RegisterGscIgnored{ scan.ScanSingle("40 53 48 83 EC ?? 33 C0 66 C7 81 ?? ?? ?? ?? ?? ?? 48 89 81").location };
		void* Scr_RegisterGscSortIgnored{ scan.ScanSingle("40 53 48 83 EC 20 80 B9 ?? ?? ?? ?? ?? 48 8B D9 75 20").location };
		std::vector<hook::library::ScanResult> Scr_RegisterGens{ mod->Scan("48 89 5C 24 08 55 48 8B EC 48 83 EC 40 48 8B D9 E8 ? ? ? ? ? ? ? ?") };
		void* Load_AssetType{ scan.ScanSingle("E8 ?? ?? ?? ?? 4C 8D 43 08 4C 8B CB 33 D2 48 8B CF E8 ?? ?? ?? ?? 48 8B 5C 24 50", "Load_AssetType").GetRelative<int32_t>(1) };
		void* Load_AssetHeader{ scan.ScanSingle("E8 ?? ?? ?? ?? 4C 8D 43 08 4C 8B CB 33 D2 48 8B CF E8 ?? ?? ?? ?? 48 8B 5C 24 50", "Load_AssetHeader").GetRelative<int32_t>(18) };


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

		for (hook::library::ScanResult& Scr_RegisterGen : Scr_RegisterGens) {
			LOG_INFO("Loading gsc registry function: {}", hook::library::CodePointer{ Scr_RegisterGen.location });

			iwGscData.pool.emplace_back();
			iwGscData.functions = &iwGscData.pool[iwGscData.pool.size() - 1];

			Scr_RegisterGen.GetPtr<void(*)(byte*)>()(tmpData.get());
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

		games::cod::asset_names::AssetNames<size_t, size_t> assets{ "physicssfxeventasset", "string" };
		LOG_INFO("Loading asset names");
		assets.InitMap(mod);

		size_t count{ assets.TypesCount() };
		deps::idc_builder::IdcEnumId assetTypeId{ idcBuilder.AddEnum("AssetType") };
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

		idcBuilder.AddStruct("DBLoadCtx");
		idcBuilder.AddAddress(Load_AssetType, "Load_AssetType", "void Load_AssetType(DBLoadCtx* ctx, AssetType* type)\");");
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"DB_AddAsset\"), \"void* DB_AddAsset(DBLoadCtx* ctx, AssetType type, void** handle)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"DB_AddAssetRef\"), \"void* DB_AddAssetRef(AssetType type, uint64_t name, void* strName)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"LoadStreamTA\"), \"bool LoadStreamTA(DBLoadCtx * context, bool atStreamStart, void* ptr, int64_t len)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"Load_String\"), \"void Load_String(DBLoadCtx * context, char** pstr)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"Load_StringName\"), \"void Load_StringName(DBLoadCtx * context, char** pstr)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"Load_CustomScriptString\"), \"void Load_CustomScriptString(DBLoadCtx * context, uint32_t * pstr)\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"DB_LoadStreamOffset\"), \"void DB_LoadStreamOffset(DBLoadCtx * ctx, uint64_t val, void** pptr);\");\n";
		//utils::Padding(idaScript, 1) << "SetType(LocByName(\"DB_RegisterStreamOffset\"), \"void DB_RegisterStreamOffset(DBLoadCtx * ctx, uint64_t val, void* ptr);\");\n";

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

		idcBuilder.AddHeader("This file was generated using atian-cod-tools' bo7_ida tool");

		idcBuilder.WriteIdcFile(idcFile);
		LOG_INFO("created {}", idcFile.string());

        return tool::OK;
    }
    ADD_TOOL(bo7_ida, "bo7", " [exe] [dump]", "Create idc file from an exe dump", bo7_ida);
}