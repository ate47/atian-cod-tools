#pragma once

namespace hash {
	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t HashT89Scr(const char* str) {
		uint32_t hash = 0x4B9ACE2F;

		for (const char* data = str; *data; data++) {
			char c = *data >= 'A' && *data <= 'Z' ? (*data - 'A' + 'a') : *data;
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
	constexpr uint64_t Hash64A(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		uint64_t hash = start;

		for (const char* data = str; *data; data++) {
			if (*data >= 'A' && *data <= 'Z') {
				// to lower
				hash = hash ^ (uint8_t)('a' + (*data - (byte)'A'));
			}
			else if (*data == '\\') {
				// replace path char
				hash = hash ^ '/';
			}
			else {
				hash = hash ^ *data;
			}

			hash *= iv;
		}

		return hash;
	}

	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted. Remove high bit
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		return Hash64A(str, start, iv) & 0x7FFFFFFFFFFFFFFF;
	}

	bool TryHashPattern(const char* str, uint64_t& outVal);

	inline uint64_t HashPattern(const char* str) {
		uint64_t out;
		if (TryHashPattern(str, out)) {
			return out;
		}
		return 0;
	}


	inline uint64_t HashPattern(const wchar_t* str) {
		std::wstring_view v{ str };

		if (!v.rfind(L"var_", 0)) {
			return std::wcstoull(&str[4], nullptr, 16);
		}
		if (!v.rfind(L"event_", 0)) {
			return std::wcstoull(&str[6], nullptr, 16);
		}
		if (!v.rfind(L"function_", 0)) {
			return std::wcstoull(&str[9], nullptr, 16);
		}
		if (!v.rfind(L"namespace_", 0)) {
			return std::wcstoull(&str[10], nullptr, 16);
		}
		if (!v.rfind(L"script_", 0)) {
			return std::wcstoull(&str[7], nullptr, 16);
		}
		if (!v.rfind(L"hash_", 0)) {
			return std::wcstoull(&str[5], nullptr, 16);
		}
		if (!v.rfind(L"file_", 0)) {
			return std::wcstoull(&str[5], nullptr, 16);
		}
		return 0;
	}

	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint32_t HashT89ScrPattern(const char* str) {
		uint32_t p = (uint32_t) HashPattern(str);
		return p ? p : HashT89Scr(str);
	}
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint64_t Hash64Pattern(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		uint64_t p = HashPattern(str);
		return p ? p : Hash64(str, start, iv);
	}

	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t HashT89Scr(const wchar_t* str) {
		uint32_t hash = 0x4B9ACE2F;

		for (const wchar_t* data = str; *data; data++) {
			char c = tolower(*data) & 0xFF;
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
	constexpr uint64_t Hash64(const wchar_t* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
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

		return hash & 0x7FFFFFFFFFFFFFFF;
	}
	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint32_t HashT89ScrPattern(const wchar_t* str) {
		uint32_t p = (uint32_t)HashPattern(str);
		return p ? p : HashT89Scr(str);
	}
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint64_t Hash64Pattern(const wchar_t* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		uint64_t p = HashPattern(str);
		return p ? p : Hash64(str, start, iv);
	}
	constexpr uint64_t HashSecure(const char* pattern, uint64_t start, const char* str, uint64_t iv) {
		if (!str || !*str) {
			return 0;
		}

		uint64_t base = hash::Hash64A(pattern, (start ^ *str) * iv, iv);
		return hash::Hash64A(str + 1, base, iv);
	}
	constexpr uint64_t HashIWRes(const char* str, uint64_t start = 0x47F5817A5EF961BA) { return hash::Hash64(str, start); }
	constexpr uint64_t HashIWTag(const char* str, uint64_t start = 0x811C9DC5) { return hash::Hash64(str, start, 0x1000193) & 0xFFFFFFFF; }
	constexpr uint64_t HashJupScr(const char* str, uint64_t start = 0x79D6530B0BB9B5D1) { return hash::Hash64A(str, start, 0x10000000233); }
	constexpr uint32_t HashT7(const char* str) { return (uint32_t)(hash::Hash64A(str, 0x4B9ACE2F, 0x1000193) & 0xFFFFFFFF) * 0x1000193; }
	constexpr uint64_t HashIWDVar(const char* str, uint64_t start = 0) { return !start ? HashSecure("q6n-+7=tyytg94_*", 0xD86A3B09566EBAAC, str, 0x10000000233) : hash::Hash64A(str, start, 0x10000000233); }
	constexpr uint64_t HashT10Scr(const char* str, uint64_t start = 0) { return !start ? HashSecure("zt@f3yp(d[kkd=_@", 0x1C2F2E3C8A257D07, str, 0x10000000233) : hash::Hash64A(str, start, 0x10000000233); }
	constexpr uint64_t HashT10ScrSP(const char* str) { return hash::Hash64A("zt@f3yp(d[kkd=_@", hash::Hash64A(str, 0x1C2F2E3C8A257D07, 0x10000000233), 0x10000000233); }
}