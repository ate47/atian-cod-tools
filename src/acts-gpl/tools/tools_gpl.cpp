#include <includes_gpl.hpp>
#include <acts.hpp>


namespace tool {
	GPLTool::GPLTool(const char* name, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[])) {
		RegisterActsTool(name, category, usage, description, func);
	}
}