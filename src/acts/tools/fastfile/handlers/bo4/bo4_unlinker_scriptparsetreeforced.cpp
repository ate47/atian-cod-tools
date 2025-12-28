#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class ScriptParseTreeForcedWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTreeForced {
				XHash name;
				uint32_t gscCount;
				uint32_t cscCount;
				XHash* gscScripts;
				XHash* cscScripts;
			};

			ScriptParseTreeForced* asset{ (ScriptParseTreeForced*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "scriptparsetreeforced" / std::format("{}.csv", n) };

			std::filesystem::create_directories(outFile.parent_path());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't open {}", outFile.string());
				return;
			}

			LOG_INFO("Dump scriptparsetreeforced {}", outFile.string());

			os << "vm,name";

			for (size_t i = 0; i < asset->gscCount; i++) {
				os << "\nserver,#" << hashutils::ExtractTmp("script", asset->gscScripts[i].name);
			}
			for (size_t i = 0; i < asset->cscCount; i++) {
				os << "\nclient,#" << hashutils::ExtractTmp("script", asset->cscScripts[i].name);
			}
		}
	};

	utils::MapAdder<ScriptParseTreeForcedWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_SCRIPTPARSETREEFORCED };
}