#pragma once
#include <utils/hash.hpp>

namespace core::hashes {
	/*
	 * Alloc string hash memory, it is used to store the hashes
	 * @param len size
	 * @return allocated memory
	 */
	void* AllocHashMemory(size_t len);
	
	/*
	 * Clone a string
	 * @param str
	 * @return copy str
	 */
	const char* CloneHashStr(const char* str);
	
	/*
	 * Clean the storage
	 */
	void Clean();
	
	/*
	 * @return the hash map
	 */
	const std::unordered_map<uint64_t, const char*>& GetMap();
	
	/*
	 * Add a hash to the store
	 * @param value hash value
	 * @param str unhashed value
	 * @param clone clone the unhashed value
	 * @return allocated string or str if clone = false
	 */
	const char* AddPrecomputed(uint64_t value, const char* str, bool clone = true);
	
	/*
	 * Extract a string pointer
	 * @param hash value to lookup
	 * @return unhashed value or nullptr if not in the store
	 */
	const char* ExtractPtr(uint64_t hash);
	
	/*
	 * Extract a string representation
	 * @param type type of the hash
	 * @param hash hash value
	 * @param out out buffer
	 * @param outSize out buffer size
	 * @return if the value was in the store
	 */
	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize);
	
	/*
	 * Extract a string representation
	 * @param type type of the hash
	 * @param hash hash value
	 * @return if the value was in the store and the hash representation
	 */
	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash);

	/*
	 * Extract a string representation
	 * @param type type of the hash
	 * @param hash hash value
	 * @return hash representation
	 */
	char* ExtractTmp(const char* type, uint64_t hash);
}