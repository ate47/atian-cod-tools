#include <includes.hpp>
#include <tools/sp23/sp23.hpp>
#include <tools/tools_nui.hpp>
#include <core/config.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/ps4/codtool.hpp>

namespace tool::ps4::codtool {
    CodTool codTool;

    std::vector<CodToolHandler*>& GetHandlers() {
        static std::vector<CodToolHandler*> handlers{};
        return handlers;
    }

    CodToolHandler* GetHandler(const char* name) {
        for (CodToolHandler* h : GetHandlers()) {
            if (!std::strcmp(h->name, name)) return h;
        }
        return nullptr;
    }

    void CodTool::Load() {
        std::string injGsc = core::config::GetString("ui.codtool.path");
        std::string injCbuff = core::config::GetString("ui.codtool.cbuf");
        std::string injHandler = core::config::GetString("ui.codtool.handler");
        std::string injPs4 = core::config::GetString("ui.ps4.ipd");

        snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
        snprintf(cbuffIn, sizeof(cbuffIn), "%s", injCbuff.data());
        snprintf(ps4In, sizeof(ps4In), "%s", injPs4.data());

        handler = GetHandler(injHandler.data());
        if (handler) {
            handlername = handler->name;
        }
        else {
            handlername = handlernameDefault;
        }

        loadAfter = true;
    }

    void CodTool::Handle() {
        loadAfter = false;
        ImGui::SeparatorText("Cod PS4 utilitites");

        if (ImGui::InputText("PS4 IP", ps4In, sizeof(ps4In))) {
            core::config::SetString("ui.ps4.ipd", ps4In);
            loadAfter = true;
        }

        CodToolHandler* prev{ handler };

        ImGui::Text("Game");
        if (ImGui::BeginCombo("##handlername", handlername)) {
            bool isdsel = handlernameDefault == handlername;
            if (ImGui::Selectable(handlernameDefault, isdsel)) {
                handlername = handlernameDefault;
                handler = nullptr;
            }
            if (isdsel) {
                ImGui::SetItemDefaultFocus();
            }

            for (CodToolHandler* h : GetHandlers()) {
                bool isSelected = (handlername == h->name);
                if (ImGui::Selectable(h->name, isSelected)) {
                    handlername = h->name;
                    handler = h;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        if (prev != handler && handler) {
            core::config::SetString("ui.codtool.handler", handler->name);
            loadAfter = true;
        }


        if (ImGui::Button(ps4 ? "Reload Console" : "Load Console")) {
            if (ps4) {
                try {
                    ps4 = nullptr;
                }
                catch (...) {}
            }

            try {
                ps4 = std::make_unique<utils::ps4::PS4Process>(ps4In, handler->process);
            }
            catch (std::runtime_error& err) {
                notif = std::format("Can't load console: {}", err.what());
                try {
                    ps4 = nullptr;
                }
                catch (...) {}
            }
        }

        if (!ps4) {
            return;
        }

        if (handler->CBuf) {
            ImGui::Spacing();
            ImGui::SeparatorText("cbuff");

            if (ImGui::InputText("Command", cbuffIn, sizeof(cbuffIn))) {
                core::config::SetString("ui.codutils.cbuf", cbuffIn);
                loadAfter = true;
            }
            if (ImGui::Button("Send command")) {
                try {
                    std::string cmd = cbuffIn;

                    handler->CBuf(&codTool, cmd.c_str());
                    notif = "";
                }
                catch (std::exception& e) {
                    notif = std::format("Exception: {}", e.what());
                }
            }

        }

        if (handler->InjectGSC) {
            ImGui::Spacing();
            ImGui::SeparatorText("GSC injection");

            if (ImGui::InputText("GSC File", gscFileIn, sizeof(gscFileIn))) {
                core::config::SetString("ui.codutils.path", gscFileIn);
                codTool.loadAfter = true;
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
                    core::config::SetString("ui.codutils.path", injGsc);
                    snprintf(gscFileIn, sizeof(gscFileIn), "%s", injGsc.data());
                    loadAfter = true;
                }
            }


            if (ImGui::Button("Inject PS4 Script")) {
                handler->InjectGSC(&codTool, gscFileIn);
            }
        }
    }

    namespace {
        void codps4() {
            tool::nui::NuiUseDefaultWindow dw{};

            static std::once_flag of{};
            std::call_once(of, []{ codTool.Load(); });

            codTool.Handle();

            if (codTool.loadAfter) tool::nui::SaveNextConfig();

            if (!codTool.notif.empty()) {
                ImGui::Separator();

                ImGui::Text("%s", codTool.notif.data());
            }
        }

        ADD_TOOL_NUI_DEV(codps4, "Call of duty PS4", codps4);
    }
}