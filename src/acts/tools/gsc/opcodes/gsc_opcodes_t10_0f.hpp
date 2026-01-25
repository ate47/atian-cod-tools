#pragma once
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>

namespace tool::gsc::opcode {
	class OpCodeT100FRegistry {
	public:
		static void OpCode() {
		VmInfo* t10f = RegisterVM(VMI_T10_0F, "Call of Duty: Black Ops 6 (0F)", "t10", "bo6_f", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_HASH_CER | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_LIKE | VmFlags::VMF_ISTRING_HASHED); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t10f->RegisterVmName("cerf", "t10_f", "blackops6_f");
		t10f->AddPlatform(PLATFORM_PC);
		t10f->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t10f->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t10f->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t10f->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t10f->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t10f->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t10f->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t10f->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t10f->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t10f->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t10f->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t10f->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t10f->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t10f->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t10f->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t10f->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWAsset(str); });
		t10f->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashX32(str); });
		t10f->RegisterVMHashOPCode('s', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10ScrSP(str); });
		t10f->RegisterDevCall("assert", "assertmsg", "assertex", "function_79901b4637c83c86", "println", "print");
		t10f->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
	}
	};
}