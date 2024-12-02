#pragma once

namespace tool {

	class GPLTool {
	public:
		GPLTool(const char* name, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[]));
	};

}
#define ADD_TOOL(id, category, usage, desc, func) static tool::GPLTool __toolgplfunctiondata_##id(#id, category, usage, desc, func)