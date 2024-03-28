#include <includes.hpp>
#include <clisync.hpp>

namespace {
	int t8dll(int argc, const char* argv[]) {
		constexpr auto gameName = "blackops4.exe";
		constexpr auto dllName = "acts-bo4-dll.dll";
		uint64_t features{};
		for (size_t i = 3; i < argc; i++) {
			auto arg = argv[i];

			if (strcmp(arg, "-l") || _strcmpi(arg, "--log")) {
				features |= clisync::FEATURE_LOG;
			}
			else {
				std::cerr << "Unknown option: " << arg << "\n";
				return tool::BAD_USAGE;
			}
		}

		Process proc{ gameName };

		if (!proc) {
			std::wcerr << L"Can't find game process\n";
			return tool::BASIC_ERROR;
		}
	
		if (!proc.Open()) {
			std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
			return tool::BASIC_ERROR;
		}

		std::filesystem::path progpath = std::filesystem::absolute(g_progPath).parent_path();
		std::filesystem::path dllfile = progpath / std::filesystem::path(dllName);
		auto str = dllfile.string();
	
		std::cout << "dll location -> " << str << "\n";

		if (!proc.LoadDll(str.c_str())) {
			std::cerr << "Can't inject dll\n";
			return tool::BASIC_ERROR;
		}
		std::cout << "dll injected, sync data\n";

		auto& rSyncCLI = proc[dllName]["SyncCLI"];

		if (!rSyncCLI) {
			std::cerr << "Can't find " << dllName << "@SyncCLI\n";
			return tool::BASIC_ERROR;
		}

		clisync::CliSyncData data{ .features = features };

		auto ppstr = progpath.string();

		std::filesystem::path pwd = std::filesystem::current_path();
		auto cpstr = pwd.string();

		strncpy_s(data.execDir, sizeof(data.execDir), ppstr.c_str(), ppstr.length());
		strncpy_s(data.workDir, sizeof(data.workDir), cpstr.c_str(), cpstr.length());

		auto rsync = proc.AllocateMemory(sizeof(data));

		if (!rsync || !proc.WriteMemory(rsync, &data, sizeof(data))) {
			std::cerr << "Can't write sync data\n";
			proc.FreeMemory(rsync, sizeof(data));
			return tool::BASIC_ERROR;
		}

		auto thr = proc.Exec(rSyncCLI.m_location, rsync);

		if (thr == INVALID_HANDLE_VALUE || !thr) {
			std::cerr << "Can't call sync data\n";
			proc.FreeMemory(rsync, sizeof(data));
			return tool::BASIC_ERROR;
		}

		WaitForSingleObject(thr, INFINITE);
		CloseHandle(thr);

		proc.FreeMemory(rsync, sizeof(data));
		return tool::OK;
	}

	int t9dll(int argc, const char* argv[]) {
		Process proc{ "BlackOpsColdWar.exe" };

		if (!proc) {
			std::wcerr << L"Can't find game process\n";
			return tool::BASIC_ERROR;
		}

		if (!proc.Open()) {
			std::cerr << "Can't open game process: x" << std::hex << GetLastError() << "\n";
			return tool::BASIC_ERROR;
		}

		std::filesystem::path progpath = std::filesystem::absolute(g_progPath).parent_path();
		std::filesystem::path dllfile = progpath / std::filesystem::path("acts-bocw-dll.dll");
		auto str = dllfile.string();

		std::cout << "dll location -> " << str << "\n";

		if (!proc.LoadDll(str.c_str())) {
			std::cerr << "Can't inject dll\n";
			return tool::BASIC_ERROR;
		}
		std::cout << "dll injected\n";

		if (argc > 3) {
			proc.ComputeModules();
			for (size_t i = 3; i < argc; i++) {
				auto& mod = proc["acts-bocw-dll.dll"][argv[i]];

				if (!mod) {
					std::cerr << "Can't find export " << argv[i] << "\n";
					continue;
				}

				std::cout << "Calling " << mod << "\n";

				auto thr = proc.Exec(mod.m_location, 0);

				if (thr == INVALID_HANDLE_VALUE || !thr) {
					return false;
				}
				WaitForSingleObject(thr, INFINITE);
				CloseHandle(thr);

				std::cout << "Done\n";
			}
		}

		return tool::OK;
	}

}
ADD_MOD("t8dll", "Inject BO4 DLL", t8dll);
ADD_MOD("t9dll", "Inject CW DLL", t9dll);