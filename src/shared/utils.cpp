#include <includes_shared.hpp>

LPCCH utils::va(LPCCH fmt, ...) {
    static CHAR buffer[0x10][0x50];
    static size_t bufferIndex = 0;
    bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
    auto& buff = buffer[bufferIndex];

    va_list va{};
    va_start(va, fmt);
    vsprintf_s(buff, fmt, va);
    va_end(va);

    return buff;
}

bool utils::ReadFileNotAlign(const std::filesystem::path& path, LPVOID& buffer, size_t& size, bool nullTerminate) {
    std::ifstream in{ path, std::ios::binary };
    if (!in) {
        return false;
    }

    in.seekg(0, std::ios::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios::beg);

    
    if (buffer) {
        LPVOID all = std::realloc(buffer, length + (nullTerminate ? 1 : 0));
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

bool utils::ReadFileAlign(const std::filesystem::path& path, LPVOID& buffer, LPVOID& bufferAligned, size_t& size, size_t& sizeAligned) {
    std::ifstream in{ path, std::ios::binary };
    if (!in) {
        return false;
    }

    in.seekg(0, std::ios::end);
    size_t length = in.tellg();
    in.seekg(0, std::ios::beg);


    if (buffer) {
        LPVOID all = std::realloc(buffer, length + 15);
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
    bufferAligned = reinterpret_cast<LPVOID>((reinterpret_cast<uintptr_t>(buffer) + 0xF) & ~0xF);
    in.read(reinterpret_cast<char*>(bufferAligned), length);
    sizeAligned = length;
    size = length + 15;

    in.close();
    return true;
}

bool utils::WriteFile(const std::filesystem::path& path, LPCVOID ptr, size_t size) {
    std::ofstream out{ path, std::ios::binary };
    if (!out) {
        return false;
    }

    out.write(reinterpret_cast<const char*>(ptr), size);

    out.close();
    return true;
}

std::ostream& utils::Padding(std::ostream& out, int padding) {
    for (size_t i = 0; i < padding; i++) {
        out << "    ";
    }
    return out;
}

UINT64 utils::CatLocated(UINT32 name_space, UINT32 local) {
    return ((UINT64)local << 32) | name_space;
}

std::pair<UINT32, UINT32> utils::UnCatLocated(UINT64 located) {
    UINT32 name_space = (located << 32) >> 32;
    UINT32 local = located >> 32;
    return std::make_pair<>(name_space, local);
}

UINT32 utils::CatLocated32(UINT16 a, UINT16 b) {
    return ((UINT32)b << 16) | a;
}

std::pair<UINT16, UINT16> utils::UnCatLocated32(UINT32 located) {
    UINT16 a = (located << 16) >> 16;
    UINT16 b = located >> 16;
    return std::make_pair<>(a, b);
}

UINT16 utils::CatLocated16(BYTE a, BYTE b) {
    return ((UINT16)b << 8) | a;
}

std::pair<BYTE, BYTE> utils::UnCatLocated16(UINT16 located) {
    BYTE a = (located << 8) >> 8;
    BYTE b = located >> 8;
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

void utils::GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate) {
    if (!std::filesystem::exists(parent)) {
        return;
    }
    GetFileRecurse0(parent, files, predicate);
}


void utils::GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files) {
    GetFileRecurse(parent, files, [](const auto& ref) { return true; });
}

// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
std::string utils::WStrToStr(const std::wstring& wstr) {
    if (wstr.empty()) {
        return {};
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utils::StrToWStr(const std::string& str) {
    if (str.empty()) {
        return {};
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}