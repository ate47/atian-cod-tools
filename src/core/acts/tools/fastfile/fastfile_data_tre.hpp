#pragma once
#include <tools/fastfile/fastfile_handlers.hpp>

namespace fastfile {


	struct DBStreamHeader {
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint32_t alignedSize;
		uint32_t offset;
	}; static_assert(sizeof(DBStreamHeader) == 0x10);

	enum FastFileCompression : byte {
		XFILE_UNCOMPRESSED = 0x0,
		XFILE_ZLIB = 0x1,
		XFILE_ZLIB_HC = 0x2,
		XFILE_LZ4 = 0x3,
		XFILE_LZ4_HC = 0x4,
		XFILE_BDELTA_UNCOMP = 0x5,
		XFILE_BDELTA_ZLIB = 0x6,
		XFILE_BDELTA_LZMA = 0x7,
		XFILE_OODLE_KRAKEN = 0x8,
		XFILE_OODLE_MERMAID = 0x9,
		XFILE_OODLE_SELKIE = 0xA,
		XFILE_OODLE_LZNA = 0xB,
		XFILE_COMPRESSION_COUNT = 0xC,
	};

	struct TXFileHeader {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		FastFileCompression compression;
		FastFilePlatform platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
	};
	struct TX32FileHeader {
		uint8_t magic[4];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
		uint8_t platform;
		uint8_t encrypted;
	};

	const char* GetFastFileCompressionName(FastFileCompression comp);
	FastFileCompression GetFastFileCompression(const char* name);
	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileCompression comp);

	namespace flexible {
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

		inline PFFFastFileInfo& GetTrFastFileInfo(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFFastFileInfo>(SectionType::ST_FASTFILE_INFO, true); }
		inline PFPlatformData& GetTrPlatformData(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFPlatformData>(SectionType::ST_PLATFORM_DATA, true); }
		inline PFFBuildData& GetTrBuildData(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFBuildData>(SectionType::ST_BUILD_DATA, true); }
		inline PFFLinkResult& GetTrLinkResult(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFLinkResult>(SectionType::ST_LINK_RESULT, true); }
		inline PFFSizeData& GetTrSizeData(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFSizeData>(SectionType::ST_SIZE_DATA, true); }
	}

}