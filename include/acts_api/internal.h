#ifndef __ACTS_API_INTERNAL_H__
#define __ACTS_API_INTERNAL_H__
#include "api.h"
/*
 * Internal API used by Atian Tools
 * IMPORTANT: None of these functions are meant to be used by external systems, they are not ABI safe and may change without any notice.
 * If you want to use ACTS API, please refer to api.h
 */

 /*
  * Init ACTS common API, remove ACTS options from argc/argv and update them if needed
  * @param cli cli mode
  * @param argc argc
  * @param argv argv
  * @param version acts API version, used to check for version mismatch between the API and the common library
  * @return ret code
  */
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
 * get the shared config, used to store global data for ACTSharedLibrary.
 * @return the shared config
 */
ACTS_COMMON_API void* GetActsSharedConfig();

/*
 * set the shared config, used to store global data for ACTSharedLibrary.
 * usage: core::shared_cfg::SetSharedConfigPtr(GetActsSharedConfig());
 * @param cfg the shared config
 */
ACTS_COMMON_API void SetActsSharedConfig(void* cfg);

#endif // __ACTS_API_INTERNAL_H__