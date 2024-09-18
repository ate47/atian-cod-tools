#pragma once

namespace tool::gsc::acts_debug {
	constexpr uint64_t MAGIC = 0x0d0a42444124;
	constexpr byte CURRENT_VERSION = 0x15;

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


		constexpr bool HasFeature(DebugFeatures feature) {
			return version >= feature;
		}

		constexpr bool HasFlag(uint32_t flag) {
			return (flags & flag) == flag;
		}
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
		size_t lineNum;
	};

	struct GSC_ACTS_FILES {
		uint32_t filename;
		size_t lineStart;
		size_t lineEnd;
	};

}