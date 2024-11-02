#include <includes.hpp>
#include "custom_ees.hpp"
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <offsets.hpp>

/*
 * integrated version of the T8CustomEE mod.
 * https://github.com/ate47/t8-custom-ee
 */

class TargetReplace {
public:
	uint64_t m_name;
	uint32_t m_replace;
	byte* m_toReplace;
	size_t m_size;
	bool link = false;
	uintptr_t patchLocation = 0;
	std::unique_ptr<byte[]> m_replacedData;

	TargetReplace(uint64_t name, uint32_t replace, byte* toReplace, size_t size) : 
		m_name(name), m_replace(replace), m_toReplace(toReplace), m_size(size), m_replacedData(new byte[size]) {
	}
};

int mods::ee::CustomEET8(Process& proc, std::string& notif) {
	uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[offset::XASSET_SCRIPTPARSETREE]);
	int32_t poolSize = proc.ReadMemory<int32_t>(proc[offset::XASSET_SCRIPTPARSETREE + 0x14]);


	if (!poolPtr || !poolSize) {
		notif = "Can't read scriptparsetree pool";
		return tool::BASIC_ERROR;
	}

	auto buffer = std::make_unique<tool::dump::T8ScriptParseTreeEntry[]>(poolSize);


	if (!proc.ReadMemory(&buffer[0], poolPtr, poolSize * sizeof(buffer[0]))) {
		std::cerr << "Can't read scriptparsetree buffer" << poolPtr << "\n";
		return tool::BASIC_ERROR; // can't read buffer
	}

	constexpr uint64_t name = hash::Hash64("scripts/zm_common/zm_utility");
	constexpr uint32_t targetFunction = hash::HashT89Scr("is_ee_enabled");

	// CheckClearParams 0x000d
	// GetByte 0x018a 0x01
	// Align 0x00
	// Return 0x003c
	byte data[] = { 0x0d, 0x00, 0x8a, 0x01, 0x01, 0x00, 0x3c, 0x00 };

	TargetReplace targets[2] = {
		TargetReplace(hash::Hash64(".gsc", name), targetFunction, data, sizeof(data)),
		TargetReplace(hash::Hash64(".csc", name), targetFunction, data, sizeof(data))
	};

	const int count = sizeof(targets) / sizeof(targets[0]);
	int find = count;
	std::cout << "Searching " << find << " scripts...\n\n";
	tool::gsc::T8GSCOBJ obj;
	tool::gsc::T8GSCExport exports[100];
	for (size_t i = 0; i < poolSize; i++) {
		for (auto& target : targets) {
			if (target.link) {
				continue;
			}

			if (buffer[i].name != target.m_name) {
				continue;
			}

			// find
			if (!proc.ReadMemory(&obj, buffer[i].buffer, sizeof(obj)) || *(uint64_t*)obj.magic != tool::gsc::opcode::VMI_T8) {
				std::cerr << "Can't read searched script " << std::hex << buffer[i].name << "\n";
				continue; // can't read buffer
			}

			size_t toRead = obj.exports_count;
			size_t shift = 0;

			while (toRead > 0) {
				size_t read = std::min(sizeof(exports) / sizeof(exports[0]), toRead);

				if (!proc.ReadMemory(&exports[0], buffer[i].buffer + obj.export_table_offset + shift * sizeof(exports[0]), read * sizeof(exports[0]))) {
					std::cerr << "Can't read searched script exports " << std::hex << buffer[i].name << "\n";
					toRead = 0;
					continue; // can't read buffer
				}
				toRead = (size_t)(toRead - read);
				shift += read;

				for (size_t j = 0; j < read; j++) {
					if (exports[j].name != target.m_replace) {
						continue; // not what we are searching for
					}

					std::cout << "found " << std::hex << buffer[i].name << "::" << exports[j].name << ", patching...\n";

					// align even if I don't think it is useful
					auto patchLocation = utils::Aligned<uint16_t>((buffer[i].buffer + exports[j].address));


					if (!proc.ReadMemory(&target.m_replacedData[0], patchLocation, target.m_size)) {
						std::cerr << "Error when reading previous info\n";
						break;
					}

					if (!proc.WriteMemory(patchLocation, target.m_toReplace, target.m_size)) {
						std::cerr << "Error when patching\n";
						break;
					}

					target.patchLocation = patchLocation;
					target.link = true;

					std::cout << "Function " << std::hex << target.m_replace << " replaced\n";

					break;
				}

			}
			if (target.link) {
				find--;
			}
			else {
				std::cerr << "Can't find export in script " << std::hex << buffer[i].name << "/" << target.m_name << "\n";
			}
		}
		if (!find) {
			break; // found all of them
		}
	}


	if (!find) {
		notif = ("zm_utility::is_ee_enabled patched, leave the zombies mode to erase");
		return tool::OK;
	}
	else {
		notif = ("Can't find zm_utility::is_ee_enabled, are you in the zombies mode?");
		return tool::BASIC_ERROR;
	}
}

int t8customee(int argc, const char* argv[]) {
	Process proc{ L"BlackOps4.exe" };

	if (!proc) {
		std::wcerr << L"Can't find game process\n";
		return -1;
	}

	if (!proc.Open()) {
		std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
		return -1;
	}

	std::string notif;
	int ret = mods::ee::CustomEET8(proc, notif);

	if (ret) {
		LOG_ERROR("{}", notif);
	}
	else {
		LOG_INFO("{}", notif);
	}

	return ret;
}

ADD_MOD("t8cee", "Enable EEs in custom mutation/offline/casual", t8customee);
