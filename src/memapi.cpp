#include <includes.hpp>

static ProcessModule g_invalidModule = { .handle = INVALID_HANDLE_VALUE };

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

Process::Process(LPCWCH processName, LPCWCH moduleName) {
	m_pid = GetProcId(processName);

	if (!m_pid || !GetModuleAddress(m_pid, moduleName, &m_modAddress, &m_modSize)) {
		m_modAddress = 0;
		m_modSize = 0;
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
	if (m_handle) {
		return reinterpret_cast<uintptr_t>(VirtualAllocEx(m_handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	}
	return NULL;
}

bool Process::IsInsideModule(uintptr_t ptr) const {
	return ptr >= m_modAddress && ptr < m_modAddress + m_modSize;
}

void Process::FreeMemory(uintptr_t ptr, SIZE_T size) const {
	if (m_handle) {
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
				auto& e = m_modules.emplace_back();

				// convert to ascii string, assume that the name is correct
				LPWCH p = entry.szModule;
				LPCH s = e.name;

				while (*p) {
					*(s++) = (CHAR)*(p++);
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

const ProcessModule& Process::operator[](LPCCH module) const {
	for (const auto& mod : m_modules) {
		if (!strncmp(mod.name, module, MAX_MODULE_NAME32)) {
			return mod;
		}
	}

	return g_invalidModule;
}

const ProcessModule& Process::GetLocationModule(uintptr_t ptr) const {
	for (const auto& mod : m_modules) {
		if (ptr >= mod.start && ptr < mod.start + mod.size) {
			return mod;
		}
	}

	return g_invalidModule;
}
