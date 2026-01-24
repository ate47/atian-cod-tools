#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class ScriptParseTreeWorker : public Worker {

		void GenDefaultXHashes(fastfile::FastFileContext* ctx) override {
			if (!ctx) {
				return; // no base ones
			}

			const char* mapname{ ctx->ffname };
			std::string gamemode{ mapname };
			size_t fdd{ gamemode.find('_') };
			if (fdd != std::string::npos) {
				gamemode.resize(fdd);
			}

			const char* suffixes[]{
				"", "_fx", "_scripted", "_sound"
			};
			const char* types[]{
				"gsc", "csc"
			};

			for (const char* suffix : suffixes) {
				for (const char* type : types) {
					const char* str{ utils::va("scripts/%s/%s%s.%s", gamemode, ctx->ffname, suffix, type) };
					hashutils::AddPrecomputed(hash::Hash64(str), str, true);
				}
			}
		}

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTree {
				XHash name;
				byte* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x20);
			ScriptParseTree* spt{ (ScriptParseTree*)ptr };


			std::filesystem::path outDir{ opt.m_output / "bo4" / "spt" };

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/script_{:x}.gscc", *(uint64_t*)spt->buffer, spt->name.name) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump scriptparsetree {} 0x{:x} ({})", outFile.string(), spt->len, hashutils::ExtractTmpScript(spt->name.name));
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			if (!opt.disableScriptsDecomp) {

				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / "bo4" / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = fastfile::GetCurrentContext().gscPlatform;
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / "bo4" / "source" };
				std::string outSourceStr{ outSource.string() };
				gdctx.opt.m_outputDir = outSourceStr.data();
				gdctx.opt.m_formatter = opt.gscFormatter;

				std::vector<byte> scriptData;
				byte* alignedAlloc{ utils::Aligned(spt->buffer, 0x20) };

				if (spt->buffer != alignedAlloc) {
					// we clone the buffer to align it
					utils::Allocate(scriptData, spt->len + 0x20);
					alignedAlloc = utils::Aligned(scriptData.data(), 0x20);
					std::memcpy(alignedAlloc, spt->buffer, spt->len);
				}

				int r{ tool::gsc::DecompileGsc(alignedAlloc, spt->len, outFile, gdctx) };

				if (r) {
					LOG_ERROR("Error when decompiling script");
				}
			}

		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTPARSETREE };
}