#pragma once

namespace tool::gsc::opcode {
	constexpr uint64_t GetGscOldVm(const char* id) {
		uint64_t uid{ 0x2d435347 }; // "GSC-"
		size_t offset{ 32 };

		while (*id) {
			uid |= (uint64_t) *(id++) << offset;
			offset += 8;
		}

		return uid;
	}
	enum VMId : uint64_t {
		VMI_UNKNOWN = 0,

		// Treyarch VMs
		VMI_T71B = 0x1B000a0d43534780,
		VMI_T7 = 0x1C000a0d43534780,
		VMI_T831 = 0x31000a0d43534780,
		VMI_T832 = 0x32000a0d43534780,
		VMI_T833 = 0x33000a0d43534780,
		VMI_T834 = 0x34000a0d43534780,
		VMI_T835 = 0x35000a0d43534780,
		VMI_T8 = 0x36000a0d43534780,
		VMI_T937 = 0x37000a0d43534780,
		VMI_T9 = 0x38000a0d43534780,

		// IW linked VMs
		VMI_JUP_8A = 0xa0d4353478a,
		VMI_JUP_8B = 0xa0d4353478b,
		VMI_T10_06 = 0xa0d43534706,
		VMI_T10_07 = 0xa0d43534707,
		VMI_T10_0B = 0xa0d4353470B,
		VMI_T10_0C = 0xa0d4353470C,

		// IW old VMs
		VMI_IW_GSCBIN = 0x00435347, // "GSC\0"
		VMI_IW_BIN_GHOSTS = GetGscOldVm("GH"),
		VMI_IW_BIN_AW = GetGscOldVm("AW"),
		VMI_IW_BIN_IW = GetGscOldVm("IW"),
		VMI_IW_BIN_RM_MW1 = GetGscOldVm("RM1"),
		VMI_IW_BIN_WW2 = GetGscOldVm("WW2"),
		VMI_IW_BIN_MW19 = GetGscOldVm("M19"),
		VMI_IW_BIN_RM_MW2 = GetGscOldVm("RM2"),
		VMI_IW_BIN_VANGUARD = GetGscOldVm("VNG"),
		VMI_IW_BIN_MW22 = GetGscOldVm("M22"),

		// ACTS VM
		VMI_ACTS_F1 = 0x4d565354434124F1,
	};

	enum VMOldRevisions : byte {
		VMOld_UNKNOWN = 0,

		// Treyarch style VM
		VMOld_T71B = 0x1B,
		VMOld_T7 = 0x1C,
		VMOld_T831 = 0x31,
		VMOld_T8 = 0x36,
		VMOld_T937 = 0x37,
		VMOld_T9 = 0x38,

		// IW style VM
		VMOld_MW23 = 0x8a,
		VMOld_MW23B = 0x8b,
		VMOld_BO6_06 = 0x06,
		VMOld_BO6_07 = 0x07,
		VMOld_BO6_0B = 0x0B,
		VMOld_BO6_0C = 0x0C,

		// ACTS VM
		VMOld_ACTS_F1 = 0xF1,
	};

	enum Platform : byte {
		PLATFORM_UNKNOWN = 0,
		PLATFORM_PC = 1,
		PLATFORM_PLAYSTATION = 2,
		PLATFORM_XBOX = 3,
		PLATFORM_PC_ALPHA = 4,
		PLATFORM_COUNT,
	};

	Platform PlatformOf(const char* name);
	const char* PlatformName(Platform plt);
	const char* PlatformIdName(Platform plt);
	VMId VMOf(const char* name);
	VMId OldVmOf(byte vm);
	byte MapAsOldVM(uint64_t vm);

	enum OPCode : uint16_t {
		OPCODE_Undefined,
		OPCODE_Unknown0,
		OPCODE_Unknown1,
		OPCODE_Unknown2,
		OPCODE_Unknown3,
		OPCODE_Unknown4,
		OPCODE_Unknown5,
		OPCODE_Unknown6,
		OPCODE_Unknown7,
		OPCODE_Unknowna,
		OPCODE_Unknownb,
		OPCODE_Unknown38,
		OPCODE_Nop,
		OPCODE_GetUndefined,
		OPCODE_Unknown10e,
		OPCODE_Unknown126,
		OPCODE_End,
		OPCODE_EvalLocalVariableCachedDebug,
		OPCODE_EvalLocalVariableRefCachedDebug,
		OPCODE_ClearParams,
		OPCODE_CheckClearParams,
		OPCODE_PreScriptCall,
		OPCODE_CallBuiltinFunction,
		OPCODE_CallBuiltinMethod,
		OPCODE_EmptyArray,
		OPCODE_EmptyStruct,
		OPCODE_WaitTill,
		OPCODE_ClearFieldVariableOnStack,
		OPCODE_ClearArray,
		OPCODE_ScriptMethodThreadCallPointer,
		OPCODE_WaitTillMatchTimeout,
		OPCODE_GetHash,
		OPCODE_WaitFrame,
		OPCODE_ScriptMethodThreadCallPointerEndOn,
		OPCODE_WaitTillMatch,
		OPCODE_ScriptThreadCallEndOn,
		OPCODE_Switch,
		OPCODE_ScriptFunctionCallPointer,
		OPCODE_DevblockBegin,
		OPCODE_NotEqual,
		OPCODE_ScriptMethodCallPointer,
		OPCODE_JumpOnTrue,
		OPCODE_ScriptMethodThreadCallEndOn,
		OPCODE_SafeCreateLocalVariables,
		OPCODE_GetNegUnsignedShort,
		OPCODE_ClassFunctionCall,
		OPCODE_Return,
		OPCODE_EvalFieldVariableRef,
		OPCODE_SafeDecTop,
		OPCODE_Bit_Or,
		OPCODE_ScriptThreadCall,
		OPCODE_Bit_Xor,
		OPCODE_GetSelfObject,
		OPCODE_GetNegByte,
		OPCODE_ScriptThreadCallPointerEndOn,
		OPCODE_BoolComplement,
		OPCODE_IsDefined,
		OPCODE_AddToArray,
		OPCODE_Wait,
		OPCODE_SuperEqual,
		OPCODE_ScriptFunctionCall,
		OPCODE_JumpOnTrueExpr,
		OPCODE_CreateArray,
		OPCODE_Inc,
		OPCODE_ShiftLeft,
		OPCODE_JumpOnGreaterThan,
		OPCODE_Plus,
		OPCODE_CastAndEvalFieldVariable,
		OPCODE_ShiftRight,
		OPCODE_CreateStruct,
		OPCODE_CastCanon,
		OPCODE_GreaterThanOrEqualTo,
		OPCODE_GetUIntPtr,
		OPCODE_GetLongInteger,
		OPCODE_EvalArray,
		OPCODE_WaitTillFrameEnd,
		OPCODE_EndOnCallback,
		OPCODE_EndOn,
		OPCODE_SuperNotEqual,
		OPCODE_GetFloat,
		OPCODE_ProfileStart,
		OPCODE_GetString,
		OPCODE_BoolNot,
		OPCODE_CastBool,
		OPCODE_Equal,
		OPCODE_GetUnsignedInteger,
		OPCODE_WaittillTimeout,
		OPCODE_GreaterThan,
		OPCODE_Jump,
		OPCODE_Divide,
		OPCODE_EndSwitch,
		OPCODE_JumpOnFalse,
		OPCODE_JumpOnFalseExpr,
		OPCODE_Minus,
		OPCODE_ProfileStop,
		OPCODE_GetInteger,
		OPCODE_ClassFunctionThreadCall,
		OPCODE_VectorScale,
		OPCODE_Modulus,
		OPCODE_VectorConstant,
		OPCODE_SizeOf,
		OPCODE_Notify,
		OPCODE_Vector,
		OPCODE_Dec,
		OPCODE_JumpOnLessThan,
		OPCODE_Bit_And,
		OPCODE_GetObjectType,
		OPCODE_Multiply,
		OPCODE_EvalFieldVariableOnStack,
		OPCODE_GetTime,
		OPCODE_AddToStruct,
		OPCODE_ClassFunctionThreadCallEndOn,
		OPCODE_LessThanOrEqualTo,
		OPCODE_GetGlobal,
		OPCODE_GetZero,
		OPCODE_ClearFieldVariable,
		OPCODE_EvalFieldObjectFromRef,
		OPCODE_GetSelf,
		OPCODE_GetResolveFunction,
		OPCODE_GetGlobalObject,
		OPCODE_GetByte,
		OPCODE_CastFieldObject,
		OPCODE_ScriptThreadCallPointer,
		OPCODE_LessThan,
		OPCODE_ScriptMethodCall,
		OPCODE_DecTop,
		OPCODE_GetVector,
		OPCODE_ScriptMethodThreadCall,
		OPCODE_JumpPush,
		OPCODE_GetUnsignedShort,
		OPCODE_EvalLocalVariableCached,
		OPCODE_EvalFieldVariable,
		OPCODE_GetFunction,
		OPCODE_EvalArrayRef,
		OPCODE_SetNextArrayKeyCached,
		OPCODE_Unknown9e,
		OPCODE_EvalLocalVariableCachedSafe,
		OPCODE_SetLocalVariableCachedOnStack,
		OPCODE_Unknownc7,
		OPCODE_Unknown35,
		OPCODE_FirstArrayKey,
		OPCODE_EvalFieldVariableOnStackRef,
		OPCODE_SetVariableFieldRef,
		OPCODE_SetVariableField,
		OPCODE_EvalSelfFieldVariable,
		OPCODE_SetLocalVariableCached,
		OPCODE_FirstArrayKeyCached,
		OPCODE_EvalLocalVariableRefCached,
		OPCODE_EvalGlobalObjectFieldVariable,
		OPCODE_EvalLocalVariableDefined,

		// T8-Compiler Custom opcodes
		OPCODE_T8C_GetLazyFunction,

		// T9
		OPCODE_PixBeginEvent,
		OPCODE_PixEndEvent,
		OPCODE_GetNegUnsignedInteger,
		OPCODE_T9_EvalFieldVariableFromObjectCached,
		OPCODE_T9_EvalFieldVariableFromObjectFromRef,
		OPCODE_T9_SetFieldVariableFromObjectFromRef,
		OPCODE_T9_EvalLocalVariableCachedDouble,
		OPCODE_T9_EvalFieldVariableFromGlobalObject,
		OPCODE_T9_SetVariableFieldFromEvalArrayRef,
		OPCODE_T9_IncLocalVariableCached,
		OPCODE_T9_DecLocalVariableCached,
		OPCODE_T9_EvalArrayCached,
		OPCODE_T9_EndOnCallbackParam,
		OPCODE_T9_GetVarRef,
		OPCODE_T9_IteratorKey,
		OPCODE_T9_IteratorVal,
		OPCODE_T9_IteratorNext,

		// MW
		OPCODE_IW_RegisterVariable,
		OPCODE_IW_GetAnimation,
		OPCODE_IW_GetAnimationTree,
		OPCODE_IW_GetBuiltinFunction,
		OPCODE_IW_GetBuiltinMethod,
		OPCODE_IW_GetIString,
		OPCODE_IW_GetEmptyUnkE,
		OPCODE_IW_GetTagHash,
		OPCODE_IW_GetResourceHash,
		OPCODE_IW_GetDVarHash,
		OPCODE_IW_EvalLocalVariableCached0,
		OPCODE_IW_EvalLocalVariableCached1,
		OPCODE_IW_EvalLocalVariableCached2,
		OPCODE_IW_EvalLocalVariableCached3,
		OPCODE_IW_EvalLocalVariableCached4,
		OPCODE_IW_EvalLocalVariableCached5,
		OPCODE_IW_EvalLocalArrayCached,
		OPCODE_IW_EvalLocalVariableObjectCached,
		OPCODE_IW_ClearFieldVariableRef,
		OPCODE_IW_EvalLevelFieldVariableRef,
		OPCODE_IW_EvalLevelFieldVariable,
		OPCODE_IW_EvalSelfFieldVar,
		OPCODE_IW_EvalSelfFieldVarRef,
		OPCODE_IW_EvalAnimFieldVar,
		OPCODE_IW_EvalAnimFieldVarRef,
		OPCODE_IW_SetAnimFieldVar,
		OPCODE_IW_SetLevelFieldVariable,
		OPCODE_IW_SetSelfFieldVar,
		OPCODE_IW_DevBlock,
		OPCODE_IW_SingleEndon,
		OPCODE_IW_SingleWaitTill,
		OPCODE_IW_IsTrue,
		OPCODE_IW_Switch,
		OPCODE_IW_EndSwitch,
		OPCODE_IW_SetWaittillVariableFieldCached,
		OPCODE_IW_WaitFrame,
		OPCODE_IW_AddToStruct,
		OPCODE_IW_BuiltinFunctionCallPointer,
		OPCODE_IW_BuiltinMethodCallPointer,

		OPCODE_IW_GetLevel,
		OPCODE_IW_GetLevelGRef,
		OPCODE_IW_GetGame,
		OPCODE_IW_GetGameRef,
		OPCODE_IW_GetAnim,
		OPCODE_GetAnimGRef,
		OPCODE_IW_EvalArrayCachedField,
		OPCODE_IW_GetThread,
		OPCODE_IW_Notify,
		OPCODE_IW_WaitTillMatch,

		OPCODE_DevOp,

		OPCODE_GetSignedByte,
		OPCODE_GetShort,
		OPCODE_GetHash32,
		OPCODE_NextArrayKey,
		OPCODE_T7_ProfileStart,
		OPCODE_GetClasses,
		OPCODE_GetClassesObject,
		OPCODE_GetWorld,
		OPCODE_GetWorldObject,
		OPCODE_Wait2,
		OPCODE_WaitTillMatch2,
		OPCODE_IgnoreWaittillVariableFieldCached,

		OPCODE_IW_RegisterMultipleVariables,

		OPCODE_Abort,
		OPCODE_Breakpoint,
		OPCODE_AutoBreakpoint,
		OPCODE_ErrorBreakpoint,
		OPCODE_WatchBreakpoint,
		OPCODE_NotifyBreakpoint,
		OPCODE_ThreadEndBreakpoint,
		OPCODE_PushVar,
		OPCODE_PushEntityVar,
		OPCODE_InvalidOpCode,

		OPCODE_DEV_Consume0,
		OPCODE_DEV_Consume1,
		OPCODE_DEV_Consume2,
		OPCODE_DEV_Consume4,
		OPCODE_DEV_Consume8,
		OPCODE_DEV_Consume9,

		OPCODE_DEV_Consume0Push,
		OPCODE_DEV_Consume1Push,
		OPCODE_DEV_Consume2Push,
		OPCODE_DEV_Consume4Push,
		OPCODE_DEV_Consume8Push,
		OPCODE_DEV_Consume9Push,

		OPCODE_JumpOnDefined,
		OPCODE_JumpOnDefinedExpr,
		OPCODE_T10_GetScrHash,
		OPCODE_T10_FlatArgs,
		OPCODE_T10_GreaterThanOrSuperEqualTo,
		OPCODE_T10_LowerThanOrSuperEqualTo,
		OPCODE_IW_GetResourceHash2,

		OPCODE_IW_GetPositionRef,

		// undefined opcodes to pass data in gscbin
		OPCODE_GSCBIN_SKIP_0, // skip 0 bytes
		OPCODE_GSCBIN_SKIP_1, // skip 1 byte
		OPCODE_GSCBIN_SKIP_2, // skip 2 bytes
		OPCODE_GSCBIN_SKIP_3, // skip 3 bytes
		OPCODE_GSCBIN_SKIP_4, // skip 4 bytes
		OPCODE_GSCBIN_SKIP_5, // skip 5 bytes
		OPCODE_GSCBIN_SKIP_N, // skip N bytes (N a byte)
		OPCODE_GSCBIN_SKIP_3BC_4SD, // skip 3 bytes and 4 script data
		OPCODE_GSCBIN_SKIP_4BC_4SD, // skip 4 bytes and 4 script data
		OPCODE_GSCBIN_SKIP_STR_TOKEN, // skip opaque or str
		OPCODE_GSCBIN_SKIP_4BC_1STR, // skip 4 bytes and read 1 string
		
		OPCODE_IW_CallBuiltinFunction0,
		OPCODE_IW_CallBuiltinFunction1,
		OPCODE_IW_CallBuiltinFunction2,
		OPCODE_IW_CallBuiltinFunction3,
		OPCODE_IW_CallBuiltinFunction4,
		OPCODE_IW_CallBuiltinFunction5,

		OPCODE_IW_CallBuiltinMethod0,
		OPCODE_IW_CallBuiltinMethod1,
		OPCODE_IW_CallBuiltinMethod2,
		OPCODE_IW_CallBuiltinMethod3,
		OPCODE_IW_CallBuiltinMethod4,
		OPCODE_IW_CallBuiltinMethod5,

		OPCODE_IW_ScriptFunctionCall2,
		OPCODE_IW_Jump32,

		OPCODE_IW_LocalCall,
		OPCODE_IW_GetLocal,
		OPCODE_IW_LocalCall2,
		OPCODE_IW_LocalThreadCall,

		OPCODE_COUNT,
	};
	void RegisterOpCodesMap();

	const char* OpCodeName(OPCode op);
	OPCode OpCodeFromName(const char* op);
}