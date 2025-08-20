#pragma once

namespace fastfile::bdiff {
    // used for the hash
    enum BDiffType {
        BDT_UNKNOWN = 0,
        BDT_TREYARCH = 1,
        BDT_IW = 2,
    };
    struct BDiffState {
        bool headerRead;
        const char* error;
        void* state;
        BDiffType type;
    };
    typedef uint8_t* vcSourceCB_t(void* state, size_t offset, size_t size);
    typedef uint8_t* vcDiffCB_t(void* state, size_t offset, size_t size, size_t* pOffset);
    typedef uint8_t* vcDestCB_t(void* state, size_t size);

    bool bdiff(BDiffState* state, vcSourceCB_t* sourceDataCB, vcDiffCB_t* patchDataCB, vcDestCB_t* destDataCB);
}