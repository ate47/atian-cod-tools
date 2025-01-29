#pragma once
#include <hook/module_mapper.hpp>
#include <core/bytebuffer.hpp>

namespace fastfile {
	template<typename T>
	inline bool IsSame(T t, int64_t val) {
		return reinterpret_cast<T>(val) == t;
	}

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

	enum FastFilePlatform : byte {
		XFILE_PC = 0x0,
		XFILE_XBOX = 0x1,
		XFILE_PLAYSTATION = 0x2,
		XFILE_COUNT = 0x3,
	};


	struct TXFileHeader {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
		fastfile::FastFilePlatform platform;
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

	class FFHandler;

	struct FastFileContext {
		const char* file;
		const char* ffname{};
	};

	class FastFileOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_header{};
		bool print_handlers{};
		const char* m_casc{};
		const char* game{};
		const char* exec{};
		HANDLE cascStorage{};
		std::filesystem::path m_output{ "output_ff" };
		std::vector<const char*> files{};
		FFHandler* handler{};
		hook::module_mapper::Module mod{};

		~FastFileOption();
		bool Compute(const char** args, size_t startIndex, size_t endIndex);
		void PrintHelp();
		std::vector<std::string> GetFileRecurse(const char* path);
		bool ReadFile(const char* path, std::vector<byte>& buff);


		inline bool ReadFile(const std::string& path, std::vector<byte>& buff) {
			return ReadFile(path.data(), buff);
		}
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

	class FFDecompressor {
	public:
		const char* name;
		uint64_t magic;
		uint64_t mask;

		FFDecompressor(const char* name, uint64_t magic, uint64_t mask) : name(name), magic(magic), mask(mask) {
		}

		virtual void LoadFastFile(FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, FastFileContext& ctx, std::vector<byte>& ffdata) = 0;
	};

	class FFHandler {
	public:
		const char* name;
		const char* description;

		FFHandler(const char* name, const char* description) : name(name), description(description) {
		}

		virtual void Init(FastFileOption& opt) {}

		virtual void Cleanup() {}

		virtual void Handle(FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, FastFileContext& ctx) = 0;
	};

	const char* GetFastFileCompressionName(FastFileCompression comp);
	const char* GetFastFilePlatformName(FastFilePlatform comp);
	std::vector<FFDecompressor*>& GetDecompressors();
	FFDecompressor* FindDecompressor(uint64_t magic);
	std::vector<FFHandler*>& GetHandlers();
	FFHandler* FindHandler(const char* name);
}