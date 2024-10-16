#include <includes.hpp>
#include "lua_opcodes.hpp"

namespace tool::lua::opcodes {
	struct LuaVMInfo {
		LuaVM vm{};
		const char* name{};

		std::unordered_map<uint64_t, HKSInstId> insts{};
	};
	namespace {
		std::unordered_map<LuaVM, LuaVMInfo> vms{};
	}

	HKSInstId GetOpCode(LuaVM vm, uint64_t val) {
		auto it = vms.find(vm);
		if (it == vms.end()) return LUA_OPCODE_UNDEFINED;

		auto it2 = it->second.insts.find(val);
		return it2 != it->second.insts.end() ? it2->second : LUA_OPCODE_UNDEFINED;
	}

	void RegisterLuaVm(LuaVM vm, const char* name) {
		if (!vm) throw std::runtime_error("Empty VM Id");
		LuaVMInfo& nfo = vms[vm];
		if (nfo.vm) {
			LOG_WARNING("The VM '{}' is already defined: '{}'", name, nfo.name);
			return;
		}

		nfo.vm = vm;
		nfo.name = name;
	}

	void RegisterOpCode(LuaVM vm, HKSInstId id, uint64_t val) {
		auto it = vms.find(vm);
		if (it == vms.end()) {
			LOG_WARNING("Trying to register opcode with bad vm 0x{:x}", (int)vm);
			return;
		}
		it->second.insts[val] = id;
	}
	const char* GetOpCodeName(HKSInstId val) {
		switch (val) {
        case LUA_OPCODE_UNDEFINED: return "UNDEFINED";
        case LUA_OPCODE_GETFIELD: return "GETFIELD";
        case LUA_OPCODE_TEST: return "TEST";
        case LUA_OPCODE_CALL_I: return "CALL_I";
        case LUA_OPCODE_CALL_C: return "CALL_C";
        case LUA_OPCODE_EQ: return "EQ";
        case LUA_OPCODE_EQ_BK: return "EQ_BK";
        case LUA_OPCODE_GETGLOBAL: return "GETGLOBAL";
        case LUA_OPCODE_MOVE: return "MOVE";
        case LUA_OPCODE_SELF: return "SELF";
        case LUA_OPCODE_RETURN: return "RETURN";
        case LUA_OPCODE_GETTABLE_S: return "GETTABLE_S";
        case LUA_OPCODE_GETTABLE_N: return "GETTABLE_N";
        case LUA_OPCODE_GETTABLE: return "GETTABLE";
        case LUA_OPCODE_LOADBOOL: return "LOADBOOL";
        case LUA_OPCODE_TFORLOOP: return "TFORLOOP";
        case LUA_OPCODE_SETFIELD: return "SETFIELD";
        case LUA_OPCODE_SETTABLE_S: return "SETTABLE_S";
        case LUA_OPCODE_SETTABLE_S_BK: return "SETTABLE_S_BK";
        case LUA_OPCODE_SETTABLE_N: return "SETTABLE_N";
        case LUA_OPCODE_SETTABLE_N_BK: return "SETTABLE_N_BK";
        case LUA_OPCODE_SETTABLE: return "SETTABLE";
        case LUA_OPCODE_SETTABLE_BK: return "SETTABLE_BK";
        case LUA_OPCODE_TAILCALL_I: return "TAILCALL_I";
        case LUA_OPCODE_TAILCALL_C: return "TAILCALL_C";
        case LUA_OPCODE_TAILCALL_M: return "TAILCALL_M";
        case LUA_OPCODE_LOADK: return "LOADK";
        case LUA_OPCODE_LOADNIL: return "LOADNIL";
        case LUA_OPCODE_SETGLOBAL: return "SETGLOBAL";
        case LUA_OPCODE_JMP: return "JMP";
        case LUA_OPCODE_CALL_M: return "CALL_M";
        case LUA_OPCODE_CALL: return "CALL";
        case LUA_OPCODE_INTRINSIC_INDEX: return "INTRINSIC_INDEX";
        case LUA_OPCODE_INTRINSIC_NEWINDEX: return "INTRINSIC_NEWINDEX";
        case LUA_OPCODE_INTRINSIC_SELF: return "INTRINSIC_SELF";
        case LUA_OPCODE_INTRINSIC_INDEX_LITERAL: return "INTRINSIC_INDEX_LITERAL";
        case LUA_OPCODE_INTRINSIC_NEWINDEX_LITERAL: return "INTRINSIC_NEWINDEX_LITERAL";
        case LUA_OPCODE_INTRINSIC_SELF_LITERAL: return "INTRINSIC_SELF_LITERAL";
        case LUA_OPCODE_TAILCALL: return "TAILCALL";
        case LUA_OPCODE_GETUPVAL: return "GETUPVAL";
        case LUA_OPCODE_SETUPVAL: return "SETUPVAL";
        case LUA_OPCODE_ADD: return "ADD";
        case LUA_OPCODE_ADD_BK: return "ADD_BK";
        case LUA_OPCODE_SUB: return "SUB";
        case LUA_OPCODE_SUB_BK: return "SUB_BK";
        case LUA_OPCODE_MUL: return "MUL";
        case LUA_OPCODE_MUL_BK: return "MUL_BK";
        case LUA_OPCODE_DIV: return "DIV";
        case LUA_OPCODE_DIV_BK: return "DIV_BK";
        case LUA_OPCODE_MOD: return "MOD";
        case LUA_OPCODE_MOD_BK: return "MOD_BK";
        case LUA_OPCODE_POW: return "POW";
        case LUA_OPCODE_POW_BK: return "POW_BK";
        case LUA_OPCODE_NEWTABLE: return "NEWTABLE";
        case LUA_OPCODE_UNM: return "UNM";
        case LUA_OPCODE_NOT: return "NOT";
        case LUA_OPCODE_LEN: return "LEN";
        case LUA_OPCODE_LT: return "LT";
        case LUA_OPCODE_LT_BK: return "LT_BK";
        case LUA_OPCODE_LE: return "LE";
        case LUA_OPCODE_LE_BK: return "LE_BK";
        case LUA_OPCODE_SHIFT_LEFT: return "SHIFT_LEFT";
        case LUA_OPCODE_SHIFT_LEFT_BK: return "SHIFT_LEFT_BK";
        case LUA_OPCODE_SHIFT_RIGHT: return "SHIFT_RIGHT";
        case LUA_OPCODE_SHIFT_RIGHT_BK: return "SHIFT_RIGHT_BK";
        case LUA_OPCODE_BITWISE_AND: return "BITWISE_AND";
        case LUA_OPCODE_BITWISE_AND_BK: return "BITWISE_AND_BK";
        case LUA_OPCODE_BITWISE_OR: return "BITWISE_OR";
        case LUA_OPCODE_BITWISE_OR_BK: return "BITWISE_OR_BK";
        case LUA_OPCODE_CONCAT: return "CONCAT";
        case LUA_OPCODE_TESTSET: return "TESTSET";
        case LUA_OPCODE_FORPREP: return "FORPREP";
        case LUA_OPCODE_FORLOOP: return "FORLOOP";
        case LUA_OPCODE_SETLIST: return "SETLIST";
        case LUA_OPCODE_CLOSE: return "CLOSE";
        case LUA_OPCODE_CLOSURE: return "CLOSURE";
        case LUA_OPCODE_VARARG: return "VARARG";
        case LUA_OPCODE_TAILCALL_I_R1: return "TAILCALL_I_R1";
        case LUA_OPCODE_CALL_I_R1: return "CALL_I_R1";
        case LUA_OPCODE_SETUPVAL_R1: return "SETUPVAL_R1";
        case LUA_OPCODE_TEST_R1: return "TEST_R1";
        case LUA_OPCODE_NOT_R1: return "NOT_R1";
        case LUA_OPCODE_GETFIELD_R1: return "GETFIELD_R1";
        case LUA_OPCODE_SETFIELD_R1: return "SETFIELD_R1";
        case LUA_OPCODE_NEWSTRUCT: return "NEWSTRUCT";
        case LUA_OPCODE_DATA: return "DATA";
        case LUA_OPCODE_SETSLOTN: return "SETSLOTN";
        case LUA_OPCODE_SETSLOTI: return "SETSLOTI";
        case LUA_OPCODE_SETSLOT: return "SETSLOT";
        case LUA_OPCODE_SETSLOTS: return "SETSLOTS";
        case LUA_OPCODE_SETSLOTMT: return "SETSLOTMT";
        case LUA_OPCODE_CHECKTYPE: return "CHECKTYPE";
        case LUA_OPCODE_CHECKTYPES: return "CHECKTYPES";
        case LUA_OPCODE_GETSLOT: return "GETSLOT";
        case LUA_OPCODE_GETSLOTMT: return "GETSLOTMT";
        case LUA_OPCODE_SELFSLOT: return "SELFSLOT";
        case LUA_OPCODE_SELFSLOTMT: return "SELFSLOTMT";
        case LUA_OPCODE_GETFIELD_MM: return "GETFIELD_MM";
        case LUA_OPCODE_CHECKTYPE_D: return "CHECKTYPE_D";
        case LUA_OPCODE_GETSLOT_D: return "GETSLOT_D";
        case LUA_OPCODE_GETGLOBAL_MEM: return "GETGLOBAL_MEM";
        case LUA_OPCODE_MAX: return "MAX";
        case LUA_OPCODE_DELETE: return "DELETE";
        case LUA_OPCODE_DELETE_BK: return "DELETE_BK";
        case LUA_OPCODE_UNK: return "UNK";
        default: return "UNKNOWN";
		}
	}

}