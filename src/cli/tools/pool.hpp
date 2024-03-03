#pragma once

#include <includes.hpp>

namespace tool::pool {

	void WriteHex(std::ostream& out, uintptr_t base, BYTE* buff, SIZE_T size, const Process& proc);

	struct XHash {
		uint64_t name;
		uintptr_t pad0;
	};

	// keep compatibility
	typedef XHash Hash;
}