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

namespace systems::gsc::debug {
	namespace {
		bo4::vec4_t colorWhite{ 1, 1, 1, 1 };

		struct {
			bool vmStarted{};
		} dbgData{};
		
		void GScr_NewDebugHudElem(bo4::scriptInstance_t inst) {
			LOG_WARNING("NewDebugHudElem not implemented");
			bo4::ScrVm_AddStruct(inst); // create empty struct
		}

		void HECmd_SetText(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetText not implemented");
		}

		void HECmd_SetMaterial(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetMaterial not implemented");
		}

		void HECmd_SetTargetEnt(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetTargetEnt not implemented");
		}

		void HECmd_ClearTargetEnt(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_ClearTargetEnt not implemented");
		}

		void HECmd_SetTimer(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetTimer not implemented");
		}

		void HECmd_SetTenthsTimer(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetTenthsTimer not implemented");
		}

		void HECmd_SetValue(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetValue not implemented");
		}

		void HECmd_SetWaypoint(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetWaypoint not implemented");
		}

		void HECmd_FadeOverTime(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_FadeOverTime not implemented");
		}

		void HECmd_ScaleOverTime(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_ScaleOverTime not implemented");
		}

		void HECmd_MoveOverTime(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_MoveOverTime not implemented");
		}

		void HECmd_Destroy(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_Destroy not implemented");
		}

		void HECmd_SetPulseFX(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetPulseFX not implemented");
		}

		void HECmd_SetCOD7DecodeFX(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetCOD7DecodeFX not implemented");
		}

		void HECmd_SetPlayerNameString(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_SetPlayerNameString not implemented");
		}

		void HECmd_ChangeFontScaleOverTime(bo4::scriptInstance_t inst, void* ref) {
			LOG_WARNING("HECmd_ChangeFontScaleOverTime not implemented");
		}


		std::array actsGscDebugFunctions{
			bo4::BuiltinFunctionDef{ // NewDebugHudElem(owner) -> helem
				.canonId = hash::HashT89Scr("NewDebugHudElem"),
				.min_args = 0,
				.max_args = 1,
				.actionFunc = GScr_NewDebugHudElem,
				.type = bo4::BUILTIN_DEV,
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
			return bo4::sharedUiInfo->assets.whiteMaterial && bo4::sharedUiInfo->assets.bigFont && dbgData.vmStarted;
		}

		void RenderPostFrame() {
			if (!R_IsInit()) {
				return;
			}

			//bo4::R_AddCmdDrawText(utils::va("acts %s", core::actsinfo::VERSION), 128, bo4::sharedUiInfo->assets.textFont, 0, 2, 1, 1, 0, colorWhite, 0);
		}

		void Scr_ResetLinkInfo(bo4::scriptInstance_t inst) {
			dbgData.vmStarted = true;
		}

		void PostInit(uint64_t uid) {
			systems::gsc::funcs::ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_SERVER, false, actsGscDebugFunctions);
			systems::gsc::funcs::ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_SERVER, true, actsGscDebugMethods);

			systems::events::EVENT_RESET_LINKS.Callback(Scr_ResetLinkInfo);
			systems::events::EVENT_RENDERER_ENDFRAME.Callback(RenderPostFrame);
		}

		REGISTER_SYSTEM(gsc_debug, nullptr, PostInit);
	}
}