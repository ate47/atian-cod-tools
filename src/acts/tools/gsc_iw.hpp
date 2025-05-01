#pragma once
#include <tools/gsc_opcodes.hpp>

namespace tool::gsc::iw {


    struct BINGSCExport {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint64_t checksum;
        uint32_t address;
        uint32_t size;
        uint8_t param_count;
        uint8_t flags;
    };

    const char* GetOpaqueStringForVm(uint64_t vm, uint32_t str, bool useDefault = true);
    const char* GetMethodForVm(uint64_t vm, uint32_t str, bool useDefault = true);
    const char* GetFunctionForVm(uint64_t vm, uint32_t str, bool useDefault = true);
}