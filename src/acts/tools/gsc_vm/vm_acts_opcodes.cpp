#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <acts_vm_registry.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void OpCode() {
		VmInfo* af1 = RegisterVM(VMI_ACTS_F1, "ACTS VM", "f1", "acts", VmFlags::VMF_EXPORT_NOCHECKSUM | VmFlags::VMF_ALIGN | VmFlags::VMF_CALL_NO_PARAMS);
		af1->RegisterVmName("f1");
		af1->AddPlatform(PLATFORM_PC);
		af1->SetMaxOpCode(0xFF);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_CheckClearParams, acts::vm::opcodes::OPCODE_EXPORT_NO_PARAMS);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_End, acts::vm::opcodes::OPCODE_END);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_SafeCreateLocalVariables, acts::vm::opcodes::OPCODE_EXPORT_PARAMS);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GetFloat, acts::vm::opcodes::OPCODE_GET_FLOAT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GetInteger, acts::vm::opcodes::OPCODE_GET_INT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GetHash, acts::vm::opcodes::OPCODE_GET_HASH);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GetUndefined, acts::vm::opcodes::OPCODE_GET_UNDEFINED);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_IsDefined, acts::vm::opcodes::OPCODE_IS_DEFINED);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_DecTop, acts::vm::opcodes::OPCODE_DEC_TOP);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_PreScriptCall, acts::vm::opcodes::OPCODE_PRE_CALL);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Jump, acts::vm::opcodes::OPCODE_JUMP);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_DevblockBegin, acts::vm::opcodes::OPCODE_DEV_BLOCK);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnDefinedExpr, acts::vm::opcodes::OPCODE_JUMP_IF_DEFINED);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalse, acts::vm::opcodes::OPCODE_JUMP_IF_FALSE);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnFalseExpr, acts::vm::opcodes::OPCODE_JUMP_IF_FALSE_EXPR);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrue, acts::vm::opcodes::OPCODE_JUMP_IF_TRUE);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_JumpOnTrueExpr, acts::vm::opcodes::OPCODE_JUMP_IF_TRUE_EXPR);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Plus, acts::vm::opcodes::OPCODE_OP_PLUS);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Minus, acts::vm::opcodes::OPCODE_OP_MINUS);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_BoolNot, acts::vm::opcodes::OPCODE_OP_NOT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Multiply, acts::vm::opcodes::OPCODE_OP_MULTIPLY);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Divide, acts::vm::opcodes::OPCODE_OP_DIVIDE);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Modulus, acts::vm::opcodes::OPCODE_OP_MODULUS);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Xor, acts::vm::opcodes::OPCODE_OP_BIT_XOR);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_Or, acts::vm::opcodes::OPCODE_OP_BIT_OR);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Bit_And, acts::vm::opcodes::OPCODE_OP_BIT_AND);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_BoolComplement, acts::vm::opcodes::OPCODE_OP_BIT_NOT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftLeft, acts::vm::opcodes::OPCODE_OP_BIT_SHIFT_LEFT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_ShiftRight, acts::vm::opcodes::OPCODE_OP_BIT_SHIFT_RIGHT);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_LessThan, acts::vm::opcodes::OPCODE_OP_LOWER_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThan, acts::vm::opcodes::OPCODE_OP_GREATER_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_LessThanOrEqualTo, acts::vm::opcodes::OPCODE_OP_LOWER_OR_EQUAL_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_GreaterThanOrEqualTo, acts::vm::opcodes::OPCODE_OP_GREATER_OR_EQUAL_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_T10_LowerThanOrSuperEqualTo, acts::vm::opcodes::OPCODE_OP_LOWER_OR_SUPER_EQUAL_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_T10_GreaterThanOrSuperEqualTo, acts::vm::opcodes::OPCODE_OP_GREATER_OR_SUPER_EQUAL_THAN);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_Equal, acts::vm::opcodes::OPCODE_OP_EQUAL);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_NotEqual, acts::vm::opcodes::OPCODE_OP_NOT_EQUAL);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_SuperEqual, acts::vm::opcodes::OPCODE_OP_SUPER_EQUAL);
		af1->RegisterOpCode(PLATFORM_PC, OPCODE_SuperNotEqual, acts::vm::opcodes::OPCODE_OP_SUPER_NOT_EQUAL);

	}
}
REGISTER_GSC_VM_OPCODES(acts, OpCode);