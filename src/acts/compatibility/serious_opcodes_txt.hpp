#pragma once

#include "tools/gsc_opcodes.hpp"

namespace compatibility::serious::txt {
    constexpr const char* OP_INVALID = "op_invalid";
    tool::gsc::opcode::OPCode ConvertFrom(tool::gsc::opcode::VM vm, const char* id);
    const char* ConvertTo(tool::gsc::opcode::VM vm, tool::gsc::opcode::OPCode id, const char* defaultVal = OP_INVALID);
}