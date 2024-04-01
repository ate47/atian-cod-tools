#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	enum VmFlags {
		VMF_NONE = 0,
		VMF_OPCODE_SHORT = 1,
		VMF_HASH64 = 2,
		VMF_NO_VERSION = 4,
		VMF_NO_PARAM_FLAGS = 8,
		VMF_FULL_FILE_NAMESPACE = 0x10,
		VMF_INV_ADD_TO_OBJECT = 0x20,
	};
	struct GlobalVariableDef {
		const char* name;
		OPCode getOpCode{};
		OPCode getRefOpCode{};
	};

	class OPCodeInfo;
	struct VmInfo {
		byte vm;
		const char* name;
		uint64_t flags;
		byte platforms{};
		std::unordered_map<uint16_t, std::unordered_map<Platform, OPCode>> opcodemap{};
		std::unordered_map<OPCode, std::unordered_map<Platform, uint16_t>> opcodemaplookup{};
		std::unordered_map<Platform, std::unordered_map<OPCode, std::vector<uint16_t>>> opcodemappltlookup{};
		std::unordered_map<std::string, GlobalVariableDef> globalvars{};

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
	const std::unordered_map<byte, VmInfo>& GetVMMaps();
	bool IsValidVm(byte vm, VmInfo*& info, bool registerOpCodes = true);
	const OPCodeInfo* LookupOpCode(byte vm, Platform platform, uint16_t opcode);
	std::pair<bool, uint16_t> GetOpCodeId(byte vm, Platform platform, OPCode opcode);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	void RegisterVM(byte vm, const char* name, uint64_t flags);
	void RegisterVMGlobalVariable(byte vm, const char* name, OPCode getOpCode = OPCODE_Undefined, OPCode getRefOpCode = OPCODE_Undefined);
	void RegisterVMPlatform(byte vm, Platform plt);
	void RegisterOpCode(byte vm, Platform platform, OPCode enumValue, uint16_t op);
	void RegisterOpCodes();

	inline void RegisterOpCode(byte vm, Platform platform, OPCode enumValue) {}
	template<typename... OpTypes>
	inline void RegisterOpCode(byte vm, Platform platform, OPCode enumValue, uint16_t op, OpTypes... ops) {
		RegisterOpCode(vm, platform, enumValue, op);
		RegisterOpCode(vm, platform, enumValue, ops...);
	}

}