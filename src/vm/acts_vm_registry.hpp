#pragma once

namespace acts::vm::opcodes {
	enum OpCodeId : uint8_t {
		OPCODE_NOP = 0,
		OPCODE_EXPORT_NO_PARAMS = 0xd,
		OPCODE_END = 0x10,
		OPCODE_EXPORT_PARAMS = 0x11,
		OPCODE_GET_INT = 0x20,
		OPCODE_GET_FLOAT = 0x21,
		OPCODE_GET_HASH = 0x22,
		OPCODE_GET_UNDEFINED = 0x23,


		OPCODE_IS_DEFINED = 0x30,
	};
}