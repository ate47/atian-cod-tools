#pragma once

namespace hash {
	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	inline UINT32 Hash32(LPCCH str) {
		uint32_t hash = 0x4B9ACE2F;

		for (LPCCH data = str; *data; data++) {
			char c = tolower(*data);
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
	inline UINT64 Hash64(LPCCH str, UINT64 start = 0xcbf29ce484222325LL) {
		UINT64 hash = start;

		for (LPCCH data = str; *data; data++) {
			if (*data >= 'A' && *data <= 'Z') {
				// to lower
				hash = hash ^ (UINT8)(*data - 'A');
			}
			else if (*data == '\\') {
				// replace path char
				hash = hash ^ '/';
			}
			else {
				hash = hash ^ *data;
			}

			hash *= 0x100000001b3;
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

		return Hash64(str);
	}
}