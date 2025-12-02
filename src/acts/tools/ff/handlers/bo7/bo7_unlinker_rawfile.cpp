#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo7.hpp>
#include <utils/compress_utils.hpp>

namespace {
	const char* RawFileTypeName(uint32_t type) {
		static bool load{};
		if (!load) {
			const char* exts[]{
				".json",
				".cfg",
				".tif",
				".bin",
				".png",
				".ttf",
				".txt",
				".news",
				".graph",
			};
			load = true;

			for (const char* name : exts) {
				hashutils::AddPrecomputed(hash::HashX32(name), name, false, false);
			}

		}
		const char* ext{ hashutils::ExtractPtr(type) };
		if (ext) return ext;
		return ".raw";
	}

	using namespace fastfile::handlers::bo7;

	struct RawFile {
		uint64_t name;
		uint32_t type;
		uint32_t compressedLen;
		uint32_t uncompressedLen;
		byte* data;
	}; static_assert(sizeof(RawFile) == 0x20);

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx, void* ptr) override {
			RawFile* asset{ (RawFile*)ptr };

			const char* n{ hashutils::ExtractPtr(asset->name) };

			if (!n) {
				n = utils::va("hashed/rawfile/file_%llx%s", asset->name, RawFileTypeName(asset->type));
			}

			std::filesystem::path outFile{ opt.m_output / gamePath / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump raw file {} type: 0x{:x} len: 0x{:x}", outFile.string(), asset->type, asset->uncompressedLen);
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

	utils::MapAdder<ImplWorker, SatHashAssetType, Worker> impl{ GetWorkers(), SatHashAssetType::SATH_ASSET_RAWFILE, sizeof(RawFile) };
}