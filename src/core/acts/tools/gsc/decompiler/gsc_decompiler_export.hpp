#pragma once
#include <tools/gsc/gsc_opcodes.hpp>

namespace tool::gsc {
    struct GSCExportReader {
        virtual uint64_t GetName() = 0;
        virtual uint64_t GetNamespace() = 0;
        virtual uint64_t GetFileNamespace() = 0;
        virtual uint64_t GetChecksum() = 0;
        virtual uint32_t GetAddress() = 0;
        virtual uint8_t GetParamCount() = 0;
        virtual uint8_t GetFlags() = 0;
        virtual size_t SizeOf() = 0;
        virtual uint32_t GetSize() { return 0; };
        virtual void SetHandle(void* handle) = 0;
        virtual void SetAddress(uint32_t addr) = 0;
        virtual void SetName(uint64_t name) = 0;
        virtual void SetNamespace(uint64_t nameSpace) = 0;
        virtual void SetFileNamespace(uint64_t fileNamespace) = 0;
        virtual void SetChecksum(uint64_t checksum) = 0;
        virtual void SetFlags(uint8_t flags) = 0;
        virtual void SetParamCount(uint8_t flags) = 0;
    };

    std::unique_ptr<GSCExportReader> CreateExportReader(tool::gsc::opcode::VmInfo* vmInfo);
} // namespace tool::gsc