#include <includes.hpp>

ProcessModule::ProcessModule(Process& parent) : m_parent(parent), m_invalid(*this, "invalid", 0, 0) {
}

INT64 ProcessModule::GetRelativeOffset(uintptr_t ptr) const {
	return ptr - start;
}

DWORD Process::GetProcId(LPCWCH name) {
	DWORD pid = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Process32First(hSnap, &entry)) {
			do {
				if (!_wcsicmp(entry.szExeFile, name)) {
					pid = entry.th32ProcessID;
				}
			} while (Process32Next(hSnap, &entry));
		}
	}
	CloseHandle(hSnap);
	return pid;
}

bool Process::GetModuleAddress(DWORD pid, LPCWCH name, uintptr_t* hModule, DWORD* modSize) {
	*hModule = 0;
	*modSize = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Module32First(hSnap, &entry)) {
			if (!name) {
				*hModule = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
				*modSize = entry.modBaseSize;
			}
			else {
				do {
					if (!_wcsicmp(entry.szModule, name)) {
						*hModule = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
						*modSize = entry.modBaseSize;
					}
				} while (Module32Next(hSnap, &entry));
			}
		}
	}
	CloseHandle(hSnap);
	return *hModule != 0;
}

Process::Process(LPCWCH processName, LPCWCH moduleName) : m_invalid(ProcessModule(*this)) {
	m_pid = GetProcId(processName);

	if (!m_pid || !GetModuleAddress(m_pid, moduleName, &m_modAddress, &m_modSize)) {
		m_modAddress = 0;
		m_modSize = 0;
	}
	m_handle = NULL;
}

Process::Process(LPCCH processName, LPCCH moduleName) : m_invalid(ProcessModule(*this)) {
	WCHAR processNameW[MAX_PATH + 1] = { 0 };
	size_t i;
	for (i = 0; processName[i]; i++) {
		processNameW[i] = (WCHAR)processName[i];
	}
	processNameW[i] = 0;

	m_pid = GetProcId(processNameW);

	if (moduleName) {
		WCHAR moduleNameW[MAX_PATH + 1] = { 0 };
		for (i = 0; moduleName[i]; i++) {
			moduleNameW[i] = (WCHAR)moduleName[i];
		}
		moduleNameW[i] = 0;
		if (!m_pid || !GetModuleAddress(m_pid, moduleNameW, &m_modAddress, &m_modSize)) {
			m_modAddress = 0;
			m_modSize = 0;
		}
	}
	else {
		if (!m_pid || !GetModuleAddress(m_pid, NULL, &m_modAddress, &m_modSize)) {
			m_modAddress = 0;
			m_modSize = 0;
		}
	}

	m_handle = NULL;

}

Process::~Process() {
	Close();
}

bool Process::Open() {
	if (!this) {
		return false;
	}
	if (m_handle) {
		return true; // already open
	}
	m_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, m_pid);

	ComputeModules();

	return m_handle != NULL;
}

void Process::Close() {
	if (m_handle) {
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	m_modules.clear();
}

bool Process::operator!() const {
	return m_pid == 0;
}

bool Process::operatorbool() const {
	return m_pid != 0;
}

uintptr_t Process::operator[](UINT64 offset) const {
	return m_modAddress + offset;
}

INT64 Process::GetModuleRelativeOffset(uintptr_t ptr) const {
	return ptr - m_modAddress;
}

uintptr_t Process::AllocateMemory(SIZE_T size) const {
	return AllocateMemory(size, PAGE_READWRITE);
}

uintptr_t Process::AllocateMemory(SIZE_T size, DWORD protection) const {
	if (m_handle) {
		return reinterpret_cast<uintptr_t>(VirtualAllocEx(m_handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, protection));
	}
	return NULL;
}

bool Process::SetMemoryProtection(uintptr_t ptr, SIZE_T size, DWORD flNewProtected, DWORD& lpflOldProtect) const {
	if (m_handle) {
		return VirtualProtectEx(m_handle, reinterpret_cast<LPVOID>(ptr), size, flNewProtected, &lpflOldProtect);
	}
	return false;
}

bool Process::IsInsideModule(uintptr_t ptr) const {
	return ptr >= m_modAddress && ptr < m_modAddress + m_modSize;
}


HANDLE Process::Exec(uintptr_t location, uintptr_t arg) const {
	if (!m_handle) {
		return INVALID_HANDLE_VALUE;
	}

	 return CreateRemoteThread(
		m_handle, 0, 0,
		reinterpret_cast<LPTHREAD_START_ROUTINE>(location), 
		reinterpret_cast<LPVOID>(arg), 
		0, 0
	 );
}

void Process::FreeMemory(uintptr_t ptr, SIZE_T size) const {
	if (m_handle && ptr) {
		VirtualFreeEx(m_handle, reinterpret_cast<LPVOID>(ptr), size, MEM_FREE);
	}
}

bool Process::ReadMemory(LPVOID dest, uintptr_t src, SIZE_T size) const {
	if (m_handle) {
		SIZE_T out = 0;
		if (!ReadProcessMemory(m_handle, reinterpret_cast<LPVOID>(src), dest, size, &out)) {
			return false;
		}
		return out == size;
	}
	return false;
}

INT64 Process::ReadString(LPCH dest, uintptr_t src, SIZE_T size) const {
	INT64 len = 0;

	CHAR c = -1;
	// TODO: use buffer
	while (c) {
		if (!ReadMemory(&c, src++, 1)) {
			return -1; // can't read memory
		}
		if (len == size) {
			return -2; // not enought space
		}
		dest[len++] = c;
	}

	return len;
}

INT64 Process::ReadWString(LPWCH dest, uintptr_t src, SIZE_T size) const {
	INT64 len = 0;

	WCHAR c = -1;
	// TODO: use buffer
	while (c) {
		if (!ReadMemory(&c, src++, 1)) {
			return -1; // can't read memory
		}
		if (len == size) {
			return -2; // not enought space
		}
		dest[len++] = c;
	}

	return len;
}

bool Process::WriteMemory(uintptr_t dest, LPCVOID src, SIZE_T size) const {
	if (m_handle) {
		SIZE_T out = 0;
		if (!WriteProcessMemory(m_handle, reinterpret_cast<LPVOID>(dest), src, size, &out)) {
			return false;
		}
		return out == size;
	}
	return false;
}

void Process::ComputeModules() {
	m_modules.clear();

	if (!this || !m_handle) {
		return;
	}

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pid);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Module32First(hSnap, &entry)) {
			do {
				ProcessModule& e = m_modules.emplace_back(ProcessModule(*this));

				// convert to ascii string, assume that the name is correct
				LPWCH p = entry.szModule;
				LPCH s = e.name;

				while (*p) {
					*(s++) = (CHAR) * (p++);
				}
				*s = 0;

				p = entry.szExePath;
				s = e.path;

				while (*p) {
					*(s++) = (CHAR) * (p++);
				}
				*s = 0;
				
				e.handle = entry.hModule;
				e.size = entry.modBaseSize;
				e.start = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			} while (Module32Next(hSnap, &entry));
		}
	}
	CloseHandle(hSnap);
}

std::ostream& Process::WriteLocation(std::ostream& out, uintptr_t location) const {
	const auto& mod = GetLocationModule(location);
	if (mod.handle == INVALID_HANDLE_VALUE) {
		// not in a module
		out << std::hex << location;
	}
	else {
		out << mod.name << "+" << std::hex << mod.GetRelativeOffset(location);
	}
	return out;
}

bool Process::LoadDll(LPCCH dll) {
	ProcessModuleExport& rLoadLibraryA =  (*this)["kernel32.dll"]["LoadLibraryA"];

	if (!rLoadLibraryA) {
		return false;
	}
	auto dllsize = strlen(dll) + 1;
	auto argptr = AllocateMemory(dllsize, PAGE_READWRITE);

	if (!argptr || !WriteMemory(argptr, dll, dllsize)) {
		FreeMemory(argptr, dllsize);
		return false;
	}

	auto thr = Exec(rLoadLibraryA.m_location, argptr);

	if (thr == INVALID_HANDLE_VALUE || !thr) {
		FreeMemory(argptr, dllsize);
		return false;
	}
	WaitForSingleObject(thr, INFINITE);
	CloseHandle(thr);
	FreeMemory(argptr, dllsize);
	// we need to recompute the modules because a new one was injected
	ComputeModules();

	return true;
}

ProcessModule& Process::operator[](LPCCH module) {
	if (!module) {
		if (m_modules.size()) {
			return m_modules[0];
		}
		return m_invalid;
	}
	for (auto& mod : m_modules) {
		if (!_strcmpi(mod.name, module)) {
			return mod;
		}
	}

	return m_invalid;
}

const ProcessModule& Process::operator[](LPCCH module) const {
	if (!module) {
		if (m_modules.size()) {
			return m_modules[0];
		}
		return m_invalid;
	}
	for (const auto& mod : m_modules) {
		if (!_strcmpi(mod.name, module)) {
			return mod;
		}
	}

	return m_invalid;
}

const ProcessModule& Process::GetLocationModule(uintptr_t ptr) const {
	for (const auto& mod : m_modules) {
		if (ptr >= mod.start && ptr < mod.start + mod.size) {
			return mod;
		}
	}

	return m_invalid;
}

void ProcessModule::ComputeExports() {
	m_export_computed = true;


	if (!this) {
		return;
	}

	IMAGE_DOS_HEADER dosHeader;

	if (!m_parent.ReadMemory(&dosHeader, start, sizeof(dosHeader)) || dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
		std::cerr << "Can't read dos module header\n";
		return;
	}
	IMAGE_NT_HEADERS ntHeader;

	if (!m_parent.ReadMemory(&ntHeader, start + dosHeader.e_lfanew, sizeof(ntHeader)) || ntHeader.Signature != IMAGE_NT_SIGNATURE) {
		std::cerr << "Can't read nt module header\n";
		return;
	}

	IMAGE_EXPORT_DIRECTORY exports;

	if (!m_parent.ReadMemory(&exports, start + ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress, sizeof(exports))) {
		std::cerr << "Can't read module exports\n";
		return;
	}

	CHAR name[2000];


	auto names = std::make_unique<DWORD[]>(exports.NumberOfNames);
	if (!m_parent.ReadMemory(&names[0], start + exports.AddressOfNames, sizeof(names[0]) * exports.NumberOfNames)) {
		std::cerr << "Can't read module exports names\n";
		return;
	}
	auto ordinals = std::make_unique<WORD[]>(exports.NumberOfNames);
	if (!m_parent.ReadMemory(&ordinals[0], start + exports.AddressOfNameOrdinals, sizeof(ordinals[0]) * exports.NumberOfNames)) {
		std::cerr << "Can't read module exports ordinals\n";
		return;
	}
	auto functions = std::make_unique<DWORD[]>(exports.NumberOfFunctions);
	if (!m_parent.ReadMemory(&functions[0], start + exports.AddressOfFunctions, sizeof(functions[0]) * (size_t)(exports.NumberOfFunctions))) {
		std::cerr << "Can't read module exports functions\n";
		return;
	}

	bool found = false;
	for (size_t i = 0; i < exports.NumberOfNames; i++) {
		uintptr_t loc = start + names[i];
		if (m_parent.ReadString(name, loc, 2000) < 0) {
			std::cerr << "Can't read name at loc 0x" << std::hex << loc << " " << std::dec << i << "/" << exports.NumberOfNames << "\n";
			return;
		}

		WORD ord = ordinals[i];
		m_exports.emplace_back(*this, name, start + functions[ord], (WORD)(exports.Base + ord));
	}
}

ProcessModuleExport& ProcessModule::operator[](LPCCH name) {
	if (!m_export_computed) {
		ComputeExports();
	}
	for (auto& exp : m_exports) {
		if (!_strcmpi(&exp.m_name[0], name)) {
			return exp;
		}
	}
	return m_invalid;
}

ProcessModuleExport::ProcessModuleExport(ProcessModule& module, LPCCH name, uintptr_t location, WORD ordinal)
	: m_module(module), m_location(location), m_name(std::make_unique<CHAR[]>(strlen(name) + 1)), m_ordinal(ordinal) {
	strcpy_s(&m_name[0], strlen(name) + 1, name);
}
