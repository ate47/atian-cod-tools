#pragma once

namespace tool::gsc::opcode {
    class OpCodeSat1ARegistry {
      public:
        static void OpCode() {
            VmInfo* t101a = RegisterVM(
                VMI_SAT_1A,
                "Call of Duty: Black Ops 7",
                "t11",
                "bo7",
                VmFlags::VMF_CRC_DUMP | VmFlags::VMF_FOREACH_IW_KEYS | VmFlags::VMF_EXPORT_CRC32 | VmFlags::VMF_HASH64 |
                    VmFlags::VMF_HASH_CER | VmFlags::VMF_HASH_PATH_IW | VmFlags::VMF_CALL_NO_PARAMS |
                    VmFlags::VMF_IW_LIKE | VmFlags::VMF_ISTRING_HASHED | VmFlags::VMF_WAITTILL_RETURN
            ); // VmFlags::VMF_IW_CALLS | VmFlags::VMF_NO_PARAM_FLAGS |
               // VmFlags::VMF_NO_PLATFORM
            t101a->RegisterVmName("sat", "blackops7", "bo7");
            t101a->AddPlatform(PLATFORM_PC);
            t101a->RegisterVMGlobalVariable("level", OPCODE_IW_GetLevel);
            t101a->RegisterVMGlobalVariable("game", OPCODE_IW_GetGame);
            t101a->RegisterVMGlobalVariable("anim", OPCODE_IW_GetAnim);
            t101a->RegisterVMOperatorFunction(
                "waittill",
                "<caller> waittill(event, var*)",
                OPCODE_IW_SingleWaitTill,
                VPFD_SELF_PARAM | VPFD_UNPACK | VPFD_RETURN_VALUE,
                1
            );
            t101a->RegisterVMOperatorFunction(
                "isdefined",
                "isdefined(object) -> bool",
                OPCODE_IsDefined,
                VPFD_RETURN_VALUE,
                1,
                1
            );
            t101a->RegisterVMOperatorFunction(
                "notify",
                "<caller> notify(event, param*)",
                OPCODE_IW_Notify,
                VPFD_SELF_PARAM | VPFD_USE_PRE_SCRIPT_CALL,
                1
            );
            t101a->RegisterVMOperatorFunction(
                "endon",
                "<caller> endon(event+)",
                OPCODE_EndOn,
                VPFD_SELF_PARAM | VPFD_USE_COUNT,
                1
            );
            t101a->RegisterVMOperatorFunction("wait", "wait(time)", OPCODE_Wait, VPFD_NONE, 1, 1);
            t101a->RegisterVMOperatorFunction("waitframe", "waitframe()", OPCODE_IW_WaitFrame, VPFD_NONE, 0, 0);
            t101a->RegisterVMOperatorFunction(
                "getthread",
                "getthread() -> thread",
                OPCODE_IW_GetThread,
                VPFD_RETURN_VALUE,
                0,
                0
            );
            t101a->RegisterVMOperatorFunction(
                "istrue",
                "istrue(object) -> bool",
                OPCODE_IW_IsTrue,
                VPFD_RETURN_VALUE,
                1,
                1
            );
            t101a->RegisterVMOperatorFunction(
                "flat_args",
                "flat_args(array, count) -> bool",
                OPCODE_T10_FlatArgs,
                VPFD_RETURN_VALUE,
                2,
                2
            );
            t101a->RegisterVMOperatorFunction(
                "waittillframeend",
                "waittillframeend()",
                OPCODE_WaitTillFrameEnd,
                VPFD_NONE,
                0,
                0
            );
            t101a->RegisterVMHashOPCode('#', OPCODE_GetHash, 8, [](const char* str) { return hash::Hash64A(str); });
            t101a->RegisterVMHashOPCode('@', OPCODE_IW_GetDVarHash, 8, [](const char* str) {
                return hash::HashIWDVar(str);
            });
            t101a->RegisterVMHashOPCode('%', OPCODE_IW_GetResourceHash, 8, [](const char* str) {
                return hash::HashIWAsset(str);
            });
            t101a->RegisterVMHashOPCode('t', OPCODE_IW_GetTagHash, 4, [](const char* str) {
                return hash::HashX32(str);
            });
            t101a->RegisterVMHashOPCode('s', OPCODE_T10_GetScrHash, 8, [](const char* str) {
                return hash::HashT10Scr(str);
            });
            t101a->RegisterVMHashOPCode('o', OPCODE_SAT_GetOmnVarHash, 8, [](const char* str) {
                return hash::HashT10OmnVar(str);
            });
            t101a->RegisterDevCall(
                "assert",
                "assertmsg",
                "projectplanning_assertmsgcerberusorlater",
                "assertex",
                "println",
                "print"
            );
            t101a->RegisterDatatype(
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

            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, 0x2c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalVariableObjectCached, 0x42);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableCached, 0x6f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, 0x34);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, 0x55);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Notify, 0x49);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, 0x9a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash, 0x82);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevel, 0x65);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCallPointer, 0x23);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_End, 0x36);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, 0x93);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, 0x76);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, 0x9f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_WaitTillFrameEnd, 0x8b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, 0x86);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, 0x60);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelf, 0x4d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_T9_EvalLocalVariableCachedDouble, 0x5d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCallPointer, 0x2a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Return, 0x50);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_BoolComplement, 0x61);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinFunction, 0x54);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetZero, 0x8a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Wait, 0x90);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CallBuiltinMethod, 0x2b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalLocalVariableRefCached, 0xa2);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SetLocalVariableCached, 0x3e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_RegisterMultipleVariables, 0x1d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinMethodCallPointer, 0x6d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetString, 0x5c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, 0x48);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ClearParams, 0x71);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, 0x27);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodCall, 0x46);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, 0x1f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetLevelFieldVariable, 0x87);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SAT_Endon, 0x84);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CastBool, 0x4a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptFunctionCall, 0x40);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CastFieldObject, 0x45);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, 0x72);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariableRef, 0x9);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefined, 0x6c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, 0x3b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CreateArray, 0x8e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArrayRef, 0x7f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SetVariableField, 0x5a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalFieldVariable, 0x38);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCall, 0xa1);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, 0x1b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftRight, 0x1e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_EvalArray, 0x6b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetDVarHash, 0x53);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_ClearFieldVariableRef, 0x20);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ClearArray, 0x6a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, 0x5f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariable, 0x63);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, 0x32);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_T10_EvalLocalVariableCachedTriple, 0x51);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SizeOf, 0x16);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Inc, 0xc);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetByte, 0x9b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, 0x39);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetResolveFunction, 0x85);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SingleWaitTill, 0x66);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVar, 0xa0);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetSelfFieldVar, 0x67);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegByte, 0x3f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetVector, 0x24);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, 0xa4);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLocalArrayCached, 0x69);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedShort, 0xd);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Dec, 0x47);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallEndOn, 0x33);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetWaittillVariableFieldCached, 0x1c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, 0x79);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointer, 0x73);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EndSwitch, 0x57);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ClearFieldVariable, 0x3a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SuperEqual, 0x7a);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCall, 0x95);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Vector, 0x9d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_Switch, 0x94);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalSelfFieldVarRef, 0x4f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetSelfObject, 0x7e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, 0x4b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointer, 0x7c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, 0x21);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedShort, 0x28);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefinedExpr, 0xf);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_AddToArray, 0xe);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalArrayCachedField, 0x31);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitFrame, 0x75);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, 0x8c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_CreateStruct, 0xa3);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_AddToStruct, 0x22);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalLevelFieldVariableRef, 0x2d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLevelGRef, 0x88);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetLocalizedHash, 0x43);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_T10_FlatArgs, 0x52);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_SetAnimFieldVar, 0x4e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_IsTrue, 0x5b);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVar, 0x99);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, 0x96);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_EvalAnimFieldVarRef, 0x2f);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_T10_GetScrHash, 0x77);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_And, 0x41);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinFunction, 0x78);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_BuiltinFunctionCallPointer, 0x70);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetUnsignedInteger, 0x64);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGame, 0x68);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStart, 0x92);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ProfileStop, 0x6e);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Or, 0x91);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftLeft, 0x26);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetBuiltinMethod, 0x19);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, 0x74);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetGameRef, 0xb);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallEndOn, 0x18);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_WaitTillMatch, 0x37);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimation, 0x4c);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetResourceHash, 0x11);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetNegUnsignedInteger, 0x15);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_IW_GetAnimationTree, 0x14);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptThreadCallPointerEndOn, 0x62);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_ScriptMethodThreadCallPointerEndOn, 0x3d);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_GetLongInteger, 0x35);
            t101a->RegisterOpCode(PLATFORM_PC, OPCODE_SAT_GetOmnVarHash, 0x56);

        }
    };
} // namespace tool::gsc::opcode