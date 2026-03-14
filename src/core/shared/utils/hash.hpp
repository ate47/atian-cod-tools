#pragma once
#include "hash_mini.hpp"

namespace hash {
	constexpr wchar_t lowerc(wchar_t c) {
		if (c >= 'A' && c <= 'Z') {
			// to lower
			return 'a' + (c - 'A');
		}
		else if (c == '\\') {
			// replace path char
			return '/';
		}
		else {
			return c;
		}
	}

	constexpr uint32_t HashT89Scr(const wchar_t* str) {
		uint32_t hash = 0x4B9ACE2F;

		for (const wchar_t* data = str; *data; data++) {
			char c = lowerc(*data) & 0xFF;
			hash = ((c + hash) ^ ((c + hash) << 10)) + (((c + hash) ^ ((c + hash) << 10)) >> 6);
		}

		return 0x8001 * ((9 * hash) ^ ((9 * hash) >> 11));
	}
	constexpr uint64_t Hash64A(const wchar_t* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (const wchar_t* data = str; *data; data++) {
			hash = (hash ^ (lowerc(*data) & 0xFF)) * iv;
		}

		return hash;
	}

	inline uint64_t Hash64(const std::string& str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		return Hash64(str.data(), start, iv);
	}

	constexpr bool TryHashPattern(const char* str, uint64_t& outVal) {
		std::string_view v{ str };

		if (!str || !*str) {
			outVal = 0;
			return true;
		}

		if (!v.rfind("var_", 0)) {
			outVal = std::strtoull(&str[4], nullptr, 16);
			return true;
		}
		if (!v.rfind("event_", 0)) {
			outVal = std::strtoull(&str[6], nullptr, 16);
			return true;
		}
		if (!v.rfind("function_", 0)) {
			outVal = std::strtoull(&str[9], nullptr, 16);
			return true;
		}
		if (!v.rfind("namespace_", 0)) {
			outVal = std::strtoull(&str[10], nullptr, 16);
			return true;
		}
		if (!v.rfind("script_", 0)) {
			outVal = std::strtoull(&str[7], nullptr, 16);
			return true;
		}
		if (!v.rfind("hash_", 0)) {
			outVal = std::strtoull(&str[5], nullptr, 16);
			return true;
		}
		if (!v.rfind("file_", 0)) {
			outVal = std::strtoull(&str[5], nullptr, 16);
			return true;
		}
		return false;
	}

	constexpr bool TryHashPattern(const wchar_t* str, uint64_t& outVal) {
		std::wstring_view v{ str };

		if (!str || !*str) {
			outVal = 0;
			return true;
		}

		if (!v.rfind(L"var_", 0)) {
			outVal = std::wcstoull(&str[4], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"event_", 0)) {
			outVal = std::wcstoull(&str[6], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"function_", 0)) {
			outVal = std::wcstoull(&str[9], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"namespace_", 0)) {
			outVal = std::wcstoull(&str[10], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"script_", 0)) {
			outVal = std::wcstoull(&str[7], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"hash_", 0)) {
			outVal = std::wcstoull(&str[5], nullptr, 16);
			return true;
		}
		if (!v.rfind(L"file_", 0)) {
			outVal = std::wcstoull(&str[5], nullptr, 16);
			return true;
		}
		return false;
	}

	constexpr uint64_t HashPattern(const char* str) {
		uint64_t out;
		if (TryHashPattern(str, out)) {
			return out;
		}
		return 0;
	}

	constexpr uint64_t HashPattern(const wchar_t* str) {
		uint64_t out;
		if (TryHashPattern(str, out)) {
			return out;
		}
		return 0;
	}

	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64Pattern(const char* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t out;
		if (TryHashPattern(str, out)) return out;
		return Hash64(str, start, iv);
	}
	template<typename StringType>
	constexpr uint64_t Hash64AStr(const StringType& str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (auto it = str.begin(); it != str.end(); it++) {
			hash = (hash ^ lowerc(*it)) * iv;
		}

		return hash;
	}
	
	template<typename StringType>
	constexpr uint64_t Hash64Str(const StringType& str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		return Hash64AStr<StringType>(str, start, iv) & MASK63;
	}
	
	constexpr uint64_t Hash64(const wchar_t* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		return Hash64A(str, start, iv) & MASK63;
	}

	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64Pattern(const wchar_t* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t out;
		if (TryHashPattern(str, out)) return out;
		return Hash64(str, start, iv);
	}

	inline uint64_t HashIWAsset(const std::string& str, uint64_t start = FNV1A_IW_ASSET_PRIME) { return HashIWAsset(str.data(), FNV1A_IW_ASSET_PRIME); }
	inline uint64_t HashX32(const std::string& str, uint64_t start = FNV1A_32_PRIME) { return HashX32(str.data(), FNV1A_32_PRIME); }
	inline uint64_t HashJupScr(const std::string& str, uint64_t start = FNV1A_IW_SCR_PRIME) { return HashJupScr(str.data(), FNV1A_IW_SCR_PRIME); }
	inline uint32_t HashT7(const std::string& str) { return HashT7(str.data()); }
	inline uint64_t HashIWDVar(const std::string& str, uint64_t start = 0) { return HashIWDVar(str.data(), start); }
	inline uint64_t HashT10Scr(const std::string& str, uint64_t start = 0) { return HashT10Scr(str.data(), start); }
	inline uint64_t HashT10ScrSPPre(const std::string& str, uint64_t start = FNV1A_T10_SCR_OFFSET) { return HashT10ScrSPPre(str.data(), start); }
	inline uint64_t HashT10ScrSP(const std::string& str) { return HashT10ScrSP(str.data()); }
}