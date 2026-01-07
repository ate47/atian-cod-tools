#pragma once

namespace compatibility::acti::crypto_keys {
	constexpr const char* DEFAULT_RSA_KEY_FILE = "keys.csv";
	constexpr const char* DEFAULT_AES_KEY_FILE = "keys_aes.csv";
	enum KeyVersion {
		VER_INVALID = 0,
		VER_BO3,
		VER_BO4,
	};
	enum KeyType {
		KEYT_INVALID = 0,
		KEYT_PRIVATE,
		KEYT_PUBLIC,
	};

	struct RsaKeyLocal {
		const char* id;
		size_t len;
		KeyType type;
		byte key[0x2000];
	};
	struct AesKeyLocal {
		const char* fileName;
		KeyVersion version;
		byte key[32];
	};

	std::vector<RsaKeyLocal*>& GetRsaKeys();
	std::vector<AesKeyLocal*>& GetKeys();
	KeyVersion GetKeyVersionByName(const char* version);
	KeyType GetKeyTypeByName(const char* version);
	const char* GetKeyVersionName(KeyVersion version);
	const char* GetKeyTypeName(KeyType type);
	AesKeyLocal* GetKeyByName(const char* filename, KeyVersion version);
	RsaKeyLocal* GetRSAKeyByName(const char* id);
	void LoadAesKeys(const std::filesystem::path& path);
	void LoadRsaKeys(const std::filesystem::path& path);
}
#define REGISTER_ACTI_KEY(name, version, ...) utils::ArrayAdder<AesKeyLocal> __##name##version{ GetKeys(), AesKeyLocal{#name, version, { __VA_ARGS__  } }}
#define REGISTER_ACTI_RSA_KEY(name, type, size, ...) utils::ArrayAdder<RsaKeyLocal> __##name{ GetRsaKeys(), RsaKeyLocal{#name, size, type, { __VA_ARGS__  } }}
