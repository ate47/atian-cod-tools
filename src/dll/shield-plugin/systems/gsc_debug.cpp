#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/actsinfo.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <systems/gsc_funcs.hpp>
#include <systems/events.hpp>
#include <systems/errors.hpp>

namespace systems::gsc::debug {
	namespace {
		bo4::vec4_t colorWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

		bo4::vec4_t& GetColor(uint32_t hex) {
			static thread_local struct {
				size_t idx{};
				bo4::vec4_t buff[16];
			} tmp{};

			bo4::vec4_t& v{ tmp.buff[tmp.idx] };
			tmp.idx = (tmp.idx + 1) % ACTS_ARRAYSIZE(tmp.buff);

			// 0xRRGGBBAA
			v[3] = (hex & 0xFF) / 255.0f;
			v[2] = ((hex << 8) & 0xFF) / 255.0f;
			v[1] = ((hex << 16) & 0xFF) / 255.0f;
			v[0] = ((hex << 24) & 0xFF) / 255.0f;

			return v;
		}


		enum HudVAlign {
			HVA_SUBTOP, HVA_TOP, HVA_MIDDLE, HVA_BOTTOM
		};
		std::array HudVAlignNames{ "subtop", "top", "middle", "bottom" };


		enum HudHAlign {
			HHA_SUBLEFT, HHA_LEFT, HHA_CENTER, HVH_RIGHT
		};
		std::array HudHAlignNames{ "subleft", "left", "center", "right" };

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

		enum HudElemType {
			HET_FREE = 0,
			HET_ALLOC = 1,
		};

		struct ScrHudElem {
			float x;
			float y;
			float z;
			float fontScale;
			float fromFontScale;
			HudVAlign valign;
			HudHAlign halign;
			bo4::vec4_t color;
			HudElemType type;
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

			__CREATE_INVALID_HUDELEM_FIELD(font),
			__CREATE_INVALID_HUDELEM_FIELD(alignx),
			__CREATE_INVALID_HUDELEM_FIELD(aligny),
			__CREATE_INVALID_HUDELEM_FIELD(horzalign),
			__CREATE_INVALID_HUDELEM_FIELD(vertalign),
			__CREATE_INVALID_HUDELEM_FIELD(color),
			__CREATE_INVALID_HUDELEM_FIELD(alpha),
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
		} dbgData{};

		ScrHudElem* AllocHudElem() {
			for (ScrHudElem& elem : dbgData.huds) {
				if (!elem.type) {
					std::memset(&elem, 0, sizeof(elem));
					elem.type = HET_ALLOC;
					return &elem;
				}
			}
			return nullptr; // no empty slot
		}

		ScrHudElem* GetHudElem(size_t idx) {
			if (idx >= numMaxHudElements || dbgData.huds[idx].type == HET_FREE) {
				return nullptr; // invalid / free id
			}
			return &dbgData.huds[idx];
		}

		ScrHudElem* GetHudElem(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			if (ref->classnum != bo4::CLASS_NUM_HUDELEM) {
				systems::errors::ScrVm_Error(inst, "not a hud element", false);
			}

			ScrHudElem* he{ GetHudElem(ref->u.hudElemIndex) };

			if (!he || he->type == HET_FREE) {
				systems::errors::ScrVm_Error(inst, "not a valid hud element", false);
				return nullptr;
			}
			return he;
		}

		int32_t GetHudElemId(ScrHudElem* hud) {
			if (!hud) {
				return numMaxHudElements;
			}
			return (int32_t)(hud - dbgData.huds);
		}

		
		void GScr_NewDebugHudElem(bo4::scriptInstance_t inst) {
			// TODO: param[0] = player

			ScrHudElem* hud{ AllocHudElem() };

			if (!hud) {
				systems::errors::ScrVm_Error(inst, "out of hudelems", false);
				return;
			}

			bo4::EntRefUnion v{ .entnum = GetHudElemId(hud) };
			bo4::ScrVm_AddEntityNum(inst, v, bo4::CLASS_NUM_HUDELEM, bo4::LOCAL_CLIENT_0);
		}

		void HECmd_Destroy(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (he) {
				he->type = HET_FREE;
			}

			bo4::ScrEvent_FireCallbackEventEnt(inst, ref->u.hudElemIndex, bo4::CLASS_NUM_HUDELEM, false);
			//ScrVar_FreeEntityNum(inst, ref->u, bo4::CLASS_NUM_HUDELEM, bo4::LOCAL_CLIENT_0);
			systems::errors::ScrVm_Error(inst, "HECmd_Destroy not implemented", false);
		}

		void HECmd_SetText(bo4::scriptInstance_t inst, bo4::scr_entref_t* ref) {
			ScrHudElem* he{ GetHudElem(inst, ref) };
			if (!he) return;
			systems::errors::ScrVm_Error(inst, "HECmd_SetText not implemented", false);
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

			/*
			const bo4::ScreenPlacement& screen{ *bo4::ScrPlace_GetView(bo4::LOCAL_CLIENT_0) };

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
			std::memset(dbgData.huds, 0, sizeof(dbgData.huds));
		}

		hook::library::Detour ScrVar_InitClassMap_Detour;
		hook::library::Detour Scr_SetObjectField_Detour;
		hook::library::Detour Scr_GetObjectField_Detour;


		bool Scr_SetObjectField_Stub(bo4::ClassNum classnum, bo4::EntRefUnion entRefUnion, uint32_t offset) {
			if (classnum == bo4::CLASS_NUM_HUDELEM) {
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
					bo4::Scr_SetGenericField(bo4::scriptInstance_t::SI_SERVER, (byte*)field, field->type, field->ofs);
				}

				return true;
			}

			return Scr_SetObjectField_Detour.Call<bool>(classnum, entRefUnion, offset);
		}

		void Scr_GetObjectField_Stub(bo4::ClassNum classnum, bo4::EntRefUnion entRefUnion, uint32_t offset) {
			if (classnum == bo4::CLASS_NUM_HUDELEM) {
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
					bo4::Scr_GetGenericField(bo4::scriptInstance_t::SI_SERVER, (byte*)field, field->type, field->ofs);
				}

				return;
			}

			Scr_GetObjectField_Detour.Call(classnum, entRefUnion, offset);
		}


		void ScrVar_InitClassMap_Stub(bo4::scriptInstance_t inst, bo4::ClassNum classnum) {
			ScrVar_InitClassMap_Detour.Call(inst, classnum);

			if (inst == bo4::scriptInstance_t::SI_SERVER && classnum == bo4::CLASS_NUM_HUDELEM) {
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
}