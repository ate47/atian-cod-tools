#include <includes.hpp>
#include "serious_opcodes_txt.hpp"

namespace compatibility::serious::txt {
    using namespace tool::gsc::opcode;

    namespace {
        struct TranslationMap {
            template<typename T>
            struct DataVm {
                T defaultVal{};
                std::unordered_map<VM, T> vmDep{};

                template<typename... VmDep>
                void AddOpVm(VM vm, T vmop, VmDep... dep) {
                    vmDep[vm] = vmop;
                    AddOpVm(dep...);
                }
                void AddOpVm() {}

                template<typename A, typename... VmDep>
                void AddOpVmLookup(T vmop, VM vm, A any, VmDep... dep) {
                    vmDep[vm] = vmop;
                    AddOpVmLookup(vmop, dep...);
                }
                void AddOpVmLookup(T vmop) {}

                T FindForVm(VM vm) {
                    auto it{ vmDep.find(vm) };
                    if (it == vmDep.end()) return defaultVal;
                    return it->second;
                }
            };

            std::unordered_map<uint64_t, DataVm<OPCode>> idToOpcode{};
            std::unordered_map<OPCode, DataVm<const char*>> opcodeToId{};

            template<typename... VmDep>
            inline void AddOp(const char* str, OPCode op, VmDep... dep) {
                auto& ito{ idToOpcode[hashutils::Hash64(str)] };
                auto& oti{ opcodeToId[op] };

                ito.defaultVal = op;
                ito.AddOpVm(dep...);
                oti.defaultVal = str;
                oti.AddOpVmLookup(str, dep...);
            }

            TranslationMap() {
                AddOp("AddToArray", OPCODE_AddToArray);
                AddOp("AddToStruct", OPCODE_AddToStruct);
                AddOp("Bit_And", OPCODE_Bit_And);
                AddOp("Bit_Not", OPCODE_BoolComplement);
                AddOp("Bit_Or", OPCODE_Bit_Or);
                AddOp("Bit_Xor", OPCODE_Bit_Xor);
                AddOp("BoolNot", OPCODE_BoolNot);
                AddOp("CallBuiltin", OPCODE_CallBuiltinFunction);
                AddOp("CallBuiltinMethod", OPCODE_CallBuiltinMethod);
                AddOp("CastAndEvalFieldVariable", OPCODE_CastAndEvalFieldVariable);
                AddOp("CastBool", OPCODE_CastBool);
                AddOp("CastFieldObject", OPCODE_CastFieldObject);
                AddOp("CastVariableName", OPCODE_CastCanon);
                AddOp("CheckClearParams", OPCODE_CheckClearParams);
                AddOp("ClassFunctionCall", OPCODE_ClassFunctionCall);
                AddOp("ClassFunctionThreadCall", OPCODE_ClassFunctionThreadCall);
                AddOp("ClearArray", OPCODE_ClearArray);
                AddOp("ClearFieldVariable", OPCODE_ClearFieldVariable);
                AddOp("ClearFieldVariableOnStack", OPCODE_ClearFieldVariableOnStack);
                AddOp("ClearParams", OPCODE_ClearParams);
                AddOp("CreateStruct", OPCODE_CreateStruct);
                AddOp("Dec", OPCODE_Dec);
                AddOp("DecCached", OPCODE_T9_DecLocalVariableCached);
                AddOp("DecTop", OPCODE_DecTop);
                AddOp("DevblockBegin", OPCODE_DevblockBegin);
                AddOp("DiscardWaittillVariableField", OPCODE_IgnoreWaittillVariableFieldCached);
                AddOp("Divide", OPCODE_Divide);
                AddOp("End", OPCODE_End);
                AddOp("EndOn", OPCODE_EndOn);
                AddOp("EndOn", OPCODE_IW_SingleEndon);
                AddOp("EndOnCallback", OPCODE_EndOnCallback);
                AddOp("EndonCallbackA", OPCODE_T9_EndOnCallbackParam);
                AddOp("EndSwitch", OPCODE_EndSwitch);
                AddOp("Equal", OPCODE_Equal);
                AddOp("EvalArray", OPCODE_EvalArray);
                AddOp("EvalArrayRef", OPCODE_EvalArrayRef);
                AddOp("EvalFieldVariable", OPCODE_EvalFieldVariable);
                AddOp("EvalFieldVariableCached", OPCODE_T9_EvalFieldVariableFromObjectFromRef);
                AddOp("EvalFieldVariableOnStack", OPCODE_EvalFieldVariableOnStack);
                AddOp("EvalFieldVariableOnStackRef", OPCODE_EvalFieldVariableOnStackRef);
                AddOp("EvalFieldVariableRef", OPCODE_EvalFieldVariableRef);
                AddOp("EvalFieldVariableRefCached", OPCODE_T9_EvalFieldVariableFromObjectCached);
                AddOp("EvalGlobalObjectFieldVariable", OPCODE_EvalGlobalObjectFieldVariable);
                AddOp("EvalGlobalObjectFieldVariableRef", OPCODE_T9_EvalFieldVariableFromGlobalObject);
                AddOp("EvalLevelFieldVariable", OPCODE_IW_EvalLevelFieldVariable);
                AddOp("EvalLevelFieldVariableRef", OPCODE_IW_EvalLevelFieldVariableRef);
                AddOp("EvalLocalArrayCached", OPCODE_T9_EvalArrayCached);
                AddOp("EvalLocalVariableCached", OPCODE_EvalLocalVariableCached);
                AddOp("EvalLocalVariableCached2", OPCODE_EvalLocalVariableCachedSafe);
                AddOp("EvalLocalVariableDefined", OPCODE_EvalLocalVariableDefined);
                AddOp("EvalLocalVariableRefCached", OPCODE_EvalFieldObjectFromRef);
                AddOp("EvalLocalVariableRefCached", OPCODE_EvalLocalVariableRefCached);
                AddOp("EvalLocalVariableRefCached2", OPCODE_EvalLocalVariableRefCached);
                AddOp("EvalLocalVariablesCached", OPCODE_T9_EvalLocalVariableCachedDouble);
                AddOp("EvalSelfFieldVariable", OPCODE_IW_EvalSelfFieldVar);
                AddOp("EvalSelfFieldVariable", OPCODE_EvalSelfFieldVariable);
                AddOp("EvalSelfFieldVariableRef", OPCODE_IW_EvalSelfFieldVarRef);
                AddOp("FirstArrayKey", OPCODE_FirstArrayKey);
                AddOp("GetAnim", OPCODE_IW_GetAnim);
                AddOp("GetAnimation", OPCODE_IW_GetAnimation);
                AddOp("GetAnimObject", OPCODE_GetAnimGRef);
                AddOp("GetAPIFunction", OPCODE_GetResolveFunction);
                AddOp("GetArrayKeyIndex", OPCODE_T9_IteratorKey);
                AddOp("GetArrayValue", OPCODE_T9_IteratorVal);
                AddOp("GetByte", OPCODE_GetByte, VM::VM_T7, OPCODE_GetSignedByte, VM::VM_T71B, OPCODE_GetSignedByte);
                AddOp("GetClassesObject", OPCODE_GetClassesObject);
                AddOp("GetEmptyArray", OPCODE_CreateArray);
                AddOp("GetEmptyArray", OPCODE_EmptyArray);
                AddOp("GetFloat", OPCODE_GetFloat);
                AddOp("GetFunction", OPCODE_GetFunction);
                AddOp("GetGame", OPCODE_IW_GetGame);
                AddOp("GetGameRef", OPCODE_IW_GetGameRef);
                AddOp("GetGlobalObject", OPCODE_GetGlobal);
                AddOp("GetGlobalObjectRef", OPCODE_GetGlobalObject);
                AddOp("GetHash", OPCODE_GetHash, VM::VM_T7, OPCODE_GetHash32, VM::VM_T71B, OPCODE_GetHash32);
                AddOp("GetInteger", OPCODE_GetInteger);
                AddOp("GetIString", OPCODE_IW_GetIString);
                AddOp("GetLevel", OPCODE_IW_GetLevel);
                AddOp("GetLevelObject", OPCODE_IW_GetLevelGRef);
                AddOp("GetLocalFunction", OPCODE_Nop);
                AddOp("GetNegByte", OPCODE_GetNegByte);
                AddOp("GetNegUnsignedInteger", OPCODE_GetNegUnsignedInteger);
                AddOp("GetNegUnsignedShort", OPCODE_GetNegUnsignedShort);
                AddOp("GetObjectHandle", OPCODE_T9_GetVarRef);
                AddOp("GetObjectType", OPCODE_GetObjectType);
                AddOp("GetSelf", OPCODE_GetSelf);
                AddOp("GetSelfObject", OPCODE_GetSelfObject);
                AddOp("GetString", OPCODE_GetString);
                AddOp("GetTime", OPCODE_GetTime);
                AddOp("GetUndefined", OPCODE_GetUndefined);
                AddOp("GetUnsignedInteger", OPCODE_GetUnsignedInteger);
                AddOp("GetUnsignedShort", OPCODE_GetUnsignedShort, VM::VM_T7, OPCODE_GetShort, VM::VM_T71B, OPCODE_GetShort);
                AddOp("GetWorldObject", OPCODE_GetWorldObject);
                AddOp("GetZero", OPCODE_GetZero);
                AddOp("GreaterThan", OPCODE_GreaterThan);
                AddOp("GreaterThanOrEqualTo", OPCODE_GreaterThanOrEqualTo);
                AddOp("Inc", OPCODE_Inc);
                AddOp("IncCached", OPCODE_T9_IncLocalVariableCached);
                AddOp("IsDefined", OPCODE_IsDefined);
                AddOp("Jump", OPCODE_Jump);
                AddOp("JumpOnFalse", OPCODE_JumpOnFalse);
                AddOp("JumpOnFalseExpr", OPCODE_JumpOnFalseExpr);
                AddOp("JumpOnGreaterThan", OPCODE_JumpOnGreaterThan);
                AddOp("JumpOnLessThan", OPCODE_JumpOnLessThan);
                AddOp("JumpOnTrue", OPCODE_JumpOnTrue);
                AddOp("JumpOnTrueExpr", OPCODE_JumpOnTrueExpr);
                AddOp("LessThan", OPCODE_LessThan);
                AddOp("LessThanOrEqualTo", OPCODE_LessThanOrEqualTo);
                AddOp("Minus", OPCODE_Minus);
                AddOp("Modulus", OPCODE_Modulus);
                AddOp("Multiply", OPCODE_Multiply);
                AddOp("NextArrayKey", OPCODE_NextArrayKey, VM::VM_T9, OPCODE_T9_IteratorNext, VM::VM_T937, OPCODE_T9_IteratorNext);
                AddOp("NotEqual", OPCODE_NotEqual);
                AddOp("Notify", OPCODE_IW_Notify);
                AddOp("Notify", OPCODE_Notify);
                AddOp("PixBeginEvent", OPCODE_PixBeginEvent);
                AddOp("PixEndEvent", OPCODE_PixEndEvent);
                AddOp("Plus", OPCODE_Plus);
                AddOp("PreScriptCall", OPCODE_PreScriptCall);
                AddOp("ProfileStart", OPCODE_ProfileStart);
                AddOp("ProfileStop", OPCODE_ProfileStop);
                AddOp("Return", OPCODE_Return);
                AddOp("SafeCreateLocalVariables", OPCODE_SafeCreateLocalVariables);
                AddOp("SafeSetVariableFieldCached", OPCODE_T9_SetFieldVariableFromObjectFromRef);
                AddOp("ScriptFunctionCall", OPCODE_ScriptFunctionCall);
                AddOp("ScriptFunctionCallPointer", OPCODE_ScriptFunctionCallPointer);
                AddOp("ScriptMethodCall", OPCODE_ScriptMethodCall);
                AddOp("ScriptMethodCallPointer", OPCODE_ScriptMethodCallPointer);
                AddOp("ScriptMethodThreadCall", OPCODE_ScriptMethodThreadCall);
                AddOp("ScriptMethodThreadCall2", OPCODE_ScriptMethodThreadCallEndOn);
                AddOp("ScriptMethodThreadCallPointer", OPCODE_ScriptMethodThreadCallPointer);
                AddOp("ScriptMethodThreadCallPointer2", OPCODE_ScriptMethodThreadCallPointerEndOn);
                AddOp("ScriptThreadCall", OPCODE_ScriptThreadCall);
                AddOp("ScriptThreadCall2", OPCODE_ScriptThreadCallEndOn);
                AddOp("ScriptThreadCallPointer", OPCODE_ScriptThreadCallPointer);
                AddOp("SetArrayField", OPCODE_T9_SetVariableFieldFromEvalArrayRef);
                AddOp("SetLocalVariableCached", OPCODE_SetLocalVariableCached);
                AddOp("SetVariableField", OPCODE_SetVariableField);
                AddOp("SetVariableFieldRef", OPCODE_SetVariableFieldRef);
                AddOp("SetWaittillVariableFieldCached", OPCODE_IW_SetWaittillVariableFieldCached);
                AddOp("ShiftLeft", OPCODE_ShiftLeft);
                AddOp("ShiftRight", OPCODE_ShiftRight);
                AddOp("SizeOf", OPCODE_SizeOf);
                AddOp("SuperEqual", OPCODE_SuperEqual);
                AddOp("SuperNotEqual", OPCODE_SuperNotEqual);
                AddOp("Switch", OPCODE_Switch);
                AddOp("Vector", OPCODE_Vector);
                AddOp("VectorConstant", OPCODE_VectorConstant);
                AddOp("VectorScale", OPCODE_VectorScale);
                AddOp("Wait", OPCODE_Wait);
                AddOp("WaitFrame", OPCODE_WaitFrame);
                AddOp("WaitRealTime", OPCODE_Wait2);
                AddOp("WaitTill", OPCODE_IW_SingleWaitTill, VM::VM_T8, OPCODE_WaitTill, VM::VM_T9, OPCODE_WaitTill, VM::VM_T937, OPCODE_WaitTill);
                AddOp("WaitTillFrameEnd", OPCODE_WaitTillFrameEnd);
                AddOp("WaitTillMatch", OPCODE_WaitTillMatch);
                AddOp("WaitTillMatch", OPCODE_WaitTillMatch2);
                AddOp("WaittillTimeout", OPCODE_WaittillTimeout);
                AddOp("WaittillTimeoutS", OPCODE_WaitTillMatchTimeout);
            }


            OPCode ConvertFrom(tool::gsc::opcode::VM vm, const char* id) {
                auto it{ idToOpcode.find(hash::Hash64(id)) };
                if (it == idToOpcode.end()) {
                    return OPCODE_Undefined;
                }

                return it->second.FindForVm(vm);
            }

            const char* ConvertTo(tool::gsc::opcode::VM vm, OPCode id, const char* defaultVal) {
                auto it{ opcodeToId.find(id) };
                if (it == opcodeToId.end()) {
                    return defaultVal;
                }

                return it->second.FindForVm(vm);
            }
        };

        TranslationMap& GetMap() {
            static TranslationMap map{};
            return map;
        }
    }


    OPCode ConvertFrom(tool::gsc::opcode::VM vm, const char* id) {
        return GetMap().ConvertFrom(vm, id);
    }

    const char* ConvertTo(tool::gsc::opcode::VM vm, OPCode id, const char* defaultVal) {
        return GetMap().ConvertTo(vm, id, defaultVal);
    }
}