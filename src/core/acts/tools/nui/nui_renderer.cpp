#include <includes.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "tools/tools_nui.hpp"
#include "nui_renderer.hpp"

namespace tool::nui::renderer {
	NuiFont::NuiFont() : textureId(0) {}
	void NuiFont::LoadFont(GLuint textureId, const char* str, size_t len, int w, int h) {
		this->textureId = textureId;
		rapidjson::Document doc{};
		doc.Parse(str, len);
		if (doc.HasParseError()) {
			throw std::runtime_error("Can't load font: Json parse error");
		}

		// read config
		auto configIt = doc.FindMember("config");
		if (configIt != doc.MemberEnd()) {
			auto charHeightIt = configIt->value.FindMember("charHeight");
			auto sizeIt = configIt->value.FindMember("size");
			if (charHeightIt != configIt->value.MemberEnd()) fontOriginalHeight = charHeightIt->value.GetFloat() / h;
			if (sizeIt != configIt->value.MemberEnd()) fontOriginalWidth = sizeIt->value.GetFloat() / w;
		}

		// read symbols array
		auto symbolsIt = doc.FindMember("symbols");
		int loaded{};
		if (symbolsIt != doc.MemberEnd()) {
			if (symbolsIt->value.IsArray()) {
				auto arr = symbolsIt->value.GetArray();

				for (const auto& symbol : arr) {
					auto idIt = symbol.FindMember("id");
					auto xIt = symbol.FindMember("x");
					auto yIt = symbol.FindMember("y");
					auto heightIt = symbol.FindMember("height");
					auto widthIt = symbol.FindMember("width");
					auto xoffsetIt = symbol.FindMember("xoffset");
					auto yoffsetIt = symbol.FindMember("yoffset");

					if (idIt == symbol.MemberEnd()) continue; // no id, error

					int id = idIt->value.GetInt();

					if (id < minChar || id > maxChar) continue; // bad id

					NuiFontSymbolData& data{ symbolsData[id - minChar] };

					if (xIt != symbol.MemberEnd()) data.x = xIt->value.GetFloat() / w;
					if (yIt != symbol.MemberEnd()) data.y = yIt->value.GetFloat() / h;
					if (xoffsetIt != symbol.MemberEnd()) data.offx = xoffsetIt->value.GetFloat() / w;
					if (yoffsetIt != symbol.MemberEnd()) data.offy = yoffsetIt->value.GetFloat() / h;
					if (heightIt != symbol.MemberEnd()) data.h = heightIt->value.GetFloat() / h;
					if (widthIt != symbol.MemberEnd()) data.w = widthIt->value.GetFloat() / w;

					loaded++;
				}
			}
		}
		LOG_TRACE("Loaded font json with {} symbol(s)", loaded);
	}



	void NuiFont::RenderChar(float& x, float& y, const char c) const {
		float aspectRatio = fontSize / fontOriginalHeight;
		float sizeX = fontOriginalWidth * aspectRatio;
		if (c >= minChar && c <= maxChar) {
			const NuiFontSymbolData& data{ symbolsData[c - minChar] };

			float midSizeX = sizeX * data.w / fontOriginalWidth;
			float midSizeY = fontSize * data.h / fontOriginalHeight;
			float xc = x + aspectRatio * data.offx;
			float yc = y + fontSize * data.offy;
			glTexCoord2f(data.x, data.y);
			glVertex2f(xc, yc);
			glTexCoord2f(data.x + data.w, data.y);
			glVertex2f(xc + midSizeX, yc);
			glTexCoord2f(data.x + data.w, data.y + data.h);
			glVertex2f(xc + midSizeX, yc + midSizeY);
			glTexCoord2f(data.x, data.y + data.h);
			glVertex2f(xc, yc + midSizeY);
		}

		x += sizeX + 1;

	}

	void LoadColor(uint32_t color) {
		if (color & 0xFF000000) {
			glColor4f(
				(float)((color >> 16) & 0xFF) / 0xFF,
				(float)((color >> 8) & 0xFF) / 0xFF,
				(float)(color & 0xFF) / 0xFF,
				(float)((color >> 24) & 0xFF) / 0xFF
			);
		}
		else {
			glColor3f(
				(float)((color >> 16) & 0xFF) / 0xFF,
				(float)((color >> 8) & 0xFF) / 0xFF,
				(float)(color & 0xFF) / 0xFF
			);
		}
	}

	void NuiFont::RenderString2D(uint32_t color, float x, float y, const char* str) const {
		LoadColor(color);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glBegin(GL_QUADS);

		while (*str) {
			RenderChar(x, y, *(str++));
		}

		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

	}

	void NuiFont::RenderString3D(uint32_t color, float x, float y, float z, const char* str) const {
		glBindTexture(GL_TEXTURE_2D, textureId);

		glBindTexture(GL_TEXTURE_2D, 0);

		throw std::runtime_error("Not implemented: 3D strings");
	}

	void NuiFont::RenderString2Df(uint32_t color, float x, float y, const char* str, ...) const {
		va_list ap;
		va_start(ap, str);
		const char* s = utils::vap(str, ap);
		va_end(ap);
		RenderString2D(color, x, y, s);
	}

	void NuiFont::RenderString3Df(uint32_t color, float x, float y, float z, const char* str, ...) const {
		va_list ap;
		va_start(ap, str);
		const char* s = utils::vap(str, ap);
		va_end(ap);
		RenderString3D(color, x, y, z, s);
	}
}