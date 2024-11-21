#pragma once
#include <includes_shared.hpp>

namespace alogs {

	enum loglevel {
		LVL_TRACE_PATH = -1,
		LVL_TRACE = 0,
		LVL_DEBUG = 1,
		LVL_INFO = 2,
		LVL_WARNING = 3,
		LVL_ERROR = 4,
	};

	const char* name(loglevel lvl);
	void setlevel(loglevel lvl);
	loglevel getlevel();
	void setbasiclog(bool basiclog);

	void setfile(const char* filename);
	const char* logfile();

	void addoutstream(std::ostream* outStream);

	void log(loglevel level, 

#ifndef CI_BUILD
		const char* file, size_t line,
#endif
		const std::string& str);
}

#ifndef CI_BUILD
#define LOG_LVL(LEVEL, msg) if (alogs::getlevel() <= LEVEL) alogs::log(LEVEL, __FILE__, __LINE__, msg)
#else
#define LOG_LVL(LEVEL, msg) if (alogs::getlevel() <= LEVEL) alogs::log(LEVEL, msg)
#endif
#define LOG_LVLF(LEVEL, ...) LOG_LVL(LEVEL, std::format(__VA_ARGS__))
#define LOG_TRACE(...) LOG_LVLF(alogs::loglevel::LVL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LVLF(alogs::loglevel::LVL_DEBUG, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LVLF(alogs::loglevel::LVL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG_LVLF(alogs::loglevel::LVL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG_LVLF(alogs::loglevel::LVL_INFO, __VA_ARGS__)