#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct KeyValuePair {
				int64_t keyHash;
				const char* value;
			};
			static_assert(sizeof(KeyValuePair) == 0x10);

			struct KeyValuePairs {
				CWXHash name;
				uint32_t count;
				KeyValuePair* kvp;
			};
			static_assert(sizeof(KeyValuePairs) == 0x18);
			KeyValuePairs* asset{ (KeyValuePairs*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "keyvaluepairs" / std::format("{}.csv", n) };

			std::filesystem::create_directories(outFile.parent_path());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't open {}", outFile.string());
				return;
			}

			LOG_INFO("Dump keyvaluepairs {}", outFile.string());

			os << "key,value";

			for (size_t i = 0; i < asset->count; i++) {
				KeyValuePair* kvp{ asset->kvp + i };
				os << "\n" << "0x" << std::hex << kvp->keyHash << "," << utils::PtrOrElse(kvp->value, "");
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_KEYVALUEPAIRS };
}