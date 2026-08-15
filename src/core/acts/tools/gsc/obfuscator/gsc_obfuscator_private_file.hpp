#pragma once

namespace tool::gsc::obfuscator::private_file {

    class PrivateFile {
        // private file uid, used to differentiate the generated strings
        size_t uidVal{};
        std::unordered_map<std::string, std::string> scripts{};
        std::unordered_map<std::string, std::string> strings{};
        std::unordered_map<std::string, uint32_t> hashes{};

      public:
        PrivateFile();
        void EncodeBuffer(char* k, size_t start, size_t end, size_t val);
        void EncodeVal(size_t val, char* buff, size_t buffSize);
        const char* EncodeValTmp(size_t val);
        template<typename N>
        N EncodeNumber(N val) {
            return val ^ uidVal;
        }
        template<typename N>
        N EncodeValNumber(size_t in) {
            uint64_t v{ hash::Hash64A(EncodeValTmp(in)) };
            if constexpr (sizeof(N) <= 4) {
                v = v ^ (v >> 32);
            }
            if constexpr (sizeof(N) <= 2) {
                v = v ^ (v >> 16);
            }
            if constexpr (sizeof(N) == 1) {
                v = v ^ (v >> 8);
            }
            return (N)v;
        }

        constexpr const std::unordered_map<std::string, std::string>& GetScripts() const { return scripts; }
        constexpr const std::unordered_map<std::string, std::string>& GetStrings() const { return strings; }
        constexpr const std::unordered_map<std::string, uint32_t>& GetHashes() const { return hashes; }

        bool ReadFile(const char* file);
        bool RenamedString(char* str);
        bool RenamedScript(char* str);
        bool RenamedScriptExt(char* str, bool client);
        bool RenamedScriptHashed(uint64_t& hash);
    };
} // namespace tool::gsc::obfuscator::private_file