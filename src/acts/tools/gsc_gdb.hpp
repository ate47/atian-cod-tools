#pragma once

namespace tool::gsc::gdb {
	constexpr uint64_t MAGIC = 0x21000A0D42444780;

	struct GSC_GDB {
		byte magic[8];
		uint32_t version;
		uint32_t source_crc;
		uint32_t lineinfo_offset;
		uint32_t lineinfo_count;
		uint32_t devblock_stringtable_offset;
		uint32_t devblock_stringtable_count;
		uint32_t stringtable_offset;
		uint32_t stringtable_count;
	};

	struct GSC_LINEINFO {
		uint32_t offset;
		uint32_t nulled4;
	};
}