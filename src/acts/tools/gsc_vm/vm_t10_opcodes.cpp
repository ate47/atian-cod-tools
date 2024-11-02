#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		RegisterVM(VMI_T10_06, "Call of Duty: Black Ops 6 (06)", "t10_6", "bo6_6", VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VMI_T10_06, "cer6", "t10_6", "blackops6_6");
		RegisterVMPlatform(VMI_T10_06, PLATFORM_PC);
		RegisterVMGlobalVariable(VMI_T10_06, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_T10_06, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_T10_06, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_T10_06, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_06, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_06, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_06, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VMI_T10_06, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_T10_06, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VMI_T10_06, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_T10_06, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_06, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_06, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_T10_06, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		RegisterDevCall(VMI_T10_06, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_T10_06, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		// compatibility
		RegisterSameCodePlatform(VMI_T10_06, PLATFORM_PC, PLATFORM_PC_ALPHA);
		RegisterVMPlatform(VMI_T10_06, PLATFORM_PC_ALPHA);

		// some opcodes, not all of them, but enough for a compiler
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Nop, 0);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetHash, 0x2b); // 9
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetResourceHash, 0x9a); // 10
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetDVarHash, 0x72); // 11
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetTagHash, 0x57); // 12
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_T10_GetScrHash, 0x16); // 13
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetResourceHash2, 0x69);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0x80);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetThread, 0x07);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_T10_FlatArgs, 0x6a);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IsDefined, 0xa9);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CheckClearParams, 0x76);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_End, 0x17);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_PreScriptCall, 0x9c);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x21);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x41);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_DevblockBegin, 0x2e);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetZero, 0x5e);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetByte, 0x59);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptThreadCallEndOn, 0x4e, 0x5a); // wtf
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EndOn, 0x92);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Jump, 0x1f);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetAnim, 0x6d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CreateStruct, 0x1a);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_AddToStruct, 0x14);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetNegUnsignedInteger, 0x2a); // 20
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetLongInteger, 0x34);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SuperEqual, 0x1e);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SuperNotEqual, 0x4d); // wtf???
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_T10_GreaterThanOrSuperEqualTo, 0x79);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_T10_LowerThanOrSuperEqualTo, 0x6e);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CastFieldObject, 0x60);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x88);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SetVariableField, 0x49);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalFieldVariable, 0x82);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptThreadCall, 0x2d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_WaitFrame, 0x95);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GreaterThan, 0x13);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Multiply, 0x3b);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Minus, 0x84);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Divide, 0x1c);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Plus, 0x78);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_NotEqual, 0x29);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Return, 0x8d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetSelf, 0x36);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetLevel, 0x97);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Bit_Or, 0x1d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Bit_And, 0x85);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_LessThan, 0x77);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalArray, 0x58);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Inc, 0x63);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetUndefined, 0x22);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0x45);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetNegByte, 0x35);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x9d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0x37);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetUnsignedShort, 0x73);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetUnsignedInteger, 0x42);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_WaitTillMatch, 0x40);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ShiftLeft, 0x4a);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ShiftRight, 0x4f);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_BoolComplement, 0x53);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Bit_Xor, 0x26);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x12);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x1b);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptThreadCallPointerEndOn, 0x23);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_BuiltinMethodCallPointer, 0x8c);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_BuiltinFunctionCallPointer, 0x98);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ProfileStop, 0x8e);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ProfileStart, 0x90);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnDefined, 0x71);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnDefinedExpr, 0x74);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptFunctionCall, 0x32);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_AddToArray, 0x38);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetGame, 0x5d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CallBuiltinMethod, 0xa4);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SizeOf, 0x9b);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_Notify, 0xa2);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CallBuiltinFunction, 0x65);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnTrue, 0x66);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_CreateArray, 0xa8);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetString, 0x52);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_DecTop, 0x3d);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Equal, 0x39);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnFalse, 0x96);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetFloat, 0x68);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Wait, 0x6f);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x0f);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0x54);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_SetLocalVariableCached, 0x62);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalArrayRef, 0x94);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetResolveFunction, 0x55);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptMethodCall, 0x20);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0x75);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0x33);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x15);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x7f);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_BoolNot, 0xa6, 0x47);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_IsTrue, 0x81);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Vector, 0x25);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x8a);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x5c);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ClearParams, 0x18);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Dec, 0x50);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_Modulus, 0x7a);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0xa7);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_ScriptThreadCallPointer, 0x11);
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetBuiltinFunction, 0xa1); // 17
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetBuiltinMethod, 0x3a); // 18
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetAnimation, 0x4c); // 20
		RegisterOpCode(VMI_T10_06, PLATFORM_PC, OPCODE_IW_GetAnimationTree, 0xa0);


		RegisterVM(VMI_T10_07, "Call of Duty: Black Ops 6 (07)", "t10", "bo6_7", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VMI_T10_07, "cer7", "t10_7", "blackops6_7");
		RegisterVMPlatform(VMI_T10_07, PLATFORM_PC);
		RegisterVMGlobalVariable(VMI_T10_07, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_T10_07, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_T10_07, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_T10_07, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_07, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_07, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_07, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VMI_T10_07, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_T10_07, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VMI_T10_07, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_T10_07, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_07, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_07, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_T10_07, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		RegisterDevCall(VMI_T10_07, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_T10_07, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_Nop, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_CheckClearParams, 0x4d);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x5a);
		RegisterOpCode(VMI_T10_07, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x53);

		RegisterVM(VMI_T10_0B, "Call of Duty: Black Ops 6 (0B)", "t10", "bo6_b", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER_SP | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VMI_T10_0B, "cerb", "t10_b", "blackops6_b", "t10sp", "bo6sp");
		RegisterVMPlatform(VMI_T10_0B, PLATFORM_PC);
		RegisterVMGlobalVariable(VMI_T10_0B, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_T10_0B, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_T10_0B, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_T10_0B, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_0B, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_0B, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0B, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VMI_T10_0B, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_T10_0B, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VMI_T10_0B, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_T10_0B, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_0B, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_0B, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_T10_0B, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10ScrSP(str); });
		RegisterDevCall(VMI_T10_0B, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_T10_0B, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VMI_T10_0B, PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		RegisterOpCode(VMI_T10_0B, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		RegisterOpCode(VMI_T10_0B, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

		RegisterVM(VMI_T10_0C, "Call of Duty: Black Ops 6", "t10", "bo6", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VMI_T10_0C, "cer", "t10", "blackops6");
		RegisterVMPlatform(VMI_T10_0C, PLATFORM_PC);
		RegisterVMGlobalVariable(VMI_T10_0C, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VMI_T10_0C, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VMI_T10_0C, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VMI_T10_0C, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_0C, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VMI_T10_0C, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VMI_T10_0C, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VMI_T10_0C, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VMI_T10_0C, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VMI_T10_0C, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		RegisterVMHashOPCode(VMI_T10_0C, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_0C, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		RegisterVMHashOPCode(VMI_T10_0C, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		RegisterVMHashOPCode(VMI_T10_0C, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		RegisterDevCall(VMI_T10_0C, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VMI_T10_0C, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VMI_T10_0C, PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		RegisterOpCode(VMI_T10_0C, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		RegisterOpCode(VMI_T10_0C, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

	}


}
REGISTER_GSC_VM_OPCODES(bo6, OpCode);