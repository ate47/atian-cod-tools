#pragma once

namespace systems::gsc::link {
	byte* Scr_FindDetour(bo4::scriptInstance_t inst, byte* origin, uint64_t script, uint32_t nsp, uint32_t func);
}