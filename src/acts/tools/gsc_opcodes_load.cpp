#include <includes.hpp>
#include <actscli.hpp>
#include "compatibility/scobalula_wni.hpp"
#include "compatibility/serious_db2.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_opcodes_load.hpp"
#include "tools/gsc_vm.hpp"

namespace tool::gsc::opcode {
	void RegisterOpCodesMap() {
		static std::once_flag f{};

		std::call_once(f, [] {
			tool::gsc::vm::RegisterVmOpCodes();

			// loading serious db2 files (if any)
			const char* seriousDBDir = actscli::options().seriousDBFile;
			std::filesystem::path seriousDBDirPath;
			if (!seriousDBDir) {
				seriousDBDirPath = utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir;
			}
			else {
				seriousDBDirPath = seriousDBDir;
			}

			std::vector<std::filesystem::path> dbFiles{};

			utils::GetFileRecurse(seriousDBDirPath, dbFiles, [](const std::filesystem::path& p) {
				auto s = p.string();
				return s.ends_with(".db2");
			});
			
			for (const auto& db : dbFiles) {
				compatibility::serious::db2::LoadVMDatabase(db);
			}

			std::filesystem::path actsopDirPath = utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir;
			std::vector<std::filesystem::path> actsopFiles{};

			utils::GetFileRecurse(actsopDirPath, actsopFiles, [](const std::filesystem::path& p) {
				auto s = p.string();
				return s.ends_with(".actsop");
			});

			for (const auto& db : actsopFiles) {
				// TODO: read opcode csv file

			}

		});
	}

	class OpCodeLookupTable {
	public:
		std::unordered_map<OPCode, const char*> opToName{};
		std::unordered_map<std::string, OPCode> nameToOp{};
	private:
		void RegisterOpCode(OPCode op, const char* name) {
			opToName[op] = name;
			auto& opl = nameToOp[utils::LowerCase(utils::CloneString(name))];
			if (opl && opl != op) {
				LOG_WARNING("Registered opcode with the same name : '{}' 0x{:x} != 0x{:x}", name, (int)opl, (int)op);
			}
			else {
				opl = op;
			}
		}
	public:
		OpCodeLookupTable() {
			RegisterOpCode(OPCODE_Undefined, "Undefined");
			RegisterOpCode(OPCODE_Unknown0, "Unknown0");
			RegisterOpCode(OPCODE_Unknown1, "Unknown1");
			RegisterOpCode(OPCODE_Unknown2, "Unknown2");
			RegisterOpCode(OPCODE_Unknown3, "Unknown3");
			RegisterOpCode(OPCODE_Unknown4, "Unknown4");
			RegisterOpCode(OPCODE_Unknown5, "Unknown5");
			RegisterOpCode(OPCODE_Unknown6, "Unknown6");
			RegisterOpCode(OPCODE_Unknown7, "Unknown7");
			RegisterOpCode(OPCODE_Unknowna, "Unknowna");
			RegisterOpCode(OPCODE_Unknownb, "Unknownb");
			RegisterOpCode(OPCODE_Unknown38, "Unknown38");
			RegisterOpCode(OPCODE_Nop, "Nop");
			RegisterOpCode(OPCODE_GetUndefined, "GetUndefined");
			RegisterOpCode(OPCODE_Unknown10e, "Unknown10e");
			RegisterOpCode(OPCODE_Unknown126, "Unknown126");
			RegisterOpCode(OPCODE_End, "End");
			RegisterOpCode(OPCODE_EvalLocalVariableCachedDebug, "EvalLocalVariableCachedDebug");
			RegisterOpCode(OPCODE_EvalLocalVariableRefCachedDebug, "EvalLocalVariableRefCachedDebug");
			RegisterOpCode(OPCODE_ClearParams, "ClearParams");
			RegisterOpCode(OPCODE_CheckClearParams, "CheckClearParams");
			RegisterOpCode(OPCODE_PreScriptCall, "PreScriptCall");
			RegisterOpCode(OPCODE_CallBuiltinFunction, "CallBuiltinFunction");
			RegisterOpCode(OPCODE_CallBuiltinMethod, "CallBuiltinMethod");
			RegisterOpCode(OPCODE_EmptyArray, "EmptyArray");
			RegisterOpCode(OPCODE_EmptyStruct, "EmptyStruct");
			RegisterOpCode(OPCODE_WaitTill, "WaitTill");
			RegisterOpCode(OPCODE_ClearFieldVariableOnStack, "ClearFieldVariableOnStack");
			RegisterOpCode(OPCODE_ClearArray, "ClearArray");
			RegisterOpCode(OPCODE_ScriptMethodThreadCallPointer, "ScriptMethodThreadCallPointer");
			RegisterOpCode(OPCODE_WaitTillMatchTimeout, "WaitTillMatchTimeout");
			RegisterOpCode(OPCODE_GetHash, "GetHash");
			RegisterOpCode(OPCODE_WaitFrame, "WaitFrame");
			RegisterOpCode(OPCODE_ScriptMethodThreadCallPointerEndOn, "ScriptMethodThreadCallPointerEndOn");
			RegisterOpCode(OPCODE_WaitTillMatch, "WaitTillMatch");
			RegisterOpCode(OPCODE_ScriptThreadCallEndOn, "ScriptThreadCallEndOn");
			RegisterOpCode(OPCODE_Switch, "Switch");
			RegisterOpCode(OPCODE_ScriptFunctionCallPointer, "ScriptFunctionCallPointer");
			RegisterOpCode(OPCODE_DevblockBegin, "DevblockBegin");
			RegisterOpCode(OPCODE_NotEqual, "NotEqual");
			RegisterOpCode(OPCODE_ScriptMethodCallPointer, "ScriptMethodCallPointer");
			RegisterOpCode(OPCODE_JumpOnTrue, "JumpOnTrue");
			RegisterOpCode(OPCODE_ScriptMethodThreadCallEndOn, "ScriptMethodThreadCallEndOn");
			RegisterOpCode(OPCODE_SafeCreateLocalVariables, "SafeCreateLocalVariables");
			RegisterOpCode(OPCODE_GetNegUnsignedShort, "GetNegUnsignedShort");
			RegisterOpCode(OPCODE_ClassFunctionCall, "ClassFunctionCall");
			RegisterOpCode(OPCODE_Return, "Return");
			RegisterOpCode(OPCODE_EvalFieldVariableRef, "EvalFieldVariableRef");
			RegisterOpCode(OPCODE_SafeDecTop, "SafeDecTop");
			RegisterOpCode(OPCODE_Bit_Or, "Bit_Or");
			RegisterOpCode(OPCODE_ScriptThreadCall, "ScriptThreadCall");
			RegisterOpCode(OPCODE_Bit_Xor, "Bit_Xor");
			RegisterOpCode(OPCODE_GetSelfObject, "GetSelfObject");
			RegisterOpCode(OPCODE_GetNegByte, "GetNegByte");
			RegisterOpCode(OPCODE_ScriptThreadCallPointerEndOn, "ScriptThreadCallPointerEndOn");
			RegisterOpCode(OPCODE_BoolComplement, "BoolComplement");
			RegisterOpCode(OPCODE_IsDefined, "IsDefined");
			RegisterOpCode(OPCODE_AddToArray, "AddToArray");
			RegisterOpCode(OPCODE_Wait, "Wait");
			RegisterOpCode(OPCODE_SuperEqual, "SuperEqual");
			RegisterOpCode(OPCODE_ScriptFunctionCall, "ScriptFunctionCall");
			RegisterOpCode(OPCODE_JumpOnTrueExpr, "JumpOnTrueExpr");
			RegisterOpCode(OPCODE_CreateArray, "CreateArray");
			RegisterOpCode(OPCODE_Inc, "Inc");
			RegisterOpCode(OPCODE_ShiftLeft, "ShiftLeft");
			RegisterOpCode(OPCODE_JumpOnGreaterThan, "JumpOnGreaterThan");
			RegisterOpCode(OPCODE_Plus, "Plus");
			RegisterOpCode(OPCODE_CastAndEvalFieldVariable, "CastAndEvalFieldVariable");
			RegisterOpCode(OPCODE_ShiftRight, "ShiftRight");
			RegisterOpCode(OPCODE_CreateStruct, "CreateStruct");
			RegisterOpCode(OPCODE_CastCanon, "CastCanon");
			RegisterOpCode(OPCODE_GreaterThanOrEqualTo, "GreaterThanOrEqualTo");
			RegisterOpCode(OPCODE_GetUIntPtr, "GetUIntPtr");
			RegisterOpCode(OPCODE_GetLongInteger, "GetLongInteger");
			RegisterOpCode(OPCODE_EvalArray, "EvalArray");
			RegisterOpCode(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd");
			RegisterOpCode(OPCODE_EndOnCallback, "EndOnCallback");
			RegisterOpCode(OPCODE_EndOn, "EndOn");
			RegisterOpCode(OPCODE_SuperNotEqual, "SuperNotEqual");
			RegisterOpCode(OPCODE_GetFloat, "GetFloat");
			RegisterOpCode(OPCODE_ProfileStart, "ProfileStart");
			RegisterOpCode(OPCODE_GetString, "GetString");
			RegisterOpCode(OPCODE_BoolNot, "BoolNot");
			RegisterOpCode(OPCODE_CastBool, "CastBool");
			RegisterOpCode(OPCODE_Equal, "Equal");
			RegisterOpCode(OPCODE_GetUnsignedInteger, "GetUnsignedInteger");
			RegisterOpCode(OPCODE_WaittillTimeout, "WaittillTimeout");
			RegisterOpCode(OPCODE_GreaterThan, "GreaterThan");
			RegisterOpCode(OPCODE_Jump, "Jump");
			RegisterOpCode(OPCODE_Divide, "Divide");
			RegisterOpCode(OPCODE_EndSwitch, "EndSwitch");
			RegisterOpCode(OPCODE_JumpOnFalse, "JumpOnFalse");
			RegisterOpCode(OPCODE_JumpOnFalseExpr, "JumpOnFalseExpr");
			RegisterOpCode(OPCODE_Minus, "Minus");
			RegisterOpCode(OPCODE_ProfileStop, "ProfileStop");
			RegisterOpCode(OPCODE_GetInteger, "GetInteger");
			RegisterOpCode(OPCODE_ClassFunctionThreadCall, "ClassFunctionThreadCall");
			RegisterOpCode(OPCODE_VectorScale, "VectorScale");
			RegisterOpCode(OPCODE_Modulus, "Modulus");
			RegisterOpCode(OPCODE_VectorConstant, "VectorConstant");
			RegisterOpCode(OPCODE_SizeOf, "SizeOf");
			RegisterOpCode(OPCODE_Notify, "Notify");
			RegisterOpCode(OPCODE_Vector, "Vector");
			RegisterOpCode(OPCODE_Dec, "Dec");
			RegisterOpCode(OPCODE_JumpOnLessThan, "JumpOnLessThan");
			RegisterOpCode(OPCODE_Bit_And, "Bit_And");
			RegisterOpCode(OPCODE_GetObjectType, "GetObjectType");
			RegisterOpCode(OPCODE_Multiply, "Multiply");
			RegisterOpCode(OPCODE_EvalFieldVariableOnStack, "EvalFieldVariableOnStack");
			RegisterOpCode(OPCODE_GetTime, "GetTime");
			RegisterOpCode(OPCODE_AddToStruct, "AddToStruct");
			RegisterOpCode(OPCODE_ClassFunctionThreadCallEndOn, "ClassFunctionThreadCallEndOn");
			RegisterOpCode(OPCODE_LessThanOrEqualTo, "LessThanOrEqualTo");
			RegisterOpCode(OPCODE_GetGlobal, "GetGlobal");
			RegisterOpCode(OPCODE_GetZero, "GetZero");
			RegisterOpCode(OPCODE_ClearFieldVariable, "ClearFieldVariable");
			RegisterOpCode(OPCODE_EvalFieldObjectFromRef, "EvalFieldObjectFromRef");
			RegisterOpCode(OPCODE_GetSelf, "GetSelf");
			RegisterOpCode(OPCODE_GetResolveFunction, "GetResolveFunction");
			RegisterOpCode(OPCODE_GetGlobalObject, "GetGlobalObject");
			RegisterOpCode(OPCODE_GetByte, "GetByte");
			RegisterOpCode(OPCODE_CastFieldObject, "CastFieldObject");
			RegisterOpCode(OPCODE_ScriptThreadCallPointer, "ScriptThreadCallPointer");
			RegisterOpCode(OPCODE_LessThan, "LessThan");
			RegisterOpCode(OPCODE_ScriptMethodCall, "ScriptMethodCall");
			RegisterOpCode(OPCODE_DecTop, "DecTop");
			RegisterOpCode(OPCODE_GetVector, "GetVector");
			RegisterOpCode(OPCODE_ScriptMethodThreadCall, "ScriptMethodThreadCall");
			RegisterOpCode(OPCODE_JumpPush, "JumpPush");
			RegisterOpCode(OPCODE_GetUnsignedShort, "GetUnsignedShort");
			RegisterOpCode(OPCODE_EvalLocalVariableCached, "EvalLocalVariableCached");
			RegisterOpCode(OPCODE_EvalFieldVariable, "EvalFieldVariable");
			RegisterOpCode(OPCODE_GetFunction, "GetFunction");
			RegisterOpCode(OPCODE_EvalArrayRef, "EvalArrayRef");
			RegisterOpCode(OPCODE_SetNextArrayKeyCached, "SetNextArrayKeyCached");
			RegisterOpCode(OPCODE_Unknown9e, "Unknown9e");
			RegisterOpCode(OPCODE_EvalLocalVariableCachedSafe, "EvalLocalVariableCachedSafe");
			RegisterOpCode(OPCODE_SetLocalVariableCachedOnStack, "SetLocalVariableCachedOnStack");
			RegisterOpCode(OPCODE_Unknownc7, "Unknownc7");
			RegisterOpCode(OPCODE_Unknown35, "Unknown35");
			RegisterOpCode(OPCODE_FirstArrayKey, "FirstArrayKey");
			RegisterOpCode(OPCODE_EvalFieldVariableOnStackRef, "EvalFieldVariableOnStackRef");
			RegisterOpCode(OPCODE_SetVariableFieldRef, "SetVariableFieldRef");
			RegisterOpCode(OPCODE_SetVariableField, "SetVariableField");
			RegisterOpCode(OPCODE_EvalSelfFieldVariable, "EvalSelfFieldVariable");
			RegisterOpCode(OPCODE_SetLocalVariableCached, "SetLocalVariableCached");
			RegisterOpCode(OPCODE_FirstArrayKeyCached, "FirstArrayKeyCached");
			RegisterOpCode(OPCODE_EvalLocalVariableRefCached, "EvalLocalVariableRefCached");
			RegisterOpCode(OPCODE_EvalGlobalObjectFieldVariable, "EvalGlobalObjectFieldVariable");
			RegisterOpCode(OPCODE_EvalLocalVariableDefined, "EvalLocalVariableDefined");
			RegisterOpCode(OPCODE_T8C_GetLazyFunction, "T8C_GetLazyFunction");
			RegisterOpCode(OPCODE_PixBeginEvent, "PixBeginEvent");
			RegisterOpCode(OPCODE_PixEndEvent, "PixEndEvent");
			RegisterOpCode(OPCODE_GetNegUnsignedInteger, "GetNegUnsignedInteger");
			RegisterOpCode(OPCODE_T9_EvalFieldVariableFromObjectCached, "EvalFieldVariableFromObjectCached");
			RegisterOpCode(OPCODE_T9_EvalFieldVariableFromObjectFromRef, "EvalFieldVariableFromObjectFromRef");
			RegisterOpCode(OPCODE_T9_SetFieldVariableFromObjectFromRef, "SetFieldVariableFromObjectFromRef");
			RegisterOpCode(OPCODE_T9_EvalLocalVariableCachedDouble, "EvalLocalVariableCachedDouble");
			RegisterOpCode(OPCODE_T9_EvalFieldVariableFromGlobalObject, "EvalFieldVariableFromGlobalObject");
			RegisterOpCode(OPCODE_T9_SetVariableFieldFromEvalArrayRef, "SetVariableFieldFromEvalArrayRef");
			RegisterOpCode(OPCODE_T9_IncLocalVariableCached, "IncLocalVariableCached");
			RegisterOpCode(OPCODE_T9_DecLocalVariableCached, "DecLocalVariableCached");
			RegisterOpCode(OPCODE_T9_EvalArrayCached, "EvalArrayCached");
			RegisterOpCode(OPCODE_T9_EndOnCallbackParam, "EndOnCallbackParam");
			RegisterOpCode(OPCODE_T9_GetVarRef, "GetVarRef");
			RegisterOpCode(OPCODE_T9_IteratorKey, "IteratorKey");
			RegisterOpCode(OPCODE_T9_IteratorVal, "IteratorVal");
			RegisterOpCode(OPCODE_T9_IteratorNext, "IteratorNext");
			RegisterOpCode(OPCODE_IW_RegisterVariable, "RegisterVariable");
			RegisterOpCode(OPCODE_IW_GetAnimation, "GetAnimation");
			RegisterOpCode(OPCODE_IW_GetAnimationTree, "GetAnimationTree");
			RegisterOpCode(OPCODE_IW_GetBuiltinFunction, "GetBuiltinFunction");
			RegisterOpCode(OPCODE_IW_GetBuiltinMethod, "GetBuiltinMethod");
			RegisterOpCode(OPCODE_IW_GetIString, "GetIString");
			RegisterOpCode(OPCODE_IW_GetEmptyUnkE, "GetEmptyUnkE");
			RegisterOpCode(OPCODE_IW_GetTagHash, "GetTagHash");
			RegisterOpCode(OPCODE_IW_GetResourceHash, "GetResourceHash");
			RegisterOpCode(OPCODE_IW_GetDVarHash, "GetDVarHash");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached0, "EvalLocalVariableCached0");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached1, "EvalLocalVariableCached1");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached2, "EvalLocalVariableCached2");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached3, "EvalLocalVariableCached3");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached4, "EvalLocalVariableCached4");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableCached5, "EvalLocalVariableCached5");
			RegisterOpCode(OPCODE_IW_EvalLocalArrayCached, "EvalLocalArrayCached");
			RegisterOpCode(OPCODE_IW_EvalLocalVariableObjectCached, "EvalLocalVariableObjectCached");
			RegisterOpCode(OPCODE_IW_ClearFieldVariableRef, "ClearFieldVariableRef");
			RegisterOpCode(OPCODE_IW_EvalLevelFieldVariableRef, "EvalLevelFieldVariableRef");
			RegisterOpCode(OPCODE_IW_EvalLevelFieldVariable, "EvalLevelFieldVariable");
			RegisterOpCode(OPCODE_IW_EvalSelfFieldVar, "EvalSelfFieldVar");
			RegisterOpCode(OPCODE_IW_EvalSelfFieldVarRef, "EvalSelfFieldVarRef");
			RegisterOpCode(OPCODE_IW_EvalAnimFieldVar, "EvalAnimFieldVar");
			RegisterOpCode(OPCODE_IW_EvalAnimFieldVarRef, "EvalAnimFieldVarRef");
			RegisterOpCode(OPCODE_IW_SetAnimFieldVar, "SetAnimFieldVar");
			RegisterOpCode(OPCODE_IW_SetLevelFieldVariable, "SetLevelFieldVariable");
			RegisterOpCode(OPCODE_IW_SetSelfFieldVar, "SetSelfFieldVar");
			RegisterOpCode(OPCODE_IW_DevBlock, "DevBlock");
			RegisterOpCode(OPCODE_IW_SingleEndon, "SingleEndon");
			RegisterOpCode(OPCODE_IW_SingleWaitTill, "SingleWaitTill");
			RegisterOpCode(OPCODE_IW_IsTrue, "IsTrue");
			RegisterOpCode(OPCODE_IW_Switch, "IWSwitch");
			RegisterOpCode(OPCODE_IW_EndSwitch, "IWEndSwitch");
			RegisterOpCode(OPCODE_IW_SetWaittillVariableFieldCached, "SetWaittillVariableFieldCached");
			RegisterOpCode(OPCODE_IW_WaitFrame, "IWWaitFrame");
			RegisterOpCode(OPCODE_IW_AddToStruct, "IWAddToStruct");
			RegisterOpCode(OPCODE_IW_BuiltinFunctionCallPointer, "BuiltinFunctionCallPointer");
			RegisterOpCode(OPCODE_IW_BuiltinMethodCallPointer, "BuiltinMethodCallPointer");
			RegisterOpCode(OPCODE_IW_GetLevel, "GetLevel");
			RegisterOpCode(OPCODE_IW_GetLevelGRef, "GetLevelGRef");
			RegisterOpCode(OPCODE_IW_GetGame, "GetGame");
			RegisterOpCode(OPCODE_IW_GetGameRef, "GetGameRef");
			RegisterOpCode(OPCODE_IW_GetAnim, "GetAnim");
			RegisterOpCode(OPCODE_GetAnimGRef, "GetAnimGRef");
			RegisterOpCode(OPCODE_IW_EvalArrayCachedField, "EvalArrayCachedField");
			RegisterOpCode(OPCODE_IW_GetThread, "GetThread");
			RegisterOpCode(OPCODE_IW_Notify, "IWNotify");
			RegisterOpCode(OPCODE_IW_WaitTillMatch, "IWWaitTillMatch");
			RegisterOpCode(OPCODE_DevOp, "DevOp");
			RegisterOpCode(OPCODE_GetSignedByte, "GetSignedByte");
			RegisterOpCode(OPCODE_GetShort, "GetShort");
			RegisterOpCode(OPCODE_GetHash32, "GetHash32");
			RegisterOpCode(OPCODE_NextArrayKey, "NextArrayKey");
			RegisterOpCode(OPCODE_T7_ProfileStart, "T7ProfileStart");
			RegisterOpCode(OPCODE_GetClasses, "GetClasses");
			RegisterOpCode(OPCODE_GetClassesObject, "GetClassesObject");
			RegisterOpCode(OPCODE_GetWorld, "GetWorld");
			RegisterOpCode(OPCODE_GetWorldObject, "GetWorldObject");
			RegisterOpCode(OPCODE_Wait2, "Wait2");
			RegisterOpCode(OPCODE_WaitTillMatch2, "WaitTillMatch2");
			RegisterOpCode(OPCODE_IgnoreWaittillVariableFieldCached, "IgnoreWaittillVariableFieldCached");
			RegisterOpCode(OPCODE_IW_RegisterMultipleVariables, "RegisterMultipleVariables");
			RegisterOpCode(OPCODE_Abort, "Abort");
			RegisterOpCode(OPCODE_Breakpoint, "Breakpoint");
			RegisterOpCode(OPCODE_AutoBreakpoint, "AutoBreakpoint");
			RegisterOpCode(OPCODE_ErrorBreakpoint, "ErrorBreakpoint");
			RegisterOpCode(OPCODE_WatchBreakpoint, "WatchBreakpoint");
			RegisterOpCode(OPCODE_NotifyBreakpoint, "NotifyBreakpoint");
			RegisterOpCode(OPCODE_ThreadEndBreakpoint, "ThreadEndBreakpoint");
			RegisterOpCode(OPCODE_PushVar, "PushVar");
			RegisterOpCode(OPCODE_PushEntityVar, "PushEntityVar");
			RegisterOpCode(OPCODE_DEV_Consume0, "DevConsume0");
			RegisterOpCode(OPCODE_DEV_Consume1, "DevConsume1");
			RegisterOpCode(OPCODE_DEV_Consume2, "DevConsume2");
			RegisterOpCode(OPCODE_DEV_Consume4, "DevConsume4");
			RegisterOpCode(OPCODE_DEV_Consume8, "DevConsume8");
			RegisterOpCode(OPCODE_DEV_Consume9, "DevConsume9");
			RegisterOpCode(OPCODE_DEV_Consume0Push, "DevConsume0Push");
			RegisterOpCode(OPCODE_DEV_Consume1Push, "DevConsume1Push");
			RegisterOpCode(OPCODE_DEV_Consume2Push, "DevConsume2Push");
			RegisterOpCode(OPCODE_DEV_Consume4Push, "DevConsume4Push");
			RegisterOpCode(OPCODE_DEV_Consume8Push, "DevConsume8Push");
			RegisterOpCode(OPCODE_DEV_Consume9Push, "DevConsume9Push");
			RegisterOpCode(OPCODE_JumpOnDefined, "JumpOnDefined");
			RegisterOpCode(OPCODE_JumpOnDefinedExpr, "JumpOnDefinedExpr");
			RegisterOpCode(OPCODE_T10_GetScrHash, "GetScrHash");
			RegisterOpCode(OPCODE_T10_FlatArgs, "FlatArgs");
			RegisterOpCode(OPCODE_T10_GreaterThanOrSuperEqualTo, "GreaterThanOrSuperEqualTo");
			RegisterOpCode(OPCODE_T10_LowerThanOrSuperEqualTo, "LowerThanOrSuperEqualTo");
			RegisterOpCode(OPCODE_IW_GetResourceHash2, "GetResourceHash2");
			RegisterOpCode(OPCODE_IW_GetPositionRef, "GetPositionRef");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_0, "GscBinSkip0");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_1, "GscBinSkip1");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_2, "GscBinSkip2");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_3, "GscBinSkip3");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_4, "GscBinSkip4");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_5, "GscBinSkip5");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_N, "GscBinSkipN");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_3BC_4SD, "GscBinSkip3BC4SD");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_4BC_4SD, "GscBinSkip4BC4SD");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_STR_TOKEN, "GscBinSkipSTRTOKEN");
			RegisterOpCode(OPCODE_GSCBIN_SKIP_4BC_1STR, "GscBinSkip4BC1STR");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction0, "CallBuiltinFunction0");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction1, "CallBuiltinFunction1");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction2, "CallBuiltinFunction2");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction3, "CallBuiltinFunction3");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction4, "CallBuiltinFunction4");
			RegisterOpCode(OPCODE_IW_CallBuiltinFunction5, "CallBuiltinFunction5");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod0, "CallBuiltinMethod0");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod1, "CallBuiltinMethod1");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod2, "CallBuiltinMethod2");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod3, "CallBuiltinMethod3");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod4, "CallBuiltinMethod4");
			RegisterOpCode(OPCODE_IW_CallBuiltinMethod5, "CallBuiltinMethod5");
			RegisterOpCode(OPCODE_IW_ScriptFunctionCall2, "ScriptFunctionCall2");
			RegisterOpCode(OPCODE_IW_Jump32, "Jump32");
			RegisterOpCode(OPCODE_IW_LocalCall, "LocalCall");
			RegisterOpCode(OPCODE_IW_GetLocal, "GetLocal");
			RegisterOpCode(OPCODE_IW_LocalCall2, "LocalCall2");
			RegisterOpCode(OPCODE_IW_LocalThreadCall, "LocalThreadCall");
			RegisterOpCode(OPCODE_InvalidOpCode, "InvalidOpCode");
		}
	};
	namespace {
		static OpCodeLookupTable table{};
	}

	const char* OpCodeName(OPCode op) {
		auto it = table.opToName.find(op);
		return it != table.opToName.end() ? it->second : nullptr;
	}
	OPCode OpCodeFromName(const char* op) {
		auto it = table.nameToOp.find(op);
		return it != table.nameToOp.end() ? it->second : OPCODE_Undefined;
	}
	int dop(Process& proc, int argc, const char* argv[]) {
		std::filesystem::path outFile;
		if (argc == 2) {
			outFile = "opcodes.actsop";
		}
		else {
			outFile = argv[2];
		}
		tool::gsc::opcode::RegisterOpCodesMap();
		const std::unordered_map<uint64_t, VmInfo>& maps{ opcode::GetVMMaps() };

		std::ofstream os{ outFile };

		os << "VM,PLATFORM,OPCODE,VALUES";
		for (auto& [vm, vmInfo] : maps) {
			const char* vmname{ utils::UpperCase(utils::CloneString(vmInfo.codeName)) };
			for (auto& [plt, lookupmap] : vmInfo.opcodemappltlookup) {
				const char* pltname{ utils::UpperCase(utils::CloneString(PlatformName(plt))) };
				LOG_INFO("Dump {}/{}", vmname, pltname);
				std::unordered_set<uint16_t> defined{};
				for (auto& [opco, opcodes] : lookupmap) {

					const char* opcodename{ OpCodeName(opco) };
					if (!opcodename) {
						LOG_ERROR("Invalid opcode : {}", (int)opco);
						opcodename = "<error>";
					}
					os << "\n" << vmname << "," << pltname << "," << opcodename << ",";

					size_t i{};
					for (auto& opcode : opcodes) {
						if (i++) os << ";";
						os << "0x" << std::hex << opcode;
						defined.insert(opcode);
					}
				}
				if (defined.size() < vmInfo.maxOpCode) {
					os << "\n" << vmname << "," << pltname << ",Undefined,";
					bool def{};
					for (size_t i = 0; i <= vmInfo.maxOpCode; i++) {
						if (!defined.contains((uint16_t)i)) {
							if (def) {
								os << ";";
							}
							else {
								def = true;
							}
							os << "0x" << std::hex << i;
						}
					}
				}
			}
		}

		os.close();
		LOG_INFO("Dump into {}", outFile.string());

		return tool::OK;
	}

	int opcodetest(Process& proc, int argc, const char* argv[]) {
		LOG_INFO("{}", utils::PtrOrElse(OpCodeName(OPCODE_End), "null"));
		return tool::OK;
	}

	ADD_TOOL(opcodetest, "dev", "", "test opcodes names lookup", nullptr, opcodetest);
	ADD_TOOL(dop, "gsc", " [file=opcodes.actsop]", "dump opcodes", nullptr, dop);

}