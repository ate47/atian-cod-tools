#pragma once
#include <acts_api.hpp>

namespace acts::api {

	struct HashType {
		const char* id;
		const char* desc;
		uint64_t(*hashFunc)(const char* text);
	};

	class ActsAPIHash {
	public:
		virtual HashType* GetHashes() = 0;
		virtual size_t GetHashesCount() = 0;
	};

}

ACTS_COMMON_API acts::api::ActsAPIHash& ActsAPIHash();