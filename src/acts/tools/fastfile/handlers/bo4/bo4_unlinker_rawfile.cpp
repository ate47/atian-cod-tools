#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	class ScriptParseTreeWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct RawFile {
				XHash name;
				int32_t len;
				byte* buffer;
			}; static_assert(sizeof(RawFile) == 0x20);
			RawFile* asset{ (RawFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name.name) };

			if (!n) {
				n = utils::va("hashed/rawfile/file_%llx.raw", asset->name.name);
			}

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump raw file {} 0x{:x}", outFile.string(), asset->len);
			if (!asset->len && std::filesystem::exists(outFile)) return; // ignore empty files
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ScriptParseTreeWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_RAWFILE };
}