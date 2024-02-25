#pragma once

#include <dll_includes.hpp>
#include <scriptinstance.hpp>

namespace find_linking {
	constexpr auto ERROR_LINKING = 1670707254;

	int CheckLinkingError(scriptinstance::ScriptInstance inst);
}