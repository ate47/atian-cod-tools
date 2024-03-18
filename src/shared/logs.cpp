#include <includes_shared.hpp>
#include "logs.hpp"

LPCCH alogs::name(loglevel lvl) {
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
	LPCCH g_logfile = NULL;
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

void alogs::setfile(LPCCH filename) {
	g_logfile = filename;
}
LPCCH alogs::logfile() {
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

void alogs::log(loglevel level, std::string&& str) {
	if (getlevel() > level) {
		return;
	}

	auto f = [&](std::ostream& out) {
		if (!g_basiclog) {
			std::tm tm = localtime_xp(std::time(nullptr));
			out
				<< "[" << std::put_time(&tm, "%H:%M:%S") << "]"
				<< '[' << name(level) << "] "
				;
		}
		out
			<< str
			<< "\n";
		};

	auto* lf = logfile();
	if (lf) {
		std::ofstream out{ lf, std::ios::app };

		f(out);

		out.close();
	}
	else {
		f(level < LVL_WARNING ? std::cout : std::cerr);
	}
}
