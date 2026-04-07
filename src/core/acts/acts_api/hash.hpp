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
	};

}

// @return hash api
ACTS_COMMON_API acts::api::ActsAPIHash& ActsAPIHash();