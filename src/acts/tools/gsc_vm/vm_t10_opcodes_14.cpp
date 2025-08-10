#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* t1014 = RegisterVM(VMI_T10_14, "Call of Duty: Black Ops 6", "t10", "bo6", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW_KEYS | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_HASH_CER | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_LIKE | VmFlags::VMF_ISTRING_HASHED); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t1014->RegisterVmName("cer", "t10", "blackops6");
		t1014->AddPlatform(PLATFORM_PC);
		t1014->AddPlatform(PLATFORM_OLD);
		t1014->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t1014->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t1014->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t1014->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t1014->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t1014->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t1014->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t1014->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t1014->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t1014->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t1014->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t1014->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t1014->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t1014->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t1014->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t1014->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWAsset(str); });
		t1014->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashX32(str); });
		t1014->RegisterVMHashOPCode('s', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		t1014->RegisterDevCall("assert", "assertmsg", "function_79901b4637c83c86", "assertex", "println", "print");
		t1014->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector", "xhash", "xhashasset", "istring");
		t1014->RegisterOpCode(PLATFORM_OLD, OPCODE_CheckClearParams, 0x31);
		t1014->RegisterOpCode(PLATFORM_OLD, OPCODE_SafeCreateLocalVariables, 0x5f);
		t1014->RegisterOpCode(PLATFORM_OLD, OPCODE_IW_RegisterMultipleVariables, 0x8f);

		t1014->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x0b);
		t1014->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x56);
		t1014->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x6a);
	}
}
REGISTER_GSC_VM_OPCODES(bo6, OpCode);