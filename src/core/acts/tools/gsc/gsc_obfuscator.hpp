#pragma once
#include <tools/gsc/obfuscator/gsc_obfuscator_private_file.hpp>

namespace tool::gsc::obfuscator::options {
    struct GscObfOptions {
        tool::gsc::obfuscator::private_file::PrivateFile privateFileData{};
        bool printHelp{};
        bool printData{};
        bool noRemoveLocals{};
        bool noRemovePrivateExports{};
        bool noDebugKill{};
        bool noTrampoline{};
        bool recomputeCRC{};
        size_t seed{};
        const char* logLevel{};
        const char* privateFile{};
        const char* output{ "output_ff/obf" };
        const char* fastfileBuilder{};
        const char* fastfileCompression{};
    };
} // namespace tool::gsc::obfuscator::options