#include <includes_shared.hpp>
#include "logs.hpp"
#include "clicolor.hpp"

const char* alogs::name(loglevel lvl) {
	switch (lvl)
	{
	case alogs::LVL_TRACE:
		return "TRACE";
	case alogs::LVL_ERROR:
		return "ERROR";
	case alogs::LVL_WARNING:
		return "WARN.";
	case alogs::LVL_INFO:
		return "INFO.";
	case alogs::LVL_DEBUG:
		return "DEBUG";
	default:
		return "UKN..";
	}
}
namespace {
	alogs::loglevel g_loglevel = alogs::LVL_INFO;
	const char* g_logfile = NULL;
	bool g_basiclog{};
}

void alogs::setlevel(loglevel lvl) {
	g_loglevel = lvl;
}
alogs::loglevel alogs::getlevel() {
	return g_loglevel;
}
void alogs::setbasiclog(bool basiclog) {
	g_basiclog = basiclog;
}

void alogs::setfile(const char* filename) {
	g_logfile = filename;
}
const char* alogs::logfile() {
	return g_logfile;
}

inline std::tm localtime_xp(std::time_t timer) {
	// https://stackoverflow.com/questions/38034033/c-localtime-this-function-or-variable-may-be-unsafe
	std::tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

void alogs::log(loglevel level, const std::string& str) {
	if (getlevel() > level) {
		return;
	}

	auto f = [&](std::ostream& out, bool color) {
		if (!g_basiclog) {
			if (color) {
				out << clicolor::Color(5, 5, 5);
			}
			std::tm tm = localtime_xp(std::time(nullptr));
			out
				<< "[" << std::put_time(&tm, "%H:%M:%S") << "]"
				;
		}
		if (color) {
			switch (level) {
			case alogs::LVL_ERROR: out << clicolor::Color(5, 1, 1); break;
			case alogs::LVL_WARNING: out << clicolor::Color(5, 4, 1); break;
			case alogs::LVL_INFO: out << clicolor::Color(5, 5, 5); break;
			case alogs::LVL_DEBUG: out << clicolor::Color(4, 5, 1); break;
			case alogs::LVL_TRACE: out << clicolor::Color(1, 5, 5); break;
			default: out << clicolor::Reset(); break;
			}
		}

		if (!g_basiclog) {
			out
				<< '[' << name(level) << "] "
				;
		}
		out << str;

		if (color) {
			out << clicolor::Reset();
		}
		out << "\n";
	};

	auto* lf = logfile();
	if (lf) {
		std::ofstream out{ lf, std::ios::app };

		f(out, false);

		out.close();
	}
	else {
		static bool allowColor = clicolor::ConsoleAllowColor();

		f(level < LVL_WARNING ? std::cout : std::cerr, allowColor);
	}
}
