#include <includes.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "tools/tools_nui.hpp"
#include "nui_renderer.hpp"

namespace tool::nui::renderer {
	NuiFont::NuiFont() : textureId(0) {}
	void NuiFont::LoadFont(GLuint textureId, const char* str, size_t len) {
		this->textureId = textureId;
		rapidjson::Document doc{};
		doc.Parse(str, len);
		if (doc.HasParseError()) {
			throw std::runtime_error("Can't load font: Json parse error");
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

					if (idIt == symbol.MemberEnd()) continue; // no id, error

					int id = idIt->value.GetInt();

					if (id < minChar || id > maxChar) continue; // bad id

					NuiFontSymbolData& data{ symbolsData[id - minChar] };

					if (xIt != symbol.MemberEnd()) data.x = xIt->value.GetInt();
					if (yIt != symbol.MemberEnd()) data.y = yIt->value.GetInt();
					if (heightIt != symbol.MemberEnd()) data.h = heightIt->value.GetInt();
					if (widthIt != symbol.MemberEnd()) data.w = widthIt->value.GetInt();

					loaded++;
				}
			}
		}
		LOG_TRACE("Loaded font json with {} symbol(s)", loaded);
	}




	void NuiFont::RenderString2D(float x, float y, const char* str) {
		glBindTexture(GL_TEXTURE_2D, textureId);

		glBindTexture(GL_TEXTURE_2D, 0);

		throw std::runtime_error("Not implemented: 2D strings");
	}

	void NuiFont::RenderString3D(float x, float y, float z, const char* str) {
		glBindTexture(GL_TEXTURE_2D, textureId);

		glBindTexture(GL_TEXTURE_2D, 0);

		throw std::runtime_error("Not implemented: 3D strings");
	}

	void NuiFont::RenderString2Df(float x, float y, const char* str, ...) {
		va_list ap;
		va_start(ap, str);
		const char* s = utils::vap(str, ap);
		va_end(ap);
		RenderString2D(x, y, s);
	}

	void NuiFont::RenderString3Df(float x, float y, float z, const char* str, ...) {
		va_list ap;
		va_start(ap, str);
		const char* s = utils::vap(str, ap);
		va_end(ap);
		RenderString3D(x, y, z, s);
	}
}