#include <includes_shared.hpp>
#include "utils.hpp"

namespace utils {
    char* va(const char* fmt, ...) {
        static char buffer[0x10][0x500];
        static size_t bufferIndex = 0;
        bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
        auto& buff = buffer[bufferIndex];

        va_list va{};
        va_start(va, fmt);
        vsprintf_s(buff, fmt, va);
        va_end(va);

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

    static void GetFileRecurse0(const std::filesystem::path& dir, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate) {
        if (std::filesystem::is_directory(dir)) {
            for (const auto& sub : std::filesystem::directory_iterator{ dir }) {
                GetFileRecurse0(sub, files, predicate);
            }
            return;
        }
        if (predicate(dir)) {
            files.emplace_back(dir);
        }
    }

    void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate) {
        if (!std::filesystem::exists(parent)) {
            return;
        }
        GetFileRecurse0(parent, files, predicate);
    }


    void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files) {
        GetFileRecurse(parent, files, [](const auto& ref) { return true; });
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
        GetModuleFileName(NULL, szFileName, MAX_PATH);
        return std::filesystem::absolute(szFileName).parent_path();
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
}