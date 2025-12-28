#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTreeDBG {
				XHash name;
				int32_t gdbLen;
				int32_t srcLen;
				void* gdb;
				const char* src;
			}; static_assert(sizeof(ScriptParseTreeDBG) == 0x28);
			ScriptParseTreeDBG* spt{ (ScriptParseTreeDBG*)ptr };


			std::filesystem::path outDir{ opt.m_output / "bo4" / "sptdbg" };

			
			const char* filename{ hashutils::ExtractPtr(spt->name) };
			if (!filename) {
				filename = utils::va("script_%llx.gsc", spt->name.name);
			}

			if (spt->gdb && spt->gdbLen) {
				std::filesystem::path outFile{ outDir / std::format("{}gdbc", filename) };
				std::filesystem::create_directories(outFile.parent_path());
				LOG_INFO("Dump scriptparsetreedbg gdb {} 0x{:x} ({})", outFile.string(), spt->gdbLen, hashutils::ExtractTmpScript(spt->name));
				if (!utils::WriteFile(outFile, spt->gdb, spt->gdbLen)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
					return;
				}
			}

			if (spt->src && spt->srcLen) {
				std::filesystem::path outFile{ outDir / filename };
				std::filesystem::create_directories(outFile.parent_path());
				LOG_INFO("Dump scriptparsetreedbg src {} 0x{:x} ({})", outFile.string(), spt->srcLen, filename);
				if (!utils::WriteFile(outFile, spt->src, spt->srcLen)) {
					LOG_ERROR("Error when dumping {}", outFile.string());
					return;
				}
			}

		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG };
}