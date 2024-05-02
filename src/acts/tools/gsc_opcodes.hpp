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
		VMF_HASH_IW = 0x40,
	};
	enum VmOperatorFunctionData {
		VPFD_NONE = 0,
		VPFD_USE_COUNT = 0x1,
		VPFD_USE_PRE_SCRIPT_CALL = 0x2,
		VPFD_RETURN_VALUE = 0x4,
		VPFD_SELF_PARAM = 0x8,
	};
	struct GlobalVariableDef {
		const char* name;
		OPCode getOpCode{};
		OPCode getRefOpCode{};
	};
	struct FunctionOperator {
		const char* name;
		const char* usage;
		OPCode opCode{};
		int flags;
		int minParam;
		int maxParam;

		constexpr bool HasFlag(VmOperatorFunctionData flag) {
			return flags & flag;
		}
	};

	class OPCodeInfo;
	struct VmInfo {
		byte vm;
		const char* name;
		uint64_t flags;
		byte platforms{};
		std::unordered_set<uint64_t> devCallsNames{};
		std::unordered_map<uint16_t, std::unordered_map<Platform, OPCode>> opcodemap{};
		std::unordered_map<OPCode, std::unordered_map<Platform, uint16_t>> opcodemaplookup{};
		std::unordered_map<Platform, std::unordered_map<OPCode, std::vector<uint16_t>>> opcodemappltlookup{};
		std::unordered_map<uint64_t, GlobalVariableDef> globalvars{};
		std::unordered_map<uint64_t, FunctionOperator> opFuncs{};

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
		 * Add a dev call name for this platform
		 */
		void AddDevCallName(uint64_t name);

		/*
		 * Is this VM available for this platform
		 * @param plt platform
		 * @return true if available
		 */
		inline bool HasPlatform(Platform plt) const {
			return plt != PLATFORM_UNKNOWN && (platforms & (1 << (plt - 1)));
		}

		/*
		 * Test if this vm has a flag
		 * @param flag flag
		 * @return if the vm has the flag
		 */
		constexpr bool HasFlag(VmFlags flag) const {
			return flags & flag;
		}

		uint64_t HashField(const char* value) const;
		uint64_t HashPath(const char* value) const;
		uint64_t HashFilePath(const char* value) const;

		inline uint64_t HashField(const std::string& value) const {
			return HashField(value.c_str());
		}
		inline uint64_t HashPath(const std::string& value) const {
			return HashPath(value.c_str());
		}
		inline uint64_t HashFilePath(const std::string& value) const {
			return HashFilePath(value.c_str());
		}
	};
	const std::unordered_map<byte, VmInfo>& GetVMMaps();
	bool IsValidVm(byte vm, VmInfo*& info, bool registerOpCodes = true);
	const OPCodeInfo* LookupOpCode(byte vm, Platform platform, uint16_t opcode);
	std::pair<bool, uint16_t> GetOpCodeId(byte vm, Platform platform, OPCode opcode);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	void RegisterVM(byte vm, const char* name, uint64_t flags);
	void RegisterVMGlobalVariable(byte vm, const char* name, OPCode getOpCode = OPCODE_Undefined, OPCode getRefOpCode = OPCODE_Undefined);
	void RegisterVMOperatorFunction(byte vm, const char* name, const char* usage, OPCode opcode, int flags, int minArgs = 0, int maxArgs = 255);
	void RegisterVMPlatform(byte vm, Platform plt);
	void RegisterOpCode(byte vm, Platform platform, OPCode enumValue, uint16_t op);
	void RegisterDevCall(byte vm, const char* devCall);
	void RegisterOpCodes();

	inline void RegisterOpCode(byte vm, Platform platform, OPCode enumValue) {}
	template<typename... OpTypes>
	inline void RegisterOpCode(byte vm, Platform platform, OPCode enumValue, uint16_t op, OpTypes... ops) {
		RegisterOpCode(vm, platform, enumValue, op);
		RegisterOpCode(vm, platform, enumValue, ops...);
	}


	inline void RegisterDevCall(byte vm) {}
	template<typename... DevCalls>
	inline void RegisterDevCall(byte vm, const char* devCall, DevCalls... calls) {
		RegisterDevCall(vm, devCall);
		RegisterDevCall(vm, calls...);
	}

}