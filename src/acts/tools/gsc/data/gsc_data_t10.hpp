#pragma once

namespace tool::gsc {

    struct GscObj24 {
        byte magic[8];
        uint64_t name;
        uint16_t unk10;
        uint16_t animtree_use_count;
        uint16_t animtree_count;
        uint16_t devblock_string_count;
        uint16_t export_count;
        uint16_t fixup_count;
        uint16_t unk1C;
        uint16_t imports_count;
        uint16_t includes_count;
        uint16_t string_count;
        uint32_t checksum;
        uint32_t unk28;
        uint32_t animtree_use_offset;
        uint32_t animtree_offset;
        uint32_t cseg_offset;
        uint32_t cseg_size;
        uint32_t devblock_string_offset;
        uint32_t export_offset;
        uint32_t fixup_offset;
        uint32_t size1;
        uint32_t import_table;
        uint32_t include_table;
        uint32_t string_table;
    };
    struct IW24GSCExport {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint32_t address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct IW24GSCExport2 {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint32_t checksum;
        uint32_t address;
        uint8_t param_count;
        uint8_t flags;
    };

}