#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	class opcodeinfo;
	struct vminfo {
		BYTE vm;
		LPCCH name;
		std::unordered_map<UINT16, opcode> opcodemap{};
	};
	bool IsValidVm(BYTE vm, vminfo*& info);
	const opcodeinfo* LookupOpCode(BYTE vm, UINT16 opcode);
	void RegisterOpCodeHandler(const opcodeinfo* info);
	void RegisterVM(BYTE vm, LPCCH name);
	void RegisterOpCode(BYTE vm, opcode enumValue, UINT16 op);
	void RegisterOpCodes();

	inline void RegisterOpCode(BYTE vm, opcode enumValue) {}
	template<typename... OpTypes>
	inline void RegisterOpCode(BYTE vm, opcode enumValue, UINT16 op, OpTypes... ops) {
		RegisterOpCode(vm, enumValue, op);
		RegisterOpCode(vm, enumValue, ops...);
	}

}