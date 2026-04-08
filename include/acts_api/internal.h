#ifndef __ACTS_API_INTERNAL_H__
#define __ACTS_API_INTERNAL_H__
#include "api.h"
/*
 * Internal API used by Atian Tools
 */

// init cli
ACTS_COMMON_API int InitActsAPI(bool cli, int* argc, const char*** argv, uint32_t version);

/*
 * Init ACTS common
 * @param argc main argc
 * @param argv main argv
 * @param hInstance hInstance, if non null, ui otherwise CLI
 * @return ret code
 */
ACTS_COMMON_API int MainActs(int argc, const char* argv[], void* hInstance = nullptr, int nShowCmd = 0);

/*
 * Register a new tool
 * @param name tool name
 * @param filename file name
 * @param line file line
 * @param category category
 * @param usage usage
 * @param description description
 * @param func function callback
 */
ACTS_COMMON_API void RegisterActsTool(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[]));

/*
 * Register a new tool category
 * @param description description
 * @param visible if the category is visible
 */
ACTS_COMMON_API void RegisterActsCategory(const char* name, const char* description, bool visible);

/*
 * @return the shared config
 */
ACTS_COMMON_API void* GetActsSharedConfig();

/*
 * set the shared config
 */
ACTS_COMMON_API void SetActsSharedConfig(void* cfg);

#endif // __ACTS_API_INTERNAL_H__