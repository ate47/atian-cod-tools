#include <includes.hpp>
#include <utils/decrypt.hpp>
#include <utils/decrypt_t8old.hpp>
#include <hook/module_mapper.hpp>
#include "decryptutils.hpp"

namespace acts::decryptutils {

	namespace {
		char* (*DecryptStringImpl)(char* str) {};
		hook::module_mapper::Module decryptModule{ true };
		T8Decryption t8old{};
	}

	char* DecryptString(char* str) {
		if (!DecryptStringImpl || !str || (*str & 0xC0) != 0x80) {
			return str;
		}

		return DecryptStringImpl(str);
	}
	bool LoadDecrypt(const std::filesystem::path& exec) {
		decryptModule.Free();
		DecryptStringImpl = nullptr;

		struct {
			const char* pattern;
			const char* id;
		} knownScans[]{
			{ "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 0F B6 01", "iw" },
			{ "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6", "cw" },
			{ "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B6 01", "mw19" },
		};

		if (!decryptModule.Load(exec)) {
			LOG_ERROR("Can't load decryption module {}", exec.string());
			return false;
		}

		for (auto& cfg : knownScans) {
			std::vector<hook::library::ScanResult> res = decryptModule->Scan(cfg.pattern);

			if (res.size() != 1) {
				if (res.size() > 1) {
					LOG_TRACE("Too many finds for scan {}", cfg.id);
				}
				continue;
			}

			DecryptStringImpl = res[0].GetPtr<char*(*)(char* str)>();
			LOG_TRACE("Loaded DecryptStringImpl=0x{:x} ({})", decryptModule->Rloc(DecryptStringImpl), cfg.id);
			return true; // loaded
		}
		
		decryptModule.Free();
		LOG_ERROR("Can't load decryption DecryptString in {}", exec.string());
		return false;
	}


	void SetT8OldDecryption(T8Decryption alg) {
		t8old = alg;
	}
	char* DecryptStringT8(char* str) {
		switch (t8old) {
		case T8D_X1_LATEST: return decrypt::DecryptString(str);
		case T8D_X3_20181013: return decrypt::DecryptStringT8Old(str);
		case T8D_X2_UNK_OLD: // the one in the ff
		default: return DecryptString(str); // fallback on the module one
		}
	}

	const char* GetT8OldDecryptionName(T8Decryption alg) {
		switch (alg) {
			case T8D_X1_LATEST: return "latest";
			case T8D_X2_UNK_OLD: return "unk_x2";
			case T8D_X3_20181013: return "x3_181013";
			default: return "unk_dec";
		}
	}

}