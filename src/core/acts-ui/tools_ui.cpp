#include <ui_includes.hpp>
#include <tools_ui.hpp>
#include <QMessageBox>

namespace ui3::tools {
	std::vector<AbstractUITool*>& GetTools() {
		static std::vector<AbstractUITool*> tools{};
		return tools;
	}


	void AbstractUITool::SetId(const char* id) {
		strncpy(this->id, id, sizeof(this->id) - 1);
		this->id[sizeof(this->id) - 1] = '\0';

		std::string_view idView{ this->id };

		if (idView.ends_with("Widget")) {
			this->id[idView.length() - 6] = '\0';
		}
	}

	AbstractUITool* FindTool(const char* id) {
		for (AbstractUITool*& tool : GetTools()) {
			if (strcmp(tool->id, id) == 0) {
				return tool;
			}
		}
		return nullptr;
	}

	void OpenMenu(const char* id, const char* file) {
		AbstractUITool* tool{ FindTool(id) };
		if (!tool) {
			QMessageBox::warning(nullptr, "Error", QString("Can't find UI Tool with id '%1'").arg(id));
			return;
		}

		if (file) {
			tool->OpenFile(file);
			if (!tool->SupportFileOpen()) {
				QMessageBox::warning(nullptr, "Error", QString("UI Tool '%1' does not support opening files").arg(tool->name));
			}
		}
		else {
			tool->Activate();
		}
		

	}
}