#pragma once

#include "tools/gsc/gsc_opcodes.hpp"

namespace compatibility::serious::txt {
    constexpr const char* OP_INVALID = "op_invalid";
    tool::gsc::opcode::OPCode ConvertFrom(tool::gsc::opcode::VMId vm, const char* id);
    const char* ConvertTo(tool::gsc::opcode::VMId vm, tool::gsc::opcode::OPCode id, const char* defaultVal = OP_INVALID);
}