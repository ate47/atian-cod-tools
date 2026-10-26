#include <includes_shared.hpp>
#include <cli/clicolor.hpp>
#include <utils/utils.hpp>
#include <platform/platform.hpp>

namespace cli::clicolor {
    bool ConsoleAllowColor() { return platform::PlatformConsoleAllowColor(); }

    ColoredText ColorData::Text(const std::string& text) const { return ColoredText{ text, color }; }

} // namespace cli::clicolor
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColoredText data) {
    return out << data.color << data.text << cli::clicolor::CD_RESET;
}
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorInfo color) {
    if (color) {
        if (color & cli::clicolor::CD_BACKGROUND) {
            return out << "\033[48;5;" << std::dec << (int)(color & cli::clicolor::CD_COLOR_MASK) << "m";
        }
        return out << "\033[38;5;" << std::dec << (int)(color & cli::clicolor::CD_COLOR_MASK) << "m";
    }

    return out << "\033[0m";
}
std::ostream& operator<<(std::ostream& out, cli::clicolor::ColorData data) { return out << data.color; }