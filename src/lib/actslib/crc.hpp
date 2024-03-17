#pragma once
#include "io.hpp"

namespace actslib::crc {
	template<typename CRCType>
	class CRC {
	public:
		CRCType value{};
		CRC() {}

		virtual void Update(const char val) = 0;
		void Update(const char* buffer, size_t offset, size_t length) {
			for (size_t i = 0; i < length; i++) {
				Update(buffer[offset + i]);
			}
		}

		constexpr void Reset() {
			value = 0;
		}

		constexpr void WriteCRC(std::ostream& os) const {
			actslib::io::WriteUInt<CRCType>(os, value);
		}

		constexpr bool CheckCRC(std::istream& is) const {
			return actslib::io::ReadUInt<CRCType>(is) == value;
		}
	};

	class CRC8 : public CRC<uint8_t> {
	public:
		void Update(const char val) override;
	};
	class CRC16 : public CRC<uint16_t> {
	public:
		using CRC::Update;
		using CRC::Reset;
		using CRC::WriteCRC;
		void Update(const char val) override;
	};
	class CRC32 : public CRC<uint32_t> {
	public:
		using CRC::Update;
		using CRC::Reset;
		using CRC::WriteCRC;
		void Update(const char val) override;
	};
}