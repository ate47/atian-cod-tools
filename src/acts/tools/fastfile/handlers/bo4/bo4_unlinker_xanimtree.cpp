#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ImplWorker : public Worker {
		using Worker::Worker;
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct XAnimTree {
				XHash name;
				XHash unk10;
				uint32_t len;
				byte* buffer;
			};
			static_assert(sizeof(XAnimTree) == 0x30);
			XAnimTree* asset{ (XAnimTree*)ptr };

			const char* n;
			if (asset->unk10) {
				n = utils::va("%s/%s.atrc", hashutils::ExtractTmp("hash", asset->unk10), hashutils::ExtractTmp("hash", asset->name));
			}
			else {
				n = utils::va("%s.atrc", hashutils::ExtractTmp("hash", asset->name));
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "graphics" / "xanimtree" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump xanimtree {} 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return;
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_XANIM_TREE, true };
}