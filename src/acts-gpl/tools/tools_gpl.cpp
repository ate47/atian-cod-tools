#include <includes_gpl.hpp>
#include <acts.hpp>


namespace tool {
	GPLTool::GPLTool(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[])) {
		RegisterActsTool(name, filename, line, category, usage, description, func);
	}
}