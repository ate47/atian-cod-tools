#pragma once

namespace hash {
	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t Hash32(const char* str) {
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

	inline uint64_t HashPattern(const char* str) {
		std::string_view v{ str };

		if (!v.rfind("var_", 0)) {
			return std::strtoull(&str[4], nullptr, 16);
		}
		if (!v.rfind("event_", 0)) {
			return std::strtoull(&str[6], nullptr, 16);
		}
		if (!v.rfind("function_", 0)) {
			return std::strtoull(&str[9], nullptr, 16);
		}
		if (!v.rfind("namespace_", 0)) {
			return std::strtoull(&str[10], nullptr, 16);
		}
		if (!v.rfind("script_", 0)) {
			return std::strtoull(&str[7], nullptr, 16);
		}
		if (!v.rfind("hash_", 0)) {
			return std::strtoull(&str[5], nullptr, 16);
		}
		if (!v.rfind("file_", 0)) {
			return std::strtoull(&str[5], nullptr, 16);
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
	inline uint32_t Hash32Pattern(const char* str) {
		uint32_t p = (uint32_t) HashPattern(str);
		return p ? p : Hash32(str);
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
	constexpr uint32_t Hash32(const wchar_t* str) {
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
	inline uint32_t Hash32Pattern(const wchar_t* str) {
		uint32_t p = (uint32_t)HashPattern(str);
		return p ? p : Hash32(str);
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
}