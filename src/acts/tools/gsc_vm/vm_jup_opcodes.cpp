#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		RegisterVM(VMI_JUP_8A, "Call of Duty: Modern Warfare III", "jup", "mwiiia", VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		RegisterVmName(VMI_JUP_8A, "jupa", "s5a", "mwiiia", "modernwarfareiiia", "mw23a");
		RegisterVMPlatform(VMI_JUP_8A, PLATFORM_PC);
		RegisterSameCodePlatform(VMI_JUP_8A, PLATFORM_PC, PLATFORM_PLAYSTATION);
		RegisterVMGlobalVariable(VMI_JUP_8A, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_JUP_8A, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_JUP_8A, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_JUP_8A, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_JUP_8A, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_JUP_8A, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8A, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_JUP_8A, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		RegisterVMHashOPCode(VMI_JUP_8A, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_JUP_8A, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_JUP_8A, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_JUP_8A, '&', OPCODE_GetHash, 8, [](const char* str) { return hash::HashJupScr(str); });
		RegisterDevCall(VMI_JUP_8A, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_JUP_8A, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VMI_JUP_8A, PLATFORM_PC, OPCODE_CheckClearParams, 0x43);
		RegisterOpCode(VMI_JUP_8A, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x98);
		RegisterOpCode(VMI_JUP_8A, PLATFORM_PC, OPCODE_IW_RegisterVariable, 0x2B, 0xA4);

		RegisterVM(VMI_JUP_8B, "Call of Duty: Modern Warfare III (8B)", "jup8b", "mwiii", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		RegisterVmName(VMI_JUP_8B, "jup", "s5", "mwiii", "modernwarfareiii", "mw23");
		RegisterVMPlatform(VMI_JUP_8B, PLATFORM_PC);
		RegisterVMGlobalVariable(VMI_JUP_8B, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_JUP_8B, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_JUP_8B, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_JUP_8B, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_JUP_8B, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_JUP_8B, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_JUP_8B, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_JUP_8B, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		RegisterVMHashOPCode(VMI_JUP_8B, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_JUP_8B, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_JUP_8B, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_JUP_8B, '&', OPCODE_GetHash, 8, [](const char* str) { return hash::HashJupScr(str); });
		RegisterDevCall(VMI_JUP_8B, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_JUP_8B, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VMI_JUP_8B, PLATFORM_PC, OPCODE_CheckClearParams, 0x39);
		RegisterOpCode(VMI_JUP_8B, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x4d);
		RegisterOpCode(VMI_JUP_8B, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x79);

	}


}
REGISTER_GSC_VM_OPCODES(mwiii, OpCode);