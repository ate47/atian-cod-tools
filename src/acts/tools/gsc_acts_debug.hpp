#pragma once

namespace tool::gsc::acts_debug {
	constexpr uint64_t MAGIC = 0x0d0a42444124;
	constexpr byte CURRENT_VERSION = 0x10;

	enum DebugFeatures : byte {
		ADF_STRING = 0x10,
	};

	struct GSC_ACTS_DEBUG {
		byte magic[sizeof(MAGIC)];
		byte version;
		uint64_t actsVersion;
		uint32_t strings_offset{};
		uint32_t strings_count{};
	};


}