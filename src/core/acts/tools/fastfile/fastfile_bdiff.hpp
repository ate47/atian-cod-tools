#pragma once
#include <core/bytebuffer.hpp>
#include <acts_api/fastfile.h>

namespace fastfile::bdiff {
    // used for the checksum
	using BDiffType = ActsAPIFastFile_BDiffType;
	using BDiffState = ActsAPIFastFile_BDiffState;

    using vcSourceCB_t = ActsAPIFastFile_sourceCallback;
    using vcDiffCB_t = ActsAPIFastFile_diffCallback;
    using vcDestCB_t = ActsAPIFastFile_destCallback;

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