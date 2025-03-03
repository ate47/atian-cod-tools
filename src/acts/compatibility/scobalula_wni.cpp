#include <includes.hpp>
#include <core/async.hpp>
#include <lz4.h>
#include "compatibility/scobalula_wni.hpp"

namespace compatibility::scobalula::wni {
	namespace {
		int WniRead(byte* buffer, size_t size, std::function<void(uint64_t hash, const char* str)>& each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
			if (size < WNI_HEADER_SIZE) {
				LOG_ERROR("Bad file size 0x{:x}", size);
				return false;
			}
			size_t off{};
			if (*reinterpret_cast<decltype(WNI_MAGIC)*>(buffer) != WNI_MAGIC) {
				LOG_ERROR("Bad file magic");
				return false;
			}
			off += sizeof(WNI_MAGIC);
			if (*reinterpret_cast<decltype(WNI_VERSION)*>(buffer + off) != WNI_VERSION) {
				LOG_ERROR("Unknown WNI version");
				return false;
			}
			off += sizeof(WNI_VERSION);

			uint32_t* headerData = reinterpret_cast<uint32_t*>(buffer + off);

			uint32_t entries = headerData[0];
			uint32_t compressedSize = headerData[1];
			uint32_t decompressedSize = headerData[2];

			LOG_TRACE("wni: entries: {}, compressedSize: {}, decompressedSize: {}", entries, compressedSize, decompressedSize);

			off += sizeof(*headerData) * 3;

			if (size < off + compressedSize) {
				LOG_ERROR("Bad wni compressed size {}/{}", compressedSize, size - off);
				return false;
			}

			byte* buff;
			std::unique_ptr<byte[]> decompBuffer;
			
			if (allocMemory) {
				buff = (byte*)allocMemory(decompressedSize);
			}
			else {
				decompBuffer = std::make_unique<byte[]>(decompressedSize);
				buff = decompBuffer.get();
			}

			if (LZ4_decompress_safe((const char*)(buffer + off), (char*)buff, compressedSize, decompressedSize) < 0) {
				LOG_ERROR("Error with LZ4 decompression, bad file?");
				return false;
			}

			size_t doff{};
			{
				core::async::opt_lock_guard lg{ loadMutex };

				for (size_t i = 0; i < entries; i++) {
					if (doff + 9 > decompressedSize) {
						LOG_ERROR("Bad decompressed size, bad file?");
						return false;
					}

					uint64_t hash = *reinterpret_cast<uint64_t*>(&buff[doff]);
					doff += sizeof(hash);
					const char* str = reinterpret_cast<const char*>(&buff[doff]);
					doff += strlen(str) + 1;

					each(hash, str);
				}
			}

			return true;
		}
	}
	bool ReadWNIFile(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<byte> buff{};
		LOG_DEBUG("Read {}", path.string());

		if (!utils::ReadFile(path, buff)) {
			LOG_ERROR("Can't read {}", path.string());
			return false;
		}

		if (!WniRead((byte*)buff.data(), buff.size(), each, allocMemory, loadMutex)) {
			return false;
		}
		return true;
	}

	bool ReadWNIFiles(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each, void*(*allocMemory)(size_t len), std::mutex* loadMutex) {
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurseExt(path, paths, ".wni\0");

		for (const std::filesystem::path& p : paths) {
			if (!ReadWNIFile(path, each, allocMemory, loadMutex)) return false;
		}
		return true;
	}

}