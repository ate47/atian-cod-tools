#include "logging.hpp"

namespace actslib::logging{
	namespace {
		struct LogInfo {
			LoggingLevel lvl{ LoggingLevel::LEVEL_INFO };
			const char* file{};
			bool basic;
		};

		LogInfo nfo{};
	}

	const char* LoggingLevelName(LoggingLevel lvl) {
		switch (lvl) {
		case actslib::logging::LEVEL_TRACE:   return "TRACE";
		case actslib::logging::LEVEL_DEBUG:   return "DEBUG";
		case actslib::logging::LEVEL_INFO:    return "INFO.";
		case actslib::logging::LEVEL_WARNING: return "WARN.";
		case actslib::logging::LEVEL_ERROR:   return "ERROR";
		default: return "unk";
		}
	}

	void SetLevel(LoggingLevel lvl) {
		nfo.lvl = lvl;
	}
	
	LoggingLevel GetLevel() {
		return nfo.lvl;
	}

	void SetBasicLog(bool basiclog) {
		nfo.basic = basiclog;
	}

	void SetLogFile(const char* filename) {
		nfo.file = filename;
	}
	
	const char* GetLogFile() {
		return nfo.file;
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

	void Log(LoggingLevel level, const std::string&& str) {
		if (nfo.lvl > level) {
			return;
		}

		auto f = [&](std::ostream& out) {
			if (!nfo.basic) {
				std::tm tm = localtime_xp(std::time(nullptr));
				out
					<< "[" << std::put_time(&tm, "%H:%M:%S") << "]"
					<< '[' << LoggingLevelName(level) << "] "
					;
			}
			out
				<< str
				<< "\n";
		};

		const char* lf = nfo.file;
		if (lf) {
			std::ofstream out{ lf, std::ios::app };

			f(out);

			out.close();
		}
		else {
			f(level < LEVEL_WARNING ? std::cout : std::cerr);
		}

	}
}