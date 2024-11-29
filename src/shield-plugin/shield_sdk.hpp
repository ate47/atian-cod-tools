#pragma once

#ifdef SHIELD_PROXY_DLL
# define SHIELD_SDK __declspec(dllexport)
#else
# define SHIELD_SDK __declspec(dllimport)
#endif

namespace shield_sdk
{
	enum log_type
	{
		LOG_TYPE_DEBUG = 0,
		LOG_TYPE_INFO = 1,
		LOG_TYPE_WARN = 2,
		LOG_TYPE_ERROR = 3,
		LOG_TYPE_CONSOLE = 4
	};

	SHIELD_SDK void log(log_type type, std::string str);
	SHIELD_SDK void log(log_type type, const char* fmt, ...);
}
