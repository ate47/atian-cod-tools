#include <ui_includes.hpp>
#include <utils/memapi.hpp>
#include <utils/memapi_calls.hpp>
#include <games/bo4/scriptinstance.hpp>
#include <games/bo4/offsets.hpp>
#include <utils/hash.hpp>
#include <cli/clisync.hpp>
#include <acts.hpp>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace {
    using namespace games::bo4;

    struct ReadScriptDetour { // t8compiler/t8cinternal/detours.cpp
        int32_t FixupName;
        int32_t ReplaceNamespace;
        int32_t ReplaceFunction;
        int32_t FixupOffset;
        int32_t FixupSize;
    };

    struct T8ScriptParseTreeEntry {
        uint64_t name;
        uint64_t pad0;
        uintptr_t buffer;
        uint32_t size;
        uint32_t pad02;
    };

    struct T8ScriptInjectionEntry {
        bool injected = false;
        DWORD lastInjectionPID = 0;
        T8ScriptParseTreeEntry prePatchEntry{};
        uintptr_t prePatchEntryLocation = 0;
        bool detoursInjected = false;
        uintptr_t injectedScript = 0;
        size_t injectedScriptSize = 0;
    };

    struct T8GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t ukn20;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t ukn2c;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t ukn4c_offset;
        int32_t ukn50;
        int32_t ukn54;
        uint16_t include_count;
        byte ukn5a;
        byte ukn4c_count;


        // @return the vm
        inline byte GetVm() {
            return magic[7];
        }
    };

    struct T8GSCImport {
        uint32_t name;
        uint32_t import_namespace;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    constexpr int AUI_WIDTH = 800;
    constexpr int AUI_HEIGHT = 600;

    struct ActsConfig {
    
        enum ActsConfigFlags {
            ACTS_DLL = 1,
            SERIOUS_DLL = 2
        };
        std::wstring name{};
        std::wstring injectionPoint{};
        bool actsDll{};
        bool seriousDll{};

        static void ReadString(std::istream& is, std::wstring& str) {
            size_t strLen{};
            is.read((char*)&strLen, sizeof(strLen));

            wchar_t path[MAX_PATH + 1];

            while (strLen >= sizeof(path) / sizeof(*path)) {
                is.read((char*)path, sizeof(path));
                strLen -= sizeof(path) / sizeof(*path);
            }

            *path = 0;

            if (strLen) {
                is.read((char*)path, strLen * sizeof(*path));
            }

            path[strLen] = '\0';
            
            str = path;
        }
        static void WriteString(std::ostream& of, std::wstring& str) {
            size_t size = str.size();
            of.write((char*)&size, sizeof(size));
            of.write((char*)str.data(), size * sizeof(*str.data()));
        }

        const std::filesystem::path& GetCfgPath() {
            static std::filesystem::path path = ([]() {
                wchar_t szFileName[MAX_PATH];
                GetModuleFileName(NULL, szFileName, MAX_PATH);
                std::filesystem::path progpath = std::filesystem::absolute(szFileName).parent_path();
                return progpath / L"actsui.bincfg";
            })();
            return path;
        }

        void Sync() {
            std::ifstream is{ GetCfgPath() , std::ios::in | std::ios::binary };

            if (is) {
                // read cfg

                char flags{};
                is.read(&flags, 1);

                actsDll = (flags & ActsConfigFlags::ACTS_DLL) != 0;
                seriousDll = (flags & ActsConfigFlags::SERIOUS_DLL) != 0;

                ReadString(is, name);
                ReadString(is, injectionPoint);

                is.close();
            }


            Save();
        }
        void Save() {
            std::ofstream of{ GetCfgPath(), std::ios::out | std::ios::binary };

            if (!of) {
                return;
            }
        
            char flags{};
            if (actsDll) {
                flags |= ActsConfigFlags::ACTS_DLL;
            }
            if (seriousDll) {
                flags |= ActsConfigFlags::SERIOUS_DLL;
            }

            of.write(&flags, 1);

            WriteString(of, name);
            WriteString(of, injectionPoint);

            of.close();
        }
    };

    class ActsWindow {
    public:
        ActsConfig cfg{};
        HWND hwnd;
        HWND filePathEdit;
        HWND filePathEditLabel;
        HWND filePathButton;
        HWND injectHookCombo;
        HWND injectHookComboLabel;
        HWND injectButton;
        HWND revertButton;
        HWND injectDLLButton;
        HWND injectSeriousDLLButton;
        HWND statusMessageText;

        T8ScriptInjectionEntry injectedScript{};

        std::unordered_map<HWND, std::function<void()>> buttonEvent{};

        void SaveCfg() {
            cfg.actsDll = Button_GetCheck(injectDLLButton);
            cfg.seriousDll = Button_GetCheck(injectSeriousDLLButton);

            wchar_t injectPath[std::max(MAX_PATH, 256)];

            if (SUCCEEDED(ComboBox_GetText(injectHookCombo, &injectPath[0], sizeof(injectPath) / sizeof(*injectPath)))) {
                cfg.injectionPoint = injectPath;
            }

            if (SUCCEEDED(ComboBox_GetText(filePathEdit, &injectPath[0], sizeof(injectPath) / sizeof(*injectPath)))) {
                cfg.name = injectPath;
            }

            // save config for later
            cfg.Save();
        }

        void ClearPreviousScript() {
            if (!injectedScript.injected) {
                Button_Enable(revertButton, FALSE);
                return;
            }

            Process game{ L"blackops4" };

            if (!game || game.m_pid != injectedScript.lastInjectionPID || !game.Open()) {
                Button_Enable(revertButton, FALSE);
                return; // no need to open the handle if the pid is not the same
            }

            ClearPreviousScript(game);
        }

        void ClearPreviousScript(Process& game) {
            Button_Enable(revertButton, FALSE);
            if (!injectedScript.injected) {
                return;
            }
            injectedScript.injected = false;

            if (game.m_pid != injectedScript.lastInjectionPID) {
                return; // not injected in this game
            }

            // put back the entry info
            game.WriteMemory(injectedScript.prePatchEntryLocation, &injectedScript.prePatchEntry, sizeof(injectedScript.prePatchEntryLocation));

            // free the script
            game.FreeMemory(injectedScript.injectedScript, injectedScript.injectedScriptSize);

            if (injectedScript.detoursInjected) {
                memapi::Call<int>(game, game["t8cinternal.dll"]["RemoveDetours"], scriptinstance::SI_SERVER);
            }
        }

        std::wstring InjectScript() {

            void* script = nullptr;
            void* file = nullptr;
            size_t size = 0;
            size_t sizeAlign = 0;
            if (!utils::ReadFileAlign(cfg.name, file, script, size, sizeAlign)) {
                return std::format(L"Can't read {}", cfg.name);
            }

            if (sizeAlign < sizeof(T8GSCOBJ)) {
                return L"Bad GSC header size"; // avoid weird things
            }

            auto res = InjectScript((T8GSCOBJ*)file, sizeAlign);

            std::free(script);

            return res;
        }

        std::pair<bool, const wchar_t*> NeedBuiltins(T8GSCOBJ* obj, size_t size) {
            if (!obj->imports_count) {
                return std::make_pair(false, nullptr);
            }
            if (obj->imports_offset + obj->imports_count * sizeof(T8GSCImport) > size) {
                return std::make_pair(false, L"Invalid GSC file, bad imports");
            }
            constexpr uint32_t customBuiltinsHash = hash::HashT89Scr("IsProfileBuild");

            T8GSCImport* imports = reinterpret_cast<T8GSCImport*>(obj->magic + obj->imports_offset);
            for (size_t i = 0; i < obj->imports_count; i++) {
                if (imports->name == customBuiltinsHash && imports->param_count > 0 && (imports->flags & 0xF) != 1) { 
                    // by default this function has 0 params, so a positive param count is saying we're using compiler::xxxxx
                    // flags&F = 1 => GetFunction operator, so we ignore it
                    return std::make_pair(true, nullptr);
                }
                // pass import
                imports = reinterpret_cast<T8GSCImport*>(reinterpret_cast<uintptr_t>(imports + 1) + imports->num_address * sizeof(uint32_t));
            }

            return std::make_pair(false, nullptr);
        }

        std::wstring InjectScript(T8GSCOBJ* obj, size_t size) {
            LOG_INFO("Injecting script...");
            constexpr auto detourinfosize = 256;
            struct GsicDetour {
                uint32_t name;
                uint32_t replaceNamespace;
                uint32_t replaceFunction;
                int32_t fixupOffset;
                int32_t fixupSize;
                int32_t replaceScriptTop;
                int32_t replaceScriptBottom;
            };
            struct {
                std::vector<GsicDetour*> detours{};
            } gsicInfo{};
            auto magic = *reinterpret_cast<uint64_t*>(obj->magic);

            if (!memcmp(obj->magic, "GSIC", 4)) {
                // GSIC file, reading detours
                LOG_INFO("GSIC magic detected");

                size_t location = 4; // pass after the magic

                auto fields = *reinterpret_cast<int32_t*>(obj->magic + location);
                location += 4;

                for (size_t i = 0; i < fields; i++) {
                    if (location + 4 > size) {
                        return L"Invalid GSIC file (GSIC field type)";
                    }
                    auto fieldtype = *reinterpret_cast<int32_t*>(obj->magic + location);
                    location += 4;

                    switch (fieldtype) {
                    case 0: {// detour

                        if (location + 4 > size) {
                            return L"Invalid GSIC file (GSIC detour count)";
                        }
                        auto detourCount = *reinterpret_cast<int32_t*>(obj->magic + location);
                        location += 4;

                        for (size_t j = 0; j < detourCount; j++) {
                            if (location + detourinfosize > size) {
                                return L"Invalid GSIC file (GSIC detour field)";
                            }

                            gsicInfo.detours.push_back(reinterpret_cast<GsicDetour*>(&obj->magic[location]));
                            location += detourinfosize;
                        }
                    }
                        break;
                    default: // unknown
                        return std::format(L"Unknown GSIC field type: {}", fieldtype);
                    }
                }

                // move after gsic header
                obj = reinterpret_cast<T8GSCOBJ*>(reinterpret_cast<uintptr_t>(obj) + location);
                size -= location;

                if (size < sizeof(T8GSCOBJ)) {
                    return L"Bad GSC header size"; // avoid weird things again
                }
                LOG_INFO("parsed {} detours", gsicInfo.detours.size());
                magic = *reinterpret_cast<uint64_t*>(obj->magic);
            }

            if (magic != 0x36000A0D43534780) {
                return L"This script isn't a valid script for bo4 (BAD MAGIC)";
            }

            // we test if we need to inject the builtins
            auto [needBuiltins, err] = NeedBuiltins(obj, size);

            if (err) {
                return err;
            }

            if (needBuiltins && !cfg.seriousDll) {
                if (
                    MessageBoxW(hwnd, 
                        L"This script is requiring the serious DLL to be injected, but it wasn't checked.\n"
                        L"Do you want to continue without the DLL?\n"
                        L"Error: Usage of IsProfileBuild() with parameters", 
                        L"Serious DLL detected",
                        MB_OKCANCEL | MB_ICONWARNING
                    ) == IDCANCEL) {
                    return L"Injection cancelled";
                }
            }

            // Inject the code
            static auto serverReplaceScript = hash::Hash64Pattern("scripts/core_common/clientids_shared.gsc");
            auto hookScript = hash::Hash64Pattern(cfg.injectionPoint.c_str());

            Process game{ L"blackops4.exe" };

            if (!game) {
                return L"Can't find Black Ops 4 game instance, is your game started?";
            }

            if (!game.Open()) {
                return L"Can't open the game instance, try to run the injector as administrator.";
            }

            uintptr_t poolPtr = game.ReadMemory<uintptr_t>(game[offset::XASSET_SCRIPTPARSETREE]);
            int32_t poolSize = game.ReadMemory<int32_t>(game[offset::XASSET_SCRIPTPARSETREE + 0x14]);

            if (!poolPtr || poolSize <= 0) {
                return L"Can't read SPT pool data, this game version is probably not supported.";
            }

            LOG_INFO("Reading pools...");
            T8ScriptParseTreeEntry* replaced = nullptr;
            uintptr_t replacedPtr = 0;
            T8ScriptParseTreeEntry* hooked = nullptr;
            uintptr_t hookedPtr = 0;

            auto pool = std::make_unique<T8ScriptParseTreeEntry[]>(poolSize);

            if (!game.ReadMemory(&pool[0], poolPtr, poolSize * sizeof(pool[0]))) {
                return L"Can't read pools";
            }

            for (size_t i = 0; i < poolSize; i++) {
                auto& p = pool[i];

                if (p.name == serverReplaceScript) {
                    replaced = &pool[i];
                    replacedPtr = poolPtr + i * sizeof(pool[0]);
                    if (hookScript) break;
                }
                else if (p.name == hookScript) {
                    hooked = &pool[i];
                    hookedPtr = poolPtr + i * sizeof(pool[0]);
                    if (replacedPtr) break;
                }
            }

            if (!hooked) {
                return std::format(L"Can't find hook script '{}' ({:x}), are you sure to be in the right mode? ({} checked)", cfg.injectionPoint.c_str(), hookScript, poolSize);
            }

            if (!replaced) {
                return L"Can't find replaced script.";
            }

            T8GSCOBJ hookedHeader{};

            if (!game.ReadMemory(&hookedHeader, hooked->buffer, sizeof(hookedHeader))) {
                return L"Can't read hook script buffer.";
            }


            auto includes = std::make_unique<uint64_t[]>(hookedHeader.include_count);

            if (!game.ReadMemory(&includes[0], hooked->buffer + hookedHeader.include_offset, sizeof(includes[0]) * hookedHeader.include_count)) {
                return L"Can't read hook script includes.";
            }

            bool find = false;
            for (size_t i = 0; i < hookedHeader.include_count; i++) {
                if (includes[i] == serverReplaceScript) {
                    find = true;
                    break;
                }
            }

            if (!find) {
                // can't find the replaced script inside the hook script, we need to add it
                if (!game.WriteMemory(hooked->buffer + hookedHeader.include_offset + sizeof(includes[0]) * hookedHeader.include_count, &serverReplaceScript, sizeof(serverReplaceScript))) {
                    return L"Can't add include to hook script.";
                }

                hookedHeader.include_count++;

                if (!game.WriteMemory(hooked->buffer, &hookedHeader, sizeof(hooked->buffer))) {
                    return L"Can't update hook script includes count.";
                }
            }

            // inject DLL and activate right parts
            if (cfg.actsDll) {

                wchar_t szFileName[MAX_PATH];
                GetModuleFileName(NULL, szFileName, MAX_PATH);

                std::filesystem::path progpath = std::filesystem::absolute(szFileName).parent_path();
                auto ppstr = progpath.string();
                // ACTS DLL injection

                auto dllPath = progpath / "acts-bo4-dll.dll";
                auto dllPathStr = dllPath.string();
                LOG_INFO("ACTS DLL Injection asked, injecting {}", dllPathStr);

                if (!game.LoadDll(dllPathStr.c_str())) {
                    return L"Can't inject DLL, try to run the injector as administrator";
                }

                LOG_INFO("Injected.");

                auto& mod = game["acts-bo4-dll.dll"];

                if (!mod) {
                    return L"Can't inject ACTS DLL";
                }

                auto& rSyncCLI = mod["SyncCLI"];

                if (!rSyncCLI) {
                    return L"Can't find acts-bo4-dll.dll@SyncCLI";
                }

                cli::clisync::CliSyncData data{ };
                strncpy_s(data.execDir, sizeof(data.execDir), ppstr.c_str(), ppstr.length());
                strncpy_s(data.workDir, sizeof(data.workDir), ppstr.c_str(), ppstr.length());

                LOG_INFO("Syncing ACTS data");

                auto rsync = game.AllocateMemory(sizeof(data));

                if (!rsync || !game.WriteMemory(rsync, &data, sizeof(data))) {
                    game.FreeMemory(rsync, sizeof(data));
                    return L"Can't write ACTS sync data";
                }

                if (!memapi::Call(game, rSyncCLI, rsync)) {
                    game.FreeMemory(rsync, sizeof(data));
                    return L"Can't call ACTS sync data";
                }

                game.FreeMemory(rsync, sizeof(data));
            }

            if (cfg.seriousDll || needBuiltins) { // || gsicInfo.detours.size() // no need because it is useless without the builtins?
                wchar_t szFileName[MAX_PATH];
                GetModuleFileName(NULL, szFileName, MAX_PATH);
                std::filesystem::path progpath = std::filesystem::absolute(szFileName).parent_path();
                auto dllPath = progpath / "t8cinternal.dll";
                auto dllPathStr = dllPath.string();

                LOG_INFO("Serious DLL Injection asked, injecting {}", dllPathStr);

                if (!game.LoadDll(dllPathStr.c_str())) {
                    return L"Can't inject DLL, try to run the injector as administrator";
                }
                LOG_INFO("Injected.");

                auto& mod = game["t8cinternal.dll"];

                if (!mod) {
                    return L"Can't inject T8Compiler DLL, did you add the t8cinternal.dll file to the acts-ui installation?";
                }

                auto& T8Dll_LazyLinkInit = mod["T8Dll_LazyLinkInit"];

                if (!T8Dll_LazyLinkInit) {
                    return L"Can't find t8cinternal.dll@T8Dll_LazyLinkInit, are you using the latest T8Compiler dll?";
                }

                // Init lazylinks
                LOG_INFO("Init lazylinks...");
                if (!memapi::Call(game, T8Dll_LazyLinkInit)) {
                    return L"Can't call t8cinternal.dll@T8Dll_LazyLinkInit";
                }

                if (needBuiltins) {
                    auto& T8Dll_BuiltinsInit = mod["T8Dll_BuiltinsInit"];

                    if (!T8Dll_BuiltinsInit) {
                        return L"Can't find t8cinternal.dll@T8Dll_BuiltinsInit, are you using the latest T8Compiler dll?";
                    }

                    // Init builtins
                    LOG_INFO("Init builtins...");
                    if (!memapi::Call(game, T8Dll_BuiltinsInit)) {
                        return L"Can't call t8cinternal.dll@T8Dll_BuiltinsInit";
                    }

                    if (gsicInfo.detours.size()) {
                        auto& T8Dll_DetoursInit = mod["T8Dll_DetoursInit"];

                        if (!T8Dll_DetoursInit) {
                            return L"Can't find t8cinternal.dll@T8Dll_DetoursInit, are you using the latest T8Compiler dll?";
                        }

                        // Init detour hooks
                        LOG_INFO("Init detours...");
                        if (!memapi::Call(game, T8Dll_DetoursInit)) {
                            return L"Can't call t8cinternal.dll@T8Dll_DetoursInit";
                        }
                    }
                }
            }

            // replace the script itself
            ClearPreviousScript(game);

            T8GSCOBJ replaceHeader{};
            if (!game.ReadMemory(&replaceHeader, replaced->buffer, sizeof(replaceHeader))) {
                return L"Can't read replaced script header";
            }

            // overwrite the injected header
            obj->crc = replaceHeader.crc;
            obj->name = replaceHeader.name;

            // inject script + detours
            auto scriptAllocationSize = size;
            auto scriptAllocation = game.AllocateMemory(scriptAllocationSize);

            if (!scriptAllocation) {
                return L"Can't allocate script";
            }

            if (!game.WriteMemory(scriptAllocation, reinterpret_cast<const void*>(obj), size)) {
                auto err = GetLastError();
                game.FreeMemory(scriptAllocation, scriptAllocationSize);
                return std::format(L"Can't write script of size {:x}@{:x} : {:x}", scriptAllocation, size, err);
            }

            injectedScript.prePatchEntry = *replaced;
            injectedScript.prePatchEntryLocation = replacedPtr;
            replaced->buffer = scriptAllocation;
            replaced->size = (uint32_t)scriptAllocationSize;

            if (!game.WriteMemory(replacedPtr, replaced, sizeof(*replaced))) {
                game.FreeMemory(scriptAllocation, scriptAllocationSize);
                return L"Can't write SPT entry";
            }

            injectedScript.injected = true;
            injectedScript.lastInjectionPID = game.m_pid;
            injectedScript.injectedScript = scriptAllocation;
            injectedScript.injectedScriptSize = scriptAllocationSize;

            if (gsicInfo.detours.size()) {
                auto& registerDetours = game["t8cinternal.dll"]["RegisterDetours"];

                if (!registerDetours) {
                    ClearPreviousScript(game);
                    return L"Can't find t8cinternal.dll@RegisterDetours function";
                }

                injectedScript.detoursInjected = true;

                auto detourDataSize = gsicInfo.detours.size() * detourinfosize;
                auto detourData = std::make_unique<byte[]>(detourDataSize);

                for (size_t i = 0; i < gsicInfo.detours.size(); i++) {
                    auto& dtData = *reinterpret_cast<ReadScriptDetour*>(&detourData[i]);
                    auto& dt = gsicInfo.detours[i];

                    dtData.FixupName = dt->name;
                    dtData.ReplaceNamespace = dt->replaceNamespace;
                    dtData.ReplaceFunction = dt->replaceFunction;
                    dtData.FixupOffset = dt->fixupOffset;
                    dtData.FixupSize = dt->fixupSize;
                }

                auto detourInfo = game.AllocateMemory(detourDataSize);

                if (!detourInfo || !game.WriteMemory(detourInfo, &detourData[0], detourDataSize)) {
                    game.FreeMemory(scriptAllocation, scriptAllocationSize);
                    return L"Can't allocate and write detour info";
                }

                // EXPORT bool RegisterDetours(void* DetourData, int NumDetours, int64_t scriptOffset, int32_t inst)
                auto ret = memapi::Call<uintptr_t, int, int64_t, int32_t>(game, registerDetours, detourInfo, (int) gsicInfo.detours.size(), scriptAllocation, scriptinstance::SI_SERVER);

                game.FreeMemory(detourInfo, detourDataSize);

                if (!ret) {
                    game.FreeMemory(scriptAllocation, scriptAllocationSize);
                    return L"Can't register detour info";
                }
            }
            Button_Enable(revertButton, TRUE);

            return L"Injected";
        }
    };

    ActsWindow actsWindow{};
}

inline int CenterLocation(int x, int w) {
    return x + w / 2;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
    return MainActs(0, nullptr, hInstance, nShowCmd);
    /*
#ifndef CI
    WCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);

    std::filesystem::path progpath = utils::GetProgDir();
    auto ppstr = (progpath / "atian-ui.log").string();
    core::logs::setfile(ppstr.c_str());
#endif
    LOG_INFO("Init ACTS-UI");
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (FAILED(hr)) {
        MessageBox(NULL, L"Can't init con.", L"ACTS", MB_OK | MB_ICONERROR);
        return -1;
    }

    actsWindow.cfg.Sync();

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Atian Tools Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, L"acts_logo");

    RegisterClass(&wc);
    auto title = std::format(L"Atian Tools ({}) - Black Ops 4 GSC injector", actsinfo::VERSIONW);
    
    // Create the window.
    actsWindow.hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, AUI_WIDTH, AUI_HEIGHT,

        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (actsWindow.hwnd == NULL) {
        return -1;
    }

    SendMessage(actsWindow.hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)FALSE);
    

    actsWindow.filePathEdit = CreateWindowExW(
        0,
        L"EDIT",
        L"",
        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        
        AUI_WIDTH / 2 - 200,
        AUI_HEIGHT / 2 - 136,

        320, 24,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.filePathEditLabel = CreateWindowEx(
        0,
        L"STATIC",
        L"GSC File : ",
        SS_RIGHT | WS_CHILD | WS_VISIBLE,
        0,
        AUI_HEIGHT / 2 - 136,
        AUI_WIDTH / 2 - 200, 24,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.filePathButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"...",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        AUI_WIDTH / 2 + 120,
        AUI_HEIGHT / 2 - 136,
        80, 24,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );
    actsWindow.injectHookCombo = CreateWindowEx(
        0,
        WC_COMBOBOX,
        L"",
        CBS_SIMPLE | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
        AUI_WIDTH / 2 - 200,
        AUI_HEIGHT / 2 - 96,
        400, 88,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.injectHookComboLabel = CreateWindowEx(
        0,
        L"STATIC",
        L"Inject hook : ",
        SS_RIGHT | WS_CHILD | WS_VISIBLE,
        0,
        AUI_HEIGHT / 2 - 96,
        AUI_WIDTH / 2 - 200, 24,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.injectDLLButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"ACTS DLL",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_AUTOCHECKBOX,
        AUI_WIDTH / 2 - 200,
        AUI_HEIGHT / 2,
        150, 40,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.injectSeriousDLLButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"T8Compiler DLL (Need install)",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_AUTOCHECKBOX,
        AUI_WIDTH / 2 - 50,
        AUI_HEIGHT / 2,
        250, 40,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.injectButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Inject",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
        AUI_WIDTH / 2 - 200,
        AUI_HEIGHT / 2 + 48,
        400, 40,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.revertButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Revert",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT | WS_DISABLED,
        AUI_WIDTH / 2 - 200,
        AUI_HEIGHT / 2 + 96,
        400, 40,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    actsWindow.statusMessageText = CreateWindowEx(
        0,
        L"STATIC",
        L"",
        SS_CENTER | WS_CHILD | WS_VISIBLE,
        0,
        AUI_HEIGHT / 2 + 144,
        AUI_WIDTH, 88,
        actsWindow.hwnd,
        NULL,
        hInstance,
        NULL
    );

    wchar_t injections[3][64] = {
        L"scripts\\zm_common\\load.gsc",
        L"scripts\\mp_common\\bb.gsc",
        L"scripts\\core_common\\load_shared.gsc"
    };

    wchar_t buff[sizeof(*injections)];

    memset(&buff, 0, sizeof(buff));
    auto comboElements = sizeof(injections) / sizeof(*injections);

    for (size_t k = 0; k < comboElements; k++) {
        wcscpy_s(buff, sizeof(buff) / sizeof(*buff), (const wchar_t*)injections[k]);

        SendMessage(actsWindow.injectHookCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)buff);
    }
    
    if (
        actsWindow.injectDLLButton == NULL
        || actsWindow.injectButton == NULL
        || actsWindow.filePathEdit == NULL
        || actsWindow.injectSeriousDLLButton == NULL
        || actsWindow.injectHookCombo == NULL
        || actsWindow.filePathButton == NULL
        || actsWindow.statusMessageText == NULL
        || actsWindow.filePathEditLabel == NULL
        || actsWindow.injectHookComboLabel == NULL
        || actsWindow.revertButton == NULL
        ) {
        return -1;
    }

    SendMessage(actsWindow.filePathEdit, EM_SETLIMITTEXT, (WPARAM)0, (LPARAM)0);

    if (actsWindow.cfg.injectionPoint.size()) {
        SendMessage(actsWindow.injectHookCombo, WM_SETTEXT, (WPARAM)0, (LPARAM)actsWindow.cfg.injectionPoint.data());
    }
    else {
        // no cfg, use default
        SendMessage(actsWindow.injectHookCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    }

    if (actsWindow.cfg.name.size()) {
        SendMessage(actsWindow.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)actsWindow.cfg.name.data());
    }
    
    Button_SetCheck(actsWindow.injectDLLButton, actsWindow.cfg.actsDll);
    Button_SetCheck(actsWindow.injectSeriousDLLButton, actsWindow.cfg.seriousDll);

    actsWindow.buttonEvent[actsWindow.filePathButton] = []() {
        // Open file

        OPENFILENAME ofn;       // common dialog box structure
        TCHAR szFile[MAX_PATH + 1] = { 0 };       // if using Tchar macros

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = actsWindow.hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Compiled GSC file (.gscc, .gsic, .gscobj)\0*.gscc;*.gsic;*.gscobj\0All\0*.*\0";
        ofn.lpstrTitle = L"Open GSC file";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE) {
            actsWindow.cfg.name = ofn.lpstrFile;
            SendMessage(actsWindow.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)ofn.lpstrFile);
        }
    };

    actsWindow.buttonEvent[actsWindow.injectButton] = []() {
        // sync config
        actsWindow.SaveCfg();

        std::wstring res = actsWindow.InjectScript();

        Static_SetText(actsWindow.statusMessageText, res.data());
    };

    ShowWindow(actsWindow.hwnd, nShowCmd);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();

    return 0;
    */
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (wParam == BN_CLICKED) {
            auto it = actsWindow.buttonEvent.find((HWND)lParam);
            if (it != actsWindow.buttonEvent.end()) {
                it->second();
                return 0;
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CTLCOLORSTATIC: {
        if (lParam == (LPARAM)actsWindow.statusMessageText
            || lParam == (LPARAM)actsWindow.filePathEditLabel
            || lParam == (LPARAM)actsWindow.injectHookComboLabel) {
            return 0;
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
