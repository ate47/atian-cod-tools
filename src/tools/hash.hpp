#pragma once
#include <includes.hpp>

namespace tool::hash {
	int lookuptool(const process& proc, int argc, const char* argv[]);
	int hash32(const process& proc, int argc, const char* argv[]);
	int hash64(const process& proc, int argc, const char* argv[]);
}