#ifndef __ACTS_API_GSC_COMPILER_H__
#define __ACTS_API_GSC_COMPILER_H__
#include "gsc.h"
/*
 * Atian Tools GSC API - Compiler
 */

typedef struct {
	int32_t checksum;
	bool noDefaultChecksum;
	bool computeDevOption;
	bool obfuscate;
	bool defineAsConstExpr;
	bool noDevCallInline;
	bool useModToolOpCodes;
} ActsAPIGscCompiler_OptionalConfig;

ACTS_COMMON_API ActsHandle ActsAPIGscCompiler_CreateCompilerContext(
	ActsAPIGsc_VmMagic vm, ActsAPIGsc_Platform platform,
	const char* name, bool clientScript, ActsAPIGscCompiler_OptionalConfig* ACTS_DEFAULT(optCfg, nullptr)
);

ACTS_COMMON_API void ActsAPIGscCompiler_AddInput(ActsHandle context, const char* file, bool ACTS_DEFAULT(recurse, false));


ACTS_COMMON_API ActsStatus ActsAPIGscCompiler_Compile(ActsHandle context, void (*writerCallback)(void* ud, const uint8_t* data, size_t len), void* ACTS_DEFAULT(ud, nullptr));


#endif // __ACTS_API_GSC_COMPILER_H__