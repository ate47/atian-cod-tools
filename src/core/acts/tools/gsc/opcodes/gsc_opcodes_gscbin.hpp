#pragma once

namespace tool::gsc::opcode {
	class OpCodeGscBinRegistry {
	public:
		static void OpCode() {
			VmInfo* gscbin = RegisterVM(VMI_IW_GSCBIN, "GSCBIN", "gscbin", "gscbin", VmFlags::VMF_GSCBIN | VmFlags::VMF_NO_VERSION);
			gscbin->RegisterVmName("gscbin");
			gscbin->AddPlatform(PLATFORM_PC);

		}
	};
}