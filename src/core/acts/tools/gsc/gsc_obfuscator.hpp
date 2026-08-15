#pragma once
#include <tools/gsc/obfuscator/gsc_obfuscator_private_file.hpp>
#include <tools/gsc/gsc_opcodes_load.hpp>

namespace tool::gsc::obfuscator::options {
    struct GscObfOptions {
        tool::gsc::obfuscator::private_file::PrivateFile privateFileData{};
        bool printHelp{};
        bool printData{};
        bool noRemoveLocals{};
        bool noRemovePrivateExports{};
        bool noDebugKill{};
        bool noTrampoline{};
        bool noDevOpCodeReplace{};
        bool recomputeCRC{};
        bool dumpDecompiledScripts{};
        const char* logLevel{};
        const char* privateFile{};
        const char* output{ "output_ff/obf" };
        const char* fastfileBuilder{};
        const char* fastfileCompression{};
        const char* pltName{};
        opcode::Platform plt{ opcode::PLATFORM_PC };
    };
} // namespace tool::gsc::obfuscator::options