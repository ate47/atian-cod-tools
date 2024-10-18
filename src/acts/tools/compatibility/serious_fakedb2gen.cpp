#include <includes.hpp>
#include "compatibility/serious_db2.hpp"
#include "tools/gsc.hpp"

namespace {
	using namespace tool::gsc::opcode;
	using namespace compatibility::serious::db2;

	int fakedb(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		auto plt = PlatformOf(argv[2]);

		if (plt == PLATFORM_UNKNOWN) {
			LOG_ERROR("Bad platform: {}", argv[2]);
			return tool::BAD_USAGE;
		}

		byte vm = (byte) std::strtol(argv[3], nullptr, 16);

		

		VmInfo* nfo;

		if (!IsValidVm(vm, nfo)) {
			LOG_ERROR("Bad VM: {}", argv[3]);
			return tool::BAD_USAGE;
		}

		LOG_INFO("Building db for {} (0x{:x}) {}", nfo->name, (int)nfo->vm, PlatformName(plt));

		std::ofstream vmfile{ VM_CODES_DB, std::ios::binary };

		if (!vmfile) {
			LOG_ERROR("Can't open file {}", VM_CODES_DB);
			return tool::BASIC_ERROR;
		}

		byte buffer[0x1004] = {};
		// clear buffer
		memset(buffer, SERID_Invalid, sizeof(buffer));


		// header
		buffer[0] = vm;
		buffer[1] = plt - 1;
		*reinterpret_cast<uint16_t*>(&buffer[2]) = 0x1000;

		std::unordered_set<SeriousId> look{};


		// build mapping, basically it writes all the nop as the opcodes, except the PreScriptCall converted
		// to an end opcode.
		// so if the user is calling a function, it'll end the function, all the other opcodes are nop
		// basically it'll create an error only if the opcode has data, so it's stupid you might ask?
		// no, because most devs are using the default autoexec __init_system__ function or are calling 
		// a function inside the autoexec, so it'll act like the script is injected and compiled, but not working

		int nopCodes = 0;
		for (size_t i = 0; i < 0x1000; i++) {
			auto* loc = LookupOpCode(nfo->vm, plt, (uint16_t)i);
			if (loc->m_id == OPCODE_Nop) {
				nopCodes++;
			}
		}

		auto delta = nopCodes / SERID_COUNT;
		if (!delta) {
			delta = 1;
		}

		int seriousIdCurr = 0;
		int nopId = 0;
		for (size_t i = 0; i < 0x1000; i++) {
			auto* loc = LookupOpCode(nfo->vm, plt, (uint16_t)i);

			auto mappedId = ConvertTo(loc->m_id);

			if (mappedId == SERID_SafeCreateLocalVariables || mappedId == SERID_CheckClearParams) {
				// keep basic structure
				buffer[4 + i] = (byte)mappedId;
				continue;
			}

			if (mappedId == SERID_End) {
				buffer[4 + i] = (byte)SERID_PreScriptCall;
				continue;
			}

			if (mappedId == SERID_Invalid || mappedId != SERID_Nop || seriousIdCurr >= SERID_COUNT) {
				continue;
			}


			// gen fake id

			if (nopId++ % delta) {
				// put a random value to fake ids and to increase the entropy
				mappedId = (SeriousId)(rand() % seriousIdCurr);
				if (mappedId == SERID_PreScriptCall) {
					continue;
				}
				buffer[4 + i] = (byte) mappedId;
				continue;
			}

			while (seriousIdCurr == SERID_PreScriptCall) {
				seriousIdCurr++; // ignore this one because it is already set to the end value
			}

			buffer[4 + i] = (byte)seriousIdCurr++;
		}

		// write mapping
		vmfile.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));

		vmfile.close();
		LOG_INFO("Fake DB created into {}.", VM_CODES_DB);



		return tool::OK;
	}
	
	ADD_TOOL(serious_fake_db, "compatibility", " [type] [vm]", "Create a FAKE t7compiler database", nullptr, fakedb);
}

