#pragma once
#include "../actslib.hpp"

namespace actslib::data::deltafile {
	constexpr const char DF_MAGIC[] = "$DltF0\n\r";

	enum WikidataChangesFlavor : uint8_t {
		FLAVOR_DUMP = 0x63,
		FLAVOR_SIMPLE = 0x64,
		FLAVOR_FULL = 0x65
	};

	struct DeltaFileHeader {
		char magic[8];
		uint64_t count{};
		uint64_t start{};
		uint64_t end{};
		WikidataChangesFlavor flavor{ FLAVOR_FULL };
		uint8_t __pad[3]{};
	};

	constexpr size_t DF_HEADER_SIZE = 0x24;
}