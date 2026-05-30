#pragma once
#include <data/bo4.hpp>

namespace systems::gsc::funcs {
	uint32_t ScrVm_GetHashScr(bo4::scriptInstance_t inst, unsigned int index);
	void ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t inst, bool isMethods, bo4::BuiltinFunctionDef* array, size_t len);

	template<size_t Count>
	void ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t inst, bool isMethods, std::array<bo4::BuiltinFunctionDef, Count>& array) {
		ScrVm_RegisterFunctionContainer(inst, isMethods, array.data(), array.size());
	}
}