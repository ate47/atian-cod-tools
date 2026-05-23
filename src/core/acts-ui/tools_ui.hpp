#pragma once
#include <QAction>
#include <widgets/common/UI3MdiArea.h>

namespace tools::ui {
	class AbstractUITool {
	public:
		char id[64];
		const char* path;
		const char* name;
		const char* extensions;
		bool allowDupe;
		bool needsInitialization;
		QAction* action{ nullptr };

		AbstractUITool(const char* id, const char* name, const char* path, const char* extensions = nullptr, bool allowDupe = false, bool needsInitialization = false)
			: path(path), name(name), extensions(extensions), allowDupe(allowDupe), needsInitialization(needsInitialization) {
			SetId(id);
		}

		virtual void Activate() = 0;
		virtual void OpenFile(const QString& path) = 0;
		virtual bool SupportFileOpen() = 0;
	private:
		void SetId(const char* id);
	};

	template<typename T>
	concept WidgetHasLoadFile = requires(T t, const QString & path) {
		{ t.LoadFile(path) };
	};

	template<typename WidgetType>
	class UITool : public AbstractUITool {
	public:
		UITool(const char* id, const char* name, const char* path,  const char* extensions = nullptr, bool allowDupe = false, bool needsInitialization = false)
			: AbstractUITool(id, name, path, extensions, allowDupe, needsInitialization) {}

		void Activate() override {
			GetMainArea()->LoadToolUi<WidgetType>(allowDupe);
		}

		void OpenFile(const QString& path) override {
			if constexpr (WidgetHasLoadFile<WidgetType>) {
				WidgetType* w{ GetMainArea()->LoadToolUi<WidgetType>(allowDupe) };
				w->LoadFile(path);
			}
			else {
				LOG_WARNING("UI Tool {} does not support opening files!", name);
			}
		}

		bool SupportFileOpen() override {
			if constexpr (WidgetHasLoadFile<WidgetType>) {
				return true;
			}
			else {
				return false;
			}
		}
	};

	AbstractUITool* FindTool(const char* id);

	void OpenMenu(const char* id, const char* file = nullptr);

	std::vector<AbstractUITool*>& GetTools();
}

// widgetType, name, path, extensions=nullptr, allowDupe=false, needsInitialization=false
#define ADD_UI_TOOL(WidgetType, ...) \
	static utils::ArrayAdder<tools::ui::UITool<WidgetType>, tools::ui::AbstractUITool> impl{ tools::ui::GetTools(), #WidgetType, __VA_ARGS__ }