#pragma once
#include <Windows.h>
#include <hook/process.hpp>
#include <utils/utils.hpp>

class Process;
class ProcessModule;
class ProcessModuleExport;
class ProcessLocation;

// ugly module I quickly wrote to access process memory
// supports:
// - Write/Read memory
// - Allocate memory
// - Find modules
// - Find exports
// - Call functions

class ProcessLocation {
private:
	const Process& proc;
	uintptr_t loc;
public:
	ProcessLocation(const Process& proc, uintptr_t loc);

	friend std::ostream& operator<<(std::ostream& os, const ProcessLocation& obj);
	friend std::wostream& operator<<(std::wostream& os, const ProcessLocation& obj);
};

// Process module export
class ProcessModuleExport {
public:
	std::unique_ptr<char[]> m_name;
	uintptr_t m_location;
	WORD m_ordinal = 0;

	ProcessModuleExport(ProcessModule& module, const char* name, uintptr_t location, WORD ordinal);
	// @return if the export is invalid
	inline bool operator!() const {
		return m_location == 0;
	}
	// @return if the export is valid
	inline bool operatorbool() const {
		return m_location != 0;
	}

	friend std::ostream& operator<<(std::ostream& os, const ProcessModuleExport& obj);
	friend std::wostream& operator<<(std::wostream& os, const ProcessModuleExport& obj);
private:
	ProcessModule& m_module;
};

// Process module
class ProcessModule {
public:
	Process& m_parent;
	char name[MAX_MODULE_NAME32 + 1] = { 0 };
	char path[MAX_PATH + 1] = { 0 };
	uintptr_t start = 0;
	DWORD size = 0;
	HANDLE handle = 0;

	ProcessModule(Process& parent);

	/*
	 * Get a relative offset in a module
	 * @param ptr Pointer
	 * @return Location in the module
	 */
	int64_t GetRelativeOffset(uintptr_t ptr) const;
	// @return if the module is invalid
	inline bool operator!() const {
		return handle == INVALID_HANDLE_VALUE;
	}
	// @return if the module is valid
	inline bool operatorbool() const {
		return handle != INVALID_HANDLE_VALUE;
	}

	/*
	 * Scan for a pattern inside the module
	 * @param pattern pattern to search
	 * @param start start location in the module
	 * @return scan result or 0 for no search
	 */
	uintptr_t Scan(const char* pattern, DWORD start = 0);

	/*
	 * Compute the export names
	 */
	void ComputeExports();
	/*
	 * Search for an export name
	 * @param name name
	 * @return export location or 0 if it can't be find
	 */
	ProcessModuleExport& operator[](const char* name);
	/*
	 * @return the proccess modules
	 */
	inline const std::vector<ProcessModuleExport>& exports() const {
		return m_exports;
	}
	friend std::ostream& operator<<(std::ostream& os, const ProcessModule& obj);
	friend std::wostream& operator<<(std::wostream& os, const ProcessModule& obj);
private:
	std::vector<ProcessModuleExport> m_exports{};
	ProcessModuleExport m_invalid;
	bool m_export_computed = false;
};

// Process
class Process {
public:
	Process(const wchar_t* processName, const wchar_t* moduleName = NULL);
	Process(const char* processName, const char* moduleName = NULL);
	~Process();
	/*
	 * Open the process
	 * @return if the process was open
	 */
	bool Open();
	/*
	 * Close the process (done inside the destructor)
	 */
	void Close();
	// @return if the process wasn't found
	bool operator!() const;
	// @return if the process was found
	bool operatorbool() const;
	/*
	 * Get the offset inside the module
	 * @param offset Relative offset
	 * @return absolute offset
	 */
	uintptr_t operator[](uint64_t offset) const;
	/*
	 * Get a module inside the process
	 * @param module Module name
	 * @return Module, if the handle is equals to INVALID_HANDLE_VALUE, the module isn't valid
	 */
	ProcessModule& operator[](const char* module);
	/*
	 * Get a module inside the process
	 * @param module Module name
	 * @return Module, if the handle is equals to INVALID_HANDLE_VALUE, the module isn't valid
	 */
	const ProcessModule& operator[](const char* module) const;
	/*
	 * Get the relative offset of a ptr inside the module
	 * @param ptr Pointer
	 * @return relative offset, can be negative if the pointer is before the module
	 */
	int64_t GetModuleRelativeOffset(uintptr_t ptr) const;
	/*
	 * Get the module of an absolute location
	 * @param ptr Pointer
	 * @return module, if the handle is equals to INVALID_HANDLE_VALUE, the module isn't valid
	 */
	const ProcessModule& GetLocationModule(uintptr_t ptr) const;
	/*
	 * Allocate memory in the process
	 * @param size Size to allocate
	 * @return pointer or null
	 */
	uintptr_t AllocateMemory(size_t size) const;
	/*
	 * Allocate memory in the process with protection
	 * @param size Size to allocate
	 * @param protection Protection
	 * @return pointer or null
	 */
	uintptr_t AllocateMemory(size_t size, DWORD protection) const;
	/*
	 * Allocate string in the process
	 * @param string String to allocate
	 * @return pointer or null
	 */
	uintptr_t AllocateString(const char* string, size_t* sizeOut) const;
	/*
	 * Set the protection of a location in memory
	 * @param ptr Pointer to set
	 * @param size Size of the pointer
	 * @param flNewProtected New protect value
	 * @param lpflOldProtect Old protect value to write
	 * @return pointer or null
	 */
	bool SetMemoryProtection(uintptr_t ptr, size_t size, DWORD flNewProtected, DWORD& lpflOldProtect) const;
	/*
	 * Create a thread in the process
	 * @param location Pointer to the function
	 * @param arg Pointer to the argument
	 * @return handle to the thread or INVALID_HANDLE_VALUE in case of error
	 */
	HANDLE Exec(uintptr_t location, uintptr_t arg) const;
	/*
	 * Free memory in the process
	 * @param ptr Pointer to free
	 * @param size Pointer size
	 */
	void FreeMemory(uintptr_t ptr, size_t size) const;
	/*
	 * Read memory from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Size to read
	 * @return true if the memory was read, false otherwise
	 */
	bool ReadMemory(void* dest, uintptr_t src, size_t size) const;
	/*
	 * Read memory array
	 * @param src Process source
	 * @param count Array count to read
	 * @return pair containing the allocated resource with the error, if pair.second == false, an error occured
	 */
	template<typename Type>
	std::pair<std::unique_ptr<Type[]>, bool> ReadMemoryArray(uintptr_t src, size_t count) const {
		std::pair<std::unique_ptr<Type[]>, bool> pair = std::make_pair<std::unique_ptr<Type[]>, bool>(std::make_unique<Type[]>(count), true);

		if (!ReadMemory(&pair.first[0], src, sizeof(Type) * count)) {
			pair.second = false;
		}

		return pair;
	}
	/*
	 * Read memory array
	 * @param src Process source
	 * @param count Array count to read
	 * @return pair containing the allocated resource with the error, if pair.second == false, an error occured
	 */
	template<typename Type>
	std::unique_ptr<Type[]> ReadMemoryArrayEx(uintptr_t src, size_t count) const {
		std::unique_ptr<Type[]> v{ std::make_unique<Type[]>(count) };

		if (!ReadMemory(&v[0], src, sizeof(Type) * count)) {
			throw std::runtime_error(utils::va("can't array read at 0x%llx 0x%llx bytes", src, sizeof(Type) * count));
		}

		return v;
	}
	/*
	 * Read memory object
	 * @param src Process source
	 * @return pair containing the allocated resource with the error, if pair.second == false, an error occured
	 */
	template<typename Type>
	std::pair<std::unique_ptr<Type>, bool> ReadMemoryObject(uintptr_t src) const {
		std::pair<std::unique_ptr<Type>, bool> pair = std::make_pair<std::unique_ptr<Type>, bool>(std::make_unique<Type>(), true);

		if (!ReadMemory(&*pair.first, src, sizeof(Type))) {
			pair.second = false;
		}

		return pair;
	}
	/*
	 * Read memory object
	 * @param src Process source
	 * @return pair containing the allocated resource with the error, if pair.second == false, an error occured
	 */
	template<typename Type>
	std::unique_ptr<Type> ReadMemoryObjectEx(uintptr_t src) const {
		std::unique_ptr<Type> v{ std::make_unique<Type>() };

		if (!ReadMemory(&*v, src, sizeof(Type))) {
			throw std::runtime_error(utils::va("can't read object at 0x%llx 0x%llx bytes", src, sizeof(Type)));
		}

		return v;
	}
	/*
	 * Read c string into a temp buffer
	 * @return string or an error message
	 */
	const char* ReadStringTmp(uintptr_t src, const char* defaultVal = "<invalid>") const;
	/*
	 * Read c w string into a temp buffer
	 * @return string or an error message
	 */
	const wchar_t* ReadStringTmpW(uintptr_t src, const wchar_t* defaultVal = L"<invalid>") const;
	/*
	 * Read c string from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Max buffer size to read
	 * @return read string size, -2 if the buffer is too small, -1 if can't read
	 */
	int64_t ReadString(char* dest, uintptr_t src, size_t size) const;
	/*
	 * Read wc string from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Max buffer size to read
	 * @return read string size, -2 if the buffer is too small, -1 if can't read
	 */
	int64_t ReadWString(wchar_t* dest, uintptr_t src, size_t size) const;
	/*
	 * Write memory from a source to a destination
	 * @param dest Process destination
	 * @param src Local source
	 * @param size Size to read
	 * @return true if the memory was written, false otherwise
	 */
	bool WriteMemory(uintptr_t dest, const void* src, size_t size) const;
	/*
	 * Write a location in a stream
	 * @param out output stream
	 * @param location location
	 * @return out with module+relative_location
	 */
	std::ostream& WriteLocation(std::ostream& out, uintptr_t location) const;
	/*
	 * Write a location in a stream
	 * @param out output stream
	 * @param location location
	 * @return out with module+relative_location
	 */
	std::wostream& WriteLocation(std::wostream& out, uintptr_t location) const;
	/*
	 * Get a location for rendering
	 * @param location location
	 * @return process location usable with a stream or a format
	 */
	inline ProcessLocation GetLocation(uintptr_t location) const {
		return ProcessLocation{ *this, location };
	}

	/*
	 * Inject a dll in the process
	 * @param dll Dll file
	 * @return if the dll was injected
	 */
	bool LoadDll(const char* dll);
	/*
	 * Read memory integer type
	 * @param location Process location to read
	 * @return value or 0 if nothing was read
	 */
	template<typename Type>
	inline Type ReadMemory(uintptr_t location) const {
		Type buff;
		if (!ReadMemory(&buff, location, sizeof(buff))) {
			buff = 0;
		}
		return buff;
	}
	/*
	 * Is a pointer inside the module
	 * @param ptr Process location
	 * @return if the location is inside the module
	 */
	bool IsInsideModule(uintptr_t ptr) const;
	// process pid
	DWORD m_pid;
	/*
	 * @return the proccess modules
	 */
	inline const std::vector<ProcessModule>& modules() const {
		return m_modules;
	}
	/*
	 * Compute the modules names
	 */
	void ComputeModules();
	/*
	 * @return handle
	 */
	inline HANDLE GetHandle() const {
		return m_handle;
	}

	/*
	 * Scan for a pattern inside the main module
	 * @param pattern pattern to search
	 * @param start start location in the module
	 * @return scan result or 0 for no search
	 */
	inline uintptr_t Scan(const char* pattern, DWORD start = 0) {
		return (*this)[nullptr].Scan(pattern, start);
	}

	friend std::ostream& operator<<(std::ostream& os, const Process& obj);
private:
	static DWORD GetProcId(const wchar_t* name);
	static bool GetModuleAddress(DWORD pid, const wchar_t* name, uintptr_t* hModule, DWORD* modSize);
	std::vector<ProcessModule> m_modules{};
	ProcessModule m_invalid;
	uintptr_t m_modAddress{};
	DWORD m_modSize{};
	HANDLE m_handle{};
};

std::ostream& operator<<(std::ostream& os, const Process& obj);
std::ostream& operator<<(std::ostream& os, const ProcessModule& obj);
std::ostream& operator<<(std::ostream& os, const ProcessModuleExport& obj);
std::ostream& operator<<(std::ostream& os, const ProcessLocation& obj);
std::wostream& operator<<(std::wostream& os, const Process& obj);
std::wostream& operator<<(std::wostream& os, const ProcessModule& obj);
std::wostream& operator<<(std::wostream& os, const ProcessModuleExport& obj);
std::wostream& operator<<(std::wostream& os, const ProcessLocation& obj);

template<>
struct std::formatter<Process, char> : utils::BasicFormatter<Process> {};
template<>
struct std::formatter<ProcessModule, char> : utils::BasicFormatter<ProcessModule> {};
template<>
struct std::formatter<ProcessModuleExport, char> : utils::BasicFormatter<ProcessModuleExport> {};
template<>
struct std::formatter<ProcessLocation, char> : utils::BasicFormatter<ProcessLocation> {};