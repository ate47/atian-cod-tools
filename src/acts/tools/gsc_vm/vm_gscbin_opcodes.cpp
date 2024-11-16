#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		VmInfo* gscbin = RegisterVM(VMI_IW_GSCBIN, "GSCBIN", "gscbin", "gscbin", VmFlags::VMF_NONE);
		gscbin->RegisterVmName("gscbin");
		gscbin->AddPlatform(PLATFORM_PC);

	}


}
REGISTER_GSC_VM_OPCODES(gscbin, OpCode);