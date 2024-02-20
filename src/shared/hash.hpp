#pragma once

namespace hash {
	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t Hash32(const char* str) {
		uint32_t hash = 0x4B9ACE2F;

		for (LPCCH data = str; *data; data++) {
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
	constexpr uint64_t Hash64(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		UINT64 hash = start;

		for (LPCCH data = str; *data; data++) {
			if (*data >= 'A' && *data <= 'Z') {
				// to lower
				hash = hash ^ (UINT8)('a' + (*data - (BYTE)'A'));
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

		return hash & 0x7FFFFFFFFFFFFFFF;
	}
	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT32 Hash32Pattern(LPCCH str) {
		std::string_view v{ str };

		if (!v.rfind("var_", 0)) {
			return std::strtoul(&str[4], nullptr, 16);
		}
		if (!v.rfind("event_", 0)) {
			return std::strtoul(&str[6], nullptr, 16);
		}
		if (!v.rfind("function_", 0)) {
			return std::strtoul(&str[9], nullptr, 16);
		}
		if (!v.rfind("namespace_", 0)) {
			return std::strtoul(&str[10], nullptr, 16);
		}

		return Hash32(str);
	}
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT64 Hash64Pattern(LPCCH str) {
		std::string_view v{ str };

		if (!v.rfind("script_", 0)) {
			return std::strtoull(&str[7], nullptr, 16);
		}

		if (!v.rfind("hash_", 0)) {
			return std::strtoull(&str[5], nullptr, 16);
		}

		if (!v.rfind("file_", 0)) {
			return std::strtoull(&str[5], nullptr, 16);
		}

		return Hash64(str);
	}

	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t Hash32(LPCWCH str) {
		uint32_t hash = 0x4B9ACE2F;

		for (LPCWCH data = str; *data; data++) {
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
	constexpr uint64_t Hash64(LPCWCH str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) {
		uint64_t hash = start;

		for (LPCWCH data = str; *data; data++) {
			if (*data >= 'A' && *data <= 'Z') {
				// to lower
				hash = hash ^ (UINT8)(*data - (BYTE)'A');
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
	inline UINT32 Hash32Pattern(LPCWCH str) {
		std::wstring_view v{ str };

		if (!v.rfind(L"var_", 0)) {
			return std::wcstoul(&str[4], nullptr, 16);
		}
		if (!v.rfind(L"event_", 0)) {
			return std::wcstoul(&str[6], nullptr, 16);
		}
		if (!v.rfind(L"function_", 0)) {
			return std::wcstoul(&str[9], nullptr, 16);
		}
		if (!v.rfind(L"namespace_", 0)) {
			return std::wcstoul(&str[10], nullptr, 16);
		}

		return Hash32(str);
	}
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT64 Hash64Pattern(LPCWCH str) {
		std::wstring_view v{ str };

		if (!v.rfind(L"script_", 0)) {
			return std::wcstoull(&str[7], nullptr, 16);
		}

		if (!v.rfind(L"hash_", 0)) {
			return std::wcstoull(&str[5], nullptr, 16);
		}

		if (!v.rfind(L"file_", 0)) {
			return std::wcstoull(&str[5], nullptr, 16);
		}

		return Hash64(str);
	}
}