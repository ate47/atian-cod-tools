#pragma once
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/data/gsc_data_jup.hpp>

namespace tool::gsc {
    struct GSCImportReader {
        virtual uint64_t GetName() = 0;
        virtual uint64_t GetNamespace() = 0;
        virtual uint8_t GetFlags() = 0;
        virtual size_t SizeOf() = 0;
        virtual uint8_t GetParamCount() = 0;
        virtual uint16_t GetNumAddresses() = 0;
        virtual uint32_t* GetAddresses() = 0;
        virtual void SetHandle(void* handle) = 0;

        virtual void SetName(uint64_t name) = 0;
        virtual void SetNamespace(uint64_t nameSpace) = 0;
        virtual void SetFlags(uint8_t flags) = 0;
        virtual void SetParamCount(uint8_t paramCount) = 0;
        virtual void ReadGeneric(IW23GSCImport& imp) = 0;
    };

    std::unique_ptr<GSCImportReader> CreateImportReader(tool::gsc::opcode::VmInfo* vmInfo);
} // namespace tool::gsc