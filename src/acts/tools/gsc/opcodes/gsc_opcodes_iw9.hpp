#pragma once
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>

namespace tool::gsc::opcode {
class OpCodeIW9Registry {
public:
	static void OpCode() {
		VmInfo* gscbin = RegisterVM(VMI_IW_BIN_MW22, "Call of Duty: Modern Warfare II (2022)", "iw9", "mw22", VmFlags::VMF_GSCBIN | VmFlags::VMF_NO_MAGIC | VmFlags::VMF_VAR_ID | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_IW_LIKE);
		gscbin->AddPlatform(PLATFORM_PC);
		gscbin->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x32);
	}
};
}