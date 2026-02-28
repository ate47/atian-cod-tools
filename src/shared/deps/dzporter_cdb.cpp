#include <includes_shared.hpp>
#include <core/async.hpp>
#include <utils/compress_utils.hpp>
#include <deps/dzporter_cdb.hpp>
#include <core/bytebuffer.hpp>
#include <core/bytebuffer_file.hpp>
#include <core/hashes/hash_store.hpp>

namespace deps::dzporter::cdb {
	namespace {
		void CDBRead(core::bytebuffer::FileReader& reader, std::function<void(uint64_t hash, const char* str)>& each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {

			NameDatabaseHeader header;
			reader.Read(&header, sizeof(header));

			if (header.magic != CDB_MAGIC) {
				throw std::runtime_error(std::format("invalid magic: {:x}", header.magic));
			}

			LOG_TRACE("cdb: entries: {}, compressedSize: {}, decompressedSize: {}", header.entries, header.compressedSize, header.decompressedSize);

			if (!header.entries) {
				return; // nothing to read
			}

			std::unique_ptr<byte[]> decompressed{ std::make_unique<byte[]>(header.decompressedSize) };
			std::unique_ptr<byte[]> compressed{ reader.ReadArray<byte>(header.compressedSize) };

			int r{ utils::compress::Decompress2(
				utils::compress::COMP_LZ4,
				decompressed.get(), header.decompressedSize,
				compressed.get(), header.compressedSize)};

			if (r < 0) {
				throw std::runtime_error(std::format("can't decompress {}", utils::compress::DecompressResultName(r)));
			}

			core::bytebuffer::ByteBuffer dreader{ decompressed.get(), header.decompressedSize };

			std::unique_ptr<const char* []> vals{ std::make_unique<const char* []>(header.entries) };

			for (size_t i = 0; i < header.entries; i++) {
				vals[i] = dreader.ReadString();
			}

			std::unique_ptr<uint64_t[]> keys{ dreader.ReadArray<uint64_t>(header.entries) };

			{
				core::async::opt_lock_guard lg{ loadMutex };

				for (size_t i = 0; i < header.entries; i++) {
					each(keys[i], vals[i]);
				}
			}
		}
	}
	bool ReadCDBFile(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<byte> buff{};
		LOG_DEBUG("Read {}", path.string());

		utils::InFileCE is{ path, false, std::ios::binary };
		if (!is) {
			LOG_ERROR("Can't open {}", path.string());
			return false;
		}
		core::bytebuffer::FileReader reader{ is };

		try {
			CDBRead(reader, each, allocMemory, loadMutex);
			return true;
		}
		catch (std::runtime_error& err) {
			LOG_ERROR("Can't read {}: {}", path.string(), err.what());
			return false;
		}
	}

	bool ReadCDBFiles(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurseExt(path, paths, ".cdb\0");

		for (const std::filesystem::path& p : paths) {
			if (!ReadCDBFile(p, each, allocMemory, loadMutex)) return false;
		}
		return true;
	}

	void LoadHashFile(std::filesystem::path p) {
		ReadCDBFiles(p, [](uint64_t hash, const char* str) {
			core::hashes::AddPrecomputed(hash, str, false);
			}, core::hashes::AllocHashMemory);
	}
}