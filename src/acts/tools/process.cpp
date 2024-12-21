#include <includes.hpp>
#include <utils/memapi_calls.hpp>
#include "tools/pool.hpp"

	/*
	 * memapi test commands
	 */
namespace {
	int PTSearch(Process& proc, int argc, const char* argv[]);
	int PTAlloc(Process& proc, int argc, const char* argv[]);
	int PTCall(Process& proc, int argc, const char* argv[]);
	int PTRead(Process& proc, int argc, const char* argv[]);
	int PTCallExt(Process& proc, int argc, const char* argv[]);
	int PTCallExt2(Process& proc, int argc, const char* argv[]);
	int PTCallExt3(Process& proc, int argc, const char* argv[]);
	int PTCallExt4(Process& proc, int argc, const char* argv[]);
	int PTCallExt5(Process& proc, int argc, const char* argv[]);
	int PTCallExt6(Process& proc, int argc, const char* argv[]);

	int processtool(Process& unused, int argc, const char* argv[]) {
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
		else if (!_strcmpi("r", argv[3])) {
			func = PTRead;
		}
		else if (!_strcmpi("ce", argv[3])) {
			func = PTCallExt;
		}
	#ifndef CI
		else if (!_strcmpi("ce2", argv[3])) {
			func = PTCallExt2;
		}
		else if (!_strcmpi("ce3", argv[3])) {
			func = PTCallExt3;
		}
		else if (!_strcmpi("ce4", argv[3])) {
			func = PTCallExt4;
		}
		else if (!_strcmpi("ce5", argv[3])) {
			func = PTCallExt5;
		}
		else if (!_strcmpi("ce6", argv[3])) {
			func = PTCallExt6;
		}
	#endif // !CI

		if (!func) {
			std::cerr << "Bad function: " << argv[3] << "\n";
			return tool::BAD_USAGE;
		}

		proc.Open();

		return func(proc, argc, argv);
	}

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

		char helloWorld[200] = "hello world";

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

		char hello2[200] = { 0 };

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

	struct TestStruct {
		uint64_t a;
		uint64_t b;
		uint64_t c;
		uint64_t d;
		uint64_t e;
	};

	void __declspec(noinline) __declspec(cdecl) Test(TestStruct* str) {
		reinterpret_cast<void(*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)>(0x7ffcafe67260)(str->a, str->b, str->c, str->d, str->e);
	}

	int PTCallExt(Process& proc, int argc, const char* argv[]) {
		Beep(750, 300);
		auto ret = memapi::Call<DWORD, DWORD>(proc, proc["Kernel32.dll"]["beep"], 750, 300); // r8 r9
		if (!ret) {
			std::cerr << "Error when calling func\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok\n";

		return tool::OK;
	}

	void TestFunc1(void* value, const wchar_t* value2, int value3, int value4, int value5, int value6, int value7) {
		std::wcout << value << ":" << value2 << ":" << value3 << ":" << value4 << ":" << value5 << ":" << value6 << ":" << value7 << "\n";
	}

	int PTCallExt2(Process& proc, int argc, const char* argv[]) {
		auto ret = memapi::Call<void*, const wchar_t*, int, int, int, int, int>(proc, reinterpret_cast<uintptr_t>(&TestFunc1), nullptr, L"test", 32, 42, 65, 72, 85);

		if (!ret) {
			std::cerr << "Error when calling func\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok\n";

		return tool::OK;
	}

	void TestFunc2(double value, const wchar_t* value2, int value3, int value4, int value5, const wchar_t* value6, double value7) {
		std::wcout << value << ":" << value2 << ":" << value3 << ":" << value4 << ":" << value5 << ":" << value6 << ":" << value7 << "\n";
	}

	int PTCallExt3(Process& proc, int argc, const char* argv[]) {
		auto ret = memapi::Call<double, const wchar_t*, int, int, int, const wchar_t*, double>(proc, reinterpret_cast<uintptr_t>(&TestFunc2), 12.3, L"test", 32, 42, 65, L"test 3", 85.65);

		if (!ret) {
			std::cerr << "Error when calling func\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok\n";

		return tool::OK;
	}

	void TestFunc3() {
		std::cout << "Hello world\n";
	}

	int PTCallExt4(Process& proc, int argc, const char* argv[]) {
		auto ret = memapi::Call<>(proc, reinterpret_cast<uintptr_t>(&TestFunc3));

		if (!ret) {
			std::cerr << "Error when calling func\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok\n";

		return tool::OK;
	}

	int PTCallExt5(Process& proc, int argc, const char* argv[]) {
		if (!proc.LoadDll("test-dll.dll")) {
			std::cerr << "Can't inject test dll\n";
			return tool::BASIC_ERROR;
		}

		auto& mod = proc["test-dll.dll"];
		auto& ProcessInjectionTest1 = mod["ProcessInjectionTest1"];
		auto& ProcessInjectionTest2 = mod["ProcessInjectionTest2"];


		std::cout << "dll: " << mod << "\n"
			<< "test1: " << ProcessInjectionTest1 << "\n"
			<< "test2: " << ProcessInjectionTest2 << "\n";

		if (!memapi::Call<>(proc, ProcessInjectionTest1)) {
			std::cerr << "Error when calling ProcessInjectionTest1\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok 1\n";

		if (!memapi::Call<int, double, float, const wchar_t*, bool>(proc, ProcessInjectionTest2, 2, 3.5, 4.5, L"Hello", true)) {
			std::cerr << "Error when calling ProcessInjectionTest2\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "ok 2\n";

		return tool::OK;
	}

	int PTCallExt6(Process& proc, int argc, const char* argv[]) {
		if (!proc.LoadDll("test-dll.dll")) {
			std::cerr << "Can't inject test dll\n";
			return tool::BASIC_ERROR;
		}

		auto& mod = proc["test-dll.dll"];
		auto& TestFunctionScan = mod["TestFunctionScan"];

		std::cout << "dll: " << mod << "\n"
			<< "test1: " << TestFunctionScan << "\n";

		if (!memapi::Call<>(proc, TestFunctionScan)) {
			std::cerr << "Error when calling TestFunctionScan\n";
			return tool::BASIC_ERROR;
		}

		std::cout << "prepatch\n";

		auto scan1 = mod.Scan("01 02 03 04 ?? ?? ?? ?? 09 10");
		proc.WriteLocation(std::cout << "scan1: ", scan1) << "\n";

		auto scan2 = mod.Scan("01 02 03 04 ?? ?? ?? ?? 09 10", (DWORD)((scan1 + 1) - mod.start));
		proc.WriteLocation(std::cout << "scan2: ", scan2) << "\n";

		auto scan3 = mod.Scan("01 02 03 04 ?? ?? ?? ?? 09 10", (DWORD)((scan2 + 1) - mod.start));
		proc.WriteLocation(std::cout << "scan3: ", scan3) << "\n";


		if (!scan1) {
			std::cerr << "Bad scan\n";
			return tool::BASIC_ERROR;
		}

		if (!scan2) {
			std::cerr << "Bad scan2\n";
			return tool::BASIC_ERROR;
		}

		int over = 0x08070605;

		if (!proc.WriteMemory(scan1 + 4, &over, sizeof(over))) {
			std::cerr << "Can't write memory 1\n";
			return tool::BASIC_ERROR;
		}

		over = 0x05060708;

		if (!proc.WriteMemory(scan2 + 4, &over, sizeof(over))) {
			std::cerr << "Can't write memory 2\n";
			return tool::BASIC_ERROR;
		}

		if (!memapi::Call<>(proc, TestFunctionScan)) {
			std::cerr << "Error when calling TestFunctionScan\n";
			return tool::BASIC_ERROR;
		}

		return tool::OK;
	}

	int PTCall(Process& proc, int argc, const char* argv[]) {
		ProcessModuleExport& beep = proc["kernel32.dll"]["beep"];

		if (!beep) {
			std::cerr << "Can't find beep\n";
			return tool::BASIC_ERROR;
		}

		uintptr_t location = beep.m_location;

		std::vector<byte> cll{};
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

		std::vector<byte> args{};

		//Beep(750, 300);
		args.resize(sizeof(uint64_t) * 2);
		((uint64_t*)&args[0])[0] = 750;
		((uint64_t*)&args[0])[1] = 300;

		auto allocsize = cll.size() + 16;
		auto funcsize = cll.size();
		auto func = proc.AllocateMemory(allocsize, PAGE_EXECUTE_READWRITE);
		auto argptr = proc.AllocateMemory(args.size(), PAGE_READWRITE);
		auto funcalign = utils::Aligned<uint64_t>(func);

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

	int PTRead(Process& proc, int argc, const char* argv[]) {
		if (argc <= 5) {
			std::cerr << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " [(+)location] [size]";
			return tool::BASIC_ERROR;
		}

		hashutils::ReadDefaultFile();

		bool relative = argv[4][0] == '+';
		auto idx = std::stoull(argv[4], nullptr, 16);
		auto len = std::stoull(argv[5], nullptr, 16);

		std::cout << std::hex << "reading 0x";

		if (relative) {
			std::cout << proc[(uint64_t)0] << "+";
		}

		std::cout << idx << "[0:" << len << "]...\n";

		if (relative) idx = proc[idx];

		auto [buffer, ok] = proc.ReadMemoryArray<byte>(idx, len);

		if (!ok) {
			std::cerr << "Error when reading memory\n";
			return tool::BASIC_ERROR;
		}

		tool::pool::WriteHex(std::cout, idx, &buffer[0], len, proc);

		return tool::OK;
	}

	int procloop(Process& unused, int argc, const char* argv[]) {
		uint64_t c{};
		while (true) {
			std::cout << "c: " << (c * 10) << "s\n";
			Sleep(10000);
			c += 10;
		}
	}


	int crashproc(Process& unused, int argc, const char* argv[]) {
		*reinterpret_cast<byte*>(0x123456) = 2;
		return tool::BASIC_ERROR;
	}
}

ADD_TOOL(crashproc, "dev", "", "", nullptr, crashproc);
ADD_TOOL(procloop, "dev", "", "proc loop", nullptr, procloop);
ADD_TOOL(proc, "dev", " (process) (s) [module] [function]", "process explorer", nullptr, processtool);