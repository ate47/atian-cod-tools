#pragma once
#include <core/eventhandler.hpp>
#include <data/bo4.hpp>

namespace systems::events {
	constexpr core::eventhandler::EventInfo<bo4::scriptInstance_t> EVENT_RESET_LINKS = "Scr_ResetLinkInfo";
	constexpr core::eventhandler::EventInfo EVENT_RENDERER_ENDFRAME = "R_EndFrame";
}