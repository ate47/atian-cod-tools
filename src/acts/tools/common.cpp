#include <includes.hpp>

namespace {
	bool CommonToolTest(int argc, const char* argv[], int& ret, const char* name) {
		const tool::toolfunctiondata& tool = tool::findtool(name);

		if (!tool || !tool.m_game) {
			LOG_WARNING("Invalid common tool '{}' config: {}", argv[1], name);
			return false;
		}

		Process proc{ tool.m_game };

		if (!proc) return false;

		LOG_INFO("Find process {} {}", utils::WStrToStr(tool.m_game), proc);

		if (!proc.Open()) {
			LOG_ERROR("Can't open game process: 0x{:x}", GetLastError());
			return false;
		}

		ret = tool.m_func(proc, argc, argv);
		return true;
	}

	void CommonToolDisplay(const char* name) {
		const tool::toolfunctiondata& tool = tool::findtool(name);

		if (!(!tool || !tool.m_game)) {
			LOG_ERROR("- {} : {} ({})", utils::WStrToStr(tool.m_game), tool.m_description, tool.m_category);
		}
	}

	template<typename... Tools>
	inline bool CommonToolTest(int argc, const char* argv[], int& ret, const char* toolName, Tools... tool) {
		return CommonToolTest(argc, argv, ret, toolName) || CommonToolTest(argc, argv, ret, tool...);
	}

	template<typename... Tools>
	inline void CommonToolDisplay(const char* toolName, Tools... tool) {
		CommonToolDisplay(toolName);
		CommonToolDisplay(tool...);
	}

	template<typename... Tools>
	int CommonTool(int argc, const char* argv[], Tools... tools) {
		int ret{};
		if (CommonToolTest(argc, argv, ret, tools...)) {
			return ret;
		}

		LOG_ERROR("Can't find any supported game");
		LOG_ERROR("Available games:");
		CommonToolDisplay(tools...);
		return tool::BASIC_ERROR;
	}

#define REGISTER_COMMON_TOOL(id, category, usage, desc, ...) static tool::toolfunctiondata __toolfunctiondata_##id(#id, LOG_GET_LOG_REF_STR, __LINE__, category, usage, desc, nullptr, \
			[](Process& _, int argc, const char* argv[]) -> int { return CommonTool(argc, argv, __VA_ARGS__); })

	REGISTER_COMMON_TOOL(dp, "common", " [pool]+", "dump pool", "dpbo4", "dpcw", "dpt7", "dpcord");
	REGISTER_COMMON_TOOL(wps, "common", " [output]", "write pooled scripts", "wpsbo4", "wpscw", "wpst7", "wpscod2020");
	REGISTER_COMMON_TOOL(dbgp, "common", "", "dump bg pool", "dbgpbo4", "dbgpcw");
	REGISTER_COMMON_TOOL(dpn, "common", "", "dump pool names", "dpnbo4", "dpncw");
	REGISTER_COMMON_TOOL(dbgcache, "common", "", "dump bg cache", "dbgcachebo4");
	REGISTER_COMMON_TOOL(dbmtstrs, "common", "", "dump mt strings", "dbmtstrsbo4");
	REGISTER_COMMON_TOOL(dbg, "common", "", "vm debugger", "dbgbo4", "dbgcw");
	REGISTER_COMMON_TOOL(dbg_link, "common", "", "debug linking error", "dbg_linkbo4");
}
