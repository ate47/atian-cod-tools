#include <includes.hpp>
#ifdef _WIN32
#include <utils/memapi.hpp>
#include <hook/library.hpp>
#include <tools/exe_dump.hpp>
#include <cli/cli_options.hpp>
#include <cli/clicolor.hpp>
#include <core/bytebuffer.hpp>
#include <acts_api/process.h>
#include <tools/mcp/mcp_tool.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace tool::exe_dump {
    const char* ImageDirName(size_t id) {
        switch (id) {
        case IMAGE_DIRECTORY_ENTRY_EXPORT:
            return "EXPORT";
        case IMAGE_DIRECTORY_ENTRY_IMPORT:
            return "IMPORT";
        case IMAGE_DIRECTORY_ENTRY_RESOURCE:
            return "RESOURCE";
        case IMAGE_DIRECTORY_ENTRY_EXCEPTION:
            return "EXCEPTION";
        case IMAGE_DIRECTORY_ENTRY_SECURITY:
            return "SECURITY";
        case IMAGE_DIRECTORY_ENTRY_BASERELOC:
            return "BASERELOC";
        case IMAGE_DIRECTORY_ENTRY_DEBUG:
            return "DEBUG";
        case IMAGE_DIRECTORY_ENTRY_ARCHITECTURE:
            return "ARCHITECTURE";
        case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:
            return "GLOBALPTR";
        case IMAGE_DIRECTORY_ENTRY_TLS:
            return "TLS";
        case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:
            return "LOAD_CONFIG";
        case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:
            return "BOUND_IMPORT";
        case IMAGE_DIRECTORY_ENTRY_IAT:
            return "IAT";
        case IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT:
            return "DELAY_IMPORT";
        case IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR:
            return "COM_DESCRIPTOR";
        default:
            return utils::va("UNKNOWN_%lld", id);
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
            VirtualQueryEx(
                proc.GetHandle(),
                (LPCVOID)current,
                reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page),
                sizeof(MEMORY_BASIC_INFORMATION)
            );
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
            VirtualQueryEx(
                proc.GetHandle(),
                (LPCVOID)current,
                reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page),
                sizeof(MEMORY_BASIC_INFORMATION)
            );
            current = (uintptr_t)page.BaseAddress;

            bool notCommit{ page.State != MEM_COMMIT };
            bool noAccess{ page.Protect == PAGE_NOACCESS };
            bool guard{ (page.Protect & PAGE_GUARD) != 0 };

            if (notCommit || noAccess || guard) {
                current += page.RegionSize;
                LOG_TRACE(
                    "Ignore page {}->{} (0x{:x}){}{}{}",
                    proc.GetLocation(page.BaseAddress),
                    proc.GetLocation(page.BaseAddress + page.RegionSize),
                    page.RegionSize,
                    notCommit ? " notCommit" : "",
                    noAccess ? " noAccess" : "",
                    guard ? " guard" : ""
                );
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
        IMAGE_NT_HEADERS64 ntHeader;

        if (!proc.ReadMemory(&dosHeader, main.start, sizeof(dosHeader)) || dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
            throw std::runtime_error(std::format("{} Can't read dos module header", proc));
        }

        if (!proc.ReadMemory(&ntHeader, main[dosHeader.e_lfanew], sizeof(ntHeader)) ||
            ntHeader.Signature != IMAGE_NT_SIGNATURE) {
            throw std::runtime_error(std::format("{} Can't read nt module header", proc));
        }

        if (opt->dumpHeader) {
            LOG_INFO("----- dir addresses -----");
            for (size_t i = 0; i < ACTS_ARRAYSIZE(ntHeader.OptionalHeader.DataDirectory); i++) {
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

        size_t headersSize{ dosHeader.e_lfanew + ntHeader.FileHeader.SizeOfOptionalHeader +
                            offsetof(decltype(ntHeader), OptionalHeader) };

        size_t max{ headersSize };
        max = std::max<size_t>(ntHeader.OptionalHeader.SizeOfImage, max);
        for (IMAGE_DATA_DIRECTORY& dir : ntHeader.OptionalHeader.DataDirectory) {
            max = std::max<size_t>(dir.VirtualAddress + dir.Size, max);
        }
        LOG_DEBUG("copy {} : {:x} (0x{:x})", main, main.start + max, max);

        std::vector<byte> exeData{};
        exeData.resize(max);

        CopyMemorySafe(proc, exeData.data(), main.start, exeData.size());

        PIMAGE_NT_HEADERS mntHeader{ (PIMAGE_NT_HEADERS)&exeData[dosHeader.e_lfanew] };
        LOG_DEBUG("mntHeader->OptionalHeader.ImageBase {} : {:x}", main, mntHeader->OptionalHeader.ImageBase);
        PIMAGE_SECTION_HEADER sections{ IMAGE_FIRST_SECTION(mntHeader) };

        char nameBuff[9];
        nameBuff[8] = 0;
        for (size_t i = 0; i < mntHeader->FileHeader.NumberOfSections; i++) {
            PIMAGE_SECTION_HEADER sec{ &sections[i] };
            size_t shend{ (size_t)(sec->VirtualAddress +
                                   (sec->Misc.VirtualSize ? sec->Misc.VirtualSize : sec->SizeOfRawData)) };
            if (opt->dumpHeader) {
                std::memcpy(nameBuff, &sec->Name, sizeof(sec->Name));
                LOG_INFO(
                    "section '{}' va:0x{:x} sr:0x{:x}/vs:0x{:x} pr:0x{:x} -> 0x{:x}  ({:x}->{:x})",
                    nameBuff,
                    sec->VirtualAddress,
                    sec->SizeOfRawData,
                    sec->Misc.VirtualSize,
                    sec->PointerToRawData,
                    shend,
                    main.start + sec->VirtualAddress,
                    main.start + shend
                );
            }

            // patch address
            sec->PointerToRawData = sec->VirtualAddress;

            if (sec->Misc.VirtualSize) {
                sec->SizeOfRawData = sec->Misc.VirtualSize;
            }
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

                    PIMAGE_THUNK_DATA64 thunks{ (PIMAGE_THUNK_DATA64)&exeData[imports->FirstThunk] };
                    PIMAGE_THUNK_DATA64 originalThunks{ (PIMAGE_THUNK_DATA64)&exeData[imports->OriginalFirstThunk] };

                    while (originalThunks->u1.Function) {
                        IMAGE_THUNK_DATA64& thunk{ *thunks++ };
                        IMAGE_THUNK_DATA64& originalThunk{ *originalThunks++ };

                        ProcessModuleExport* func;
                        const char* id;
                        if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
                            // func = dep[(const char*)IMAGE_ORDINAL64(originalThunk.u1.Ordinal)];
                            // if (!func) {
                            //	LOG_WARNING("Can't find {}::ord<{}>", dep, IMAGE_ORDINAL64(originalThunk.u1.Ordinal));
                            //	continue;
                            // }
                            continue; // flemme
                        } else {
                            id = (const char*)&exeData[originalThunk.u1.Function + 2];
                            func = &mod[id];
                        }
                        if (!func) {
                            LOG_WARNING("Can't find import entry {}::{}", name, id);
                            continue;
                        }

                        // LOG_TRACE("{} {} {:x}", *func, (void*)func->m_location, (uint64_t)originalThunk.u1.Ordinal);
                        iatCaches[func->m_location] = originalThunk;
                    }
                }
                // remap them
                {
                    PIMAGE_THUNK_DATA64 thunks{ (PIMAGE_THUNK_DATA64)&exeData[imports->FirstThunk] };
                    PIMAGE_THUNK_DATA64 originalThunks{ (PIMAGE_THUNK_DATA64)&exeData[imports->OriginalFirstThunk] };
                    size_t patchs{};
                    while (originalThunks->u1.Function) {
                        IMAGE_THUNK_DATA64& thunk{ *thunks++ };
                        IMAGE_THUNK_DATA64& originalThunk{ *originalThunks++ };

                        uintptr_t val{ *(uintptr_t*)&thunk };

                        auto it{ iatCaches.find(val) };
                        if (it != iatCaches.end()) {
                            if (originalThunk.u1.Ordinal != it->second.u1.Ordinal) {
                                LOG_TRACE(
                                    "patched iat 0x{:x} -> 0x{:x}",
                                    originalThunk.u1.Ordinal,
                                    it->second.u1.Ordinal
                                );
                                originalThunk = it->second;
                                patchs++;
                            }
                        }
                    }
                    LOG_DEBUG("patched {} IAT issue(s) from {}", patchs, name);
                }
            }
        }

        if (opt->searchIAT) {
            // TODO: patch
            throw std::runtime_error("searchIAT not implemented");

            core::bytebuffer::ByteBuffer reader{ exeData };
            proc.ComputeModules();
            std::vector<ProcessModule>& modules{ proc.modules() };
            for (size_t i = 0; i < modules.size(); i++) {
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

        if (!utils::WriteFile(out, exeData)) {
            throw std::runtime_error(std::format("{} Can't write to {}", proc, out.string()));
        }
    }

    const char* DebugEventName(DWORD id) {
        switch (id) {
        case EXCEPTION_DEBUG_EVENT:
            return "EXCEPTION_DEBUG_EVENT";
        case CREATE_THREAD_DEBUG_EVENT:
            return "CREATE_THREAD_DEBUG_EVENT";
        case CREATE_PROCESS_DEBUG_EVENT:
            return "CREATE_PROCESS_DEBUG_EVENT";
        case EXIT_THREAD_DEBUG_EVENT:
            return "EXIT_THREAD_DEBUG_EVENT";
        case EXIT_PROCESS_DEBUG_EVENT:
            return "EXIT_PROCESS_DEBUG_EVENT";
        case LOAD_DLL_DEBUG_EVENT:
            return "LOAD_DLL_DEBUG_EVENT";
        case UNLOAD_DLL_DEBUG_EVENT:
            return "UNLOAD_DLL_DEBUG_EVENT";
        case OUTPUT_DEBUG_STRING_EVENT:
            return "OUTPUT_DEBUG_STRING_EVENT";
        case RIP_EVENT:
            return "RIP_EVENT";
        default:
            return utils::va("DBG_UNK_%x", id);
        }
    }

    void RenderDebugEvent(core::logs::loglevel lvl, DEBUG_EVENT& ev, Process& proc) {
        if (!HAS_LOG_LEVEL(lvl)) {
            return; // no render
        }

        proc.ComputeModules();

        switch (ev.dwDebugEventCode) {
        case EXCEPTION_DEBUG_EVENT: {
            EXCEPTION_RECORD& rec{ ev.u.Exception.ExceptionRecord };
            LOG_LVLF(lvl, "EXCEPTION_DEBUG_EVENT");
            LOG_LVLF(lvl, "ExceptionCode ............ 0x{:x}", rec.ExceptionCode);
            LOG_LVLF(lvl, "ExceptionFlags ........... 0x{:x}", rec.ExceptionFlags);
            LOG_LVLF(lvl, "ExceptionAddress ......... {}", ProcessLocation{ proc, (uintptr_t)rec.ExceptionAddress });
            for (size_t i = 0; i < rec.NumberParameters; i++) {
                LOG_LVLF(lvl, "ExceptionInformation[{}] . 0x{:x}", i, (uint64_t)rec.ExceptionInformation[i]);
            }
            break;
        }
        case CREATE_THREAD_DEBUG_EVENT: {
            LOG_LVLF(
                lvl,
                "CREATE_THREAD_DEBUG_EVENT: {}",
                ProcessLocation{ proc, (uintptr_t)ev.u.CreateThread.lpStartAddress }
            );
            break;
        }
        case CREATE_PROCESS_DEBUG_EVENT: {
            LOG_LVLF(
                lvl,
                "CREATE_PROCESS_DEBUG_EVENT: {}",
                ProcessLocation{ proc, (uintptr_t)ev.u.CreateProcessInfo.lpStartAddress }
            );
            break;
        }
        case LOAD_DLL_DEBUG_EVENT:
            break; // ignore
        default:
            LOG_LVLF(lvl, "{}", DebugEventName(ev.dwDebugEventCode));
            break;
        }
    }
    void InitProcessExe(const std::filesystem::path& in, std::function<void(Process& proc)> callback) {
        std::filesystem::path abs{ std::filesystem::absolute(in) };
        std::wstring instr{ abs.wstring() };

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

        if (!CreateProcessW(NULL, instr.data(), NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi)) {
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

            RenderDebugEvent(core::logs::LVL_TRACE, ev, proc);

            if (ev.dwDebugEventCode != EXIT_THREAD_DEBUG_EVENT &&
                !(ev.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                  (ev.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION ||
                   ev.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION))) {
                ContinueDebugEvent(ev.dwProcessId, ev.dwThreadId, DBG_CONTINUE);
                continue;
            }

            LOG_TRACE("received end dbg event");
            callback(proc);
            break;
        }
    }

    void DumpProcessExe(const std::filesystem::path& in, const std::filesystem::path& out, DumpProcessOpt* opt) {
        LOG_TRACE("dump process {} -> {}", in.string(), out.string());
        InitProcessExe(in, [opt, &out](Process& proc) {
            if (!opt->noDump) {
                DumpProcess(proc, out, opt);
            }
        });
    }

    namespace {
        int exe_dump(int argc, const char* argv[]) {
            DumpProcessOpt opt{};
            bool showHelp{};
            cli::options::CliOptions opts{};
            opts.addOption(&opt.rebuildIAT, "rebuild IAT table", "--rebuildIAT");
            opts.addOption(&opt.searchIAT, "search IAT table", "--searchIAT");
            opts.addOption(&opt.dumpHeader, "dump header", "--header", "", "-H");
            opts.addOption(&opt.noDump, "no dump (debug)", "--noDump");
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
            } else {
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

            void WriteDumpInfo(ActsAPIProcess_GameDumpInformation& gopt) const {
                gopt.id = id;
                gopt.exe = exe;
                gopt.ddls = ddls;
                gopt.rebuildIAT = opt.rebuildIAT;
            }
        };

        static GameConfig gameConfigs[]{
            { "iw", "iw7_ship.exe", "", {} },
            { "bo3", "BlackOps3.exe", "", {} },
            { "bo4", "BlackOps4.exe", "oo2core_6_win64.dll\0", {} },
            { "cw", "BlackOpsColdWar.exe", "oo2core_8_win64.dll\0", {} },
            { "bo6", "cod24-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
            { "bo6sp", "sp24-cod.exe", "oo2core_8_win64.dll\0", {} },
            { "bo7beta", "cod25-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
            { "bo7", "cod25-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
            { "vg", "Vanguard.exe", "oo2core_8_win64.dll\0", {} },
            { "mw19", "ModernWarfare.exe", "oo2core_7_win64.dll\0", {} },
            { "mw2", "cod22-cod.exe", "oo2core_8_win64.dll\0", {} },
            { "mw2sp", "sp22-cod.exe", "oo2core_8_win64.dll\0", {} },
            { "mw3", "../cod23-cod.exe", "../oo2core_8_win64.dll\0", {} },
            { "mw3hq", "cod23-cod.exe", "oo2core_8_win64.dll\0", {} },
            { "mw3sp", "sp23-cod.exe", "oo2core_8_win64.dll\0", {} },
            { "mw4beta", "cod26-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
            { "mw4", "../cod.exe", "../oo2core_8_win64.dll\0", { .rebuildIAT = true } },
            { "mw4sp", "sp26-cod.exe", "oo2core_8_win64.dll\0", { .rebuildIAT = true } },
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
                std::filesystem::path depsPath{ depsDir / dllPath.filename() };
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

        // MCP tools

        // Get a string argument, nullptr if missing
        const char* GetMcpStringArg(const rapidjson::Value& args, const char* key) {
            if (!args.IsObject() || !args.HasMember(key) || !args[key].IsString() || !*args[key].GetString()) {
                return nullptr;
            }
            return args[key].GetString();
        }

        // Get a bool argument, false if missing
        bool GetMcpBoolArg(const rapidjson::Value& args, const char* key) {
            return args.IsObject() && args.HasMember(key) && args[key].IsBool() && args[key].GetBool();
        }

        // Default dump path of an exe: deps/<exe name>_dump.exe
        std::filesystem::path GetDefaultDumpPath(const std::filesystem::path& exe) {
            std::filesystem::path name{ exe.filename() };
            name.replace_extension();
            return utils::GetProgDir() / "deps" / std::format("{}_dump.exe", name.string());
        }

        // Find a game config by its id, nullptr if not found
        GameConfig* FindGameConfig(const char* id) {
            for (GameConfig& cfg : gameConfigs) {
                if (!_strcmpi(cfg.id, id)) {
                    return &cfg;
                }
            }
            return nullptr;
        }

        std::string ToMcpJson(rapidjson::StringBuffer& sb) { return std::string{ sb.GetString(), sb.GetSize() }; }

        std::string McpDumpResult(const std::filesystem::path& out) {
            rapidjson::StringBuffer sb{};
            rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };
            writer.StartObject();
            writer.Key("output");
            writer.String(out.string().c_str());
            writer.EndObject();
            return ToMcpJson(sb);
        }

        std::string mcp_dump_exe(const rapidjson::Value& args, bool& isError) {
            const char* exe{ GetMcpStringArg(args, "exe") };
            if (!exe) {
                isError = true;
                return "Missing 'exe' argument";
            }
            std::filesystem::path exePath{ exe };
            std::error_code ec;
            if (!std::filesystem::is_regular_file(exePath, ec)) {
                isError = true;
                return std::format("'{}' isn't a file", exe);
            }
            const char* output{ GetMcpStringArg(args, "output") };
            std::filesystem::path outPath{ output ? std::filesystem::path{ output } : GetDefaultDumpPath(exePath) };

            DumpProcessOpt opt{};
            opt.rebuildIAT = GetMcpBoolArg(args, "rebuildIAT");
            try {
                std::filesystem::create_directories(outPath.parent_path(), ec);
                DumpProcessExe(exePath, outPath, &opt);
            } catch (std::exception& e) {
                isError = true;
                return e.what();
            }
            return McpDumpResult(outPath);
        }

        std::string mcp_dump_process(const rapidjson::Value& args, bool& isError) {
            if (!args.IsObject() || !args.HasMember("pid") || !args["pid"].IsUint()) {
                isError = true;
                return "Missing 'pid' argument";
            }
            const char* output{ GetMcpStringArg(args, "output") };

            try {
                Process proc{ NULL, args["pid"].GetUint() };
                if (!proc.Open()) {
                    isError = true;
                    return std::format("Can't open process {}", args["pid"].GetUint());
                }
                std::filesystem::path outPath;
                if (output) {
                    outPath = output;
                } else {
                    proc.ComputeModules();
                    outPath = GetDefaultDumpPath(proc.GetMainModule().path);
                }

                DumpProcessOpt opt{};
                opt.rebuildIAT = GetMcpBoolArg(args, "rebuildIAT");
                std::error_code ec;
                std::filesystem::create_directories(outPath.parent_path(), ec);
                DumpProcess(proc, outPath, &opt);
                return McpDumpResult(outPath);
            } catch (std::exception& e) {
                isError = true;
                return e.what();
            }
        }

        std::string mcp_list_game_dumps(const rapidjson::Value& args, bool& isError) {
            rapidjson::StringBuffer sb{};
            rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };
            writer.StartArray();
            for (const GameConfig& cfg : gameConfigs) {
                writer.StartObject();
                writer.Key("id");
                writer.String(cfg.id);
                writer.Key("exe");
                writer.String(cfg.exe);
                writer.Key("dlls");
                writer.StartArray();
                // the dlls are \0 separated
                for (const char* dll{ cfg.ddls }; *dll; dll += std::strlen(dll) + 1) {
                    writer.String(dll);
                }
                writer.EndArray();
                writer.Key("rebuildIAT");
                writer.Bool(cfg.opt.rebuildIAT);
                writer.EndObject();
            }
            writer.EndArray();
            return ToMcpJson(sb);
        }

        std::string mcp_dump_game(const rapidjson::Value& args, bool& isError) {
            const char* id{ GetMcpStringArg(args, "id") };
            const char* root{ GetMcpStringArg(args, "path") };
            if (!id || !root) {
                isError = true;
                return "Missing 'id' or 'path' argument";
            }
            GameConfig* cfg{ FindGameConfig(id) };
            if (!cfg) {
                std::string ids{};
                for (const GameConfig& c : gameConfigs) {
                    ids += std::format("{}{}", ids.empty() ? "" : ", ", c.id);
                }
                isError = true;
                return std::format("Unknown game id '{}', available ids: {}", id, ids);
            }

            std::filesystem::path rootPath{ root };
            std::filesystem::path exePath{ rootPath / cfg->exe };
            std::error_code ec;
            if (!std::filesystem::is_regular_file(exePath, ec)) {
                isError = true;
                return std::format("Can't find the game exe '{}', check the game path", exePath.string());
            }

            const char* output{ GetMcpStringArg(args, "output") };
            std::filesystem::path outPath{ output ? std::filesystem::path{ output } : GetDefaultDumpPath(exePath) };
            // the dlls are copied next to the dump
            std::filesystem::path depsDir{ outPath.parent_path() };

            try {
                std::filesystem::create_directories(depsDir);

                rapidjson::StringBuffer sb{};
                rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };
                writer.StartObject();

                writer.Key("installedDlls");
                writer.StartArray();
                for (const char* dll{ cfg->ddls }; *dll; dll += std::strlen(dll) + 1) {
                    std::filesystem::path dllPath{ rootPath / dll };
                    std::filesystem::path depsPath{ depsDir / dllPath.filename() };
                    std::filesystem::copy_file(dllPath, depsPath, std::filesystem::copy_options::skip_existing);
                    writer.String(depsPath.string().c_str());
                }
                writer.EndArray();

                DumpProcessExe(exePath, outPath, &cfg->opt);

                writer.Key("output");
                writer.String(outPath.string().c_str());
                writer.EndObject();
                return ToMcpJson(sb);
            } catch (std::exception& e) {
                isError = true;
                return e.what();
            }
        }

        ADD_MCP_TOOL(
            dump_exe, "common",
            "Dump an exe protected by an anti-cheat/packer: the exe is started under a debugger and its unpacked image "
            "is written into 'output' (default: deps/<exe name>_dump.exe in the ACTS directory). WARNING: this runs "
            "the "
            "exe. 'rebuildIAT' rebuilds the import table, required for the games where the anti-cheat mixes it. To "
            "dump a game, prefer dump_game. Returns { output }.",
            R"({"type":"object","properties":{"exe":{"type":"string","description":"path of the exe to dump"},"output":{"type":"string","description":"optional output file"},"rebuildIAT":{"type":"boolean"}},"required":["exe"]})",
            mcp_dump_exe
        );
        ADD_MCP_TOOL(
            dump_process, "common",
            "Dump the main module of a running process by its pid, it is written into 'output' (default: "
            "deps/<exe name>_dump.exe in the ACTS directory). 'rebuildIAT' rebuilds the import table. Returns "
            "{ output }.",
            R"({"type":"object","properties":{"pid":{"type":"integer","minimum":0},"output":{"type":"string","description":"optional output file"},"rebuildIAT":{"type":"boolean"}},"required":["pid"]})",
            mcp_dump_process
        );
        ADD_MCP_TOOL(
            list_game_dumps, "common",
            "List the games supported by dump_game. Returns an array of { id, exe (relative to the game directory), "
            "dlls (copied next to the dump), rebuildIAT }.",
            nullptr, mcp_list_game_dumps
        );
        ADD_MCP_TOOL(
            dump_game, "common",
            "Dump the exe of a game so it can be used by ACTS (the exe is started to be dumped, the required dlls are "
            "copied next to the dump). 'id' is a game id from list_game_dumps, 'path' is the game directory. The "
            "dump is written into 'output' (default: deps/<exe name>_dump.exe in the ACTS directory). Returns { "
            "installedDlls[], output }.",
            R"({"type":"object","properties":{"id":{"type":"string","description":"game id, see list_game_dumps"},"path":{"type":"string","description":"game directory"},"output":{"type":"string","description":"optional output file"}},"required":["id","path"]})",
            mcp_dump_game
        );

        ADD_TOOL(exe_dump, "common", "[exe] (out)", "Dump exe", exe_dump);
        ADD_TOOL(game_dump, "common", "[id] [path]", "Dump game exe to acts", game_dump);
    } // namespace
} // namespace tool::exe_dump

ActsStatus ActsAPIProcess_DumpProcessExe(const char* in, const char* out, ActsAPIProcess_DumpProcessOption* options) {
    tool::exe_dump::DumpProcessOpt opt{};
    if (options) {
        opt.rebuildIAT = options->rebuildIAT;
    }
    try {
        std::filesystem::path exePath{ in };
        std::filesystem::path outPath;
        if (!out || !*out) {
            outPath = exePath.filename();
            outPath.replace_extension();
            outPath = utils::GetProgDir() / "deps" / std::format("{}_dump.exe", outPath.string());
        } else {
            outPath = out;
        }

        tool::exe_dump::DumpProcessExe(exePath, outPath, &opt);

        std::string outPathStr{ outPath.string() };
        ActsAPISetLastMessage("Dumped to %s", outPathStr.c_str());
        return ActsStatus::ACTS_STATUS_OK;
    } catch (std::runtime_error& e) {
        ActsAPISetLastMessage("%s", e.what());
        return ActsStatus::ACTS_STATUS_ERROR;
    }
}

ActsStatus ActsAPIProcess_DumpProcess(uint32_t pid, const char* out, ActsAPIProcess_DumpProcessOption* options) {
    try {
        Process proc{ NULL, pid };

        if (!proc.Open()) {
            ActsAPISetLastMessage("Can't open process %ul", pid);
            return ActsStatus::ACTS_STATUS_ERROR;
        }
        tool::exe_dump::DumpProcessOpt opt{};
        if (options) {
            opt.rebuildIAT = options->rebuildIAT;
        }

        std::filesystem::path outPath;
        if (!out || !*out) {
            proc.ComputeModules();
            outPath = proc.GetMainModule().path;
            outPath = outPath.filename();
            outPath.replace_extension();
            outPath = utils::GetProgDir() / "deps" / std::format("{}_dump.exe", outPath.string());
        } else {
            outPath = out;
        }

        tool::exe_dump::DumpProcess(proc, outPath, &opt);

        std::string outPathStr{ outPath.string() };
        ActsAPISetLastMessage("Dumped to %s", outPathStr.c_str());
        return ActsStatus::ACTS_STATUS_OK;
    } catch (std::runtime_error& e) {
        ActsAPISetLastMessage("%s", e.what());
        return ActsStatus::ACTS_STATUS_ERROR;
    }
}
ActsAPIProcess_GameDumpInformationList* ActsAPIProcess_GetGameDumpsList() {
    static struct {
        ActsAPIProcess_GameDumpInformation games[ACTS_ARRAYSIZE(tool::exe_dump::gameConfigs)]{};
        ActsAPIProcess_GameDumpInformationList list{};
    } _s;

    if (!_s.list.count) {
        _s.list.count = ACTS_ARRAYSIZE(tool::exe_dump::gameConfigs);
        _s.list.values = _s.games;

        ActsAPIProcess_GameDumpInformation* game{ _s.games };
        for (tool::exe_dump::GameConfig& cfg : tool::exe_dump::gameConfigs) {
            cfg.WriteDumpInfo(*(game++));
        }
    }

    return &_s.list;
}
ActsStatus ActsAPIProcess_DumpGame(ActsAPIProcess_GameDumpInformation& game, const char* root, const char* out) {
    try {
        std::filesystem::path path{ root };
        std::filesystem::path depsDir;
        std::filesystem::path outPath;
        if (!out || !*out) {
            depsDir = utils::GetProgDir() / "deps";
            outPath = game.exe;
            outPath = outPath.filename();
            outPath.replace_extension();
            outPath = utils::GetProgDir() / "deps" / std::format("{}_dump.exe", outPath.string());
        } else {
            outPath = out;
            depsDir = outPath.parent_path();
        }

        const char* dllName{ game.ddls };
        while (*dllName) {
            std::filesystem::path dllPath{ path / dllName };
            std::filesystem::path depsPath{ depsDir / dllPath.filename() };
            std::filesystem::copy_file(dllPath, depsPath, std::filesystem::copy_options::skip_existing);
            LOG_INFO("installed dll {} to {}", dllName, depsPath.string());
            dllName += std::strlen(dllName) + 1;
        }

        tool::exe_dump::DumpProcessOpt opt{};
        opt.rebuildIAT = game.rebuildIAT;

        tool::exe_dump::DumpProcessExe(path / game.exe, outPath, &opt);
        return ActsStatus::ACTS_STATUS_OK;
    } catch (std::runtime_error& e) {
        ActsAPISetLastMessage("%s", e.what());
        return ActsStatus::ACTS_STATUS_ERROR;
    }
}

#endif // _WIN32