#pragma once

#include <includes.hpp>

namespace tool::pool {

	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size, const Process& proc);
	inline void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size, const Process& proc) {
		WriteHex(out, base, reinterpret_cast<byte*>(buff), size, proc);
	}

	struct XHash {
		uint64_t name;
		uintptr_t pad0;
	};

	// keep compatibility
	typedef XHash Hash;
}