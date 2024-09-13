#include <includes_shared.hpp>
#include "process.hpp"

#ifdef ASMJIT_STATIC
asmjit::JitRuntime& process::GetJitRuntime() {
	static asmjit::JitRuntime jr{};
	return jr;
}
#endif

byte* process::BasePtr() {
	return reinterpret_cast<byte*>(BaseHandle());
}

HMODULE process::BaseHandle() {
	static HMODULE ptr = NULL;
	if (!ptr) {
		ptr = GetModuleHandleA(NULL);
	}
	return ptr;
}

PIMAGE_DOS_HEADER process::PImageDosHeader(HMODULE mod) {
	return reinterpret_cast<PIMAGE_DOS_HEADER>(mod ? mod : BaseHandle());
}

void process::WriteMemSafe(void* dest, void* src, size_t len) {
	DWORD old = 0;
	VirtualProtect(dest, len, PAGE_EXECUTE_READWRITE, &old);

	memcpy(dest, src, len);

	VirtualProtect(dest, len, old, &old);
	FlushInstructionCache(GetCurrentProcess(), dest, len);
}

PIMAGE_NT_HEADERS process::PImageNtHeader(HMODULE mod) {
	return reinterpret_cast<PIMAGE_NT_HEADERS>((mod ? mod : BaseHandle()) + PImageDosHeader(mod)->e_lfanew);
}

PIMAGE_OPTIONAL_HEADER process::PImageOptHeader(HMODULE mod) {
	return &(PImageNtHeader(mod)->OptionalHeader);
}

HMODULE process::LoadLib(const char* lib) {
	return LoadLibraryA(lib);
}

HMODULE process::LoadSysLib(const char* lib) {
	char dir[MAX_PATH]{ 0 };
	GetSystemDirectoryA(dir, sizeof(dir));

	return LoadLib(std::format("{}/{}", dir, lib));
}

void** process::GetImportAddrTableEntry(const char* lib, const char* entry) {
	auto mod = GetModuleHandleA(lib);
	if (!mod || !lib) {
		return nullptr;
	}

	auto addr = GetProcAddress(mod, entry);
	assert(addr && "can't find entry");

	auto* base = BasePtr();
	auto desc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(base + PImageOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for (;desc->Name; desc++) {
		if (_strcmpi((const char*)&base[desc->Name], lib) != 0) {
			continue;
		}

		auto* originalThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(base + desc->OriginalFirstThunk);
		auto* thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(base + desc->FirstThunk);


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

static byte ctob(char c) {
	if (c >= 'A' && c <= 'F') {
		return (byte)(c - 'A' + 0xA);
	}
	if (c >= 'a' && c <= 'f') {
		return (byte)(c - 'a' + 0xA);
	}
	if (c >= '0' && c <= '9') {
		return (byte)(c - 'a' + 0);
	}
	throw std::runtime_error(std::format("Invalid character {:x}", c));

}

std::vector<byte*> process::Scan(std::string pattern) {
	std::vector<byte*> find{};

	std::vector<byte> mask{};
	std::vector<byte> searched{};

	bool mid = true;

	// parse pattern
	for (char c : pattern) {
		if (isspace(c)) {
			continue;
		}

		mid = !mid;

		if (c == '?') {
			if (!mid) {
				mask.push_back(0);
				searched.push_back(0);
			}
			continue;
		}

		auto b = ctob(c);

		if (mid) {
			*mask.end() |= 0xF;
			*searched.end() |= b;
		}
		else {
			mask.push_back(0xF0);
			searched.push_back(b << 4);
		}
	}

	// reversed because we set it by default to 0
	if (!mid) {
		throw std::runtime_error("Scan pattern has half byte!");
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
		throw std::runtime_error("Empty pattern!");
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
		throw std::runtime_error("Empty pattern!");
	}

	// pattern computed, scanning
	auto ptr = process::BasePtr();
	auto il = process::PImageOptHeader()->SizeOfImage;
	if (il < mask.size()) {
		// empty
		return find;
	}
	auto end = ptr + (il - mask.size());

	while (ptr != end) {
		// maybe add // computing like with JLA
		size_t loc = 0;

		do {
			if ((ptr[loc] & mask[loc]) != searched[loc]) {
				break;
			}
		} while (++loc < mask.size());

		// it's a match
		if (loc == mask.size()) {
			find.push_back(ptr);
		}

		ptr++;
	}


	return find;
}


#ifdef ASMJIT_STATIC
void process::AssemblerExp::pushregisters() {
	using namespace asmjit::x86;

	this->push(rax);
	this->push(rcx);
	this->push(rdx);
	this->push(rbx);
	this->push(rsp);
	this->push(rbp);
	this->push(rsi);
	this->push(rdi);

	this->sub(rsp, 0x40);
}

void process::AssemblerExp::popregisters() {
	using namespace asmjit::x86;

	this->add(rsp, 0x40);

	this->pop(rdi);
	this->pop(rsi);
	this->pop(rbp);
	this->pop(rsp);
	this->pop(rbx);
	this->pop(rdx);
	this->pop(rcx);
	this->pop(rax);
}

void process::AssemblerExp::prepareStackForCall() {
	using namespace asmjit::x86;

	const auto reserveCalleeSpace = this->newLabel();
	const auto unalignedStack = this->newLabel();

	this->test(rsp, 0xF);
	this->jnz(unalignedStack);

	this->sub(rsp, 0x8);
	this->push(rsp);

	this->push(rax);
	this->mov(rax, ptr(rsp, 8, 8));
	this->add(rax, 0x8);
	this->mov(ptr(rsp, 8, 8), rax);
	this->pop(rax);

	this->jmp(reserveCalleeSpace);

	this->bind(unalignedStack);
	this->push(rsp);

	this->bind(reserveCalleeSpace);
	this->sub(rsp, 0x40);
}

void process::AssemblerExp::restoreStackAfterCall() {
	using namespace asmjit::x86;

	this->lea(rsp, ptr(rsp, 0x40));
	this->pop(rsp);
}

asmjit::Error process::AssemblerExp::call(void* target) {
	return Assembler::call(size_t(target));
}

asmjit::Error process::AssemblerExp::jmp(void* target) {
	return Assembler::jmp(size_t(target));
}
#endif