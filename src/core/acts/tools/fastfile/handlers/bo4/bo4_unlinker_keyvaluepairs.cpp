#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class KVPWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct KeyValuePair {
				int64_t keyHash;
				const char* value;
			};

			struct KeyValuePairs {
				XHash name;
				int numVariables;
				KeyValuePair* keyValuePairs;
			};
			static_assert(sizeof(KeyValuePairs) == 0x20);
			KeyValuePairs* asset{ (KeyValuePairs*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "keyvaluepairs" / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name.name)) };

			std::filesystem::create_directories(outFile.parent_path());


			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());

			}

			LOG_OPT_INFO("Dump keyvaluepairs {}", outFile.string());

			os << "key,value";

			for (size_t i = 0; i < asset->numVariables; i++) {
				KeyValuePair& pair{ asset->keyValuePairs[i] };
				os << "\n0x" << std::hex << pair.keyHash << "," << GetValidString(pair.value);
			}

		}
	};

	utils::MapAdder<KVPWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_KEYVALUEPAIRS };
}