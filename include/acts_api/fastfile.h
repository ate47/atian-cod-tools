#ifndef __ACTS_API_FASTFILE_H__
#define __ACTS_API_FASTFILE_H__
#include "api.h"
/*
 * Atian Tools FastFile API
 */

// bdiff type, used for checksum validation
typedef enum {
    // unknown, no checksum validation will be done.
    BDT_UNKNOWN = 0,
    // treyarch VM games (bo3/bo4/cw) XXH32 per block.
    BDT_TREYARCH = 1,
    // IW VM games (iw) reduced XXH64 with last checksum usage.
    BDT_IW = 2,
} ActsAPIFastFile_BDiffType;

// state to keep between bdiff calls.
typedef struct {
    // if the bdiff header has been read.
    bool headerRead;
	// if bdiff returns false, this will be set to an error message.
    const char* error;
	// custom state for the bdiff implementation, will be passed to the callbacks.
    void* state;
    // last obtained checksum.
    uint32_t lastChecksum;
	// bdiff type, used for checksum validation.
    ActsAPIFastFile_BDiffType type;
} ActsAPIFastFile_BDiffState;


/*
 * Source data callback (.ff data), should return a pointer with the requested size at the requested offset.
 * @param state ActsAPIFastFile_BDiffState::state value, can be used to keep custom state between calls.
 * @param offset offset to read from
 * @param size size to read
 * @return pointer to the requested data, should be valid until the next call to the callback
 */
typedef uint8_t* ActsAPIFastFile_sourceCallback(void* state, size_t offset, size_t size);

/*
 * Diff data callback (.fp/.fd data), should return a pointer with the requested size at the requested offset.
 * if offset is 0, the last offset should be used and pOffset should be set to the last offset.
 * @param state ActsAPIFastFile_BDiffState::state value, can be used to keep custom state between calls.
 * @param offset offset to read from or 0 for last offset
 * @param size size to read
 * @param pOffset output for the used offset
 * @return pointer to the requested data, should be valid until the next call to the callback
 */
typedef uint8_t* ActsAPIFastFile_diffCallback(void* state, size_t offset, size_t size, size_t* pOffset);

/*
 * Destination data callback, should return a pointer with the requested size
 * @param state ActsAPIFastFile_BDiffState::state value, can be used to keep custom state between calls.
 * @param size size to read
 * @return pointer to the requested data, should be valid until the next call to the callback
 */
typedef uint8_t* ActsAPIFastFile_destCallback(void* state, size_t size);

/*
 * Perform a binary difference between a source and a diff to create a destination data.
 * @param state ActsAPIFastFile_BDiffState state to keep between calls, should be initialized with zeros before the first call.
 * @param sourceDataCB callback to read source data.
 * @param patchDataCB callback to read patch data.
 * @param destDataCB callback to write destination data.
 * @return true if the diff was applied successfully, false otherwise with state->error set to an error message.
 */
ACTS_COMMON_API bool ActsAPIFastFile_bdiff(
    ActsAPIFastFile_BDiffState* state, 
    ActsAPIFastFile_sourceCallback* sourceDataCB, 
    ActsAPIFastFile_diffCallback* patchDataCB, 
    ActsAPIFastFile_destCallback* destDataCB
);

#endif // __ACTS_API_FASTFILE_H__