#include "../../../mw23-dll/gsctools.hpp"
#include <includes.hpp>

namespace {


	int gsctooltest(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}
		std::string buff{};

		if (!utils::ReadFile(argv[2], buff)) {
			LOG_ERROR("Can't read file {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		mwiii::gsc::GscTool tool{ buff.data() };

		if (tool) {
			tool.ClearDevBlock(tool.FindFunction("ob", 0xfcc474b48416cd3c), 0x1c8);

			tool.NullFunction(tool.FindFunction("ob", 0x36474fc103a73cce));
		}



		return tool::OK;
	}

#ifndef CI_BUILD
	ADD_TOOL("gsctooltest", " [gsc file]", "", nullptr, gsctooltest);
#endif
}