#pragma once
#include <Windows.h>

class Process;
class ProcessModule;
class ProcessModuleExport;

// ugly module I quickly wrote to access process memory
// supports:
// - Write/Read memory
// - Allocate memory
// - Find modules
// - Find exports
// - Call functions

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

	friend std::ostream& operator<<(std::ostream& os, const ProcessModuleExport& obj);
	friend std::wostream& operator<<(std::wostream& os, const ProcessModuleExport& obj);
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
	ProcessModuleExport& operator[](LPCCH name);
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
	 * Allocate memory in the process with protection
	 * @param size Size to allocate
	 * @param protection Protection
	 * @return pointer or null
	 */
	uintptr_t AllocateMemory(SIZE_T size, DWORD protection) const;
	/*
	 * Set the protection of a location in memory
	 * @param ptr Pointer to set
	 * @param size Size of the pointer
	 * @param flNewProtected New protect value
	 * @param lpflOldProtect Old protect value to write
	 * @return pointer or null
	 */
	bool SetMemoryProtection(uintptr_t ptr, SIZE_T size, DWORD flNewProtected, DWORD& lpflOldProtect) const;
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
	 * Inject a dll in the process
	 * @param dll Dll file
	 * @return if the dll was injected
	 */
	bool LoadDll(LPCCH dll);
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
	inline HANDLE GetHandle() {
		return m_handle;
	}

private:
	// param+ = stack, last first pushed
	inline void CallExtFuncBuildStack(process::AssemblerExp& asml, BYTE* params, int idx) {
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuildStack(process::AssemblerExp& asml, BYTE* params, int idx, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<Arg0*>(params + idx) = arg;
		CallExtFuncBuildStack(asml, params, idx + 8, args...);
		asml.mov(rax, qword_ptr(rcx, idx));
		asml.push(rax);
	}

	// param 3 -> R9  | XMM3
	inline void CallExtFuncBuild3(process::AssemblerExp& asml, BYTE* params) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild3(process::AssemblerExp& asml, BYTE* params, double arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<double*>(params + 24) = arg;
		CallExtFuncBuildStack(asml, params, 32, args...);
		asml.movsd(xmm3, qword_ptr(rcx, 24));
	}
	template<typename... Args>
	inline void CallExtFuncBuild3(process::AssemblerExp& asml, BYTE* params, float arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<float*>(params + 24) = arg;
		CallExtFuncBuildStack(asml, params, 32, args...);
		asml.movss(xmm3, dword_ptr(rcx, 24));
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild3(process::AssemblerExp& asml, BYTE* params, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<Arg0*>(params + 24) = arg;
		CallExtFuncBuildStack(asml, params, 32, args...);
		asml.mov(r9, qword_ptr(rcx, 24));
	}

	// param 2 -> R8  | XMM2
	inline void CallExtFuncBuild2(process::AssemblerExp& asml, BYTE* params) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild2(process::AssemblerExp& asml, BYTE* params, double arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<double*>(params + 16) = arg;
		CallExtFuncBuild3(asml, params, args...);
		asml.movsd(xmm2, qword_ptr(rcx, 16));
	}
	template<typename... Args>
	inline void CallExtFuncBuild2(process::AssemblerExp& asml, BYTE* params, float arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<float*>(params + 16) = arg;
		CallExtFuncBuild3(asml, params, args...);
		asml.movss(xmm2, dword_ptr(rcx, 16));
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild2(process::AssemblerExp& asml, BYTE* params, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<Arg0*>(params + 16) = arg;
		CallExtFuncBuild3(asml, params, args...);
		asml.mov(r8, qword_ptr(rcx, 16));
	}

	// param 1 -> RDX | XMM1
	inline void CallExtFuncBuild1(process::AssemblerExp& asml, BYTE* params) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild1(process::AssemblerExp& asml, BYTE* params, double arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<double*>(params + 8) = arg;
		CallExtFuncBuild2(asml, params, args...);
		asml.movsd(xmm1, qword_ptr(rcx, 8));
	}
	template<typename... Args>
	inline void CallExtFuncBuild1(process::AssemblerExp& asml, BYTE* params, float arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<float*>(params + 8) = arg;
		CallExtFuncBuild2(asml, params, args...);
		asml.movss(xmm1, dword_ptr(rcx, 8));
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild1(process::AssemblerExp& asml, BYTE* params, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<Arg0*>(params + 8) = arg;
		CallExtFuncBuild2(asml, params, args...);
		asml.mov(rdx, qword_ptr(rcx, 8));
	}

	// param 0 -> RCX | XMM0
	inline void CallExtFuncBuild0(process::AssemblerExp& asml, BYTE* params) {
	}
	template<typename... Args>
	inline void CallExtFuncBuild0(process::AssemblerExp& asml, BYTE* params, double arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<double*>(params) = arg;
		CallExtFuncBuild1(asml, params, args...);
		asml.movsd(xmm0, qword_ptr(rcx));
	}
	template<typename... Args>
	inline void CallExtFuncBuild0(process::AssemblerExp& asml, BYTE* params, float arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<float*>(params) = arg;
		CallExtFuncBuild1(asml, params, args...);
		asml.movss(xmm0, dword_ptr(rcx));
	}
	template<typename Arg0, typename... Args>
	inline void CallExtFuncBuild0(process::AssemblerExp& asml, BYTE* params, Arg0 arg, Args... args) {
		using namespace asmjit::x86;
		*reinterpret_cast<Arg0*>(params) = arg;
		CallExtFuncBuild1(asml, params, args...);
		asml.mov(rcx, qword_ptr(rcx));
	}


public:
	/*
	 * Call a function in the target process
	 * @param location function location
	 * @param args function parameters
	 * @return if the function was called correctly
	 */
	template<typename... Args>
	bool Call(uintptr_t location, Args... args) {
		using namespace asmjit::x86;

		constexpr auto argcount = sizeof...(args);
		BYTE params[argcount * 8 + 1]; // add +1 to avoid 0 case
		auto& jr = process::GetJitRuntime();
		asmjit::CodeHolder holder;
		holder.init(jr.environment());
		void* fullInt = nullptr;
		{
			using namespace asmjit::x86;
			process::AssemblerExp asml{ &holder };
			// https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170#parameter-passing

			// param0 = RCX | XMM0
			// param1 = RDX | XMM1
			// param2 = R8  | XMM2
			// param3 = R9  | XMM3
			// param+ = stack, last first pushed
			
			// build params into asm

			CallExtFuncBuild0(asml, params, args...);

			asml.sub(rsp, 0x20);
			asml.mov(rax, location);
			asml.call(rax);
			// clean the push and store back the func
			asml.add(rsp, 0x20 + (argcount > 4 ? argcount - 4 : 0) * 8);
			asml.ret();

		}
		auto size = holder.codeSize();
		jr.add(&fullInt, &holder);

		if (!fullInt) {
			return false;
		}

		auto allocsize = size + 16;
		auto allocargsize = sizeof(params) - 1;
		auto func = AllocateMemory(allocsize, PAGE_EXECUTE_READWRITE);
		auto argptr = allocargsize ? AllocateMemory(allocargsize, PAGE_READWRITE) : 0;
		auto funcalign = utils::Aligned<UINT64>(func);

		if (!func || (allocargsize && !argptr)) {
			std::cerr << "bad alloc\n";
			FreeMemory(func, allocsize);
			FreeMemory(argptr, allocargsize);
			return false;
		}


		if (!WriteMemory(funcalign, fullInt, size) || !(!allocargsize || WriteMemory(argptr, params, allocargsize))) {
			std::cerr << "bad write\n";
			FreeMemory(func, allocsize);
			FreeMemory(argptr, allocargsize);
			return false;
		}

		auto thr = Exec(funcalign, argptr);

		bool ok;
		if (thr == INVALID_HANDLE_VALUE || !thr) {
			ok = false;
		}
		else {
			ok = true;
			WaitForSingleObject(thr, INFINITE);
			CloseHandle(thr);
		}

		FreeMemory(func, allocsize);
		FreeMemory(argptr, allocargsize);
		return ok;
	}

	/*
	 * Call a function in the target process
	 * @param exp function export
	 * @param args function parameters
	 * @return if the function was called correctly
	 */
	template<typename... Args>
	inline bool Call(const ProcessModuleExport& exp, Args... args) {
		if (!exp) return false;
		return Call(exp.m_location, args...);
	}

	friend std::ostream& operator<<(std::ostream& os, const Process& obj);
private:
	static DWORD GetProcId(LPCWCH name);
	static bool GetModuleAddress(DWORD pid, LPCWCH name, uintptr_t* hModule, DWORD* modSize);
	std::vector<ProcessModule> m_modules{};
	ProcessModule m_invalid;
	uintptr_t m_modAddress;
	DWORD m_modSize;
	HANDLE m_handle;
};

std::ostream& operator<<(std::ostream& os, const Process& obj);
std::ostream& operator<<(std::ostream& os, const ProcessModule& obj);
std::ostream& operator<<(std::ostream& os, const ProcessModuleExport& obj);
std::wostream& operator<<(std::wostream& os, const Process& obj);
std::wostream& operator<<(std::wostream& os, const ProcessModule& obj);
std::wostream& operator<<(std::wostream& os, const ProcessModuleExport& obj);
