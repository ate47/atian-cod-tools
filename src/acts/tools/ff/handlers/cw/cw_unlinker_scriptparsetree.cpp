#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct ScriptParseTree {
				CWXHash name;
				void* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x18);
			ScriptParseTree* asset{ (ScriptParseTree*)ptr };

			std::filesystem::path outFile{ opt.m_output / "cw" / "spt" / std::format("script_{:x}.gscc", asset->name) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump ScriptParseTree {} len: 0x{:x}", outFile.string(), asset->len);

			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_SCRIPTPARSETREE };
}