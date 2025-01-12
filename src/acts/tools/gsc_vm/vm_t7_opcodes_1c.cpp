#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* vt71b = RegisterVM(VMI_T71B, "Call of Duty: Black ops 3 (1B)", "t7_1b", "bo3_1b", VmFlags::VMF_CLIENT_VM | VmFlags::VMF_NO_FILE_NAMESPACE | VmFlags::VMF_ALIGN | VmFlags::VMF_ANIMTREE_T7 | VmFlags::VMF_HASH_T7); // | VmFlags::VMF_CALL_NO_PARAMS
		vt71b->RegisterVmName("t7_1b", "blackops3_1b");
		vt71b->AddPlatform(PLATFORM_PC);
		vt71b->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		vt71b->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		vt71b->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		vt71b->RegisterVMGlobalVariable("world", OPCODE_GetWorld);
		vt71b->RegisterVMGlobalVariable("classes", OPCODE_GetClasses);
		vt71b->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		vt71b->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		vt71b->RegisterVMOperatorFunction("endon", "<caller> endon(event)", OPCODE_IW_SingleEndon, VPFD_SELF_PARAM, 1, 1);
		vt71b->RegisterVMOperatorFunction("vectorscale", "vectorscale(vector, factor) -> vector", OPCODE_VectorScale, VPFD_RETURN_VALUE, 2, 2);
		vt71b->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		// vt71b->RegisterVMOperatorFunction("waittillmatch", "<caller> waittillmatch(event, match) -> struct", OPCODE_WaitTillMatch2, VPFD_SELF_PARAM | VPFD_USE_COUNT | VPFD_RETURN_VALUE, 2); // todo
		vt71b->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		vt71b->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		vt71b->RegisterVMHashOPCode('#', OPCODE_GetHash32, 4, [](const char* str) { return hash::HashT7(str); });
		vt71b->RegisterDevCall("assert", "assertmsg", "errormsg", "throw", "println");
		vt71b->RegisterDatatype("functionptr", "string", "array", "weapon", "int", "float", "vec");
		vt71b->RegisterDatatypeRenamed("function", "functionptr");
		vt71b->SetMaxOpCode(0xFF);


		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Abort, 0x0);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0x1);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Breakpoint, 0x2);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_AutoBreakpoint, 0x3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ErrorBreakpoint, 0x4);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_WatchBreakpoint, 0x5);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_NotifyBreakpoint, 0x6);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ThreadEndBreakpoint, 0x7);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCachedDebug, 0x8);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCachedDebug, 0x9);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_PushVar, 0xa);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_PushEntityVar, 0xb);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0xc);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0xd);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0xe);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0xf);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x10);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x82);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetResolveFunction, 0xd9);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0x2a);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, 0xb4);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0xae);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariableRef, 0x11);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArrayRef, 0x33);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_SetVariableField, 0x29);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0xde);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalSelfFieldVariable, 0xe3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, 0x5b);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetSignedByte, 0xe2);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x76);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x5d);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, 0x18);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0x9d);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevel, 0xd4);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0x16);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, 0x7f);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0xac);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0xe5);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_CastFieldObject, 0xd1);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariable, 0xef);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelf, 0x91);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0x7b);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x26);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVarRef, 0x37);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelfObject, 0x24);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearFieldVariable, 0xf4);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, 0xdd);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash32, 0xed);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleEndon, 0x44);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, 0x67);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0x98);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetShort, 0xa7);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Wait, 0xc6);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetInteger, 0x23);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariable, 0xfa);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_FirstArrayKey, 0xb3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArray, 0x51);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_NextArrayKey, 0x17);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, 0xdb);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x42);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0x43);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, 0x4f);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, 0x3d);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x3b);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, 0xd6);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Switch, 0x71);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EndSwitch, 0x21);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x49);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Vector, 0x1f);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0xc0);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetIString, 0x4d);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_SizeOf, 0x9b);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_SuperNotEqual, 0x66);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Notify, 0x5a);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Inc, 0xc3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x56);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, 0x57);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0xf3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_SuperEqual, 0x28);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_VectorConstant, 0x95);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x89);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0x72);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_VectorScale, 0x48);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_EmptyArray, 0x1c);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, 0x40);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0xe6);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0xcc);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetTime, 0xa3);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevelGRef, 0x35);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, 0xad);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, 0xb0);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Dec, 0x9c);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCall, 0xe8);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ClassFunctionCall, 0x27);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x3f);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x3a);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimation, 0x83);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillMatch2, 0x1b);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Wait2, 0xb7);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearArray, 0x30);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_GetObjectType, 0x34);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_IgnoreWaittillVariableFieldCached, 0xa1);
		vt71b->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, 0x45);

	}


}
REGISTER_GSC_VM_OPCODES(bo3_1b, OpCode);