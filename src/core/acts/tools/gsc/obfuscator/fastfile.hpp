#pragma once
#include <tools/fastfile/fastfile_data_tre.hpp>

namespace tool::gsc::obfuscator::fastfile {
    struct FastfileInfo {
        size_t maxBlockSize;
        ::fastfile::XFileBO3 header;
        std::vector<byte> out;
    };

    FastfileInfo DecompressFastfile(void* file, size_t fileSize);
    void CompressFastfile(std::vector<byte>& out, FastfileInfo& info);
} // namespace tool::gsc::obfuscator::fastfile