#pragma once
#include <gl/GL.h>

namespace tool::nui::renderer {
	struct NuiFontSymbolData {
		int x;
		int y;
		int w;
		int h;
	};
	constexpr int minChar = 0x20;
	constexpr int maxChar = 0x7E;
	static_assert(minChar < maxChar);
	class NuiFont {
	private:
		NuiFontSymbolData symbolsData[maxChar - minChar + 1]{};
		GLuint textureId{};
	public:
		NuiFont();

		void LoadFont(GLuint textureId, const char* str, size_t len);

		constexpr operator bool() {
			return textureId;
		}

		void RenderString2D(float x, float y, const char* str);
		void RenderString3D(float x, float y, float z, const char* str);
		void RenderString2Df(float x, float y, const char* str, ...);
		void RenderString3Df(float x, float y, float z, const char* str, ...);
	};
}