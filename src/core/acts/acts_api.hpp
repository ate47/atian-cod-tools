#pragma once


#ifndef ACTS_COMMON_API
#define ACTS_EXTERN_C extern "C"
#ifdef ACTS_COMMON_DLL
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllexport)
#else
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllimport)
#endif
#endif

/*
 * Init ACTS common api
 */
ACTS_COMMON_API int InitActsAPI(bool cli, int* argc, const char*** argv, uint32_t version);