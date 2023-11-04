#include <includes.hpp>

using namespace tool::gsc::opcode;
using namespace compatibility::serious;

namespace {

	class LookupMaps {
	private:
		std::unordered_map<OPCode, SeriousId> opCodeToSID{};
		std::unordered_map<SeriousId, OPCode> sIdToOpCode{};

	public:
		void Add(SeriousId sid, OPCode op) {
			opCodeToSID[op] = sid;
			sIdToOpCode[sid] = op;
		}

		bool operator!() {
			return opCodeToSID.empty();
		}

		OPCode ConvertFrom(SeriousId id) {
			auto f = sIdToOpCode.find(id);
			if (f == sIdToOpCode.end()) {
				return OPCODE_Undefined;
			}

			return f->second;
		}

		SeriousId ConvertTo(OPCode id) {
			auto f = opCodeToSID.find(id);
			if (f == opCodeToSID.end()) {
				return SERID_Invalid;
			}

			return f->second;
		}
	};


	LookupMaps& Maps() {
		static LookupMaps lmaps{};

		if (!lmaps) {
			// init lookup
			lmaps.Add(SERID_Invalid, OPCODE_Undefined);
			lmaps.Add(SERID_Nop, OPCODE_Nop);
			lmaps.Add(SERID_End, OPCODE_End);
			lmaps.Add(SERID_Return, OPCODE_Return);
			lmaps.Add(SERID_GetUndefined, OPCODE_GetUndefined);
			lmaps.Add(SERID_GetZero, OPCODE_GetZero);
			lmaps.Add(SERID_GetByte, OPCODE_GetByte);
			lmaps.Add(SERID_GetNegByte, OPCODE_GetNegByte);
			lmaps.Add(SERID_GetUnsignedShort, OPCODE_GetUnsignedShort);
			lmaps.Add(SERID_GetNegUnsignedShort, OPCODE_GetNegUnsignedShort);
			lmaps.Add(SERID_GetInteger, OPCODE_GetInteger);
			lmaps.Add(SERID_GetFloat, OPCODE_GetFloat);
			lmaps.Add(SERID_GetString, OPCODE_GetString);
			lmaps.Add(SERID_GetVector, OPCODE_GetVector);
			lmaps.Add(SERID_GetSelf, OPCODE_GetSelf);
			lmaps.Add(SERID_GetFunction, OPCODE_GetFunction);
			lmaps.Add(SERID_SafeCreateLocalVariables, OPCODE_SafeCreateLocalVariables);
			lmaps.Add(SERID_EvalLocalVariableCached, OPCODE_EvalLocalVariableCached);
			lmaps.Add(SERID_EvalArray, OPCODE_EvalArray);
			lmaps.Add(SERID_EvalArrayRef, OPCODE_EvalArrayRef);
			lmaps.Add(SERID_ClearArray, OPCODE_ClearArray);
			lmaps.Add(SERID_GetEmptyArray, OPCODE_EmptyArray);
			lmaps.Add(SERID_GetSelfObject, OPCODE_GetSelfObject);
			lmaps.Add(SERID_EvalFieldVariable, OPCODE_EvalFieldVariable);
			lmaps.Add(SERID_EvalFieldVariableRef, OPCODE_EvalFieldVariableRef);
			lmaps.Add(SERID_ClearFieldVariable, OPCODE_ClearFieldVariable);
			lmaps.Add(SERID_ClearParams, OPCODE_ClearParams);
			lmaps.Add(SERID_CheckClearParams, OPCODE_CheckClearParams);
			lmaps.Add(SERID_EvalLocalVariableRefCached, OPCODE_EvalLocalVariableRefCached);
			lmaps.Add(SERID_SetVariableField, OPCODE_SetVariableField);
			lmaps.Add(SERID_CallBuiltin, OPCODE_CallBuiltinFunction);
			lmaps.Add(SERID_CallBuiltinMethod, OPCODE_CallBuiltinMethod);
			lmaps.Add(SERID_Wait, OPCODE_Wait);
			lmaps.Add(SERID_WaitTillFrameEnd, OPCODE_WaitTillFrameEnd);
			lmaps.Add(SERID_PreScriptCall, OPCODE_PreScriptCall);
			lmaps.Add(SERID_ScriptFunctionCall, OPCODE_ScriptFunctionCall);
			lmaps.Add(SERID_ScriptFunctionCallPointer, OPCODE_ScriptFunctionCallPointer);
			lmaps.Add(SERID_ScriptMethodCall, OPCODE_ScriptMethodCall);
			lmaps.Add(SERID_ScriptMethodCallPointer, OPCODE_ScriptMethodCallPointer);
			lmaps.Add(SERID_ScriptThreadCall, OPCODE_ScriptThreadCall);
			lmaps.Add(SERID_ScriptThreadCallPointer, OPCODE_ScriptThreadCallPointer);
			lmaps.Add(SERID_ScriptMethodThreadCall, OPCODE_ScriptMethodThreadCall);
			lmaps.Add(SERID_ScriptMethodThreadCallPointer, OPCODE_ScriptMethodThreadCallPointer);
			lmaps.Add(SERID_DecTop, OPCODE_DecTop);
			lmaps.Add(SERID_CastFieldObject, OPCODE_CastFieldObject);
			lmaps.Add(SERID_CastBool, OPCODE_CastBool);
			lmaps.Add(SERID_BoolNot, OPCODE_BoolNot);
			lmaps.Add(SERID_BoolComplement, OPCODE_BoolComplement);
			lmaps.Add(SERID_JumpOnFalse, OPCODE_JumpOnFalse);
			lmaps.Add(SERID_JumpOnTrue, OPCODE_JumpOnTrue);
			lmaps.Add(SERID_JumpOnFalseExpr, OPCODE_JumpOnFalseExpr);
			lmaps.Add(SERID_JumpOnTrueExpr, OPCODE_JumpOnTrueExpr);
			lmaps.Add(SERID_Jump, OPCODE_Jump);
			lmaps.Add(SERID_JumpBack, OPCODE_JumpPush);
			lmaps.Add(SERID_Inc, OPCODE_Inc);
			lmaps.Add(SERID_Dec, OPCODE_Dec);
			lmaps.Add(SERID_Bit_Or, OPCODE_Bit_Or);
			lmaps.Add(SERID_Bit_Xor, OPCODE_Bit_Xor);
			lmaps.Add(SERID_Bit_And, OPCODE_Bit_And);
			lmaps.Add(SERID_Equal, OPCODE_Equal);
			lmaps.Add(SERID_NotEqual, OPCODE_NotEqual);
			lmaps.Add(SERID_LessThan, OPCODE_LessThan);
			lmaps.Add(SERID_GreaterThan, OPCODE_GreaterThan);
			lmaps.Add(SERID_LessThanOrEqualTo, OPCODE_LessThanOrEqualTo);
			lmaps.Add(SERID_GreaterThanOrEqualTo, OPCODE_GreaterThanOrEqualTo);
			lmaps.Add(SERID_ShiftLeft, OPCODE_ShiftLeft);
			lmaps.Add(SERID_ShiftRight, OPCODE_ShiftRight);
			lmaps.Add(SERID_Plus, OPCODE_Plus);
			lmaps.Add(SERID_Minus, OPCODE_Minus);
			lmaps.Add(SERID_Multiply, OPCODE_Multiply);
			lmaps.Add(SERID_Divide, OPCODE_Divide);
			lmaps.Add(SERID_Modulus, OPCODE_Modulus);
			lmaps.Add(SERID_SizeOf, OPCODE_SizeOf);
			lmaps.Add(SERID_WaitTillMatch, OPCODE_WaitTillMatch);
			lmaps.Add(SERID_WaitTill, OPCODE_WaitTill);
			lmaps.Add(SERID_Notify, OPCODE_Notify);
			lmaps.Add(SERID_EndOn, OPCODE_EndOn);
			lmaps.Add(SERID_Switch, OPCODE_Switch);
			lmaps.Add(SERID_EndSwitch, OPCODE_EndSwitch);
			lmaps.Add(SERID_Vector, OPCODE_Vector);
			lmaps.Add(SERID_GetHash, OPCODE_GetHash);
			lmaps.Add(SERID_VectorConstant, OPCODE_VectorConstant);
			lmaps.Add(SERID_IsDefined, OPCODE_IsDefined);
			lmaps.Add(SERID_VectorScale, OPCODE_VectorScale);
			lmaps.Add(SERID_GetTime, OPCODE_GetTime);
			lmaps.Add(SERID_FirstArrayKey, OPCODE_FirstArrayKey);
			lmaps.Add(SERID_ProfileStart, OPCODE_ProfileStart);
			lmaps.Add(SERID_ProfileStop, OPCODE_ProfileStop);
			lmaps.Add(SERID_SafeDecTop, OPCODE_SafeDecTop);
			lmaps.Add(SERID_Nop, OPCODE_Nop);
			lmaps.Add(SERID_DevblockBegin, OPCODE_DevblockBegin);
			lmaps.Add(SERID_GetObjectType, OPCODE_GetObjectType);
			lmaps.Add(SERID_ClassFunctionCall, OPCODE_ClassFunctionCall);
			lmaps.Add(SERID_EvalSelfFieldVariable, OPCODE_EvalSelfFieldVariable);
			lmaps.Add(SERID_SuperEqual, OPCODE_SuperEqual);
			lmaps.Add(SERID_SuperNotEqual, OPCODE_SuperNotEqual);
			lmaps.Add(SERID_SetLocalVariableCached, OPCODE_SetLocalVariableCached);
			lmaps.Add(SERID_EvalLocalVariableDefined, OPCODE_EvalLocalVariableDefined);
			lmaps.Add(SERID_JumpOnGreaterThan, OPCODE_JumpOnGreaterThan);
			lmaps.Add(SERID_SetVariableFieldRef, OPCODE_SetVariableFieldRef);
			lmaps.Add(SERID_CreateStruct, OPCODE_CreateStruct);
			lmaps.Add(SERID_AddToStruct, OPCODE_AddToStruct);
			lmaps.Add(SERID_GetGlobalObject, OPCODE_GetGlobal);
			lmaps.Add(SERID_JumpOnLessThan, OPCODE_JumpOnLessThan);
			lmaps.Add(SERID_GetGlobalObjectRef, OPCODE_GetGlobalObject);
			lmaps.Add(SERID_EvalGlobalObjectFieldVariable, OPCODE_EvalGlobalObjectFieldVariable);
			lmaps.Add(SERID_WaitFrame, OPCODE_WaitFrame);
			lmaps.Add(SERID_CastAndEvalFieldVariable, OPCODE_CastAndEvalFieldVariable);
			lmaps.Add(SERID_FirstArrayKeyCached, OPCODE_FirstArrayKeyCached);
			lmaps.Add(SERID_SetNextArrayKeyCached, OPCODE_SetNextArrayKeyCached);
			lmaps.Add(SERID_GetAPIFunction, OPCODE_GetResolveFunction);
			lmaps.Add(SERID_GetUnsignedInteger, OPCODE_GetUnsignedInteger);
			lmaps.Add(SERID_CastVariableName, OPCODE_CastCanon);
			lmaps.Add(SERID_EvalFieldVariableOnStack, OPCODE_EvalFieldVariableOnStack);
			lmaps.Add(SERID_EvalFieldVariableOnStackRef, OPCODE_EvalFieldVariableOnStackRef);
			lmaps.Add(SERID_EndOnCallback, OPCODE_EndOnCallback);
			lmaps.Add(SERID_WaittillTimeout, OPCODE_WaittillTimeout);
			lmaps.Add(SERID_AddToArray, OPCODE_AddToArray);
			lmaps.Add(SERID_ClassFunctionThreadCall, OPCODE_ClassFunctionThreadCall);
			lmaps.Add(SERID_ScriptThreadCall2, OPCODE_ScriptThreadCallEndOn);
			lmaps.Add(SERID_ScriptThreadCallPointer2, OPCODE_ScriptThreadCallPointerEndOn);
			lmaps.Add(SERID_ScriptMethodThreadCall2, OPCODE_ScriptMethodThreadCallEndOn);
			lmaps.Add(SERID_ScriptMethodThreadCallPointer2, OPCODE_ScriptMethodThreadCallPointerEndOn);
			lmaps.Add(SERID_ClassFunctionThreadCall2, OPCODE_ClassFunctionThreadCallEndOn);
		}

		return lmaps;
	}
}

OPCode compatibility::serious::ConvertFrom(SeriousId id) {
	return Maps().ConvertFrom(id);
}

compatibility::serious::SeriousId compatibility::serious::ConvertTo(OPCode id) {
	return Maps().ConvertTo(id);
}
