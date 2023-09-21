#pragma once
#include <includes_shared.hpp>

namespace alogs {

	enum loglevel {
		LVL_TRACE = 0,
		LVL_DEBUG = 1,
		LVL_INFO = 2,
		LVL_WARNING = 3,
		LVL_ERROR = 4,
	};

	LPCCH name(loglevel lvl);
	void setlevel(loglevel lvl);
	loglevel getlevel();

	void setfile(LPCCH filename);
	LPCCH logfile();

	void log(loglevel level, std::string&& str);

}

#define LOG_LVL(LEVEL, ...) if (alogs::getlevel() <= LEVEL) alogs::log(LEVEL, std::format(__VA_ARGS__))
#define LOG_TRACE(...) LOG_LVL(alogs::loglevel::LVL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LVL(alogs::loglevel::LVL_DEBUG, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LVL(alogs::loglevel::LVL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG_LVL(alogs::loglevel::LVL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG_LVL(alogs::loglevel::LVL_INFO, __VA_ARGS__)
