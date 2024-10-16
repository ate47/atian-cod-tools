#include <includes.hpp>
#include "lua_opcodes.hpp"
#include "lua_opcodes_load.hpp"

namespace tool::lua::opcodes {
	void RegisterLuaOpCodesMap() {
		static std::once_flag f{};

		std::call_once(f, [] {
			RegisterLuaVm(LVM_HKS_51, "Lua HKS 51");

			// https://github.com/JariKCoding/CoDLuaDecompiler/blob/master/CoDLuaDecompiler.Decompiler/LuaFile/Structures/LuaOpCodeTable/Havok/HavokLuaOpCodeTableT7.cs
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETFIELD, 0);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TEST, 1);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CALL_I, 2);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CALL_C, 3);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_EQ, 4);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_EQ_BK, 5);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETGLOBAL, 6);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_MOVE, 7);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SELF, 8);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_RETURN, 9);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETTABLE_S, 10);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETTABLE_N, 11);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETTABLE, 12);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LOADBOOL, 13);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TFORLOOP, 14);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETFIELD, 15);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE_S, 16);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE_S_BK, 17);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE_N, 18);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE_N_BK, 19);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE, 20);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETTABLE_BK, 21);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TAILCALL_I, 22);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TAILCALL_C, 23);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TAILCALL_M, 24);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LOADK, 25);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LOADNIL, 26);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETGLOBAL, 27);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_JMP, 28);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CALL_M, 29);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CALL, 30);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_INDEX, 31);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_NEWINDEX, 32);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_SELF, 33);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_INDEX_LITERAL, 34);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_NEWINDEX_LITERAL, 35);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_INTRINSIC_SELF_LITERAL, 36);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TAILCALL, 37);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETUPVAL, 38);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETUPVAL, 39);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_ADD, 40);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_ADD_BK, 41);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SUB, 42);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SUB_BK, 43);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_MUL, 44);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_MUL_BK, 45);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_DIV, 46);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_DIV_BK, 47);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_MOD, 48);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_MOD_BK, 49);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_POW, 50);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_POW_BK, 51);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_NEWTABLE, 52);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_UNM, 53);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_NOT, 54);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LEN, 55);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LT, 56);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LT_BK, 57);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LE, 58);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_LE_BK, 59);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SHIFT_LEFT, 60);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SHIFT_LEFT_BK, 61);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SHIFT_RIGHT, 62);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SHIFT_RIGHT_BK, 63);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_BITWISE_AND, 64);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_BITWISE_AND_BK, 65);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_BITWISE_OR, 66);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_BITWISE_OR_BK, 67);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CONCAT, 68);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TESTSET, 69);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_FORPREP, 70);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_FORLOOP, 71);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETLIST, 72);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CLOSE, 73);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CLOSURE, 74);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_VARARG, 75);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TAILCALL_I_R1, 76);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CALL_I_R1, 77);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETUPVAL_R1, 78);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_TEST_R1, 79);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_NOT_R1, 80);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETFIELD_R1, 81);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETFIELD_R1, 82);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_NEWSTRUCT, 83);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_DATA, 84);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETSLOTN, 85);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETSLOTI, 86);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETSLOT, 87);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETSLOTS, 88);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SETSLOTMT, 89);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CHECKTYPE, 90);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CHECKTYPES, 91);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETSLOT, 92);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETSLOTMT, 93);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SELFSLOT, 94);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_SELFSLOTMT, 95);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETFIELD_MM, 96);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_CHECKTYPE_D, 97);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETSLOT_D, 98);
			RegisterOpCode(LVM_HKS_51, LUA_OPCODE_GETGLOBAL_MEM, 99);
		});
	}
}