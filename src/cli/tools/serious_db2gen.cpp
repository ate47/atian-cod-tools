#include <includes.hpp>

namespace {
	using namespace tool::gsc::opcode;

	struct BuildDbGen {
		Platform platform;
		VmInfo* vm;
	};

	int builddb(Process& proc, int argc, const char* argv[]) {
		if (argc < 4 || (argc % 2) != 0) {
			return tool::BAD_USAGE;
		}


		std::map<BYTE, BuildDbGen> asked{};

		for (int i = 2; i < argc; i += 2) {
			auto plt = PlatformOf(argv[i]);

			if (plt == PLATFORM_UNKNOWN) {
				std::cerr << "Bad platform: " << argv[i] << "\n";
				return tool::BASIC_ERROR;
			}

			BYTE vm = (BYTE)std::strtol(argv[i + 1], nullptr, 16);

			VmInfo* nfo;

			if (!IsValidVm(vm, nfo)) {
				std::cerr << "Bad VM: " << argv[i + 1] << "\n";
				return tool::BASIC_ERROR;
			}

			if (asked.find(nfo->vm) != asked.end()) {

				std::cerr << "Can't register twice a VM, VM " << nfo->name << " (0x" << std::hex << (int)nfo->vm << ") asked twitce!\n";
				return tool::BASIC_ERROR;
			}

			asked[nfo->vm] = {
				.platform = plt,
				.vm = nfo
			};
		}

		std::cout << "Building db for " << asked.size() << " vm(s)...\n";


		std::ofstream vmfile{ compatibility::serious::VM_CODES_DB, std::ios::binary };

		if (!vmfile) {
			std::cerr << "Can't open file " << compatibility::serious::VM_CODES_DB << "\n";
			return tool::BASIC_ERROR;
		}

		BYTE buffer[0x1004] = {};

		for (const auto& [vm, bld] : asked) {
			// clear buffer
			memset(buffer, compatibility::serious::SERID_Invalid, sizeof(buffer));


			// header
			buffer[0] = vm;
			*reinterpret_cast<UINT16*>(&buffer[2]) = 0x1000;

			// build mapping
			for (size_t i = 0; i < 0x1000; i++) {
				auto* loc = LookupOpCode(vm, bld.platform, (UINT16) i);

				auto mappedId = compatibility::serious::ConvertTo(loc->m_id);

				if (mappedId == compatibility::serious::SERID_Invalid) {
					continue;
				}

				buffer[4 + i] = (BYTE) mappedId;
			}

			// write mapping
			vmfile.write(reinterpret_cast<LPCCH>(buffer), sizeof(buffer));
		}

		vmfile.close();
		std::cout << "DB created into " << compatibility::serious::VM_CODES_DB << " .\n";


		return tool::OK;
	}

	int dumpdb(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LPVOID buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, size, false)) {
			std::cerr << "Can't read file " << argv[2] << "\n";
			return tool::BASIC_ERROR;
		}

		auto* db = reinterpret_cast<BYTE*>(buffer);

		size_t real = size;
		VmInfo* nfo{};

		while (real) {
			if (real < 4) {
				std::cerr << "Bad chunk size: " << std::hex << real << "\n";
				break;
			}
			auto vm = *db;
			auto platform = db[1];
			auto size = *reinterpret_cast<UINT16*>(db + 2);

			real -= 4;
			db += 4;

			if (!tool::gsc::opcode::IsValidVm(vm, nfo)) {
				std::cerr << "Invalid vm: " << std::hex << (int)vm << ", plt: " << (int) platform << ", size: " << size << "\n";

				if (real < size) {
					std::cerr << "Bad chunk size: " << std::hex << real << "\n";
					break;
				}
				db += size;
				real -= size;
				continue;
			}



			std::cout << "Vm: " << nfo->name << "(" << std::hex << (int)nfo->vm << "), plt: " << (int) platform << ", size: " << size << "\n";

			std::map<BYTE, std::vector<UINT16>> opcodes{};

			if (real < size) {
				std::cerr << "Bad chunk size: " << std::hex << real << "\n";
				break;
			}

			for (size_t i = 0; i < size; i++) {
				auto op = *(db++);

				if (op == 0xFF) continue;

				opcodes[op].emplace_back((UINT16)i);
			}

			for (const auto& [val, mapping] : opcodes) {
				auto* code = tool::gsc::opcode::LookupOpCode(nfo->vm, PLATFORM_PC, mapping[0]);
				std::cout << "0x" << std::hex << (int)val << "/" << std::dec << (int)val << " (" << code->m_name << ") -> ";

				for (size_t i = 0; i < mapping.size(); i++) {
					if (i) {
						std::cout << ", ";
					}
					std::cout << std::hex << "0x" << mapping[i];
				}
				std::cout << "\n";
			}
			std::cout << "\n";

			real -= size;
		}
		
		std::free(buffer);


		return tool::OK;
	}

	int remapdb(Process& proc, int argc, const char* argv[]) {
		if (argc < 7) {
			return tool::BAD_USAGE;
		}

		auto dbfile = argv[2];
		auto vm = std::strtol(argv[3], nullptr, 16);
		auto plt = std::strtol(argv[4], nullptr, 16);
		auto oldop = std::strtol(argv[5], nullptr, 10);
		auto newop = std::strtol(argv[6], nullptr, 10);

		LPVOID buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(dbfile, buffer, size, false)) {
			std::cerr << "Can't read file " << dbfile << "\n";
			return tool::BASIC_ERROR;
		}

		size_t real = size;
		auto* db = reinterpret_cast<BYTE*>(buffer);

		bool patched = false;
		while (real) {
			if (real < 4) {
				std::cerr << "Bad chunk size: " << std::hex << real << "\n";
				break;
			}
			auto vmval = *db;
			auto platform = db[1];
			auto size = *reinterpret_cast<UINT16*>(db + 2);

			real -= 4;
			db += 4;

			if (real < size) {
				std::cerr << "Bad chunk block size: " << std::hex << real << "\n";
				break;
			}

			if (vmval == vm && platform == plt) {
				for (size_t i = 0; i < size; i++) {
					if (db[i] == oldop) {
						db[i] = (byte)newop;
						patched = true;
					}
				}
			}

			db += size;
			real -= size;
		}

		if (patched) {
			if (!utils::WriteFile(dbfile, buffer, size)) {
				std::cerr << "Error when writing file\n";
			}
			else {
				std::cout << "Updated\n";
			}
		}
		else {
			std::cout << "Done\n";
		}

		std::free(buffer);

		return tool::OK;
	}

	ADD_TOOL("serious_db", " ([type] [vm])+", "Create a t7compiler database", nullptr, builddb);
	ADD_TOOL("serious_dbd", " [database]", "Dump a t7compiler database", nullptr, dumpdb);
	ADD_TOOL("serious_remap", " [database] [vm] [plt] [old] [new]", "Remap an opcode in a vm", nullptr, remapdb);
}

