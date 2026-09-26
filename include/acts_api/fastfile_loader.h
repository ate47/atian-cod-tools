#ifndef __ACTS_API_FASTFILE_LOADER_H__
#define __ACTS_API_FASTFILE_LOADER_H__
#include "api.h"

/*
 * Load fastfiles using the asset-pool workflow and write the generated asset index.
 * The function returns after the index is written and does not start the console REPL.
 */
typedef struct {
    size_t structSize;
    const char* gamePath;
    const char* handler;
    const char* outputPath;
    bool patch;
} ActsAPIFastFile_AssetPoolOptions;

typedef struct {
    const char* name;
    bool loaded;
} ActsAPIFastFile_FastFileEntry;

typedef struct {
    const char* id;
    const char* description;
} ActsAPIFastFile_FastFileHandlerEntry;

// create an asset pool context
ACTS_COMMON_API ActsHandle ActsAPIFastFile_CreateAssetPoolContext(const ActsAPIFastFile_AssetPoolOptions* options);

// init the context
ACTS_COMMON_API ActsStatus ActsAPIFastFile_AssetPoolInit(ActsHandle assetPool);

// load fastfiles
ACTS_COMMON_API ActsStatus ActsAPIFastFile_AssetPoolLoadFastFile(
    ActsHandle assetPool, const char* file, const char* wildcard, const char* ignoreWildcard
);
ACTS_COMMON_API ActsStatus ActsAPIFastFile_AssetPoolLoadFastFileEntry(
    ActsHandle assetPool, const ActsAPIFastFile_FastFileEntry* entry, bool force
);

// callback for ActsAPIFastFile_ListFastFile, called for each matched entry
// return if we need to continue the listing
typedef bool (*ActsAPIFastFile_ListFastFile_Callback)(const ActsAPIFastFile_FastFileEntry* entry, void* ud);

// callback for ActsAPIFastFile_ListHandlers
typedef void (*ActsAPIFastFile_ListHandlers_Callback)(const ActsAPIFastFile_FastFileHandlerEntry* entry, void* ud);

// list the fastfiles
ACTS_COMMON_API ActsStatus ActsAPIFastFile_ListFastFile(
    ActsHandle assetPool, const char* file, const char* wildcard, const char* ignoreWildcard,
    ActsAPIFastFile_ListFastFile_Callback callback, void* ud
);
// list the fastfile handlers
ACTS_COMMON_API ActsStatus ActsAPIFastFile_ListHandlers(ActsAPIFastFile_ListHandlers_Callback callback, void* ud);

// load the common fastfiles
ACTS_COMMON_API ActsStatus ActsAPIFastFile_AssetPoolLoadCommonFastFiles(ActsHandle assetPool);

// dump the cordycep index
ACTS_COMMON_API ActsStatus ActsAPIFastFile_AssetPoolWriteIndex(ActsHandle assetPool);

#endif // __ACTS_API_FASTFILE_LOADER_H__
