#pragma once

namespace tool::gsc {
    struct T8GSCString {
        uint32_t string;
        uint8_t num_address;
        uint8_t type;
        uint16_t pad;
    };

    enum T8GSCExportFlags : uint8_t {
        LINKED = 0x01,
        AUTOEXEC = 0x02,
        PRIVATE = 0x04,
        CLASS_MEMBER = 0x08,
        CLASS_DESTRUCTOR = 0x10,
        VE = 0x20,
        EVENT = 0x40,
        CLASS_LINKED = 0x80,
        CLASS_VTABLE = 0x86
    };

    enum T8GSCImportFlags : uint8_t {
        FUNC_METHOD = 0x1,
        FUNCTION = 0x2,
        FUNCTION_THREAD = 0x3,
        FUNCTION_CHILDTHREAD = 0x4,
        METHOD = 0x5,
        METHOD_THREAD = 0x6,
        METHOD_CHILDTHREAD = 0x7,
        CALLTYPE_MASK = 0xF,
        DEV_CALL = 0x10,
        GET_CALL = 0x20,
        LOCAL_CALL = 0x40
    };
    
    struct T831GSCOBJ {
        byte magic[8];
        uint32_t source_crc;
        uint32_t include_offset;
        uint32_t animtree_offset;
        uint32_t cseg_offset;
        uint32_t stringtablefixup_offset;
        uint32_t devblock_stringtablefixup_offset;
        uint32_t exports_offset;
        uint32_t imports_offset;
        uint32_t fixup_offset;
        uint32_t globalvar_offset;
        uint32_t profile_offset;
        uint32_t cseg_size;
        uint32_t name;
        uint16_t stringtablefixup_count;
        uint16_t exports_count;
        uint16_t imports_count;
        uint16_t fixup_count;
        uint16_t globalvar_count;
        uint16_t profile_count;
        uint16_t devblock_stringtablefixup_count;
        uint8_t include_count;
        uint8_t animtree_count;
        uint8_t flags;

        inline const char* GetName() const {
            return reinterpret_cast<const char*>(&magic[name]);
        }
    };

    struct T8GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t cseg_offset;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t devblock_string_offset;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t requires_implements_offset;
        int16_t ukn50;
        int16_t devblock_string_count;
        int32_t cseg_size;
        uint16_t include_count;
        byte flags;
        byte requires_implements_count;
    };
    
    struct T8GSCFixup {
        uintptr_t offset;
        uintptr_t address;
    };

    struct T8GSCImport {
        uint32_t name;
        uint32_t import_namespace;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct T8GSCGlobalVar {
        uint32_t name;
        uint32_t num_address;
    };

    struct T8GSCExport {
        uint32_t checksum;
        uint32_t address;
        uint32_t name;
        uint32_t name_space;
        uint32_t callback_event;
        uint8_t param_count;
        uint8_t flags;
        uint16_t padding;
    };
}