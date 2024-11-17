#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* gscbin = RegisterVM(VMI_IW_BIN_MW22, "Call of Duty: Modern Warfare II (2022)", "iw9", "mw22", VmFlags::VMF_GSCBIN | VmFlags::VMF_NO_MAGIC);
		gscbin->RegisterVmName("gb_32");
		gscbin->AddPlatform(PLATFORM_PC);

	}


}
REGISTER_GSC_VM_OPCODES(gscbin, OpCode);