#pragma once
#include <utils/utils.hpp>

namespace cli::clicolor {

    enum ColorInfo : uint32_t {
        CD_RESET = 0,
        CD_BACKGROUND = 0x10000000,
        CD_COLOR_MASK = ~(CD_BACKGROUND),
    };

    struct ColoredText;

    class ColorData {
      public:
        ColorInfo color;
        constexpr ColorData(ColorInfo color) : color(color) {}

        ColoredText Text(const std::string& text) const;

        template<typename... Args>
        ColoredText Format(std::format_string<Args...> format, Args&&... args) const {
            return Text(std::format<Args...>(format, std::forward<Args>(args)...));
        }
    };

    struct ColoredText {
        std::string text;
        ColorData color;
    };

    bool ConsoleAllowColor();

    constexpr ColorInfo Reset() { return CD_RESET; }

    constexpr ColorInfo Color(int r, int g, int b) { return (ColorInfo)(16 + 36 * r + 6 * g + b); }

    constexpr ColorInfo ResetBackground() { return CD_RESET; }

    constexpr ColorInfo ColorBackground(int r, int g, int b) {
        return (ColorInfo)(16 + 36 * r + 6 * g + b | CD_BACKGROUND);
    }

    // Gray
    constexpr ColorData COLOR_BLACK = Color(0, 0, 0);
    constexpr ColorData COLOR_DARK_GRAY = Color(2, 2, 2);
    constexpr ColorData COLOR_GRAY = Color(3, 3, 3);
    constexpr ColorData COLOR_LIGHT_GRAY = Color(4, 4, 4);
    constexpr ColorData COLOR_WHITE = Color(5, 5, 5);

    // Reds
    constexpr ColorData COLOR_RED = Color(5, 1, 1);
    constexpr ColorData COLOR_LIGHT_RED = Color(5, 2, 2);
    constexpr ColorData COLOR_DARK_RED = Color(3, 0, 0);
    constexpr ColorData COLOR_CRIMSON = Color(4, 0, 1);
    constexpr ColorData COLOR_MAROON = Color(2, 0, 0);

    // Oranges
    constexpr ColorData COLOR_ORANGE = Color(5, 3, 0);
    constexpr ColorData COLOR_DARK_ORANGE = Color(4, 2, 0);
    constexpr ColorData COLOR_CORAL = Color(5, 3, 2);
    constexpr ColorData COLOR_AMBER = Color(5, 4, 0);

    // Yellows
    constexpr ColorData COLOR_YELLOW = Color(5, 5, 0);
    constexpr ColorData COLOR_GOLD = Color(5, 4, 1);
    constexpr ColorData COLOR_LEMON = Color(5, 5, 2);
    constexpr ColorData COLOR_DARK_YELLOW = Color(3, 3, 0);

    // Greens
    constexpr ColorData COLOR_GREEN = Color(1, 5, 1);
    constexpr ColorData COLOR_LIGHT_GREEN = Color(2, 5, 2);
    constexpr ColorData COLOR_DARK_GREEN = Color(0, 3, 0);
    constexpr ColorData COLOR_LIME = Color(0, 5, 0);
    constexpr ColorData COLOR_FOREST_GREEN = Color(1, 4, 1);
    constexpr ColorData COLOR_OLIVE = Color(3, 3, 0);

    // Cyans
    constexpr ColorData COLOR_CYAN = Color(0, 5, 5);
    constexpr ColorData COLOR_LIGHT_CYAN = Color(2, 5, 5);
    constexpr ColorData COLOR_DARK_CYAN = Color(0, 3, 3);
    constexpr ColorData COLOR_TURQUOISE = Color(1, 5, 4);
    constexpr ColorData COLOR_TEAL = Color(0, 3, 2);

    // Blues
    constexpr ColorData COLOR_BLUE = Color(1, 1, 5);
    constexpr ColorData COLOR_LIGHT_BLUE = Color(2, 3, 5);
    constexpr ColorData COLOR_DARK_BLUE = Color(0, 0, 3);
    constexpr ColorData COLOR_NAVY = Color(0, 0, 2);
    constexpr ColorData COLOR_SKY_BLUE = Color(2, 4, 5);
    constexpr ColorData COLOR_AZURE = Color(0, 4, 5);

    // Purples
    constexpr ColorData COLOR_MAGENTA = Color(5, 0, 5);
    constexpr ColorData COLOR_LIGHT_MAGENTA = Color(5, 2, 5);
    constexpr ColorData COLOR_DARK_MAGENTA = Color(3, 0, 3);
    constexpr ColorData COLOR_PURPLE = Color(3, 0, 5);
    constexpr ColorData COLOR_VIOLET = Color(4, 2, 5);
    constexpr ColorData COLOR_INDIGO = Color(1, 0, 4);

    // Pinks
    constexpr ColorData COLOR_PINK = Color(5, 2, 4);
    constexpr ColorData COLOR_HOT_PINK = Color(5, 0, 3);
    constexpr ColorData COLOR_ROSE = Color(5, 1, 3);

    // Browns
    constexpr ColorData COLOR_BROWN = Color(3, 1, 0);
    constexpr ColorData COLOR_DARK_BROWN = Color(2, 1, 0);
    constexpr ColorData COLOR_CHOCOLATE = Color(4, 2, 1);
    constexpr ColorData COLOR_TAN = Color(4, 3, 2);
} // namespace cli::clicolor

std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorData data);
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColoredText data);
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorInfo data);
template<>
struct std::formatter<cli::clicolor::ColorData, char> : utils::BasicFormatter<cli::clicolor::ColorData> {};
template<>
struct std::formatter<cli::clicolor::ColoredText, char> : utils::BasicFormatter<cli::clicolor::ColoredText> {};
template<>
struct std::formatter<cli::clicolor::ColorInfo, char> : utils::BasicFormatter<cli::clicolor::ColorInfo> {};