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
		uint64_t* data;
		size_t datalen;
		size_t numbits;
		uint64_t maxValue;
	public:
		static constexpr size_t WORD_LEN = sizeof(*Sequence::data) * 8;

		Sequence(char* data, size_t datalen, size_t numbits) : 
			data(reinterpret_cast<uint64_t*>(data)), datalen(datalen), numbits(numbits), 
			maxValue(numbits == 64 ? ~0 : ((1ull << numbits) - 1)) {
			if (numbits > WORD_LEN) {
				throw std::runtime_error(actslib::va("numbits can't be above %lld: %lld > %lld", WORD_LEN, numbits, WORD_LEN));
			}
		}

		uint64_t operator[](size_t id) const {
			size_t offset = id * numbits;
			size_t word = offset / WORD_LEN;
			size_t wordOff = offset % WORD_LEN;

			if (wordOff + numbits > WORD_LEN) {
				// 2 words
				return (data[word] >> wordOff) | ((data[word + 1] << ((WORD_LEN << 1) - wordOff - numbits)) >> (WORD_LEN - numbits));
			}
			else {
				// 1 word
				return (data[word] << (WORD_LEN - wordOff - numbits)) >> (WORD_LEN - numbits);
			}
		}

		void Set(size_t id, uint64_t value) {
			if (value > maxValue) {
				throw std::runtime_error(actslib::va("value is above limit for this structure %lld > %lld", value, maxValue));
			}
			size_t offset = id * numbits;
			size_t word = offset / WORD_LEN;
			size_t wordOff = offset % WORD_LEN;



		}


		void UpdateCRC(actslib::crc::CRC32& crc) const {
			crc.Update(reinterpret_cast<char*>(data), 0, datalen);
		}
	};


}