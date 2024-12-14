#pragma once
#include <utils/utils.hpp>

namespace cli::clicolor {

	enum ColorData : uint32_t {
		CD_RESET = 0,
		CD_BACKGROUND = 0x10000000,
		CD_COLOR_MASK = ~(CD_BACKGROUND),
	};

	bool ConsoleAllowColor();

	constexpr ColorData Reset() {
		return CD_RESET;
	}

	constexpr ColorData Color(int r, int g, int b) {
		return (ColorData)(16 + 36 * r + 6 * g + b);
	}

	constexpr ColorData ResetBackground() {
		return CD_RESET;
	}

	constexpr ColorData ColorBackground(int r, int g, int b) {
		return (ColorData)(16 + 36 * r + 6 * g + b | CD_BACKGROUND);
	}

}
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorData data);
template<>
struct std::formatter<cli::clicolor::ColorData, char> : utils::BasicFormatter<cli::clicolor::ColorData> {};