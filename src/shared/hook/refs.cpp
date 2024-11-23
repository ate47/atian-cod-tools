#include <includes_shared.hpp>
#include <hook/process.hpp>
#include "refs.hpp"

namespace hook::refs {
	
}

byte* operator"" _g(size_t val) {
	return hook::process::Relativise(val - 0x140000000);
}

byte* operator"" _a(size_t val) {
	return hook::process::Relativise(val);
}
