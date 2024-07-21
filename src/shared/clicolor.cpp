#include "includes_shared.hpp"
#include "clicolor.hpp"
#include "utils.hpp"

namespace clicolor {
	bool ConsoleAllowColor() {
		static bool color = [] {
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD mode{};
			return handle && GetConsoleMode(handle, &mode) && (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
		}();
		return color;
	}

}
std::ostream& operator<<(std::ostream& out, clicolor::ColorData data) {
	if (data) {
		if (data & clicolor::CD_BACKGROUND) {
			return out << "\033[48;5;" << std::dec << (int)(data & clicolor::CD_COLOR_MASK) << "m";
		}
		return out << "\033[38;5;" << std::dec << (int)(data & clicolor::CD_COLOR_MASK) << "m";
	}

	return out << "\033[0m";
}