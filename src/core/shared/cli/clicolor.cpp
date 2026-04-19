#include <includes_shared.hpp>
#include <cli/clicolor.hpp>
#include <utils/utils.hpp>
#include <platform/platform.hpp>

namespace cli::clicolor {
	bool ConsoleAllowColor() {
		return platform::PlatformConsoleAllowColor();
	}

}
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorData data) {
	if (data) {
		if (data & cli::clicolor::CD_BACKGROUND) {
			return out << "\033[48;5;" << std::dec << (int)(data & cli::clicolor::CD_COLOR_MASK) << "m";
		}
		return out << "\033[38;5;" << std::dec << (int)(data & cli::clicolor::CD_COLOR_MASK) << "m";
	}

	return out << "\033[0m";
}