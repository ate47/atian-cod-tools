#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/actsinfo.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/config.hpp>
#include <core/memory_allocator.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <systems/gsc_funcs.hpp>
#include <systems/gsc_debug.hpp>
#include <systems/events.hpp>
#include <systems/errors.hpp>

namespace systems::gsc::debug {
	namespace {
		bo4::vec4_t colorWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

		enum HudXAlign : int { HXA_LEFT, HXA_CENTER, HXH_RIGHT };
		enum HudYAlign : int { HYA_TOP, HYA_MIDDLE, HYA_BOTTOM };
		enum HudHAlign : int { HHA_SUBLEFT, HHA_LEFT, HHA_CENTER, HVH_RIGHT, HHA_FULLSCREEN, HHA_NOSCALE, HHA_ALIGNTO640, HHA_CENTER_SAFEAREA, HHA_USER_LEFT, HHA_USER_CENTER, HHA_USER_RIGHT };
		enum HudVAlign : int { HVA_SUBTOP, HVA_TOP, HVA_MIDDLE, HVA_BOTTOM, HVA_FULLSCREEN, HVA_NOSCALE, HVA_ALIGNTO480, HVA_CENTER_SAFEAREA, HVA_USER_TOP, HVA_USER_CENTER, HVA_USER_BOTTOM };

		std::array HudXAlignNames{ "left", "center", "right" };
		std::array HudYAlignNames{ "top", "middle", "bottom" };
		std::array HudHAlignNames{ "subleft", "left", "center", "right", "fullscreen", "noscale", "alignto640", "center_safearea", "user_left", "user_center", "user_right" };
		std::array HudVAlignNames{ "subtop", "top", "middle", "bottom" , "fullscreen", "noscale", "alignto480", "center_safearea", "user_top", "user_center", "user_bottom" };

		void SetEnumField(bo4::scriptInstance_t inst, byte* data, int size, int ofs, const char** types, size_t count) {
			const char* v{ bo4::ScrVm_GetString(inst, 0) };

			for (size_t i = 0; i < count; i++) {
				if (_strcmpi(types[i], v)) continue;

				switch (size) {

				}

				return;
			}

			systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Value '%s' is not a valid enum field", false, v);
		}

		struct ScrHudElem {
			float x{};
			float y{};
			float z{};
			float fontScale{ 1 };
			float fromFontScale{};
			bo4::vec3_t color{ 1, 1, 1 };
			float alpha{ 1 };
			HudHAlign horzalign{};
			HudVAlign vertalign{};
			HudXAlign alignx{}; 
			HudYAlign aligny{}; 
			bool allocated{};
			const char* text{};
		};
		constexpr uint32_t numMaxHudElements = 0x100;



		struct HudElemField;
		typedef void (*ScriptCallbackHudElem)(ScrHudElem* hud, HudElemField* field);

		struct HudElemField {
			uint32_t canonId;
			bo4::fieldtype_t type;
			int ofs;
			size_t size;
			bool readOnly;
			ScriptCallbackHudElem setter;
			ScriptCallbackHudElem getter;
		};

		template<bool isGetter>
		void InvalidField(ScrHudElem* hud, HudElemField* field) {
			systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Field %s %s not implemented", false, isGetter ? "getter" : "setter", core::hashes::ExtractTmp("var", field->canonId));
		}

		void Scr_SetEnumString(bo4::scriptInstance_t inst, const char** names, size_t count, int* out) {
			const char* v{ bo4::ScrVm_GetString(inst, 0) };
			for (size_t i = 0; i < count; i++) {
				if (!_strcmpi(names[i], v)) {
					*out = (int)i;
					return;
				}
			}
			systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid enum value '%s'", false, v);
		}

		void Scr_GetEnumString(bo4::scriptInstance_t inst, const char** names, size_t count, int out) {
			if (out >= count) {
				systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid enum value '%d'", false, out);
				return;
			}
			bo4::ScrVm_AddString(inst, names[out]);
		}

		void Set_HudHAlign(ScrHudElem* hud, HudElemField* field) { Scr_SetEnumString(bo4::scriptInstance_t::SI_SERVER, HudHAlignNames.data(), HudHAlignNames.size(), (int*)&hud->horzalign); }
		void Get_HudHAlign(ScrHudElem* hud, HudElemField* field) { Scr_GetEnumString(bo4::scriptInstance_t::SI_SERVER, HudHAlignNames.data(), HudHAlignNames.size(), hud->horzalign); }
		void Set_HudVAlign(ScrHudElem* hud, HudElemField* field) { Scr_SetEnumString(bo4::scriptInstance_t::SI_SERVER, HudVAlignNames.data(), HudVAlignNames.size(), (int*)&hud->vertalign); }
		void Get_HudVAlign(ScrHudElem* hud, HudElemField* field) { Scr_GetEnumString(bo4::scriptInstance_t::SI_SERVER, HudVAlignNames.data(), HudVAlignNames.size(), hud->vertalign); }
		void Set_HudXAlign(ScrHudElem* hud, HudElemField* field) { Scr_SetEnumString(bo4::scriptInstance_t::SI_SERVER, HudXAlignNames.data(), HudXAlignNames.size(), (int*)&hud->alignx); }
		void Get_HudXAlign(ScrHudElem* hud, HudElemField* field) { Scr_GetEnumString(bo4::scriptInstance_t::SI_SERVER, HudXAlignNames.data(), HudXAlignNames.size(), hud->alignx); }
		void Set_HudYAlign(ScrHudElem* hud, HudElemField* field) { Scr_SetEnumString(bo4::scriptInstance_t::SI_SERVER, HudYAlignNames.data(), HudYAlignNames.size(), (int*)&hud->aligny); }
		void Get_HudYAlign(ScrHudElem* hud, HudElemField* field) { Scr_GetEnumString(bo4::scriptInstance_t::SI_SERVER, HudYAlignNames.data(), HudYAlignNames.size(), hud->aligny); }

#define __CREATE_GENERIC_HUDELEM_FIELD(field) \
    HudElemField{\
        .canonId = hash::HashT89Scr(#field),\
        .type = bo4::GetFieldType<decltype(ScrHudElem::field)>(),\
        .ofs = offsetof(ScrHudElem, field),\
		.size = sizeof(ScrHudElem::field),\
    }
#define __CREATE_HUDELEM_FIELD(field, _getter, _setter) \
    HudElemField{\
        .canonId = hash::HashT89Scr(#field),\
		.setter = _setter,\
		.getter = _getter,\
    }
#define __CREATE_INVALID_HUDELEM_FIELD(field) __CREATE_HUDELEM_FIELD(field, InvalidField<true>, InvalidField<false>)

		std::array hudFields{
			__CREATE_GENERIC_HUDELEM_FIELD(x),
			__CREATE_GENERIC_HUDELEM_FIELD(y),
			__CREATE_GENERIC_HUDELEM_FIELD(z),
			__CREATE_GENERIC_HUDELEM_FIELD(fontScale),
			__CREATE_GENERIC_HUDELEM_FIELD(color),
			__CREATE_GENERIC_HUDELEM_FIELD(alpha),

			__CREATE_HUDELEM_FIELD(alignx, Get_HudXAlign, Set_HudXAlign),
			__CREATE_HUDELEM_FIELD(aligny, Get_HudYAlign, Set_HudYAlign),
			__CREATE_HUDELEM_FIELD(horzalign, Get_HudHAlign, Set_HudHAlign),
			__CREATE_HUDELEM_FIELD(vertalign, Get_HudVAlign, Set_HudVAlign),

			__CREATE_INVALID_HUDELEM_FIELD(font),
			__CREATE_INVALID_HUDELEM_FIELD(label),
			__CREATE_INVALID_HUDELEM_FIELD(sort),
			__CREATE_INVALID_HUDELEM_FIELD(foreground),
			__CREATE_INVALID_HUDELEM_FIELD(hidewhendead),
			__CREATE_INVALID_HUDELEM_FIELD(hidewheninkillcam),
			__CREATE_INVALID_HUDELEM_FIELD(hidewhenindemo),
			__CREATE_INVALID_HUDELEM_FIELD(immunetodemogamehudsettings),
			__CREATE_INVALID_HUDELEM_FIELD(immunetodemofreecamera),
			__CREATE_INVALID_HUDELEM_FIELD(hidewheninmenu),
			__CREATE_INVALID_HUDELEM_FIELD(fadewhentargeted),
			__CREATE_INVALID_HUDELEM_FIELD(fontstyle3d),
			__CREATE_INVALID_HUDELEM_FIELD(glowcolor),
			__CREATE_INVALID_HUDELEM_FIELD(glowalpha),
			__CREATE_INVALID_HUDELEM_FIELD(archived),
		};

#undef __CREATE_INVALID_HUDELEM_FIELD
#undef __CREATE_HUDELEM_FIELD
#undef __CREATE_GENERIC_HUDELEM_FIELD

		struct {
			ScrHudElem huds[numMaxHudElements]{};
			core::memory_allocator::MemoryAllocator alloc{};
		} dbgData{};

		ScrHudElem* AllocHudElem() {
			for (ScrHudElem& elem : dbgData.huds) {
				if (!elem.allocated) {
					elem = { .allocated = true };
					return &elem;
				}
			}
			return nullptr; // no empty slot
		}

		ScrHudElem* GetHudElem(uint32_t idx) {
			if (idx >= numMaxHudElements || !dbgData.huds[idx].allocated) {
				return nullptr; // invalid / free id
			}
			return &dbgData.huds[idx];
		}

		ScrHudElem* GetHudElem(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			if (ref->classnum != bo4::CLASS_NUM_HUDELEM) {
				systems::errors::ScrVm_Error(inst, "not a hud element", false);
			}

			ScrHudElem* he{ GetHudElem(ref->u.hudElemIndex) };

			if (!he || !he->allocated) {
				systems::errors::ScrVm_Error(inst, "not a valid hud element", false);
				return nullptr;
			}
			return he;
		}

		uint32_t GetHudElemId(ScrHudElem* hud) {
			if (!hud) {
				return numMaxHudElements;
			}
			return (uint32_t)(hud - dbgData.huds);
		}

		
		void GScr_NewDebugHudElem(bo4::scriptInstance_t inst) {
			// TODO: param[0] = player

			ScrHudElem* hud{ AllocHudElem() };

			if (!hud) {
				systems::errors::ScrVm_Error(inst, "out of hudelems", false);
				return;
			}

			bo4::EntRefUnion v{ .hudElemIndex = GetHudElemId(hud) };
			bo4::ScrVm_AddEntityNum(inst, v, bo4::CLASS_NUM_HUDELEM, bo4::LOCAL_CLIENT_0);
		}

		void HECmd_Destroy(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (he) {
				if (he->text) {
					dbgData.alloc.Free((void*)he->text);
				}
				he->allocated = false;
			}

			bo4::ScrEvent_FireCallbackEventEnt(inst, ref->u.hudElemIndex, bo4::CLASS_NUM_HUDELEM, false);
			//ScrVar_FreeEntityNum(inst, ref->u, bo4::CLASS_NUM_HUDELEM, bo4::LOCAL_CLIENT_0);
			systems::errors::ScrVm_Error(inst, "HECmd_Destroy not implemented", false);
		}

		void HECmd_SetText(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			bo4::MessageString string{};
			bo4::Scr_ConstructMessageString(inst, &string, 0, -1);
			const char* old{ he->text };
			he->text = dbgData.alloc.ClonePtr(string.string, string.len + 1);
			if (old) dbgData.alloc.Free((void*)old);
		}

		void HECmd_SetMaterial(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetMaterial not implemented", false);
		}

		void HECmd_SetTargetEnt(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetTargetEnt not implemented", false);
		}

		void HECmd_ClearTargetEnt(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_ClearTargetEnt not implemented", false);
		}

		void HECmd_SetTimer(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetTimer not implemented", false);
		}

		void HECmd_SetTenthsTimer(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetTenthsTimer not implemented", false);
		}

		void HECmd_SetValue(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetValue not implemented", false);
		}

		void HECmd_SetWaypoint(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetWaypoint not implemented", false);
		}

		void HECmd_FadeOverTime(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_FadeOverTime not implemented", false);
		}

		void HECmd_ScaleOverTime(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_ScaleOverTime not implemented", false);
		}

		void HECmd_MoveOverTime(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_MoveOverTime not implemented", false);
		}

		void HECmd_SetPulseFX(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetPulseFX not implemented", false);
		}

		void HECmd_SetCOD7DecodeFX(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetCOD7DecodeFX not implemented", false);
		}

		void HECmd_SetPlayerNameString(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetPlayerNameString not implemented", false);
		}

		void HECmd_ChangeFontScaleOverTime(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_ChangeFontScaleOverTime not implemented", false);
		}


		std::array actsGscDebugFunctions{
			bo4::BuiltinFunctionDef{ // NewDebugHudElem(owner) -> helem
				.canonId = hash::HashT89Scr("NewDebugHudElem"),
				.min_args = 0,
				.max_args = 1,
				.actionFunc = GScr_NewDebugHudElem,
				.type = bo4::BUILTIN_DEFAULT, // fixme: BUILTIN_DEV
			},
		};
		std::array actsGscDebugMethods{
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("settext"),
				.min_args = 1,
				.max_args = 0x7FFFFFFF,
				.actionFunc = HECmd_SetText,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setshader"),
				.min_args = 1,
				.max_args = 3,
				.actionFunc = HECmd_SetMaterial,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("settargetent"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_SetTargetEnt,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("cleartargetent"),
				.min_args = 0,
				.max_args = 0,
				.actionFunc = HECmd_ClearTargetEnt,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("settimer"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_SetTimer,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("settenthstimer"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_SetTenthsTimer,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setvalue"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_SetValue,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setwaypoint"),
				.min_args = 1,
				.max_args = 4,
				.actionFunc = HECmd_SetWaypoint,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("fadeovertime"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_FadeOverTime,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("scaleovertime"),
				.min_args = 3,
				.max_args = 3,
				.actionFunc = HECmd_ScaleOverTime,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("moveovertime"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_MoveOverTime,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("destroy"),
				.min_args = 0,
				.max_args = 0,
				.actionFunc = HECmd_Destroy,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setpulsefx"),
				.min_args = 3,
				.max_args = 3,
				.actionFunc = HECmd_SetPulseFX,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setcod7decodefx"),
				.min_args = 3,
				.max_args = 3,
				.actionFunc = HECmd_SetCOD7DecodeFX,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("setplayernamestring"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_SetPlayerNameString,
				.type = bo4::BUILTIN_DEFAULT
			},
			bo4::BuiltinFunctionDef{
				.canonId = hash::HashT89Scr("changefontscaleovertime"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = HECmd_ChangeFontScaleOverTime,
				.type = bo4::BUILTIN_DEFAULT
			},
		};

		bool R_IsInit() {
			return bo4::sharedUiInfo->assets.whiteMaterial && bo4::sharedUiInfo->assets.bigFont;
		}

		void RenderFrame() {
			if (!R_IsInit()) {
				return;
			}
			const bo4::ScreenPlacement* scrPlace{ bo4::ScrPlace_GetView(bo4::LOCAL_CLIENT_0) };
			bo4::Font* font{ bo4::sharedUiInfo->assets.bigFont };
			
			for (ScrHudElem& elem : dbgData.huds) {
				if (!elem.allocated) {
					continue;
				}

				const char* text{ elem.text };

				if (!text) {
					continue;
				}


				uint32_t w{ bo4::UI_TextWidth(bo4::LOCAL_CLIENT_0, text, INT_MAX, font, elem.fontScale) };
				uint32_t h{ bo4::UI_TextHeight(font, elem.fontScale) };

				float x{ elem.x };
				float y{ elem.y };

				// horzalign/vertalign = alignement on the screen
				// alignx/y = alignment of the text

				switch (elem.horzalign) {
				case HHA_SUBLEFT: x = (x * scrPlace->scaleVirtualToReal[0]); break;
				case HHA_LEFT: x = (x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realViewableMin[0]; break;
				case HHA_CENTER: x = ((x * scrPlace->scaleVirtualToReal[0]) + (scrPlace->realViewportSize[0] * elem.fontScale)) + scrPlace->realViewportBase[0]; break;
				case HVH_RIGHT: x = (x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realViewableMax[0]; break;
				case HHA_FULLSCREEN: x = (x * scrPlace->scaleVirtualToFull[0]) + scrPlace->realViewportBase[0]; break;
				case HHA_NOSCALE: x = x; break;
				case HHA_ALIGNTO640: x = x * scrPlace->scaleRealToVirtual[0]; break;
				case HHA_CENTER_SAFEAREA: x = (x * scrPlace->scaleVirtualToReal[0]) + ((scrPlace->realViewableMin[0] + scrPlace->realViewableMax[0]) * elem.fontScale); break;
				case HHA_USER_LEFT: x = (x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realTweakableMin[0];; break;
				case HHA_USER_CENTER: x = (x * scrPlace->scaleVirtualToReal[0]) + ((scrPlace->realTweakableMin[0] + scrPlace->realTweakableMax[0]) * elem.fontScale); break;
				case HHA_USER_RIGHT: x = (x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realTweakableMax[0]; break;
				default: x = x; break;
				}
				switch (elem.vertalign) {
				case HVA_SUBTOP: y = (y * scrPlace->scaleVirtualToReal[0]); break;
				case HVA_TOP: y = (y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realViewableMin[1]; break;
				case HVA_MIDDLE: y = ((y * scrPlace->scaleVirtualToReal[1]) + (scrPlace->realViewportSize[1] * elem.fontScale)) + scrPlace->realViewportBase[1]; break;
				case HVA_BOTTOM: y = (y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realViewableMax[1]; break;
				case HVA_FULLSCREEN: y = (y * scrPlace->scaleVirtualToFull[1]) + scrPlace->realViewportBase[1]; break;
				case HVA_NOSCALE: y = y; break;
				case HVA_ALIGNTO480: y = y * scrPlace->scaleRealToVirtual[1]; break;
				case HVA_CENTER_SAFEAREA: y = (y * scrPlace->scaleVirtualToReal[1]) + ((scrPlace->realViewableMin[1] + scrPlace->realViewableMax[1]) * elem.fontScale); break;
				case HVA_USER_TOP: y = (y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realTweakableMin[1];; break;
				case HVA_USER_CENTER: y = (y * scrPlace->scaleVirtualToReal[1]) + ((scrPlace->realTweakableMin[1] + scrPlace->realTweakableMax[1]) * elem.fontScale); break;
				case HVA_USER_BOTTOM: y = (y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realTweakableMax[1]; break;
				default: y = x; break;
				}

				switch (elem.alignx) {
				case HXA_LEFT: break;
				case HXA_CENTER: x -= w / 2.0f; break;
				case HXH_RIGHT: x -= w; break;
				default: x = x; break;
				}
				switch (elem.aligny) {
				case HYA_TOP: y += h; break;
				case HYA_MIDDLE: y += h / 2.0f; break;
				case HYA_BOTTOM: break;
				default: y = x; break;
				}

				bo4::vec4_t color{};
				color[0] = elem.color[0];
				color[1] = elem.color[1];
				color[2] = elem.color[2];
				color[3] = elem.alpha;

				bo4::R_AddCmdDrawText(text, INT_MAX, font, x, y, elem.fontScale, elem.fontScale, 0, color, bo4::ITEM_TEXTSTYLE_NORMAL);
			}

			/*

			uint32_t height{ bo4::UI_TextHeight(bo4::sharedUiInfo->assets.bigFont, 0.4f) };

			bo4::R_AddCmdDrawText(
				utils::va("acts %s", core::actsinfo::VERSION), 
				128, 
				bo4::sharedUiInfo->assets.bigFont, 
				screen.realViewportBase[0] + 15, screen.realViewportBase[1] + 15 + height,
				0.4f, 0.4f, 0.0f, colorWhite, bo4::ITEM_TEXTSTYLE_NORMAL
			);
			*/
		}

		void Scr_ResetLinkInfo(bo4::scriptInstance_t inst) {
			if (inst == bo4::scriptInstance_t::SI_SERVER) {
				std::memset(dbgData.huds, 0, sizeof(dbgData.huds));
				dbgData.alloc.FreeAll();
			}
		}

		hook::library::Detour ScrVar_InitClassMap_Detour;
		hook::library::Detour Scr_SetObjectField_Detour;
		hook::library::Detour Scr_GetObjectField_Detour;


		bool Scr_SetObjectField_Stub(bo4::ClassNum classnum, bo4::EntRefUnion entRefUnion, uint32_t offset) {
			if (classnum == bo4::CLASS_NUM_HUDELEM) {
				// LOG_TRACE("Scr_SetObjectField({}, {}, {})", magic_enum::enum_name(classnum), entRefUnion.val, offset);
				if (offset >= hudFields.size()) {
					systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid Hud Elem offset: %d", false, offset);
					return true;
				}
				uint32_t hudElemIndex{ entRefUnion.hudElemIndex };
				ScrHudElem* hud{ GetHudElem(hudElemIndex) };
				if (!hud) {
					systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid Hud Elem index: %d", false, hudElemIndex);
					return true;
				}
				HudElemField* field{ &hudFields[offset] };

				if (field->readOnly) {
					systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Field %s is readonly", false, core::hashes::ExtractTmp("var", field->canonId));
					return true;
				}

				if (field->setter) {
					field->setter(hud, field);
				}
				else {
					bo4::Scr_SetGenericField(bo4::scriptInstance_t::SI_SERVER, (byte*)hud, field->type, field->ofs);
				}

				return true;
			}

			return Scr_SetObjectField_Detour.Call<bool>(classnum, entRefUnion, offset);
		}

		void Scr_GetObjectField_Stub(bo4::ClassNum classnum, bo4::EntRefUnion entRefUnion, uint32_t offset) {
			if (classnum == bo4::CLASS_NUM_HUDELEM) {
				// LOG_TRACE("Scr_GetObjectField({}, {}, {})", magic_enum::enum_name(classnum), entRefUnion.val, offset);
				if (offset >= hudFields.size()) {
					systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid Hud Elem offset: %d", false, offset);
					return;
				}
				uint32_t hudElemIndex{ entRefUnion.hudElemIndex };
				ScrHudElem* hud{ GetHudElem(hudElemIndex) };
				if (!hud) {
					systems::errors::ScrVm_Error(bo4::scriptInstance_t::SI_SERVER, "Invalid Hud Elem index: %d", false, hudElemIndex);
					return;
				}
				HudElemField* field{ &hudFields[offset] };


				if (field->getter) {
					field->getter(hud, field);
				}
				else {
					bo4::Scr_GetGenericField(bo4::scriptInstance_t::SI_SERVER, (byte*)hud, field->type, field->ofs);
				}

				return;
			}

			Scr_GetObjectField_Detour.Call(classnum, entRefUnion, offset);
		}


		void ScrVar_InitClassMap_Stub(bo4::scriptInstance_t inst, bo4::ClassNum classnum) {
			ScrVar_InitClassMap_Detour.Call(inst, classnum);

			if (inst == bo4::scriptInstance_t::SI_SERVER && classnum == bo4::CLASS_NUM_HUDELEM) {
				LOG_TRACE("ScrVar_InitClassMap({}, {}) {} -> add {} fields", inst ? "client" : "server", (int)classnum, hook::library::CodePointer{ _ReturnAddress() }, hudFields.size());
				for (size_t i = 0; i < hudFields.size(); i++) {
					bo4::ScrVar_AddClassFields(inst, classnum, hudFields[i].canonId, (int32_t)i);
				}
			}
		}

		void PostInit(uint64_t uid) {
			systems::gsc::funcs::ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_SERVER, false, actsGscDebugFunctions);
			systems::gsc::funcs::ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_SERVER, true, actsGscDebugMethods);

			ScrVar_InitClassMap_Detour.Create(0x275FF10_a, ScrVar_InitClassMap_Stub);
			Scr_SetObjectField_Detour.Create(0x2D72140_a, Scr_SetObjectField_Stub);
			Scr_GetObjectField_Detour.Create(0x2D6FBC0_a, Scr_GetObjectField_Stub);

			systems::events::EVENT_RESET_LINKS.Callback(Scr_ResetLinkInfo);
			systems::events::EVENT_RENDERER_ENDFRAME.Callback(RenderFrame);
		}

		REGISTER_SYSTEM(gsc_debug, nullptr, PostInit);
	}

	const char* RenderHudElemField(uint32_t element) {
		ScrHudElem* he{ GetHudElem(element) };
		if (!he) {
			return "HudElem[invalid]";
		}
		return utils::va("HudElem%u[x=%f y=%f z=%f fs=%f rgba=(%f, %f, %f, %f)] %s", GetHudElemId(he), he->x, he->y, he->z, he->fontScale, he->color[0], he->color[1], he->color[2], he->alpha, he->text ? he->text : "<empty>");
	}
}