#pragma once
#include <Windows.h>

namespace scriptinstance {
	enum ScriptInstance : uint32_t {
		SI_SERVER = 0,
		SI_CLIENT = 1,
		SI_COUNT = 2
	};
	const char* Name(ScriptInstance inst);
	inline const char* Name(uint64_t inst) {
		return Name((ScriptInstance)inst);
	}
}