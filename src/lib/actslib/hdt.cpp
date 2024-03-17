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
}