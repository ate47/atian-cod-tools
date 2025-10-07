#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6_sp.hpp>
#include <tools/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo6sp;
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

	class ImplWorker : public Worker {
		using Worker::Worker;

		void GenDefaultXHashes(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				return; // no base ones
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

			std::filesystem::path outFile{ opt.m_output / "bo6sp" / "gscobj" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump gscobj {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}

			if (!opt.disableScriptsDecomp) {
				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / "bo6sp" / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = fastfile::GetCurrentContext().gscPlatform;
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / "bo6sp" / "source" };
				std::string outSourceStr{ outSource.string() };
				gdctx.opt.m_outputDir = outSourceStr.data();
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

			std::filesystem::path outFile{ opt.m_output / "bo6sp" / "gscobj" / std::format("script_{:x}.gsc.gdb", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump gscgdb {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10HashAssetType, Worker> impl{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_GSCOBJ, sizeof(GscObj) };
	utils::MapAdder<ImplWorkerGdb, bo6::T10HashAssetType, Worker> implGdb{ GetWorkers(), bo6::T10HashAssetType::T10H_ASSET_GSCGDB, sizeof(GscGdb) };
}