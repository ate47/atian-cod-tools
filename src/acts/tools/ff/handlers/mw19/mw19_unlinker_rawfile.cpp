#include <includes.hpp>
#include <tools/ff/handlers/handler_game_mw19.hpp>
#include <utils/compress_utils.hpp>

namespace {
	using namespace fastfile::handlers::mw19;

	struct RawFile {
		XString name;
		uint32_t compressedLen;
		uint32_t uncompressedLen;
		byte* data;
	}; static_assert(sizeof(RawFile) == 0x18);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			RawFile* asset{ (RawFile*)ptr };

			const char* n{ ValidateName(asset->name) };

			std::filesystem::path outFile{ opt.m_output / gamePath / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump raw file {} len: 0x{:x}", outFile.string(), asset->uncompressedLen);
			if (!asset->uncompressedLen && std::filesystem::exists(outFile)) return; // ignore empty files
			std::unique_ptr<byte[]> decomp{ std::make_unique<byte[]>(asset->uncompressedLen + 1) };

			if (asset->uncompressedLen) {
				if (utils::compress::Decompress2(utils::compress::COMP_ZLIB, decomp.get(), asset->uncompressedLen, asset->data, asset->compressedLen) < 0) {
					LOG_ERROR("Error when decompresing {}", outFile.string());
					return;
				}
			}

			if (!utils::WriteFile(outFile, decomp.get(), asset->uncompressedLen)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, HandlerHashedAssetType, Worker> impl{ GetWorkers(), HandlerHashedAssetType::IW8H_ASSET_RAWFILE, sizeof(RawFile) };
}