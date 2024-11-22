#include <includes_shared.hpp>
#include <core/async.hpp>
#include "utils.hpp"
#include "logs.hpp"
#include "clicolor.hpp"

namespace alogs {
	namespace {
		alogs::loglevel g_loglevel = alogs::LVL_INFO;
		const char* g_logfile{};
		bool g_basiclog{};
		std::ostream* g_outStream{};

		std::vector<std::string> paths{};
	}

	std::mutex* getasyncmutex() {
		if (core::async::IsSync(core::async::AT_LOGS)) {
			static std::mutex mtx{};
			return &mtx;
		}
		return nullptr;
	}
	void addlogpath(const std::string& path) {
		size_t start{};
		while (start < path.size()) {
			size_t idx{ path.find(';', start) };
			if (idx == std::string::npos) {
				idx = path.size();
			}

			paths.push_back(path.substr(start, idx));

			start = idx + 1;
		}
	}
	void cleanuplogpaths() {
		paths.clear();
	}

	const char* name(loglevel lvl) {
		switch (lvl) {
		case alogs::LVL_TRACE:
		case alogs::LVL_TRACE_PATH:
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

	void setlevel(loglevel lvl) {
		g_loglevel = lvl;
	}
	loglevel getlevel() {
		return g_loglevel;
	}
	void setbasiclog(bool basiclog) {
		g_basiclog = basiclog;
	}

	void setfile(const char* filename) {
		g_logfile = filename;
	}
	void addoutstream(std::ostream* outStream) {
		g_outStream = outStream;
	}
	const char* logfile() {
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

	void log(loglevel level, const char* file, size_t line, const std::string& str) {
		if (getlevel() > level) {
			return;
		}

		if (paths.size()) {
			// check if the file can be handled
			std::string_view fsw{ file };
			bool match{};
			for (const std::string& p : paths) {
				if (fsw.starts_with(p)) {
					match = true;
					break;
				}
			}
			if (!match) return; // not matching our pattern
		}

		auto f = [&](std::ostream& out, bool color) {
			bool locked{};
			core::async::opt_lock_guard lg{ getasyncmutex() };
			if (!g_basiclog) {
				if (color) {
					out << clicolor::Color(5, 5, 5);
				}
				std::tm tm = localtime_xp(std::time(nullptr));
				out
					<< "[" << std::put_time(&tm, "%H:%M:%S") << "]"
					;
			}
			if (color) {
				switch (level) {
				case alogs::LVL_ERROR: out << clicolor::Color(5, 1, 1); break;
				case alogs::LVL_WARNING: out << clicolor::Color(5, 4, 1); break;
				case alogs::LVL_INFO: out << clicolor::Color(5, 5, 5); break;
				case alogs::LVL_DEBUG: out << clicolor::Color(4, 5, 1); break;
				case alogs::LVL_TRACE_PATH:
				case alogs::LVL_TRACE: out << clicolor::Color(1, 5, 5); break;
				default: out << clicolor::Reset(); break;
				}
			}

			if (!g_basiclog) {
				out << '[' << name(level) << ']';
				if (g_loglevel == LVL_TRACE_PATH) {
					out << '[' << file << '@' << std::dec << line << ']';
				}
			}
			out << " " << str;

			if (color) {
				out << clicolor::Reset();
			}
			out << "\n";
			};

		auto* lf = logfile();
		if (lf) {
			std::ofstream out{ lf, std::ios::app };

			f(out, false);

			out.close();
		}
		else {
			static bool allowColor = clicolor::ConsoleAllowColor();

			f(level < LVL_WARNING ? std::cout : std::cerr, allowColor);
		}
		if (g_outStream) {
			f(*g_outStream, false);
		}
	}
}