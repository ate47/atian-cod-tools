#pragma once
#include "actslib.hpp"
#include "crc.hpp"

namespace actslib::hdt {
	constexpr const char HDT_COOKIE_MAGIC[] = "$HDT";

	enum HDTCookieType : uint8_t {
		HCT_UNKNOWN = 0,
		HCT_GLOBAL,
		HCT_HEADER,
		HCT_DICTIONARY,
		HCT_TRIPLES,
		HCT_INDEX,
		HCT_QEPCORE_MERGE,
		HCT_COUNT
	};

	const char* FormatName(HDTCookieType type);

	class HDTCookie {
		HDTCookieType type;
		std::string format;
		std::unordered_map<std::string, std::string> props{};
	public:
		using iterator = decltype(props)::iterator;
		using const_iterator = decltype(props)::const_iterator;

		HDTCookie(HDTCookieType type, std::string format) : type(type), format(format) {}
		HDTCookie(std::istream& is) {
			actslib::crc::CRC16 crc{};
			char buffer[sizeof(HDT_COOKIE_MAGIC) - 1];
			is.read(buffer, sizeof(HDT_COOKIE_MAGIC) - 1);

			if (memcmp(buffer, HDT_COOKIE_MAGIC, sizeof(HDT_COOKIE_MAGIC) - 1)) {
				throw std::runtime_error("Invalid HDT cookie magic");
			}
			crc.Update(buffer, 0, sizeof(HDT_COOKIE_MAGIC) - 1);

			type = (HDTCookieType)is.get();
			crc.Update((const char)type);

			std::getline(is, format, '\0');
			crc.Update(format.data(), 0, format.size() + 1);

			std::string propsVal;
			std::getline(is, propsVal, '\0');

			crc.Update(propsVal.data(), 0, propsVal.size() + 1);

			size_t off{};
			while (off < propsVal.size()) {
				size_t idx = propsVal.find(';', off);

				if (idx == std::string::npos) {
					idx = propsVal.size();
				}

				size_t idxeq = propsVal.find('=', off);

				if (idxeq != std::string::npos && idxeq < idx) {
					// split
					props[propsVal.substr(off, idxeq - off)] = propsVal.substr(idxeq + 1, idx - idxeq - 1);
				}

				off = idx + 1;
			}

			if (!crc.CheckCRC(is)) {
				throw std::runtime_error("Invalid HDT cookie CRC");
			}
		}

		constexpr HDTCookieType GetType() const {
			return type;
		}

		constexpr const std::string& GetFormat() const {
			return format;
		}

		constexpr const std::unordered_map<std::string, std::string>& GetProperties() const {
			return props;
		}

		iterator begin() {
			return props.begin();
		}

		iterator end() {
			return props.end();
		}

		const_iterator cbegin() const {
			return props.begin();
		}

		const_iterator cend() const {
			return props.cend();
		}
	};
}