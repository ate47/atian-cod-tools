#include <includes.hpp>

namespace {
	using namespace tool::gsc::opcode;

	struct BuildDbGen {
		Platform platform;
		VmInfo* vm;
	};

	int builddb(const Process& proc, int argc, const char* argv[]) {
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

	ADD_TOOL("serious_db", " ([type] [vm])+", "Create a t7compiler database", false, builddb);
}

