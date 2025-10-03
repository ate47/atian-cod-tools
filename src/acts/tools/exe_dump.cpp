#include <includes.hpp>
#include <hook/library.hpp>
#include <tools/exe_dump.hpp>
#include <cli/cli_options.hpp>
#include <cli/clicolor.hpp>

namespace tool::exe_dump {
	const char* ImageDirName(size_t id) {
		switch (id) {
		case IMAGE_DIRECTORY_ENTRY_EXPORT: return "EXPORT";
		case IMAGE_DIRECTORY_ENTRY_IMPORT: return "IMPORT";
		case IMAGE_DIRECTORY_ENTRY_RESOURCE: return "RESOURCE";
		case IMAGE_DIRECTORY_ENTRY_EXCEPTION: return "EXCEPTION";
		case IMAGE_DIRECTORY_ENTRY_SECURITY: return "SECURITY";
		case IMAGE_DIRECTORY_ENTRY_BASERELOC: return "BASERELOC";
		case IMAGE_DIRECTORY_ENTRY_DEBUG: return "DEBUG";
		case IMAGE_DIRECTORY_ENTRY_ARCHITECTURE: return "ARCHITECTURE";
		case IMAGE_DIRECTORY_ENTRY_GLOBALPTR: return "GLOBALPTR";
		case IMAGE_DIRECTORY_ENTRY_TLS: return "TLS";
		case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG: return "LOAD_CONFIG";
		case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT: return "BOUND_IMPORT";
		case IMAGE_DIRECTORY_ENTRY_IAT: return "IAT";
		case IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT: return "DELAY_IMPORT";
		case IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR: return "COM_DESCRIPTOR";
		default: return utils::va("UNKNOWN_%lld", id);
		}
	}


	void DumpProcess(Process& proc, const std::filesystem::path& out, DumpProcessOpt* opt) {
		proc.ComputeModules();
		const ProcessModule& main{ proc.GetMainModule() };

		LOG_DEBUG("dump {} : {}", proc, main);

		IMAGE_DOS_HEADER dosHeader;
		IMAGE_NT_HEADERS ntHeader;

		if (!proc.ReadMemory(&dosHeader, main.start, sizeof(dosHeader)) || dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
			throw std::runtime_error(std::format("{} Can't read dos module header", proc));
		}

		if (!proc.ReadMemory(&ntHeader, main[dosHeader.e_lfanew], sizeof(ntHeader)) || ntHeader.Signature != IMAGE_NT_SIGNATURE) {
			throw std::runtime_error(std::format("{} Can't read nt module header", proc));
		}


		LOG_DEBUG("0x{:x}", dosHeader.e_lfanew);

		for (size_t i = 0; i < ARRAYSIZE(ntHeader.OptionalHeader.DataDirectory); i++) {
			IMAGE_DATA_DIRECTORY* dir{ &ntHeader.OptionalHeader.DataDirectory[i] };
			LOG_DEBUG("{} 0x{:x}:0x{:x}", ImageDirName(i), dir->VirtualAddress, dir->Size);
		}
		LOG_DEBUG("--------- header --------");
		LOG_DEBUG("Magic: 0x{:x}", ntHeader.OptionalHeader.Magic);
		LOG_DEBUG("MajorLinkerVersion: 0x{:x}", ntHeader.OptionalHeader.MajorLinkerVersion);
		LOG_DEBUG("MinorLinkerVersion: 0x{:x}", ntHeader.OptionalHeader.MinorLinkerVersion);
		LOG_DEBUG("SizeOfCode: 0x{:x}", ntHeader.OptionalHeader.SizeOfCode);
		LOG_DEBUG("SizeOfInitializedData: 0x{:x}", ntHeader.OptionalHeader.SizeOfInitializedData);
		LOG_DEBUG("SizeOfUninitializedData: 0x{:x}", ntHeader.OptionalHeader.SizeOfUninitializedData);
		LOG_DEBUG("AddressOfEntryPoint: 0x{:x}", ntHeader.OptionalHeader.AddressOfEntryPoint);
		LOG_DEBUG("BaseOfCode: 0x{:x}", ntHeader.OptionalHeader.BaseOfCode);
		LOG_DEBUG("ImageBase: 0x{:x}", ntHeader.OptionalHeader.ImageBase);
		LOG_DEBUG("SectionAlignment: 0x{:x}", ntHeader.OptionalHeader.SectionAlignment);
		LOG_DEBUG("FileAlignment: 0x{:x}", ntHeader.OptionalHeader.FileAlignment);
		LOG_DEBUG("MajorOperatingSystemVersion: 0x{:x}", ntHeader.OptionalHeader.MajorOperatingSystemVersion);
		LOG_DEBUG("MinorOperatingSystemVersion: 0x{:x}", ntHeader.OptionalHeader.MinorOperatingSystemVersion);
		LOG_DEBUG("MajorImageVersion: 0x{:x}", ntHeader.OptionalHeader.MajorImageVersion);
		LOG_DEBUG("MinorImageVersion: 0x{:x}", ntHeader.OptionalHeader.MinorImageVersion);
		LOG_DEBUG("MajorSubsystemVersion: 0x{:x}", ntHeader.OptionalHeader.MajorSubsystemVersion);
		LOG_DEBUG("MinorSubsystemVersion: 0x{:x}", ntHeader.OptionalHeader.MinorSubsystemVersion);
		LOG_DEBUG("Win32VersionValue: 0x{:x}", ntHeader.OptionalHeader.Win32VersionValue);
		LOG_DEBUG("SizeOfImage: 0x{:x}", ntHeader.OptionalHeader.SizeOfImage);
		LOG_DEBUG("SizeOfHeaders: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeaders);
		LOG_DEBUG("CheckSum: 0x{:x}", ntHeader.OptionalHeader.CheckSum);
		LOG_DEBUG("Subsystem: 0x{:x}", ntHeader.OptionalHeader.Subsystem);
		LOG_DEBUG("DllCharacteristics: 0x{:x}", ntHeader.OptionalHeader.DllCharacteristics);
		LOG_DEBUG("SizeOfStackReserve: 0x{:x}", ntHeader.OptionalHeader.SizeOfStackReserve);
		LOG_DEBUG("SizeOfStackCommit: 0x{:x}", ntHeader.OptionalHeader.SizeOfStackCommit);
		LOG_DEBUG("SizeOfHeapReserve: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeapReserve);
		LOG_DEBUG("SizeOfHeapCommit: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeapCommit);
		LOG_DEBUG("LoaderFlags: 0x{:x}", ntHeader.OptionalHeader.LoaderFlags);
		LOG_DEBUG("NumberOfRvaAndSizes: 0x{:x}", ntHeader.OptionalHeader.NumberOfRvaAndSizes);
		
	}
	void DumpProcessExe(const std::filesystem::path& in, const std::filesystem::path& out, DumpProcessOpt* opt) {
		std::wstring instr{ in.wstring() };
		LOG_TRACE("dump process {} -> {}", in.string(), out.string());

		HANDLE ht;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &ht)) {
			LUID luid;
			if (LookupPrivilegeValueA(nullptr, SE_DEBUG_NAME, &luid)) {
				TOKEN_PRIVILEGES tp{};
				tp.PrivilegeCount = 1;
				tp.Privileges->Luid = luid;
				tp.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
				AdjustTokenPrivileges(ht, FALSE, &tp, NULL, NULL, NULL);
			}
			CloseHandle(ht);
		}

		SHELLEXECUTEINFOW shellExeInfo{};
		shellExeInfo.cbSize = sizeof(shellExeInfo);
		shellExeInfo.lpFile = instr.data();
		shellExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellExeInfo.nShow = SW_SHOW;

		STARTUPINFOW si{};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi{};

		if (!CreateProcessW(
			NULL,
			instr.data(),
			NULL,
			NULL,
			FALSE,
			DEBUG_ONLY_THIS_PROCESS,
			NULL,
			NULL,
			&si,
			&pi
		)) {
			throw std::runtime_error(std::format("Can't start process {}", in.string()));
		}
		Process proc{ pi.hProcess, pi.dwProcessId };

		utils::CloseEnd ce{ [&pi] { CloseHandle(pi.hThread); } };

		DEBUG_EVENT ev{};
		while (true) {
			if (!WaitForDebugEvent(&ev, 15000)) {
				proc.EndThreads();
				continue;
			}

			if (ev.dwDebugEventCode != EXIT_THREAD_DEBUG_EVENT
				&& !(ev.dwDebugEventCode == EXCEPTION_DEBUG_EVENT && (
					ev.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION
					|| ev.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION))) {
				ContinueDebugEvent(ev.dwProcessId, ev.dwThreadId, DBG_CONTINUE);
				continue;
			}

			DumpProcess(proc, out, opt);
			break;
		}
	}


	namespace {
		int exe_dump(int argc, const char* argv[]) {
			DumpProcessOpt opt{};
			bool showHelp{};
			cli::options::CliOptions opts{};
			opts.addOption(&opt.rebuildIAT, "rebuild IAT table", "--rebuildIAT");
			opts.addOption(&opt.searchIAT, "search IAT table", "--searchIAT");
			opts.addOption(&showHelp, "help", "--help", "", "-h");

			if (!opts.ComputeOptions(2, argc, argv) || showHelp || opts.NotEnoughParam(1)) {
				opts.PrintOptions();
				return showHelp ? tool::OK : tool::BAD_USAGE;
			}

			std::filesystem::path exePath{ opts[0] };
			std::filesystem::path outPath;

			if (opts.NotEnoughParam(2)) {
				outPath = exePath.filename();
				outPath.replace_extension();
				outPath = utils::GetProgDir() / "deps" / std::format("{}_dump.exe", outPath.string());
			}
			else {
				outPath = opts[1];
			}

			DumpProcessExe(exePath, outPath, &opt);

			return tool::OK;
		}

		struct GameConfig {
			const char* id;
			const char* exe;
			const char* ddls;
			DumpProcessOpt opt;
		};

		static GameConfig gameConfigs[]{
			{ "bo3", "BlackOps3.exe", "", {} },
			{ "bo4", "BlackOps4.exe", "oo2core_6_win64.dll\0", {} },
			{ "cw", "BlackOpsColdWar.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw19", "ModernWarfare.exe", "oo2core_7_win64.dll\0", {} },
			{ "mw3", "cod23-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw3sp", "sp23-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "bo6", "../cod.exe", "../oo2core_8_win64.dll\0", { .rebuildIAT = true } },
			{ "bo6sp", "sp24-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "deathloop", "Deathloop.exe", "oo2core_8_win64.dll\0oo2net_8_win64.dll\0", { .searchIAT = true } },
		};

		int game_install(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 1)) {
				LOG_INFO("Available games:");

				for (GameConfig& cfg : gameConfigs) {
					LOG_INFO("- {}", cfg.id);
				}

				LOG_INFO("");
				LOG_INFO("Enter this command with the wanted game:");
				LOG_INFO("{} {} [id] [path]", argv[0], argv[1]);
				LOG_INFO("");

				return tool::OK;
			}
			if (tool::NotEnoughParam(argc, 2)) {
				return tool::BAD_USAGE;
			}

			const char* id{ argv[2] };
			std::filesystem::path path{ argv[3] };
			GameConfig* cfg{};

			for (GameConfig& c : gameConfigs) {
				if (!_strcmpi(c.id, id)) {
					cfg = &c;
					break;
				}
			}
			if (!cfg) {
				LOG_ERROR("Can't find game id {}", id);
				LOG_ERROR("To list the available games, type");
				LOG_ERROR("{} {}", argv[0], argv[1]);
				return tool::BASIC_ERROR;
			}

			std::filesystem::path exePath{ path / cfg->exe };
			std::filesystem::path depsDir{ utils::GetProgDir() / "deps" };
			std::filesystem::path outPath{ exePath.filename() };
			outPath.replace_extension();
			outPath = depsDir / std::format("{}_dump.exe", outPath.string());

			std::filesystem::create_directories(depsDir);

			const char* dllName{ cfg->ddls };
			while (*dllName) {
				std::filesystem::path dllPath{ path / dllName };
				std::filesystem::path depsPath{ depsDir / dllName };
				std::filesystem::copy_file(dllPath, depsPath, std::filesystem::copy_options::skip_existing);
				LOG_INFO("install dll {}", dllName);
				dllName += std::strlen(dllName) + 1;
			}

			DumpProcessExe(path / cfg->exe, outPath, &cfg->opt);

			LOG_INFO("game {} installed, it can now be used.", cfg->id);
			return tool::OK;
		}


		ADD_TOOL(exe_dump, "common", "[exe] (out)", "Dump exe", exe_dump);
		ADD_TOOL(game_install, "common", "[id] [path]", "Install game exe to acts", game_install);
	}
}