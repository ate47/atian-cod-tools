#pragma once

namespace tool::gsc {
    struct GscObj23 {
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
        uint16_t unk22;
        uint16_t string_count;
        uint16_t unk26;
        uint32_t checksum;
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
        uint32_t size2;
        uint32_t string_table;
        uint32_t unk5C;
    };
    struct IW23GSCImport {
        uint64_t name;
        uint64_t name_space;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct IW23GSCExport {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint64_t checksum;
        uint32_t address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct GSC_USEANIMTREE_ITEM {
        uint32_t num_address;
        uint32_t address;
    };

    struct GSC_ANIMTREE_ITEM {
        uint32_t num_address;
        uint32_t address_str1;
        uint32_t address_str2;
    };

}