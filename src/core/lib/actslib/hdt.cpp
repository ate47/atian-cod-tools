#include "hdt.hpp"

namespace actslib::hdt {
	namespace {
		const char* HDTCookieTypeNames[] {
			"unknown",
			"global",
			"header",
			"dictionary",
			"triples",
			"index",
			"qepcore_merge",
		};
	}

	const char* FormatName(HDTCookieType type) {
		return type >= 0 && type < HDTCookieType::HCT_COUNT ? HDTCookieTypeNames[type] : "bad_type";
	}

	Header* LoadHeader(std::istream& is) {
		HDTCookie cookie{ is };

		if (cookie.GetType() != HCT_HEADER) {
			throw std::runtime_error(actslib::va("Excepted header type, find: %s", FormatName(cookie.GetType())));
		}

		const auto& format = cookie.GetFormat();

		if (format == "ntriples") {
			return new PlainHeader(is, cookie);
		}
		else {
			throw std::runtime_error(actslib::va("Unknown header format: %s", format.c_str()));
		}

	}
}