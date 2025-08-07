#include <includes_shared.hpp>
#include <utils/utils.hpp>

namespace utils {
    Timestamp GetTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::ostream& NullStream() {
        static thread_local std::ofstream nullstream{ ([] {
            std::ofstream s;
            s.setstate(std::ios_base::badbit);
            return s;
        })() };
        return nullstream;
    }

    char* va(const char* fmt, ...) {
        va_list va{};
        va_start(va, fmt);
        char* buff = vap(fmt, va);
        va_end(va);

        return buff;
    }

    char* vap(const char* fmt, va_list ap) {
        static char buffer[0x10][0x500];
        static size_t bufferIndex = 0;
        bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
        auto& buff = buffer[bufferIndex];

        vsprintf_s(buff, fmt, ap);

        return buff;
    }

    bool ReadFile(const std::filesystem::path& path, std::string& buffer, bool append) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);
        size_t offset;
        if (append) {
            offset = buffer.size();
            buffer.resize(length + buffer.size());
        }
        else {
            offset = 0;
            buffer.resize(length);
        }

        in.read(buffer.data() + offset, length);

        in.close();
        return true;
    }

    void* ReadFilePtr(const std::filesystem::path& path, size_t* len, std::function<void* (size_t)> alloc) {
        static byte emptyBuff[]{ 0 };
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return nullptr;
        }

        in.seekg(0, std::ios::end);
        size_t length = (size_t)in.tellg();
        if (len) *len = length;
        in.seekg(0, std::ios::beg);

        if (!length) {
            in.close();
            return emptyBuff;
        }

        char* ptr{ (char*)alloc(length + 1) };

        in.read(ptr, length);
        ptr[length] = 0;

        in.close();
        return ptr;
    }

    bool ReadFile(const std::filesystem::path& path, std::vector<byte>& buffer, bool append) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);
        size_t offset;
        if (append) {
            offset = buffer.size();
            buffer.resize(length + buffer.size());
        }
        else {
            offset = 0;
            buffer.resize(length);
        }

        in.read((char*)buffer.data() + offset, length);

        in.close();
        return true;
    }

    bool ReadFileAlign(const std::filesystem::path& path, std::string& buffer, void*& bufferAligned, size_t& sizeAligned) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        sizeAligned = in.tellg();
        in.seekg(0, std::ios::beg);
        buffer.resize(sizeAligned + 0xF);

        bufferAligned = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(buffer.data()) + 0xF) & ~0xF);

        in.read((char*)bufferAligned, sizeAligned);

        in.close();
        return true;
    }

    bool ReadFileAlign(const std::filesystem::path& path, std::vector<byte>& buffer, void*& bufferAligned, size_t& sizeAligned) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        sizeAligned = in.tellg();
        in.seekg(0, std::ios::beg);
        buffer.resize(sizeAligned + 0xF);

        bufferAligned = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(buffer.data()) + 0xF) & ~0xF);

        in.read((char*)bufferAligned, sizeAligned);

        in.close();
        return true;
    }

    bool ReadFileNotAlign(const std::filesystem::path& path, void*& buffer, size_t& size, bool nullTerminate) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);


        if (buffer) {
            void* all = std::realloc(buffer, length + (nullTerminate ? 1 : 0));
            if (!all) {
                std::free(buffer);
                buffer = NULL;
                return false; // error realloc, free old buffer
            }
        }
        else {
            buffer = std::malloc(length + (nullTerminate ? 1 : 0));
            if (!buffer) {
                return false; // error malloc
            }
        }

        auto* buff = reinterpret_cast<char*>(buffer);
        in.read(buff, length);
        if (nullTerminate) {
            buff[length] = 0;
        }
        size = length;

        in.close();
        return true;
    }

    bool ReadFileAlign(const std::filesystem::path& path, void*& buffer, void*& bufferAligned, size_t& size, size_t& sizeAligned) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);


        if (buffer) {
            void* all = std::realloc(buffer, length + 15);
            if (!all) {
                std::free(buffer);
                buffer = NULL;
                return false; // error realloc, free old buffer
            }
        }
        else {
            buffer = std::malloc(length + 15);
            if (!buffer) {
                return false; // error malloc
            }
        }

        // align address to end by 0
        bufferAligned = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(buffer) + 0xF) & ~0xF);
        in.read(reinterpret_cast<char*>(bufferAligned), length);
        sizeAligned = length;
        size = length + 15;

        in.close();
        return true;
    }

    bool WriteFile(const std::filesystem::path& path, const void* ptr, size_t size) {
        std::ofstream out{ path, std::ios::binary };
        if (!out) {
            return false;
        }

        out.write(reinterpret_cast<const char*>(ptr), size);

        out.close();
        return true;
    }

    size_t Allocate(std::vector<byte>& data, size_t len) {
        size_t begin = data.size();
        for (size_t i = 0; i < len; i++) {
            data.push_back(0);
        }
        return begin;
    }

    std::ostream& Padding(std::ostream& out, int padding) {
        for (size_t i = 0; i < padding; i++) {
            out << "    ";
        }
        return out;
    }

    uint64_t CatLocated(uint32_t name_space, uint32_t local) {
        return ((uint64_t)local << 32) | name_space;
    }

    std::pair<uint32_t, uint32_t> UnCatLocated(uint64_t located) {
        uint32_t name_space = (located << 32) >> 32;
        uint32_t local = located >> 32;
        return std::make_pair<>(name_space, local);
    }

    uint32_t CatLocated32(uint16_t a, uint16_t b) {
        return ((uint32_t)b << 16) | a;
    }

    std::pair<uint16_t, uint16_t> UnCatLocated32(uint32_t located) {
        uint16_t a = (located << 16) >> 16;
        uint16_t b = located >> 16;
        return std::make_pair<>(a, b);
    }

    uint16_t CatLocated16(byte a, byte b) {
        return ((uint16_t)b << 8) | a;
    }

    std::pair<byte, byte> UnCatLocated16(uint16_t located) {
        byte a = (located << 8) >> 8;
        byte b = located >> 8;
        return std::make_pair<>(a, b);
    }

    static void GetFileRecurse0(const std::filesystem::path& dir, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate, const std::filesystem::path& base, bool removeBase) {
        if (std::filesystem::is_directory(dir)) {
            for (const auto& sub : std::filesystem::directory_iterator{ dir }) {
                GetFileRecurse0(sub, files, predicate, base, removeBase);
            }
            return;
        }
        if (predicate(dir)) {
            if (removeBase) {
                files.emplace_back(std::filesystem::relative(dir, base));
            }
            else {
                files.emplace_back(dir);
            }
        }
    }

    void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate, bool removeParent) {
        if (!std::filesystem::exists(parent)) {
            return;
        }
        GetFileRecurse0(parent, files, predicate, parent, removeParent);
    }


    void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files) {
        GetFileRecurse(parent, files, [](const auto& ref) { return true; });
    }
    void GetFileRecurseExt(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files, const char* ends, bool removeParent) {
        GetFileRecurse(parent, files, [ends](const std::filesystem::path& p) -> bool {
            std::string s{ p.string() };

            for (const char* exts{ ends }; *exts; exts += std::strlen(exts) + 1) {
                if (s.ends_with(exts)) return true;
            }

            return false;
        }, removeParent);
    }

    // https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
    std::string WStrToStr(const std::wstring& wstr) {
        if (wstr.empty()) {
            return {};
        }
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    std::wstring StrToWStr(const std::string& str) {
        if (str.empty()) {
            return {};
        }
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::filesystem::path GetProgDir() {
        wchar_t szFileName[MAX_PATH];
        GetModuleFileNameW(NULL, szFileName, MAX_PATH);
        return std::filesystem::absolute(szFileName).parent_path();
    }
    int64_t ParseFormatInt(const char* number) {
        if (!number || !*number) {
            throw std::runtime_error(va("Invalid number %s", number ? number : "nullptr"));
        }

        if (number[0] == '0') {
            // 0x / ob / 0
            if (!number[1]) {
                return 0; // 0
            }

            if (number[1] == 'x') {
                return std::strtoll(number + 2, nullptr, 16);
            }
            if (number[1] == 'b') {
                return std::strtoll(number + 2, nullptr, 2);
            }

            return std::strtoll(number + 1, nullptr, 8);
        }

        return std::strtoll(number, nullptr, 10);
    }
    const char* FancyNumber(uint64_t number, bool si) {
        int base = si ? 1000 : 1024;

        if (number < base) {
            return utils::va("%lld", number);
        }

        static char suffixes[]{ 'K', 'M', 'G', 'T', 'P', 'E' };
        constexpr size_t suffixesSize = ARRAYSIZE(suffixes);
        int lg{};
        do {
            number = number / base;
            lg++;
        } while (number > base && lg < suffixesSize);

        return utils::va("%lld%c", number, suffixes[lg - 1]);
    }

    bool IsProbablyHex(int64_t number) {
        if (number < 0x800) {
            return false; // too small or negative
        }

        size_t frequencies[0x10]{};
        size_t log16{};

        // count the numbers and try to find one which has a 2/3 frequency
        // 0x100 would work in this logic
        while (number) {
            frequencies[number & 0xf]++;
            number >>= 4;
            log16++;
        }

        size_t okay{ log16 * 2 / 3 };

        for (size_t i = 0; i < 0x10; i++) {
            if (frequencies[i] >= okay) {
                return true;
            }
        }

        return false;
    }

    void SwapByte(void* ptr, size_t n) {
        byte* in{ (byte*)ptr };
        for (size_t lo = 0, hi = n - 1; hi > lo; ++lo, --hi) {
            in[lo] ^= in[hi], in[hi] ^= in[lo], in[lo] ^= in[hi];
        }
    }

    char* MapString(char* buffer, std::function<char(char)> map) {
        std::transform(buffer, buffer + strlen(buffer), buffer, map);
        return buffer;
    }

    char* UpperCase(char* buffer) {
        return MapString(buffer, [](char c) { return std::toupper(c); });
    }

    char* LowerCase(char* buffer) {
        return MapString(buffer, [](char c) { return std::tolower(c); });
    }

    int RemoveCase(int c) {
        if (c == '\\') return '/';
        return tolower(c);
    }

    bool EqualIgnoreCase(const char* s1, const char* s2) {
        while (*s1) {
            if (RemoveCase(*s1) != RemoveCase(*s2)) {
                return false;
            }
            s1++;
            s2++;
        }
        return !*s2;
    }

    bool IsSubDir(const std::filesystem::path& parent, const std::filesystem::path& child) {
        std::filesystem::path::iterator prit{ parent.begin() };
        std::filesystem::path::iterator chit{ child.begin() };

        while (prit != parent.end()) {
            if (chit == child.end()) return false; // too small
            if (*prit != *chit) return false; // not same
            prit++; 
            chit++;
        }

        return true;
    }

    std::ostream& PrintFormattedString(std::ostream& out, const char* str, size_t len) {
        if (!str) {
            return out << "nullptr";
        }

        for (size_t i = 0; (!len && *str) || (len && i < len); str++, i++) {
            switch (*str) {
            case '\n':
                out << "\\n";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\t':
                out << "\\t";
                break;
            case '\a':
                out << "\\a";
                break;
            case '\b':
                out << "\\b";
                break;
            case '\v':
                out << "\\v";
                break;
            case '"':
                out << "\\\"";
                break;
            default:
                if (*str < 0x20 || *str >= 0x7F) {
                    out << "\\x" << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(*reinterpret_cast<const byte*>(str)) << std::dec;
                }
                else {
                    out << *str;
                }
                break;
            }
        }
        return out;
    }

    std::string FormatString(const char* str) {
        std::ostringstream out;

        PrintFormattedString(out, str);

        return out.str();
    }

    int LineStreamBuf::sync() {
        std::string_view s{ view() };
        // load the lines
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] == '\n') line++;
        }

        in.write(s.data(), s.length());

        str({});

        return 0;
    }
}
std::ostream& operator<<(std::ostream& stream, const utils::FormattedString& fs) {
    return utils::PrintFormattedString(stream, fs.str, fs.len);
}