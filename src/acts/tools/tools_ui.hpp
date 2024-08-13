#pragma once
#include <windowsx.h>
#include <CommCtrl.h>

namespace tool::ui {
	constexpr const wchar_t* CLASS_NAME = L"Atian Tools Class";
	constexpr const wchar_t* PAGE_CLASS_NAME = L"Atian Tools Page Class";
	constexpr int AUI_WIDTH = 800;
	constexpr int AUI_HEIGHT = 600;

	typedef int(*toolfunctionui)(HWND window, HINSTANCE hInstance);
	typedef void(*toolfunctionuiresize)(int width, int height);

	class tooluifunctiondata {
	public:
		const char* m_id;
		const wchar_t* m_description;
		toolfunctionui m_func;
		toolfunctionuiresize m_resize;
		WNDPROC m_msg;
		int tabId{};

		tooluifunctiondata(const char* id, const wchar_t* description, toolfunctionui registerFunc, WNDPROC msg, toolfunctionuiresize resize);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::ui::tooluifunctiondata*>& tools();

	struct ActsWindow {
		HINSTANCE hinst;
		HWND hwnd;
		HWND hwndLogTab;
		HWND hwndTab;
		HWND hwndDisplay{};
		int width{ AUI_WIDTH }, height{ AUI_HEIGHT };

		std::vector<tooluifunctiondata*> pages{};

		void UpdateWindowName();
		void SetTitleFont(HWND hwnd);
		void RelocateDisplay(int x, int y);
		void SetLogMessage(const std::wstring& str);
		void LoadPage();
	};

	ActsWindow& window();
	std::wstring GetWindowTextVal(HWND hwnd);
}
#define ADD_TOOL_UI(id, desc, function, msg, resize) static tool::ui::tooluifunctiondata __tooluifunctiondata_##function(id, desc, function, msg, resize)