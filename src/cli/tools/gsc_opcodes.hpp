#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	class OPCodeInfo;
	struct VmInfo {
		BYTE vm;
		LPCCH name;
		BYTE platforms{};
		std::unordered_map<UINT16, std::unordered_map<Platform, OPCode>> opcodemap{};
		std::unordered_map<OPCode, std::unordered_map<Platform, UINT16>> opcodemaplookup{};

		/*
		 * Add an available platform to this VM
		 * @param plt platform
		 */
		inline void AddPlatform(Platform plt) {
			if (plt != PLATFORM_UNKNOWN) {
				platforms |= 1 << (plt - 1);
			}
		}
		/*
		 * Is this VM available for this platform
		 * @param plt platform
		 * @return true if available
		 */
		inline bool HasPlatform(Platform plt) const {
			return plt != PLATFORM_UNKNOWN && (platforms & (1 << (plt - 1)));
		}
	};
	const std::unordered_map<BYTE, VmInfo>& GetVMMaps();
	bool IsValidVm(BYTE vm, VmInfo*& info);
	const OPCodeInfo* LookupOpCode(BYTE vm, Platform platform, UINT16 opcode);
	std::pair<bool, UINT16> GetOpCodeId(BYTE vm, Platform platform, OPCode opcode);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	void RegisterVM(BYTE vm, LPCCH name);
	void RegisterVMPlatform(BYTE vm, Platform plt);
	void RegisterOpCode(BYTE vm, Platform platform, OPCode enumValue, UINT16 op);
	void RegisterOpCodes();

	inline void RegisterOpCode(BYTE vm, Platform platform, OPCode enumValue) {}
	template<typename... OpTypes>
	inline void RegisterOpCode(BYTE vm, Platform platform, OPCode enumValue, UINT16 op, OpTypes... ops) {
		RegisterOpCode(vm, platform, enumValue, op);
		RegisterOpCode(vm, platform, enumValue, ops...);
	}

}