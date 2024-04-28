#include <includes.hpp>
#include "tools/tools_ui.hpp"
#include "tools/cw/cw.hpp"
#include "tools/gsc.hpp"
#include "mods/custom_ees.hpp"
#include "config.hpp"

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
        info.titleLabel = CreateWindowEx(
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

        info.notificationLabel = CreateWindowEx(
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
        
        info.filePathEditLabel = CreateWindowEx(
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
            WC_COMBOBOX,
            L"",
             CBS_HASSTRINGS | CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            window,
            NULL,
            hInstance,
            NULL
        );

        info.hookPathEditLabel = CreateWindowEx(
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

        info.filePathButton = CreateWindowEx(
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

        info.injectButton = CreateWindowEx(
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

        info.injectEEButton = CreateWindowEx(
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

        std::wstring hook = utils::StrToWStr(acts::config::GetString("ui.injector.hook", "scripts\\zm_common\\load.gsc"));

        if (hook.size()) {
            SendMessage(info.hookPathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)hook.data());
        }
        else {
            // no cfg, use default
            SendMessage(info.hookPathEdit, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
        }

        std::wstring gscPath = utils::StrToWStr(acts::config::GetString("ui.injector.path", ""));
        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)gscPath.c_str());
        return 0;
    }

    LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COMMAND) {
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                if (info.hookPathEdit == (HWND)lParam) {
                    DWORD sel = ComboBox_GetCurSel(info.hookPathEdit);
                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(ComboBox_GetLBText(info.hookPathEdit, sel, path))) {
                        path[ARRAYSIZE(path) - 1] = 0;
                        acts::config::SetString("ui.injector.hook", utils::WStrToStr(path));
                        acts::config::SaveConfig();
                    }
                }
            } else if (HIWORD(wParam) == CBN_EDITUPDATE) {
                if (info.hookPathEdit == (HWND)lParam) {
                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(ComboBox_GetText(info.hookPathEdit, &path[0], ARRAYSIZE(path)))) {
                        acts::config::SetString("ui.injector.hook", utils::WStrToStr(path));
                        acts::config::SaveConfig();
                    }
                }
            } else if (HIWORD(wParam) == EN_CHANGE) {
                if (info.filePathEdit == (HWND)lParam) {

                    wchar_t path[MAX_PATH + 1];

                    if (SUCCEEDED(GetWindowText(info.filePathEdit, &path[0], ARRAYSIZE(path)))) {
                        acts::config::SetString("ui.injector.path", utils::WStrToStr(path));
                        acts::config::SaveConfig();
                    }
                } 
            } else if (wParam == BN_CLICKED) {
                if (info.filePathButton == (HWND)lParam) {
                    // Open file

                    OPENFILENAME ofn;
                    TCHAR szFile[MAX_PATH + 1] = { 0 };

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

                    if (GetOpenFileName(&ofn) == TRUE) {
                        acts::config::SetString("ui.injector.path", utils::WStrToStr(ofn.lpstrFile));
                        acts::config::SaveConfig();
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

                            cw::InjectScriptCW(proc, filePath.c_str(), hookPath.c_str(), "scripts/core_common/clientids_shared.gsc", notif);
                            SetNotif(notif);

                            proc.Close();
                        }
                        else {
                            SetNotif(std::format("Invalid magic: 0x{:x}", magic));
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
	ADD_TOOL_UI("gsc_inject", L"GSC Inject", Render, Update, Resize);
}