#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* vt71a = RegisterVM(VMI_T7_1A, "Call of Duty: Black ops 3 (1A)", "t7", "bo3_1a", VmFlags::VMF_CLIENT_VM | VmFlags::VMF_NO_FILE_NAMESPACE | VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16 | VmFlags::VMF_ANIMTREE_T7 | VmFlags::VMF_HASH_T7 | VmFlags::VMF_ISTRING | VmFlags::VMF_UNIQUE_HASH); // | VmFlags::VMF_CALL_NO_PARAMS
		vt71a->RegisterVmName("t7_1a", "blackops3_1a");
		vt71a->AddPlatform(PLATFORM_PC);
		vt71a->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		vt71a->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		vt71a->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		vt71a->RegisterVMGlobalVariable("world", OPCODE_GetWorld);
		vt71a->RegisterVMGlobalVariable("classes", OPCODE_GetClasses);
		vt71a->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		vt71a->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		vt71a->RegisterVMOperatorFunction("endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		vt71a->RegisterVMOperatorFunction("vectorscale", "vectorscale(vector, factor) -> vector", OPCODE_VectorScale, VPFD_RETURN_VALUE, 2, 2);
		vt71a->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		// vt71a->RegisterVMOperatorFunction("waittillmatch", "<caller> waittillmatch(event, match) -> struct", OPCODE_WaitTillMatch2, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 2); // todo
		vt71a->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		vt71a->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		vt71a->RegisterVMHashOPCode('#', OPCODE_GetHash32, 4, [](const char* str) { return hash::HashT7(str); });
		vt71a->RegisterVMHashOPCode('s', OPCODE_GetHash32, 4, [](const char* str) { return hash::HashT7(str); });
		vt71a->RegisterDevCall("assert", "assertmsg", "errormsg", "throw", "println");
		vt71a->RegisterDatatype("functionptr", "string", "array", "weapon", "int", "float", "vec");
		vt71a->RegisterDatatypeRenamed("function", "functionptr");
		vt71a->SetCompilerHookFunctionName("isprofilebuild");
		vt71a->SetMaxOpCode(0x3FFF);
		vt71a->SetModToolFlag(0x2000);
		vt71a->SetGDBType(VMI_DBG_T7_12);

		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_Abort, 0x0);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0x1);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_Breakpoint, 0x2);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_AutoBreakpoint, 0x3);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_ErrorBreakpoint, 0x4);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_WatchBreakpoint, 0x5);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_NotifyBreakpoint, 0x6);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_ThreadEndBreakpoint, 0x7);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCachedDebug, 0x8);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCachedDebug, 0x9);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_PushVar, 0xa);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_PushEntityVar, 0xb);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0xc);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0xd);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0xe);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0xf);
		vt71a->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x10);
	}


}
REGISTER_GSC_VM_OPCODES(bo3_1a, OpCode);