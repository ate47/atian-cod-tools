#include <includes.hpp>
#include "tools_ui.hpp"
#include "tools_nui.hpp"

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

	std::wstring GetActsDesc(bool nui) {
		std::filesystem::path dir = utils::GetProgDir();
		return std::format(
			L"-- Atian Tools {} \n\r"
			"Version: {} (0x{:x})"
#ifdef DEBUG
			" (DEBUG)"
#endif
#ifdef CI_BUILD
			" (CI)"
#else
			" (DEV)"
#endif
			"\n\r"
			"UI Tools: {} \n\r"
			"NUI Tools: {} \n\r"
			"CLI Tools: {} \n\r"
			"Hash(es) loaded: {} \n\r"
			"Path: {} \n\r"
			"\n\r"
			"-- Games \n\r"
			"Black Ops 4 loaded: {} \n\r"
			"Black Ops Cold War loaded: {} \n\r"
			"Cordycep loaded: {} \n\r"
			,
			nui ? L"NUI" : L"UI",
			core::actsinfo::VERSIONW,
			core::actsinfo::VERSION_ID,
			tools().size(),
			tool::nui::tools().size(),
			tool::tools().size(),
			hashutils::GetMap().size(),
			utils::StrToWStr(dir.string()),
			!!Process{ L"BlackOps4.exe" },
			!!Process{ L"BlackOpsColdWar.exe" },
			!!Process{ L"Cordycep.CLI.exe" }
		);
	}
}
namespace {
	struct {
		HWND mainLabel;
	} toolInfo;
	int RenderActs(HWND window, HINSTANCE hInstance) {
		std::wstring text = tool::ui::GetActsDesc();
		toolInfo.mainLabel = CreateWindowExW(
			0,
			L"STATIC",
			text.data(),
			SS_LEFT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		return 0;
	}
	LRESULT UpdateActs(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_CTLCOLORSTATIC && (lParam == (LPARAM)toolInfo.mainLabel)) {
			return 0;
		}
		return 1;
	}
	void ResizeActs(int width, int height) {
		SetWindowPos(toolInfo.mainLabel, NULL, 8, 8, width - 8, height - 8, SWP_SHOWWINDOW);
	}
}
ADD_TOOL_UI(acts, L"Atian Tools", RenderActs, UpdateActs, ResizeActs);