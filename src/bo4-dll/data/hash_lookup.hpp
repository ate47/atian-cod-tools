#pragma once
#include <dll_includes.hpp>

namespace hash_lookup {
	void LoadFile(const char* file);
	const char* Extract(uint64_t hash);
	const char* ExtractTmp(scriptinstance::ScriptInstance inst, uint64_t hash, const char* prefix = NULL);
}