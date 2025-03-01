#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	class BgCacheWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct BGCacheInfoDef {
				games::bo4::pool::BGCacheTypes type;
				XHash name;
				uint64_t asset;
			};

			struct BGCacheInfo {
				XHash name;
				BGCacheInfoDef* def;
				int defCount;
			};


			BGCacheInfo* asset{ (BGCacheInfo*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "bgcache" / std::format("{}.csv", n) };

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
				os << "\n" << games::bo4::pool::BGCacheNameFromId(def.type) << ",#" << hashutils::ExtractTmp("hash", def.name.name);
			}

		}
	};

	utils::MapAdder<BgCacheWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_BG_CACHE };
}