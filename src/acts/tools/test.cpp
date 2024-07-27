#include <includes.hpp>


#ifndef CI_BUILD
#include <jsonrpccxx/server.hpp>

namespace {
	int test(Process& proc, int argc, const char* argv[]) {

		return tool::OK;
	}
}

ADD_TOOL("test", "dev", "", "Tests", nullptr, test);
#endif