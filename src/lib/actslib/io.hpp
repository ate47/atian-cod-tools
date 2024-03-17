#pragma once
#include "actslib.hpp"

namespace actslib::io {
	uint32_t ReadUInt32(std::istream& is);
	int32_t ReadInt32(std::istream& is);
	uint64_t ReadUInt64(std::istream& is);
	int64_t ReadInt64(std::istream& is);

	void WriteUInt32(std::ostream& os, uint32_t value);
	void WriteInt32(std::ostream& os, int32_t value);
	void WriteUInt64(std::ostream& os, uint64_t value);
	void WriteInt64(std::ostream& os, int64_t value);

}