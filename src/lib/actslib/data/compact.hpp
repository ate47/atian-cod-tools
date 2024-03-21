#pragma once
#include "../actslib.hpp"
#include "../crc.hpp"

namespace actslib::data::compact {

	class Bitmap64 {
		char* data;
		size_t datalen;
	public:
		Bitmap64(char* data, size_t datalen) : data(data), datalen(datalen) {}

		inline size_t WordOfId(size_t id) const {
			return id >> 3;
		}

		bool operator[](size_t id) const {
			size_t word = WordOfId(id);
			if (word >= datalen) {
				return false;
			}

			return data[word] & (1 << (id & 7));
		}

		void Set(size_t id, bool value) {
			size_t word = WordOfId(id);
			if (value) {
				if (word >= datalen) {
					throw std::runtime_error(actslib::va("Trying to add in a bitmap an element outside the words! %lld (%lld/%lld)", id, word, datalen));
				}
				data[word] |= (1 << (id & 7));
			}
			else {
				data[word] &= ~(1 << (id & 7));
			}
		}

		void UpdateCRC(actslib::crc::CRC32& crc) const {
			crc.Update(data, 0, datalen);
		}
	};

	class Sequence {

	public:
		Sequence() {}
	};


}