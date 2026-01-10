#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo3.hpp>

namespace {
	using namespace fastfile::handlers::bo3;
	class RawFileWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
			struct RawFile {
				const char* name;
				int32_t len;
				byte* buffer;
			}; static_assert(sizeof(RawFile) == 0x18);
			RawFile* asset{ (RawFile*)ptr };

			if (!FixupName(asset->name)) {
				return;
			}

			std::filesystem::path outFile{ opt.m_output / "bo3" / "source" / asset->name };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump raw file {} 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<RawFileWorker, bo3::pool::T7XAssetType, Worker> impl{ GetWorkers(), bo3::pool::T7_ASSET_TYPE_RAWFILE };
}