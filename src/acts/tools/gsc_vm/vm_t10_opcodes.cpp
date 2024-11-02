#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {

		VmInfo* t106 = RegisterVM(VMI_T10_06, "Call of Duty: Black Ops 6 (06)", "t10_6", "bo6_6", VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t106->RegisterVmName("cer6", "t10_6", "blackops6_6");
		t106->AddPlatform(PLATFORM_PC);
		t106->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t106->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t106->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t106->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t106->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t106->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t106->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t106->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t106->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t106->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t106->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t106->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t106->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t106->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t106->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t106->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		t106->RegisterVMHashOPCode('r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		t106->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		t106->RegisterVMHashOPCode('&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		t106->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		t106->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		// compatibility
		t106->RegisterSameCodePlatform(PLATFORM_PC, PLATFORM_PC_ALPHA);
		t106->AddPlatform(PLATFORM_PC_ALPHA);

		// some opcodes, not all of them, but enough for a compiler
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash, 0x2b); // 9
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetResourceHash, 0x9a); // 10
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetDVarHash, 0x72); // 11
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetTagHash, 0x57); // 12
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_T10_GetScrHash, 0x16); // 13
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetResourceHash2, 0x69);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0x80);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetThread, 0x07);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_T10_FlatArgs, 0x6a);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, 0xa9);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x76);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x17);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0x9c);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x21);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x41);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, 0x2e);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x5e);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetByte, 0x59);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallEndOn, 0x4e, 0x5a); // wtf
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EndOn, 0x92);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, 0x1f);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnim, 0x6d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CreateStruct, 0x1a);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_AddToStruct, 0x14);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedInteger, 0x2a); // 20
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetLongInteger, 0x34);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SuperEqual, 0x1e);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SuperNotEqual, 0x4d); // wtf???
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_T10_GreaterThanOrSuperEqualTo, 0x79);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_T10_LowerThanOrSuperEqualTo, 0x6e);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CastFieldObject, 0x60);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x88);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SetVariableField, 0x49);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariable, 0x82);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCall, 0x2d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitFrame, 0x95);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, 0x13);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, 0x3b);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, 0x84);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, 0x1c);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, 0x78);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, 0x29);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x8d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelf, 0x36);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevel, 0x97);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Or, 0x1d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_And, 0x85);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, 0x77);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArray, 0x58);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Inc, 0x63);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x22);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0x45);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x35);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x9d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0x37);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedShort, 0x73);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedInteger, 0x42);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitTillMatch, 0x40);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftLeft, 0x4a);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftRight, 0x4f);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_BoolComplement, 0x53);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Xor, 0x26);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x12);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x1b);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointerEndOn, 0x23);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinMethodCallPointer, 0x8c);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinFunctionCallPointer, 0x98);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStop, 0x8e);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStart, 0x90);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefined, 0x71);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefinedExpr, 0x74);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCall, 0x32);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_AddToArray, 0x38);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGame, 0x5d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0xa4);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SizeOf, 0x9b);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Notify, 0xa2);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0x65);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, 0x66);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_CreateArray, 0xa8);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0x52);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, 0x3d);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, 0x39);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, 0x96);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0x68);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Wait, 0x6f);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x0f);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0x54);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_SetLocalVariableCached, 0x62);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArrayRef, 0x94);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetResolveFunction, 0x55);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCall, 0x20);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0x75);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0x33);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x15);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x7f);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0xa6, 0x47);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_IsTrue, 0x81);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Vector, 0x25);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x8a);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x5c);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0x18);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Dec, 0x50);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, 0x7a);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0xa7);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointer, 0x11);
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinFunction, 0xa1); // 17
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinMethod, 0x3a); // 18
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimation, 0x4c); // 20
		t106->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimationTree, 0xa0);


		VmInfo* t107 = RegisterVM(VMI_T10_07, "Call of Duty: Black Ops 6 (07)", "t10", "bo6_7", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t107->RegisterVmName("cer7", "t10_7", "blackops6_7");
		t107->AddPlatform(PLATFORM_PC);
		t107->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t107->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t107->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t107->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t107->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t107->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t107->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t107->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t107->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t107->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t107->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t107->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t107->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t107->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t107->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t107->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		t107->RegisterVMHashOPCode('r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		t107->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		t107->RegisterVMHashOPCode('&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		t107->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		t107->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_Nop, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x4d);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x5a);
		t107->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x53);

		VmInfo* t10b = RegisterVM(VMI_T10_0B, "Call of Duty: Black Ops 6 (0B)", "t10", "bo6_b", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER_SP | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t10b->RegisterVmName("cerb", "t10_b", "blackops6_b", "t10sp", "bo6sp");
		t10b->AddPlatform(PLATFORM_PC);
		t10b->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t10b->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t10b->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t10b->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t10b->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t10b->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t10b->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t10b->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t10b->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t10b->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t10b->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t10b->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t10b->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t10b->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t10b->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t10b->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		t10b->RegisterVMHashOPCode('r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		t10b->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		t10b->RegisterVMHashOPCode('&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10ScrSP(str); });
		t10b->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		t10b->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		t10b->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		t10b->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		t10b->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

		VmInfo* t10c = RegisterVM(VMI_T10_0C, "Call of Duty: Black Ops 6", "t10", "bo6", VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 | VmFlags::VMF_FULL_FILE_NAMESPACE | VmFlags::VMF_HASH_CER | VmFlags::VMF_CALL_NO_PARAMS); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
		t10c->RegisterVmName("cer", "t10", "blackops6");
		t10c->AddPlatform(PLATFORM_PC);
		t10c->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
		t10c->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
		t10c->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
		t10c->RegisterVMOperatorFunction("waittill", "<caller> waittill(event, var*)", OPCODE_IW_SingleWaitTill, VPFD_SELF_PARAM | VPFD_UNPACK, 1);
		t10c->RegisterVMOperatorFunction("isdefined", "isdefined(object) -> bool", OPCODE_IsDefined, VPFD_RETURN_VALUE, 1, 1);
		t10c->RegisterVMOperatorFunction("notify", "<caller> notify(event, param*)", OPCODE_IW_Notify, VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL, 1);
		t10c->RegisterVMOperatorFunction("endon", "<caller> endon(event+)", OPCODE_EndOn, VPFD_SELF_PARAM | VPFD_USE_COUNT, 1);
		t10c->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
		t10c->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
		t10c->RegisterVMOperatorFunction("getthread", "getthread() -> thread", OPCODE_IW_GetThread, VPFD_RETURN_VALUE, 0, 0);
		t10c->RegisterVMOperatorFunction("istrue", "istrue(object) -> bool", OPCODE_IW_IsTrue, VPFD_RETURN_VALUE, 1, 1);
		t10c->RegisterVMOperatorFunction("flat_args", "flat_args(array, count) -> bool", OPCODE_T10_FlatArgs, VPFD_RETURN_VALUE, 2, 2);
		t10c->RegisterVMOperatorFunction("waittillframeend", "waittillframeend()", OPCODE_WaitTillFrameEnd, VPFD_NONE, 0, 0);
		t10c->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
		t10c->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) { return hash::HashIWDVar(str); });
		t10c->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) { return hash::HashIWRes(str); });
		t10c->RegisterVMHashOPCode('r', OPCODE_IW_GetResourceHash2, 8, [](const char* str) { return hash::HashIWRes(str); });
		t10c->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) { return hash::HashIWTag(str); });
		t10c->RegisterVMHashOPCode('&', OPCODE_T10_GetScrHash, 8, [](const char* str) { return hash::HashT10Scr(str); });
		t10c->RegisterDevCall("assert", "assertmsg", "assertex", "println");
		t10c->RegisterDatatype("builtinfunction", "builtinmethod", "function", "string", "istring", "struct", "int", "float", "vector");
		t10c->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x24);
		t10c->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x20);
		t10c->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x93);

	}


}
REGISTER_GSC_VM_OPCODES(bo6, OpCode);