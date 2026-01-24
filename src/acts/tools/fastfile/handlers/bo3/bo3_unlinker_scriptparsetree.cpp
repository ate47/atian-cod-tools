#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo3.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo3;

	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTree {
				const char* name;
				int32_t len;
				int32_t pad;
				byte* buffer;
			}; static_assert(sizeof(ScriptParseTree) == 0x18);
			ScriptParseTree* spt{ (ScriptParseTree*)ptr };

			if (!FixupName(spt->name)) {
				return;
			}


			std::filesystem::path outDir{ opt.m_output / "bo3" / "spt" };

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}c", *(uint64_t*)spt->buffer, spt->name) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump scriptparsetree {} 0x{:x} ({})", outFile.string(), spt->len, spt->name);
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			if (!opt.disableScriptsDecomp) {

				tool::gsc::GscDecompilerGlobalContext gdctx{};

				std::filesystem::path outSourceGDB{ opt.m_output / "bo3" / "source" / "gdb" };
				std::string outSourceGDBStr{ outSourceGDB.string() };
				gdctx.opt.m_generateGdbData = true;
				gdctx.opt.m_generateGdbBaseData = false;
				gdctx.opt.m_dbgOutputDir = outSourceGDBStr.data();
				gdctx.opt.m_platform = tool::gsc::opcode::PLATFORM_PC; // todo: use header type
				gdctx.opt.m_dcomp = true;
				std::filesystem::path outSource{ opt.m_output / "bo3" / "source" };
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

	utils::MapAdder<ScriptParseTreeWorker, bo3::pool::T7XAssetType, Worker> impl{ GetWorkers(), bo3::pool::T7_ASSET_TYPE_SCRIPTPARSETREE };
}