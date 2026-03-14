#pragma once
#include <data/bo4.hpp>

namespace systems::errors {
	constexpr uint64_t CUSTOM_ERROR_ID = 0x53544341;
	void ScrVm_Error(bo4::scriptInstance_t inst, const char* msg, bool terminal, ...);
}