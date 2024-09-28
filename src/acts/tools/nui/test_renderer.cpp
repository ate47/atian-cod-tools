#include <includes.hpp>
#include <core/config.hpp>
#include "tools/tools_nui.hpp"
#include <gl/GL.h>
#include <gl/GLU.h>

namespace {
	using namespace tool::nui;

	class TestRenderer {
	public:
		float cameraX{}, cameraY{ 0.1f }, cameraZ{};
		float pitch{}, yaw{};
		float viewDistance{ 200 };
		float rotateSpeed{ 0.10f };
		float moveSpeed{ 1 };

		template<bool yaxis>
		inline bool IsInBox(float x, float z, float len, float height) const {
			if (cameraX < x && cameraX > x + len) return false;
			if (cameraZ < z && cameraZ > z + len) return false;
			if constexpr (yaxis) {
				return cameraY < height;
			}
			return true;
		}

		void RenderBox(float x, float z, float len, float height, float r, float g, float b) {
			// top
			if (cameraY > height || IsInBox<false>(x, z, len, height)) {
				glColor3f(r, g, b);
				glVertex3f(x, height, z);
				glVertex3f(x + len, height, z);
				glVertex3f(x + len, height, z + len);
				glVertex3f(x, height, z + len);
			}
			bool in = IsInBox<true>(x, z, len, height);

			glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
			// xl
			if (in || (z > cameraZ)) {
				glVertex3f(x, 0, z);
				glVertex3f(x, height, z);
				glVertex3f(x + len, height, z);
				glVertex3f(x + len, 0, z);
			}
			// xr
			if (in || (z + len < cameraZ)) {
				glVertex3f(x, 0, z + len);
				glVertex3f(x, height, z + len);
				glVertex3f(x + len, height, z + len);
				glVertex3f(x + len, 0, z + len);
			}
			glColor3f(r * 0.50f, g * 0.50f, b * 0.50f);
			// yl
			if (in || (x > cameraX)) {
				glVertex3f(x, 0, z);
				glVertex3f(x, height, z);
				glVertex3f(x, height, z + len);
				glVertex3f(x, 0, z + len);
			}
			// yr
			if (in || (x + len < cameraX)) {
				glVertex3f(x + len, 0, z);
				glVertex3f(x + len, height, z);
				glVertex3f(x + len, height, z + len);
				glVertex3f(x + len, 0, z + len);
			}
		}

		void Render() {
			float cp = std::cos(pitch);
			float sp = std::sin(pitch);
			float cy = std::cos(yaw);
			float sy = std::sin(yaw);

			float normalLookX = cy * cp;
			float normalLookY = sp;
			float normalLookZ = sy * cp;

			float partial = ActsNUI::nui->bg.partialTick;
			float speed = moveSpeed / partial;

			if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
				speed *= 2;
			}
			

			if (ImGui::IsKeyDown(ImGuiKey_Z)) {
				cameraX += speed * normalLookX;
				cameraY += speed * normalLookY;
				cameraZ += speed * normalLookZ;
			}
			else if (ImGui::IsKeyDown(ImGuiKey_S)) {
				cameraX -= speed * normalLookX;
				cameraY -= speed * normalLookY;
				cameraZ -= speed * normalLookZ;
			}
			if (ImGui::IsKeyDown(ImGuiKey_Q)) {
				float cy = std::cos(yaw + (PI / 2.0f));
				float sy = std::sin(yaw + (PI / 2.0f));
				cameraX -= speed * cy;
				cameraZ -= speed * sy;
			}
			else if (ImGui::IsKeyDown(ImGuiKey_D)) {
				float cy = std::cos(yaw + (PI / 2.0f));
				float sy = std::sin(yaw + (PI / 2.0f));
				cameraX += speed * cy;
				cameraZ += speed * sy;
			}
			if (ImGui::IsKeyDown(ImGuiKey_Space)) {
				cameraY += speed;
			}
			else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
				cameraY = std::max<float>(cameraY - speed, 0.1f);
			}

			struct MouseDragOpt {
				bool drag{};

				double startX, startY;
				double lastX, lastY;
				double deltaX, deltaY;

			};
			static MouseDragOpt dragInfo{};
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
				glfwGetCursorPos(ActsNUI::nui->wnd, &dragInfo.lastX, &dragInfo.lastY);

				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				if (!dragInfo.drag) {
					dragInfo.startX = dragInfo.lastX;
					dragInfo.startY = dragInfo.lastY;
					dragInfo.drag = true;
				}
				else {
					float deltaX = (float)(dragInfo.lastX - dragInfo.startX);
					float deltaY = (float)(dragInfo.lastY - dragInfo.startY);

					glfwSetCursorPos(ActsNUI::nui->wnd, dragInfo.startX, dragInfo.startY);

					yaw += deltaX * rotateSpeed / partial;
					if (yaw > PI2) {
						yaw -= PI2;
					}
					else if (yaw < 0) {
						yaw += PI2;
					}
					constexpr float maxDelta = (PI / 2) * 0.95f;
					pitch = std::clamp<float>(pitch - deltaY * rotateSpeed / partial, -maxDelta, maxDelta);
				}

			}
			else {
				dragInfo.drag = false;
			}

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			glLoadIdentity();
			gluPerspective(ActsNUI::nui->bg.fov / 2, (GLdouble)ActsNUI::nui->width / ActsNUI::nui->height, 0.1, viewDistance);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			gluLookAt(
				cameraX, cameraY, cameraZ,
				cameraX + normalLookX, cameraY + normalLookY, cameraZ + normalLookZ,
				0, 1, 0
			);
			// floor
			glBegin(GL_QUADS);
			glColor3f(0.2f, 0.2f, 0.2f);
			glVertex3f(-100, 0, -100);
			glVertex3f(-100, 0, 100);
			glVertex3f(100, 0, 100);
			glVertex3f(100, 0, -100);


			// boxes

			RenderBox(1, -1, 1, 1, 1, 0, 0);
			RenderBox(2, 3, 1, 1.5f, 1, 1, 0);
			RenderBox(1, 5, 1, 1.2f, 0, 0, 1);
			RenderBox(-1, -3, 1, 2, 0, 1, 1);
			RenderBox(-10, -10, 5, 5, 1, 1, 1);
			RenderBox(-9, 3, 1.3f, 2, 1, 0, 1);


			glEnd();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}
	};

	TestRenderer test{};

	bool render_test_setup() {
		test.viewDistance = (float)core::config::GetDouble("nui.testRenderer.viewDistance", test.viewDistance);
		test.rotateSpeed = (float)core::config::GetDouble("nui.testRenderer.rotateSpeed", test.rotateSpeed);
		test.moveSpeed = (float)core::config::GetDouble("nui.testRenderer.moveSpeed", test.moveSpeed);

		return true;
	}

	bool render_test() {
		// disable the default background
		DisableNextBackground();

		bool c = false;

		ImGui::SetNextWindowPos(GetDefaultWindowStart(), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(ActsNUI::nui->bg.bgAlpha / 100.0f);
		if (ImGui::Begin("Renderer tests", nullptr)) {
			ImGui::SeparatorText("World info");
			ImGui::Text("Camera: %.1f,%.1f,%.1f", test.cameraX, test.cameraY, test.cameraZ);
			ImGui::Text("Angles: %.1f,%.1f", test.pitch / PI2 * 360, test.yaw / PI2 * 360);

			ImGui::SeparatorText("Camera config");

			if (ImGui::SliderFloat("View distance", &test.viewDistance, 1, 500, "%.0f")) {
				core::config::SetDouble("nui.testRenderer.viewDistance", test.viewDistance);
				c = true;
			}
			if (ImGui::SliderFloat("Rotate speed", &test.rotateSpeed, 0.01f, 0.5f, "%.2f")) {
				core::config::SetDouble("nui.testRenderer.rotateSpeed", test.rotateSpeed);
				c = true;
			}
			if (ImGui::SliderFloat("Move speed", &test.moveSpeed, 0.1f, 2.0f, "%.1f")) {
				core::config::SetDouble("nui.testRenderer.moveSpeed", test.moveSpeed);
				c = true;
			}

			ImGui::End();
		}

		test.Render();

		return c;
	}


	ADD_TOOL_NUI2_DEV("render_test", "Renderer test", render_test_setup, render_test);
}