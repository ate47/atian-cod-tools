#include <includes.hpp>
#include "tools/tools_ui.hpp"
#include "tools/cw/cw.hpp"

namespace {
    struct {
        HWND filePathEdit;
        HWND filePathEditLabel;
        HWND filePathButton;
        HWND injectButton;
        HWND injectEEButton;
    } info;

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
            L"Enable EEs (Zombies)",
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
            ) {
            return -1;
        }
        return 0;
    }

    LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_COMMAND) {

            if (wParam == BN_CLICKED) {
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
                        //actsWindow.cfg.name = ofn.lpstrFile;
                        SendMessage(info.filePathEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)ofn.lpstrFile);
                    }
                    return 1;
                }
                else if (info.injectButton == (HWND)lParam) {
                    std::string file{};




                    return 1;
                }
                else if (info.injectEEButton == (HWND)lParam) {

                    return 1;
                }
            }
        }
        else if (uMsg == WM_CTLCOLORSTATIC) {
            if (lParam == (LPARAM)info.filePathEdit
                || lParam == (LPARAM)info.filePathEditLabel
                || lParam == (LPARAM)info.filePathButton
                || lParam == (LPARAM)info.injectButton
                || lParam == (LPARAM)info.injectEEButton
                ) {
                return 0; 
            }
        }
		return 1;
	}

	void Resize(int width, int height) {
        SetWindowPos(info.filePathEdit, NULL, width / 2 - 200, height / 2 - 64, 320, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathEditLabel, NULL, 0, height / 2 - 64, width / 2 - 200, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.filePathButton, NULL, width / 2 + 121, height / 2 - 64, 80, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.injectButton, NULL, width / 2 + 2, height / 2 - 36, 198, 24, SWP_SHOWWINDOW);
        SetWindowPos(info.injectEEButton, NULL, width / 2 - 200, height / 2 - 36, 198, 24, SWP_SHOWWINDOW);
            
	}
	ADD_TOOL_UI("gsc_inject", L"GSC Inject", Render, Update, Resize);
}