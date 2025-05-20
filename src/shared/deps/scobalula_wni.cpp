#include <includes_shared.hpp>
#include <core/async.hpp>
#include <utils/compress_utils.hpp>
#include <deps/scobalula_wni.hpp>
#include <core/bytebuffer.hpp>
#include <core/bytebuffer_file.hpp>
#include <core/hashes/hash_store.hpp>

namespace deps::scobalula::wni {
	namespace {
		void WniRead(core::bytebuffer::FileReader& reader, std::function<void(uint64_t hash, const char* str)>& each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
			if (reader.Read<uint32_t>() != WNI_MAGIC) {
				throw std::runtime_error("Bad file magic");
			}

			if (reader.Read<uint16_t>() != WNI_VERSION) {
				throw std::runtime_error("Unknown WNI version");
			}

			uint32_t entries = reader.Read<uint32_t>();
			uint32_t compressedSize = reader.Read<uint32_t>();
			uint32_t decompressedSize = reader.Read<uint32_t>();

			LOG_TRACE("wni: entries: {}, compressedSize: {}, decompressedSize: {}", entries, compressedSize, decompressedSize);

			byte* buff;
			std::unique_ptr<byte[]> compBuffer = std::make_unique<byte[]>(std::max<size_t>(1, compressedSize));
			
			reader.Read(compBuffer.get(), compressedSize);


			std::unique_ptr<byte[]> decompBuffer;
			if (allocMemory) {
				buff = (byte*)allocMemory(decompressedSize);
			}
			else {
				decompBuffer = std::make_unique<byte[]>(decompressedSize);
				buff = decompBuffer.get();
			}

			if (utils::compress::Decompress2(utils::compress::COMP_LZ4, buff, decompressedSize, compBuffer.get(), compressedSize) < 0) {
				throw std::runtime_error("Error with LZ4 decompression, bad file?");
			}

			core::bytebuffer::ByteBuffer decr{ buff, decompressedSize };

			{
				core::async::opt_lock_guard lg{ loadMutex };

				for (size_t i = 0; i < entries; i++) {
					uint64_t hash{ decr.Read<uint64_t>() };
					const char* str{ decr.ReadString() };
					each(hash, str);
				}
			}
		}
	}
	bool ReadWNIFile(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<byte> buff{};
		LOG_DEBUG("Read {}", path.string());

		utils::InFileCE is{ path, false, std::ios::binary };
		if (!is) {
			LOG_ERROR("Can't open {}", path.string());
			return false;
		}
		core::bytebuffer::FileReader reader{ is };

		try {
			WniRead(reader, each, allocMemory, loadMutex);
			return true;
		} catch (std::runtime_error& err) {
			LOG_ERROR("Can't read {}: {}", path.string(), err.what());
			return false;
		}
	}

	bool ReadWNIFiles(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void*(*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurseExt(path, paths, ".wni\0");

		for (const std::filesystem::path& p : paths) {
			if (!ReadWNIFile(path, each, allocMemory, loadMutex)) return false;
		}
		return true;
	}
	
	void LoadHashFile(std::filesystem::path p) {
		ReadWNIFiles(p, [](uint64_t hash, const char* str) {
			core::hashes::AddPrecomputed(hash, str, false);
		}, core::hashes::AllocHashMemory);
	}
}