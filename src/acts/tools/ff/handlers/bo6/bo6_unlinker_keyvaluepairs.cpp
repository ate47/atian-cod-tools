#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>

namespace {

	using namespace fastfile::handlers::bo6;
	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			struct KeyValuePairs {
				uint64_t name;
				const char** kv;
				int32_t count;
			};
			
			static_assert(sizeof(KeyValuePairs) == 0x18);
			KeyValuePairs* asset{ (KeyValuePairs*)ptr };


			std::filesystem::path outFile{ opt.m_output / "bo6" / "source" / "tables" / "keyvaluepairs" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump keyvaluepairs {}", outFile.string());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			for (size_t i = 0; i < asset->count; i++) {
				os << asset->kv[i] << "\n";
			}
		}
	};

	utils::MapAdder<ImplWorker, bo6::T10RAssetType, Worker> impl{ GetWorkers(), bo6::T10RAssetType::T10R_ASSET_KEYVALUEPAIRS, true };

}