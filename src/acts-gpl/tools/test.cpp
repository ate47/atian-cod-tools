#include <includes_gpl.hpp>
#include <Log.h>

namespace {
	int gpltest(int argc, const char* argv[]) {
		LOG_INFO("test");

		ps::log::Log(ps::LogType::Normal, "normal log");
		ps::log::Log(ps::LogType::Verbose, "verbose log");
		ps::log::Log(ps::LogType::Error, "error log");
		ps::log::Log(ps::LogType::Debug, "debug log");
		ps::log::Print("Normal", "test log 2 %d", 5);

		return tool::OK;
	}

	ADD_TOOL(gpltest, "dev", "", "shit test", gpltest);
}
