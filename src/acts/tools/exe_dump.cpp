#include <includes.hpp>
#include <hook/library.hpp>
#include <tools/exe_dump.hpp>
#include <cli/cli_options.hpp>
#include <cli/clicolor.hpp>
#include <core/bytebuffer.hpp>

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

	void CopyMemorySafe(Process& proc, void* dest, uintptr_t src, size_t len) {
		// copy function considering the page accesses
		MEMORY_BASIC_INFORMATION64 page{};
		uintptr_t current{};
		uintptr_t end{ src + len };

		// search first page
		do {
			current = (uintptr_t)page.BaseAddress + page.RegionSize;
			VirtualQueryEx(proc.GetHandle(), (LPCVOID)current, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page), sizeof(MEMORY_BASIC_INFORMATION));
		} while (page.BaseAddress + page.RegionSize < src);

		// equals or before, read first page
		uintptr_t baseLen{ std::min<size_t>(page.BaseAddress + page.RegionSize - src, len) };
		if (baseLen) {
			if (!(page.State != MEM_COMMIT || page.Protect == PAGE_NOACCESS || (page.Protect & PAGE_GUARD))) {
				proc.ReadMemoryEx(dest, src, baseLen);
			}
		}
		current = (uintptr_t)page.BaseAddress + page.RegionSize;

		byte* destb{ (byte*)dest };

		while (current < end) {
			VirtualQueryEx(proc.GetHandle(), (LPCVOID)current, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page), sizeof(MEMORY_BASIC_INFORMATION));
			current = (uintptr_t)page.BaseAddress;

			if (page.State != MEM_COMMIT || page.Protect == PAGE_NOACCESS || (page.Protect & PAGE_GUARD)) {
				current += page.RegionSize;
				continue;
			}
			size_t offset{ current - src };
			size_t readLen{ std::min<size_t>(page.RegionSize, len - offset) };
			byte* to{ &destb[offset] };

			proc.ReadMemoryEx(to, current, readLen);
			current += page.RegionSize;
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

		if (opt->dumpHeader) {
			LOG_INFO("----- dir addresses -----");
			for (size_t i = 0; i < ARRAYSIZE(ntHeader.OptionalHeader.DataDirectory); i++) {
				IMAGE_DATA_DIRECTORY* dir{ &ntHeader.OptionalHeader.DataDirectory[i] };
				LOG_INFO("{} 0x{:x}:0x{:x}", ImageDirName(i), dir->VirtualAddress, dir->Size);
			}
			LOG_INFO("--------- header --------");
			LOG_INFO("Magic: 0x{:x}", ntHeader.OptionalHeader.Magic);
			LOG_INFO("MajorLinkerVersion: 0x{:x}", ntHeader.OptionalHeader.MajorLinkerVersion);
			LOG_INFO("MinorLinkerVersion: 0x{:x}", ntHeader.OptionalHeader.MinorLinkerVersion);
			LOG_INFO("SizeOfCode: 0x{:x}", ntHeader.OptionalHeader.SizeOfCode);
			LOG_INFO("SizeOfInitializedData: 0x{:x}", ntHeader.OptionalHeader.SizeOfInitializedData);
			LOG_INFO("SizeOfUninitializedData: 0x{:x}", ntHeader.OptionalHeader.SizeOfUninitializedData);
			LOG_INFO("AddressOfEntryPoint: 0x{:x}", ntHeader.OptionalHeader.AddressOfEntryPoint);
			LOG_INFO("BaseOfCode: 0x{:x}", ntHeader.OptionalHeader.BaseOfCode);
			LOG_INFO("ImageBase: 0x{:x}", ntHeader.OptionalHeader.ImageBase);
			LOG_INFO("SectionAlignment: 0x{:x}", ntHeader.OptionalHeader.SectionAlignment);
			LOG_INFO("FileAlignment: 0x{:x}", ntHeader.OptionalHeader.FileAlignment);
			LOG_INFO("MajorOperatingSystemVersion: 0x{:x}", ntHeader.OptionalHeader.MajorOperatingSystemVersion);
			LOG_INFO("MinorOperatingSystemVersion: 0x{:x}", ntHeader.OptionalHeader.MinorOperatingSystemVersion);
			LOG_INFO("MajorImageVersion: 0x{:x}", ntHeader.OptionalHeader.MajorImageVersion);
			LOG_INFO("MinorImageVersion: 0x{:x}", ntHeader.OptionalHeader.MinorImageVersion);
			LOG_INFO("MajorSubsystemVersion: 0x{:x}", ntHeader.OptionalHeader.MajorSubsystemVersion);
			LOG_INFO("MinorSubsystemVersion: 0x{:x}", ntHeader.OptionalHeader.MinorSubsystemVersion);
			LOG_INFO("Win32VersionValue: 0x{:x}", ntHeader.OptionalHeader.Win32VersionValue);
			LOG_INFO("SizeOfImage: 0x{:x}", ntHeader.OptionalHeader.SizeOfImage);
			LOG_INFO("SizeOfHeaders: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeaders);
			LOG_INFO("CheckSum: 0x{:x}", ntHeader.OptionalHeader.CheckSum);
			LOG_INFO("Subsystem: 0x{:x}", ntHeader.OptionalHeader.Subsystem);
			LOG_INFO("DllCharacteristics: 0x{:x}", ntHeader.OptionalHeader.DllCharacteristics);
			LOG_INFO("SizeOfStackReserve: 0x{:x}", ntHeader.OptionalHeader.SizeOfStackReserve);
			LOG_INFO("SizeOfStackCommit: 0x{:x}", ntHeader.OptionalHeader.SizeOfStackCommit);
			LOG_INFO("SizeOfHeapReserve: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeapReserve);
			LOG_INFO("SizeOfHeapCommit: 0x{:x}", ntHeader.OptionalHeader.SizeOfHeapCommit);
			LOG_INFO("LoaderFlags: 0x{:x}", ntHeader.OptionalHeader.LoaderFlags);
			LOG_INFO("NumberOfRvaAndSizes: 0x{:x}", ntHeader.OptionalHeader.NumberOfRvaAndSizes);
		}

		size_t headersSize{ dosHeader.e_lfanew + ntHeader.FileHeader.SizeOfOptionalHeader + offsetof(decltype(ntHeader), OptionalHeader) };

		size_t max{ headersSize };
		max = std::max<size_t>(ntHeader.OptionalHeader.SizeOfImage, max);
		for (IMAGE_DATA_DIRECTORY& dir : ntHeader.OptionalHeader.DataDirectory) {
			max = std::max<size_t>(dir.VirtualAddress + dir.Size, max);
		}
		
		std::unique_ptr<byte[]> exeData{ std::make_unique<byte[]>(max) };


		CopyMemorySafe(proc, exeData.get(), main.start, max);

		PIMAGE_NT_HEADERS mntHeader{ (PIMAGE_NT_HEADERS)&exeData[dosHeader.e_lfanew] };
		PIMAGE_SECTION_HEADER sections{ IMAGE_FIRST_SECTION(mntHeader) };

		char nameBuff[9];
		nameBuff[8] = 0;
		for (size_t i = 0; i < mntHeader->FileHeader.NumberOfSections; i++) {
			PIMAGE_SECTION_HEADER sec{ &sections[i] };
			if (opt->dumpHeader) {
				std::memcpy(nameBuff, &sec->Name, sizeof(sec->Name));
				LOG_INFO("section '{}' va:0x{:x} sr:0x{:x} pr:0x{:x}", nameBuff, sec->VirtualAddress, sec->SizeOfRawData, sec->PointerToRawData);
			}
			// patch address
			sec->PointerToRawData = sec->VirtualAddress;
		}

		if (opt->rebuildIAT) {
			proc.ComputeModules();

			PIMAGE_DATA_DIRECTORY dir{ &mntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };

			PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)&exeData[dir->VirtualAddress] };
			for (; imports->Name; imports++) {

				const char* name{ (const char*)&exeData[imports->Name] };
				ProcessModule& mod{ proc[name] };
				if (!mod) {
					continue; // not loaded, ignore
				}
				mod.ComputeExports();

				LOG_TRACE("check {}", name);


				std::unordered_map<uintptr_t, IMAGE_THUNK_DATA> iatCaches{};
				{
					// cache iat names

					PIMAGE_THUNK_DATA thunks{ (PIMAGE_THUNK_DATA)&exeData[imports->FirstThunk] };
					PIMAGE_THUNK_DATA originalThunks{ (PIMAGE_THUNK_DATA)&exeData[imports->OriginalFirstThunk] };

					while (originalThunks->u1.Function) {
						IMAGE_THUNK_DATA& thunk{ *thunks++ };
						IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

						ProcessModuleExport* func;
						const char* id;
						if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
							//func = dep[(const char*)IMAGE_ORDINAL64(originalThunk.u1.Ordinal)];
							//if (!func) {
							//	LOG_WARNING("Can't find {}::ord<{}>", dep, IMAGE_ORDINAL64(originalThunk.u1.Ordinal));
							//	continue;
							//}
							continue; // flemme
						}
						else {
							id = (const char*)&exeData[originalThunk.u1.Function + 2];
							func = &mod[id];
						}
						if (!func) {
							LOG_WARNING("Can't find import entry {}::{}", name, id);
							continue;
						}

						//LOG_TRACE("{} {} {:x}", *func, (void*)func->m_location, (uint64_t)originalThunk.u1.Ordinal);
						iatCaches[func->m_location] = originalThunk;
					}
				}
				// remap them
				{
					PIMAGE_THUNK_DATA thunks{ (PIMAGE_THUNK_DATA)&exeData[imports->FirstThunk] };
					PIMAGE_THUNK_DATA originalThunks{ (PIMAGE_THUNK_DATA)&exeData[imports->OriginalFirstThunk] };
					size_t patchs{};
					while (originalThunks->u1.Function) {
						IMAGE_THUNK_DATA& thunk{ *thunks++ };
						IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

						uintptr_t val{ *(uintptr_t*)&thunk };

						auto it{ iatCaches.find(val) };
						if (it != iatCaches.end()) {
							if (originalThunk.u1.Ordinal != it->second.u1.Ordinal) {
								LOG_TRACE("patched iat 0x{:x} -> 0x{:x}", originalThunk.u1.Ordinal, it->second.u1.Ordinal);
								originalThunk = it->second;
								patchs++;
							}
						}
					}
					if (patchs) LOG_DEBUG("patched {} IAT issue(s)", patchs);
				}
			}
		}

		if (opt->searchIAT) {
			// TODO: patch
			throw std::runtime_error("searchIAT not implemented");

			core::bytebuffer::ByteBuffer reader{ exeData.get(), max };
			proc.ComputeModules();
			std::vector<ProcessModule>& modules{ proc.modules() };
			for (size_t i = 0; i < modules.size(); i++){
				ProcessModule& mod{ modules[i] };
				LOG_INFO("loading iat... {} {}% ({}/{})", mod.name, i * 100 / modules.size(), i + 1, modules.size());
				mod.ComputeExports();
				for (const ProcessModuleExport& exp : mod.exports()) {
					uintptr_t loc{ exp.m_location };
					for (size_t rva : reader.FindAll(&loc, sizeof(loc))) {
						LOG_INFO("found iat {} at 0x{:x}", exp.m_name.get(), rva);

						// TODO: find if != from IAT table
					}
				}
			}


		}

		if (!utils::WriteFile(out, exeData.get(), max)) {
			throw std::runtime_error(std::format("{} Can't write to {}", proc, out.string()));
		}
	}
	void DumpProcessExe(const std::filesystem::path& in, const std::filesystem::path& out, DumpProcessOpt* opt) {
		std::filesystem::path abs{ std::filesystem::absolute(in) };
		std::wstring instr{ abs.wstring() };
		LOG_TRACE("dump process {} -> {}", in.string(), out.string());

		HANDLE ht;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &ht)) {
			throw std::runtime_error("Can't adjust process token");
		}
		LUID luid;
		if (!LookupPrivilegeValueA(nullptr, SE_DEBUG_NAME, &luid)) {
			throw std::runtime_error("Can't find " SE_DEBUG_NAME " value");
		}
		TOKEN_PRIVILEGES tp{};
		tp.PrivilegeCount = 1;
		tp.Privileges->Luid = luid;
		tp.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(ht, FALSE, &tp, NULL, NULL, NULL);
		CloseHandle(ht);

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
			throw std::runtime_error(std::format("Can't start process {} / 0x{:x}", abs.string(), GetLastError()));
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
			opts.addOption(&opt.dumpHeader, "dump header", "--header", "", "-H");
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

			LOG_INFO("dump into {}", outPath.string());

			return tool::OK;
		}

		struct GameConfig {
			const char* id;
			const char* exe;
			const char* ddls;
			DumpProcessOpt opt;
		};

		static GameConfig gameConfigs[]{
			{ "iw", "iw7_ship.exe", "", {} },
			{ "bo3", "BlackOps3.exe", "", {} },
			{ "bo4", "BlackOps4.exe", "oo2core_6_win64.dll\0", {} },
			{ "cw", "BlackOpsColdWar.exe", "oo2core_8_win64.dll\0", {} },
			{ "bo6", "cod24-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
			{ "bo6sp", "sp24-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "bo7beta", "cod25-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
			{ "bo7", "../cod.exe", "../oo2core_8_win64.dll\0", { .rebuildIAT = true } },
			{ "vg", "Vanguard.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw19", "ModernWarfare.exe", "oo2core_7_win64.dll\0", {} },
			{ "mw2", "cod22-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw2sp", "sp22-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw3", "../cod23-cod.exe", "../oo2core_8_win64.dll\0", {} },
			{ "mw3hq", "cod23-cod.exe", "oo2core_8_win64.dll\0", {} },
			{ "mw3sp", "sp23-cod.exe", "oo2core_8_win64.dll\0", {} },
			//{ "deathloop", "Deathloop.exe", "oo2core_8_win64.dll\0oo2net_8_win64.dll\0", { .searchIAT = true } },
		};

		int game_dump(int argc, const char* argv[]) {
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

			LOG_INFO("Dumping {}...", cfg->id);

			LOG_INFO("----------");
			const char* dllName{ cfg->ddls };
			while (*dllName) {
				std::filesystem::path dllPath{ path / dllName };
				std::filesystem::path depsPath{ depsDir / dllPath.filename()};
				std::filesystem::copy_file(dllPath, depsPath, std::filesystem::copy_options::skip_existing);
				LOG_INFO("installed dll {} to {}", dllName, depsPath.string());
				dllName += std::strlen(dllName) + 1;
			}

			DumpProcessExe(path / cfg->exe, outPath, &cfg->opt);
			LOG_INFO("dumped {} to {}", cfg->exe, outPath.string());

			LOG_INFO("----------");

			LOG_INFO("game {} dumped, it can now be used.", cfg->id);
			return tool::OK;
		}

		ADD_TOOL(exe_dump, "common", "[exe] (out)", "Dump exe", exe_dump);
		ADD_TOOL(game_dump, "common", "[id] [path]", "Dump game exe to acts", game_dump);
	}
}