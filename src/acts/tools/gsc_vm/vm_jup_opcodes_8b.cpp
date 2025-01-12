#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* v8b = RegisterVM(VMI_JUP_8B, "Call of Duty: Modern Warfare III (8B)", "jup8b", "mwiii", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_NO_PARAM_FLAGS | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_IW | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS | VmFlags::VMF_IW_CALLS | VmFlags::VMF_IW_LIKE);
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

		// gvars:
		// v15 + 16 = anim
		// v15 + 15 = game
		// v15 + 14 = level

		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_DevOp, 0x02);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x39);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x4d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0x69);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedShort, 0x74);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetSelfFieldVar, 0x6F);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetByte, 0x57);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0x3B);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetResolveFunction, 0x6c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVar, 0x1B);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x17);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0x38);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevel, 0x2f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Notify, 0x7B);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x63);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached0, 0x09);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached1, 0x0a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached2, 0x0b);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached3, 0x0c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached4, 0x0d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableCached5, 0x0e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x0f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelf, 0x3e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0x4f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleEndon, 0x81);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0x29);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0x5e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCall, 0x90);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, 0x76);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x79);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0x92);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, 0x25);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVar, 0x7c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_SetLocalVariableCached, 0xa2);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, 0x80);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, 0x3D);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, 0x12);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, 0x75);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, 0x45);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableObjectCached, 0x5F);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariable, 0x28);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, 0x41);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalArrayCached, 0x71);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, 0x99);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x84);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, 0x4a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0xa6);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArray, 0x5c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CastBool, 0x9b);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0x1a, 0x53);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x2d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CastFieldObject, 0x6e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0x43);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, 0x31);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, 0x56);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x26);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_IsTrue, 0x85);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCall, 0x89);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, 0x77);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetDVarHash, 0x20);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x64);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x95);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_SetVariableField, 0x67);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CreateArray, 0x4c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetLevelFieldVariable, 0x58);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariable, 0x14);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearFieldVariable, 0x1f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetVector, 0x34);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, 0x6a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArrayRef, 0x10);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallEndOn, 0x7d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x4b);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x19);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Inc, 0x96);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Dec, 0x13);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_AddToArray, 0x9e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Wait, 0x48);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Switch, 0x1d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EndSwitch, 0x86);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitFrame, 0x30);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Vector, 0x40);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0xa4);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_SizeOf, 0x5a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, 0x4e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0x3c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ClearArray, 0xa9);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_ClearFieldVariableRef, 0x72);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0x37);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalArrayCachedField, 0x3f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCall, 0x3a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash, 0x7a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetTagHash, 0x9a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetResourceHash, 0x21);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariableRef, 0x2e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevelGRef, 0xa8);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallEndOn, 0x8f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x52);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointer, 0xa5);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0x98);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointerEndOn, 0x91);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x33);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedInteger, 0xa7);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinFunction, 0x6d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinFunctionCallPointer, 0x73);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinMethod, 0x70);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinMethodCallPointer, 0x2a);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelfObject, 0x82);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x93);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGame, 0x9d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVarRef, 0x1c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x59);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitTillMatch, 0x8d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimation, 0x7e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftLeft, 0x5b);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftRight, 0x15);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedInteger, 0x8e);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_GetLongInteger, 0x2b);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetAnimFieldVar, 0x22);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVarRef, 0x55);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_And, 0x5d);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Or, 0x66);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Xor, 0x8c);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_BoolComplement, 0x36);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_CreateStruct, 0x16);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetThread, 0x8);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetIString, 0x18);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGameRef, 0x7f);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimationTree, 0x11);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_AddToStruct, 0xaa);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnim, 0x88);
		v8b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetEmptyUnkE, 0x9c);
	}


}
REGISTER_GSC_VM_OPCODES(mwiii_8b, OpCode);