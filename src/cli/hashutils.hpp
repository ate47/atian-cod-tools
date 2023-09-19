#pragma once
#include <Windows.h>
#include <hash.hpp>

namespace hashutils {
	constexpr auto DEFAULT_HASH_FILE = L"strings.txt";
	/*
	 * Read the default hash file
	 * @param ignoreCol ignore collisions
	 */
	void ReadDefaultFile(bool ignoreCol = true);
	/*
	 * Load a hash file
	 * @param file file to load
	 * @param ignoreCol ignore collisions
	 * @return collisions found (if ignoreCol = false)
	 */
	int LoadMap(LPCWCH file, bool ignoreCol = true);
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
	 * @return if it collided with another string
	 */
	bool Add(LPCCH str, bool ignoreCol = true);
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
	 * Call ExtractTmp for a script name and apply formatting
	 * @param hash Script hash
	 * @return script formatted value, same condition as ExtractTmp
	 */
	inline LPCH ExtractTmpScript(UINT64 hash) {
		LPCH unhash = ExtractTmp("script", hash);

		// replace '/' -> '\' in script
		for (LPCH script = unhash; *script; script++) {
			if (*script == '/') {
				*script = '\\';
			}
		}

		return unhash;
	}
	/*
	 * Extract a pointer to a hash value
	 * @param hash Hashed value
	 * @return String to the value or NULL if the hash isn't in the hash map
	 */
	LPCCH ExtractPtr(UINT64 hash);
	/*
	 * @return the size of the hash map
	 */
	SIZE_T Size();

	/*
	 * Compute the hash32 on a string (canon id)
	 * @param str String to compute
	 * @return hashed value
	 */
	inline UINT32 Hash32(LPCCH str) { return hash::Hash32(str); }
	/*
	 * Compute the hash64 on a string (fnva1), path are unformatted
	 * @param str String to compute
	 * @param start Start value, can be a previous hash to concatenate hashes
	 * @return Hashed value
	 */
	inline UINT64 Hash64(LPCCH str, UINT64 start = 0xcbf29ce484222325LL) { return hash::Hash64(str, start); }
	/*
	 * Compute the hash32 on a string (canon id), but allow pattern like "function_123456"
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT32 Hash32Pattern(LPCCH str) { return hash::Hash32Pattern(str); }
	/*
	 * Compute the hash64 on a string (fnva1), but allow pattern like "hash_123456", path are unformatted
	 * @param str String to compute
	 * @return Hashed value
	 */
	inline UINT64 Hash64Pattern(LPCCH str) { return hash::Hash64Pattern(str); }
}