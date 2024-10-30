#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		RegisterVM(VM_BO6_06, "Call of Duty: Black Ops 6 (06)", "t10_6", "bo6_6", VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VM_BO6_06, "cer6", "t10_6", "blackops6_6");
		RegisterVMPlatform(VM_BO6_06, PLATFORM_PC_ALPHA);
		RegisterVMGlobalVariable(VM_BO6_06, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_BO6_06, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_BO6_06, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_BO6_06, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_06, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_06, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_06, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VM_BO6_06, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_BO6_06, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VM_BO6_06, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_BO6_06, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_06, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_06, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_BO6_06, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hashutils::HashT10Scr(str); });
		RegisterDevCall(VM_BO6_06, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_BO6_06, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");

		// some opcodes, not all of them, but enough for a compiler
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Nop, 0);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetHash, 0x2b); // 9
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetResourceHash, 0x9a); // 10
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetDVarHash, 0x72); // 11
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetTagHash, 0x57); // 12
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_T10_GetScrHash, 0x16); // 13
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetResourceHash2, 0x69);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_SingleWaitTill, 0x80);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetThread, 0x07);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_T10_FlatArgs, 0x6a);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IsDefined, 0xa9);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CheckClearParams, 0x76);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_End, 0x17);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_PreScriptCall, 0x9c);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SafeCreateLocalVariables, 0x21);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_RegisterMultipleVariables, 0x41);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_DevblockBegin, 0x2e);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetZero, 0x5e);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetByte, 0x59);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptThreadCallEndOn, 0x4e, 0x5a); // wtf
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EndOn, 0x92);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Jump, 0x1f);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetAnim, 0x6d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CreateStruct, 0x1a);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_AddToStruct, 0x14);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetNegUnsignedInteger, 0x2a); // 20
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetLongInteger, 0x34);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SuperEqual, 0x1e);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SuperNotEqual, 0x4d); // wtf???
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_T10_GreaterThanOrSuperEqualTo, 0x79);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_T10_LowerThanOrSuperEqualTo, 0x6e);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CastFieldObject, 0x60);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalFieldVariableRef, 0x88);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SetVariableField, 0x49);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalFieldVariable, 0x82);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptThreadCall, 0x2d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_WaitFrame, 0x95);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GreaterThan, 0x13);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Multiply, 0x3b);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Minus, 0x84);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Divide, 0x1c);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Plus, 0x78);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_NotEqual, 0x29);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Return, 0x8d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetSelf, 0x36);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetLevel, 0x97);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Bit_Or, 0x1d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Bit_And, 0x85);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_LessThan, 0x77);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalArray, 0x58);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Inc, 0x63);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetUndefined, 0x22);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnFalseExpr, 0x45);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetNegByte, 0x35);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_LessThanOrEqualTo, 0x9d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GreaterThanOrEqualTo, 0x37);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetUnsignedShort, 0x73);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetUnsignedInteger, 0x42);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_WaitTillMatch, 0x40);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ShiftLeft, 0x4a);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ShiftRight, 0x4f);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_BoolComplement, 0x53);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Bit_Xor, 0x26);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x12);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptMethodCallPointer, 0x1b);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptThreadCallPointerEndOn, 0x23);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_BuiltinMethodCallPointer, 0x8c);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_BuiltinFunctionCallPointer, 0x98);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ProfileStop, 0x8e);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ProfileStart, 0x90);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnDefined, 0x71);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnDefinedExpr, 0x74);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptFunctionCall, 0x32);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_AddToArray, 0x38);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetGame, 0x5d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CallBuiltinMethod, 0xa4);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SizeOf, 0x9b);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_Notify, 0xa2);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CallBuiltinFunction, 0x65);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnTrue, 0x66);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_CreateArray, 0xa8);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetString, 0x52);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_DecTop, 0x3d);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Equal, 0x39);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnFalse, 0x96);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetFloat, 0x68);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Wait, 0x6f);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalLocalVariableCached, 0x0f);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_JumpOnTrueExpr, 0x54);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_SetLocalVariableCached, 0x62);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalArrayRef, 0x94);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetResolveFunction, 0x55);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptMethodCall, 0x20);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_EvalLocalVariableRefCached, 0x75);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptFunctionCallPointer, 0x33);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptMethodThreadCall, 0x15);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_WaitTillFrameEnd, 0x7f);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_BoolNot, 0xa6, 0x47);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_IsTrue, 0x81);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Vector, 0x25);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_GetNegUnsignedShort, 0x8a);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_SetWaittillVariableFieldCached, 0x5c);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ClearParams, 0x18);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Dec, 0x50);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_Modulus, 0x7a);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptMethodThreadCallPointer, 0xa7);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_ScriptThreadCallPointer, 0x11);
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetBuiltinFunction, 0xa1); // 17
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetBuiltinMethod, 0x3a); // 18
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetAnimation, 0x4c); // 20
		RegisterOpCode(VM_BO6_06, PLATFORM_PC_ALPHA, OPCODE_IW_GetAnimationTree, 0xa0);


		RegisterVM(VM_BO6_07, "Call of Duty: Black Ops 6 (07)", "t10", "bo6_7", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VM_BO6_07, "cer7", "t10_7", "blackops6_7");
		RegisterVMPlatform(VM_BO6_07, PLATFORM_PC);
		RegisterVMGlobalVariable(VM_BO6_07, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_BO6_07, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_BO6_07, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_BO6_07, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_07, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_07, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_07, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VM_BO6_07, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_BO6_07, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VM_BO6_07, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_BO6_07, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_07, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_07, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_BO6_07, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hashutils::HashT10Scr(str); });
		RegisterDevCall(VM_BO6_07, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_BO6_07, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_Nop, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_CheckClearParams, 0x4d);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x5a);
		RegisterOpCode(VM_BO6_07, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x53);

		RegisterVM(VM_BO6_0B, "Call of Duty: Black Ops 6 (0B)", "t10", "bo6_b", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER_SP | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VM_BO6_0B, "cerb", "t10_b", "blackops6_b", "t10sp", "bo6sp");
		RegisterVMPlatform(VM_BO6_0B, PLATFORM_PC);
		RegisterVMGlobalVariable(VM_BO6_0B, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_BO6_0B, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_BO6_0B, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_BO6_0B, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_0B, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_0B, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0B, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VM_BO6_0B, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_BO6_0B, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VM_BO6_0B, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_BO6_0B, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_0B, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_0B, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_BO6_0B, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hashutils::HashT10ScrSP(str); });
		RegisterDevCall(VM_BO6_0B, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_BO6_0B, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VM_BO6_0B, PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		RegisterOpCode(VM_BO6_0B, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		RegisterOpCode(VM_BO6_0B, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

		RegisterVM(VM_BO6_0C, "Call of Duty: Black Ops 6", "t10", "bo6", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		RegisterVmName(VM_BO6_0C, "cer", "t10", "blackops6");
		RegisterVMPlatform(VM_BO6_0C, PLATFORM_PC);
		RegisterVMGlobalVariable(VM_BO6_0C, "level", OPCODE_IW_GetLevel);
		RegisterVMGlobalVariable(VM_BO6_0C, "game", OPCODE_IW_GetGame);
		RegisterVMGlobalVariable(VM_BO6_0C, "anim", OPCODE_IW_GetAnim);
		RegisterVMOperatorFunction(VM_BO6_0C, "waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_0C, "getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		RegisterVMOperatorFunction(VM_BO6_0C, "istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		RegisterVMOperatorFunction(VM_BO6_0C, "flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		RegisterVMOperatorFunction(VM_BO6_0C, "waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		RegisterVMHashOPCode(VM_BO6_0C, '#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		RegisterVMHashOPCode(VM_BO6_0C, '@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hashutils::HashIWDVar(str); });
		RegisterVMHashOPCode(VM_BO6_0C, '%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_0C, 'r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hashutils::HashIWRes(str); });
		RegisterVMHashOPCode(VM_BO6_0C, 't', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hashutils::HashIWTag(str); });
		RegisterVMHashOPCode(VM_BO6_0C, '&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hashutils::HashT10Scr(str); });
		RegisterDevCall(VM_BO6_0C, "assert", "assertmsg", "assertex", "println");
		RegisterDatatype(VM_BO6_0C, "builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		RegisterOpCode(VM_BO6_0C, PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		RegisterOpCode(VM_BO6_0C, PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		RegisterOpCode(VM_BO6_0C, PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

	}


}
REGISTER_GSC_VM_OPCODES(bo6, OpCode);