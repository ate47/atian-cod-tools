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

	static bool LoadDecryptModule0(std::function<std::vector<hook::library::ScanResult>(const char* path, const char* name)> ScanFunc) {
		DecryptStringImpl = nullptr;

		struct {
			const char* pattern;
			const char* id;
		} knownScans[]{
			{ "40 53 48 83 EC ?? 48 8B D9 0F B6 01 24 ?? 3C ?? 0F 85 88 04 00 00 48 8D 0D ?? ?? ?? ?? 48 89 4C 24 ?? E8 ?? ?? ?? ?? 90 0F BE 03 83 E8 80 83 F8", "cod2020" },
			{ "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 0F B6 01", "iw" },
			{ "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B6 01 4C 8B F9", "bo6" },
			{ "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6 01 24 ? 3C", "cw" },
			{ "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 4C 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F", "bo6_2" },
			{ "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 4C 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B6", "bo7" },
			{ "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B6 01", "mw19" },
			{ "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B6 11", "vg" },
		};

		for (auto& cfg : knownScans) {
			std::vector<hook::library::ScanResult> res{ ScanFunc(cfg.pattern, cfg.id) };

			if (res.size() != 1) {
				if (res.size() > 1) {
					LOG_TRACE("Too many finds for scan {}", cfg.id);
				}
				continue;
			}

			DecryptStringImpl = res[0].GetPtr<char* (*)(char* str)>();
			if (res[0].entry) {
				// rename it to DecryptString if we want to dump the scans
				res[0].entry->name = "DecryptString";
			}
			LOG_TRACE("Loaded DecryptStringImpl={} ({})", hook::library::CodePointer{ DecryptStringImpl }, cfg.id);
			return true; // loaded
		}

		return false;
	}

	bool LoadDecryptModule(hook::module_mapper::Module& mod) {
		hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
		return LoadDecryptModule0([&scan](const char* path, const char* name) { return scan.Scan(path, name); });
	}

	bool LoadDecryptModule(const hook::library::Library& lib) {
		return LoadDecryptModule0([&lib](const char* path, const char* name) { return lib.Scan(path, false, name); });
	}

	bool LoadDecrypt(const std::filesystem::path& exec) {
		decryptModule.Free();

		if (!decryptModule.Load(exec)) {
			LOG_ERROR("Can't load decryption module {}", exec.string());
			return false;
		}

		if (LoadDecryptModule(decryptModule)) {
			return true;
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