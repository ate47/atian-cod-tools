#include <includes.hpp>
#include "compatibility/serious_db2.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc.hpp"

namespace {
	using namespace tool::gsc::opcode;

	struct BuildDbGen {
		byte vmRev;
		Platform platform;
		const VmInfo* vm;
	};

	int builddb(Process& proc, int argc, const char* argv[]) {
		if ((argc % 2) != 0) {
			return tool::BAD_USAGE;
		}

		std::map<uint16_t, BuildDbGen> asked{};

		if (argc == 2) {
			LOG_INFO("Full db generation");

			const auto& vms = GetVMMaps();

			for (const auto& [_, vm] : vms) {
				for (size_t i = 1; i < Platform::PLATFORM_COUNT; i++) {
					if (vm.HasPlatform((Platform)i)) {
						byte vmOld{ tool::gsc::opcode::MapAsOldVM(vm.vmMagic) };

						if (!vmOld) {
							LOG_WARNING("Can't map vm {:x} to old", vm.vmMagic);
							continue;
						}
						auto uid = utils::CatLocated16(vmOld, (byte)i);

						asked[uid] = {
							.vmRev = vmOld,
							.platform = (Platform)i,
							.vm = &vm
						};
					}
				}
				
			}
		}
		else {
			for (int i = 2; i < argc; i += 2) {
				auto plt = PlatformOf(argv[i]);

				if (plt == PLATFORM_UNKNOWN) {
					LOG_ERROR("Bad platform: {}", argv[i]);
					return tool::BASIC_ERROR;
				}

				uint64_t nvm{ tool::gsc::opcode::VMOf(argv[i + 1])};

				VmInfo* nfo;

				if (!IsValidVm(nvm, nfo)) {
					LOG_ERROR("Bad VM: {}", argv[i + 1]);
					return tool::BASIC_ERROR;
				}

				byte vmOld{ tool::gsc::opcode::MapAsOldVM(nfo->vmMagic) };

				if (!vmOld) {
					LOG_WARNING("Can't map vm {:x} to old", nfo->vmMagic);
					continue;
				}
				auto uid = utils::CatLocated16(vmOld, plt);

				if (asked.find(uid) != asked.end()) {
					continue; // already asked
				}

				asked[uid] = {
					.vmRev = vmOld,
					.platform = plt,
					.vm = nfo
				};
			}
		}


		LOG_INFO("Building db for {} vm(s)...", asked.size());


		std::ofstream vmfile{ compatibility::serious::db2::VM_CODES_DB, std::ios::binary };

		if (!vmfile) {
			LOG_ERROR("Can't open file {}", compatibility::serious::db2::VM_CODES_DB);
			return tool::BASIC_ERROR;
		}

		byte buffer[0x1004] = {};

		for (const auto& [uid, bld] : asked) {
			// clear buffer
			memset(buffer, compatibility::serious::db2::SERID_Invalid, sizeof(buffer));


			// header
			buffer[0] = bld.vmRev;
			buffer[1] = bld.platform ? bld.platform - 1 : 0; // platform starts at 0 in compiler
			*reinterpret_cast<uint16_t*>(&buffer[2]) = 0x1000;

			// build mapping
			for (size_t i = 0; i < 0x1000; i++) {
				auto* loc = LookupOpCode(bld.vm->vmMagic, bld.platform, (uint16_t) i);

				auto mappedId = compatibility::serious::db2::ConvertTo(loc->m_id);

				if (mappedId == compatibility::serious::db2::SERID_Invalid) {
					continue;
				}

				buffer[4 + i] = (byte) mappedId;
			}

			// write mapping
			vmfile.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));
			LOG_INFO("Added vm {} / {}", bld.vm->name, PlatformName(bld.platform));
		}

		vmfile.close();
		LOG_INFO("DB created into {}", compatibility::serious::db2::VM_CODES_DB);

		return tool::OK;
	}

	int dumpdb(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		void* buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(argv[2], buffer, size, false)) {
			LOG_ERROR("Can't read file {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		auto* db = reinterpret_cast<byte*>(buffer);

		size_t real = size;
		VmInfo* nfo{};

		while (real) {
			if (real < 4) {
				LOG_ERROR("Bad chunk size: {:x}", real);
				break;
			}
			auto vm = *db;
			uint32_t platform = (int)db[1];
			auto size = *reinterpret_cast<uint16_t*>(db + 2);

			real -= 4;
			db += 4;

			if (!tool::gsc::opcode::IsValidVm(tool::gsc::opcode::OldVmOf(vm), nfo)) {
				LOG_ERROR("Invalid vm: {:x}, plt: {:x}, size: {:x}", (int)vm, platform, size);

				if (real < size) {
					LOG_ERROR("Bad chunk size: {:x}", real);
					break;
				}
				db += size;
				real -= size;
				continue;
			}

			const char* pltName;
			Platform plt;

			if ((platform + 1) < PLATFORM_COUNT) {
				plt = (Platform)(platform + 1);
				pltName = PlatformName(plt);
			}
			else {
				pltName = "<unknown>";
				plt = PLATFORM_UNKNOWN;
			}


			LOG_INFO("Vm: {} ({:x}), platform: {} ({:x}), size: {:x}", nfo->name, nfo->vmMagic, pltName, (int)platform, size);

			std::map<byte, std::vector<uint16_t>> opcodes{};

			if (real < size) {
				LOG_ERROR("Bad chunk size: {:x}", real);
				break;
			}

			for (size_t i = 0; i < size; i++) {
				auto op = *(db++);

				if (op == 0xFF) continue;

				opcodes[op].emplace_back((uint16_t)i);
			}

			for (const auto& [val, mapping] : opcodes) {
				std::ostringstream ss{};
				auto* code = tool::gsc::opcode::LookupOpCode(nfo->vmMagic, plt, mapping[0]);
				ss << "0x" << std::hex << (int)val << "/" << std::dec << (int)val << " (" << code->m_name << ") -> ";

				for (size_t i = 0; i < mapping.size(); i++) {
					if (i) {
						ss << ", ";
					}
					ss << std::hex << "0x" << mapping[i];
				}

				LOG_INFO("{}", ss.str());
			}

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

		void* buffer{};
		size_t size{};

		if (!utils::ReadFileNotAlign(dbfile, buffer, size, false)) {
			LOG_ERROR("Can't read file {}", dbfile);
			return tool::BASIC_ERROR;
		}

		size_t real = size;
		auto* db = reinterpret_cast<byte*>(buffer);

		bool patched = false;
		while (real) {
			if (real < 4) {
				LOG_ERROR("Bad chunk size: {:x}", real);
				break;
			}
			auto vmval = *db;
			auto platform = db[1];
			auto size = *reinterpret_cast<uint16_t*>(db + 2);

			real -= 4;
			db += 4;

			if (real < size) {
				LOG_ERROR("Bad chunk block size: {:x}", real);
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
				LOG_ERROR("Error when writing file");
			}
			else {
				LOG_INFO("Updated");
			}
		}
		else {
			LOG_INFO("Done");
		}

		std::free(buffer);

		return tool::OK;
	}

	ADD_TOOL(serious_db, "compatibility", " ([type] [vm])+", "Create a t7compiler database", nullptr, builddb);
	ADD_TOOL(serious_dbd, "compatibility", " [database]", "Dump a t7compiler database", nullptr, dumpdb);
	ADD_TOOL(serious_remap, "compatibility", " [database] [vm] [plt] [old] [new]", "Remap an opcode in a vm", nullptr, remapdb);
}

