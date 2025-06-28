#pragma once

namespace fastfile::packed {

	constexpr uint32_t MAGIC = 0x46464154;
	constexpr uint32_t MAGIC_A = 0x41464154;
	enum SectionType : uint32_t {
		ST_VERSION = 0x39CB44F1u,
		ST_PLATFORM_DATA = 0x2C2381CFu,
		ST_BUILD_DATA = 0x37F9D612u,
		ST_BLOCK_SIZES = 0x66C65056u,
		ST_LINK_RESULT = 0x8578C004u,
		ST_SIZE_DATA = 0xF992DFEu,
		ST_UNK_0x6AEBB196u = 0x6AEBB196u,
		ST_FASTFILE_INFO = 0xC7708CDAu,
		ST_UNK_0x1CE68F50u = 0x1CE68F50u,

		ST_IW_UNK_0xfd3d473d = 0xfd3d473d,
		ST_IW_UNK_0x671833c0 = 0x671833c0,
		ST_IW_UNK_0xb1eb41e6 = 0xb1eb41e6,
		ST_IW_UNK_0x2de89214 = 0x2de89214,
		ST_IW_DEPS = 0x803010a3,
	};

	struct PFFFastFileInfo {
		char fastfileName[0x40];
		uint8_t signature[0x100];
		uint8_t aesIV[0x10];
	};
	struct PFPlatformData {
		bool server;
		fastfile::FastFileCompression compression;
		fastfile::FastFilePlatform platform;
		bool encrypted;
	};
	struct PFFBuildData {
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builderName[32];
		uint32_t unk4c;
	};
	struct PFFLinkResult {
		uint32_t unk50;
		byte linkResultCode;
		char linkResult[256];
	};
	struct PFFSizeData {
		uint64_t size;
		uint64_t externalSize;
		uint64_t memMappedOffset;
	};
	struct PFFIWUnk {
		uint64_t unk0;
		uint64_t unk8;
		uint64_t unk10;
	};
	struct PFFIWDeps {
		uint64_t fastfileHash;
		uint64_t unk8;
	};

	class PackedFastFileReader {
	public:
		uint32_t version{};
		PFFFastFileInfo ff{};
		PFPlatformData platform{};
		PFFBuildData build{};
		PFFLinkResult link{};
		uint64_t blockSizes[0x20]{};
		uint64_t blocksCount{};
		PFFSizeData size{};

		PFFIWDeps iwDeps{};

		void ReadHeader(core::bytebuffer::AbstractByteBuffer& reader);
	};

	struct PackChunk {
		size_t offset;
		const void* buffer;
		size_t size;
	};

	class HeaderPacker {
	public:
		std::vector<byte>& data;
		std::vector<PackChunk> chunks{};
		HeaderPacker(std::vector<byte>& data);
		void AddBlock(SectionType type, const void* ptr, uint32_t len);
		void WriteEnd();
	};
}