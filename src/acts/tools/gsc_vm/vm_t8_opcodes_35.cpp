#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* vt835 = RegisterVM(VMI_T835, "Call of Duty: Black ops 4 (35)", "t8", "bo4_35", VmFlags::VMF_OPCODE_U16 | VmFlags::VMF_ALIGN | VmFlags::VMF_INV_ADD_TO_OBJECT | VmFlags::VMF_CLIENT_VM);
		vt835->RegisterVmName("t8_35", "blackops4_35");
		vt835->AddPlatform(PLATFORM_PC);
		vt835->SetMaxOpCode(0xFFF);
		vt835->RegisterVMGlobalVariable("level");
		vt835->RegisterVMGlobalVariable("game");
		vt835->RegisterVMGlobalVariable("classes");
		vt835->RegisterVMGlobalVariable("mission");
		vt835->RegisterVMGlobalVariable("anim");
		vt835->RegisterVMGlobalVariable("world");
		vt835->RegisterVMGlobalVariable("sharedstructs");
		vt835->RegisterVMGlobalVariable("memory");
		vt835->RegisterVMOperatorFunction("profilestart", "profilestart()", OPCODE_ProfileStart, VPFD_NONE, 0, 0);
		vt835->RegisterVMOperatorFunction("profilestop", "profilestop()", OPCODE_ProfileStop, VPFD_NONE, 0, 0);
		vt835->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		vt835->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		vt835->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		vt835->RegisterVMOperatorFunction("endoncallback", "<caller> endoncallback(func, event+)", OPCODE_EndOnCallback, VPFD_SELF_PARAM | VPFD_USE_COUNT, 2);
		vt835->RegisterVMOperatorFunction("vectorscale", "vectorscale(vector, factor) -> vector", OPCODE_VectorScale, VPFD_RETURN_VALUE, 2, 2);
		vt835->RegisterVMOperatorFunction("waittill", "<caller> waittill(event) -> struct", OPCODE_WaitTill, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 1);
		vt835->RegisterVMOperatorFunction("waittillmatch", "<caller> waittillmatch(event, match) -> struct", OPCODE_WaitTillMatch, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 2);
		vt835->RegisterVMOperatorFunction("waittillmatchtimeout", "<caller> waittillmatchtimeout(event, match, timeout) -> struct", OPCODE_WaitTillMatchTimeout, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 3);
		vt835->RegisterVMOperatorFunction("waittilltimeout", "<caller> waittilltimeout(event, timeout) -> struct", OPCODE_WaittillTimeout, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 2);
		vt835->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		vt835->RegisterVMOperatorFunction("waitframe", "waitframe(frames)", OPCODE_WaitFrame, VPFD_NONE, 1, 1);
		vt835->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		vt835->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64(str); });
		vt835->RegisterVMHashOPCode('&', OPCODE_GetHash, 8, [](const char* str) { return hash::HashT89Scr(str); });
		vt835->RegisterDevCall("assert", "assertmsg", "errormsg", "throw", "println");
		vt835->RegisterDatatype("functionptr", "scriptfunctionptr", "codefunctionptr", "string", "array", "weapon", "int", "float", "vec", "class", "struct", "hash");
		vt835->RegisterDatatypeRenamed("function", "functionptr");
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_Abort, 0x0);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0x1);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_Breakpoint, 0x2);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_AutoBreakpoint, 0x3);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_ErrorBreakpoint, 0x4);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_WatchBreakpoint, 0x5);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_NotifyBreakpoint, 0x6);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_ThreadEndBreakpoint, 0x7);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCachedDebug, 0x8);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCachedDebug, 0x9);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_PushVar, 0xa);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_PushEntityVar, 0xb);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0xc);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0xd);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0xe);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0xf);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0x10);
		vt835->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x11);

	}
}
REGISTER_GSC_VM_OPCODES(bo4_35, OpCode);