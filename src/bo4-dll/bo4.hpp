#pragma once

#include <dll_includes.hpp>

namespace bo4 {
	BYTE* BasePtr();
	
	enum offset : uintptr_t {
		OFFSET_ScrVm_Error = 0x2770330,
		OFFSET_DB_FindXAssetHeader = 0x2EB75B0,
		OFFSET_StringTable_GetAsset = 0x28A2660,
		OFFSET_LogCompilerError = 0x2890470,
		OFFSET_Error = 0x3D36CC0
	};
}