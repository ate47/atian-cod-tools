#ifdef _WIN32
#define NO_PLATFORM_IMPLEMENT
#include <platform/platform_windows.hpp>
#include <conio.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#pragma comment(lib, "imagehlp.lib")
#if __has_include(<detours.h>)
#define __ACTS_PLATFORM_HAS_DETOURS
#include <detours.h>
#endif
#include <includes_shared.hpp>
#include <process.h>
#include <platform/platform.hpp>
#include <utils/utils.hpp>
#include <utils/crc.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <hook/memory.hpp>

namespace platform {

	int32_t LoadSharedFlagsToPlatform(int32_t flags) {
		int32_t nflags{};

		if ((flags & LSF_INVALID_MASK)) throw std::runtime_error("Unknown flags in LoadSharedFlagsToPlatform");

		if (flags & LSF_SEARCH_DEFAULT_DIRS) nflags |= LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
		if (flags & LSF_DONT_RESOLVE_REFERENCES) nflags |= DONT_RESOLVE_DLL_REFERENCES;

		return nflags;
	}

	void* GetModuleOfAddress(const void* address) {
		HMODULE handle{};
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(address), &handle);
		return handle;
	}


	void* GetModulePointer(const char* mod) {
		return GetModuleHandleA(mod);
	}

	void* BaseHandle() {
		static void* ptr = NULL;
		if (!ptr) {
			ptr = platform::GetModulePointer();
		}
		return ptr;
	}

	byte* BasePtr() {
		return reinterpret_cast<byte*>(BaseHandle());
	}

	PIMAGE_DOS_HEADER PImageDosHeader(void* mod) {
		return reinterpret_cast<PIMAGE_DOS_HEADER>(mod ? mod : BaseHandle());
	}

	PIMAGE_NT_HEADERS PImageNtHeader(void* mod) {
		return ImageNtHeader((mod ? mod : BaseHandle()));
	}

	PIMAGE_OPTIONAL_HEADER PImageOptHeader(void* mod) {
		return &(PImageNtHeader(mod)->OptionalHeader);
	}

	void** GetImportAddrTableEntry(const char* lib, const char* entry) {
		HMODULE mod{ (HMODULE)platform::GetModulePointer(lib) };
		if (!mod || !lib) {
			return nullptr;
		}

		auto addr = GetProcAddress(mod, entry);
		assert(addr && "can't find entry");

		auto* base = BasePtr();
		PIMAGE_IMPORT_DESCRIPTOR desc{ reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(base + PImageOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress) };

		for (;desc->Name; desc++) {
			if (_strcmpi((const char*)&base[desc->Name], lib) != 0) {
				continue;
			}

			PIMAGE_THUNK_DATA originalThunk{ reinterpret_cast<PIMAGE_THUNK_DATA>(base + desc->OriginalFirstThunk) };
			PIMAGE_THUNK_DATA thunk{ reinterpret_cast<PIMAGE_THUNK_DATA>(base + desc->FirstThunk) };


			for (; originalThunk->u1.AddressOfData; originalThunk++, thunk++) {
				if (reinterpret_cast<decltype(addr)>(thunk->u1.Function) == addr) {
					return reinterpret_cast<void**>(&thunk->u1.Function);
				}

				const size_t ordinal = originalThunk->u1.AddressOfData & 0xFFFFFFF;

				if (ordinal <= 0xFFFF) {
					if (GetProcAddress(mod, reinterpret_cast<char*>(ordinal)) == addr) {
						return reinterpret_cast<void**>(&thunk->u1.Function);
					}
				}
			}
		}

		return nullptr;
	}

	void* LoadShared(const char* lib, int32_t flags) {
		if (flags) {
			return LoadLibraryExA(lib, nullptr, LoadSharedFlagsToPlatform(flags));
		}
		return LoadLibraryA(lib);
	}

	void* LoadSystemShared(const char* lib, int32_t flags) {
		char dir[MAX_PATH]{ 0 };
		GetSystemDirectoryA(dir, sizeof(dir));

		return LoadShared(std::format("{}/{}", dir, lib), flags);
	}
	const char* GetSharedName(void* hmod) {
		if (!hmod) {
			return "";
		}

		char name[MAX_PATH]{};
		GetModuleFileNameA((HMODULE)hmod, name, sizeof(name));

		std::string_view nv{ name };

		size_t last = nv.find_last_of('\\');
		if (last == std::string::npos) {
			return utils::va("%s", name);
		}

		return utils::va("%s", name + last + 1);
	}

	const char* GetSharedPath(void* hmod) {
		if (!hmod) {
			return "";
		}

		char name[MAX_PATH]{};
		GetModuleFileNameA((HMODULE)hmod, name, sizeof(name));

		return utils::va("%s", name);
	}

	const char* GetDebugPath(void* hmod) {
		static std::unordered_map<uintptr_t, std::string> located{};
		uintptr_t hmodPtr = reinterpret_cast<uintptr_t>(hmod);

		auto it = located.find(hmodPtr);

		if (it != located.end()) {
			const char* pdb = it->second.c_str();
			return *pdb ? pdb : nullptr;
		}

		std::string lib{};

		if (utils::ReadFile(GetSharedPath(hmod), lib)) {
			PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(lib.data());
			if (lib.size() < sizeof(IMAGE_DOS_HEADER) || dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
				throw std::runtime_error(actssec("Bad dos signature for module"));
			}

			PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(lib.data() + dosHeader->e_lfanew);

			if (lib.size() < dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) || ntHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC) {
				throw std::runtime_error(actssec("Bad nt signature for module"));
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
						throw std::runtime_error(actssec("Bad magic for cv data"));
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



	// Get IAT modules
	std::vector<const char*> GetIATModules(const hook::library::Library& lib) {
		std::vector<const char*> res{};

		IMAGE_DATA_DIRECTORY& dir{ PImageOptHeader(*lib)->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return res; // nothing

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)(lib[dir.VirtualAddress]) };

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			res.push_back(lib.Get<const char>(imp.Name));
		}
		return res;
	}

	static bool IsLibrarySafe(const char* name) {
		static const char* safeLibs[]{
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

	void PatchIAT(hook::library::Library& lib) {
		// return if a library can safely be loaded in acts or if PatchIAT should ignore the refs
		IMAGE_DATA_DIRECTORY& dir{ PImageOptHeader(*lib)->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return; // nothing to patch

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)(lib[dir.VirtualAddress]) };

		LOG_TRACE("Loading imports 0x{:x}/0x{:x}", dir.VirtualAddress, dir.Size);

		std::ostringstream osnames{};

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			const char* name{ lib.Get<const char>(imp.Name) };

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
				LOG_TRACE("Can't load IAT lib {} to patch {}", name, lib);
				continue;
			}

			IMAGE_THUNK_DATA* thunks{ lib.Get<IMAGE_THUNK_DATA>(imp.FirstThunk) };
			IMAGE_THUNK_DATA* originalThunks{ lib.Get<IMAGE_THUNK_DATA>(imp.OriginalFirstThunk) };

			while (originalThunks->u1.Function) {
				IMAGE_THUNK_DATA& thunk{ *thunks++ };
				IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

				void* func;
				if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
					func = dep[(const char*)IMAGE_ORDINAL64(originalThunk.u1.Ordinal)];
					if (!func) {
						LOG_DEBUG("Can't find {}::ord<{}>", dep, IMAGE_ORDINAL64(originalThunk.u1.Ordinal));
						continue;
					}
				}
				else {
					func = dep[lib.Get<const char>(originalThunk.u1.Function + 2)];
					if (!func) {
						LOG_DEBUG("Can't find {}::{}", dep, lib.Get<const char>(originalThunk.u1.Function + 2));
						continue;
					}
				}

				WriteMemSafe(&thunk, &func, sizeof(func));
			}
		}

		LOG_TRACE("Loaded IAT for {}:{}", lib, osnames.str());
	}




	bool FreeShared(void* lib) {
		return FreeLibrary((HMODULE)lib);
	}

	void* GetFunctionAddress(void* mod, const char* name) {
		return GetProcAddress((HMODULE)mod, name);
	}

	void WriteMemSafe(void* dest, void* src, size_t len) {
		DWORD old = 0;
		VirtualProtect(dest, len, PAGE_EXECUTE_READWRITE, &old);

		memcpy(dest, src, len);

		VirtualProtect(dest, len, old, &old);
		FlushInstructionCache(GetCurrentProcess(), dest, len);
	}


	namespace {
		bool Int32Distance(void* from, void* to) {
			constexpr size_t uint32dist = 1ull << 31;
			return ((uintptr_t)from - (uintptr_t)to) < uint32dist;
		}

		class NearContainer {
			void* container;
			size_t used{};
			size_t capacity{};
		public:
			NearContainer(void* container, size_t capacity) : container(container), capacity(capacity) {
			}

			constexpr bool CanAllocate(size_t len) const {
				return used + len <= capacity;
			}

			void* Allocate(size_t size) {
				if (!CanAllocate(size)) {
					throw std::exception(utils::va(actssec("Can't allocate into this container 0x%llx + 0x%llx > 0x%llx"), used, size, capacity));
				}

				void* ptr = (byte*)container + used;

				used += size;

				return ptr;
			}

			bool IsInside(const void* location) const {
				return location >= container && location <= (byte*)container + capacity;
			}

			bool IsCloseEnough(void* location, size_t len = 1) const {
				return Int32Distance((byte*)location + used, container) && Int32Distance((byte*)location + used + len, container);
			}

			bool CanAllocateClose(void* location, size_t len) const {
				return CanAllocate(len) && Int32Distance((byte*)location + used, container) && Int32Distance((byte*)location + used + len, container);
			}
		};

		SYSTEM_INFO& GetSysInfo() {
			static SYSTEM_INFO sysInfo{};
			static std::once_flag pageSizeFlag{};
			std::call_once(pageSizeFlag, [] {
				GetSystemInfo(&sysInfo);
				});
			return sysInfo;
		}

		std::vector<NearContainer>& NearPool() {
			static std::vector<NearContainer> nearPool{};
			return nearPool;
		}
	}

	bool ReadMemorySafe(void* location, void* buffer, size_t size) {
		return ReadProcessMemory(GetCurrentProcess(), location, buffer, size, NULL);
	}

	void* GetRelativeMemorySafe(void* location) {
		DWORD rloc{};
		if (!ReadProcessMemory(GetCurrentProcess(), location, &rloc, sizeof(rloc), NULL)) {
			return nullptr;
		}
		return (byte*)location + rloc + sizeof(rloc);
	}

	void* AllocateNearPage(void* location) {
		SYSTEM_INFO& sysInfo = GetSysInfo();
		uint64_t pageSize = sysInfo.dwPageSize;
		uint64_t startAddr = (uint64_t(location) & ~(pageSize - 1));
		uint64_t minAddr = std::min(startAddr - 0x7FFFFF00, (uint64_t)sysInfo.lpMinimumApplicationAddress);
		uint64_t maxAddr = std::max(startAddr + 0x7FFFFF00, (uint64_t)sysInfo.lpMaximumApplicationAddress);

		uint64_t startPage = (startAddr - (startAddr % pageSize));

		uint64_t pageOffset = 1;
		while (true) {
			uint64_t byteOffset = pageOffset * pageSize;
			uint64_t highAddr = startPage + byteOffset;
			uint64_t lowAddr = (startPage > byteOffset) ? startPage - byteOffset : 0;

			bool needsExit = highAddr > maxAddr && lowAddr < minAddr;

			if (highAddr < maxAddr) {
				void* outAddr = VirtualAlloc((void*)highAddr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (outAddr) {
					return outAddr;
				}
			}

			if (lowAddr > minAddr) {
				void* outAddr = VirtualAlloc((void*)lowAddr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (outAddr != nullptr) {
					return outAddr;
				}
			}

			pageOffset++;

			if (needsExit) {
				throw std::exception(utils::va(actssec("Can't allocate near page %p"), location));
			}
		}
	}

	void* AllocateNear(void* location, size_t size) {
		std::vector<NearContainer>& nearPool = NearPool();
		for (auto& container : nearPool) {
			if (!container.CanAllocateClose(location, size)) {
				continue;
			}

			return container.Allocate(size);
		}
		DWORD pageSize = GetSysInfo().dwPageSize;
		if (pageSize < size) {
			throw std::exception(utils::va(actssec("Can't allocate near %p of size 0x%llx: too big (0x%llx max)"), location, size, GetSysInfo().dwPageSize));
		}

		void* pool = AllocateNearPage(location);
		auto& container = nearPool.emplace_back(pool, pageSize);

		return container.Allocate(size);
	}

	bool IsInsideNearContainer(const void* location) {
		for (const auto& container : NearPool()) {
			if (container.IsInside(location)) {
				return true;
			}
		}
		return false;
	}

	void DumpMemory(void* location, size_t size) {
		byte* b = (byte*)location;

		DWORD old{};
		VirtualProtect(location, size, PAGE_EXECUTE_READWRITE, &old);


		for (size_t i = 0; i < size;) {
			std::stringstream ss{};
			for (size_t j = 0; j < 8 && i < size; j++, i++) {
				ss << std::setw(2) << std::setfill('0') << std::hex << (int)b[i] << " ";
			}
			LOG_DEBUG("{}", ss.str());
		}

		VirtualProtect(location, size, old, &old);
	}


	void CreateDetour(void** base, void* to) {
#ifdef __ACTS_PLATFORM_HAS_DETOURS
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)*base, to);

		LONG error = DetourTransactionCommit();

		if (error != NO_ERROR) {
			throw std::runtime_error(utils::va(actssec("Can't commit detour %p -> %p"), base, to));
		}
#else
		throw std::runtime_error("detours not available for this build");
#endif
	}

	void ClearDetour(void* base, void* to) {
#ifdef __ACTS_PLATFORM_HAS_DETOURS
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)base, to);

		LONG error = DetourTransactionCommit();

		if (error != NO_ERROR) {
			throw std::runtime_error(utils::va(actssec("Can't commit clear detour %p -> %p"), base));
		}
#else
throw std::runtime_error("detours not available for this build");
#endif
	}


	std::vector<MemoryRegion> EnumerateMemoryRegions(void* mod) {
		std::vector<MemoryRegion> regions;

		// Determine module range
		uint8_t* base{ reinterpret_cast<uint8_t*>(mod) };

		// Read PE header to get module size
		size_t moduleSize{ PImageOptHeader(mod)->SizeOfImage };
		uint8_t* moduleEnd{ base + moduleSize };

		MEMORY_BASIC_INFORMATION mbi{};
		uint8_t* current{ base };

		while (current < moduleEnd) {
			if (!VirtualQuery(current, &mbi, sizeof(mbi))) {
				break;
			}

			uint8_t* regionBase = reinterpret_cast<uint8_t*>(mbi.BaseAddress);
			size_t regionSize = mbi.RegionSize;

			bool readable =
				(mbi.State == MEM_COMMIT) &&
				!(mbi.Protect & PAGE_GUARD) &&
				!(mbi.Protect & PAGE_NOACCESS);

			// Clamp region to module boundaries
			uint8_t* regionStart = std::max(regionBase, base);
			uint8_t* regionEnd = std::min(regionBase + regionSize, moduleEnd);

			if (regionEnd > regionStart) {
				regions.push_back({
					regionStart,
					static_cast<size_t>(regionEnd - regionStart),
					readable
				});
			}

			current = regionBase + regionSize;
		}

		return regions;
	}
	namespace {
		const char* PtrInfo(void* location) {
			uintptr_t relativeLocation;
			const char* moduleName;

			if (hook::error::GetLocInfo(location, relativeLocation, moduleName)) {
				return utils::va("%s 0x%llx (%p)", moduleName, relativeLocation, location);
			}
			else {
				return utils::va("%p", location);
			}
		}

		const char* ExceptionName(DWORD ex) {
			switch (ex) {
			case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
			case EXCEPTION_BREAKPOINT: return "BREAKPOINT";
			case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
			case EXCEPTION_FLT_DENORMAL_OPERAND: return "FLT_DENORMAL_OPERAND";
			case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLT_DIVIDE_BY_ZERO";
			case EXCEPTION_FLT_INEXACT_RESULT: return "FLT_INEXACT_RESULT";
			case EXCEPTION_FLT_INVALID_OPERATION: return "FLT_INVALID_OPERATION";
			case EXCEPTION_FLT_OVERFLOW: return "FLT_OVERFLOW";
			case EXCEPTION_FLT_STACK_CHECK: return "FLT_STACK_CHECK";
			case EXCEPTION_FLT_UNDERFLOW: return "FLT_UNDERFLOW";
			case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
			case EXCEPTION_IN_PAGE_ERROR: return "IN_PAGE_ERROR";
			case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INT_DIVIDE_BY_ZERO";
			case EXCEPTION_INT_OVERFLOW: return "INT_OVERFLOW";
			case EXCEPTION_INVALID_DISPOSITION: return "INVALID_DISPOSITION";
			case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
			case EXCEPTION_PRIV_INSTRUCTION: return "PRIV_INSTRUCTION";
			case EXCEPTION_SINGLE_STEP: return "SINGLE_STEP";
			case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
			default: return "UNKNOWN";
			}
		}

		LONG ACTSUnhandledExceptionFilter(_EXCEPTION_POINTERS* ExceptionInfo) {
			hook::error::ErrorConfig& cfg{ hook::error::GetErrorConfig() };
			LOG_ERROR("--- EXCEPTION DETECTED ---");
			uintptr_t relativeLocation;
			const char* moduleName;

			if (!hook::error::GetLocInfo(ExceptionInfo->ExceptionRecord->ExceptionAddress, relativeLocation, moduleName)) {
				LOG_ERROR("Error code: 0x{:x} at {}", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress);
			}
			else {
				LOG_ERROR("Error code: 0x{:x} at {} ({} 0x{:x})", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress, moduleName, relativeLocation);
			}
			LOG_ERROR("Error type: {}", ExceptionName(ExceptionInfo->ExceptionRecord->ExceptionCode));
			DWORD threadId = GetCurrentThreadId();
			LOG_ERROR("Thread: {} ({})", threadId, threadId == cfg.mainThread ? "Main" : "Other");

			// error info
			switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
			case EXCEPTION_ACCESS_VIOLATION: {
				LOG_ERROR("Error info: invalid {} at 0x{:x}",
					!ExceptionInfo->ExceptionRecord->ExceptionInformation[0] ? "read" : "write", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
				break;
			}
			case EXCEPTION_IN_PAGE_ERROR: {
				LOG_ERROR("Error info: invalid {} at 0x{:x} status: 0x{:x}",
					!ExceptionInfo->ExceptionRecord->ExceptionInformation[0] ? "read" : "write", ExceptionInfo->ExceptionRecord->ExceptionInformation[1], ExceptionInfo->ExceptionRecord->ExceptionInformation[2]);
				break;
			}
			default: {
				if (ExceptionInfo->ExceptionRecord->NumberParameters) {
					std::stringstream info{};

					for (size_t i = 0; i < ExceptionInfo->ExceptionRecord->NumberParameters; i++) {
						info << " " << ExceptionInfo->ExceptionRecord->ExceptionInformation[i];
					}

					LOG_ERROR("Error info: {}", info.str());
				}
			}
				   break;
			}

			if (cfg.heavyDump) {
				LOG_ERROR("Registers:");
				auto PrintRegister = [](const char* name, uint64_t val) {
					uintptr_t relativeLocation;
					const char* moduleName;

					std::stringstream ss{};

					ss << "- " << name << " . 0x" << std::hex << val;

					if (hook::error::GetLocInfo(reinterpret_cast<void*>(val), relativeLocation, moduleName)) {
						ss << " | " << moduleName << " 0x" << std::hex << relativeLocation;
					}

					byte strBuff[0x100]{};

					if (ReadMemorySafe(reinterpret_cast<void*>(val), strBuff, sizeof(strBuff))) {
						bool isValidStr{ true };
						size_t i = 0;
						while (i < sizeof(strBuff)) {
							byte c = strBuff[i];
							if (!c) {
								break;
							}
							if ((c < 0x20 && c != '\r' && c != '\n' && c != '\t') || c >= 0x7F) {
								isValidStr = false;
								break;
							}
							i++;
						}
						if (i == sizeof(strBuff)) {
							isValidStr = false;
						}

						if (isValidStr && *strBuff) {
							ss << " | \"" << strBuff << "\"";
						}
						else {
							ss << " ->";
							for (size_t j = 0; j < 8; j++) {
								ss << " " << std::hex << std::setfill('0') << std::setw(2) << (int)strBuff[j];
							}
						}
					}


					LOG_ERROR("{}", ss.str());
					};
				auto PrintRegisterXmm = [PrintRegister](const char* name, M128A& val) {
					PrintRegister(utils::va("%s-high", name), val.High);
					PrintRegister(utils::va("%s-low", name), val.Low);
					};


				LOG_ERROR("DebugRegisters: 0:0x{:x} 1:0x{:x} 2:0x{:x} 3:0x{:x} 6:0x{:x} 7:0x{:x}",
					ExceptionInfo->ContextRecord->Dr0, ExceptionInfo->ContextRecord->Dr1,
					ExceptionInfo->ContextRecord->Dr2, ExceptionInfo->ContextRecord->Dr3,
					ExceptionInfo->ContextRecord->Dr6, ExceptionInfo->ContextRecord->Dr7
				);
				PrintRegister("rax", ExceptionInfo->ContextRecord->Rax);
				PrintRegister("rbx", ExceptionInfo->ContextRecord->Rbx);
				PrintRegister("rcx", ExceptionInfo->ContextRecord->Rcx);
				PrintRegister("rdx", ExceptionInfo->ContextRecord->Rdx);
				PrintRegister("rsp", ExceptionInfo->ContextRecord->Rsp);
				PrintRegister("rbp", ExceptionInfo->ContextRecord->Rbp);
				PrintRegister("rsi", ExceptionInfo->ContextRecord->Rsi);
				PrintRegister("rdi", ExceptionInfo->ContextRecord->Rdi);
				PrintRegister("r8 ", ExceptionInfo->ContextRecord->R8);
				PrintRegister("r9 ", ExceptionInfo->ContextRecord->R9);
				PrintRegister("r10", ExceptionInfo->ContextRecord->R10);
				PrintRegister("r11", ExceptionInfo->ContextRecord->R11);
				PrintRegister("r12", ExceptionInfo->ContextRecord->R12);
				PrintRegister("r13", ExceptionInfo->ContextRecord->R13);
				PrintRegister("r14", ExceptionInfo->ContextRecord->R14);
				PrintRegister("r15", ExceptionInfo->ContextRecord->R15);
				PrintRegister("rip", ExceptionInfo->ContextRecord->Rip);
				PrintRegisterXmm("xmm0", ExceptionInfo->ContextRecord->Xmm0);
				PrintRegisterXmm("xmm1", ExceptionInfo->ContextRecord->Xmm1);
				PrintRegisterXmm("xmm2", ExceptionInfo->ContextRecord->Xmm2);
				PrintRegisterXmm("xmm3", ExceptionInfo->ContextRecord->Xmm3);

				LOG_ERROR("Stack trace:");
				DumpStackTraceFrom(core::logs::LVL_ERROR, ExceptionInfo->ExceptionRecord->ExceptionAddress);
			}

			for (auto d : cfg.dumpers) {
				d();
			}

			if (cfg.cont || ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			return EXCEPTION_EXECUTE_HANDLER;
		}

		void WINAPI EmptySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
			if (HAS_LOG_LEVEL(core::logs::LVL_TRACE)) {
				return;
			}

			LOG_TRACE("Tried to insert hook from {} / {}", PtrInfo(lpTopLevelExceptionFilter), PtrInfo(_ReturnAddress()));
			hook::error::DumpStackTraceFrom(core::logs::LVL_TRACE);
		}

	}


	void DumpStackTraceFrom(core::logs::loglevel level, const void* location) {
		if (!HAS_LOG_LEVEL(level)) return; // useless
		void* locs[50];
		WORD capture = RtlCaptureStackBackTrace(0, ACTS_ARRAYSIZE(locs), locs, NULL);

		uintptr_t relativeLocation;
		const char* moduleName;
		size_t i{};
		if (location) {
			for (; i < capture; i++) {
				if (locs[i] == location) break; // skip until we reach our location
			}
		}
		if (i == capture) {
			i = 0;
		}
		for (; i < capture; i++) {
			if (hook::error::GetLocInfo(locs[i], relativeLocation, moduleName)) {
				LOG_LVLF(level, "- {} 0x{:x} ({})", moduleName, relativeLocation, locs[i]);
			}
			else {
				LOG_LVLF(level, "- {}", locs[i]);
			}
		}
	}

	void InstallErrorHooks(bool clearSetFunction) {
		hook::error::GetErrorConfig().mainThread = GetCurrentThreadId();
		SetUnhandledExceptionFilter(ACTSUnhandledExceptionFilter);
		try {
			if (clearSetFunction) {
				// redirect the previous function so it is not used
				hook::memory::RedirectJmp(SetUnhandledExceptionFilter, EmptySetUnhandledExceptionFilter);
			}
		}
		catch (std::exception& e) {
			LOG_ERROR("Can't install error hooks: {}", e.what());
		}
	}
	uint32_t GetLastPlatformError() {
		return GetLastError();
	}

	bool CreatePlatformProcess(char* cmd, bool attachConsole) {
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		if (!CreateProcessA(
			NULL,
			cmd,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si,
			&pi
		)) {
			LOG_ERROR("Can't create process {}", cmd);
			return false;
		}

		if (attachConsole) {
			AttachConsole(pi.dwProcessId);
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}


	class WinModuleInformation : public ModuleInformation {
	public:
		HMODULE hmod;
		WinModuleInformation() {}
		~WinModuleInformation() override {}

		size_t SizeOfImage() override {
			return PImageOptHeader(hmod)->SizeOfImage;
		}

		size_t ImageBase() override {
			return PImageOptHeader(hmod)->ImageBase;
		}

		uint32_t GetUID() override {
			utils::crc::CRC32 crc{};

			PIMAGE_OPTIONAL_HEADER ohd{ PImageOptHeader(hmod) };
			PIMAGE_DOS_HEADER dhd{ PImageDosHeader(hmod) };

			crc.Update(GetSharedPath(hmod));
			crc.Update(ohd->SizeOfCode);
			crc.Update(ohd->SizeOfHeaders);
			crc.Update(ohd->SizeOfImage);
			crc.Update(ohd->SizeOfInitializedData);
			crc.Update(ohd->SizeOfUninitializedData);
			crc.Update(dhd->e_cp);
			return crc;
		}
	};

	ModuleInformation& ModuleInformationTmp(void* mod) {
		thread_local WinModuleInformation tmp{};
		tmp.hmod = (HMODULE)(mod ? mod : GetModulePointer());
		return tmp;
	}

	bool PlatformConsoleAllowColor() {
		static bool color = [] {
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD mode{};
			return handle && GetConsoleMode(handle, &mode) && (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
			}();
		return color;
	}
	bool InfoMessageBox(const char* title, const char* text, bool cancelButton, bool error) {
		UINT type{};
		if (error) {
			type |= MB_ICONERROR;
		}
		else {
			type |= MB_ICONINFORMATION;
		}
		if (cancelButton) {
			type |= MB_OKCANCEL;
		}
		else {
			type |= MB_OK;
		}

		return MessageBoxA(NULL, text, title, type) == IDOK;
	}

	bool GetComputerInfoName(char* name, size_t nameSize) {
		DWORD dwSize = (DWORD)nameSize;
		if (!GetComputerNameA(name, &dwSize)) {
			LOG_WARNING("Can't get computer name");
			strncpy_s(name, nameSize, "ERROR", _TRUNCATE);
		}

		return true;
	}

	// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
	std::string WStrToStr(const std::wstring& wstr) {
		if (wstr.empty()) {
			return {};
		}
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	std::wstring StrToWStr(const std::string& str) {
		if (str.empty()) {
			return {};
		}
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::filesystem::path GetProgDir() {
		wchar_t szFileName[MAX_PATH];
		GetModuleFileNameW(NULL, szFileName, MAX_PATH);
		return std::filesystem::absolute(szFileName).parent_path();
	}
}

#else // !WIN32
#include <includes_shared.hpp>
#endif