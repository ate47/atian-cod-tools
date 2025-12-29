#pragma once
#include <core/bytebuffer.hpp>

namespace fastfile::flexible {
	constexpr uint32_t MAGIC = 0x46464154;
	constexpr uint32_t MAGIC_V2 = 0x41464154;

	constexpr bool IsFlexibleDataMagic(uint32_t magic) {
		return magic == MAGIC || magic == MAGIC_V2;
	}
	struct TAFASecureInfo {
		const char* rsaKeyName;
		const char* fastfileName;
	};

	struct TAFASignature {
		uint32_t magic;
		byte hash[0x20];
		byte signature[0x100];
	};
	static_assert(sizeof(TAFASignature) == 0x124);

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

		ST_IW_DEPS = 0x803010a3,
		ST_IW_FASTFILE_CHECKSUM = 0xfd3d473d,
		ST_IW_BOOT_INFO = 0x92c016b3,
		ST_IW_UNK_0x2de89214 = 0x2de89214,
		ST_IW_UNK_0x4EEB924F = 0x4EEB924F,
		ST_IW_UNK_0x631095b6 = 0x631095b6,
		ST_IW_UNK_0x671833c0 = 0x671833c0,
		ST_IW_UNK_0x6a1a96e4 = 0x6a1a96e4,
		ST_IW_UNK_0x7337DACF = 0x7337DACF,
		ST_IW_UNK_0x92382A93 = 0x92382A93,
		ST_IW_UNK_0xb1eb41e6 = 0xb1eb41e6,
		ST_IW_UNK_0xc95c6b9c = 0xc95c6b9c,
		ST_IW_UNK_0xd4cb9f1b = 0xd4cb9f1b,
	};

	struct FlexibleFastFileChunk {
		SectionType type;
		size_t size;
		void* buffer;

		template<typename T>
		T* GetPtr() {
			if (size < sizeof(T)) {
				throw std::runtime_error(std::format("Section too small {:x} size:0x{:x} < wanted:0x{:x}", (uint32_t)type, size, sizeof(T)));
			}
			return (T*)buffer;
		}
		template<typename T>
		T& GetVal() {
			return *GetPtr<T>();
		}
	};

	class FlexibleFastFileReader {
		FlexibleFastFileChunk chunks[0x10];
		size_t chunksCount{};
	public:

		void ReadHeader(core::bytebuffer::ByteBuffer& reader, TAFASecureInfo* secureInf = nullptr);

		FlexibleFastFileChunk* GetChunk(SectionType type, bool failMissing);

		void Clear();

		constexpr FlexibleFastFileChunk* begin() {
			return chunks;
		}
		constexpr FlexibleFastFileChunk* end() {
			return &chunks[chunksCount];
		}

		template<typename T>
		T* GetChunkVal(SectionType type, bool failMissing) {
			FlexibleFastFileChunk* ch{ GetChunk(type, failMissing) };
			if (!ch) {
				return nullptr;
			}
			return ch->GetPtr<T>();
		}

		uint64_t* GetTrXBlocks() { return GetChunkVal<uint64_t>(SectionType::ST_BLOCK_SIZES, true); }
		size_t GetTrXBlocksCount() { return GetChunk(SectionType::ST_BLOCK_SIZES, true)->size / sizeof(uint64_t); }
		uint32_t GetTrVersion() { return *GetChunkVal<uint32_t>(SectionType::ST_VERSION, true); }
	};

	struct FlexibleWriteChunk {
		SectionType type;
		size_t offset;
		const void* buffer;
		size_t size;
	};

	class FlexibleFastFileWriter {
	public:
		uint32_t magic;
		std::vector<byte>& data;
		FlexibleWriteChunk chunks[0x10]{};
		size_t chunksCount{};
		size_t hashOffset{};
		FlexibleFastFileWriter(std::vector<byte>& data, uint32_t magic);
		void AddBlock(SectionType type, const void* ptr, uint32_t len);
		void WriteEnd(TAFASecureInfo* secureInfo = nullptr);
	};
}