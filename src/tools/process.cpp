#include <includes.hpp>

int PTSearch(Process& proc, int argc, const char* argv[]) {
	using namespace tool;
	if (argc <= 4) {
		for (const auto& mod : proc.modules()) {
			std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";
		}
		return OK;
	}

	auto& mod = proc[argv[4]];

	if (!mod) {
		std::cerr << "Can't find module '" << argv[4] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";

	mod.ComputeExports();

	if (argc <= 5) {
		for (const auto& exp : mod.exports()) {
			std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";
		}
		return OK;
	}

	auto& exp = mod[argv[5]];

	if (!exp) {
		std::cerr << "Can't find export '" << argv[5] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";

	return OK;
}

int PTAlloc(Process& proc, int argc, const char* argv[]) {
	using namespace tool;

	CHAR helloWorld[200] = "hello world";

	auto ptr = proc.AllocateMemory(200);

	if (!ptr) {
		std::cerr << "Can't allocate memory\n";
		return BASIC_ERROR;
	}

	if (!proc.WriteMemory(ptr, helloWorld, 200)) {
		std::cerr << "Can't write memory\n";
		proc.FreeMemory(ptr, 200);
		return BASIC_ERROR;
	}

	CHAR hello2[200] = { 0 };

	if (!proc.ReadMemory(&hello2[0], ptr, 200)) {
		std::cerr << "Can't read memory\n";
		proc.FreeMemory(ptr, 200);
		return BASIC_ERROR;
	}

	std::cout << "local:" << helloWorld << "\n";
	std::cout << "remote:" << hello2 << "\n";

	proc.FreeMemory(ptr, 200);

	std::cout << "ok\n";
	return OK;
}

int processtool(const Process& unused, int argc, const char* argv[]) {
	if (argc <= 3) {
		std::cerr << "Missing process name and tool\n";
		return tool::BAD_USAGE;
	}

	Process proc{ argv[2] };

	if (!proc) {
		std::cerr << "Can't find proc '" << argv[2] << "'\n";
		return tool::BASIC_ERROR;
	}

	std::cout << argv[2] << ": " << std::dec << proc.m_pid << "\n";

	int (*func)(Process & proc, int argc, const char* argv[]) = nullptr;

	if (!_strcmpi("s", argv[3])) {
		func = PTSearch;
	}
	else if (!_strcmpi("hello", argv[3])) {
		func = PTAlloc;
	}

	if (!func) {
		std::cerr << "Bad function: " << argv[3] << "\n";
		return tool::BAD_USAGE;
	}

	proc.Open();

	return func(proc, argc, argv);
}

ADD_TOOL("proc", " (process) (s) [module] [function]", "process explorer", false, processtool);
