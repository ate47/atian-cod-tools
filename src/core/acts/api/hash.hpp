#pragma once
#include <acts_api.hpp>

namespace acts::api {

	struct HashType {
		const char* id;
		const char* desc;
		uint64_t(*hashFunc)(const char* text);
	};

	typedef const char* ErrorCode[4];

	class ActsAPIHash {
	public:
		virtual HashType* GetHashes() = 0;
		virtual size_t GetHashesCount() = 0;
		virtual void EncodeErrorCode(ErrorCode& code, uint32_t val, bool alternative) = 0;
		virtual uint32_t DecodeErrorCode(const ErrorCode& code) = 0;
		virtual const char* ErrorCodeToStr(const ErrorCode& code) = 0;
	};

}

ACTS_COMMON_API acts::api::ActsAPIHash& ActsAPIHash();