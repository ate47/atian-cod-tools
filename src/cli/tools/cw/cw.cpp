#include <includes.hpp>

char* cw::DecryptString(char* str) {
	// for now I'm using the DLL to call the decrypt function
	str += 3;
	if ((*str & 0xC0) == 0x80) {
		// encrypted
		str[0] = 0;
		str[1] = 0;
	}
	return str;
}