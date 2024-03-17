#include "actslib.hpp"
#include "io.h"

namespace actslib::io {
	uint32_t ReadUInt32(std::istream& is) {
		char buffer[4];

		is.read(buffer, sizeof(buffer));

		return
			(buffer[0] & 0xFFUL)
			| ((buffer[1] & 0xFFUL) << 8)
			| ((buffer[2] & 0xFFUL) << 16)
			| ((buffer[3] & 0xFFUL) << 24);
	}

	int32_t ReadInt32(std::istream& is) {
		uint32_t u = ReadUInt32(is);
		return *reinterpret_cast<int32_t*>(&u);
	}

	uint64_t ReadUInt64(std::istream& is) {
		char buffer[8]{};

		is.read(buffer, sizeof(buffer));

		return
			(buffer[0] & 0xFFULL)
			| ((buffer[1] & 0xFFULL) << 8) 
			| ((buffer[2] & 0xFFULL) << 16)
			| ((buffer[3] & 0xFFULL) << 24) 
			| ((buffer[4] & 0xFFULL) << 32) 
			| ((buffer[5] & 0xFFULL) << 40)
			| ((buffer[6] & 0xFFULL) << 48) 
			| ((buffer[7] & 0xFFULL) << 56);
	}

	int64_t ReadInt64(std::istream& is) {
		uint64_t u = ReadUInt64(is);
		return *reinterpret_cast<int64_t*>(&u);
	}

	void WriteUInt32(std::ostream& os, uint32_t value) {
		char buffer[sizeof(value)];

		buffer[0] = value & 0xFF;
		buffer[1] = ((value >> 8) & 0xFF);
		buffer[2] = ((value >> 16) & 0xFF);
		buffer[3] = ((value >> 24) & 0xFF);

		os.write(buffer, sizeof(buffer));
	}

	void WriteInt32(std::ostream& os, int32_t value) {
		WriteUInt32(os, *reinterpret_cast<uint32_t*>(&value));
	}

	void WriteUInt64(std::ostream& os, uint64_t value) {
		char buffer[sizeof(value)];

		buffer[0] = value & 0xFF;
		buffer[1] = ((value >> 8) & 0xFF);
		buffer[2] = ((value >> 16) & 0xFF);
		buffer[3] = ((value >> 24) & 0xFF);
		buffer[4] = ((value >> 32) & 0xFF);
		buffer[5] = ((value >> 40) & 0xFF);
		buffer[6] = ((value >> 48) & 0xFF);
		buffer[7] = ((value >> 56) & 0xFF);

		os.write(buffer, sizeof(buffer));
	}

	void WriteInt64(std::ostream& os, int64_t value) {
		WriteUInt64(os, *reinterpret_cast<uint64_t*>(&value));
	}
}