#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define ACTS_EXTERN_C extern "C"

#ifdef ACTS_COMMON_DLL
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllexport)
#else
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllimport)
#endif
