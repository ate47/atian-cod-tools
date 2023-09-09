#pragma once
#include <Windows.h>

class Process;
class ProcessModule;
class ProcessModuleExport;

// ugly module I quickly wrote to access process memory

// Process module export
class ProcessModuleExport {
public:
	std::unique_ptr<CHAR[]> m_name;
	uintptr_t m_location;
	WORD m_ordinal = 0;

	ProcessModuleExport(ProcessModule& module, LPCCH name, uintptr_t location, WORD ordinal);
	// @return if the export is invalid
	inline bool operator!() const {
		return m_location == 0;
	}
	// @return if the export is valid
	inline bool operatorbool() const {
		return m_location != 0;
	}
private:
	ProcessModule& m_module;
};

// Process module
class ProcessModule {
public:
	Process& m_parent;
	CHAR name[MAX_MODULE_NAME32 + 1] = { 0 };
	CHAR path[MAX_PATH + 1] = { 0 };
	uintptr_t start = 0;
	DWORD size = 0;
	HANDLE handle = 0;

	ProcessModule(Process& parent);

	/*
	 * Get a relative offset in a module
	 * @param ptr Pointer
	 * @return Location in the module
	 */
	INT64 GetRelativeOffset(uintptr_t ptr) const;
	// @return if the module is invalid
	inline bool operator!() const {
		return handle == INVALID_HANDLE_VALUE;
	}
	// @return if the module is valid
	inline bool operatorbool() const {
		return handle != INVALID_HANDLE_VALUE;
	}

	/*
	 * Compute the export names
	 */
	void ComputeExports();
	/*
	 * Search for an export name
	 * @param name name
	 * @return export location or 0 if it can't be find
	 */
	ProcessModuleExport& operator[](LPCCH name);
	/*
	 * @return the proccess modules
	 */
	inline const std::vector<ProcessModuleExport>& exports() const {
		return m_exports;
	}
private:
	std::vector<ProcessModuleExport> m_exports{};
	ProcessModuleExport m_invalid;
	bool m_export_computed = false;
};

// Process
class Process {
public:
	Process(LPCWCH processName, LPCWCH moduleName = NULL);
	Process(LPCCH processName, LPCCH moduleName = NULL);
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
	uintptr_t operator[](UINT64 offset) const;
	/*
	 * Get a module inside the process
	 * @param module Module name
	 * @return Module, if the handle is equals to INVALID_HANDLE_VALUE, the module isn't valid
	 */
	ProcessModule& operator[](LPCCH module);
	/*
	 * Get a module inside the process
	 * @param module Module name
	 * @return Module, if the handle is equals to INVALID_HANDLE_VALUE, the module isn't valid
	 */
	const ProcessModule& operator[](LPCCH module) const;
	/*
	 * Get the relative offset of a ptr inside the module
	 * @param ptr Pointer
	 * @return relative offset, can be negative if the pointer is before the module
	 */
	INT64 GetModuleRelativeOffset(uintptr_t ptr) const;
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
	uintptr_t AllocateMemory(SIZE_T size) const;
	/*
	 * Free memory in the process
	 * @param ptr Pointer to free
	 * @param size Pointer size
	 */
	void FreeMemory(uintptr_t ptr, SIZE_T size) const;
	/*
	 * Read memory from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Size to read
	 * @return true if the memory was read, false otherwise
	 */
	bool ReadMemory(LPVOID dest, uintptr_t src, SIZE_T size) const;
	/*
	 * Read c string from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Max buffer size to read
	 * @return read string size, -2 if the buffer is too small, -1 if can't read
	 */
	INT64 ReadString(LPCH dest, uintptr_t src, SIZE_T size) const;
	/*
	 * Read wc string from a source to a destination
	 * @param dest Local destination
	 * @param src Process source
	 * @param size Max buffer size to read
	 * @return read string size, -2 if the buffer is too small, -1 if can't read
	 */
	INT64 ReadWString(LPWCH dest, uintptr_t src, SIZE_T size) const;
	/*
	 * Write memory from a source to a destination
	 * @param dest Process destination
	 * @param src Local source
	 * @param size Size to read
	 * @return true if the memory was written, false otherwise
	 */
	bool WriteMemory(uintptr_t dest, LPCVOID src, SIZE_T size) const;
	/*
	 * Write a location in a stream
	 * @param out output stream
	 * @param location location
	 * @return out with module+relative_location
	 */
	std::ostream& WriteLocation(std::ostream& out, uintptr_t location) const;
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
private:
	static DWORD GetProcId(LPCWCH name);
	static bool GetModuleAddress(DWORD pid, LPCWCH name, uintptr_t* hModule, DWORD* modSize);
	std::vector<ProcessModule> m_modules{};
	ProcessModule m_invalid;
	uintptr_t m_modAddress;
	DWORD m_modSize;
	HANDLE m_handle;
};
