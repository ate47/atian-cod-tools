#pragma once

#include <includes_shared.hpp>
#include "tools/gsc_opcodes.hpp"

namespace compatibility::serious {
    constexpr auto VM_CODES_DB = "vm_codes.db2";

    enum SeriousId : byte {
        SERID_End = 0x0,
        SERID_Return = 0x1,
        SERID_GetUndefined = 0x2,
        SERID_GetZero = 0x3,
        SERID_GetByte = 0x4,
        SERID_GetNegByte = 0x5,
        SERID_GetUnsignedShort = 0x6,
        SERID_GetNegUnsignedShort = 0x7,
        SERID_GetInteger = 0x8,
        SERID_GetFloat = 0x9,
        SERID_GetString = 0xA,
        SERID_GetIString = 0xB,
        SERID_GetVector = 0xC,
        SERID_GetLevelObject = 0xD,
        SERID_GetAnimObject = 0xE,
        SERID_GetSelf = 0xF,
        SERID_GetLevel = 0x10,
        SERID_GetGame = 0x11,
        SERID_GetAnim = 0x12,
        SERID_GetAnimation = 0x13,
        SERID_GetGameRef = 0x14,
        SERID_GetFunction = 0x15,
        SERID_CreateLocalVariable = 0x16,
        SERID_SafeCreateLocalVariables = 0x17,
        SERID_RemoveLocalVariables = 0x18,
        SERID_EvalLocalVariableCached = 0x19,
        SERID_EvalArray = 0x1A,
        SERID_EvalLocalArrayRefCached = 0x1B,
        SERID_EvalArrayRef = 0x1C,
        SERID_ClearArray = 0x1D,
        SERID_GetEmptyArray = 0x1E,
        SERID_GetSelfObject = 0x1F,
        SERID_EvalFieldVariable = 0x20,
        SERID_EvalFieldVariableRef = 0x21,
        SERID_ClearFieldVariable = 0x22,
        SERID_SafeSetVariableFieldCached = 0x23, //t9
        SERID_SetWaittillVariableFieldCached = 0x24,
        SERID_ClearParams = 0x25,
        SERID_CheckClearParams = 0x26,
        SERID_EvalLocalVariableRefCached = 0x27,
        SERID_SetVariableField = 0x28,
        SERID_CallBuiltin = 0x29,
        SERID_CallBuiltinMethod = 0x2A,
        SERID_Wait = 0x2B,
        SERID_WaitTillFrameEnd = 0x2C,
        SERID_PreScriptCall = 0x2D,
        SERID_ScriptFunctionCall = 0x2E,
        SERID_ScriptFunctionCallPointer = 0x2F,
        SERID_ScriptMethodCall = 0x30,
        SERID_ScriptMethodCallPointer = 0x31,
        SERID_ScriptThreadCall = 0x32,
        SERID_ScriptThreadCallPointer = 0x33,
        SERID_ScriptMethodThreadCall = 0x34,
        SERID_ScriptMethodThreadCallPointer = 0x35,
        SERID_DecTop = 0x36,
        SERID_CastFieldObject = 0x37,
        SERID_CastBool = 0x38,
        SERID_BoolNot = 0x39,
        SERID_BoolComplement = 0x3A,
        SERID_JumpOnFalse = 0x3B,
        SERID_JumpOnTrue = 0x3C,
        SERID_JumpOnFalseExpr = 0x3D,
        SERID_JumpOnTrueExpr = 0x3E,
        SERID_Jump = 0x3F,
        SERID_JumpBack = 0x40,
        SERID_Inc = 0x41,
        SERID_Dec = 0x42,
        SERID_Bit_Or = 0x43,
        SERID_Bit_Xor = 0x44,
        SERID_Bit_And = 0x45,
        SERID_Equal = 0x46,
        SERID_NotEqual = 0x47,
        SERID_LessThan = 0x48,
        SERID_GreaterThan = 0x49,
        SERID_LessThanOrEqualTo = 0x4A,
        SERID_GreaterThanOrEqualTo = 0x4B,
        SERID_ShiftLeft = 0x4C,
        SERID_ShiftRight = 0x4D,
        SERID_Plus = 0x4E,
        SERID_Minus = 0x4F,
        SERID_Multiply = 0x50,
        SERID_Divide = 0x51,
        SERID_Modulus = 0x52,
        SERID_SizeOf = 0x53,
        SERID_WaitTillMatch = 0x54,
        SERID_WaitTill = 0x55,
        SERID_Notify = 0x56,
        SERID_EndOn = 0x57,
        SERID_VoidCodePos = 0x58,
        SERID_Switch = 0x59,
        SERID_EndSwitch = 0x5A,
        SERID_Vector = 0x5B,
        SERID_GetHash = 0x5C,
        SERID_RealWait = 0x5D,
        SERID_VectorConstant = 0x5E,
        SERID_IsDefined = 0x5F,
        SERID_VectorScale = 0x60,
        SERID_AnglesToUp = 0x61,
        SERID_AnglesToRight = 0x62,
        SERID_AnglesToForward = 0x63,
        SERID_AngleClamp180 = 0x64,
        SERID_VectorToAngles = 0x65,
        SERID_Abs = 0x66,
        SERID_GetTime = 0x67,
        SERID_GetDvar = 0x68,
        SERID_GetDvarInt = 0x69,
        SERID_GetDvarFloat = 0x6A,
        SERID_GetDvarVector = 0x6B,
        SERID_GetDvarColorRed = 0x6C,
        SERID_GetDvarColorGreen = 0x6D,
        SERID_GetDvarColorBlue = 0x6E,
        SERID_GetDvarColorAlpha = 0x6F,
        SERID_FirstArrayKey = 0x70,
        SERID_NextArrayKey = 0x71,
        SERID_ProfileStart = 0x72,
        SERID_ProfileStop = 0x73,
        SERID_SafeDecTop = 0x74,
        SERID_Nop = 0x75,
        SERID_Abort = 0x76,
        SERID_Obj = 0x77,
        SERID_ThreadObject = 0x78,
        SERID_EvalLocalVariable = 0x79,
        SERID_EvalLocalVariableRef = 0x7A,
        SERID_DevblockBegin = 0x7B,
        SERID_DevblockEnd = 0x7C,
        SERID_Breakpoint = 0x7D,
        SERID_AutoBreakpoint = 0x7E,
        SERID_ErrorBreakpoint = 0x7F,
        SERID_WatchBreakpoint = 0x80,
        SERID_NotifyBreakpoint = 0x81,
        SERID_GetObjectType,
        SERID_WaitRealTime,
        SERID_GetWorldObject,
        SERID_GetClassesObject,
        SERID_ClassFunctionCall,
        SERID_Bit_Not,
        SERID_GetWorld,
        SERID_EvalLevelFieldVariable,
        SERID_EvalLevelFieldVariableRef,
        SERID_EvalSelfFieldVariable,
        SERID_EvalSelfFieldVariableRef,
        SERID_SuperEqual,
        SERID_SuperNotEqual,
        SERID_SetLocalVariableCached,
        SERID_EvalLocalVariableDefined,
        SERID_JumpOnGreaterThan,
        SERID_SetVariableFieldRef,
        SERID_CreateStruct,
        SERID_AddToStruct,
        SERID_GetGlobalObject,
        SERID_JumpOnLessThan,
        SERID_GetGlobalObjectRef,
        SERID_EvalGlobalObjectFieldVariable,
        SERID_WaitFrame,
        SERID_CastAndEvalFieldVariable,
        SERID_FirstArrayKeyCached,
        SERID_SetNextArrayKeyCached,
        SERID_GetAPIFunction,
        SERID_GetUnsignedInteger,
        SERID_CastVariableName,
        SERID_EvalFieldVariableOnStack,
        SERID_EvalFieldVariableOnStackRef,
        SERID_EndOnCallback,
        SERID_WaittillTimeout,
        SERID_AddToArray,
        SERID_ClassFunctionThreadCall,
        SERID_ScriptThreadCall2,
        SERID_ScriptThreadCallPointer2,
        SERID_ScriptMethodThreadCall2,
        SERID_ScriptMethodThreadCallPointer2,
        SERID_ClassFunctionThreadCall2,
        SERID_EvalLocalVariableCached2,
        SERID_EvalLocalVariableRefCached2,
        SERID_LazyGetFunction,
        SERID_T9IteratorKey,
        SERID_T9IteratorVal,
        SERID_T9IteratorNext,
        GetNegUnsignedInteger = 192,
        SERID_COUNT,
        SERID_Invalid = 0xFF,
    };

    tool::gsc::opcode::OPCode ConvertFrom(SeriousId id);
    SeriousId ConvertTo(tool::gsc::opcode::OPCode id);
    void LoadVMDatabase(const std::filesystem::path& path);
}