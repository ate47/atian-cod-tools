#include <includes_shared.hpp>
#include <deps/ntdll.hpp>

namespace deps::ntdll {
	NtDll& NtDll::GetInstance() {
		static NtDll nt;
		return nt;
	}
}