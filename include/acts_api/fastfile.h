#ifndef __ACTS_API_FASTFILE_H__
#define __ACTS_API_FASTFILE_H__
#include "api.h"
#include "gsc.h"
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

// cordycep id, used for cordycep compatibility with existing tools
typedef enum {
    CG_NULL = 0, // invalid
	CG_GHOSTS = 0xA0A5354534F4847, // call of duty ghosts
	CG_AW = 0x5241574E41564441, // call of duty advanced warfare
	CG_IW = 0x4652415749464E49, // call of duty infinite warfare
	CG_MWR = 0x30305453414D4552, // call of duty modern warfare remastered
	CG_MW2R = 0x32305453414D4552, // call of duty modern warfare 2 remastered
	CG_MW4 = 0x3931524157444F4D,// call of duty modern warfare (2019)
	CG_MW5 = 0x3232524157444F4D, // call of duty modern warfare II (2022)
	CG_MW6 = 0x4B4F4D41594D4159, // call of duty modern warfare III (2023)
	CG_VANGUARD = 0x44524155474E4156, // call of duty vanguard
	CG_BO3 = 0x33504f4b43414c42, // call of duty black ops 3
    CG_BO4 = 0x34504f4b43414c42, // call of duty black ops 4
    CG_BOCW = 0x574353504f4b4c42, // call of duty black ops cold war
    CG_BO6 = 0x36504f4b43414c42, // call of duty black ops 6
    CG_BO7 = 0x37504f4b43414c42, // call of duty black ops 7
} ActsAPIFastFile_CordycepGame;

// workflow used to read the fastfiles
typedef enum {
	FFW_NONE = 0, // default
	FFW_READER = 1, // reader
	FFW_ASSET_POOL = 2 // pool builder
} ActsAPIFastFile_FastfileWorkflow;

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

// max size for a fastfile name
#define ACTS_API_FASTFILE_MAX_SIZE 0x100
// max xblocks count, the highest value I saw was 16, but to be safe we can set it to 32
#define ACTS_API_FASTFILE_MAX_XBLOCKS 0x20

// XBlock definition, used to known the size obtained from the fastfile header
typedef struct {
    // xblock data
	uint8_t* data;
	// xblock size
	size_t size;
} ActsAPIFastFile_XBlockInfo;

// FastFile context
typedef struct {
    // loaded file path
	const char* file;
    // fastfile name
	char ffname[ACTS_API_FASTFILE_MAX_SIZE];
    // fastfile type, computed with ActsAPIFastFile_FastFileContext_GetType
	char fftype[ACTS_API_FASTFILE_MAX_SIZE];
    // set by the handler, the scr strings
	const char** scrStrings;
    // set by the handler, the scr strings count
	size_t scrStringsCount;
    // xblock sizes obtained from the header
    ActsAPIFastFile_XBlockInfo blockSizes[ACTS_API_FASTFILE_MAX_XBLOCKS];
	// xblock count obtained from the header
	size_t blocksCount;
    // return if this fastfile is using the old .gscbin format
	bool hasGSCBin;
	// the gsc platform determined or obtained from the header
	ActsAPIGsc_Platform gscPlatform;
} ActsAPIFastFile_FastFileContext;

// FastFile options
typedef struct {
	// load patch files
	bool m_fd;
	// ignore missing fast files
	bool m_fdIgnoreMissing;
	// alpha support
	bool alpha;
	// dump header info
	bool m_header;
	// dump decompressed data
	bool dump_decompressed;
	// do not dump assets
	bool noAssetDump;
	// asset container test
	bool assertContainer;
	// dump binary assets
	bool dumpBinaryAssets;
	// dump binary assets map
	bool dumpBinaryAssetsMap;
	// dump asset names
	bool dumpAssetNames;
	// dump xstrings
	bool dumpXStrings;
	// dump xhashes
	bool dumpXHash;
	// dump compiled zone
	bool dumpCompiledZone;
	// test
	bool testDump;
	// compute archive DDL
	bool archiveDDL;
	// compute graphic assets
	bool graphic;
	// reduce logs (if available for the handler)
	bool reducedLogs;
	// no worker preload (if available for the handler)
	bool noWorkerPreload;
	// CASC storage path
	const char* m_casc;
	// game
	const char* game;
	// game path
	const char* gamePath;
	// game exe
	const char* exec;
	// wildcard to match for the filename (regex)
	const char* wildcard;
	// wildcard to match for the filename ignoring (regex)
	const char* ignoreWildcard;
	// path to ignore
	const char* ignore;
	// asset to load
	const char* assets;
	// translation directory
	const char* translation;
	// header dump
	const char* headerDump;
	// RSA key name
	const char* rsaKey;
	// disable script decompilation (if available for the handler)
	bool disableScriptsDecomp;
	// asset types to dump
	const char* assetTypes;
	// output path
	const char* outputPath;
	// workflow used
	ActsAPIFastFile_FastfileWorkflow workflow;
} ActsAPIFastFile_FastFileOption;

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

// default window size for bdiff, used to reduce reallocation.
#define DEFAULT_BDIFF_WINDOW_SIZE 0x100000

/*
 * Perform a binary difference between a source and a diff to create a destination data.
 * @param sourceData source data buffer
 * @param sourceDataLen source data buffer length
 * @param patchData patch data buffer
 * @param patchDataLen patch data buffer length
 * @param outData VectorData handle
 * @param type used for checksum validation. Use BDT_UNKNOWN if unknown
 * @param winsize window size, used to reduce reallocation. Use DEFAULT_BDIFF_WINDOW_SIZE if unknown
 * @return true if the diff was applied successfully, false otherwise with state->error set to an error message.
 */
ACTS_COMMON_API ActsStatus ActsAPIFastFile_bdiffData(
    uint8_t* sourceData, size_t sourceDataLen,
    uint8_t* patchData, size_t patchDataLen,
    ActsHandle outData, ActsAPIFastFile_BDiffType ACTS_DEFAULT(type, BDT_UNKNOWN),
    size_t ACTS_DEFAULT(winsize, DEFAULT_BDIFF_WINDOW_SIZE)
);


/*
 * Custom handler init function
 * @param opt FastFile options
 * @param ud user data
 */
typedef bool (*ActsAPIFastFile_InitHandler)(ActsAPIFastFile_FastFileOption* opt, void* ud);
/*
 * Custom handler handle function, parse the decompressed data
 * @param opt FastFile options
 * @param context FastFile context
 * @param data decompressed data buffer
 * @param dataLen decompressed data buffer length
 * @param ud user data
 */
typedef bool (*ActsAPIFastFile_HandleHandler)(ActsAPIFastFile_FastFileOption* opt, ActsAPIFastFile_FastFileContext* context, uint8_t* data, size_t dataLen, void* ud);
/*
 * Custom handler cleanup function
 * @param ud user data
 */
typedef void (*ActsAPIFastFile_CleanupHandler)(void* ud);

/*
 * Register a custom handler for fastfiles, the handler will be available in the handlers
 * @param init custom handler init function, can be null
 * @param handle custom handler handle function, can be null (but why?)
 * @param cleanup custom handler cleanup function, can be null
 * @param userdata user data to pass to the handler functions
 * @param name handler name, used to identify the handler in the options
 * @param description handler description, used to describe the handler in the options
 * @param cordycepGame cordycep game id, used for cordycep compatibility with existing tools
 * @param noPatchOk if true, the handler will be available even if the patch files are not found, otherwise it will be ignored if the patch files are not found (default: false)
 */
ACTS_COMMON_API void ActsAPIFastFile_RegisterCustomHandler(
    ActsAPIFastFile_InitHandler init, ActsAPIFastFile_HandleHandler handle, ActsAPIFastFile_CleanupHandler cleanup, void* userdata,
	const char* name, const char* description, ActsAPIFastFile_CordycepGame cordycepGame, bool noPatchOk
);

/*
 * @return the current options used for the current handler, should be valid until the end of the fastfile tool execution.
 */
ACTS_COMMON_API ActsAPIFastFile_FastFileOption* ActsAPIFastFile_GetCurrentOption();
/*
 * @return the current fastfile context used for the current handler, should be valid until the end of the handler execution.
 */
ACTS_COMMON_API ActsAPIFastFile_FastFileContext* ActsAPIFastFile_GetCurrentContext();
/*
 * Compute and cache the fastfile type for a context
 * @param context FastFile context
 * @return the fastfile type, should be valid until the end of the handler execution.
 */
ACTS_COMMON_API const char* ActsAPIFastFile_FastFileContext_GetType(ActsAPIFastFile_FastFileContext* context);
/*
 * Get a translation for the current context
 * @param translation key
 * @return translation value, the unhashed value of name or a string version of name
 */
ACTS_COMMON_API const char* ActsAPIFastFile_GetTranslation(uint64_t name);
/*
 * Allocate zone memory for the current context
 * @param len memory length
 * @return allocated memory, or nullptr if no context exists
 */
ACTS_COMMON_API uint8_t* ActsAPIFastFile_AllocateZoneMemory(size_t len);
/*
 * Add an asset header to the current context
 * @param name asset name
 * @param header header pointer
 * @param type asset type
 * @param size asset type
 */
ACTS_COMMON_API void ActsAPIFastFile_AddAssetHeaderString(const char* name, void* header, uint32_t type, size_t size);
/*
 * Add an asset header to the current context
 * @param name asset name
 * @param header header pointer
 * @param type asset type
 * @param size asset type
 */
ACTS_COMMON_API void ActsAPIFastFile_AddAssetHeaderHashed(uint64_t name, void* header, uint32_t type, size_t size);
/*
 * Get a scr string
 * @param id scr string id
 * @return scr string
 */
ACTS_COMMON_API const char* ActsAPIFastFile_GetScrString(uint32_t id);
/*
 * Register a scr string
 * @param str scr string
 * @param prevId previous scr string id
 * @return scr string id
 */
ACTS_COMMON_API uint32_t ActsAPIFastFile_RegisterScrString(const char* str, uint32_t prevId);

#endif // __ACTS_API_FASTFILE_H__