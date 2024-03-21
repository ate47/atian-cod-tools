#pragma once
#include "actslib.hpp"

/*
 * Logging utilities for actslib
 */
namespace actslib::logging {
	enum LoggingLevel {
		LEVEL_TRACE = 0,
		LEVEL_DEBUG = 1,
		LEVEL_INFO = 2,
		LEVEL_WARNING = 3,
		LEVEL_ERROR = 4,
		LEVEL_COUNT
	};

	/*
	 * Get the name of a logging level
	 * @param lvl level
	 * @return name
	 */
	const char* LoggingLevelName(LoggingLevel lvl);

	/*
	 * Set the logging level
	 * @param lvl level
	 */
	void SetLevel(LoggingLevel lvl);

	/*
	 * @return Get the logging level
	 */
	LoggingLevel GetLevel();

	/*
	 * Set the basic logs mode, won't display the date and the logging level, only the message
	 * @param basiclog mode
	 */
	void SetBasicLog(bool basiclog);

	/*
	 * Set the output log file
	 * @param filename File to output, null for stdout/stderr
	 */
	void SetLogFile(const char* filename);

	/*
	 * @return Get the output log file, is null for stdout/stderr
	 */
	const char* GetLogFile();

	/*
	 * Log a message
	 * @param level level to log
	 * @param str string to log
	 */
	void Log(LoggingLevel level, const std::string&& str);

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
