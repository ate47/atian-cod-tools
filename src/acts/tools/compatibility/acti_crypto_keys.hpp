#pragma once

namespace compatibility::acti::crypto_keys {
	constexpr const char* DEFAULT_KEY_FILE = "keys.csv";
	enum KeyVersion {
		VER_INVALID = 0,
		VER_BO3,
		VER_BO4,
	};

	struct RsaKeyLocal {
		const char* id;
		byte key[270];
	};
	struct AesKeyLocal {
		const char* fileName;
		KeyVersion version;
		byte key[32];
	};

	std::vector<RsaKeyLocal*>& GetRsaKeys();
	std::vector<AesKeyLocal*>& GetKeys();
	KeyVersion GetKeyVersionByName(const char* version);
	AesKeyLocal* GetKeyByName(const char* filename, KeyVersion version);
	RsaKeyLocal* GetRSAKeyByName(const char* id);
	void LoadKeys(const std::filesystem::path& path);
}
#define REGISTER_ACTI_KEY(name, version, ...) utils::ArrayAdder<AesKeyLocal> __##name##version{ GetKeys(), AesKeyLocal{#name, version, { __VA_ARGS__  } }}
#define REGISTER_ACTI_RSA_KEY(name, ...) utils::ArrayAdder<RsaKeyLocal> __##name{ GetRsaKeys(), RsaKeyLocal{#name, { __VA_ARGS__  } }}
