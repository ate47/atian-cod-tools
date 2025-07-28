#pragma once

namespace utils::crc {
	struct CRC32Table { static uint32_t crcTable[256]; };
	struct CRC32TableB { static uint32_t crcTable[256]; };
	template<bool right = true, typename table = CRC32Table>
	class CRC32 {
		uint32_t crc{ 0xFFFFFFFF };
	public:
		void Update(const void* ptr, size_t len) {
			for (size_t i = 0; i < len; i++) {
				byte b{ ((byte*)ptr)[i] };

				size_t idx = (crc ^ b) & 0xFF;
				if constexpr (right) {
					crc = table::crcTable[idx] ^ (crc >> 8);
				}
				else {
					crc = table::crcTable[idx] ^ (crc << 8);
				}
			}
		}
		void Update(const char* str) {
			Update(str, std::strlen(str) + 1);
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