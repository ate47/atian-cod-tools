#pragma once

#include <includes.hpp>
#include <games/bo4/pool.hpp>

namespace tool::pool {

	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size, const Process& proc);
	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size);
	inline void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size, const Process& proc) {
		WriteHex(out, base, reinterpret_cast<byte*>(buff), size, proc);
	}
	inline void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size) {
		WriteHex(out, base, reinterpret_cast<byte*>(buff), size);
	}

	const char* ReadMTString(const Process& proc, uint32_t val);
	inline const char* ReadMTString(const Process& proc, uint32_t val, const char* defaultValue) {
		const char* v = ReadMTString(proc, val);
		if (v) return v;
		return defaultValue;
	}
	int pooltool(Process& proc, int argc, const char* argv[]);

	// keep compatibility
	typedef XHash Hash;
}