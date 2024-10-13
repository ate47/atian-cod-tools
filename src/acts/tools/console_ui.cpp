#include <includes.hpp>
#include <BS_thread_pool.hpp>
#include <core/config.hpp>
#include <core/async.hpp>
#include <tools/tools_nui.hpp>
#include "console_ui.hpp"

namespace tool::console_ui {
	namespace {
		std::ostringstream consoleStream{};
		char consoleInput[0x100]{};

		struct PoolInfo {
			BS::thread_pool pool{ 1 };
			bool running{};
			std::mutex mtx{};
		};

		PoolInfo& GetConsoleThreadPool() {
			static PoolInfo pool{};
			return pool;
		}

		void console_ui_setup() {
			// load cfg
			std::string inp = core::config::GetString("nui.console.input");
			snprintf(consoleInput, sizeof(consoleInput), "%s", inp.data());
			tool::nui::SaveNextConfig();

			// init pool
			GetConsoleThreadPool();
			core::async::SetAsync(core::async::AT_LOGS);
		}

		void console_ui() {
			static std::string consoleText{};
			consoleText = consoleStream.str();

			bool run;
			PoolInfo& pool{ GetConsoleThreadPool() };
			{
				std::lock_guard lg{ pool.mtx };
				run = pool.running;
			}

			ImVec2 loc{ tool::nui::GetDefaultWindowStart() };
			tool::nui::BeginDefaultWindow();

			ImVec2 winSize = ImGui::GetWindowSize();

			ImVec2 size{ winSize.x, winSize.y - ImGui::GetFontSize() * 2 - 20 };
			ImGui::InputTextMultiline("LogsB", consoleText.data(), consoleText.length(), size, ImGuiInputTextFlags_ReadOnly);
			if (ImGui::InputText("Input", consoleInput, sizeof(consoleInput), run ? ImGuiInputTextFlags_ReadOnly : 0)) {
				core::config::SetString("nui.console.input", consoleInput);
				tool::nui::SaveNextConfig();
			}

			if (!run && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
				pool.pool.detach_task([] {
					int argc{};
					std::string in{ consoleInput };
					consoleInput[0] = 0;
					const char** argv = tool::ReadParams(in, argc);

					if (argv && argc >= 2) {
						tool::HandleCommand(argc, argv);
					}

					{
						PoolInfo& pool{ GetConsoleThreadPool() };
						std::lock_guard lg{ pool.mtx };
						pool.running = false;
					}
				});
			}
			tool::nui::EndDefaultWindow();
		}
	}
	void AttachConsoleUI() {
		alogs::addoutstream(&consoleStream);
	}


	ADD_TOOL_NUI("zzzz7_console", "Console", console_ui);
}