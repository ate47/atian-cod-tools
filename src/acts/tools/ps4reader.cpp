#include <includes.hpp>

namespace {
	int ps4reader(Process& _, int argc, const char* argv[]) {
		if (argc < 5) {
			return tool::BAD_USAGE;
		}

		auto ipd = argv[2];
		auto loc = argv[3];
		auto size = std::strtoull(argv[4], nullptr, 16);

		if (size >= (1ull << ((sizeof(uint32_t) << 3) - 1))) {
			std::cerr << "Size too large: " << std::hex << size << "/" << (1ull << ((sizeof(uint32_t) << 3) - 1)) << "\n";
			return tool::BASIC_ERROR;
		}

		libdebug::PS4DBG ps4{ ipd };
		ps4.Connect();

		ps4.Notify(210, std::format("Atian Tools {}", actsinfo::VERSION));

		auto procList = ps4.GetProcessList();
		auto proc = procList.FindProcess("eboot.bin");

		if (!proc) {
			std::cerr << "Can't find eboot.bin\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		auto pid = proc->pid;

		auto entries = ps4.GetProcessMaps(proc->pid);
		uint64_t base = 0;

		for (const auto& entry : entries.entries) {
			if (entry->prot == 5) {
				std::cout << "executable base : 0x" << std::hex << entry->start << std::endl;
				base = entry->start;
				break;
			}
		}

		if (!base) {
			std::cerr << "Can't find executable base\n";
			ps4.Disconnect();
			return tool::BASIC_ERROR;
		}

		uintptr_t locData;

		if (*loc == '+') {
			locData = base + std::strtoull(loc + 1, nullptr, 16);
		}
		else {
			locData = std::strtoull(loc, nullptr, 16);
		}

		auto bytes = ps4.ReadMemory(pid, locData, (int)size);

		auto i = 0;
		for (const auto b : bytes) {
			std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
			i++;
			if (i % 16 == 0) {
				std::cout << "\n";
			}
			else if (i % 8 == 0) {
				std::cout << "  ";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << "\n";


		ps4.Disconnect();



		return tool::OK;
	}
}


#ifndef CI_BUILD

ADD_TOOL("ps4r", " [ip:port] [loc] [size]", "ps4reader", nullptr, ps4reader);

#endif