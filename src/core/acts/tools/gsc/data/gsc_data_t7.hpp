#pragma once

namespace tool::gsc {
    struct T7GSCOBJ {
        byte magic[8];
        uint32_t source_crc;
        uint32_t include_offset;
        uint32_t animtree_offset;
        uint32_t cseg_offset;
        uint32_t string_offset;
        uint32_t devblock_string_offset;
        uint32_t export_offset;
        uint32_t import_offset;
        uint32_t fixup_offsets;
        uint32_t profile_offset;
        uint32_t cseg_size;
        uint32_t name_offset;
        uint16_t string_count;
        uint16_t export_count;
        uint16_t import_count;
        uint16_t fixup_count;
        uint16_t profile_count;
        uint16_t devblock_string_count;
        uint8_t include_count;
        uint8_t animtree_count;
        uint8_t flags;

        inline const char* GetName() const { return reinterpret_cast<const char*>(&magic[name_offset]); }
    };

    struct T7GscAnimTree {
        uint32_t name;
        uint16_t num_tree_address;
        uint16_t num_node_address;
    };
    struct T7GSCExport {
        uint32_t checksum;
        uint32_t address;
        uint32_t name;
        uint32_t name_space;
        uint8_t param_count;
        uint8_t flags;
        uint16_t padding;
    };

    enum T7GSCExportFlags {
        T7GEF_LINKED = 1,
        T7GEF_AUTOEXEC = 2,
        T7GEF_PRIVATE = 4,
        T7GEF_VE = 0x20,
    };

    enum T7GSCImportFlags {
        T7GIF_FUNC_METHOD = 1,
        T7GIF_FUNCTION = 2,
        T7GIF_FUNCTION_THREAD = 3,
        T7GIF_METHOD = 4,
        T7GIF_METHOD_THREAD = 5,
        T7GIF_DEV_CALL = 0x10,
        T7GIF_GET_CALL = 0x20,
    };

} // namespace tool::gsc