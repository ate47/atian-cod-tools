#pragma once
#include <core/logs.hpp>

namespace tool {

	class GPLTool {
	public:
		GPLTool(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[]));
	};

}
#define ADD_TOOL(id, category, usage, desc, func) static tool::GPLTool __toolgplfunctiondata_##id(#id, LOG_GET_LOG_REF_STR, __LINE__, category, usage, desc, func)