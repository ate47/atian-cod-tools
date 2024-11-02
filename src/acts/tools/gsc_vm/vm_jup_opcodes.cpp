#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		VmInfo* v8a = RegisterVM(VMI_JUP_8A, "Call of Duty: Modern Warfare III", "jup", "mwiiia", VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		v8a->RegisterVmName("jupa", "s5a", "mwiiia", "modernwarfareiiia", "mw23a");
		v8a->AddPlatform(PLATFORM_PC);
		v8a->RegisterSameCodePlatform(PLATFORM_PC, PLATFORM_PLAYSTATION);
		v8a->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		v8a->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		v8a->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		v8a->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		v8a->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		v8a->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		v8a->RegisterVMOperatorFunction("endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		v8a->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		v8a->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		v8a->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		v8a->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		v8a->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		v8a->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		v8a->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		v8a->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		v8a->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		v8a->RegisterVMHashOPCode('&', OPCODE_GetHash, 8, [](const char* str) { return hash::HashJupScr(str); });
		v8a->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		v8a->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		v8a->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x43);
		v8a->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x98);
		v8a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterVariable, 0x2B, 0xA4);

		VmInfo* v8b = RegisterVM(VMI_JUP_8B, "Call of Duty: Modern Warfare III (8B)", "jup8b", "mwiii", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		v8b->RegisterVmName("jup", "s5", "mwiii", "modernwarfareiii", "mw23");
		v8b->AddPlatform(PLATFORM_PC);
		v8b->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		v8b->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		v8b->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		v8b->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		v8b->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		v8b->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		v8b->RegisterVMOperatorFunction("endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		v8b->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		v8b->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		v8b->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		v8b->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		v8b->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		v8b->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		v8b->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		v8b->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		v8b->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		v8b->RegisterVMHashOPCode('&', OPCODE_GetHash, 8, [](const char* str) { return hash::HashJupScr(str); });
		v8b->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		v8b->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x39);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x4d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x79);

	}


}
REGISTER_GSC_VM_OPCODES(mwiii, OpCode);