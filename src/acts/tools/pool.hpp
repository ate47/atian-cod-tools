#pragma once

#include <includes.hpp>

namespace tool::pool {

	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size, const Process& proc);
	inline void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size, const Process& proc) {
		WriteHex(out, base, reinterpret_cast<byte*>(buff), size, proc);
	}

	const char* ReadMTString(const Process& proc, uint32_t val);
	inline const char* ReadMTString(const Process& proc, uint32_t val, const char* defaultValue) {
		const char* v = ReadMTString(proc, val);
		if (v) return v;
		return defaultValue;
	}

	struct XHash {
		uint64_t name;
		uintptr_t pad0;
	};

	// keep compatibility
	typedef XHash Hash;
}