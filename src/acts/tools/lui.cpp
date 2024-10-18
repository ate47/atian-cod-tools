#include <includes.hpp>

namespace {

	struct LUIElementFunction
	{
		uintptr_t name;
		uintptr_t func;
		uintptr_t next;
	};


	int dump_luifuncs(Process& proc, int argc, const char* argv[]) {

		LUIElementFunction func{0, 0, 0};
		
		func.next = proc.ReadMemory<uintptr_t>(proc[0x12D263F0]);

		std::ofstream out{"luifuncs.csv"};

		if (!out) {
			LOG_ERROR("Can't open output");
			return tool::BASIC_ERROR;
		}

		out << "id,name,func";

		char outBuffer[0x300];

		int count{};

		while (func.next) {
			if (!proc.ReadMemory(&func, func.next, sizeof(func))) {
				out.close();
				LOG_ERROR("Error when reading next function");
				return tool::BASIC_ERROR;
			}

			if (proc.ReadString(outBuffer, func.name, sizeof(outBuffer)) < 0) {
				out.close();
				LOG_ERROR("Error when reading function name");
				return tool::BASIC_ERROR;
			}

			out << "\n" << std::dec << count << "," << outBuffer << ",";

			proc.WriteLocation(out, func.func);
			count++;
		}
		LOG_INFO("Dumped {}, func(s)", count);
		out.close();

		return tool::OK;
	}


}

ADD_TOOL(dluif, "bo4", "", "dump lui cfunc", L"BlackOps4.exe", dump_luifuncs);