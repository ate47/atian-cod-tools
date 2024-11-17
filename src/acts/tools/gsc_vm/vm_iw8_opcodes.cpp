#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* gscbin = RegisterVM(VMI_IW_BIN_MW19, "Call of Duty: Modern Warfare (2019)", "iw8", "mw19", VmFlags::VMF_GSCBIN | VmFlags::VMF_NO_MAGIC);
		gscbin->RegisterVmName("gb_3b");
		gscbin->AddPlatform(PLATFORM_PC);
		// GetOpaqueStringCount / Scr_InitStringConstants =  useReducedSpGoldLimits ? 0xE2C0 : 0x1472F 
		gscbin->SetOpaqueStringCount(0x1472F);
		gscbin->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x59);
	}


}
REGISTER_GSC_VM_OPCODES(gscbin, OpCode);