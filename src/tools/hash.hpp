#pragma once
#include <includes.hpp>

namespace tool::hash {
	int lookuptool(const Process& proc, int argc, const char* argv[]);
	int hash32(const Process& proc, int argc, const char* argv[]);
	int hash64(const Process& proc, int argc, const char* argv[]);
}