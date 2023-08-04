#include "includes.hpp"

struct toolfunctiondata {
	LPCCH usage;
	LPCCH description;
	bool needGame;
	toolfunction func;
};
static std::map<std::string, toolfunctiondata> g_tools;

void usage(LPCCH message, LPCCH argv0) {
	if (message) {
		std::cerr
			<< "Error: " << message << "\n";
	}
	std::cerr 
		<< argv0 << " [function] (params)\n"
		<< "Functions:\n";
	
	for (const auto& [key, value] : g_tools) {
		std::cerr << "- " << key << value.usage << " : " << value.description << "\n";
	}
}

int main(int argc, const char *argv[]) {
	g_tools["lookup"] = { " (string)*", "lookup strings", false, tool::hash::lookuptool };
	g_tools["h32"] = { " (string)*", "hash strings", false, tool::hash::hash32 };
	g_tools["h64"] = { " (string)*", "hash strings", false, tool::hash::hash64 };
	g_tools["dps"] = { " [output=pool.csv]", "dump pooled scripts", true, tool::dump::poolscripts };

	if (argc == 1) {
		usage("No input", *argv);
		return -1;
	}

	auto tool = g_tools.find(argv[1]);

	if (tool == g_tools.end()) {
		usage("Bad tool name", *argv);
		return -1;
	}

	const auto app = L"blackops4.exe";
	process proc(app);

	if (tool->second.needGame) {

		if (!proc) {
			std::wcerr << L"Can't find game process " << app << L"\n";
			return -1;
		}
		std::wcout << std::hex << L"Find process " << app << L":" << proc.m_pid << L"\n";

		if (!proc.Open()) {
			std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
			return -1;
		}
	}

	return tool->second.func(proc, argc, argv);
}