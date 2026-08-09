#include <includes.hpp>
#include <tools/gsc/obfuscator/gsc_obfuscator_private_file.hpp>
#include <rapidcsv.h>
#include <utils/data_utils.hpp>

namespace tool::gsc::obfuscator::private_file {

    constexpr const char PRIVATE_FILE_UID_DICT[] = "abcdefghijklmnopqrstuvwxyz0123456789_.";
    constexpr size_t PRIVATE_FILE_UID_DICT_LEN = sizeof(PRIVATE_FILE_UID_DICT) - 1;

    PrivateFile::PrivateFile() {}

    void PrivateFile::EncodeVal(size_t val, char* buff, size_t buffSize) {
        if (!buffSize) {
            throw std::runtime_error("PrivateDigit invalid buff");
        }

        val = EncodeNumber(val);
        while (val) {
            if (buffSize == 1) {
                break;
            }
            *(buff++) = PRIVATE_FILE_UID_DICT[val % PRIVATE_FILE_UID_DICT_LEN];
            val /= PRIVATE_FILE_UID_DICT_LEN;
            buffSize--;
        }
        *buff = 0;
    }

    const char* PrivateFile::EncodeValTmp(size_t val) {
        thread_local size_t idx{};
        thread_local char tmp[16][16]{};
        auto& v{ tmp[(idx + 1) % ACTS_ARRAYSIZE(tmp)] };
        EncodeVal(val, v, sizeof(v));
        return v;
    }

    void PrivateFile::EncodeBuffer(char* k, size_t start, size_t end, size_t val) {
        uint64_t rnd{ hash::Hash64A(EncodeValTmp(val)) };
        for (size_t i = start; i < end;) {
            uint64_t r{ rnd };

            k[i] = PRIVATE_FILE_UID_DICT[r % PRIVATE_FILE_UID_DICT_LEN];
            r /= PRIVATE_FILE_UID_DICT_LEN;
            i++;

            while (r && i < end) {
                k[i] = PRIVATE_FILE_UID_DICT[r % PRIVATE_FILE_UID_DICT_LEN];
                r /= PRIVATE_FILE_UID_DICT_LEN;
                i++;
            }
            rnd = hash::Hash64A(EncodeValTmp(val), rnd);
        }
    }

    static char* CleanPath(char* path) {
        // replace '/' -> '\' in script
        for (char* script = path; *script; script++) {
            if (*script == '/') {
                *script = '\\';
            }
        }
        return path;
    }

    bool PrivateFile::ReadFile(const char* file) {
        rapidcsv::Document doc{ file, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(',') };

        uidVal = utils::data::Rand(UINT32_MAX);
        LOG_DEBUG("using private file UID 0x{:x}", uidVal);

        if (doc.GetColumnCount() != 2) {
            LOG_ERROR("Invalid private file: should a two column CSV!");
            return false;
        }

        for (size_t i = 0; i < doc.GetRowCount(); i++) {
            std::string type{ doc.GetCell<std::string>(0, i) };
            std::string val{ doc.GetCell<std::string>(1, i) };

            if (type == "string") {
                std::string str{ EncodeValTmp(i) };
                if (str.size() > val.size()) {
                    LOG_WARNING("String '{}' is too small", val);
                    continue;
                }

                LOG_TRACE("add str '{}'->'{}'", val, str);
                this->strings[val] = std::move(str);
                continue;
            }

            if (type == "hash") {
                uint32_t hash{ EncodeNumber<uint32_t>(i) };
                LOG_TRACE("add hash '{}'->'hash_{:x}'", val, hash);
                this->hashes[val] = hash;
                continue;
            }

            if (type == "script") {
                if (!(val.ends_with(".gsc") || val.ends_with(".gsh") || val.ends_with(".csc"))) {
                    LOG_WARNING("Missing or invalid script extension for {}", val);
                    continue;
                }
                std::string str{ std::format("{}{}", EncodeValTmp(i), val.substr(val.size() - 4, 4)) };

                if (str.size() > val.size()) {
                    LOG_WARNING("Script '{}' is too small", val);
                    continue;
                }

                if (str.size() < val.size()) {
                    // add padding, it is required because we don't want to resize the asset name string in the xblock.
                    // for example
                    // in .. scripts/zm/zm_test_utils.gsc
                    // out . PFUID/123xxxxxxxxxxxxxxx.gsc
                    // same size, but one is obfuscated, maybe we should find how to not have the same size
                    // we can't use the gsc file to hide that because it is aligned
                    size_t start{ str.size() - 4 };
                    str.resize(val.size());
                    char* p{ str.data() };
                    // copy extension
                    std::memmove(&p[str.size() - 4], &p[start], 4);
                    EncodeBuffer(p, 0, str.size() - 4, i);
                }
                CleanPath(val.data());
                LOG_TRACE("add scr '{}'->'{}'", val, str);
                this->scripts[val] = std::move(str);
                continue;
            }

            LOG_WARNING("Invalid private type '{}' for key '{}'", type, val);
        }

        return true;
    }
    void PrivateFile::RenamedString(char* str) {
        auto it{ strings.find(str) };
        if (it == strings.end()) {
            return; // not known
        }

        // replace the value
        std::memcpy(str, it->second.data(), it->second.size() + 1);
    }
    void PrivateFile::RenamedScript(char* str) {
        auto it{ scripts.find(str) };
        if (it == scripts.end()) {
            return; // not known
        }

        // replace the value
        std::memcpy(str, it->second.data(), it->second.size() + 1);
    }

    void PrivateFile::RenamedScriptExt(char* str, bool client) {
        std::string k{ std::format("{}{}", str, client ? ".csc" : ".gsc") };
        CleanPath(k.data());
        auto it{ scripts.find(k) };
        if (it == scripts.end()) {
            return; // not known
        }

        // replace the value
        size_t noExtSize{ it->second.size() - 4 };
        std::memcpy(str, it->second.data(), noExtSize);
        str[noExtSize] = 0;
    }
} // namespace tool::gsc::obfuscator::private_file