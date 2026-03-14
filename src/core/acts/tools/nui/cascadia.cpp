#include <includes.hpp>

/*
  https://github.com/microsoft/cascadia-code
  SIL OPEN FONT LICENSE Version 1.1 - 26 February 2007
  xxd -i .\cascadia_mono_semibold_12.PNG > cascadia_atlas.hpp
  xxd -i .\cascadia_mono_semibold_12.json > cascadia_map.hpp
 */

#include "cascadia_atlas.hpp"
#include "cascadia_map.hpp"

namespace tool::nui::cascadia {
	void* GetCascadiaFont() {
		return __cascadia_mono_semibold_12_PNG;
	}
	size_t GetCascadiaFontLen() {
		return __cascadia_mono_semibold_12_PNG_len;
	}
	void* GetCascadiaMap() {
		return __cascadia_mono_semibold_12_json;
	}
	size_t GetCascadiaMapLen() {
		return __cascadia_mono_semibold_12_json_len;
	}
}
