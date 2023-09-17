#include <includes.hpp>

int PTSearch(Process& proc, int argc, const char* argv[]);
int PTAlloc(Process& proc, int argc, const char* argv[]);
int PTCall(Process& proc, int argc, const char* argv[]);

int processtool(const Process& unused, int argc, const char* argv[]) {
	if (argc <= 3) {
		std::cerr << "Missing process name and tool\n";
		return tool::BAD_USAGE;
	}

	Process proc{ argv[2] };

	if (!proc) {
		std::cerr << "Can't find proc '" << argv[2] << "'\n";
		return tool::BASIC_ERROR;
	}

	std::cout << argv[2] << ": " << std::dec << proc.m_pid << "\n";

	int (*func)(Process & proc, int argc, const char* argv[]) = nullptr;

	if (!_strcmpi("s", argv[3])) {
		func = PTSearch;
	}
	else if (!_strcmpi("hello", argv[3])) {
		func = PTAlloc;
	}
	else if (!_strcmpi("call", argv[3])) {
		func = PTCall;
	}

	if (!func) {
		std::cerr << "Bad function: " << argv[3] << "\n";
		return tool::BAD_USAGE;
	}

	proc.Open();

	return func(proc, argc, argv);
}

ADD_TOOL("proc", " (process) (s) [module] [function]", "process explorer", false, processtool);

int PTSearch(Process& proc, int argc, const char* argv[]) {
	using namespace tool;
	if (argc <= 4) {
		for (const auto& mod : proc.modules()) {
			std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";
		}
		return OK;
	}

	auto& mod = proc[argv[4]];

	if (!mod) {
		std::cerr << "Can't find module '" << argv[4] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::hex << mod.path << " (" << mod.name << ") 0x" << std::hex << mod.start << " " << std::dec << mod.size << " Bytes\n";

	mod.ComputeExports();

	if (argc <= 5) {
		for (const auto& exp : mod.exports()) {
			std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";
		}
		return OK;
	}

	auto& exp = mod[argv[5]];

	if (!exp) {
		std::cerr << "Can't find export '" << argv[5] << "'\n";
		return BASIC_ERROR;
	}

	std::cout << std::dec << exp.m_ordinal << ":" << exp.m_name << " " << mod.name << "+" << std::hex << (exp.m_location - mod.start) << "\n";

	return OK;
}

int PTAlloc(Process& proc, int argc, const char* argv[]) {
	using namespace tool;

	CHAR helloWorld[200] = "hello world";

	auto ptr = proc.AllocateMemory(200);

	if (!ptr) {
		std::cerr << "Can't allocate memory\n";
		return BASIC_ERROR;
	}

	if (!proc.WriteMemory(ptr, helloWorld, 200)) {
		std::cerr << "Can't write memory\n";
		proc.FreeMemory(ptr, 200);
		return BASIC_ERROR;
	}

	CHAR hello2[200] = { 0 };

	if (!proc.ReadMemory(&hello2[0], ptr, 200)) {
		std::cerr << "Can't read memory\n";
		proc.FreeMemory(ptr, 200);
		return BASIC_ERROR;
	}

	std::cout << "local:" << helloWorld << "\n";
	std::cout << "remote:" << hello2 << "\n";

	proc.FreeMemory(ptr, 200);

	std::cout << "ok\n";
	return OK;
}

/*
sizeof(*str) = 28
reinterpret_cast<void(*)(int, UINT64, UINT64, UINT64, UINT64)>(0x7ffcafe67260)(str->a, str->b, str->c, str->d, str->e);
var_18 = qword ptr -18h

// rcx, rdx, r8, r9 -> stack

rcx = str

sub     rsp, 38h			   ; 0x48, 0x83, 0xEC, 0x38
mov     rax, [rcx+20h]		   ; 0x48, 0x8B, 0x41, 0x20
mov     r9, [rcx+18h]		   ; 0x4C, 0x8B, 0x49, 0x18
mov     r8, [rcx+10h]		   ; 0x4C, 0x8B, 0x41, 0x10
mov     rdx, [rcx+8]		   ; 0x48, 0x8B, 0x51, 0x08
mov     rcx, [rcx]			   ; 0x48, 0x8B, 0x09
mov     [rsp+38h+var_18], rax  ; 0x48, 0x89, 0x44, 0x24, 0x20
mov     rax, 7FFCAFE67260h	   ; 0x48, 0xB8, 0x60, 0x72, 0xE6, 0xAF, 0xFC, 0x7F, 0x00, 0x00
call    rax					   ; 0xFF, 0xD0
add     rsp, 38h			   ; 0x48, 0x83, 0xC4, 0x38
retn                           ; 0xC3

*/

struct TestStruct {
	UINT64 a;
	UINT64 b;
	UINT64 c;
	UINT64 d;
	UINT64 e;
};

void __declspec(noinline) __declspec(cdecl) Test(TestStruct* str) {
	reinterpret_cast<void(*)(UINT64, UINT64, UINT64, UINT64, UINT64)>(0x7ffcafe67260)(str->a, str->b, str->c, str->d, str->e);
}

int PTCall(Process& proc, int argc, const char* argv[]) {
	ProcessModuleExport& beep = proc["kernel32.dll"]["beep"];

	if (!beep) {
		std::cerr << "Can't find beep\n";
		return tool::BASIC_ERROR;
	}

	uintptr_t location = beep.m_location;

	std::vector<BYTE> cll{};
	cll.reserve(0x100);

	cll.push_back(0x48);
	cll.push_back(0x8B);
	cll.push_back(0x51);
	cll.push_back(0x08); // mov     rdx,[rcx + 8];

	cll.push_back(0x48);
	cll.push_back(0x8B);
	cll.push_back(0x09); // mov     rcx,[rcx];

	cll.push_back(0x48);
	cll.push_back(0xB8);
	auto calllocidx = cll.size();
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	cll.push_back(0);
	*((uintptr_t*)&cll[calllocidx]) = location; // mov     rax, location

	cll.push_back(0xFF);
	cll.push_back(0xD0); // call    rax; 0xFF

	cll.push_back(0xC3); // retn

	std::vector<BYTE> args{};

	//Beep(750, 300);
	args.resize(sizeof(UINT64) * 2);
	((UINT64*)&args[0])[0] = 750;
	((UINT64*)&args[0])[1] = 300;

	auto allocsize = cll.size() + 16;
	auto funcsize = cll.size();
	auto func = proc.AllocateMemory(allocsize, PAGE_EXECUTE_READWRITE);
	auto argptr = proc.AllocateMemory(args.size(), PAGE_READWRITE);
	auto funcalign = utils::Aligned<UINT64>(func);

	if (!func || !argptr) {
		std::cerr << "Can't allocate memory\n";
		proc.FreeMemory(func, allocsize);
		proc.FreeMemory(argptr, args.size());
		return tool::BASIC_ERROR;
	}


	if (!proc.WriteMemory(funcalign, &cll[0], funcsize) || !proc.WriteMemory(argptr, &args[0], args.size())) {
		std::cerr << "Can't write memory\n";
		proc.FreeMemory(func, allocsize);
		proc.FreeMemory(argptr, args.size());
		return tool::BASIC_ERROR;
	}

	auto thr = proc.Exec(funcalign, argptr);

	if (thr == INVALID_HANDLE_VALUE || !thr) {
		std::cout << "Bad handle value\n";
	}
	else {
		std::cout << "Done, free\n";
		WaitForSingleObject(thr, INFINITE);
		CloseHandle(thr);
	}

	proc.FreeMemory(func, allocsize);
	proc.FreeMemory(argptr, args.size());

	return tool::OK;
}