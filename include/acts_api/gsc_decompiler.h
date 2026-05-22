#ifndef __ACTS_API_GSC_DECOMPILER_H__
#define __ACTS_API_GSC_DECOMPILER_H__
#include "gsc.h"
/*
 * Atian Tools GSC API - Decompiler
 */

 // Decompiler context optional configuration
typedef struct {
    // output directory
    const char* outputDir;
    // write callback
    void (*WriteAsmFunction)(const char* str, size_t len, void* ud);
    // write callback data
    void* WriteAsmFunctionUserData;
	// write assembly output
    bool writeAsm;
	// write decompilation output (if writeDecompilation and writeAsm are both false, this value will be set to true)
    bool writeDecompilation;
    // write header
    bool writeHeader;
    // vm magic (if required)
    ActsAPIGsc_VmMagic vm;
    // code formatter
    const char* formatter;
} ActsAPIGscDecompiler_OptionalConfig;

// code formatters
typedef struct {
    const char** formatters;
    size_t count;
} ActsAPIGscDecompiler_Formatters;

/*
 * Creates a new GSC decompiler context
 * @param platform platform
 * @param optCfg optional configuration
 * @return decompiler context or INVALID_ACTS_HANDLE_VALUE if invalid
 */
ACTS_COMMON_API ActsHandle ActsAPIGscDecompiler_CreateDecompilerContext(
    ActsAPIGsc_Platform platform, ActsAPIGscDecompiler_OptionalConfig* ACTS_DEFAULT(optCfg, nullptr)
);

/*
 * Decompile a GSC file
 * @param context decompiler context
 * @param file file to decompile
 * @return status
 */
ACTS_COMMON_API ActsStatus ActsAPIGscDecompiler_DecompileFile(ActsHandle context, const char* file);

/*
 * Decompile a GSC object
 * @param context decompiler context
 * @param data data to decompile
 * @param size size of data
 * @return status
 */
ACTS_COMMON_API ActsStatus ActsAPIGscDecompiler_DecompileObject(ActsHandle context, byte* data, size_t size);

/*
 * Get the formatter names
 * @return formatters
 */
ACTS_COMMON_API ActsAPIGscDecompiler_Formatters* ActsAPIGscDecompiler_GetFormatters();


#endif // __ACTS_API_GSC_DECOMPILER_H__