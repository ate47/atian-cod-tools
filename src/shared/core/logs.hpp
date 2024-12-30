#pragma once
#include <includes_shared.hpp>

namespace core::logs {

	enum loglevel {
		LVL_TRACE_PATH = -1,
		LVL_TRACE = 0,
		LVL_DEBUG = 1,
		LVL_INFO = 2,
		LVL_WARNING = 3,
		LVL_ERROR = 4,
	};


	constexpr size_t GetLogFileSplit(const char* line) {
		std::string_view sw{ line };

		size_t idx{ sw.rfind("/src/") };
		if (idx == std::string::npos) {
			idx = sw.rfind("\\src\\");

			if (idx == std::string::npos) {
				return 0;
			}
		}

		return idx + 5;
	}

	constexpr size_t GetLogFileLen(const char* line) {
		size_t l{ 0 };
		while (*line++) l++;
		return l;
	}

	constexpr size_t GetLogFileExt(const char* line) {
		std::string_view sw{ line };

		size_t s = sw.rfind('.');

		if (s == std::string::npos || s < GetLogFileSplit(line)) {
			return 0;
		}

		return sw.length() - s;
	}

	template<size_t len, size_t split, size_t ext>
	constexpr std::array<char, len - split + 1 - ext> GetLogFile(const char* line) {
		std::array<char, len - split + 1 - ext> a{};

		for (size_t i = split; i < len - ext; i++) {
			char c;
			if (line[i] == '\\' || line[i] == '/') {
				c = ':';
			}
			else {
				c = line[i];
			}
			a[i - split] = c;
		}

		return a;
	}

	const char* name(loglevel lvl);
	void setlevel(loglevel lvl);
	loglevel getlevel();
	void setbasiclog(bool basiclog);
	void addlogpath(const std::string& path);
	void cleanuplogpaths();

	void setfile(const char* filename);
	const char* logfile();

	void addoutstream(std::ostream* outStream);

	void log(loglevel level, const char* header, const char* file, size_t line, const char* str);
	void log(loglevel level, const char* file, size_t line, const char* str);
	inline void log(loglevel level, const char* header, const char* file, size_t line, const std::string& str) {
		log(level, header, file, line, str.c_str());
	}
	inline void log(loglevel level, const char* file, size_t line, const std::string& str) {
		log(level, file, line, str.c_str());
	}
}


// convert filename to log id
#define LOG_GET_LOG_REF_STR_DATA (core::logs::GetLogFile<core::logs::GetLogFileLen(__FILE__), core::logs::GetLogFileSplit(__FILE__), core::logs::GetLogFileExt(__FILE__)>(__FILE__))
#define LOG_GET_LOG_REF_STR (LOG_GET_LOG_REF_STR_DATA.data())
#define HAS_LOG_LEVEL(LEVEL) (core::logs::getlevel() <= LEVEL)
#define LOG_LVL(LEVEL, msg) if (HAS_LOG_LEVEL(LEVEL)) { constexpr auto ___ff = LOG_GET_LOG_REF_STR_DATA; core::logs::log(LEVEL, ___ff.data(), __LINE__, msg); }
#define LOG_LVLF(LEVEL, ...) LOG_LVL(LEVEL, std::format(__VA_ARGS__))
#define LOG_TRACE(...) LOG_LVLF(core::logs::loglevel::LVL_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LVLF(core::logs::loglevel::LVL_DEBUG, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LVLF(core::logs::loglevel::LVL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG_LVLF(core::logs::loglevel::LVL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG_LVLF(core::logs::loglevel::LVL_INFO, __VA_ARGS__)