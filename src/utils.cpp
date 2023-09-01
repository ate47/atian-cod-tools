#include "includes.hpp"

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
