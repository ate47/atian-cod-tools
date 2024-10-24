#pragma once
#include <Windows.h>
#include <hash.hpp>

namespace hashutils {
	constexpr uint64_t MASK62 = 0xFFFFFFFFFFFFFFFull;
	constexpr uint64_t MASK63 = 0x7FFFFFFFFFFFFFFFull;
	constexpr auto DEFAULT_HASH_FILE = "strings.txt";

	/*
	 * Get hash mutex
	 * @param async run async
	 * @return mutex or nullptr
	 */
	std::mutex* GetMutex(bool async);

	/*
	 * Get hash map
	 */
	const std::unordered_map<uint64_t, std::string>& GetMap();
	/*
	 * Read the default hash file
	 */
	void ReadDefaultFile(bool cleanup = false);
	/*
	 * Load a hash file
	 * @param file file to load
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 * @param async run async
	 * @return collisions found (if ignoreCol = false)
	 */
	int LoadMap(const char* file, bool ignoreCol = true, bool iw = false, bool async = false);
	/*
	 * Save the extract hashes for a future use with WriteExtracted
	 * @param value save extracted
	 */
	void SaveExtracted(bool value);
	/*
	 * Save the extract hashes
	 * @param file file to read
	 */
	void WriteExtracted(const char* file);
	/*
	 * Add a hash into the map
	 * @param str string
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 * @param async run async
	 * @return if it collided with another string
	 */
	bool Add(const char* str, bool ignoreCol = true, bool iw = false, bool async = false);
	/*
	 * Add a precomputed hash into the map
	 * @param value hash value
	 * @param str string
	 * @param async run async
	 */
	void AddPrecomputed(uint64_t value, const char* str, bool async = false);
	/*
	 * Extract a hash into a buffer
	 * @param type Hash type
	 * @param hash Hashed value
	 * @param out Out buffer
	 * @param outSize Out size
	 * @return if the hash was in the hash map
	 */
	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize);
	/*
	 * Extract a hash into a buffer
	 * @param type Hash type
	 * @param hash Hashed value
	 * @return non thread-safe temporary pointer to a representation of this hash, the result is valid until the next call to ExtractTmp
	 */
	char* ExtractTmp(const char* type, uint64_t hash);
	/*
	 * Call ExtractTmp for a path name and apply formatting
	 * @param type Hash type
	 * @param hash Script hash
	 * @return script formatted value, same condition as ExtractTmp
	 */
	inline char* ExtractTmpPath(const char* type, uint64_t hash) {
		char* unhash = ExtractTmp(type, hash);

		// replace '/' -> '\' in script
		for (char* script = unhash; *script; script++) {
			if (*script == '/') {
				*script = '\\';
			}
		}

		return unhash;
	}
	/*
	 * Call ExtractTmp for a script name and apply formatting
	 * @param hash Script hash
	 * @return script formatted value, same condition as ExtractTmp
	 */
	inline char* ExtractTmpScript(uint64_t hash) {
		return ExtractTmpPath("script", hash);
	}
	/*
	 * Extract a pointer to a hash value
	 * @param hash Hashed value
	 * @return String to the value or NULL if the hash isn't in the hash map
	 */
	const char* ExtractPtr(uint64_t hash);
	/*
	 * @return the size of the hash map
	 */
	size_t Size();
	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint32_t Hash32Pattern(const char* str) { return hash::Hash32Pattern(str); }
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline uint64_t Hash64Pattern(const char* str) { return hash::Hash64Pattern(str); }

	constexpr uint64_t HashSecure(const char* pattern, uint64_t start, const char* str, uint64_t iv) {
		if (!str || !*str) {
			return 0;
		}

		uint64_t base = hash::Hash64A(pattern, (start ^ *str) * iv, iv);
		return hash::Hash64A(str + 1, base, iv);
	}

	constexpr uint32_t Hash32(const char* str) { return hash::Hash32(str); }
	constexpr uint64_t Hash64(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) { return hash::Hash64(str, start, iv); }
	constexpr uint64_t HashIWRes(const char* str, uint64_t start = 0x47F5817A5EF961BA) { return hash::Hash64(str, start); }
	constexpr uint64_t HashIWTag(const char* str, uint64_t start = 0x811C9DC5) { return hash::Hash64(str, start, 0x1000193) & 0xFFFFFFFF; }
	constexpr uint64_t Hash64A(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) { return hash::Hash64A(str, start, iv); }
	constexpr uint64_t HashJupScr(const char* str, uint64_t start = 0x79D6530B0BB9B5D1) { return hash::Hash64A(str, start, 0x10000000233); }
	constexpr uint32_t HashT7(const char* str) { return (uint32_t)(hash::Hash64A(str, 0x4B9ACE2F, 0x1000193) & 0xFFFFFFFF) * 0x1000193; }
	constexpr uint64_t HashIWDVar(const char* str, uint64_t start = 0) { return !start ? HashSecure("q6n-+7=tyytg94_*", 0xD86A3B09566EBAAC, str, 0x10000000233) : hash::Hash64A(str, start, 0x10000000233); }
	constexpr uint64_t HashT10Scr(const char* str, uint64_t start = 0) { return !start ? HashSecure("zt@f3yp(d[kkd=_@", 0x1C2F2E3C8A257D07, str, 0x10000000233) : hash::Hash64A(str, start, 0x10000000233); }
}