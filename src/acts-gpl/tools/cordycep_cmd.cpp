#include <includes_gpl.hpp>
#include <CommandParser.h>
#include <ProcessDumper.h>


namespace {
	int cord_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		ps::ProcessDumper dumper;

		if (!dumper.Dump(argv[2])) {
			LOG_ERROR("Failed to dump {}", argv[2]);
			return tool::BASIC_ERROR;
		}


		LOG_INFO("Dumped {}", argv[2]);

		return tool::OK;
	}

	ADD_TOOL(cord_dump, "common", " [cmd] (params)", "Cordycep command handler", cord_dump);
}
