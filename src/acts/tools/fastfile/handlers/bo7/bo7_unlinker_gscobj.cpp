#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo7;
	using namespace tool::bo7;

	struct GscObj {
		uint64_t name;
		uint32_t len;
		byte* buffer;
	}; static_assert(sizeof(GscObj) == 0x18);
	struct GscGdb {
		uint64_t name;
		uint32_t len;
		byte* buffer;
	}; static_assert(sizeof(GscGdb) == 0x18);


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

	const char* LookupLocalizedImpl(uint64_t hash) {
		return fastfile::GetCurrentOptions().GetTranslation(hash);
	}
	size_t GetRva(void* loc) {
		hook::library::Library library{ hook::library::GetLibraryInfo(loc) };
		if (!library) {
			return 0;
		}
		return (size_t)((byte*)loc - (byte*)*library);
	}

	class ImplWorker : public Worker {
		using Worker::Worker;

		void PreLoadWorker(fastfile::FastFileContext* ctx) override {
			if (!ctx) {

				// dump the gsc funcs
				fastfile::FastFileOption& opt{ fastfile::GetCurrentOptions() };
				if (!opt.largeCodeSection) {
					return;
				}
				hook::module_mapper::Module& mod{ opt.GetGameModule() };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };

				iwGscData.pool.clear();

				void* Scr_RegisterGscFunction{ scan.ScanSingle("4C 8B 4A 08 4D 85 C9 74 35", "Scr_RegisterGscFunction").location };
				void* Scr_RegisterGscMethod{ scan.ScanSingle("4C 8B 4A 08 4D 85 C9 74 3F", "Scr_RegisterGscMethod").location };
				void* Scr_RegisterGscIgnored{ scan.ScanSingle("40 53 48 83 EC ?? 33 C0 66 C7 81 ?? ?? ?? ?? ?? ?? 48 89 81").location };
				void* Scr_RegisterGscSortIgnored{ scan.ScanSingle("40 53 48 83 EC 20 80 B9 ?? ?? ?? ?? ?? 48 8B D9 75 20").location };
				std::vector<hook::library::ScanResult> Scr_RegisterGens{ mod->Scan("48 89 5C 24 08 55 48 8B EC 48 83 EC 40 48 8B D9 E8 ? ? ? ? ? ? ? ?") };

				if (scan.foundMissing) {
					LOG_ERROR("Missing pattern when searching gsc function dump");
					return;
				}
				hook::memory::Nulled(Scr_RegisterGscIgnored);
				hook::memory::Nulled(Scr_RegisterGscSortIgnored);
				hook::memory::RedirectJmp(Scr_RegisterGscFunction, IWLoadFuncs<false>);
				hook::memory::RedirectJmp(Scr_RegisterGscMethod, IWLoadFuncs<false>);

				constexpr size_t len = 0x100000;

				std::unique_ptr<byte[]> tmpData{ std::make_unique<byte[]>(len) };

				for (hook::library::ScanResult& Scr_RegisterGen : Scr_RegisterGens) {
					LOG_TRACE("Loading registry function: {}", hook::library::CodePointer{ Scr_RegisterGen.location });

					iwGscData.pool.emplace_back();
					iwGscData.functions = &iwGscData.pool[iwGscData.pool.size() - 1];

					Scr_RegisterGen.GetPtr<void(*)(byte*)>()(tmpData.get());
					LOG_TRACE("done");
				}

				{
					utils::OutFileCE gscCsvOs{ opt.m_output / gamePath / "code" / std::format("{}_gsc.csv", handlerId) };
					utils::OutFileCE idaScript{ opt.m_output / gamePath / "code" / std::format("{}_gsc.idc", handlerId) };
					gscCsvOs << "pool,container,type,name,minargs,maxargs,type,offset";
					idaScript
						<< "#include <idc/idc.idc>\n\n";
					idaScript << "static main() {\n";
					utils::Padding(idaScript, 1) << "auto base = get_imagebase();\n";

					for (size_t i = 0; i < iwGscData.pool.size(); i++) {
						std::vector<GscFuncContainerDataType>& pool{ iwGscData.pool[i] };
						for (size_t j = 0; j < pool.size(); j++) {
							GscFuncContainerDataType& container{ pool[j] };
							for (size_t k = 0; k < container.data.count; k++) {
								GscFunction& func{ container.data.functions[k] };
								gscCsvOs
									<< "\n" << std::dec
									<< i << ","
									<< j << ","
									<< (container.isMethod ? "method" : "function") << ","
									<< hashutils::ExtractTmp("function", func.name) << ","
									<< (int)func.minArgs << ","
									<< (int)func.maxArgs << ","
									<< (func.devFunc ? "dev" : "common") << ","
									<< hook::library::CodePointer{ func.func };
								size_t rva{ GetRva(func.func) };
								utils::Padding(idaScript, 1) << "MakeNameEx(base + 0x" << std::hex << rva << ", \"GScr_" << hashutils::ExtractTmp(container.isMethod ? "function" : "method", func.name) << "\", SN_CHECK | SN_NOWARN);\n";
							}
						}
					}
					idaScript << "}\n";
				}
				return;
			}

			const char* gamemodes[]{ "mp", "zm", "sp" };

			const char* names[][2]{
				{ "", "" },
				{ "", "_fx" },
				{ "", "_lighting" },
				{ "", "_precache" },
				{ "", "_code" },
				{ "", "_audio" },
				{ "gen/", "_art" }
			};

			for (const char* gamemode : gamemodes) {
				for (auto& [prefix, suffix] : names) {
					const char* str{ utils::va("scripts/%s/maps/%s/%s%s%s.gsc", gamemode, ctx->ffname, prefix, ctx->ffname, suffix) };
					hashutils::AddPrecomputed(hash::HashIWAsset(str), str, true);
				}
			}

		}

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			GscObj* asset{ (GscObj*)ptr };

			std::filesystem::path outFile{ opt.m_output / gamePath / "gscobj" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump gscobj {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (!opt.disableScriptsDecomp) {
				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / gamePath / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = tool::gsc::opcode::PLATFORM_BO7;
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / gamePath / "source" };
				std::string outSourceStr{ outSource.string() };
				gdctx.opt.m_outputDir = outSourceStr.data();
				gdctx.opt.m_formatter = opt.gscFormatter;
				if (opt.translation) {
					gdctx.opt.LookupLocalizedFunc = LookupLocalizedImpl;
				}
				gdctx.stringsLoc = GetXStrings();

				int r{ tool::gsc::DecompileGsc(asset->buffer, asset->len, outFile, gdctx) };

				if (r) {
					LOG_ERROR("Error when decompiling script");
				}
			}
		}
	};

	class ImplWorkerGdb : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			GscGdb* asset{ (GscGdb*)ptr };

			std::filesystem::path outFile{ opt.m_output / gamePath / "gscobj" / std::format("script_{:x}.gsc.gdb", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump gscgdb {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_GSCOBJ, sizeof(GscObj) };
	utils::MapAdder<ImplWorkerGdb, SatHashAssetType, Worker> implGdb{ GetWorkers(), SatHashAssetType::SATH_ASSET_GSCGDB, sizeof(GscGdb) };
}