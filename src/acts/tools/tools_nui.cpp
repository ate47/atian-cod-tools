#include <includes.hpp>
#include <core/config.hpp>
#include <gl/GL.h>
#include <gl/GLU.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>
#include <cmath>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl2.h>
#include "tools_nui.hpp"
#include "tools_ui.hpp"
#include "tools/nui/cascadia.hpp"

namespace tool::nui {

	std::map<std::string, tooluifunctiondata*>& tools() {
		static std::map<std::string, tooluifunctiondata*> map{};
		return map;
	}

	tooluifunctiondata::tooluifunctiondata(const char* id, const char* description, toolfunctionui setup, toolfunctionui func, bool devTool)
		: m_id(id), m_description(description), m_setup(setup), m_func(func), m_devTool(devTool) {
		snprintf(m_descriptionSmall, sizeof(m_descriptionSmall), "%s", description);
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

	namespace {
		std::default_random_engine re;

		void NuiSizeCallBack(GLFWwindow* wnd, int width, int height) {
			ActsNUI::nui->Resize(width, height);
		}
	}

	ActsConfig::ActsConfig() {
	}

	void ActsConfig::LoadCfg() {
		enabled = core::config::GetBool("nui.bg.enabled", enabled);
		showDevTools = core::config::GetBool("nui.bg.showDevTools", showDevTools);
		open = core::config::GetBool("nui.open", open);
		fps = (int)core::config::GetInteger("nui.bg.fps", fps);
		fov = (float)core::config::GetDouble("nui.bg.fov", fov);
		loadHashStartup = core::config::GetBool("nui.loadHashesStartup", loadHashStartup);
		triangles = core::config::GetBool("nui.bg.triangles", triangles);
		nextLen = (int)core::config::GetInteger("nui.bg.spikes", nextLen);
		farLen = (float)core::config::GetDouble("nui.bg.far", farLen);
		bgAlpha = (int)core::config::GetInteger("nui.bg.alpha", bgAlpha);

		color[0] = (float)core::config::GetDouble("nui.bg.colorRed", color[0]);
		color[1] = (float)core::config::GetDouble("nui.bg.colorGreen", color[1]);
		color[2] = (float)core::config::GetDouble("nui.bg.colorBlue", color[2]);

		colorGround[0] = (float)core::config::GetDouble("nui.bg.colorGroundRed", colorGround[0]);
		colorGround[1] = (float)core::config::GetDouble("nui.bg.colorGroundGreen", colorGround[1]);
		colorGround[2] = (float)core::config::GetDouble("nui.bg.colorGroundBlue", colorGround[2]);

		colorBackground[0] = (float)core::config::GetDouble("nui.bg.colorBackgroundRed", colorBackground[0]);
		colorBackground[1] = (float)core::config::GetDouble("nui.bg.colorBackgroundGreen", colorBackground[1]);
		colorBackground[2] = (float)core::config::GetDouble("nui.bg.colorBackgroundBlue", colorBackground[2]);
		Gen();
		if (loadHashStartup) {
			hashutils::ReadDefaultFile();
		}
	}

	float& ActsConfig::Floor(size_t i, size_t j) {
		return floor[i * len + j];
	}

	void ActsConfig::Gen(bool force) {
		if (!enabled) {
			floor.clear();
			return;
		}
		if (len == nextLen && !force) return;
		len = nextLen;
		floor.resize(len * len);

		std::uniform_real_distribution<float> unif(0, 1);

		// full random?
		for (size_t i = 0; i < floor.size(); i++) {
			floor[i] = unif(re);
		}
	}


	void ActsConfig::RenderDot(float x, float y, float z) const {
		float cgr = colorGround[0];
		float cgg = colorGround[1];
		float cgb = colorGround[2];

		float cr = cgr + (color[0] / (1 - cgr) - cgr) * y;
		float cg = cgg + (color[1] / (1 - cgg) - cgg) * y;
		float cb = cgb + (color[2] / (1 - cgb) - cgb) * y;

		glColor3f(cr, cg, cb);
		glVertex3f(x, y, z);
	};

	void ActsConfig::PreLoop() {
		lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		partialTick = 1000.0f / fps;

	}

	void ActsConfig::WaitFPS() {
		long long time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		long long towait = lastTime - time + (1000 / fps);

		if (towait > 0) {
			Sleep((DWORD)towait);
		}
	}

	void ActsConfig::Render() {
		if (nextDisabled) {
			nextDisabled = false;
			return;
		}
		if (!enabled) {
			return;
		}


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		gluPerspective(fov / 2, (GLdouble)ActsNUI::nui->width / ActsNUI::nui->height, 0.1, 200);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		constexpr long long timeFrame = 3600ll * 25;
		double time = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % timeFrame);

		double cameraX = std::cos((time / timeFrame) * PI2);
		double cameraY = std::sin((time / timeFrame) * PI2);

		gluLookAt(
			5 * cameraX, 3, 5 * cameraY,
			0, 0, 0,
			0, 1, 0
		);

		if (triangles) {
			glBegin(GL_TRIANGLES);

			float d = farLen / len;
			for (size_t i = 0; i < len; i++) {
				float x = ((i - (float)len / 2) * d);
				for (size_t j = 0; j < len; j++) {
					float z = ((j - (float)len / 2) * d);

					float y = floor[i * len + j];


					float cgr = colorGround[0];
					float cgg = colorGround[1];
					float cgb = colorGround[2];

					float cr = cgr + (color[0] / (1 - cgr) - cgr) * y;
					float cg = cgg + (color[1] / (1 - cgg) - cgg) * y;
					float cb = cgb + (color[2] / (1 - cgb) - cgb) * y;


					glColor3f(cgr, cgg, cgb);
					glVertex3f(x, 0, z);
					glVertex3f(x + d, 0, z);
					glColor3f(cr, cg, cb);
					glVertex3f(x + d / 2, y, z + d / 2);

					glColor3f(cgr, cgg, cgb);
					glVertex3f(x, 0, z);
					glVertex3f(x, 0, z + d);
					glColor3f(cr, cg, cb);
					glVertex3f(x + d / 2, y, z + d / 2);

					glColor3f(cgr, cgg, cgb);
					glVertex3f(x, 0, z + d);
					glVertex3f(x + d, 0, z + d);
					glColor3f(cr, cg, cb);
					glVertex3f(x + d / 2, y, z + d / 2);

					glColor3f(cgr, cgg, cgb);
					glVertex3f(x + d, 0, z + d);
					glVertex3f(x + d, 0, z);
					glColor3f(cr, cg, cb);
					glVertex3f(x + d / 2, y, z + d / 2);
				}
			}


			glEnd();
		}
		else {
			glBegin(GL_QUADS);
			float d = farLen / len;
			for (size_t i = 1; i < len; i++) {
				float x0 = ((i - (float)len / 2) * d);
				float x1 = ((i - 1 - (float)len / 2) * d);
				for (size_t j = 1; j < len; j++) {
					float z0 = ((j - (float)len / 2) * d);
					float z1 = ((j - 1 - (float)len / 2) * d);

					float y00 = floor[i * len + j];
					float y10 = floor[(i - 1) * len + j];
					float y01 = floor[i * len + j - 1];
					float y11 = floor[(i - 1) * len + j - 1];

					RenderDot(x0, y00, z0);
					RenderDot(x0, y01, z1);
					RenderDot(x1, y11, z1);
					RenderDot(x1, y10, z0);
				}

			}



			glEnd();

		}


		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

	}

	std::mutex ActsNUI::nuiMtx{};
	ActsNUI* ActsNUI::nui{};

	ActsNUI::ActsNUI() {
		{
			std::lock_guard lg{ ActsNUI::nuiMtx };

			if (nui) throw std::runtime_error("Can't register ActsNUI twice!");

			nui = this;
		}
		glfwInit();
		wnd = glfwCreateWindow(width = 1000, height = 600, "ACTS New UI", nullptr, nullptr);
		core::config::SyncConfig(true);
		glfwMakeContextCurrent(wnd);
		bg.LoadCfg();
		glClearColor(bg.colorBackground[0], bg.colorBackground[1], bg.colorBackground[2], 1);
		glfwSetWindowSizeCallback(wnd, NuiSizeCallBack);

		{
			int fontX, fontY, comp;
			float* img = stbi_loadf_from_memory((stbi_uc const*)tool::nui::cascadia::GetCascadiaFont(), (int)tool::nui::cascadia::GetCascadiaFontLen(), &fontX, &fontY, &comp, STBI_rgb_alpha);

			if (!img) {
				throw std::runtime_error("Can't load cascadia image from memory");
			}

			utils::CloseEnd ice{ [img] { stbi_image_free(img); } };

			GLuint cascadiaTexture{};
			glGenTextures(1, &cascadiaTexture);
			glBindTexture(GL_TEXTURE_2D, cascadiaTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fontX, fontY, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			glBindTexture(GL_TEXTURE_2D, 0);

			LOG_TRACE("loaded cascadia with {}x{} {} -> {}", fontX, fontY, comp, (int)cascadiaTexture);

			nui->cascadiaFont.LoadFont(cascadiaTexture, (const char*)tool::nui::cascadia::GetCascadiaMap(), tool::nui::cascadia::GetCascadiaMapLen());
		}


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		static std::string imGuiCfg{ [] {
			std::filesystem::path f = utils::GetProgDir() / "acts-imgui.ini";
			return f.string();
		}() };
		io.IniFilename = imGuiCfg.c_str();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(wnd, true);
		ImGui_ImplOpenGL2_Init();
		core::config::SaveConfig();
	}
	ActsNUI::~ActsNUI() {
		{
			std::lock_guard lg{ nuiMtx };

			if (nui == this) {
				nui = nullptr;
			}
			else {
				LOG_WARNING("Can't unregister NUI");
			}

		}
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(wnd);
		glfwTerminate();
	}
	void ActsNUI::SetWindowName(const char* name) {
		glfwSetWindowTitle(wnd, name);
	}

	bool ActsNUI::ShouldClose() {
		return glfwWindowShouldClose(wnd);
	}

	void ActsNUI::Loop() {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ActsNUI::Render() {
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(wnd);
	}

	void ActsNUI::Resize(int _width, int _height) {
		width = _width;
		height = _height;
		glViewport(0, 0, width, height);
	}

	int GetNUIScreenWidth() {
		return ActsNUI::nui->width;
	}

	int GetNUIScreenHeight() {
		return ActsNUI::nui->height;
	}
	ImVec2 GetDefaultWindowStart() {
		return ImVec2(ActsNUI::nui->bg.open ? 200.0f : 30.0f, 0);
	}

	void BeginDefaultWindow() {
		ImVec2 vec{ GetDefaultWindowStart() };
		ImGui::SetNextWindowPos(vec);
		vec.x = (float)GetNUIScreenWidth() - vec.x;
		vec.y = (float)GetNUIScreenHeight();
		ImGui::SetNextWindowSize(vec);
		ImGui::SetNextWindowBgAlpha(ActsNUI::nui->bg.bgAlpha / 100.0f);
		ImGui::Begin("acts-nui-menu", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	}

	void EndDefaultWindow() {
		ImGui::End();
	}
	void DisableNextBackground() {
		ActsNUI::nui->bg.nextDisabled = true;
	}

	void OpenNuiWindow() {
		ActsNUI nui{};

		std::string lastMenu = core::config::GetString("nui.last");

		if (!lastMenu.empty()) {
			for (auto& [id, tool] : tool::nui::tools()) {
				if (tool->m_devTool && !nui.bg.showDevTools) continue;
				if (lastMenu == tool->m_id) {
					tool->m_open = true;
					break;
				}
			}
		}

		tool::nui::tooluifunctiondata* prev{};
		while (!nui.ShouldClose()) {
			nui.bg.PreLoop();
			nui.Loop();

			ImVec2 vec{};
			ImGui::SetNextWindowPos(vec);
			vec.x = GetDefaultWindowStart().x;
			vec.y = (float)nui.height;
			ImGui::SetNextWindowSize(vec);
			ImGui::SetNextWindowBgAlpha(std::clamp<float>(nui.bg.bgAlpha / 100.0f + 0.10f, 0, 1));
			bool c = false;
			if (ImGui::Begin("acts-nui-items", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings)) {
				if (nui.bg.open) {
					if (ImGui::ArrowButton("Open", ImGuiDir_Left)) {
						core::config::SetBool("nui.open", false);
						nui.bg.open = false;
						c = true;
					}
					static std::string titleIn = std::format("Atian tools {} UI", actsinfo::VERSION);
					ImGui::SeparatorText(titleIn.c_str());
				}
				else {
					if (ImGui::ArrowButton("Close", ImGuiDir_Right)) {
						nui.bg.open = true;
						core::config::SetBool("nui.open", true);
						c = true;
					}
					ImGui::SeparatorText("A");
				}
				tool::nui::tooluifunctiondata* data{};
				for (auto& [id, tool] : tool::nui::tools()) {
					if (tool->m_devTool && !nui.bg.showDevTools) continue;
					if (ImGui::Selectable(nui.bg.open ? tool->m_description : tool->m_descriptionSmall, &tool->m_open)) {
						data = tool;
						for (auto& [id, tool] : tool::nui::tools()) {
							if (tool != data) {
								tool->m_open = false;
							}
						}
					}
					if (tool->m_open) {
						data = tool;
					}
				}
				if (data != prev) {
					if (data && !data->m_open) {
						data = nullptr;
					}
					if (data) {
						core::config::SetString("nui.last", data->m_id);
					}
					else {
						core::config::SetString("nui.last", "");
					}
					c = true;

					static std::string title{};
					title = std::format("Atian tools {} UI{}{}", actsinfo::VERSION, ((data && data->m_description) ? " - " : ""), ((data && data->m_description) ? data->m_description : ""));
					nui.SetWindowName(title.c_str());
				}

				ImGui::End();

				if (data && data->m_open) {
					if (!data->m_issetup) {
						if (data->m_setup) {
							c |= data->m_setup();
						}
						data->m_issetup = true;
					}
					c |= data->m_func();
				}
			}
			if (c) core::config::SaveConfig();

			nui.bg.Render();

			nui.Render();
			nui.bg.WaitFPS();
		}
	}
	namespace {
		int newui(Process& proc, int argc, const char* argv[]) {
			tool::nui::OpenNuiWindow();

			return tool::OK;
		}

		bool newui_bg() {
			bool c = false;
			ActsConfig& bg = ActsNUI::nui->bg;
			ImGui::SetNextWindowPos(GetDefaultWindowStart(), ImGuiCond_Appearing);
			ImGui::SetNextWindowBgAlpha(bg.bgAlpha / 100.0f);
			if (ImGui::Begin("Config", nullptr)) {
				ImGui::SeparatorText("Utilities");
				c |= ImGui::Checkbox("Load hashes at startup (Can be slow)", &bg.loadHashStartup);
				c |= ImGui::Checkbox("Show dev tools", &bg.showDevTools);

				ImGui::SeparatorText("Background Config");
				c |= ImGui::Checkbox("Enabled", &bg.enabled);
				c |= ImGui::SliderInt("Max fps", &bg.fps, 1, 240);
				if (ImGui::SliderFloat3("Background", bg.colorBackground, 0, 1, "%.2f")) {
					glClearColor(bg.colorBackground[0], bg.colorBackground[1], bg.colorBackground[2], 1);
					c = true;
				}

				if (bg.enabled) {
					c |= ImGui::SliderFloat("fov", &bg.fov, 40, 140, "%.0f");
					c |= ImGui::SliderInt("Spikes", &bg.nextLen, 2, 200);
					c |= ImGui::SliderFloat("Far", &bg.farLen, 1, 50, "%.1f");
					c |= ImGui::Checkbox("Triangles", &bg.triangles);

					if (c) bg.Gen(false);

					c |= ImGui::SliderFloat3("Color", bg.color, 0, 1, "%.2f");
					c |= ImGui::SliderFloat3("Ground", bg.colorGround, 0.2f, 1, "%.2f");
					c |= ImGui::SliderInt("Background opacity", &bg.bgAlpha, 0, 100, "%d %%");

					if (ImGui::Button("Regen terrain")) {
						bg.Gen();
					}
				}

				if (c) {
					core::config::SetBool("nui.bg.enabled", bg.enabled);
					core::config::SetBool("nui.bg.showDevTools", bg.showDevTools);
					core::config::SetInteger("nui.bg.fps", bg.fps);
					core::config::SetBool("nui.loadHashesStartup", bg.loadHashStartup);
					core::config::SetInteger("nui.bg.spikes", bg.nextLen);
					core::config::SetDouble("nui.bg.far", (double)bg.farLen);
					core::config::SetBool("nui.bg.triangles", bg.triangles);
					core::config::SetDouble("nui.bg.fov", bg.fov);
					core::config::SetDouble("nui.bg.colorRed", bg.color[0]);
					core::config::SetDouble("nui.bg.colorGreen", bg.color[1]);
					core::config::SetDouble("nui.bg.colorBlue", bg.color[2]);
					core::config::SetDouble("nui.bg.colorGroundRed", bg.colorGround[0]);
					core::config::SetDouble("nui.bg.colorGroundGreen", bg.colorGround[1]);
					core::config::SetDouble("nui.bg.colorGroundBlue", bg.colorGround[2]);
					core::config::SetDouble("nui.bg.colorBackgroundRed", bg.colorBackground[0]);
					core::config::SetDouble("nui.bg.colorBackgroundGreen", bg.colorBackground[1]);
					core::config::SetDouble("nui.bg.colorBackgroundBlue", bg.colorBackground[2]);
					core::config::SetInteger("nui.bg.alpha", bg.bgAlpha);
				}

			}
			return c;
		}

		bool acts() {
			tool::nui::NuiUseDefaultWindow dw{};
			std::string txt = utils::WStrToStr(tool::ui::GetActsDesc(true));
			ImGui::Text(txt.c_str());
			return false;
		}

		ADD_TOOL("newui", "dev", "", "Launch new UI", nullptr, newui);
		ADD_TOOL_NUI("aaaa_acts", "Atian Tools", acts);
		ADD_TOOL_NUI("zzzz_newui_cfg", "Config", newui_bg);
	}
}