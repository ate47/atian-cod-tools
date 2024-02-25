#include <includes.hpp>
#include "tools/dump.hpp"
#include "tools/gsc.hpp"
#include <offsets.hpp>

/*
 * integrated version of the T8CustomEE mod.
 * https://github.com/ate47/t8-custom-ee
 */

class TargetReplace {
public:
	UINT64 m_name;
	UINT32 m_replace;
	BYTE* m_toReplace;
	size_t m_size;
	bool link = false;
	uintptr_t patchLocation = 0;
	std::unique_ptr<BYTE[]> m_replacedData;

	TargetReplace(UINT64 name, UINT32 replace, BYTE* toReplace, size_t size) : 
		m_name(name), m_replace(replace), m_toReplace(toReplace), m_size(size), m_replacedData(new BYTE[size]) {
	}
};

int t8customee(int argc, const char* argv[]) {
	Process proc{ L"blackops4.exe" };

	if (!proc) {
		std::wcerr << L"Can't find game process\n";
		return -1;
	}

	if (!proc.Open()) {
		std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
		return -1;
	}

	auto oldpid = proc.m_pid;

	uintptr_t poolPtr = proc.ReadMemory<uintptr_t>(proc[offset::XASSET_SCRIPTPARSETREE]);
	INT32 poolSize = proc.ReadMemory<INT32>(proc[offset::XASSET_SCRIPTPARSETREE + 0x14]);


	if (!poolPtr || !poolSize) {
		std::cerr << "Can't read scriptparsetree pool\n";
		return tool::BASIC_ERROR;
	}

	auto buffer = std::make_unique<tool::dump::T8ScriptParseTreeEntry[]>(poolSize);

	
	if (!proc.ReadMemory(&buffer[0], poolPtr, poolSize * sizeof(buffer[0]))) {
		std::cerr << "Can't read scriptparsetree buffer" << poolPtr << "\n";
		return tool::BASIC_ERROR; // can't read buffer
	}

	UINT64 name = hashutils::Hash64("scripts/zm_common/zm_utility");
	UINT32 targetFunction = hashutils::Hash32("is_ee_enabled");

	// CheckClearParams 0x000d
	// GetByte 0x018a 0x01
	// Align 0x00
	// Return 0x003c
	BYTE data[] = {0x0d, 0x00, 0x8a, 0x01, 0x01, 0x00, 0x3c, 0x00};

	TargetReplace targets[2] = {
		TargetReplace(hashutils::Hash64(".gsc", name), targetFunction, data, sizeof(data)),
		TargetReplace(hashutils::Hash64(".csc", name), targetFunction, data, sizeof(data))
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
			if (!proc.ReadMemory(&obj, buffer[i].buffer, sizeof(obj)) || obj.GetVm() != 0x36) {
				std::cerr << "Can't read searched script " << std::hex << buffer[i].name << "\n";
				continue; // can't read buffer
			}

			size_t toRead = obj.exports_count;
			size_t shift = 0;

			while (toRead > 0) {
				auto read = min(sizeof(exports) / sizeof(exports[0]), toRead);

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
					auto patchLocation = utils::Aligned<UINT16>((buffer[i].buffer + exports[j].address));


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

	if (find) {
		std::cerr << "Can't find scripts\n";
		for (auto& target : targets) {
			std::cerr << "script_" << std::hex << target.m_name << " : " << (target.link ? "found" : "not found") << "\n";
		}
		
		return tool::BASIC_ERROR;
	}

	proc.Close();

	std::cout << "\nPress any key to reset\n";
	char k = _getch();

	std::cout << k << " Reseting...\n";

	Process proc2{ L"blackops4.exe" };

	if (!proc2 || !proc2.Open() || proc2.m_pid != oldpid) {
		std::cerr << "Can't find game process\n";
		return tool::OK;
	}

	for (auto& target : targets) {
		auto raw = proc2.ReadMemory<UINT32>(target.patchLocation);

		if (raw != *reinterpret_cast<UINT32*>(target.m_toReplace)) {
			std::cerr << "Not the same start data for " << std::hex << target.m_name << ", this is not an error if you left the zombies mode\n"
				<< target.m_toReplace << " != " << raw << "\n";
			continue; // not the same data ignore
		}

		if (!proc2.WriteMemory(target.patchLocation, &target.m_replacedData[0], target.m_size)) {
			std::cerr << "Can't unpatch script_" << std::hex << target.m_name << "\n";
			continue;
		}
		std::cout << "script_" <<std::hex << target.m_name << " unpatch\n";
	}

	return tool::OK;
}

ADD_MOD("t8cee", "Enable EEs in custom mutation/offline/casual", t8customee);
