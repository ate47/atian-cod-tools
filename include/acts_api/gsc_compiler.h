#ifndef __ACTS_API_GSC_COMPILER_H__
#define __ACTS_API_GSC_COMPILER_H__
#include "gsc.h"
/*
 * Atian Tools GSC API - Compiler
 */

// Compiler context optional configuration
typedef struct {
	// checksum to use for the script, if 0, a checksum will be computed
	int32_t checksum;
	// do not use the default checksum
	bool noDefaultChecksum;
	// compute the dev option in the script
	bool computeDevOption;
	// obfuscate the script
	bool obfuscate;
	// use #define for constexpr (requires =)
	bool defineAsConstExpr;
	// disable inlining the dev calls in /# #/ blocks
	bool noDevCallInline;
	// use the mod tool opcodes if available
	bool useModToolOpCodes;
} ActsAPIGscCompiler_OptionalConfig;

/*
 * Create a compiler context
 * @param vm vm magic
 * @param platform platform
 * @param name name
 * @param clientScript client script flag
 * @param optCfg optional configuration
 * @return compiler context or INVALID_ACTS_HANDLE_VALUE if invalid
 */
ACTS_COMMON_API ActsHandle ActsAPIGscCompiler_CreateCompilerContext(
	ActsAPIGsc_VmMagic vm, ActsAPIGsc_Platform platform,
	const char* name, bool clientScript, ActsAPIGscCompiler_OptionalConfig* ACTS_DEFAULT(optCfg, nullptr)
);


/*
 * Add an input file to the compiler context.
 * @param context CompilerContext
 * @param file file path
 * @param recurse if true, if the file is a directory, it will be searched recursively for .gsc and .csc files (depending on clientScript)
 * @return ACTS_STATUS_OK if the input was compiled successfully, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API void ActsAPIGscCompiler_AddInput(ActsHandle context, const char* file, bool ACTS_DEFAULT(recurse, false));


/*
 * Compile a gsc script
 * @param context CompilerContext
 * @param writerCallback Writer callback function
 * @param ud User data for the writer callback
 * @return ACTS_STATUS_OK if the input was compiled successfully, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPIGscCompiler_Compile(ActsHandle context, void (*writerCallback)(void* ud, const uint8_t* data, size_t len), void* ACTS_DEFAULT(ud, nullptr));


#endif // __ACTS_API_GSC_COMPILER_H__