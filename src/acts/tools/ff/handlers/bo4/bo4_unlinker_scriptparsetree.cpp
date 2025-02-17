#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTree {
				XHash name;
				byte* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x20);
			ScriptParseTree* spt{ (ScriptParseTree*)ptr };


			std::filesystem::path outDir{ opt.m_output / "bo4" / "spt" };

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/script_{:x}.gscc", *(uint64_t*)spt->buffer, spt->name.hash) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump scriptparsetree {} 0x{:x} ({})", outFile.string(), spt->len, hashutils::ExtractTmpScript(spt->name.hash));
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTPARSETREE };
}