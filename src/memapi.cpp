#include <includes.hpp>

DWORD process::GetProcId(LPCWCH name) {
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

bool process::GetModuleAddress(DWORD pid, LPCWCH name, uintptr_t* hModule, DWORD* modSize) {
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

process::process(LPCWCH processName, LPCWCH moduleName) {
	m_pid = GetProcId(processName);

	if (!m_pid || !GetModuleAddress(m_pid, moduleName, &m_modAddress, &m_modSize)) {
		m_modAddress = 0;
		m_modSize = 0;
	}
	m_handle = NULL;
}

process::~process() {
	Close();
}

bool process::Open() {
	if (!this) {
		return false;
	}
	if (m_handle) {
		return true; // already open
	}
	m_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, m_pid);

	return m_handle != NULL;
}

void process::Close() {
	if (m_handle) {
		CloseHandle(m_handle);
		m_handle = NULL;
	}
}

bool process::operator!() const {
	return m_pid == 0;
}

bool process::operatorbool() const {
	return m_pid != 0;
}

uintptr_t process::operator[](UINT64 offset) const {
	return m_modAddress + offset;
}

INT64 process::GetModuleRelativeOffset(uintptr_t ptr) const {
	return ptr - m_modAddress;
}

uintptr_t process::AllocateMemory(SIZE_T size) const {
	if (m_handle) {
		return reinterpret_cast<uintptr_t>(VirtualAllocEx(m_handle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	}
	return NULL;
}

bool process::IsInsideModule(uintptr_t ptr) const {
	return ptr >= m_modAddress && ptr < m_modAddress + m_modSize;
}

void process::FreeMemory(uintptr_t ptr, SIZE_T size) const {
	if (m_handle) {
		VirtualFreeEx(m_handle, reinterpret_cast<LPVOID>(ptr), size, MEM_FREE);
	}
}

bool process::ReadMemory(LPVOID dest, uintptr_t src, SIZE_T size) const {
	if (m_handle) {
		SIZE_T out = 0;
		if (!ReadProcessMemory(m_handle, reinterpret_cast<LPVOID>(src), dest, size, &out)) {
			return false;
		}
		return out == size;
	}
	return false;
}

bool process::WriteMemory(uintptr_t dest, LPCVOID src, SIZE_T size) const {
	if (m_handle) {
		SIZE_T out = 0;
		if (!WriteProcessMemory(m_handle, reinterpret_cast<LPVOID>(dest), src, size, &out)) {
			return false;
		}
		return out == size;
	}
	return false;
}
