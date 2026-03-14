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

	constexpr ColorData COLOR_WHITE = Color(5, 5, 5);
	constexpr ColorData COLOR_LIGHT_GRAY = Color(3, 3, 3);
	constexpr ColorData COLOR_DARL_GRAY = Color(2, 2, 2);
	constexpr ColorData COLOR_BLACK = Color(0, 0, 0);
	constexpr ColorData COLOR_RED = Color(5, 1, 1);
	constexpr ColorData COLOR_GREEN = Color(1, 5, 1);
	constexpr ColorData COLOR_BLUE = Color(1, 1, 5);
}
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorData data);
template<>
struct std::formatter<cli::clicolor::ColorData, char> : utils::BasicFormatter<cli::clicolor::ColorData> {};