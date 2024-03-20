#include "actslib.hpp"
#include "io.h"

namespace actslib::io {
	uint8_t ReadUInt8(std::istream& is) {
		char buffer;

		is.read(&buffer, sizeof(buffer));

		return buffer;
	}

	int8_t ReadInt8(std::istream& is) {
		uint8_t u = ReadUInt8(is);
		return *reinterpret_cast<int8_t*>(&u);
	}

	uint16_t ReadUInt16(std::istream& is) {
		char buffer[2];

		is.read(buffer, sizeof(buffer));

		return
			(buffer[0] & 0xFFUL)
			| ((buffer[1] & 0xFFUL) << 8);
	}

	int16_t ReadInt16(std::istream& is) {
		uint16_t u = ReadUInt16(is);
		return *reinterpret_cast<int16_t*>(&u);
	}

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

	void WriteUInt8(std::ostream& os, uint8_t value) {
		os.write((const char*)&value, sizeof(value));
	}

	void WriteInt8(std::ostream& os, int8_t value) {
		WriteUInt8(os, *reinterpret_cast<uint8_t*>(&value));
	}

	void WriteUInt16(std::ostream& os, uint16_t value) {
		char buffer[sizeof(value)];

		buffer[0] = value & 0xFF;
		buffer[1] = ((value >> 8) & 0xFF);

		os.write(buffer, sizeof(buffer));
	}

	void WriteInt16(std::ostream& os, int16_t value) {
		WriteUInt16(os, *reinterpret_cast<uint16_t*>(&value));
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

	void EncodeVByte(std::ostream& os, uint64_t value) {
		while (value > 0x7F) {
			os.put((char)(value & 0x7F) | 0x80);
			value >>= 7;
		}
		os.put((char)((value & 0x7F)));
	}

	uint64_t DecodeVByte(std::istream& is) {
		uint64_t res{};
		int shift{};

		int c = is.get();

		while (c & 0x80) {
			if (shift > 50) throw std::runtime_error("Too many bits");

			res |= (c & 0x7FULL) << shift;

			c = is.get();
			shift += 7;
		}

		res |= (c & 0x7FULL) << shift;
		return res;
	}

	void EncodeVByteSigned(std::ostream& os, int64_t value) {
		if (value < 0) {
			EncodeVByte(os, ~(value << 1));
		}
		else {
			EncodeVByte(os, value << 1);
		}
	}

	int64_t DecodeVByteSigned(std::istream& is) {
		uint64_t d = DecodeVByte(is);

		if (d & 1) {
			return ~(d >> 1);
		}
		return d >> 1;
	}
}