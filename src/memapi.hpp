#pragma once
#include <Windows.h>

struct ProcessModule {
	CHAR name[MAX_MODULE_NAME32 + 1];
	uintptr_t start;
	DWORD size;
	HANDLE handle;

	/*
	 * Get a relative offset in a module
	 * @param ptr Pointer
	 * @return Location in the module
	 */
	INT64 GetRelativeOffset(uintptr_t ptr) const;
};
// ugly class I quickly wrote to access process memory
class Process {
public:
	Process(LPCWCH processName, LPCWCH moduleName = NULL);
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
private:
	static DWORD GetProcId(LPCWCH name);
	static bool GetModuleAddress(DWORD pid, LPCWCH name, uintptr_t* hModule, DWORD* modSize);
	std::vector<ProcessModule> m_modules{};
	void ComputeModules();
	uintptr_t m_modAddress;
	DWORD m_modSize;
	HANDLE m_handle;
};