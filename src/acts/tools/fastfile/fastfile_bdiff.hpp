#pragma once
#include <core/bytebuffer.hpp>

namespace fastfile::bdiff {
    // used for the checksum
    enum BDiffType {
        BDT_UNKNOWN = 0,
        BDT_TREYARCH = 1,
        BDT_IW = 2,
    };
    struct BDiffState {
        bool headerRead;
        const char* error;
        void* state;
        uint32_t lastChecksum{};
        BDiffType type;
    };
    typedef uint8_t* vcSourceCB_t(void* state, size_t offset, size_t size);
    typedef uint8_t* vcDiffCB_t(void* state, size_t offset, size_t size, size_t* pOffset);
    typedef uint8_t* vcDestCB_t(void* state, size_t size);

    bool bdiff(BDiffState* state, vcSourceCB_t* sourceDataCB, vcDiffCB_t* patchDataCB, vcDestCB_t* destDataCB);

    /*
     * bdiff byte buffers
     * @param sourceData source buffer
     * @param patchData patch buffer
     * @param type bdiff type, used for checksum validation
     * @param winsize max window size, used to avoid reallocation
     */
    std::vector<byte> bdiff(core::bytebuffer::ByteBuffer* sourceData, core::bytebuffer::ByteBuffer* patchData, BDiffType type = BDT_UNKNOWN, size_t winsize = 0x100000);
}