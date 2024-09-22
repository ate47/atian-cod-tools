#include <includes.hpp>
#include <core/config.hpp>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl2.h>
#include "tools/tools_nui.hpp"
#include "tools/tools_ui.hpp"

namespace {
	class ActsNUI;

	static ActsNUI* nui{};
	static std::default_random_engine re;

	class RandomBackground {
		std::vector<float> floor{};
		size_t len{};
		int nextLen{ 100 };
		float farLen{ 20 };
		float color[3]{ 1, 1, 1 };
		float colorGround[3]{ 0.2f, 0.2f, 0.2f };
		bool triangles{};

	public:
		int bgAlpha{ 85 };
		RandomBackground() {
		}

		void LoadCfg() {
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
			Gen();

		}

		float& Floor(size_t i, size_t j) {
			return floor[i * len + j];
		}

		void Gen(bool force = true) {
			if (len == nextLen && !force) return;
			len = nextLen;
			floor.resize(len * len);

			std::uniform_real_distribution<float> unif(0, 1);

			// full random?
			for (size_t i = 0; i < floor.size(); i++) {
				floor[i] = unif(re);
			}
		}

		bool CreateImGuiMenu() {
			bool c = false;
			ImGui::SeparatorText("Background Config");
			c |= ImGui::SliderInt("Spikes", &nextLen, 2, 200);
			c |= ImGui::SliderFloat("Far", &farLen, 1, 50);
			c |= ImGui::Checkbox("Triangles", &triangles);

			if (c) Gen(false);

			c |= ImGui::SliderFloat3("Color", color, 0, 1);
			c |= ImGui::SliderFloat3("Ground", colorGround, 0.2f, 1);
			c |= ImGui::SliderInt("Background opacity", &bgAlpha, 0, 100, "%d %%");

			if (ImGui::Button("Regen terrain")) {
				Gen();
			}

			if (c) {
				core::config::SetInteger("nui.bg.spikes", nextLen);
				core::config::SetDouble("nui.bg.far", (double)farLen);
				core::config::SetBool("nui.bg.triangles", triangles);
				core::config::SetDouble("nui.bg.colorRed", color[0]);
				core::config::SetDouble("nui.bg.colorGreen", color[1]);
				core::config::SetDouble("nui.bg.colorBlue", color[2]);
				core::config::SetDouble("nui.bg.colorGroundRed", colorGround[0]);
				core::config::SetDouble("nui.bg.colorGroundGreen", colorGround[1]);
				core::config::SetDouble("nui.bg.colorGroundBlue", colorGround[2]);
				core::config::SetInteger("nui.bg.alpha", bgAlpha);
			}
			return c;
		}


		void RenderDot(float x, float y, float z) const {
			float cgr = colorGround[0];
			float cgg = colorGround[1];
			float cgb = colorGround[2];

			float cr = cgr + (color[0] / (1 - cgr) - cgr) * y;
			float cg = cgg + (color[1] / (1 - cgg) - cgg) * y;
			float cb = cgb + (color[2] / (1 - cgb) - cgb) * y;

			glColor3f(cr, cg, cb);
			glVertex3f(x, y, z);
		};

		void Render() {
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

		}
	};
	static RandomBackground bg{};


	void NuiSizeCallBack(GLFWwindow* wnd, int width, int height);

	class ActsNUI {
		GLFWwindow* wnd;

	public:
		int width;
		int height;
		ActsNUI() {
			{
				static std::mutex mtx{};
				std::lock_guard lg{ mtx };

				if (nui) throw std::runtime_error("Can't register ActsNUI twice!");

				nui = this;
			}
			glfwInit();
			wnd = glfwCreateWindow(width = 1000, height = 600, "ACTS New UI", nullptr, nullptr);
			core::config::SyncConfig(true);
			glfwMakeContextCurrent(wnd);
			glClearColor(0.2f, 0.2f, 0.2f, 1);
			glfwSetWindowSizeCallback(wnd, NuiSizeCallBack);

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
		~ActsNUI() {
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			glfwDestroyWindow(wnd);
			glfwTerminate();
		}

		void SetWindowName(const char* name) {
			glfwSetWindowTitle(wnd, name);
		}

		bool ShouldClose() {
			return glfwWindowShouldClose(wnd);
		}

		void Loop() {
			glfwPollEvents();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void Render() {
			ImGui::Render();
			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(wnd);
		}

		void Resize(int _width, int _height) {
			width = _width;
			height = _height;
			glViewport(0, 0, width, height);
		}
	};

	void NuiSizeCallBack(GLFWwindow* wnd, int width, int height) {
		nui->Resize(width, height);
	}

	int newui(Process& proc, int argc, const char* argv[]) {
		ActsNUI nui{};
		bg.LoadCfg();

		while (!nui.ShouldClose()) {
			nui.Loop();

			ImVec2 vec{};
			ImGui::SetNextWindowPos(vec);
			vec.x = (float)nui.width;
			vec.y = (float)nui.height;
			ImGui::SetNextWindowSize(vec);
			ImGui::SetNextWindowBgAlpha(bg.bgAlpha / 100.0f);
			ImGui::Begin("Acts NUI", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
			bool c = false;
			if (ImGui::BeginTabBar("ACTS NUI")) {
				for (auto& [id, tool] : tool::nui::tools()) {
					if (ImGui::BeginTabItem(tool->m_description)) {
						static std::string title{};
						title = std::format("Acts UI - {}", tool->m_description);
						nui.SetWindowName(title.c_str());
						c |= tool->m_func();
						ImGui::EndTabItem();
					}
				}

				ImGui::EndTabBar();
			}
			if (c) core::config::SaveConfig();
			ImGui::End();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			glLoadIdentity();
			glFrustum(-1, 1, -1, 1, 1.5, 20);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			constexpr long long timeFrame = 3600ll * 25;
			double time = (double)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % timeFrame);

			double cameraX = std::cos((time / timeFrame) * 2 * 3.1415);
			double cameraY = std::sin((time / timeFrame) * 2 * 3.1415);

			gluLookAt(
				5 * cameraX, 3, 5 * cameraY,
				0, 0, 0,
				0, 1, 0
			);

			bg.Render();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			nui.Render();
		}


		return tool::OK;
	}

	bool newui_bg() {
		return bg.CreateImGuiMenu();
	}

	bool acts() {
		std::string txt = utils::WStrToStr(tool::ui::GetActsDesc(true));
		ImGui::Text(txt.c_str());
		return false;
	}

	ADD_TOOL("newui", "dev", "", "Launch new UI", nullptr, newui);
	ADD_TOOL_NUI("acts", "Acts", acts);
	ADD_TOOL_NUI("newui_bg", "Background", newui_bg);
}