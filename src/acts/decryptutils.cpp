#include <includes.hpp>
#include <utils/decrypt.hpp>
#include <hook/module_mapper.hpp>
#include "decryptutils.hpp"

namespace acts::decryptutils {

	namespace {
		char* (*DecryptStringImpl)(char* str) {};
		hook::module_mapper::Module decryptModule{ true };
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


}