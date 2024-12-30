#include <includes.hpp>
#include "tools/tools_ui.hpp"
#include "tools/tools_nui.hpp"
#include "tools/cw/cw.hpp"
#include "tools/bo3/bo3.hpp"
#include "tools/bo6/bo6.hpp"
#include "tools/gsc.hpp"
#include "mods/custom_ees.hpp"
#include "tools/utils/ps4_process.hpp"
#include <core/config.hpp>

namespace {
    struct {
        HWND titleLabel{};
        HWND filePathEdit{};
        HWND filePathEditLabel{};
        HWND hookPathEdit{};
        HWND hookPathEditLabel{};
        HWND filePathButton{};
        HWND injectButton{};
        HWND injectEEButton{};
        HWND notificationLabel{};
        std::wstring lastNotif{};
    } info{};

    void SetNotif(const std::string& msg) {
        info.lastNotif = utils::StrToWStr(msg);

        SendMessage(info.notificationLabel, WM_SETTEXT, (WPARAM)0, (LPARAM)info.lastNotif.data());
    }

    const char* InjectCW(const char* file, Process& proc) {
        uintptr_t pool = cw::ScanPool(proc);

        if (!pool) {
            return "Can't find pool";
        }

        auto [entry, ok] = proc.ReadMemoryObject<cw::XAssetPool>(pool + sizeof(cw::XAssetPool) * cw::ASSET_TYPE_SCRIPTPARSETREE);

        if (!ok) {
            return "Can't read SPT pool";
        }

        return nullptr;
    }

    int Render(HWND window, HINSTANCE hInstance) {
        info.titleLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"GSC Injector",
            SS_CENTER | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.notificationLabel = CreateWindowExW(
            0,
            L"STATIC",
            info.lastNotif.c_str(),
            SS_CENTER | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.filePathEdit = CreateWindowExW(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );
        
        info.filePathEditLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"GSC File : ",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );
        info.hookPathEdit = CreateWindowExW(
            0,
            WC_COMBOBOXW,
            L"",
             CBS_HASSTRINGS | CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.hookPathEditLabel = CreateWindowExW(
            0,
            L"STATIC",
            L"Hook : ",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.filePathButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.injectButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"Inject",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.injectEEButton = CreateWindowExW(
            0,
            L"BUTTON",
            L"Patch EEs (Zombies)",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        if (
            info.filePathEdit == NULL
            || info.filePathEditLabel == NULL
            || info.filePathButton == NULL
            || info.injectButton == NULL
            || info.injectEEButton == NULL
            || info.hookPathEdit == NULL
            || info.hookPathEditLabel == NULL
            || info.titleLabel == NULL
            ) {
            return -1;
        }

        SendMessage(info.filePathEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);

        wchar_t injections[3][64] = {
            L"scripts\\zm_common\\load.gsc",
            L"scripts\\mp_common\\bb.gsc",
            L"scripts\\core_common\\load_shared.gsc"
        };

        wchar_t buff[sizeof(*injections)];

        memset(&buff, 0, sizeof(buff));

        for (size_t k = 0; k < ARRAYSIZE(injections); k++) {
            wcscpy_s(buff, ARRAYSIZE(buff), (const wchar_t*)injections[k]);

            SendMessage(info.hookPathEdit, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)buff);
        }

        std::wstring hook = utils::StrToWStr(core::config::GetString("ui.injector.hook", "scripts\\zm_common\\load.gsc"));

        if (hook.size()) {
            SendMessage(info.hookPathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)hook.data());
        }
        else {
            // no cfg, use default
            SendMessage(info.hookPathEdit, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
        }

        std::wstring gscPath = utils::StrToWStr(core::config::GetString("ui.injector.path", ""));
        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)gscPath.c_str());
        return 0;
    }

    LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COMMAND) {
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                if (info.hookPathEdit == (HWND)lParam) {
                    DWORD sel = ComboBox_GetCurSel(info.hookPathEdit);
                    char path[MAX_PATH + 1];

                    if (SUCCEEDED(ComboBox_GetLBText(info.hookPathEdit, sel, path))) {
                        path[ARRAYSIZE(path) - 1] = 0;
                        core::config::SetString("ui.injector.hook", path);
                        core::config::SaveConfig();
                    }
                }
            } else if (HIWORD(wParam) == CBN_EDITUPDATE) {
                if (info.hookPathEdit == (HWND)lParam) {
                    char path[MAX_PATH + 1];

                    if (SUCCEEDED(ComboBox_GetText(info.hookPathEdit, &path[0], ARRAYSIZE(path)))) {
                        core::config::SetString("ui.injector.hook", path);
                        core::config::SaveConfig();
                    }
                }
            } else if (HIWORD(wParam) == EN_CHANGE) {
                if (info.filePathEdit == (HWND)lParam) {

                    char path[MAX_PATH + 1];

                    if (SUCCEEDED(GetWindowText(info.filePathEdit, &path[0], ARRAYSIZE(path)))) {
                        core::config::SetString("ui.injector.path", path);
                        core::config::SaveConfig();
                    }
                } 
            } else if (wParam == BN_CLICKED) {
                if (info.filePathButton == (HWND)lParam) {
                    // Open file

                    OPENFILENAMEW ofn;
                    WCHAR szFile[MAX_PATH + 1] = { 0 };

                    // Initialize OPENFILENAME
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = tool::ui::window().hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = L"Compiled GSC file (.gscc, .gsic, .gscobj)\0*.gscc;*.gsic;*.gscobj\0All\0*.*\0";
                    ofn.lpstrTitle = L"Open GSC file";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileNameW(&ofn) == TRUE) {
                        core::config::SetString("ui.injector.path", utils::WStrToStr(ofn.lpstrFile));
                        core::config::SaveConfig();
                        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)ofn.lpstrFile);
                    }
                    return 1;
                }
                else if (info.injectButton == (HWND)lParam) {
                    std::string file{};

                    std::string filePath = utils::WStrToStr(tool::ui::GetWindowTextVal(info.filePathEdit));
                    std::string hookPath = utils::WStrToStr(tool::ui::GetWindowTextVal(info.hookPathEdit));

                    try {
                        if (!utils::ReadFile(filePath, file)) {
                            SetNotif(std::format("Can't read '{}'", filePath));
                            return TRUE;
                        }

                        if (file.size() >= 4 && !memcmp("GSC", file.data(), 4)) {
                            SetNotif("GSCBIN format not supported");
                            return TRUE;
                        }

                        if (file.size() < 0x20) {
                            SetNotif(std::format("Invalid gsc file '{}'", filePath));
                            return TRUE;
                        }

                        uint64_t magic = *reinterpret_cast<uint64_t*>(file.data());

                        std::string notif{};
                        if (magic == cw::GSC_MAGIC) {
                            // cw gsc

                            Process proc{ L"BlackOpsColdWar.exe" };

                            if (!proc || !proc.Open()) {
                                SetNotif("Can't find Black Ops Cold War");
                                return TRUE;
                            }
                            utils::CloseEnd ce{ [&proc] { proc.Close(); } };

                            cw::InjectScriptCW(proc, filePath.c_str(), hookPath.c_str(), "scripts/core_common/clientids_shared.gsc", notif);
                            SetNotif(notif);

                        }
                        else if (magic == cw::GSC_MAGIC_37) {
                            // cw gsc

                            Process proc{ L"BlackOpsColdWar.exe" }; // base game

                            if (!!proc) {
                                if (proc.Open()) {
                                    utils::CloseEnd ce{ [&proc] { proc.Close(); } };
                                    cw::InjectScriptCW(proc, filePath.c_str(), hookPath.c_str(), "scripts/core_common/clientids_shared.gsc", notif);
                                    SetNotif(notif);
                                }
                                else {
                                    SetNotif("Can't open Black Ops Cold War");
                                    return TRUE;
                                }
                            }
                            else {
                                Process proc2{ L"COD2020.exe" }; // alpha game

                                if (!!proc2) {
                                    if (proc2.Open()) {
                                        utils::CloseEnd ce{ [&proc2] { proc2.Close(); } };
                                        cw::InjectScriptCWAlpha(proc2, filePath.c_str(), hookPath.c_str(), "scripts/core_common/clientids_shared.gsc", notif);
                                        SetNotif(notif);
                                    }
                                    else {
                                        SetNotif("Can't open Black Ops Cold War Alpha");
                                        return TRUE;
                                    }
                                }
                                SetNotif("Can't find Black Ops Cold War");
                                return TRUE;
                            }
                        }
                        else if (magic == bo3::GSC_MAGIC) {
                            // bo3 gsc

                            Process proc{ L"BlackOps3.exe" };

                            if (!proc || !proc.Open()) {
                                SetNotif("Can't find Black Ops 3");
                                return TRUE;
                            }
                            utils::CloseEnd ce{ [&proc] { proc.Close(); } };

                            bo3::InjectScriptBO3(proc, filePath.c_str(), "scripts/shared/duplicaterender_mgr.gsc", notif);
                            SetNotif(notif);
                        }
                        else {
                            tool::gsc::opcode::VmInfo* nfo{};
                            if (tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                                SetNotif(std::format("Injector not implemented for VM: {}", nfo->name));
                            }
                            else {
                                SetNotif(std::format("Invalid magic: 0x{:x}", magic));
                            }
                        }
                    }
                    catch (std::exception& e) {
                        SetNotif(std::format("Exception: {}", e.what()));
                    }
                    return TRUE;
                }
                else if (info.injectEEButton == (HWND)lParam) {
                    try {
                        {
                            Process proc{ L"BlackOpsColdWar.exe" };

                            if (proc.Open()) {

                                std::string notif{};
                                mods::ee::CustomEET9(proc, notif);
                                SetNotif(notif);

                                proc.Close();
                                return TRUE;
                            }
                        }
                        {
                            Process proc{ L"BlackOps4.exe" };

                            if (proc.Open()) {

                                std::string notif{};
                                mods::ee::CustomEET8(proc, notif);
                                SetNotif(notif);

                                proc.Close();
                                return TRUE;
                            }
                        }

                        SetNotif("Can't find game");
                    }
                    catch (std::exception& e) {
                        SetNotif(std::format("Exception: {}", e.what()));
                    }
                    return 1;
                }
            }
        }
        else if (uMsg == WM_CTLCOLORSTATIC) {
            if (lParam == (LPARAM)info.filePathEdit
                || lParam == (LPARAM)info.filePathEditLabel
                || lParam == (LPARAM)info.filePathButton
                || lParam == (LPARAM)info.hookPathEdit
                || lParam == (LPARAM)info.hookPathEditLabel
                || lParam == (LPARAM)info.injectButton
                || lParam == (LPARAM)info.injectEEButton
                || lParam == (LPARAM)info.notificationLabel
                || lParam == (LPARAM)info.titleLabel
                ) {
                return 0; 
            }
        }
		return 1;
	}

	void Resize(int width, int height) {
        SetWindowPos(info.titleLabel, NULL, 0, height / 2 - 160, width, 60, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathEdit, NULL, width / 2 - 300, height / 2 - 92, 520, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathEditLabel, NULL, 0, height / 2 - 92, width / 2 - 300, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathButton, NULL, width / 2 + 221, height / 2 - 92, 80, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.hookPathEdit, NULL, width / 2 - 300, height / 2 - 64, 600, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.hookPathEditLabel, NULL, 0, height / 2 - 64, width / 2 - 300, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.injectButton, NULL, width / 2 + 2, height / 2 - 32, 298, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.injectEEButton, NULL, width / 2 - 300, height / 2 - 32, 298, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.notificationLabel, NULL, 0, height / 2 - 4, width, 48, SWP_SHOWWINDOW);

        tool::ui::window().SetTitleFont(info.titleLabel);
	}

    void gsc_inject() {
        tool::nui::NuiUseDefaultWindow dw{};
        static char gscFileIn[MAX_PATH + 1]{ 0 };
        static char hookIn[MAX_PATH + 1]{ 0 };
        static char ps4In[0x50]{ 0 };
        static std::string notif{};

        static std::once_flag of{};

        bool c = false;
        std::call_once(of, [&c] {
            std::string injGsc = core::config::GetString("ui.injector.path");
            std::string injHook = core::config::GetString("ui.injector.hook", "scripts\\zm_common\\load.gsc");
            std::string injPs4 = core::config::GetString("ui.ps4.ipd");

            snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
            snprintf(hookIn, sizeof(hookIn), "%s", injHook.data());
            snprintf(ps4In, sizeof(ps4In), "%s", injPs4.data());
            c = true;
        });

        ImGui::SeparatorText("GSC Inject");

        if (ImGui::InputText("GSC File", gscFileIn, sizeof(gscFileIn))) {
            core::config::SetString("ui.injector.path", gscFileIn);
            c = true;
        }
        if (ImGui::Button("Open file...")) {
            // Open file

            OPENFILENAMEW ofn;
            WCHAR szFile[MAX_PATH + 1] = { 0 };

            // Initialize OPENFILENAME
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Compiled GSC file (.gscc, .gsic, .gscobj)\0*.gscc;*.gsic;*.gscobj\0All\0*.*\0";
            ofn.lpstrTitle = L"Open GSC file";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn) == TRUE) {
                std::string injGsc = utils::WStrToStr(ofn.lpstrFile);
                core::config::SetString("ui.injector.path", injGsc);
                snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
                c = true;
            }
        }
        ImGui::Spacing();
        ImGui::SeparatorText("GSC Injector (PC)");

        if (ImGui::InputText("Hook", hookIn, sizeof(hookIn))) {
            core::config::SetString("ui.injector.hook", hookIn);
            c = true;
        }
        
        if (ImGui::Button("Inject PC Script")) {
            std::string file{};

            std::string filePath = gscFileIn;
            std::string hookPath = hookIn;

            try {
                if (!utils::ReadFile(filePath, file)) {
                    throw std::runtime_error(std::format("Can't read '{}'", filePath));
                }

                if (file.size() >= 4 && !memcmp("GSC", file.data(), 4)) {
                    throw std::runtime_error("GSCBIN format not supported");
                }

                if (file.size() < 0x20) {
                    throw std::runtime_error(std::format("Invalid gsc file '{}'", filePath));
                }

                uint64_t magic = *reinterpret_cast<uint64_t*>(file.data());

                if (magic == cw::GSC_MAGIC) {
                    // cw gsc

                    Process proc{ L"BlackOpsColdWar.exe" };

                    if (!proc || !proc.Open()) {
                        throw std::runtime_error("Can't find Black Ops Cold War");
                    }
                    utils::CloseEnd ce{ [&proc] { proc.Close(); } };

                    cw::InjectScriptCW(proc, filePath.c_str(), hookPath.c_str(), "scripts/core_common/clientids_shared.gsc", notif);

                }
                else if (magic == bo3::GSC_MAGIC) {
                    // bo3 gsc

                    Process proc{ L"BlackOps3.exe" };

                    if (!proc || !proc.Open()) {
                        throw std::runtime_error("Can't find Black Ops 3");
                    }
                    utils::CloseEnd ce{ [&proc] { proc.Close(); } };

                    bo3::InjectScriptBO3(proc, filePath.c_str(), "scripts/shared/duplicaterender_mgr.gsc", notif);
                }
                else {
                    tool::gsc::opcode::VmInfo* nfo{};
                    if (tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                        notif = (std::format("PC injector not implemented for VM: {}", nfo->name));
                    }
                    else {
                        notif = (std::format("Invalid magic: 0x{:x}", magic));
                    }
                }
            }
            catch (std::exception& e) {
                notif = std::format("Exception: {}", e.what());
            }
        }
#ifdef DEV_PS4_INJECTOR
        ImGui::Spacing();
        ImGui::SeparatorText("GSC Injector (PS4)");

        if (ImGui::InputText("PS4 IP", ps4In, sizeof(ps4In))) {
            core::config::SetString("ui.ps4.ipd", ps4In);
            c = true;
        }


        if (ImGui::Button("Inject PS4 Script")) {
            std::string file{};

            std::string filePath = gscFileIn;
            std::string hookPath = hookIn;

            try {
                if (!utils::ReadFile(filePath, file)) {
                    throw std::runtime_error(std::format("Can't read '{}'", filePath));
                }

                if (file.size() >= 4 && !memcmp("GSC", file.data(), 4)) {
                    throw std::runtime_error("GSCBIN format not supported");
                }

                if (file.size() < 0x20) {
                    throw std::runtime_error(std::format("Invalid gsc file '{}'", filePath));
                }

                uint64_t magic = *reinterpret_cast<uint64_t*>(file.data());

                tool::gsc::opcode::VmInfo* nfo{};
                if (!tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                    notif = (std::format("Invalid magic: 0x{:x}", magic));
                }
                else if (nfo->vm == tool::gsc::opcode::VM::VM_BO6_06) {
                    // bo6 injector

                    try {
                        tool::gsc::GscObj24* script{ (tool::gsc::GscObj24*)file.data() };

                        uint64_t name = script->name;

                        utils::ps4::PS4Process ps4{ ps4In };

                        auto pool = ps4.ReadObject<bo6::DB_AssetPool>(ps4[0x98FEFA0] + sizeof(bo6::DB_AssetPool) * bo6::T10_ASSET_GSCOBJ);


                        LOG_INFO("Pool: {:x}, count: {}/{}, len 0x{:x}", pool->m_entries, pool->m_loadedPoolSize, pool->m_poolSize, pool->m_elementSize);
                        auto objs = ps4.ReadArray<bo6::GscObjEntry>(pool->m_entries, pool->m_loadedPoolSize);

                        size_t i;
                        for (i = 0; i < pool->m_loadedPoolSize; i++) {
                            auto& obj = objs[i];

                            if (obj.name != name) {
                                continue;
                            }

                            if (!obj.buffer) {
                                throw std::runtime_error("Empty buffer");
                            }

                            if (obj.len < file.size()) {
                                throw std::runtime_error(utils::va("Buffer too small, can't remplace %llu < %llu", (size_t)obj.len, file.size()));
                            }

                            auto scriptTarget = ps4.ReadObject<tool::gsc::GscObj24>(obj.buffer);

                            if (scriptTarget->checksum != script->checksum) {
                                notif = ("Find target script, but the checksum doesn't match");
                                return TRUE;
                            }

                            ps4.Write(obj.buffer, file.data(), file.size());

                            notif = ("Script injected");
                            ps4.Notify("Script injected");
                        }
                        if (i == pool->m_loadedPoolSize) {
                            notif = ("Can't find hook script");
                        }
                    }
                    catch (std::exception& e) {
                        notif = (std::format("Exception: {}", e.what()));
                    }
                }
                else {
                    notif = (std::format("PS4 injector not implemented for VM: {}", nfo->name));
                }
            }
            catch (std::exception& e) {
                notif = std::format("Exception: {}", e.what());
            }
        }
#endif

        ImGui::Spacing();
        ImGui::SeparatorText("Utilities");

        if (ImGui::Button("Patch Easter Eggs (Zombies/PC)")) {
            try {
                bool done{};

                if (!done) {
                    Process proc{ L"BlackOpsColdWar.exe" };

                    if (proc.Open()) {

                        mods::ee::CustomEET9(proc, notif);
                        proc.Close();
                        done = true;
                    }
                }
                if (!done) {
                    Process proc{ L"BlackOps4.exe" };

                    if (proc.Open()) {

                        std::string notif{};
                        mods::ee::CustomEET8(proc, notif);

                        proc.Close();
                        done = true;
                    }
                }
                if (!done) {
                    notif = "Can't find game";
                }
            }
            catch (std::exception& e) {
                notif = std::format("Exception: {}", e.what());
            }
        }

        if (ImGui::Button("Print script info")) {
            std::string file{};
            if (!utils::ReadFile(gscFileIn, file)) {
                notif = std::format("Can't read '{}'", gscFileIn);
            }
            else if (file.size() >= 4 && !memcmp("GSC", file.data(), 4)) {
                notif = "GSCBIN file";
            }
            else if (file.size() >= 4 && !memcmp("GSIC", file.data(), 4)) {
                notif = "GSIC file";
            }
            else if (file.size() < 8) {
                notif = "Invalid GSC compiled file";
            }
            else {
                uint64_t magic = *reinterpret_cast<uint64_t*>(file.data());
                tool::gsc::opcode::VmInfo* nfo{};

                if (!tool::gsc::opcode::IsValidVmMagic(magic, nfo)) {
                    notif = (std::format("Invalid magic: 0x{:x}", magic));
                }
                else {
                    notif = std::format("Vm: {} (0x{:x})", nfo->name, nfo->vmMagic);

                    auto* readerBuilder = tool::gsc::GetGscReader(nfo->vmMagic);

                    if (!readerBuilder) {
                        notif += "\nNo GSC handler available";
                    }
                    else {
                        std::shared_ptr<tool::gsc::GSCOBJHandler> handler{ (*readerBuilder)((byte*)file.data(), file.length()) };

                        if (!handler->IsValidHeader(file.length())) {
                            notif += "\nInvalid header";
                        }
                        else {
                            std::ostringstream oss{};
                            oss
                                << "\n"
                                << "// " << hashutils::ExtractTmpScript(handler->GetName()) << " (" << gscFileIn << ")" << " (size: " << file.length() << " Bytes / " << std::hex << "0x" << file.length() << ")\n"
                                << "// magic: 0x" << std::hex << handler->Ref<uint64_t>()
                                ;

                            handler->DumpHeader(oss << "\n", {});
                            notif += oss.str();
                        }
                    }

                }
            }

        }

        if (!notif.empty()) {
            ImGui::Separator();

            ImGui::Text("%s", notif.data());
        }


        if (c) tool::nui::SaveNextConfig();
    }

    ADD_TOOL_UI(gsc_inject, L"GSC Inject", Render, Update, Resize);
    ADD_TOOL_NUI(gsc_inject, "GSC Inject", gsc_inject);
}