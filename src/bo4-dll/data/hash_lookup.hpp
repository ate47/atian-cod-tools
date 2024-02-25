#pragma once
#include <dll_includes.hpp>
#include <scriptinstance.hpp>

namespace hash_lookup {
	void LoadFile(LPCCH file);
	LPCCH Extract(UINT64 hash);
	LPCCH ExtractTmp(scriptinstance::ScriptInstance inst, UINT64 hash, LPCCH prefix = NULL);
}