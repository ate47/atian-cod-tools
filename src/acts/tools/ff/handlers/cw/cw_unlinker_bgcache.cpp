#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>

namespace {
	using namespace fastfile::handlers::cw;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct BGCacheInfoDef {
				BGCacheTypes type;
				CWXHash name;
				uint64_t asset;
			};
			static_assert(sizeof(BGCacheInfoDef) == 0x18);

			struct BGCache {
				CWXHash name;
				BGCacheInfoDef* def;
				uint32_t defCount;
			};
			static_assert(sizeof(BGCache) == 0x18);
			BGCache* asset{ (BGCache*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "bgcache" / std::format("{}.csv", n) };

			std::filesystem::create_directories(outFile.parent_path());

			std::filesystem::create_directories(outFile.parent_path());

			utils::OutFileCE os{ outFile };

			if (!os) {
				LOG_ERROR("Can't open {}", outFile.string());
				return;
			}

			LOG_INFO("Dump bgcache {}", outFile.string());

			os << "type,name";

			for (size_t i = 0; i < asset->defCount; i++) {
				BGCacheInfoDef& def{ asset->def[i] };
				os << "\n" << BgCacheName(def.type) << ",#" << hashutils::ExtractTmp("hash", def.name);
			}
		}
	};

	utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_BGCACHE };
}