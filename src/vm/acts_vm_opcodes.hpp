#pragma once
#include "acts_vm.hpp"
namespace acts::vm::opcodes {
	typedef uint8_t OpCode;
	void HandleOpCode(OpCode opcode, acts::vm::ActsVm* vm, acts::vm::VmExecutionThread* thread, bool* terminated);
}