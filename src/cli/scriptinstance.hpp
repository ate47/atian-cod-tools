#pragma once
#include <Windows.h>

namespace scriptinstance {
	enum ScriptInstance : UINT32 {
		SI_SERVER = 0,
		SI_CLIENT = 1,
		SI_COUNT = 2
	};
	LPCCH Name(ScriptInstance inst);
	inline LPCCH Name(UINT64 inst) {
		return Name((ScriptInstance)inst);
	}
}