#pragma once
#include <hook/library.hpp>

namespace deps::oodle {
	constexpr const char* OO2CORE_PREFIX = "oo2core_";
	constexpr const char* OO2CORE_6 = "oo2core_6_win64";
	constexpr const char* OO2CORE_7 = "oo2core_7_win64";
	constexpr const char* OO2CORE_8 = "oo2core_8_win64";

	bool LoadOodle(const char* libname);
	bool LoadOodleFromGame(const hook::library::Library& game);
	void FreeOodle();
	void GetConfigValues(int32_t* cfg);
	const hook::library::Library& GetOodleLib();
	int32_t GetVersion();
    size_t Decompress(void* src, size_t srcLen, void* dest, size_t destLen);
}