#pragma once
#include <Windows.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#ifdef ERROR
#undef ERROR
#endif

namespace platform {
	PIMAGE_DOS_HEADER PImageDosHeader(void* mod = nullptr);
	PIMAGE_NT_HEADERS PImageNtHeader(void* mod = nullptr);
	PIMAGE_OPTIONAL_HEADER PImageOptHeader(void* mod = nullptr);
}
