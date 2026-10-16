#pragma once

#if __has_include("asmjit/asmjit.h")
#define ASMJIT_STATIC
#define ASMJIT_NO_AARCH64
#define ASMJIT_BUILD_RELEASE
#define ASMJIT_NO_FOREIGN
#include <asmjit/asmjit.h>
#endif
