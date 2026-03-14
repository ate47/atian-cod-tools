#pragma once
#include <gl/GL.h>

namespace tool::nui::renderer {
	struct NuiFontSymbolData {
		float x;
		float y;
		float w;
		float h;
		float offx;
		float offy;
	};
	constexpr int minChar = 0x21;
	constexpr int maxChar = 0x7E;
	static_assert(minChar < maxChar);

	void LoadColor(uint32_t color);

	class NuiFont {
	private:
		NuiFontSymbolData symbolsData[maxChar - minChar + 1]{};
		GLuint textureId{};
		float fontOriginalHeight{};
		float fontOriginalWidth{};
	public:
		float fontSize{ 12 };
		NuiFont();

		void LoadFont(GLuint textureId, const char* str, size_t len, int w, int h);

		constexpr operator bool() {
			return textureId;
		}

		void RenderChar(float& x, float& y, const char c) const;
		void RenderString2D(uint32_t color, float x, float y, const char* str) const;
		void RenderString3D(uint32_t color, float x, float y, float z, const char* str) const;
		void RenderString2Df(uint32_t color, float x, float y, const char* str, ...) const;
		void RenderString3Df(uint32_t color, float x, float y, float z, const char* str, ...) const;
	};
}