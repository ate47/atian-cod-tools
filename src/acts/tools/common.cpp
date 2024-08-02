#include <includes.hpp>

namespace {
	int commonpooltool(Process& _, int argc, const char* argv[]) {
		const char* tools[]{
			"dpbo4",
			"dpcw",
			"dpt7",
			"dpcord",
		};

		for (const char* name : tools) {
			const tool::toolfunctiondata& tool = tool::findtool(name);

			if (!tool || !tool.m_game) {
				LOG_WARNING("Invalid tool config: {}", name);
				continue;
			}

			Process proc{ tool.m_game };

			if (!proc) continue;

			LOG_INFO("Find process {} {}", utils::WStrToStr(tool.m_game), proc);

			if (!proc.Open()) {
				LOG_ERROR("Can't open game process: 0x{:x}", GetLastError());
				return -1;
			}

			return tool.m_func(proc, argc, argv);
		}

		LOG_ERROR("Can't find any supported game");
		LOG_ERROR("Available games:");
		for (const char* name : tools) {
			const tool::toolfunctiondata& tool = tool::findtool(name);

			if (!tool || !tool.m_game) {
				continue;
			}
			LOG_ERROR("- {} : {} ({})", utils::WStrToStr(tool.m_game), tool.m_description, tool.m_category);
		}

		return tool::BASIC_ERROR;
	}

}


ADD_TOOL("dp", "common", " [pool]+", "dump pool", nullptr, commonpooltool);