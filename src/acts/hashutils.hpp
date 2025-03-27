#pragma once
#include <Windows.h>
#include <utils/hash.hpp>

namespace hashutils {
	constexpr uint64_t MASK62 = hash::MASK62;
	constexpr uint64_t MASK63 = hash::MASK63;
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
	const std::unordered_map<uint64_t, const char*>& GetMap();

	/*
	 * Alloc string hash memory, it is used to store the hashes
	 * @param len size
	 * @return allocated memory
	 */
	void* AllocHashMemory(size_t len);
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
	 * @param unk unk values
	 */
	void SaveExtracted(bool value, bool unk);
	/*
	 * Save the extract hashes
	 * @param file file to read
	 */
	void WriteExtracted(const char* file);
	/*
	 * Save the extract hashes
	 * @param extracted set to add the hashes
	 */
	void SetExtracted(std::unordered_set<uint64_t>* extracted);
	/*
	 * Add a hash into the map
	 * @param str string
	 * @param ignoreCol ignore collisions
	 * @param iw hash
	 * @param async run async
	 * @param clone clone string
	 * @return if it collided with another string
	 */
	bool Add(const char* str, bool ignoreCol = true, bool iw = false, bool async = false, bool clone = true);
	/*
	 * Add a precomputed hash into the map
	 * @param value hash value
	 * @param str string
	 * @param async run async
	 * @param clone clone string
	 */
	void AddPrecomputed(uint64_t value, const char* str, bool async = false, bool clone = true);
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
	 * Extract a hash into a buffer with test value
	 * @param type Hash type
	 * @param hash Hashed value
	 * @return return value of extract and non thread-safe temporary pointer to a representation of this hash, the result is valid until the next call to ExtractTmp
	 */
	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash);
	/*
	 * Extract a hash into a buffer
	 * @param type Hash type
	 * @param hash Hashed value
	 * @return non thread-safe temporary pointer to a representation of this hash, the result is valid until the next call to ExtractTmp
	 */
	char* ExtractTmp(const char* type, uint64_t hash);
	/*
	 * Apply path formatting
	 * @param path path
	 * @return path formatted value
	 */
	inline char* CleanPath(char* path) {
		// replace '/' -> '\' in script
		for (char* script = path; *script; script++) {
			if (*script == '/') {
				*script = '\\';
			}
		}
		return path;
	}
	/*
	 * Call ExtractTmp for a path name and apply formatting
	 * @param type Hash type
	 * @param hash Script hash
	 * @return script formatted value, same condition as ExtractTmp
	 */
	inline char* ExtractTmpPath(const char* type, uint64_t hash) {
		char* unhash = ExtractTmp(type, hash);

		return CleanPath(unhash);
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
}