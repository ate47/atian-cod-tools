#include <includes.hpp>

int main(int argc, const char *argv[]) {
	g_progPath = argv[0];

	std::cout << "Atian tools " << actsinfo::VERSION << " \n\n";
	
	if (argc == 1) {
		tool::usage("No input", *argv);
		return -1;
	}

	const auto& tool = tool::findtool(argv[1]);

	if (!tool) {
		tool::usage("Bad tool name", *argv);
		return -1;
	}

	Process proc(tool.m_game);

	if (tool.m_game) {

		if (!proc) {
			std::wcerr << L"Can't find game process " << tool.m_game << L"\n";
			return -1;
		}
		std::wcout << "Find process " << tool.m_game << proc << "\n";

		if (!proc.Open()) {
			std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
			return -1;
		}
	}

	int output = tool.m_func(proc, argc, argv);

	if (output == tool::BAD_USAGE) {
		char buff[2000];
		snprintf(buff, 2000, "Bad tool usage: %s %s%s", *argv, argv[1], tool.m_usage);
		tool::usage(buff, *argv);
		return -1;
	}
}