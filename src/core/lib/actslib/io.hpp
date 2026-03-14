#pragma once
#include "actslib.hpp"

/*
 * IO utilities
 */
namespace actslib::io {
	uint8_t ReadUInt8(std::istream& is);
	int8_t ReadInt8(std::istream& is);
	uint16_t ReadUInt16(std::istream& is);
	int16_t ReadInt16(std::istream& is);
	uint32_t ReadUInt32(std::istream& is);
	int32_t ReadInt32(std::istream& is);
	uint64_t ReadUInt64(std::istream& is);
	int64_t ReadInt64(std::istream& is);

	void WriteUInt8(std::ostream& os, uint8_t value);
	void WriteInt8(std::ostream& os, int8_t value);
	void WriteUInt16(std::ostream& os, uint16_t value);
	void WriteInt16(std::ostream& os, int16_t value);
	void WriteUInt32(std::ostream& os, uint32_t value);
	void WriteInt32(std::ostream& os, int32_t value);
	void WriteUInt64(std::ostream& os, uint64_t value);
	void WriteInt64(std::ostream& os, int64_t value);

	template<typename Type>
	void WriteUInt(std::ostream& os, Type value) = delete;
	template<typename Type>
	void WriteInt(std::ostream& os, Type value) = delete;
	template<typename Type>
	Type ReadUInt(std::istream& is) = delete;
	template<typename Type>
	Type ReadInt(std::istream& is) = delete;

	template<> inline void WriteUInt(std::ostream& os, uint8_t value) { WriteUInt8(os, value); }
	template<> inline void WriteInt(std::ostream& os, int8_t value) { WriteInt8(os, value); }
	template<> inline void WriteUInt(std::ostream& os, uint16_t value) { WriteUInt16(os, value); }
	template<> inline void WriteInt(std::ostream& os, int16_t value) { WriteInt16(os, value); }
	template<> inline void WriteUInt(std::ostream& os, uint32_t value) { WriteUInt32(os, value); }
	template<> inline void WriteInt(std::ostream& os, int32_t value) { WriteInt32(os, value); }
	template<> inline void WriteUInt(std::ostream& os, uint64_t value) { WriteUInt64(os, value); }
	template<> inline void WriteInt(std::ostream& os, int64_t value) { WriteInt64(os, value); }

	template<> inline uint8_t ReadUInt(std::istream& is) { return ReadUInt8(is); }
	template<> inline int8_t ReadInt(std::istream& is) { return ReadInt8(is); }
	template<> inline uint16_t ReadUInt(std::istream& is) { return ReadUInt16(is); }
	template<> inline int16_t ReadInt(std::istream& is) { return ReadInt16(is); }
	template<> inline uint32_t ReadUInt(std::istream& is) { return ReadUInt32(is); }
	template<> inline int32_t ReadInt(std::istream& is) { return ReadInt32(is); }
	template<> inline uint64_t ReadUInt(std::istream& is) { return ReadUInt64(is); }
	template<> inline int64_t ReadInt(std::istream& is) { return ReadInt64(is); }


	void EncodeVByte(std::ostream& os, uint64_t value);
	void EncodeVByteSigned(std::ostream& os, int64_t value);
	uint64_t DecodeVByte(std::istream& is);
	int64_t DecodeVByteSigned(std::istream& is);
}