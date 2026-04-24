#ifndef __ACTS_API_LOGS_H__
#define __ACTS_API_LOGS_H__
#include "api.h"
/*
 * Atian Tools Logs API
 */

 /* log level */
typedef enum {
	LVL_TRACE_PATH = -1,
	LVL_TRACE = 0,
	LVL_DEBUG = 1,
	LVL_INFO = 2,
	LVL_WARNING = 3,
	LVL_ERROR = 4,
} ActsAPILog_Level;

ACTS_COMMON_API ActsAPILog_Level ActsAPILog_GetLevel();
ACTS_COMMON_API void ActsAPILog_SetLevel(ActsAPILog_Level level);

/*
 * Log a message
 * @param level log level
 * @param file source file, can be nullptr
 * @param line source line number
 * @param str log message
 * @param endl whether to append a newline at the end
 */
ACTS_COMMON_API void ActsAPILog_Log(ActsAPILog_Level level, const char* file, size_t line, const char* str, bool ACTS_DEFAULT(endl, true));

/*
 * Log a message with a custom header
 * @param level log level
 * @param header log header, will be printed before the file and line info
 * @param file source file, can be nullptr
 * @param line source line number
 * @param str log message
 * @param endl whether to append a newline at the end
 */
ACTS_COMMON_API void ActsAPILog_Log2(ActsAPILog_Level level, const char* header, const char* file, size_t line, const char* str, bool ACTS_DEFAULT(endl, true));


/*
 * Log a message
 * @param level log level
 * @param file source file, can be nullptr
 * @param line source line number
 * @param endl whether to append a newline at the end
 * @param fmt log message format, like printf
 * @param ... variadic arguments for the log message, like printf
 */
ACTS_COMMON_API void ActsAPILog_Logf(ActsAPILog_Level level, const char* file, size_t line, bool endl, const char* fmt, ...);

/*
 * Log a message with a custom header
 * @param level log level
 * @param header log header, will be printed before the file and line info
 * @param file source file, can be nullptr
 * @param line source line number
 * @param endl whether to append a newline at the end
 * @param fmt log message format, like printf
 * @param ... variadic arguments for the log message, like printf
 */
ACTS_COMMON_API void ActsAPILog_Log2f(ActsAPILog_Level level, const char* header, const char* file, size_t line, bool endl, const char* fmt, ...);


#define ACTSLOG_LVL(LEVEL, MSG) do { \
	if ((LEVEL) >= ActsAPILog_GetLevel()) { \
		ActsAPILog_Log(LEVEL, __FILE__, __LINE__, MSG, true); \
	} \
} while(0)
#define ACTSLOG_LVLF(LEVEL, ...) do { \
	if ((LEVEL) >= ActsAPILog_GetLevel()) { \
		ActsAPILog_Logf(LEVEL, __FILE__, __LINE__, true, __VA_ARGS__); \
	} \
} while(0)


// logs trace message
#define ACTSLOG_TRACE(MSG) ACTSLOG_LVL(LVL_TRACE, MSG)
// logs debug message
#define ACTSLOG_DEBUG(MSG) ACTSLOG_LVL(LVL_DEBUG, MSG)
// logs info message
#define ACTSLOG_INFO(MSG) ACTSLOG_LVL(LVL_INFO, MSG)
// logs warning message
#define ACTSLOG_WARNING(MSG) ACTSLOG_LVL(LVL_WARNING, MSG)
// logs error message
#define ACTSLOG_ERROR(MSG) ACTSLOG_LVL(LVL_ERROR, MSG)

// logs formatted trace message
#define ACTSLOG_TRACEF(...) ACTSLOG_LVLF(LVL_TRACE, __VA_ARGS__)
// logs formatted debug message
#define ACTSLOG_DEBUGF(...) ACTSLOG_LVLF(LVL_DEBUG, __VA_ARGS__)
// logs formatted info message
#define ACTSLOG_INFOF(...) ACTSLOG_LVLF(LVL_INFO, __VA_ARGS__)
// logs formatted warning message
#define ACTSLOG_WARNINGF(...) ACTSLOG_LVLF(LVL_WARNING, __VA_ARGS__)
// logs formatted error message
#define ACTSLOG_ERRORF(...) ACTSLOG_LVLF(LVL_ERROR, __VA_ARGS__)


#endif