#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>

namespace {
	using namespace fastfile::handlers::bo7;

	struct KeyValuePair {
		const char* value;
		XHash32 key;
	};
	struct KeyValuePairs {
		XHash64 name;
		KeyValuePair* kv;
		int32_t count;
	};

	static_assert(sizeof(KeyValuePairs) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		std::unordered_map<uint64_t, const char*> knownNames;

		void PreLoadWorker(fastfile::FastFileContext* ctx) override {
			if (knownNames.empty()) {
				const char* KnownKVPNames[]{
					"xpak_read",
					"xpak_read_shared",
				};

				knownNames.reserve(ACTS_ARRAYSIZE(KnownKVPNames));

				for (const char* name : KnownKVPNames) {
					uint64_t h{ fastfile::handlers::bo7::DB_HashScrStringName(name, strlen(name), 0) };
					knownNames[(h & hash::MASK32)] = name;
				}
			}
		}

		const char* GetName(uint64_t hash) {
			auto it{ knownNames.find(hash) };
			if (it != knownNames.end()) {
				return it->second;
			}
			return utils::va("hash_%llx", hash);
		}


		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			KeyValuePairs* asset{ (KeyValuePairs*)ptr };


			std::filesystem::path outFile{ opt.m_output / gamePath / "source" / "tables" / "keyvaluepairs" / ctx.GetFFType() / std::format("{}.csv", hashutils::ExtractTmp("file", asset->name)) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump keyvaluepairs {}", outFile.string());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Error when dumping {}", outFile.string());
				return;
			}

			for (size_t i = 0; i < asset->count; i++) {
				KeyValuePair* kvp{ asset->kv + i };
				os << GetName(kvp->key) << "," << kvp->value << "\n";
			}
		}
	};

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_KEYVALUEPAIRS, sizeof(KeyValuePairs) };

}