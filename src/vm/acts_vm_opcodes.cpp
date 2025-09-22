#include <includes_shared.hpp>
#include <utils/utils.hpp>
#include "acts_vm.hpp"
#include "acts_vm_opcodes.hpp"
#include "acts_vm_registry.hpp"


namespace acts::vm::opcodes {
	namespace {
		inline void InvalidOpCodeHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated) {
			*terminated = true;
			vm->Error("Invalid opcode", true);
		}

		inline void NopHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread*, bool*) {
			// nop
		}

		inline void ExportNoParamsHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			if (thread->top->type != VT_LOCATION) {
				do {
					vm->ReleaseVariable(thread->top--);
				} while (thread->top->type != VT_LOCATION);
				vm->Error("Called function with too many parameters", false);
			}
		}

		inline void ExportParamsHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			byte vars{ *(thread->codePos++) };

			VmVar* top{ thread->top };
			for (size_t i = 0; i < vars; i++) {
				uint64_t* baseName{ thread->SetData<uint64_t>() };
				uint64_t name = *baseName;
				thread->codePos = (byte*)(baseName + 1);


				// TODO: register var, check stack

			}

			if (top->type != VT_LOCATION) {
				do {
					vm->ReleaseVariable(thread->top--);
				} while (thread->top->type != VT_LOCATION);
				vm->Error("Called function with too many parameters", false);
			}
		}

		inline void GetIntHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			int64_t* base{ thread->SetData<int64_t>() };
			int64_t i{ *base };
			thread->codePos = (byte*)(base + 1);
			VmVar* top{ thread->top++ };
			top->val.i = i;
			top->type = VT_INTEGER;
		}

		inline void GetFloatHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			float* base{ thread->SetData<float>() };
			float f{ *base };
			thread->codePos = (byte*)(base + 1);
			VmVar* top{ thread->top++ };
			top->val.f = f;
			top->type = VT_FLOAT;
		}

		inline void GetHashHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			uint64_t* base{ thread->SetData<uint64_t>() };
			uint64_t h{ *base };
			thread->codePos = (byte*)(base + 1);
			VmVar* top{ thread->top++ };
			top->val.hash = h;
			top->type = VT_HASH;
		}

		inline void GetUndefinedHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			VmVar* top{ thread->top++ };
			top->val.i = 0;
			top->type = VT_UNDEFINED;
		}

		inline void GetStringHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			VmRef* base{ thread->SetData<VmRef>() };
			VmRef ref{ *base };
			thread->codePos = (byte*)(base + 1);
			VmVar* top{ thread->top++ };
			top->val.ref = ref;
			top->type = VT_STRING;
			vm->IncRef(top);
		}

		inline void GetBuiltinFunction(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			uint16_t* base{ thread->SetData<uint16_t>() };
			uint16_t builtin{ *base };
			thread->codePos = (byte*)(base + 1);

			VmVar* top{ thread->top++ };
			top->val.builtinFunc = builtin;
			top->type = VT_BUILTIN_FUNCTION;
		}

		inline void GetScriptFunction(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			uint32_t* base{ thread->SetData<uint32_t>() };
			uint32_t ref{ *base };
			thread->codePos = (byte*)(base + 1);

			VmVar* top{ thread->top++ };
			top->val.scriptFunc = ref;
			top->type = VT_SCRIPT_FUNCTION;
		}

		inline void IsDefinedHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			VmVar* top{ thread->top };
			vm->ReleaseVariable(thread->top);
			top->val.b = thread->top->type != VT_UNDEFINED;
			top->type = VT_INTEGER;
		}

		inline void DecTopHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			vm->ReleaseVariable(thread->top--);
		}

		inline void PreCallHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			VmVar* top{ thread->top++ };
			top->val.loc = thread->codePos;
			top->type = VT_LOCATION;
		}

		inline void JumpHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);

			thread->codePos += delta;
		}

		inline void DevBlockHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);
			if (!vm->Cfg().enabledDevBlocks) {
				thread->codePos += delta;
			}
		}

		inline void JumpIfDefinedHandler(acts::vm::ActsVm*, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);

			if (thread->top->type != VT_UNDEFINED) {
				thread->codePos += delta;
			}
			else {
				thread->top--;
			}
		}

		inline void JumpIfTrueHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);
			if (vm->CastToBool(thread->top--)) {
				thread->codePos += delta;
			}
		}

		inline void JumpIfFalseHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);
			if (!vm->CastToBool(thread->top--)) {
				thread->codePos += delta;
			}
		}

		inline void JumpIfTrueExprHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);
			if (vm->CastToBool(thread->top)) {
				thread->codePos += delta;
			}
			else {
				thread->top--;
			}
		}

		inline void JumpIfFalseExprHandler(acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool*) {
			int16_t* base{ thread->SetData<int16_t>() };
			int16_t delta{ *base };
			thread->codePos = (byte*)(base + 1);
			if (!vm->CastToBool(thread->top)) {
				thread->codePos += delta;
			}
			else {
				thread->top--;
			}
		}
	}

	void HandleOpCode(OpCode opcode, acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated) {
		switch (opcode) {
		case OpCodeId::OPCODE_NOP:
			NopHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_EXPORT_NO_PARAMS:
			ExportNoParamsHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_EXPORT_PARAMS:
			ExportParamsHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_INT:
			GetIntHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_FLOAT:
			GetFloatHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_HASH:
			GetHashHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_UNDEFINED:
			GetUndefinedHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_STRING:
			GetStringHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_BUILTIN_FUNCTION:
			GetBuiltinFunction(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_GET_FUNCTION:
			GetScriptFunction(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_IS_DEFINED:
			IsDefinedHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_DEC_TOP:
			DecTopHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_PRE_CALL:
			PreCallHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP:
			JumpHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_DEV_BLOCK:
			DevBlockHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP_IF_TRUE:
			JumpIfTrueHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP_IF_FALSE:
			JumpIfFalseHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP_IF_TRUE_EXPR:
			JumpIfTrueExprHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP_IF_FALSE_EXPR:
			JumpIfFalseExprHandler(vm, thread, terminated);
			break;
		case OpCodeId::OPCODE_JUMP_IF_DEFINED:
			JumpIfDefinedHandler(vm, thread, terminated);
			break;
		default:
			InvalidOpCodeHandler(vm, thread, terminated);
			break;
		}
	}
}