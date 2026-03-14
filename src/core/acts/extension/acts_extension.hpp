#pragma once

namespace acts::extension {
	constexpr uint32_t ACEF_MAGIC = 0x46454341;

	enum AcefVersion : uint16_t {
		ACEF_VERSION_1 = 0x31,

		
		ACEF_VERSION_MIN = ACEF_VERSION_1,
		ACEF_VERSION_LATEST = ACEF_VERSION_1,
	};

	enum AcefFlags : uint32_t {
		ACEF_BIGENDIAN = 1 << 0,
	};

	struct AcefHeader {
		uint32_t magic;
		uint16_t version;
		uint16_t flags;
		uint32_t headerSize;
		uint32_t sizeObjectHeader;
		uint32_t numObjects;
		uint32_t compressedSize;
		uint32_t decompressedSize;
		uint32_t windowSize;
		uint64_t compress;
	};
	static_assert(!(sizeof(AcefHeader) & 0x7));
	struct AcefBlockHeader {
		uint32_t blockSize;
		uint32_t decompSize;
		uint32_t checkLoc;
		uint32_t dataOffset;
		uint64_t blockId;
	};
	static_assert(!(sizeof(AcefBlockHeader) & 0x7));
	
	struct AcefArray {
		size_t len;
		void* data;
	};

	void LoadExtensionFiles();

	void GetExtensionData(uint64_t path, AcefArray** data, size_t* count);

	inline void GetExtensionData(const char* path, AcefArray** data, size_t* count) {
		GetExtensionData(hash::Hash64(path), data, count);
	}
}