#include "process.hpp"

int tool::process::processtool(const Process& unused, int argc, const char* argv[]) {
	if (argc <= 2) {
		std::cerr << "Missing process name" << "\n";
		return BAD_USAGE;
	}

	Process proc{ argv[2] };

	if (!proc) {
		std::cerr << "Can't find proc '" << argv[2] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << argv[2] << ": " << std::dec << proc.m_pid << "\n";

	proc.Open();

	if (argc <= 3) {
		for (const auto& mod : proc.modules()) {
			std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";
		}
		return OK;
	}

	auto& mod = proc[argv[3]];

	if (!mod) {
		std::cerr << "Can't find module '" << argv[3] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";

	mod.ComputeExports();

	if (argc <= 4) {
		for (const auto& exp : mod.exports()) {
			std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";
		}
		return OK;
	}

	auto& exp = mod[argv[4]];

	if (!exp) {
		std::cerr << "Can't find export '" << argv[4] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";

	return OK;
}
