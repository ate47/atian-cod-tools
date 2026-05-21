#ifndef __ACTS_API_API_UI_H__
#define __ACTS_API_API_UI_H__
#include <stdint.h>
#include <stdbool.h>
/*
 * Atian Tools Common UI API commons
 */


#if __cplusplus
#define ACTS_DEFAULT(name, value) name = (value)
#else // !__cplusplus
#define ACTS_DEFAULT(name, value) name
#endif // __cplusplus

#ifndef ACTS_COMMON_UI_API
#ifdef __cplusplus
#define ACTS_EXTERN_C extern "C"
#else
#define ACTS_EXTERN_C
#endif

#ifdef _WIN32

#ifdef ACTS_COMMON_UI_STATIC
#define ACTS_COMMON_UI_API ACTS_EXTERN_C
#elif defined(ACTS_COMMON_UI_DLL)
#define ACTS_COMMON_UI_API ACTS_EXTERN_C __declspec(dllexport)
#else
#define ACTS_COMMON_UI_API ACTS_EXTERN_C __declspec(dllimport)
#endif

#else // non‑Windows (Linux, macOS)

#ifdef ACTS_COMMON_UI_STATIC
#define ACTS_COMMON_UI_API ACTS_EXTERN_C
#else
#define ACTS_COMMON_UI_API ACTS_EXTERN_C __attribute__((visibility("default")))
#endif

#endif

#endif


#endif // __ACTS_API_API_UI_H__