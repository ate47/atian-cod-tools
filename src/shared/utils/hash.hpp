#pragma once

namespace hash {
	constexpr uint64_t MASK32 = 0xFFFFFFFF;
	constexpr uint64_t MASK62 = 0xFFFFFFFFFFFFFFFull;
	constexpr uint64_t MASK63 = 0x7FFFFFFFFFFFFFFFull;
	constexpr uint64_t IV_DEFAULT = 0x100000001b3;
	constexpr uint64_t IV_32_DEFAULT = 0x1000193;
	constexpr uint64_t IV_TYPE2 = 0x10000000233;

	constexpr uint64_t FNV1A_PRIME = 0xcbf29ce484222325LL;
	constexpr uint64_t FNV1A_32_PRIME = 0x811C9DC5;

	constexpr uint64_t FNV1A_IW_PRIME = 0x47F5817A5EF961BA;
	constexpr uint64_t FNV1A_IW_SCR_PRIME = 0x79D6530B0BB9B5D1;
	constexpr uint64_t FNV1A_IW_DVAR_OFFSET = 0xD86A3B09566EBAAC;
	constexpr uint64_t FNV1A_T10_SCR_OFFSET = 0x1C2F2E3C8A257D07;
	constexpr uint64_t FNV1A_32_T7_PRIME = 0x4B9ACE2F;

	constexpr const char* XHASHSEC_DVAR_STR = "q6n-+7=tyytg94_*";
	constexpr const char* XHASHSEC_T10_SCR_STR = "zt@f3yp(d[kkd=_@";

	constexpr char lowerc(char c) {
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

	constexpr uint32_t HashT89ScrPre(const char* str, uint32_t start = 0x4B9ACE2F) {
		uint32_t hash{ start };

		for (const char* data = str; *data; data++) {
			char c = lowerc(*data);
			hash = ((c + hash) ^ ((c + hash) << 10)) + (((c + hash) ^ ((c + hash) << 10)) >> 6);
		}

		return hash;
	}

	constexpr uint32_t HashT89ScrPost(uint32_t hash) {
		return 0x8001 * ((9 * hash) ^ ((9 * hash) >> 11));
	}
	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t HashT89Scr(const char* str) {
		return HashT89ScrPost(HashT89ScrPre(str));
	}

	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t HashT89Scr(const wchar_t* str) {
		uint32_t hash = 0x4B9ACE2F;

		for (const wchar_t* data = str; *data; data++) {
			char c = lowerc(*data) & 0xFF;
			hash = ((c + hash) ^ ((c + hash) << 10)) + (((c + hash) ^ ((c + hash) << 10)) >> 6);
		}

		return 0x8001 * ((9 * hash) ^ ((9 * hash) >> 11));
	}

	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64A(const char* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (const char* data = str; *data; data++) {
			hash = (hash ^ lowerc(*data)) * iv;
		}

		return hash;
	}
	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64A(const wchar_t* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (const wchar_t* data = str; *data; data++) {
			hash = (hash ^ (lowerc(*data) & 0xFF)) * iv;
		}

		return hash;
	}

	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted. Remove high bit
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64(const char* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		return Hash64A(str, start, iv) & MASK63;
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

	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64(const wchar_t* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (const wchar_t* data = str; *data; data++) {
			if (*data >= 'A' && *data <= 'Z') {
				// to lower
				hash = hash ^ (uint8_t)(*data - (byte)'A');
			}
			else if (*data == '\\') {
				// replace path char
				hash = hash ^ '/';
			}
			else {
				hash = hash ^ (*data & 0xFF);
			}

			hash *= iv;
		}

		return hash & MASK63;
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
	constexpr uint64_t HashSecure(const char* pattern, uint64_t start, const char* str, uint64_t iv) {
		if (!str || !*str) {
			return 0;
		}

		uint64_t base = hash::Hash64A(pattern, (start ^ *str) * iv, iv);
		return hash::Hash64A(str + 1, base, iv);
	}

	constexpr uint64_t HashIWRes(const char* str, uint64_t start = FNV1A_IW_PRIME) { return hash::Hash64(str, start); }
	constexpr uint64_t HashIWTag(const char* str, uint64_t start = FNV1A_32_PRIME) { return hash::Hash64(str, start, IV_32_DEFAULT) & MASK32; }
	constexpr uint64_t HashJupScr(const char* str, uint64_t start = FNV1A_IW_SCR_PRIME) { return hash::Hash64A(str, start, IV_TYPE2); }
	constexpr uint32_t HashT7(const char* str) { return (uint32_t)(hash::Hash64A(str, FNV1A_32_T7_PRIME, IV_32_DEFAULT) & MASK32) * IV_32_DEFAULT; }
	constexpr uint64_t HashIWDVar(const char* str, uint64_t start = 0) { return !start ? HashSecure(XHASHSEC_DVAR_STR, FNV1A_IW_DVAR_OFFSET, str, IV_TYPE2) : hash::Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10Scr(const char* str, uint64_t start = 0) { return !start ? HashSecure(XHASHSEC_T10_SCR_STR, FNV1A_T10_SCR_OFFSET, str, IV_TYPE2) : hash::Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10ScrSPPre(const char* str, uint64_t start = FNV1A_T10_SCR_OFFSET) { return hash::Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10ScrSPPost(uint64_t hash) { return hash::Hash64A(XHASHSEC_T10_SCR_STR, hash); }
	constexpr uint64_t HashT10ScrSP(const char* str) { return HashT10ScrSPPost(HashT10ScrSPPre(str)); }

	inline uint64_t HashIWRes(const std::string& str, uint64_t start = FNV1A_IW_PRIME) { return HashIWRes(str.data(), FNV1A_IW_PRIME); }
	inline uint64_t HashIWTag(const std::string& str, uint64_t start = FNV1A_32_PRIME) { return HashIWTag(str.data(), FNV1A_32_PRIME); }
	inline uint64_t HashJupScr(const std::string& str, uint64_t start = FNV1A_IW_SCR_PRIME) { return HashJupScr(str.data(), FNV1A_IW_SCR_PRIME); }
	inline uint32_t HashT7(const std::string& str) { return HashT7(str.data()); }
	inline uint64_t HashIWDVar(const std::string& str, uint64_t start = 0) { return HashIWDVar(str.data(), start); }
	inline uint64_t HashT10Scr(const std::string& str, uint64_t start = 0) { return HashT10Scr(str.data(), start); }
	inline uint64_t HashT10ScrSPPre(const std::string& str, uint64_t start = FNV1A_T10_SCR_OFFSET) { return HashT10ScrSPPre(str.data(), start); }
	inline uint64_t HashT10ScrSP(const std::string& str) { return HashT10ScrSP(str.data()); }
}