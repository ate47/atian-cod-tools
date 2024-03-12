#include <actslib.hpp>
#include <includes.hpp>

namespace {
	int actslibtest(Process& proc, int argc, const char* argv[]) {


		LOG_INFO("Ok");
		return tool::OK;
	}

}

#ifndef CI_BUILD
ADD_TOOL("actslibtest", "", "Acts lib test", nullptr, actslibtest);
#endif