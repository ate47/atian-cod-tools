#pragma once

#include <includes.hpp>

namespace tool::pool {

	void WriteHex(std::ostream& out, uintptr_t base, BYTE* buff, SIZE_T size, const Process& proc);

}