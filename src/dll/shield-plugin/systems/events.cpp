#include <dll_includes.hpp>
#include <systems/events.hpp>
#include <data/refs.hpp>
#include <hook/library.hpp>

namespace systems::events {

	namespace {
		hook::library::Detour R_EndFrame_Detour;
		hook::library::Detour Scr_ResetLinkInfo_Detour;

		void R_EndFrame_Stub() {
			systems::events::EVENT_RENDERER_ENDFRAME.Call();
			R_EndFrame_Detour.Call();
		}

		void Scr_ResetLinkInfo_Stub(bo4::scriptInstance_t inst) {
			bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };
			Scr_ResetLinkInfo_Detour.Call(inst);
			systems::events::EVENT_RESET_LINKS.Call(inst);
		}

		void PostInit(uint64_t uid) {
			R_EndFrame_Detour.Create(0x361E260_a, R_EndFrame_Stub);
			Scr_ResetLinkInfo_Detour.Create(0x2749480_a, Scr_ResetLinkInfo_Stub);
		}

		REGISTER_SYSTEM(events, nullptr, PostInit);
	}
}