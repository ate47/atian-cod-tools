#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	class OPCodeInfo;
	struct VmInfo {
		BYTE vm;
		LPCCH name;
		std::unordered_map<UINT16, OPCode> opcodemap{};
		std::unordered_map<OPCode, UINT16> opcodemaplookup{};
	};
	bool IsValidVm(BYTE vm, VmInfo*& info);
	const OPCodeInfo* LookupOpCode(BYTE vm, UINT16 opcode);
	std::pair<bool, UINT16> GetOpCodeId(BYTE vm, OPCode opcode);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	void RegisterVM(BYTE vm, LPCCH name);
	void RegisterOpCode(BYTE vm, OPCode enumValue, UINT16 op);
	void RegisterOpCodes();

	inline void RegisterOpCode(BYTE vm, OPCode enumValue) {}
	template<typename... OpTypes>
	inline void RegisterOpCode(BYTE vm, OPCode enumValue, UINT16 op, OpTypes... ops) {
		RegisterOpCode(vm, enumValue, op);
		RegisterOpCode(vm, enumValue, ops...);
	}

}