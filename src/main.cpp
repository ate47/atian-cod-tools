#include <includes.hpp>

struct toolfunctiondata {
	LPCCH usage;
	LPCCH description;
	bool needGame;
	tool::toolfunction func;
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
	g_tools["str"] = { "", "check collisions in the string file", false, tool::hash::collisiontool };
	g_tools["dps"] = { " [output=pool.csv]", "dump pooled scripts", true, tool::dump::poolscripts };
	g_tools["wps"] = { " [output=scriptparsetree]", "write pooled scripts", true, tool::dump::writepoolscripts };
	g_tools["gscinfo"] = { " (intput)*", "write info about a script in asm file", false, tool::gsc::gscinfo };
	g_tools["dls"] = { " [output=linked.csv]", "dump linked scripts", true, tool::dump::linkedscripts };
	g_tools["dfunc"] = { " [output=funcs.csv]", "dump functions", true, tool::dump::dumpfunctions };
	g_tools["devents"] = { " [output=events.csv]", "dump registered instance events", true, tool::dump::events };
	g_tools["dcfunc"] = { " [output=cfuncs.csv]", "dump cmd functions", true, tool::dump::dumpcmdfunctions };
	g_tools["dds"] = { " [input=scriptparsetree] [output=dataset.csv]", "dump dataset from gscinfo", false, tool::gsc::dumpdataset };
	g_tools["dp"] = { " [input=pool_name] (output=pool_id)", "dump pool", true, tool::pool::pooltool };
#ifdef DEBUG
	g_tools["compiler"] = { " --help", "gsc compiler", false, gsc::compiler::compiler };
#endif
	g_tools["dbg"] = { "", "vm debuger", true, tool::vm_debug::vmdebug };
	g_tools["proc"] = { " (process) [module] [function]", "process explorer", false, tool::process::processtool };
	

	std::cout << "Atian Call of Duty tools\n\n";
	
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
	Process proc(app);

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

	int output = tool->second.func(proc, argc, argv);

	if (output == tool::BAD_USAGE) {
		char buff[2000];
		snprintf(buff, 2000, "Bad tool usage: %s %s%s", *argv, argv[1], tool->second.usage);
		usage(buff, *argv);
		return -1;
	}
}