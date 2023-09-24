#pragma once

#include <dll_includes.hpp>

namespace bo4 {
	BYTE* BasePtr();

	enum offset : uintptr_t {
		OFFSET_ScrVm_Error = 0x2770330,
		OFFSET_DB_FindXAssetHeader = 0x2EB75B0,
		OFFSET_StringTable_GetAsset = 0x28A2660,
		OFFSET_LogCompilerError = 0x2890470,
		OFFSET_Error = 0x3D36CC0,

		OFFSET_Scr_GetFunctionReverseLookup = 0x33AF8A0,
		OFFSET_CScr_GetFunctionReverseLookup = 0x1F132A0,
		OFFSET_Scr_GetFunction = 0x1F13140,
		OFFSET_CScr_GetFunction = 0x33AF840,
		OFFSET_Scr_GetMethod = 0x33AFC20,
		OFFSET_CScr_GetMethod = 0x1F13650,
	};
}
