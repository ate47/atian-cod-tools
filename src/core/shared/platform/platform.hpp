#pragma once
#include <cstdint>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace hook::library {
	class Library;
}

namespace platform {
	//////////////////////// Library functions
	// flags used by load shared functions
	enum LoadSharedFlags : int32_t {
		LSF_SEARCH_DEFAULT_DIRS = 1,
		LSF_DONT_RESOLVE_REFERENCES = 2,

		LSF_INVALID_MASK = ~(LSF_SEARCH_DEFAULT_DIRS | LSF_DONT_RESOLVE_REFERENCES),
	};

	// get shared flags to platform
	int32_t LoadSharedFlagsToPlatform(int32_t);

	// get the module of an address
	void* GetModuleOfAddress(const void* address);
	// get a module with its name
	void* GetModulePointer(const char* mod = nullptr);
	// base handle
	void* BaseHandle();
	// base pointer
	byte* BasePtr();
	// get the IAT entry of a lib
	void** GetImportAddrTableEntry(const char* lib, const char* entry);
	// get the address of a module function
	void* GetFunctionAddress(void* mod, const char* name);
	// Load a shared lib
	void* LoadShared(const char* lib, int32_t flags = 0);
	// Load a shared lib (std::string)
	inline void* LoadShared(const std::string& lib, int32_t flags = 0) { return LoadShared(lib.data(), flags); }
	// Load a system shared lib
	void* LoadSystemShared(const char* lib, int32_t flags = 0);
	// Load a system shared lib (std::string)
	inline void* LoadSystemShared(const std::string& lib, int32_t flags = 0) { return LoadSystemShared(lib.data(), flags); }
	// Free a shared lib
	bool FreeShared(void* lib);
	// Get the name of a shared lib
	const char* GetSharedName(void* hmod);
	// Get the path of a shared lib
	const char* GetSharedPath(void* hmod);
	// Get the debug file of a shared lib
	const char* GetDebugPath(void* hmod);
	// Get IAT modules
	std::vector<const char*> GetIATModules(const hook::library::Library& lib);
	// path IAT
	void PatchIAT(hook::library::Library& lib);

	// Module information
	class ModuleInformation {
	public:
		virtual ~ModuleInformation() {};
		// size of the image
		virtual size_t SizeOfImage() = 0;
		// base of the image
		virtual size_t ImageBase() = 0;
		// compute uid with known information
		virtual uint32_t GetUID() = 0;
	};

	// create temp module info for this thread
	ModuleInformation& ModuleInformationTmp(void* mod = nullptr);

	//////////////////////// Memory functions

	// write to memory safely
	void WriteMemSafe(void* dest, void* src, size_t len);
	// read memory safely
	bool ReadMemorySafe(void* location, void* buffer, size_t size);
	// get relative memory safe
	void* GetRelativeMemorySafe(void* location);
	// allocate a near page
	void* AllocateNearPage(void* location);
	// allocate near a size
	void* AllocateNear(void* location, size_t size);
	// is inside a near container
	bool IsInsideNearContainer(const void* location);
	// dump memory to debug logs
	void DumpMemory(void* location, size_t size);
	// create a detour (base -> to)
	void CreateDetour(void** base, void* to);
	// clear an existing detour
	void ClearDetour(void* base, void* to);

	// memory region information
	struct MemoryRegion {
		uint8_t* base;
		size_t size;
		bool readable;
	};
	// memory regions of a module
	std::vector<MemoryRegion> EnumerateMemoryRegions(void* mod);

	//////////////////////// Error functions
	// dump stack trace
	void DumpStackTraceFrom(core::logs::loglevel level, const void* location);
	// install error hooks
	void InstallErrorHooks(bool clearSetFunction);
	
	
	//////////////////////// Process functions

	// create process
	bool CreatePlatformProcess(char* cmd, bool attachConsole);

	//////////////////////// Special

	// check if the console allows the colors
	bool PlatformConsoleAllowColor();
	// info message box
	bool InfoMessageBox(const char* title, const char* text, bool cancelButton, bool error);
	// get the computer name
	bool GetComputerInfoName(char* name, size_t nameSize);
	// wstring to string
	std::string WStrToStr(const std::wstring& wstr);
	// string to wstring
	std::wstring StrToWStr(const std::string& str);
	// get prog directory
	std::filesystem::path GetProgDir();
}