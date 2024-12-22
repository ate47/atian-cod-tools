#include <includes_shared.hpp>
#include "memapi.hpp"
#include <utils/utils.hpp>
#include <hook/library.hpp>

ProcessModule::ProcessModule(Process& parent) : m_parent(parent), m_invalid(*this, "invalid", 0, 0) {
}

int64_t ProcessModule::GetRelativeOffset(uintptr_t ptr) const {
	return ptr - start;
}

void ProcessMemory::Free() {
	if (*this) proc.FreeMemory(ptr, size);
}

DWORD Process::GetProcId(const wchar_t* name) {
	DWORD pid = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Process32FirstW(hSnap, &entry)) {
			do {
				if (!_wcsicmp(entry.szExeFile, name)) {
					pid = entry.th32ProcessID;
				}
			} while (Process32NextW(hSnap, &entry));
		}
	}
	CloseHandle(hSnap);
	return pid;
}

bool Process::GetModuleAddress(DWORD pid, const wchar_t* name, uintptr_t* hModule, DWORD* modSize) {
	*hModule = 0;
	*modSize = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Module32FirstW(hSnap, &entry)) {
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
				} while (Module32NextW(hSnap, &entry));
			}
		}
	}
	CloseHandle(hSnap);
	return *hModule != 0;
}

Process::Process(const wchar_t* processName, const wchar_t* moduleName) : m_invalid(ProcessModule(*this)) {
	if (processName && *processName) {
		m_pid = GetProcId(processName);

		if (!m_pid || !GetModuleAddress(m_pid, moduleName, &m_modAddress, &m_modSize)) {
			m_modAddress = 0;
			m_modSize = 0;
		}
	}
	else {
		m_pid = 0;
	}
	m_handle = NULL;
}

Process::Process(const char* processName, const char* moduleName) : m_invalid(ProcessModule(*this)) {
	if (processName && *processName) {
		wchar_t processNameW[MAX_PATH + 1] = { 0 };
		size_t i;
		for (i = 0; processName[i]; i++) {
			processNameW[i] = (wchar_t)processName[i];
		}
		processNameW[i] = 0;

		m_pid = GetProcId(processNameW);

		if (moduleName) {
			wchar_t moduleNameW[MAX_PATH + 1] = { 0 };
			for (i = 0; moduleName[i]; i++) {
				moduleNameW[i] = (wchar_t)moduleName[i];
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
	}
	else {
		m_pid = 0;
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
	m_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_ALL_ACCESS, FALSE, m_pid);

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

uintptr_t Process::operator[](uint64_t offset) const {
	return m_modAddress + offset;
}

int64_t Process::GetModuleRelativeOffset(uintptr_t ptr) const {
	return ptr - m_modAddress;
}

uintptr_t Process::AllocateMemory(size_t size) const {
	return AllocateMemory(size, PAGE_READWRITE);
}

uintptr_t Process::AllocateMemory(size_t size, DWORD protection) const {
	if (m_handle) {
		return reinterpret_cast<uintptr_t>(VirtualAllocEx(m_handle, 0, size, MEM_COMMIT | MEM_RESERVE, protection));
	}
	return NULL;
}
uintptr_t Process::AllocateString(const char* string, size_t* sizeOut) const {
	if (m_handle) {
		size_t len{ strlen(string) + 1 };
		if (sizeOut) {
			*sizeOut = len;
		}
		uintptr_t strptr{ AllocateMemory(len) };
		if (!strptr) {
			return NULL;
		}
		if (!WriteMemory(strptr, string, len)) {
			FreeMemory(strptr, len);
			return NULL;
		}
		return strptr;
	}
	return NULL;
}

bool Process::SetMemoryProtection(uintptr_t ptr, size_t size, DWORD flNewProtected, DWORD& lpflOldProtect) const {
	if (m_handle) {
		return VirtualProtectEx(m_handle, reinterpret_cast<void*>(ptr), size, flNewProtected, &lpflOldProtect);
	}
	return false;
}

bool Process::IsInsideModule(uintptr_t ptr) const {
	return ptr >= m_modAddress && ptr < m_modAddress + m_modSize;
}

namespace {
	HANDLE NtCreateThreadEx(const Process& proc, uintptr_t location, uintptr_t arg) {
		static
			NTSTATUS
			(NTAPI * func)(
				_Out_ PHANDLE ThreadHandle,
				_In_ ACCESS_MASK DesiredAccess,
				_In_opt_ LPVOID  ObjectAttributes,
				_In_ HANDLE ProcessHandle,
				_In_ LPVOID StartRoutine,
				_In_opt_ uintptr_t Argument,
				_In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
				_In_ SIZE_T ZeroBits,
				_In_ SIZE_T StackSize,
				_In_ SIZE_T MaximumStackSize,
				_In_opt_ LPVOID AttributeList
				) {
			([]() {
				hook::library::Library ntdll{ "ntdll.dll", true };
				if (!ntdll) {
					throw std::runtime_error("Can't load ntdll.dll");
				}

				decltype(func) f{ reinterpret_cast<decltype(func)>(ntdll["NtCreateThreadEx"]) };
				if (!f) throw std::runtime_error("Can't load ntdll.dll::NtCreateThreadEx");
				return f;
				})()
		};

		HANDLE out{};
		NTSTATUS ret{ func(&out, SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL, nullptr, proc.GetHandle(), reinterpret_cast<void*>(location), arg, 0x6, 0, 0, 0, nullptr) };

		if (ret >= 0) return out;

		LOG_ERROR("Can't create thread 0x{:x}", (uint32_t)ret);
		return nullptr;
	}
}

HANDLE Process::Exec(uintptr_t location, uintptr_t arg) const {
	if (!m_handle) {
		return INVALID_HANDLE_VALUE;
	}
	return NtCreateThreadEx(*this, location, arg);
}

void Process::FreeMemory(uintptr_t ptr, size_t size) const {
	if (m_handle && ptr) {
		VirtualFreeEx(m_handle, reinterpret_cast<void*>(ptr), size, MEM_FREE);
	}
}

bool Process::ReadMemory(void* dest, uintptr_t src, size_t size) const {
	if (m_handle) {
		DWORD oldProtect{};
		if (!SetMemoryProtection(src, size, PAGE_EXECUTE_READWRITE, oldProtect)) {
			return false;
		}
		size_t out = 0;
		if (!ReadProcessMemory(m_handle, reinterpret_cast<void*>(src), dest, size, &out)) {
			return false;
		}
		if (!SetMemoryProtection(src, size, oldProtect, oldProtect)) {
			return false;
		}
		return out == size;
	}
	return false;
}

const char* Process::ReadStringTmp(uintptr_t src, const char* defaultVal) const {
	static char buffer[10][0x200];
	static size_t bufferId{};

	auto& buff = buffer[bufferId = (bufferId + 1) % ARRAYSIZE(buffer)];

	if (ReadString(buff, src, ARRAYSIZE(buff)) < 0) {
		return defaultVal;
	}

	return buff;
}

const wchar_t* Process::ReadStringTmpW(uintptr_t src, const wchar_t* defaultVal) const {
	static wchar_t buffer[10][0x200];
	static size_t bufferId{};

	auto& buff = buffer[bufferId = (bufferId + 1) % ARRAYSIZE(buffer)];

	if (ReadWString(buff, src, ARRAYSIZE(buff)) < 0) {
		return defaultVal;
	}

	return buff;
}

int64_t Process::ReadString(char* dest, uintptr_t src, size_t size) const {
	int64_t len = 0;

	char c = -1;
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

int64_t Process::ReadWString(wchar_t* dest, uintptr_t src, size_t size) const {
	int64_t len = 0;

	wchar_t c = -1;
	// TODO: use buffer
	while (c) {
		if (!ReadMemory(&c, src++, 2)) {
			return -1; // can't read memory
		}
		if (len == size) {
			return -2; // not enought space
		}
		dest[len++] = c;
	}

	return len;
}

bool Process::WriteMemory(uintptr_t dest, const void* src, size_t size) const {
	if (m_handle) {
		size_t out = 0;
		DWORD oldProtect{};
		int data{};
		if (!SetMemoryProtection(dest, size, PAGE_EXECUTE_READWRITE, oldProtect)) {
			return false;
		}
		if (!WriteProcessMemory(m_handle, reinterpret_cast<void*>(dest), src, size, &out)) {
			return false;
		}
		if (!SetMemoryProtection(dest, size, oldProtect, oldProtect)) {
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

		if (Module32FirstW(hSnap, &entry)) {
			do {
				ProcessModule& e = m_modules.emplace_back(ProcessModule(*this));

				// convert to ascii string, assume that the name is correct
				wchar_t* p = entry.szModule;
				char* s = e.name;

				while (*p) {
					*(s++) = (char) * (p++);
				}
				*s = 0;

				p = entry.szExePath;
				s = e.path;

				while (*p) {
					*(s++) = (char) * (p++);
				}
				*s = 0;
				
				e.handle = entry.hModule;
				e.size = entry.modBaseSize;
				e.start = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			} while (Module32NextW(hSnap, &entry));
		}
	}
	CloseHandle(hSnap);
}

std::ostream& Process::WriteLocation(std::ostream& out, uintptr_t location) const {
	if (!location) {
		return out << "NULL";
	}
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

std::wostream& Process::WriteLocation(std::wostream& out, uintptr_t location) const {
	if (!location) {
		return out << L"NULL";
	}
	const auto& mod = GetLocationModule(location);
	if (mod.handle == INVALID_HANDLE_VALUE) {
		// not in a module
		out << std::hex << location;
	}
	else {
		out << utils::StrToWStr(mod.name) << L"+" << std::hex << mod.GetRelativeOffset(location);
	}
	return out;
}

bool Process::LoadDll(const char* dll) {
	ProcessModuleExport& rLoadLibraryA = (*this)["Kernel32.dll"]["LoadLibraryA"];

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

ProcessModule& Process::operator[](const char* module) {
	if (!module || !*module) {
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

const ProcessModule& Process::operator[](const char* module) const {
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

uintptr_t ProcessModule::Scan(const char* pattern, DWORD start_ptr) {
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
	
	constexpr size_t lazySize = 0x10000000;

	uintptr_t start{ this->start };
	uintptr_t end{ this->start + lazySize - mask.size() };

	byte buffer[0x1000];

	if (sizeof(buffer) < mask.size()) {
		throw std::runtime_error(utils::va("Pattern too big! %s", pattern)); // wtf?
	}

	size_t delta = sizeof(buffer) - mask.size();
	uintptr_t current = start;

	while (current < end) {
		if (!m_parent.ReadMemory(buffer, current, sizeof(buffer))) {
			current += delta;
			continue;
		}
		for (size_t off = 0; off < delta; off++) {
			size_t i;
			for (i = 0; i < mask.size(); i++) {
				if ((buffer[off + i] & mask[i]) != searched[i]) {
					break;
				}
			}
			if (i == mask.size()) {
				return current + off;
			}
		}

		current += delta;
	}

	return 0;
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

	char name[2000];


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

ProcessModuleExport& ProcessModule::operator[](const char* name) {
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

ProcessModuleExport::ProcessModuleExport(ProcessModule& module, const char* name, uintptr_t location, WORD ordinal)
	: m_module(module), m_location(location), m_name(std::make_unique<char[]>(strlen(name) + 1)), m_ordinal(ordinal) {
	strcpy_s(&m_name[0], strlen(name) + 1, name);
}
ProcessLocation::ProcessLocation(const Process& proc, uintptr_t loc) : proc(proc), loc(loc) {}

std::ostream& operator<<(std::ostream& os, const Process& obj) {
	if (!obj) {
		return os << "[BAD_PROCESS]";
	}
	return os << "[proc: pid=" << obj.m_pid << "]";
}

std::ostream& operator<<(std::ostream& os, const ProcessModule& obj) {
	if (!obj) {
		return os << "[BAD_MODULE]";
	}
	return os << "[" << obj.name << "@" << std::hex << obj.start << "]";
}

std::ostream& operator<<(std::ostream& os, const ProcessModuleExport& obj) {
	if (!obj) {
		return os << "[BAD_EXPORT]";
	}
	if (obj.m_location >= obj.m_module.start) {
		return os << "[" << obj.m_name << "@" << obj.m_module.name << "+" << std::hex << (obj.m_location - obj.m_module.start) << "]";
	}
	return os << "[" << obj.m_name << "@" << std::hex << obj.m_location << "]";
}

std::ostream& operator<<(std::ostream& os, const ProcessLocation& obj) {
	return obj.proc.WriteLocation(os, obj.loc);
}

std::wostream& operator<<(std::wostream& os, const ProcessLocation& obj) {
	return obj.proc.WriteLocation(os, obj.loc);
}


std::wostream& operator<<(std::wostream& os, const Process& obj) {
	if (!obj) {
		return os << "[BAD_PROCESS]";
	}
	return os << "[proc: pid=" << obj.m_pid << "]";
}

std::wostream& operator<<(std::wostream& os, const ProcessModule& obj) {
	if (!obj) {
		return os << "[BAD_MODULE]";
	}
	return os << "[" << obj.name << "@" << std::hex << obj.start << "]";
}

std::wostream& operator<<(std::wostream& os, const ProcessModuleExport& obj) {
	if (!obj) {
		return os << "[BAD_EXPORT]";
	}
	if (obj.m_location >= obj.m_module.start) {
		return os << "[" << obj.m_name << "@" << obj.m_module.name << "+" << std::hex << (obj.m_location - obj.m_module.start) << "]";
	}
	return os << "[" << obj.m_name << "@" << std::hex << obj.m_location << "]";
}

#ifdef DEBUG

static void BuildTemplate() {
	struct TestStruct {
		uint64_t unk0;
		uint64_t unk8;
		uint64_t unk10;
		uint64_t unk18;
	};

	Process t{ L"" };
	auto [ptrarr, ok] = t.ReadMemoryArray<TestStruct>(t[0x45896412], 22);

	if (!ok) return;

	auto [ptrobj, ok2] = t.ReadMemoryObject<TestStruct>(t[0x45896412]);

	if (!ok2) return;
}

#endif