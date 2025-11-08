#include <includes_shared.hpp>
#include "library.hpp"
#include "process.hpp"
#include <utils/utils.hpp>
#include <utils/crc.hpp>

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
	std::vector<ScanResult> ScanLibraryString(HMODULE hmod, const char* pattern, bool single) {
		size_t pl{ std::strlen(pattern) };
		std::string patStr{};
		patStr.resize((pl + 1) * 3 + 1);

		char* patStrPtr{ patStr.data() };

		for (size_t i = 0; i <= pl; i++) {
			std::snprintf(&patStrPtr[3 * i], 4, "%02x ", (uint32_t)pattern[i]);
		}

		return ScanLibrary(hmod, patStrPtr, single);
	}

	std::vector<ScanResult> ScanLibrary(HMODULE hmod, const char* pattern, bool single) {
		std::vector<ScanResult> res{};


		std::vector<uint8_t*> find{};

		std::vector<uint8_t> mask{};
		std::vector<uint8_t> searched{};

		bool mid{ true };

		// parse pattern
		const char* str{ pattern };
		while (*str) {
			char c = *(str++);
			if (isspace(c)) {
				continue;
			}

			mid = !mid;

			if (c == '?') {
				if (mid) {
					throw std::runtime_error(utils::va("Wildcard pattern in half byte! %s", pattern));
				}
				if (str[0] == '?') {
					// test if we are in a packed context
					if (!str[1] || isspace(str[1])) {
						str++; // consume both ??
					}
				}
				// consume both
				mid = !mid;
				mask.push_back(0);
				searched.push_back(0);
				continue;
			}

			auto b = utils::ctob(c);

			if (mid) {
				*(mask.end() - 1) |= 0xF;
				*(searched.end() - 1) |= b;
			}
			else {
				mask.push_back(0xF0);
				searched.push_back(b << 4);
			}
		}

		// reversed because we set it by default to 0
		if (!mid) {
			throw std::runtime_error(utils::va("Scan pattern has half byte! %s", pattern));
		}

		auto it1 = mask.begin();
		auto it2 = searched.begin();

		// clear start
		while (it1 != mask.end()) {
			if (*it1) {
				break;
			}
			it1 = mask.erase(it1);
			it2 = searched.erase(it2);
		}

		if (!mask.size()) {
			throw std::runtime_error(utils::va("Empty pattern! %s", pattern));
		}

		// clear end
		auto eit1 = mask.end() - 1;
		auto eit2 = searched.end() - 1;
		while (eit1 != mask.begin()) {
			if (*eit1) {
				break;
			}
			eit1 = mask.erase(eit1) - 1;
			eit2 = searched.erase(eit2) - 1;
		}

		if (!mask.size()) {
			throw std::runtime_error(utils::va("Empty pattern! %s", pattern));
		}

		library::Library lib{ hmod };

		constexpr size_t lazySize = 0x10000000;

		if (lazySize < mask.size()) {
			throw std::runtime_error(utils::va("Pattern too big! %s", pattern)); // wtf?
		}


		LOG_TRACE("Start searching of pattern {}", pattern);
		byte* start{ (byte*)*lib };
		byte* end{ (byte*)lib[lazySize - mask.size()] };

		MEMORY_BASIC_INFORMATION64 page{};
		page.RegionSize = 0;
		byte* current{};
		do {
			current += page.RegionSize;
			VirtualQuery(current, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page), sizeof(MEMORY_BASIC_INFORMATION));
			current = reinterpret_cast<byte*>(page.BaseAddress);
		} while (current < start);

		while (current < end) {
			VirtualQuery(current, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page), sizeof(MEMORY_BASIC_INFORMATION));
			current = reinterpret_cast<byte*>(page.BaseAddress);

			if (page.State != MEM_COMMIT || page.Protect == PAGE_NOACCESS || (page.Protect & PAGE_GUARD)) {
				current += page.RegionSize;
				continue;
			}

			for (size_t off = 0; off < page.RegionSize - mask.size(); off++) {
				size_t i;
				for (i = 0; i < mask.size(); i++) {
					if ((current[off + i] & mask[i]) != searched[i]) {
						break;
					}
				}
				if (i == mask.size()) {
					res.emplace_back(current + off);
					if (single) {
						LOG_TRACE("Pattern find -> {}", (hook::library::CodePointer)res[0].location);
						return res;
					}
				}
			}

			current += page.RegionSize;
		}
		LOG_TRACE("Pattern find -> {}", res.size());

		return res;
	}

	void hook::library::Library::Redirect(const char* pattern, void* func, const char* name) const {
		hook::memory::RedirectJmp(ScanSingle(pattern, name).location, func);
	}

	void hook::library::Library::Redirect(size_t offset, void* func) const {
		hook::memory::RedirectJmp((*this)[offset], func);
	}

	uint32_t hook::library::Library::GetUID() const {
		utils::crc::CRC32 crc{};

		PIMAGE_OPTIONAL_HEADER ohd{ GetOptHeader() };
		PIMAGE_DOS_HEADER dhd{ GetDosHeader() };

		crc.Update(GetPath());
		crc.Update(ohd->SizeOfCode);
		crc.Update(ohd->SizeOfHeaders);
		crc.Update(ohd->SizeOfImage);
		crc.Update(ohd->SizeOfInitializedData);
		crc.Update(ohd->SizeOfUninitializedData);
		crc.Update(dhd->e_cp);

		return crc;
	}

	hook::library::Detour hook::library::Library::CreateDetour(const char* pattern, void* to, const char* name) const {
		hook::library::Detour det;

		det.Create(ScanSingle(pattern, name).location, to);

		return det;
	}

	Detour hook::library::Library::CreateDetour(size_t offset, void* to) const {
		hook::library::Detour det;

		det.Create((*this)[offset], to);

		return det;
	}

	constexpr uint64_t SCAN_CONTAINER_FILE_MAGIC = 0x000a4e41435380;
	constexpr uint64_t SCAN_CONTAINER_FILE_VERSION = 0x210000000000000;

	struct ScanContainer {
		const char* name{};
		std::unordered_map<std::string, std::unordered_set<uint32_t>> deltas{};

		std::filesystem::path GetPath() {
			return utils::va("%s.scan", name);
		}
	};

	static ScanContainer container{};

	void InitScanContainer(const char* name) {
		container.name = name;

		// load scan file

		std::string buff{};
		auto path = container.GetPath();

		if (!utils::ReadFile(path, buff)) {
			LOG_WARNING("No scan container found, the loading will take some time");
			return; // empty, nothing to read
		}

		byte* file = (byte*)buff.data();

		if (*reinterpret_cast<decltype(SCAN_CONTAINER_FILE_MAGIC)*>(file) != (SCAN_CONTAINER_FILE_MAGIC | SCAN_CONTAINER_FILE_VERSION)) {
			// bad version/magic, we need to erase it
			LOG_WARNING("Detected scan container with bad magic, it'll be erased at save '{}'", path.string());
			return;
		}

		file += sizeof(SCAN_CONTAINER_FILE_MAGIC);

		uint32_t count = *reinterpret_cast<uint32_t*>(file);
		file += sizeof(count);

		for (size_t i = 0; i < count; i++) {
			if (file + 5 > (byte*)buff.data() + buff.size()) {
				return;
			}

			char* str = reinterpret_cast<char*>(file);

			file += strlen(str) + 1;

			uint32_t num = *reinterpret_cast<uint32_t*>(file);
			file += sizeof(num);

			uint32_t* locs = reinterpret_cast<uint32_t*>(file);

			auto& delta = container.deltas[str];
			for (size_t j = 0; j < num; j++) {
				file += sizeof(num);

				delta.insert(locs[j]);
			}
		}
	}
	bool ScanMatch(void* ptr, const char* scan) {
		std::vector<uint8_t> mask{};
		std::vector<uint8_t> searched{};
		bool mid{ true };
		const char* str{ scan };
		while (*str) {
			char c = *(str++);
			if (isspace(c)) {
				continue;
			}

			mid = !mid;

			if (c == '?') {
				if (mid) {
					throw std::runtime_error(utils::va("Wildcard pattern in half byte! %s", scan));
				}
				if (str[0] == '?') {
					// test if we are in a packed context
					if (!str[1] || isspace(str[1])) {
						str++; // consume both ??
					}
				}
				// consume both
				mid = !mid;
				mask.push_back(0);
				searched.push_back(0);
				continue;
			}

			auto b = utils::ctob(c);

			if (mid) {
				*(mask.end() - 1) |= 0xF;
				*(searched.end() - 1) |= b;
			}
			else {
				mask.push_back(0xF0);
				searched.push_back(b << 4);
			}
		}

		// reversed because we set it by default to 0
		if (!mid) {
			throw std::runtime_error(utils::va("Scan pattern has half byte! %s", scan));
		}

		byte* loc{ (byte*)ptr };
		for (size_t i = 0; i < searched.size(); i++) {
			if ((loc[i] & mask[i]) != searched[i]) {
				return false;
			}
		}

		return true;
	}

	void SaveScanContainer() {
		std::vector<byte> file{};

		utils::WriteValue<uint64_t>(file, SCAN_CONTAINER_FILE_MAGIC | SCAN_CONTAINER_FILE_VERSION);

		utils::WriteValue<uint32_t>(file, (uint32_t)container.deltas.size());
		LOG_TRACE("Save scan:");
		for (const auto& [name, delta] : container.deltas) {
			utils::WriteString(file, name.data());
			utils::WriteValue<uint32_t>(file, (uint32_t)delta.size());
			for (const auto loc : delta) {
				utils::WriteValue<uint32_t>(file, loc);
				LOG_TRACE("{} -> 0x{:x}", name, loc);
			}
		}

		utils::WriteFile(container.GetPath(), file.data(), file.size());

	}

	static Library main{};
	std::vector<ScanResult> QueryScanContainer(const char* name, const char* pattern, bool single) {
		auto it = container.deltas.find(name);
		if (it != container.deltas.end()) {
			std::vector<ScanResult> res{};

			for (auto loc : it->second) {
				res.emplace_back((byte*)main[loc]);
			}

			return res;
		}

		auto res = main.Scan(pattern, single);
		auto& locs = container.deltas[pattern];

		// save scan
		for (auto& loc : res) {
			locs.insert((uint32_t)(loc.location - (byte*)*main));
		}

		return res;
	}

	ScanResult QueryScanContainerSingle(const char* name, const char* pattern) {
		auto res = QueryScanContainer(name, pattern, true);

		if (res.empty()) {
			throw std::runtime_error(utils::va("Can't find pattern %s", name ? name : pattern));
		}
		if (res.size() != 1) {
			throw std::runtime_error(utils::va("Too many finds for pattern %s", name ? name : pattern));
		}

		return res[0];
	}

	hook::library::CodePointer::CodePointer(void* location) : location(location) {}

	std::ostream& operator<<(std::ostream& out, const hook::library::Library& ptr) {
		if (!ptr) {
			return out << "<INVALID_LIB>";
		}
		return out << ptr.GetName() << "[0x" << std::hex << reinterpret_cast<uintptr_t>(ptr.hmod) << "]";
	}

	std::vector<const char*> hook::library::Library::GetIATModules() const {
		std::vector<const char*> res{};

		IMAGE_DATA_DIRECTORY& dir{ GetOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return res; // nothing

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)((*this)[dir.VirtualAddress]) };

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			res.push_back(Get<const char>(imp.Name));
		}
		return res;
	}

	// return if a library can safely be loaded in acts or if PatchIAT should ignore the refs
	static bool IsLibrarySafe(const char* name) {
		static const char* safeLibs[] {
			"oo2core_6_win64.dll",
			"oo2core_7_win64.dll",
			"oo2core_8_win64.dll",
			"ntdll.dll",
		};
		return std::find_if(
			std::begin(safeLibs), 
			std::end(safeLibs), 
			[name](const char* lib) {
			return !_strcmpi(lib, name); 
		}) != std::end(safeLibs);
	}

	void hook::library::Library::PatchIAT() {
		IMAGE_DATA_DIRECTORY& dir{ GetOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return; // nothing to patch

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)((*this)[dir.VirtualAddress]) };
		
		LOG_TRACE("Loading imports 0x{:x}/0x{:x}", dir.VirtualAddress, dir.Size);

		std::ostringstream osnames{};

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			const char* name{ Get<const char>(imp.Name) };

			if (core::logs::getlevel() <= core::logs::LVL_TRACE) osnames << " " << name;

			hook::library::Library dep;
			
			if (IsLibrarySafe(name)) {
				dep = { name, false };
			}
			else {
				dep = { name, false, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS };
			}

			if (!dep) {
				osnames << "<missing>";
				LOG_TRACE("Can't load IAT lib {} to patch {}", name, *this);
				continue;
			}

			IMAGE_THUNK_DATA* thunks{ Get<IMAGE_THUNK_DATA>(imp.FirstThunk) };
			IMAGE_THUNK_DATA* originalThunks{ Get<IMAGE_THUNK_DATA>(imp.OriginalFirstThunk) };

			while (originalThunks->u1.Function) {
				IMAGE_THUNK_DATA& thunk{ *thunks++ };
				IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

				void* func;
				if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
					func = dep[(const char*)IMAGE_ORDINAL64(originalThunk.u1.Ordinal)];
					if (!func) {
						LOG_WARNING("Can't find {}::ord<{}>", dep, IMAGE_ORDINAL64(originalThunk.u1.Ordinal));
						continue;
					}
				}
				else {
					func = dep[Get<const char>(originalThunk.u1.Function + 2)];
					if (!func) {
						LOG_WARNING("Can't find {}::{}", dep, Get<const char>(originalThunk.u1.Function + 2));
						continue;
					}
				}

				process::WriteMemSafe(&thunk, func);
			}
		}

		LOG_TRACE("Loaded IAT for {}:{}", *this, osnames.str());
	}

	std::ostream& operator<<(std::ostream& out, const hook::library::CodePointer& ptr) {
		hook::library::Library library{ GetLibraryInfo(ptr.location) };
		return out << library << "+0x" << std::hex << ((byte*)ptr.location - (byte*)*library);
	}
}