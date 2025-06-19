#include <includes.hpp>
#include <tools/utils/ps4_process.hpp>
#include <tools/tools_nui.hpp>
#include <core/config.hpp>

namespace {
	void ps4_tools() {
		tool::nui::NuiUseDefaultWindow win{};

        static struct {
            std::unique_ptr<libdebug::PS4DBG> ps4{ nullptr }; 
            std::vector<std::shared_ptr<libdebug::Process>> processes;
            int32_t pid;
            libdebug::ProcessInfo pinfo;
            char ps4In[0x20]{ 0 };
            char msgIn[0x100]{ 0 };
            std::string notif{};
        } data;

        utils::CloseEnd notifEnd{ [] {
            if (!data.notif.empty()) {
                ImGui::Separator();

                ImGui::Text("%s", data.notif.data());
            }
        } };

        bool c = false;
        static std::once_flag of;
        std::call_once(of, [&c] {
            std::string injPs4 = core::config::GetString("ui.ps4.ipd");
            std::string injMsg = core::config::GetString("ui.ps4.msg");

            snprintf(data.ps4In, sizeof(data.ps4In), "%s", injPs4.data());
            snprintf(data.msgIn, sizeof(data.msgIn), "%s", injMsg.data());
            c = true;
        });




        ImGui::SeparatorText("PlayStation 4 Debug");
        ImGui::Text("(The ps4debug payload should be loaded)");

        ImGui::Spacing();

        if (ImGui::InputText("PS4 IP", data.ps4In, sizeof(data.ps4In))) {
            core::config::SetString("ui.ps4.ipd", data.ps4In);
            c = true;
        }



        if (ImGui::Button(data.ps4 ? "Reload console" : "Load console")) {
            try {
                data.ps4 = nullptr; // cleanup
                data.ps4 = std::make_unique<libdebug::PS4DBG>(data.ps4In);
                data.ps4->Connect();

                libdebug::ProcessList lst{ data.ps4->GetProcessList() };
                data.processes = lst.processes;
                data.pid = 0;
                data.notif = "Console loaded";
            }
            catch (std::runtime_error& err) {
                data.notif = std::format("Can't load console: {}", err.what());
                data.ps4 = nullptr;
            }
        }

        if (!data.ps4) {
            return;
        }


        if (ImGui::InputText("Text", data.msgIn, sizeof(data.msgIn))) {
            core::config::SetString("ui.ps4.msg", data.msgIn);
            c = true;
        }

        if (ImGui::Button("Send message")) {
            if (data.msgIn[0]) {
                data.ps4->Notify(utils::ps4::PS4N_MESSAGE, data.msgIn);
            }
        }


        if (ImGui::BeginListBox("Processes")) {
            for (auto& proc : data.processes) {
                
                if (ImGui::Button(proc->name.data())) {
                    data.pid = proc->pid;

                    try {
                        data.pinfo = data.ps4->GetProcessInfo(data.pid);
                    }
                    catch (std::runtime_error& err) {
                        data.notif = std::format("Get process info for {}[pid:{}]: {}", proc->name, proc->pid, err.what());
                        data.pid = 0;
                    }
                }
            }

            ImGui::EndListBox();
        }

        if (!data.pid) {
            return;
        }
        ImGui::Text("pid: %d", data.pid);
        ImGui::Text("path: %s", data.pinfo.path);
        ImGui::Text("contentid: %s", data.pinfo.contentid);
        ImGui::Text("titleid: %s", data.pinfo.titleid);

	}
	ADD_TOOL_NUI(ps4_tools, "PlayStation 4 Debug", ps4_tools);
}