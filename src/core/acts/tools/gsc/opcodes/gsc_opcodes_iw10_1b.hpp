#pragma once

namespace tool::gsc::opcode {
    class OpCodeIW101BRegistry {
      public:
        static void OpCode() {
            VmInfo* vminfo = RegisterVM(
                VMI_IW10_1B,
                "Call of Duty: Modern Warfare 4",
                "iw10",
                "mw4",
                VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW_KEYS | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 |
                    VmFlags::VMF_HASH_CER | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS |
                    VmFlags::VMF_IW_LIKE | VmFlags::VMF_ISTRING_HASHED | VmFlags::VMF_WAITTILL_RETURN
            ); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS
            vminfo->RegisterVmName("iw10", "modernwarfare7", "mw4");
            vminfo->AddPlatform(PLATFORM_PC);
            vminfo->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
            vminfo->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame, OPCODE_IW_GetGameRef);
            vminfo->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
            vminfo->RegisterVMOperatorFunction(
                "waittill",
                "<caller> waittill(event, var*)",
                OPCODE_IW_SingleWaitTill,
                VPFD_SELF_PARAM | VPFD_UNPACK | VPFD_RETURN_VALUE,
                1
            );
            vminfo->RegisterVMOperatorFunction(
                "isdefined",
                "isdefined(object) -> bool",
                OPCODE_IsDefined,
                VPFD_RETURN_VALUE,
                1,
                1
            );
            vminfo->RegisterVMOperatorFunction(
                "notify",
                "<caller> notify(event, param*)",
                OPCODE_IW_Notify,
                VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL,
                1
            );
            vminfo->RegisterVMOperatorFunction(
                "endon",
                "<caller> endon(event+)",
                OPCODE_SAT_Endon,
                VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL,
                1
            );
            vminfo->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
            vminfo->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
            vminfo->RegisterVMOperatorFunction(
                "getthread",
                "getthread() -> thread",
                OPCODE_IW_GetThread,
                VPFD_RETURN_VALUE,
                0,
                0
            );
            vminfo->RegisterVMOperatorFunction(
                "istrue",
                "istrue(object) -> bool",
                OPCODE_IW_IsTrue,
                VPFD_RETURN_VALUE,
                1,
                1
            );
            vminfo->RegisterVMOperatorFunction(
                "flat_args",
                "flat_args(array, count) -> bool",
                OPCODE_T10_FlatArgs,
                VPFD_RETURN_VALUE,
                2,
                2
            );
            vminfo->RegisterVMOperatorFunction(
                "waittillframeend",
                "waittillframeend()",
                OPCODE_WaitTillFrameEnd,
                VPFD_NONE,
                0,
                0
            );
            vminfo->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
            vminfo->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) {
                return hash::HashIWDVar(str);
            });
            vminfo->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) {
                return hash::HashIWAsset(str);
            });
            vminfo->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) {
                return hash::HashX32(str);
            });
            vminfo->RegisterVMHashOPCode('s', OPCODE_T10_GetScrHash, 8, [](const char* str) {
                return hash::HashT10Scr(str);
            });
            vminfo->RegisterVMHashOPCode('o', OPCODE_SAT_GetOmnVarHash, 8, [](const char* str) {
                return hash::HashT10OmnVar(str);
            });
            vminfo->RegisterDevCall(
                "assert",
                "assertmsg",
                "projectplanning_assertmsgcerberusorlater",
                "assertex",
                "println",
                "print"
            );
            vminfo->RegisterDatatype(
                "builtinfunction",
                "builtinmethod",
                "function",
                "string",
                "istring",
                "struct",
                "int",
                "float",
                "vector",
                "xhash",
                "xhashasset",
                "istring"
            );
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_AddToArray, 0x86);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_And, 0x77);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Or, 0x27);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_BoolComplement, 0x5d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0x38);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0x22);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0x78);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CastBool, 0x84);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CastFieldObject, 0x64);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x14);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ClearArray, 0x82);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ClearFieldVariable, 0x4a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_ClearFieldVariableRef, 0x63);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0x3e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CreateArray, 0x61);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_CreateStruct, 0x31);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Dec, 0x65);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, 0x1f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, 0x93);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, 0x23);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x52);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, 0x33);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVar, 0x8c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVarRef, 0x40);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArray, 0x81);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalArrayCachedField, 0x68);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArrayRef, 0x44);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariable, 0x5b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x7e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariable, 0x8e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariableRef, 0x90);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalArrayCached, 0x12);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x71);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_T9_EvalLocalVariableCachedDouble, 0x5e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableObjectCached, 0x8d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0x6d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVar, 0x17);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVarRef, 0x30);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_T10_FlatArgs, 0x46);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimation, 0x87);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimationTree, 0x56);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinFunction, 0x28);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinMethod, 0x11);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetByte, 0x5c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetDVarHash, 0x69);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0x1b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGame, 0x67);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGameRef, 0x48);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash, 0x20);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevel, 0x2b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevelGRef, 0x2f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLocalizedHash, 0x59);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetLongInteger, 0x8b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x72);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedInteger, 0x1c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x9c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SAT_GetOmnVarHash, 0x9f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetResolveFunction, 0x2d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetResourceHash, 0x9a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_T10_GetScrHash, 0x21);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelf, 0x73);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelfObject, 0x1d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0xc);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x91);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedInteger, 0x39);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedShort, 0x4f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetVector, 0x1e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x6f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, 0x45);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0x97);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_AddToStruct, 0x43);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EndSwitch, 0x7d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Notify, 0xa2);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Switch, 0x79);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitFrame, 0x25);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitTillMatch, 0xa0);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Inc, 0x57);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, 0x9b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_IsTrue, 0x92);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, 0x6b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefined, 0x66);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefinedExpr, 0x19);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, 0x42);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0x1a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, 0x54);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0xa);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, 0xb);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x70);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, 0x41);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, 0xe);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, 0x15);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, 0x5f);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, 0x2c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0x24);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStart, 0x26);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStop, 0x36);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x10);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x62);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SAT_Endon, 0x3a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x7a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCall, 0x8a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0x9d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCall, 0x34);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x7b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x4e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallEndOn, 0x5a);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0x4d);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x76);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCall, 0x4b);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallEndOn, 0x94);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointer, 0x75);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointerEndOn, 0x7c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetAnimFieldVar, 0x9);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetLevelFieldVariable, 0x47);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SetLocalVariableCached, 0x95);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetSelfFieldVar, 0xd);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SetVariableField, 0x88);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x9e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftLeft, 0x13);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftRight, 0xf);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0x60);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SizeOf, 0x3c);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_SuperEqual, 0x49);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_T10_EvalLocalVariableCachedTriple, 0x2e);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Vector, 0x50);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_Wait, 0x55);
            vminfo->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x74);
        }
    };
} // namespace tool::gsc::opcode