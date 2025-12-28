#pragma once
#include <tools/gsc/vm/vm_t8.hpp>

namespace tool::gsc {
    enum T9GSCExportFlags : uint8_t {
        T9_EF_AUTOEXEC = 0x01,
        T9_EF_LINKED = 0x02,
        T9_EF_PRIVATE = 0x04,
        T9_EF_CLASS_MEMBER = 0x08,
        T9_EF_CLASS_DESTRUCTOR = 0x10,
        T9_EF_EVENT = 0x20,
        T9_EF_VE = 0x40,
        T9_EF_CLASS_LINKED = 0x80,
        T9_EF_CLASS_VTABLE = 0x15
    };

    enum T9GSCImportFlags : uint8_t {
        T9_IF_METHOD_CHILDTHREAD = 0x1,
        T9_IF_METHOD_THREAD = 0x2,
        T9_IF_FUNCTION_CHILDTHREAD = 0x3,
        T9_IF_FUNCTION = 0x4,
        T9_IF_FUNC_METHOD = 0x5,
        T9_IF_FUNCTION_THREAD = 0x6,
        T9_IF_METHOD = 0x7,
        T9_IF_CALLTYPE_MASK = 0xF,
        T9_IF_GET_CALL = 0x10,
        T9_IF_DEV_CALL = 0x20,
    };

    struct T937GSCOBJ {
        byte magic[8];
        uint32_t crc;
        uint32_t pad0c;
        uint64_t name;
        uint32_t includes_table;
        uint16_t string_count;
        uint16_t export_count;
        uint32_t cseg_offset;
        uint32_t string_offset;
        uint16_t imports_count;
        uint16_t fixup_count;
        uint32_t devblock_string_offset;
        uint32_t exports_tables;
        uint32_t imports_offset;
        uint16_t globalvar_count;
        uint16_t unk3a;
        uint32_t fixup_offset;
        uint32_t globalvar_offset;
        uint32_t file_size;
        uint16_t unk48;
        uint16_t devblock_string_count;
        uint32_t cseg_size;
        uint16_t includes_count;
        uint16_t unk52;
        uint32_t unk54;
    };

    struct T9GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad0c;
        uint64_t name;
        uint16_t string_count;
        uint16_t exports_count;
        uint16_t imports_count;
        uint16_t unk1e;
        uint16_t globalvar_count;
        uint16_t unk22;
        uint16_t includes_count;
        uint16_t devblock_string_count;
        uint32_t devblock_string_offset;
        uint32_t cseg_offset;
        uint32_t string_offset;
        uint32_t includes_table;
        uint32_t exports_tables;
        uint32_t import_tables;
        uint32_t unk40;
        uint32_t globalvar_offset;
        uint32_t file_size;
        uint32_t unk4c;
        uint32_t cseg_size;
        uint32_t unk54;
    };
}