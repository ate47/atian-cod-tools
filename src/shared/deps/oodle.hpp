#pragma once

namespace deps::oodle {
	const char* OO2CORE_6 = "oo2core_6_win64";
	const char* OO2CORE_7 = "oo2core_7_win64";
	const char* OO2CORE_8 = "oo2core_8_win64";

	bool LoadOodle(const char* libname);
	void FreeOodle();
	void GetConfigValues(int32_t* cfg);
	int32_t GetVersion();
    size_t Decompress(void* src, size_t srcLen, void* dest, size_t destLen);
}