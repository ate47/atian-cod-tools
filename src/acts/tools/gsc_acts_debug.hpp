#pragma once

namespace tool::gsc::acts_debug {
	constexpr uint64_t MAGIC = 0x0d0a42444124;
	constexpr byte CURRENT_VERSION = 0x12;

	enum DebugFeatures : byte {
		ADF_STRING = 0x10,
		ADF_DETOUR = 0x11,
		ADF_DEVBLOCK_BEGIN = 0x12,
		ADF_LAZYLINK = 0x12,
	};

	struct GSC_ACTS_DEBUG {
		byte magic[sizeof(MAGIC)];
		byte version;
		uint64_t actsVersion;
		uint32_t strings_offset{};
		uint32_t strings_count{};
		uint32_t detour_offset{};
		uint32_t detour_count{};
		uint32_t devblock_offset{};
		uint32_t devblock_count{};
		uint32_t lazylink_offset{};
		uint32_t lazylink_count{};
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

}