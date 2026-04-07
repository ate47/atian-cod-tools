#pragma once
#include "api.hpp"
/*
 * Atian Tools Hash API
 */

namespace acts::api {

	// Hash type
	struct HashType {
		// internal id
		const char* id;
		// hash description
		const char* desc;
		// hash function
		uint64_t(*hashFunc)(const char* text);
	};

	// Decoded error code (bo4/cw)
	typedef const char* ErrorCode[4];

	class ActsAPIHash {
	public:
		// @return the registered hashes
		virtual HashType* GetHashes() = 0;
		// @return the registered hashes count
		virtual size_t GetHashesCount() = 0;
		/*
		 * Encode a t89 error code
		 * @param code output
		 * @param val value to encode
		 * @param alternative use alternative names
		 */
		virtual void EncodeErrorCode(ErrorCode& code, uint32_t val, bool alternative) = 0;
		/*
		 * Decode a t89 error code
		 * @param code input
		 * @return decoded value or 0 if error
		 */
		virtual uint32_t DecodeErrorCode(const ErrorCode& code) = 0;
		/*
		 * Convert error code to string
		 * @param code error code
		 * @return string
		 */
		virtual const char* ErrorCodeToStr(const ErrorCode& code) = 0;
		/*
		 * Alloc string hash memory, it is used to store the hashes
		 * @param len size
		 * @return allocated memory
		 */
		virtual void* AllocHashMemory(size_t len) = 0;
		/*
		 * Clone a string
		 * @param str
		 * @return copy str
		 */
		virtual const char* CloneHashStr(const char* str) = 0;
		/*
		 * Clean the storage
		 */
		virtual void Clean() = 0;
		/*
		 * Add a hash to the store
		 * @param value hash value
		 * @param str unhashed value
		 * @param clone clone the unhashed value
		 * @return allocated string or str if clone = false
		 */
		virtual const char* AddPrecomputed(uint64_t value, const char* str, bool clone = true) = 0;
		/*
		 * Extract a string pointer
		 * @param hash value to lookup
		 * @return unhashed value or nullptr if not in the store
		 */
		virtual const char* ExtractPtr(uint64_t hash) = 0;
		/*
		 * Extract a string representation
		 * @param type type of the hash
		 * @param hash hash value
		 * @param out out buffer
		 * @param outSize out buffer size
		 * @return if the value was in the store
		 */
		virtual bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) = 0;
		/*
		 * Extract a string representation
		 * @param type type of the hash
		 * @param hash hash value
		 * @return hash representation
		 */
		virtual char* ExtractTmp(const char* type, uint64_t hash) = 0;
		/*
		 * Read the default hash files
		 */
		virtual void ReadDefaultHashFiles() = 0;
	};

}

// @return hash api
ACTS_COMMON_API acts::api::ActsAPIHash& ActsAPIHash();