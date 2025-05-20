#pragma once

namespace shared::gsc::acts_debug {
	constexpr uint64_t MAGIC = 0x0d0a42444124;
	constexpr byte CURRENT_VERSION = 0x16;

	enum DebugFeatures : byte {
		ADF_STRING = 0x10,
		ADF_DETOUR = 0x11,
		ADF_DEVBLOCK_BEGIN = 0x12,
		ADF_LAZYLINK = 0x12,
		ADF_CRC_LOC = 0x13,
		ADF_DEVSTRING = 0x14,
		ADF_LINES = 0x15,
		ADF_FILES = 0x15,
		ADF_FLAGS = 0x15,
		ADF_CHECKSUM = 0x16,
	};

	enum ActsDebugFlags : uint32_t {
		ADFG_OBFUSCATED = 1 << 0,
		ADFG_DEBUG = 1 << 1,
		ADFG_CLIENT = 1 << 2,
		ADFG_PLATFORM_SHIFT = 3,
		ADFG_PLATFORM_MASK = 0xF << ADFG_PLATFORM_SHIFT,
		ADFG_PLATFORM_UNK7 = 1 << 7,
		ADFG_PLATFORM_UNK8 = 1 << 8,
		ADFG_PLATFORM_UNK9 = 1 << 9,
	};

	struct GSC_ACTS_DETOUR {
		uint64_t name_space;
		uint64_t name;
		uint64_t script;
		uint32_t location;
		uint32_t size;
	};

	struct GSC_ACTS_LAZYLINK {
		uint64_t name_space;
		uint64_t name;
		uint64_t script;
		uint32_t num_address;
	};

	struct GSC_ACTS_DEVSTRING {
		uint32_t string;
		uint32_t num_address;
	};

	struct GSC_ACTS_LINES {
		uint32_t start;
		uint32_t end;
		uint64_t lineNum;
	};

	struct GSC_ACTS_FILES {
		uint32_t filename;
		uint64_t lineStart;
		uint64_t lineEnd;
	};

	struct GSC_ACTS_DEBUG {
		byte magic[sizeof(MAGIC)];
		byte version;
		uint32_t flags;
		uint64_t actsVersion;
		uint32_t strings_offset{};
		uint32_t strings_count{};
		uint32_t detour_offset{};
		uint32_t detour_count{};
		uint32_t devblock_offset{};
		uint32_t devblock_count{};
		uint32_t lazylink_offset{};
		uint32_t lazylink_count{};
		uint32_t crc_offset{};
		uint32_t devstrings_offset{};
		uint32_t devstrings_count{};
		uint32_t lines_offset{};
		uint32_t lines_count{};
		uint32_t files_offset{};
		uint32_t files_count{};
		int32_t checksum{};


		constexpr bool HasFeature(DebugFeatures feature) {
			return version >= feature;
		}

		constexpr bool HasFlag(uint32_t flag) {
			return (flags & flag) == flag;
		}

		const char* GetString(uint32_t address) const {
			return (const char*)(magic + address);
		}

		uint64_t GetMagic() const {
			return *(uint64_t*)magic;
		}

#define __ACTS_DEBUG_FUNC1(type, id, obj) \
		type* id() const { return (type*)(magic + obj); }

#define __ACTS_DEBUG_FUNC(type, id, obj, count) \
		__ACTS_DEBUG_FUNC1(type, id, obj) \
		type* id##End() const { return id() + count; }
		
		__ACTS_DEBUG_FUNC(GSC_ACTS_LINES, GetLines, lines_offset, lines_count)
		__ACTS_DEBUG_FUNC(GSC_ACTS_FILES, GetFiles, files_offset, files_count)
		__ACTS_DEBUG_FUNC(GSC_ACTS_DETOUR, GetDetours, detour_offset, detour_count)
		__ACTS_DEBUG_FUNC1(GSC_ACTS_LAZYLINK, GetLazyLinks, lazylink_offset)
		//__ACTS_DEBUG_FUNC1(GSC_ACTS_DEVSTRING, GetDevStrings, devstrings_offset) //unused
		__ACTS_DEBUG_FUNC(uint32_t, GetDevBlocks, devblock_offset, devblock_count)
		__ACTS_DEBUG_FUNC(uint32_t, GetStrings, strings_offset, strings_count)
			
#undef __ACTS_DEBUG_FUNC
#undef __ACTS_DEBUG_FUNC1

	};

}