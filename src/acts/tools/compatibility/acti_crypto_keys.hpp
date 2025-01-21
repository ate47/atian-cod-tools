#pragma once

namespace compatibility::acti::crypto_keys {
	struct AesKeyLocal {
		const char* fileName;
		uint32_t version;
		byte key[32];
	};

	std::vector<AesKeyLocal*>& GetKeys();
	AesKeyLocal* GetKeyByName(const char* filename);
}
#define REGISTER_ACTI_KEY(name, version, ...) utils::ArrayAdder<AesKeyLocal> __##name{ GetKeys(), AesKeyLocal{#name, version, { __VA_ARGS__  } }}