#include <includes.hpp>
#include <lz4.h>
#include "compatibility/scobalula_wni.hpp"

namespace compatibility::scobalula::wni {
	namespace {
		int WniRead(byte* buffer, size_t size, std::function<void(uint64_t hash, const char* str)>& each) {
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

			LOG_TRACE("entries: {}", entries);
			LOG_TRACE("compressedSize: {}", compressedSize);
			LOG_TRACE("decompressedSize: {}", decompressedSize);

			off += sizeof(*headerData) * 3;

			if (size < off + compressedSize) {
				LOG_ERROR("Bad wni compressed size {}/{}", compressedSize, size - off);
				return false;
			}

			auto decompBuffer = std::make_unique<byte[]>(decompressedSize);

			if (LZ4_decompress_safe((const char*)(buffer + off), (char*)&decompBuffer[0], compressedSize, decompressedSize) < 0) {
				LOG_ERROR("Error with LZ4 decompression, bad file?");
				return false;
			}

			size_t doff{};

			for (size_t i = 0; i < entries; i++) {
				if (doff + 9 > decompressedSize) {
					LOG_ERROR("Bad decompressed size, bad file?");
					return false;
				}

				uint64_t hash = *reinterpret_cast<uint64_t*>(&decompBuffer[doff]);
				doff += sizeof(hash);
				const char* str = reinterpret_cast<const char*>(&decompBuffer[doff]);
				doff += strlen(str) + 1;

				each(hash, str);
			}

			return true;
		}
	}

	bool ReadWNIFiles(std::filesystem::path path, std::function<void(uint64_t hash, const char* str)> each) {
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurseExt(path, paths, ".wni\0");

		std::vector<byte> buff{};
		for (const std::filesystem::path& p : paths) {

			LOG_DEBUG("Read {}", p.string());

			if (!utils::ReadFile(p, buff)) {
				LOG_ERROR("Can't read {}", path.string());
				return false;
			}

			if (!WniRead((byte*)buff.data(), buff.size(), each)) {
				return false;
			}
		}
		return true;
	}

}