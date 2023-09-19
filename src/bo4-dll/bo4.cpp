#include <dll_includes.hpp>

BYTE* bo4::BasePtr() {
	return reinterpret_cast<BYTE*>(GetModuleHandleA(NULL));
}
