#pragma once
#include <includes.hpp>

namespace tool::vm_debug {
	int vmdebug(const Process& proc, int argc, const char* argv[]);
}