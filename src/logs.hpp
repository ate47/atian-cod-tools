#pragma once
#include <includes.hpp>

namespace alogs {

	enum loglevel {
		LVL_TRACE,
		LVL_ERROR,
		LVL_WARNING,
		LVL_INFO,
	};

	LPCCH name(loglevel lvl);
	void setlevel(loglevel lvl);
	loglevel getlevel();

	std::ostream& log(loglevel level);

}

#define LOG_TRACE if (alogs::getlevel() > alogs::loglevel::LVL_TRACE) alogs::log(alogs::loglevel::LVL_TRACE) 
#define LOG_ERROR if (alogs::getlevel() > alogs::loglevel::LVL_ERROR) alogs::log(alogs::loglevel::LVL_ERROR) 
#define LOG_WARNING if (alogs::getlevel() > alogs::loglevel::LVL_WARNING) alogs::log(alogs::loglevel::LVL_WARNING) 
#define LOG_INFO if (alogs::getlevel() > alogs::loglevel::LVL_INFO) alogs::log(alogs::loglevel::LVL_INFO) 