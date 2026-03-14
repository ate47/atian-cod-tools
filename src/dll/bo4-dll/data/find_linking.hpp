#pragma once

#include <dll_includes.hpp>

namespace find_linking {
	constexpr auto ERROR_LINKING = 1670707254;

	int CheckLinkingError(scriptinstance::ScriptInstance inst);
}