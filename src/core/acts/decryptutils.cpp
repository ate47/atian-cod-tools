#include <includes.hpp>
#include <utils/decrypt.hpp>
#include <core/config.hpp>
#include <utils/decrypt_t8old.hpp>
#include <hook/module_mapper.hpp>
#include "decryptutils.hpp"

namespace acts::decryptutils {

    namespace {
        char* (*DecryptStringImpl)(char* str){};
        hook::module_mapper::Module decryptModule{ true };
        T8Decryption t8old{};

        core::config::Config& GetDecryptData() {
            static core::config::Config data{ [] {
                std::string scanpath{ core::config::GetString("data.dir", "") };
                std::filesystem::path file;
                if (scanpath.empty()) {
                    file = utils::GetProgDir() / "data" / "decrypt.json";
                } else {
                    file = std::filesystem::path{ scanpath } / "decrypt.json";
                }

                core::config::Config cfg{ file };
                cfg.SyncConfig(false);
                return cfg;
            }() };
            return data;
        }

        bool LoadDecryptModule0(
            std::function<std::vector<hook::library::ScanResult>(const char* path, const char* name)> ScanFunc
        ) {
            DecryptStringImpl = nullptr;

            core::config::Config& cfg{ GetDecryptData() };

            core::config::RapidJsonGeneric& scans{ cfg.GetVal("scans") };

            if (!scans.IsObject()) {
                LOG_ERROR("Invalid decrypt.json file");
                return false;
            }

            for (auto& [vid, vpattern] : scans.GetObj()) {
                const char* id{ vid.GetString() };
                const char* pattern{ vpattern.GetString() };

                std::vector<hook::library::ScanResult> res{ ScanFunc(pattern, id) };

                if (res.size() != 1) {
                    if (res.size() > 1) {
                        LOG_TRACE("Too many finds for scan {}", id);
                    }
                    continue;
                }

                DecryptStringImpl = res[0].GetPtr<char* (*)(char* str)>();
                if (res[0].entry) {
                    // rename it to DecryptString if we want to dump the scans
                    res[0].entry->name = "DecryptString";
                }
                LOG_TRACE("Loaded DecryptStringImpl={} ({})", hook::library::CodePointer{ DecryptStringImpl }, id);
                return true; // loaded
            }

            return false;
        }
    } // namespace

    char* DecryptString(char* str) {
        if (!DecryptStringImpl || !str || (*str & 0xC0) != 0x80) {
            return str;
        }
        return DecryptStringImpl(str);
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

    void SetT8OldDecryption(T8Decryption alg) { t8old = alg; }
    char* DecryptStringT8(char* str) {
        switch (t8old) {
        case T8D_X1_LATEST:
            return decrypt::DecryptString(str);
        case T8D_X3_20181013:
            return decrypt::DecryptStringT8Old(str);
        case T8D_X2_UNK_OLD: // the one in the ff
        default:
            return DecryptString(str); // fallback on the module one
        }
    }

    const char* GetT8OldDecryptionName(T8Decryption alg) {
        switch (alg) {
        case T8D_X1_LATEST:
            return "latest";
        case T8D_X2_UNK_OLD:
            return "unk_x2";
        case T8D_X3_20181013:
            return "x3_181013";
        default:
            return "unk_dec";
        }
    }

} // namespace acts::decryptutils
