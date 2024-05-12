#pragma once

namespace tool::gsc::acts_debug {
	constexpr uint64_t MAGIC = 0x0d0a42444124;
	constexpr byte CURRENT_VERSION = 0x11;

	enum DebugFeatures : byte {
		ADF_STRING = 0x10,
		ADF_DETOUR = 0x11,
	};

	struct GSC_ACTS_DEBUG {
		byte magic[sizeof(MAGIC)];
		byte version;
		uint64_t actsVersion;
		uint32_t strings_offset{};
		uint32_t strings_count{};
		uint32_t detour_offset{};
		uint32_t detour_count{};
	};

	struct GSC_ACTS_DETOUR {
		uint64_t name_space;
		uint64_t name;
		uint64_t script;
		uint32_t location;
		uint32_t size;
	};

}