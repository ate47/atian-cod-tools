#include <includes.hpp>

namespace {

	struct LUIElementFunction
	{
		uintptr_t name;
		uintptr_t func;
		uintptr_t next;
	};


	int dump_luifuncs(const Process& proc, int argc, const char* argv[]) {

		LUIElementFunction func{0, 0, 0};
		
		func.next = proc.ReadMemory<uintptr_t>(proc[0x12D263F0]);

		std::ofstream out{"luifuncs.csv"};

		if (!out) {
			std::cerr << "Can't open output\n";
			return tool::BASIC_ERROR;
		}

		out << "id,name,func";

		CHAR outBuffer[0x300];

		int count{};

		while (func.next) {
			if (!proc.ReadMemory(&func, func.next, sizeof(func))) {
				out.close();
				std::cerr << "Error when reading next function\n";
				return tool::BASIC_ERROR;
			}

			if (proc.ReadString(outBuffer, func.name, sizeof(outBuffer)) < 0) {
				out.close();
				std::cerr << "Error when reading function name\n";
				return tool::BASIC_ERROR;
			}

			out << "\n" << std::dec << (int)count << "," << outBuffer << ",";

			proc.WriteLocation(out, func.func);
			count++;
		}
		std::cout << "Dumped " << std::dec << (int)count << " func(s)\n";
		out.close();

		return tool::OK;
	}


}

ADD_TOOL("dluif", "", "dump lui cfunc", true, dump_luifuncs);