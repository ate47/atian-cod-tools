#ifndef __ACTS_API_HASH_H__
#define __ACTS_API_HASH_H__
#include "api.h"
/*
 * Atian Tools Hash API
 */

// Hash type
struct ActsAPIHash_HashType {
	// internal id
	const char* id;
	// hash description
	const char* desc;
	// hash function
	uint64_t(*hashFunc)(const char* text);
};

// Hash types list
struct ActsAPIHash_HashTypeList {
	// values
	ActsAPIHash_HashType* values;
	// count
	size_t count;
};

// Decoded error code (bo4/cw)
typedef const char* ActsAPIHash_ErrorCode[4];

/*
 * @return the registered hashes list
 */
ACTS_COMMON_API ActsAPIHash_HashTypeList* ActsAPIHash_GetHashesList();
/*
 * Encode a t89 error code
 * @param code output
 * @param val value to encode
 * @param alternative use alternative names
 */
ACTS_COMMON_API void ActsAPIHash_EncodeErrorCode(ActsAPIHash_ErrorCode* code, uint32_t val, bool alternative);
/*
 * Decode a t89 error code
 * @param code input
 * @return decoded value or 0 if error
 */
ACTS_COMMON_API uint32_t ActsAPIHash_DecodeErrorCode(const ActsAPIHash_ErrorCode* code);
/*
 * Convert error code to string
 * @param code error code
 * @return string
 */
ACTS_COMMON_API const char* ActsAPIHash_ErrorCodeToStr(const ActsAPIHash_ErrorCode* code);
/*
 * Alloc string hash memory, it is used to store the hashes
 * @param len size
 * @return allocated memory
 */
ACTS_COMMON_API void* ActsAPIHash_AllocHashMemory(size_t len);
/*
 * Clone a string
 * @param str
 * @return copy str
 */
ACTS_COMMON_API const char* ActsAPIHash_CloneHashStr(const char* str);
/*
 * Clean the storage
 */
ACTS_COMMON_API void ActsAPIHash_Clean();
/*
 * Add a hash to the store
 * @param value hash value
 * @param str unhashed value
 * @param clone clone the unhashed value
 * @return allocated string or str if clone = false
 */
ACTS_COMMON_API const char* ActsAPIHash_AddPrecomputed(uint64_t value, const char* str, bool clone = true);
/*
 * Extract a string pointer
 * @param hash value to lookup
 * @return unhashed value or nullptr if not in the store
 */
ACTS_COMMON_API const char* ActsAPIHash_ExtractPtr(uint64_t hash);
/*
 * Extract a string representation
 * @param type type of the hash
 * @param hash hash value
 * @param out out buffer
 * @param outSize out buffer size
 * @return if the value was in the store
 */
ACTS_COMMON_API bool ActsAPIHash_Extract(const char* type, uint64_t hash, char* out, size_t outSize);
/*
 * Extract a string representation
 * @param type type of the hash
 * @param hash hash value
 * @return hash representation
 */
ACTS_COMMON_API char* ActsAPIHash_ExtractTmp(const char* type, uint64_t hash);
/*
 * Read the default hash files
 */
ACTS_COMMON_API void ActsAPIHash_ReadDefaultHashFiles();

#endif // __ACTS_API_HASH_H__