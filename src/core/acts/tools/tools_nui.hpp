#pragma once
#include <gl/GL.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "tools/nui/nui_renderer.hpp"

namespace tool::nui {
	constexpr float PI = 3.14159265358f;
	constexpr float PI2 = PI * 2;
	typedef void(*toolfunctionui)();

	class tooluifunctiondata {
	public:
		const char* m_id;
		const char* m_description;
		char m_descriptionSmall[3];
		bool m_issetup{};
		bool m_open{};
		bool m_devTool;
		toolfunctionui m_func;
		toolfunctionui m_setup;

		tooluifunctiondata(const char* id, const char* description, toolfunctionui m_setup, toolfunctionui registerFunc, bool devTool);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::nui::tooluifunctiondata*>& tools();

	void OpenNuiWindow();

	int GetNUIScreenWidth();
	int GetNUIScreenHeight();
	ImVec2 GetDefaultWindowStart();
	void BeginDefaultWindow();
	void EndDefaultWindow();
	void DisableNextBackground();
	void SaveNextConfig();
	bool OpenFile(const wchar_t* title, const wchar_t* patterns, wchar_t* outFile, size_t outFileLen, DWORD flags = OFN_PATHMUSTEXIST);
	bool OpenFile(const wchar_t* title, const wchar_t* patterns, char* outFile, size_t outFileLen, DWORD flags = OFN_PATHMUSTEXIST);

	class ActsConfig {
		std::vector<float> floor{};
		size_t len{};
		long long lastTime{};
	public:
		int fps{ 120 };
		float fov{ 90 };
		int nextLen{ 100 };
		float farLen{ 20 };
		float color[3]{ 1, 1, 1 };
		float colorGround[3]{ 0.2f, 0.2f, 0.2f };
		float colorBackground[3]{ 0.2f, 0.2f, 0.2f };
		bool triangles{};
		bool enabled{ true };
		bool loadHashStartup{};
		bool open{ true };
		bool showDevTools{};

		bool nextDisabled{};
		int bgAlpha{ 85 };
		float partialTick{};
		ActsConfig();
		void LoadCfg();
		float& Floor(size_t i, size_t j);
		void Gen(bool force = true);
		void RenderDot(float x, float y, float z) const;
		void PreLoop();
		void WaitFPS();
		void Render();
	};

	class ActsNUI {
	public:
		GLFWwindow* wnd;
		static std::mutex nuiMtx;
		static ActsNUI* nui;
		ActsConfig bg{};
		int width;
		int height;
		tool::nui::renderer::NuiFont cascadiaFont{};
		bool saveNext{};
		ActsNUI();
		~ActsNUI();

		void SetWindowName(const char* name);
		bool ShouldClose();
		void Loop();
		void Render();
		void Resize(int _width, int _height);
	};

	class NuiUseDefaultWindow {
	public:
		NuiUseDefaultWindow() {
			BeginDefaultWindow();
		}
		~NuiUseDefaultWindow() {
			EndDefaultWindow();
		}
	};
}

#define ADD_TOOL_NUI(id, desc, function) static tool::nui::tooluifunctiondata __toolnuifunctiondata_##id(#id, desc, nullptr, function, false)
#define ADD_TOOL_NUI2(id, desc, setup, function) static tool::nui::tooluifunctiondata __toolnuifunctiondata_##id(#id, desc, setup, function, false)
#define ADD_TOOL_NUI_DEV(id, desc, function) static tool::nui::tooluifunctiondata __toolnuifunctiondata_##id(#id, desc, nullptr, function, true)
#define ADD_TOOL_NUI2_DEV(id, desc, setup, function) static tool::nui::tooluifunctiondata __toolnuifunctiondata_##id(#id, desc, setup, function, true)