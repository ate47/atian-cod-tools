#pragma once

namespace utils::crc {
	class CRC32 {
		static uint32_t crcTable[256];
		uint32_t crc{ 0xFFFFFFFF };

	public:
		void Update(const void* ptr, size_t len) {
			for (size_t i = 0; i < len; i++) {
				byte b{ ((byte*)ptr)[i] };

				size_t idx = (crc ^ b) & 0xFF;
				crc = crcTable[idx] ^ (crc >> 8);
			}
		}
		template<typename T>
		void Update(const T val) {
			Update(&val, sizeof(val));
		}

		constexpr operator uint32_t() const {
			return ~crc;
		}
	};
}