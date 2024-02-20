#pragma once
#include <Windows.h>
#include <hash.hpp>

namespace hashutils {
	constexpr auto DEFAULT_HASH_FILE = L"strings.txt";


	/*
	 * Read the default hash file
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 */
	void ReadDefaultFile(bool ignoreCol = true, bool iw = false);
	/*
	 * Load a hash file
	 * @param file file to load
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 * @return collisions found (if ignoreCol = false)
	 */
	int LoadMap(LPCWCH file, bool ignoreCol = true, bool iw = false);
	/*
	 * Save the extract hashes for a future use with WriteExtracted
	 * @param value save extracted
	 */
	void SaveExtracted(bool value);
	/*
	 * Save the extract hashes
	 * @param file file to read
	 */
	void WriteExtracted(LPCCH file);
	/*
	 * Add a hash into the map
	 * @param str string
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 * @return if it collided with another string
	 */
	bool Add(LPCCH str, bool ignoreCol = true, bool iw = false);
	/*
	 * Extract a hash into a buffer
	 * @param type Hash type
	 * @param hash Hashed value
	 * @param out Out buffer
	 * @param outSize Out size
	 * @return if the hash was in the hash map
	 */
	bool Extract(LPCCH type, UINT64 hash, LPCH out, SIZE_T outSize);
	/*
	 * Extract a hash into a buffer
	 * @param type Hash type
	 * @param hash Hashed value
	 * @return non thread-safe temporary pointer to a representation of this hash, the result is valid until the next call to ExtractTmp
	 */
	LPCH ExtractTmp(LPCCH type, UINT64 hash);
	/*
	 * Call ExtractTmp for a path name and apply formatting
	 * @param type Hash type
	 * @param hash Script hash
	 * @return script formatted value, same condition as ExtractTmp
	 */
	inline LPCH ExtractTmpPath(LPCCH type, UINT64 hash) {
		LPCH unhash = ExtractTmp(type, hash);

		// replace '/' -> '\' in script
		for (LPCH script = unhash; *script; script++) {
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
	inline LPCH ExtractTmpScript(UINT64 hash) {
		return ExtractTmpPath("script", hash);
	}
	/*
	 * Extract a pointer to a hash value
	 * @param hash Hashed value
	 * @return String to the value or NULL if the hash isn't in the hash map
	 */
	const char* ExtractPtr(UINT64 hash);
	/*
	 * @return the size of the hash map
	 */
	SIZE_T Size();

	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	constexpr uint32_t Hash32(const char* str) { return hash::Hash32(str); }
	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	constexpr uint64_t Hash64(const char* str, uint64_t start = 0xcbf29ce484222325LL, uint64_t iv = 0x100000001b3) { return hash::Hash64(str, start, iv); }
	constexpr uint64_t HashIW(const char* str) { return hash::Hash64(str, 0x47F5817A5EF961BA); }
	constexpr uint64_t HashIW2(const char* str) { return hash::Hash64(str, 0x79D6530B0BB9B5D1, 0x10000000233); }
	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT32 Hash32Pattern(const char* str) { return hash::Hash32Pattern(str); }
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT64 Hash64Pattern(const char* str) { return hash::Hash64Pattern(str); }
}