#pragma once
#include <core/bytebuffer.hpp>

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
		fastfile::FastFileCompression compression;
		uint8_t platform;
		uint8_t encrypted;
	};

	class FastFileOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_header{};
		const char* m_casc{};
		const char* game{};
		HANDLE cascStorage{};
		const char* m_output{ "output_ff" };
		std::vector<const char*> files{};

		~FastFileOption();
		bool Compute(const char** args, size_t startIndex, size_t endIndex);
		void PrintHelp();
		std::vector<std::string> GetFileRecurse(const char* path);
		bool ReadFile(const char* path, std::vector<byte>& buff);
	};

	class FFAssetPool {
		std::vector<std::unordered_map<uint64_t, void*>> pool{};
		std::vector<std::unique_ptr<std::vector<byte>>> memory{};
	public:
		FFAssetPool() {};

		void* FindAssetHeader(size_t type, uint64_t name);
		void AddAssetHeader(size_t type, uint64_t name, void* header);
		void ClearRef();
		std::vector<byte>& CreateMemoryBuffer();
	};

	constexpr uint64_t MASK32 = 0xFFFFFFFF;
	constexpr uint64_t MASK64 = 0xFFFFFFFFFFFFFFFF;

	class FFHandler {
	public:
		const char* name;
		uint64_t magic;
		uint64_t mask;

		FFHandler(const char* name, uint64_t magic, uint64_t mask) : name(name), magic(magic), mask(mask) {
		}

		virtual void LoadFastFile(FFAssetPool& pool, FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, const char* file) = 0;
	};

	const char* GetFastFileCompressionName(FastFileCompression comp);
	std::vector<FFHandler*>& GetHandlers();
	FFHandler* FindHandler(uint64_t magic);
}