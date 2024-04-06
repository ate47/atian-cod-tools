#include "includes_shared.hpp"
#include "library.hpp"
#include "process.hpp"
#include "utils.hpp"

namespace hook::library {
	HMODULE GetLibraryInfo(const void* address) {
		HMODULE handle{};
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(address), &handle);
		return handle;
	}

	const char* GetLibraryName(HMODULE hmod) {
		if (!hmod) {
			return "";
		}

		char name[MAX_PATH]{};
		GetModuleFileNameA(hmod, name, sizeof(name));

		std::string_view nv{ name };

		size_t last = nv.find_last_of('\\');
		if (last == std::string::npos) {
			return utils::va("%s", name);
		}

		return utils::va("%s", name + last + 1);
	}

	const char* GetLibraryPath(HMODULE hmod) {
		if (!hmod) {
			return "";
		}

		char name[MAX_PATH]{};
		GetModuleFileNameA(hmod, name, sizeof(name));

		return utils::va("%s", name);
	}
	const char* LocatePDB(HMODULE hmod) {
		static std::unordered_map<uintptr_t, std::string> located{};
		uintptr_t hmodPtr = reinterpret_cast<uintptr_t>(hmod);

		auto it = located.find(hmodPtr);

		if (it != located.end()) {
			const char* pdb = it->second.c_str();
			return *pdb ? pdb : nullptr;
		}

		std::string lib{};

		if (utils::ReadFile(library::GetLibraryPath(hmod), lib)) {
			PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(lib.data());
			if (lib.size() < sizeof(IMAGE_DOS_HEADER) || dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
				throw std::runtime_error("Bad dos signature for module");
			}

			PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(lib.data() + dosHeader->e_lfanew);

			if (lib.size() < dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) || ntHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) {
				throw std::runtime_error("Bad nt signature for module");
			}
			IMAGE_DATA_DIRECTORY& debugDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

			DWORD offset{};
			for (auto* sect = IMAGE_FIRST_SECTION(ntHeader), *end = sect + ntHeader->FileHeader.NumberOfSections; sect < end; sect++) {
				if (debugDirectory.VirtualAddress >= sect->VirtualAddress && debugDirectory.VirtualAddress < sect->VirtualAddress + sect->Misc.VirtualSize) {
					offset = debugDirectory.VirtualAddress - (sect->VirtualAddress - sect->PointerToRawData);
					break; // we need to patch the virtual address
				}
			}

			if (offset) {
				auto* debugEntries = reinterpret_cast<PIMAGE_DEBUG_DIRECTORY>(lib.data() + offset);
				size_t elems = debugDirectory.Size / sizeof(*debugEntries);
				// https://www.openwatcom.org/ftp/devel/docs/CodeView.pdf
				struct CodeViewData {
					uint32_t magic;
					uint8_t guid[16];
					uint32_t age;
					char pdbPath[1];
				};
				CodeViewData* cvInfo{};
				for (size_t i = 0; i < elems; i++) {
					if (debugEntries[i].Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
						cvInfo = reinterpret_cast<CodeViewData*>(lib.data() + (debugEntries + i)->PointerToRawData);
						break;
					}
				}

				if (cvInfo) {
					if (cvInfo->magic != 0x53445352) {
						throw std::runtime_error("Bad magic for cv data");
					}
					auto& loc = located[hmodPtr];
					loc = &cvInfo->pdbPath[0];
					return loc.c_str();
				}
			}

		}
		auto& loc = located[hmodPtr];
		loc = "";
		return nullptr;
	}


}