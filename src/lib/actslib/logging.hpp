#pragma once
#include "actslib.hpp"

namespace actslib::logging {
	enum LoggingLevel {
		LEVEL_TRACE = 0,
		LEVEL_DEBUG = 1,
		LEVEL_INFO = 2,
		LEVEL_WARNING = 3,
		LEVEL_ERROR = 4,
		LEVEL_COUNT
	};

	const char* LoggingLevelName(LoggingLevel lvl);
	void SetLevel(LoggingLevel lvl);
	LoggingLevel GetLevel();
	void SetBasicLog(bool basiclog);

	void SetLogFile(const char* filename);
	const char* GetLogFile();

	void Log(LoggingLevel level, std::string&& str);

}
#define ALOG_LVL(LEVEL, ...) if (actslib::logging::GetLevel() <= LEVEL) actslib::logging::Log(LEVEL, std::format(__VA_ARGS__))

#ifdef DEBUG
#  define ALOG_IFTRACE (actslib::logging::GetLevel() <= actslib::logging::LoggingLevel::LEVEL_TRACE)
#  define ALOG_TRACE(...) ALOG_LVL(actslib::logging::LoggingLevel::LEVEL_TRACE, __VA_ARGS__)
#else
#  define ALOG_IFTRACE (false)
#  define ALOG_TRACE(...) 
#endif
#define ALOG_DEBUG(...) ALOG_LVL(actslib::logging::LoggingLevel::LEVEL_DEBUG, __VA_ARGS__)
#define ALOG_ERROR(...) ALOG_LVL(actslib::logging::LoggingLevel::LEVEL_ERROR, __VA_ARGS__)
#define ALOG_WARNING(...) ALOG_LVL(actslib::logging::LoggingLevel::LEVEL_WARNING, __VA_ARGS__)
#define ALOG_INFO(...) ALOG_LVL(actslib::logging::LoggingLevel::LEVEL_INFO, __VA_ARGS__)
