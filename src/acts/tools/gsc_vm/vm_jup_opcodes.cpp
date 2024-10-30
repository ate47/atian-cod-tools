#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		RegisterVM(VM_MW23, "Call of Duty: Modern Warfare III", "jup", "mwiiia", VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		RegisterVmName(VM_MW23, "jupa", "s5a", "mwiiia", "modernwarfareiiia", "mw23a");
		RegisterVMPlatform(VM_MW23, PLATFORM_PC);
		RegisterSameCodePlatform(VM_MW23, PLATFORM_PC, PLATFORM_PLAYSTATION);
		RegisterVMGlobalVariable(VM_MW23, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_MW23, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_MW23, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_MW23, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_MW23, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_MW23, "endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		RegisterVMOperatorFunction(VM_MW23, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_MW23, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_MW23, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_MW23, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		RegisterVMHashOPCode(VM_MW23, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_MW23, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_MW23, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_MW23, '&', OPCODE_GetHash, 8, [](const char* str) { return hashutils::HashJupScr(str); });
		RegisterDevCall(VM_MW23, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_MW23, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VM_MW23, PLATFORM_PC, OPCODE_CheckClearParams, 0x43);
		RegisterOpCode(VM_MW23, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x98);
		RegisterOpCode(VM_MW23, PLATFORM_PC, OPCODE_IW_RegisterVariable, 0x2B, 0xA4);

		RegisterVM(VM_MW23B, "Call of Duty: Modern Warfare III (8B)", "jup8b", "mwiii", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS);
		RegisterVmName(VM_MW23B, "jup", "s5", "mwiii", "modernwarfareiii", "mw23");
		RegisterVMPlatform(VM_MW23B, PLATFORM_PC);
		RegisterVMGlobalVariable(VM_MW23B, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_MW23B, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_MW23B, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_MW23B, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_MW23B, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23B, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_MW23B, "endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		RegisterVMOperatorFunction(VM_MW23B, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23B, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_MW23B, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_MW23B, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_MW23B, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_MW23B, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		RegisterVMHashOPCode(VM_MW23B, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_MW23B, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_MW23B, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_MW23B, '&', OPCODE_GetHash, 8, [](const char* str) { return hashutils::HashJupScr(str); });
		RegisterDevCall(VM_MW23B, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_MW23B, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VM_MW23B, PLATFORM_PC, OPCODE_CheckClearParams, 0x39);
		RegisterOpCode(VM_MW23B, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x4d);
		RegisterOpCode(VM_MW23B, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x79);

	}


}
REGISTER_GSC_VM_OPCODES(mwiii, OpCode);