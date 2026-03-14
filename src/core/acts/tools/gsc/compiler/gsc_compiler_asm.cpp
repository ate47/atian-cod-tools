#include <includes.hpp>
#include <tools/gsc/compiler/gsc_compiler_asm.hpp>
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>

namespace tool::gsc::compiler {

    AscmNodeOpCode* AscmNode::AsOpCode() {
        if (nodetype == ASCMNT_OPCODE) {
            return dynamic_cast<AscmNodeOpCode*>(this);
        }
        return nullptr;
    }

    void AscmNodeFunctionCall::SetScriptCall(CompileObject& obj, bool scriptCall) {
        isScriptCall = scriptCall;

        if (flags & FCF_GETTER) {
            if (scriptCall || !obj.HasOpCode(OPCODE_IW_GetBuiltinFunction)) {
                opcode = OPCODE_GetResolveFunction;
            }
            else {
                throw std::runtime_error("builtin getters not implemented for this vm");
            }
            return;
        }

        if (scriptCall) {
            if (flags & FCF_METHOD) {
                if (flags & FCF_CHILDTHREAD) {
                    opcode = OPCODE_ScriptMethodThreadCallEndOn;
                }
                else if (flags & FCF_THREAD) {
                    opcode = OPCODE_ScriptMethodThreadCall;
                }
                else {
                    opcode = OPCODE_ScriptMethodCall;
                }
            }
            else {
                // func
                if (flags & FCF_CHILDTHREAD) {
                    opcode = OPCODE_ScriptThreadCallEndOn;
                }
                else if (flags & FCF_THREAD) {
                    opcode = OPCODE_ScriptThreadCall;
                }
                else {
                    opcode = OPCODE_ScriptFunctionCall;
                }
            }
        }
        else {
            // builtin
            if (flags & FCF_METHOD) {
                opcode = OPCODE_CallBuiltinMethod;
            }
            else {
                // func
                opcode = OPCODE_CallBuiltinFunction;
            }
        }
    }

    AscmNodeCreateLocalVariables::AscmNodeCreateLocalVariables(const FunctionVar* lvars, size_t count, size_t params, const FunctionObject& fobj, bool obfuscate)
        : params(params), obfuscate(obfuscate) {
        hasRegister = fobj.obj.HasOpCode(OPCODE_IW_RegisterVariable);
        hasRegisters = fobj.obj.HasOpCode(OPCODE_IW_RegisterMultipleVariables);
        hashSize = fobj.obj.vmInfo->HasFlag(VMF_HASH64) ? 8 : 4;
        hasFlag = !fobj.obj.vmInfo->HasFlag(VMF_NO_PARAM_FLAGS);

        vars.reserve(count);
        for (size_t i = 0; i < count; i++) {
            vars.push_back(lvars[i]);
        }
    }

}
