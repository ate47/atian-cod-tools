#include <includes.hpp>
#include "tools_ui.hpp"

namespace tool::ui {

	std::map<std::string, tooluifunctiondata*>& tools() {
		static std::map<std::string, tooluifunctiondata*> map{};
		return map;
	}

	tooluifunctiondata::tooluifunctiondata(const char* id, const wchar_t* description, toolfunctionui func, WNDPROC msg, toolfunctionuiresize resize)
		: m_id(id), m_description(description), m_func(func), m_msg(msg), m_resize(resize) {
		if (id) {
			tools()[id] = this;
		}
	}

	bool tooluifunctiondata::operator!() const {
		return m_id == NULL;
	}

	bool tooluifunctiondata::operatorbool() const {
		return m_id != NULL;
	}

}
namespace {
	struct {
		HWND mainLabel;
		HWND versionLabel;
	} toolInfo;
	int RenderActs(HWND window, HINSTANCE hInstance) {
		toolInfo.mainLabel = CreateWindowEx(
			0,
			L"STATIC",
			L"Atian Tools",
			SS_CENTER | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);
		std::wstring version{ std::format(L"Version: {}", actsinfo::VERSIONW) };
		toolInfo.versionLabel = CreateWindowEx(
			0,
			L"STATIC",
			version.data(),
			SS_CENTER | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		return 0;
	}
	LRESULT UpdateActs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_CTLCOLORSTATIC && (lParam == (LPARAM)toolInfo.mainLabel || lParam == (LPARAM)toolInfo.versionLabel)) {
			return 0;
		}
		return 1;
	}
	void ResizeActs(int width, int height) {
		SetWindowPos(toolInfo.mainLabel, NULL, 0, height / 2 - 48, width, 24, SWP_SHOWWINDOW);
		SetWindowPos(toolInfo.versionLabel, NULL, 0, height / 2 - 24, width, 24, SWP_SHOWWINDOW);
	}
}
ADD_TOOL_UI("acts", L"Atian Tools", RenderActs, UpdateActs, ResizeActs);
#ifndef CI_BUILD
namespace {
	struct {
		HWND mainLabel;
	} toolInfoDev;
	int RenderActsDev(HWND window, HINSTANCE hInstance) {


		return 0;
	}
	LRESULT UpdateActsDev(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		return 1;
	}
	void ResizeActsDev(int width, int height) {

	}
}
ADD_TOOL_UI("acts_dev", L"Atian Tools Dev", RenderActsDev, UpdateActsDev, ResizeActsDev);
#endif