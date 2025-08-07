#pragma once
#pragma once

namespace tool::gsc {
    struct T6GscObj {
        byte magic[8];
        uint32_t unk8;
        uint32_t source_crc;
        uint64_t unk10;
        uint64_t unk18;
        uint64_t unk20;
        uint64_t unk28;
        uint16_t name_offset;
        uint16_t unk32;
        uint16_t unk34;
        uint16_t unk36;
        uint32_t unk38;
        uint32_t unk3c;
    };
}