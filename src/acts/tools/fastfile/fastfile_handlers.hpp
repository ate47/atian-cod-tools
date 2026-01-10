#pragma once
#include <hook/module_mapper.hpp>
#include <core/bytebuffer.hpp>
#include <core/memory_allocator.hpp>
#include <tools/fastfile/fastfile_zone.hpp>
#include <tools/gsc/gsc_opcodes_load.hpp>
#include <utils/compress_utils.hpp>
#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>
#include <tools/fastfile/fastfile_flexible.hpp>

namespace fastfile {
	template<typename T>
	inline bool IsSame(T t, int64_t val) {
		return reinterpret_cast<T>(val) == t;
	}

	// data after
	constexpr uintptr_t ALLOC_PTR = static_cast<uintptr_t>(-1);
	// data after, expect to allocate a virtual alias
	constexpr uintptr_t ALLOC_REF_PTR = static_cast<uintptr_t>(-2);

	enum FastFilePlatform : byte {
		XFILE_PC = 0x0,
		XFILE_XBOX = 0x1,
		XFILE_PLAYSTATION = 0x2,
		XFILE_DEV = 0x3,
		XFILE_PLATFORM_COUNT = 0x4,
	};

	struct XBlockCompressionBlockHeader {
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint32_t encryptionCTR;
	}; static_assert(sizeof(XBlockCompressionBlockHeader) == 0xc);


	enum FastFileIWCompression : byte {
		IWFFC_INVALID = 0,
		IWFFC_NONE = 1,
		IWFFC_ZLIB_SIZE = 0x2,
		IWFFC_ZLIB_SPEED = 0x3,
		IWFFC_LZ4_HC = 0x4,
		IWFFC_LZ4 = 0x5,
		IWFFC_OODLE_KRAKEN_SIZE = 0x6,
		IWFFC_OODLE_KRAKEN_SPEED = 0x7,
		IWFFC_HW_ZLIB_SIZE = 0x8,
		IWFFC_HW_ZLIB_SPEED = 0x9,
		IWFFC_PS4_ZLIB_SIZE = 0xA,
		IWFFC_PS4_ZLIB_SPREED = 0xB,
		IWFFC_OODLE_LEVIATHAN_SIZE = 0xC,
		IWFFC_OODLE_LEVIATHAN_SPEED = 0xD,
		IWFFC_OODLE_MERMAID_SIZE = 0xE,
		IWFFC_OODLE_MERMAID_SPEED = 0xF,
		IWFFC_OODLE_SELKIE_SIZE = 0x10,
		IWFFC_OODLE_SELKIE_SPEED = 0x11,
		IWFFC_ZSTD_SIZE = 0x12,
		IWFFC_ZSTD_SPEED = 0x13,
		IWFFC_COUNT,

		// mwiii
		// DEV ERROR 141 -> Unsupported block compression type (hw zlib) (8,9,10,11)
		// DEV ERROR 5816 -> Unknown block compression type
		// bo6
		// DEV ERROR 664 -> Unknown block compression type
	};

	class FFHandler;
	class FFCompressor;
	class FFLinker;

	struct XBlockInfo {
		byte* data{};
		size_t size{};
	};
	
	struct FastFileContext {
		const char* file;
		char ffname[0x100]{};
		char fftype[0x100]{};
		fastfile::flexible::FlexibleFastFileReader flexibleHeaderData{};
		// 0x10 to have more without recompiling
		XBlockInfo blockSizes[0x10]{};
		size_t blocksCount{};
		bool hasGSCBin{};
		tool::gsc::opcode::Platform gscPlatform{ tool::gsc::opcode::PLATFORM_PC };

		const char* GetFFType();
	};

	enum GameId : uint16_t {
		GID_INVALID = 0,
		GID_BO3,
		GID_IW,
		GID_WWII,
		GID_BO4,
		GID_MW19,
		GID_CW,
		GID_VG,
		GID_MWII,
		GID_MWIII,
		GID_BO6,
		GID_BO7,
	};

	enum GameRevId : uint16_t {
		GRID_DEFAULT = 0,
		GRID_BO6_PRE_ALPHA,
	};

	class FastFileOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_fdIgnoreMissing{};
		bool alpha{};
		bool m_header{};
		bool print_handlers{};
		bool print_decompressors{};
		bool print_gameId{};
		bool print_revId{};
		bool dump_decompressed{};
		bool noAssetDump{};
		bool assertContainer{};
		bool dumpBinaryAssets{};
		bool dumpBinaryAssetsMap{};
		bool dumpAssetNames{};
		bool dumpXStrings{};
		bool dumpCompiledZone{};
		bool testDump{};
		bool archiveDDL{};
		bool graphic{};
		GameId m_gameId{};
		GameRevId m_gameRevId{};
		const char* m_casc{};
		const char* game{};
		const char* exec{};
		const char* wildcard{};
		const char* ignoreWildcard{};
		const char* ignore{};
		const char* assets{};
		const char* translation{};
		const char* headerDump{};
		const char* rsaKey{};
		bool disableScriptsDecomp{};
		HANDLE cascStorage{};
		std::filesystem::path m_output{ "output_ff" };
		std::vector<const char*> files{};
		const char* assetTypes{};
		FFHandler* handler{};
		hook::module_mapper::Module gameMod{};
		std::vector<std::set<uint64_t>> assetNames{};
		std::unordered_map<uint64_t, const char*> translationKeys{};
		core::memory_allocator::MemoryAllocator alloc{};

		~FastFileOption();
		bool Compute(const char** args, size_t startIndex, size_t endIndex);
		void PrintHelp();

		hook::library::Library GetGame(bool crashError, bool* init = nullptr, bool needDecrypt = false, const char* defaultName = nullptr, const char* dumperName = nullptr);
		std::vector<std::string> GetFileRecurse(const char* path);
		bool ReadFile(const char* path, std::vector<byte>& buff);
		const char* GetTranslation(uint64_t key);
		const char* GetTranslation(const char* key);
		void LoadTranslationKeys();


		inline bool ReadFile(const std::string& path, std::vector<byte>& buff) {
			return ReadFile(path.data(), buff);
		}

		void AddAssetName(int type, uint64_t name);
	};

	class FastFileLinkerOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_header{};
		bool printCompressors{};
		bool printLinkers{};
		bool encrypt{};
		size_t chunkSize{};
		std::filesystem::path m_output{ "output_ff" };
		const char* ffname{};
		std::vector<const char*> files{};
		FFCompressor* compressor{};
		FFLinker* linker{};
		bool server{};
		FastFilePlatform platform{ FastFilePlatform::XFILE_PC };

		~FastFileLinkerOption();
		bool Compute(const char** args, size_t startIndex, size_t endIndex);
		void PrintHelp();
	};

	struct FastFile {
		std::vector<byte> linkedData{};
		const char* ffname{};
		size_t blockSizes[0x10]{};
	};

	class FastFileLinkerContext {
	public:
		std::filesystem::path zoneFile;
		std::filesystem::path input;
		zone::Zone zone{};
		std::string inputFileNameStr;
		const char* mainFFName{};
		std::vector<FastFile> fastfiles{};
		FastFileLinkerOption& opt;
		std::unordered_map<uint64_t, const char*> storedHashes{};
		core::memory_allocator::MemoryAllocator strs{};
		FFCompressor* compressor{};
		FFLinker* linker{};

		FastFileLinkerContext(FastFileLinkerOption& opt, std::filesystem::path zoneFile)
			: opt(opt), zoneFile(zoneFile), input(std::filesystem::absolute(zoneFile).parent_path()),
			  compressor(opt.compressor), linker(opt.linker) {
		}

		void ReadZoneFile();

		void RegisterHash(uint64_t val, const char* str);

		inline void RegisterHash(uint64_t val, const std::string& str) {
			RegisterHash(val, str.data());
		}
	};

	constexpr uint64_t MASK32 = 0xFFFFFFFF;
	constexpr uint64_t MASK64 = 0xFFFFFFFFFFFFFFFF;

	class FFDecompressor {
	public:
		const char* name;
		uint64_t magic;
		uint64_t mask;

		FFDecompressor(const char* name, uint64_t magic = 0, uint64_t mask = MASK64) : name(name), magic(magic& mask), mask(mask) {
		}
		
		virtual bool MatchFile(const std::filesystem::path& path, core::bytebuffer::ByteBuffer& data) const;

		virtual void Init(FastFileOption& opt) {}

		virtual void Cleanup() {}

		virtual void LoadFastFile(FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, FastFileContext& ctx, std::vector<byte>& ffdata) = 0;
	};

	class FFLinker {
	public:
		const char* name;
		const char* description;

		FFLinker(const char* name, const char* description) : name(name), description(description) {
		}

		virtual void Init(FastFileLinkerOption& opt) {}

		virtual void Cleanup() {}

		virtual void Link(FastFileLinkerContext& ctx) = 0;
	};

	class FFCompressor {
	public:
		const char* name;
		const char* description;

		FFCompressor(const char* name, const char* description) : name(name), description(description) {
		}

		virtual void Init(FastFileLinkerOption& opt) {}

		virtual void Cleanup() {}

		virtual void Compress(FastFileLinkerContext& ctx) = 0;
	};

	class FFHandler {
	public:
		const char* name;
		const char* description;
		bool noPatchOk;
		size_t forceNumXBlocks{};

		FFHandler(const char* name, const char* description, bool noPatchOk = false) : name(name), description(description), noPatchOk(noPatchOk) {
		}

		virtual void Init(FastFileOption& opt) {}

		virtual void Cleanup() {}

		virtual void Handle(FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, FastFileContext& ctx) = 0;
	};
	const char* GetFastFilePlatformName(FastFilePlatform comp);
	const char* GetGameIdName(GameId id);
	const char* GetGameRevIdName(GameRevId rev);

	FastFilePlatform GetFastFilePlatform(const char* name);
	GameRevId GetGameRevId(const char* name);

	std::vector<FFDecompressor*>& GetDecompressors();
	std::vector<FFHandler*>& GetHandlers();
	std::vector<FFCompressor*>& GetCompressors();
	std::vector<FFLinker*>& GetLinkers();

	FFDecompressor* FindDecompressor(const std::filesystem::path& path, core::bytebuffer::ByteBuffer& data);
	FFHandler* FindHandler(const char* name);
	FFCompressor* FindCompressor(const char* name);
	FFLinker* FindLinker(const char* name);
	fastfile::FastFileContext& GetCurrentContext();
	fastfile::FastFileOption& GetCurrentOptions();
}