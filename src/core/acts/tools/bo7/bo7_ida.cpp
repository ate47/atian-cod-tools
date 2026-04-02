#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <deps/idc_builder.hpp>
#include <hook/module_mapper.hpp>
#include <games/cod/asset_names.hpp>
#include <game_data.hpp>
#include <utils/data_utils.hpp>

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


	std::filesystem::path GetCfgDir() {
		std::string scanpath{ core::config::GetString("data.dir", "") };
		if (scanpath.empty()) {
			return utils::GetProgDir() / "data" / "asset_types.json";
		}
		return std::filesystem::path{ scanpath } / "asset_types.json";
	}

    int bo7_ida(int argc, const char* argv[]) {
		bool help{};
		bool ignoreAssetType{};
		bool ignoreGsc{};
		bool ignoreDB{};
		cli::options::CliOptions opts{};
		opts
			.addOption(&help, "show help", "--help", "", "-h")
			.addOption(&ignoreAssetType, "ignore AssetType", "--no-asset", "", "-A")
			.addOption(&ignoreGsc, "ignore Gsc functions", "--no-gsc", "", "-G")
			.addOption(&ignoreDB, "ignore DB functions", "--no-db", "", "-D")
			;

		if (!opts.ComputeOptions(2, argc, argv) || help || opts.NotEnoughParam(2)) {
			opts.PrintOptions();
			return !help ? tool::BAD_USAGE : tool::OK;
		}

		core::config::Config assetTypesConfig{ GetCfgDir() };

		if (!assetTypesConfig.SyncConfig(false)) {
			LOG_WARNING("Can't read {}, missing asset type information", assetTypesConfig.configFile.string());
		}


        std::filesystem::path exe{ opts[0] };
        std::filesystem::path idcFile{ opts[1] };

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
		void* Scr_RegisterGscFunction{};
		void* Scr_RegisterGscMethod{};
		std::vector<void(*)(byte*)> Scr_RegisterGens;
		void* Load_AssetType{};
		void* Load_AssetHeader{};

		if (!ignoreGsc) {
			Scr_RegisterGscFunction = game.GetPointer("Scr_RegisterGscFunction");
			Scr_RegisterGscMethod = game.GetPointer("Scr_RegisterGscMethod");
			game.ApplyNullScans("gsc");
			Scr_RegisterGens = game.GetPointerArray<void(*)(byte*)>("$Scr_RegisterGens");
		}
		if (!ignoreDB) {
			Load_AssetType = game.GetPointer("Load_AssetType");
			Load_AssetHeader = game.GetPointer("Load_AssetHeader");
		}


		if (scan.foundMissing) {
			LOG_ERROR("Missing pattern when searching functions");
			return tool::BASIC_ERROR;
		}

		if (!ignoreGsc) {
			hashutils::ReadDefaultFile();
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

			idcBuilder.AddStruct("scrContext_t");

			for (size_t i = 0; i < iwGscData.pool.size(); i++) {
				std::vector<GscFuncContainerDataType>& pool{ iwGscData.pool[i] };
				for (size_t j = 0; j < pool.size(); j++) {
					GscFuncContainerDataType& container{ pool[j] };
					for (size_t k = 0; k < container.data.count; k++) {
						GscFunction& func{ container.data.functions[k] };
						idcBuilder.AddAddressEx(func.func, utils::va("GScr_%s", hashutils::ExtractTmp(container.isMethod ? "function" : "method", func.name)), "SN_CHECK | SN_NOWARN", "void func(scrContext_t* ctx)");
					}
				}
			}
		}

		games::cod::asset_names::AssetNames<size_t, size_t> assets{};
		LOG_INFO("Loading asset names");
		game.InitAssetNames(assets);

		if (!ignoreAssetType) {
			size_t count{ assets.TypesCount() };
			deps::idc_builder::IdcEnumId assetTypeId{ idcBuilder.AddEnum("AssetType", true) };
			for (size_t i = 0; i < count; i++) {
				idcBuilder.AddEnumMember(assetTypeId, utils::va("ASSET_TYPE_%s", assets.GetCppName(i)), (int64_t)i);
			}
			idcBuilder.AddEnumMember(assetTypeId, "ASSET_TYPE_COUNT", (int64_t)count);
		}


		if (!ignoreDB) {
			LOG_INFO("Loading db assets load");

			if (!hook::library::ScanMatch(Load_AssetHeader, "41 8b 01 85 c0 0f 84 ?? ?? ?? ??")) {
				throw std::runtime_error("Load_AssetHeader has an invalid structure for code data generation");
			}
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
				const char* typeName{ assetTypesConfig.GetCString(std::format("{}.name", name)) };
				const char* typeCdecl{ assetTypesConfig.GetCString(std::format("{}.cdecl", name)) };
				const char* typeCdeclflags{ assetTypesConfig.GetCString(std::format("{}.cdeclflags", name)) };

				std::string strName{};
				if (!typeName) {
					LOG_DEBUG("missing type {}", name);
					strName = name;
					strName[0] = toupper(strName[0]); // make the first char upper for the type
					typeName = strName.data();
				}

				if (typeCdecl) {
					idcBuilder.AddCDecl(typeCdecl, typeCdeclflags);
				}
				else {
					idcBuilder.AddStruct(typeName);
				}

				constexpr const char loadPtrStart[] = "4C 8B DC 49 89 73 18 57 48 83 EC ?? 49 8B F0 48 8B F9 84 D2 74 40 48 8B 05 ?? ?? ?? ?? 4D 8D 4B E8 4D 89 43 E8 48 8B D1";
				constexpr const char loadCall[] = "B2 01 48 8B CF 4C 8B C0 48 89 06 E8";
				if (hook::library::ScanMatch(func, loadPtrStart)) {
					void* loadFunc{};
					for (size_t i = sizeof(loadPtrStart) / 3; i < 313; i++) {
						void* loadPart{ (byte*)func + i };

						if (hook::library::ScanMatch(loadPart, loadCall)) {
							loadFunc = hook::library::GetRelative<int32_t, void*>(loadPart, sizeof(loadCall) / 3);
							break;
						}
					}

					if (loadFunc) {
						idcBuilder.AddAddress(loadFunc,
							utils::va("Load_%s", typeName),
							utils::va("void Load_%s(DBLoadCtx* ctx, bool atStreamStart, %s* %s)", typeName, typeName, name)
						);
					}
					else {
						LOG_WARNING("Invalid second pattern for Load_{}Ptr, can't deduce Load_{} for {}", typeName, typeName, hook::library::CodePointer{ func });
					}
				}
				else {
					LOG_WARNING("Invalid pattern for Load_{}Ptr, can't deduce Load_{}", typeName, typeName, hook::library::CodePointer{ func });
				}

				idcBuilder.AddAddress(func,
					utils::va("Load_%sPtr", typeName),
					utils::va("void Load_%sPtr(DBLoadCtx* ctx, bool atStreamStart, %s** handle)", typeName, typeName)
				);

			}
		}

		idcBuilder.WriteIdcFile(idcFile);
		LOG_INFO("Created {}", idcFile.string());

        return tool::OK;
    }
    ADD_TOOL(bo7_ida, "bo7", " [exe] [dump]", "Create idc file from an exe dump", bo7_ida);
}