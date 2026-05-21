#include <ui_includes.hpp>
#include <tools_ui.hpp>

namespace tools::ui {
	std::vector<AbstractUITool*>& GetTools() {
		static std::vector<AbstractUITool*> tools{};
		return tools;
	}
}