#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	enum VmFlags : uint64_t {
		VMF_NONE = 0,
		VMF_ALIGN = 1,
		VMF_HASH64 = 2,
		VMF_NO_VERSION = 4,
		VMF_NO_PARAM_FLAGS = 8,
		VMF_FULL_FILE_NAMESPACE = 0x10,
		VMF_INV_ADD_TO_OBJECT = 0x20,
		VMF_HASH_IW = 0x40,
		VMF_CLIENT_VM = 0x80,
		VMF_NO_FILE_NAMESPACE = 0x100,
		VMF_OPCODE_U16 = 0x200,
		VMF_CALL_NO_PARAMS = 0x400,
		VMF_IW_CALLS = 0x800,
		VMF_EXPORT_NOCHECKSUM = 0x1000,
		VMF_HASH_CER = 0x2000,
		VMF_ANIMTREE_T7= 0x4000,
		VMF_HASH_T7 = 0x8000,
		VMF_FOREACH_IW = 0x10000,
		VMF_CRC_DUMP = 0x20000,
		VMF_EXPORT_CRC32 = 0x40000,
	};
	enum VmOperatorFunctionData : uint64_t {
		VPFD_NONE = 0,
		VPFD_USE_COUNT = 0x1,
		VPFD_USE_PRE_SCRIPT_CALL = 0x2,
		VPFD_RETURN_VALUE = 0x4,
		VPFD_SELF_PARAM = 0x8,
		VPFD_HASH_PARAM = 0x10,
		VPFD_UNPACK = 0x20,
		VPFD_STRING_PARAM = 0x40,
	};
	struct GlobalVariableDef {
		const char* name;
		OPCode getOpCode{};
	};
	struct FunctionOperator {
		const char* name;
		const char* usage;
		OPCode opCode{};
		int flags;
		int minParam;
		int maxParam;

		constexpr bool HasFlag(uint64_t flag) {
			return (flags & flag) == flag;
		}
	};

	struct VmHashFunc {
		char type;
		OPCode opCode;
		int size;
		std::function<uint64_t(const char*)> hashFunc;
	};

	class OPCodeInfo;
	struct VmInfo {
		byte vm;
		const char* name;
		const char* codeName;
		const char* internalName;
		uint64_t flags;
		byte platforms{};
		uint16_t maxOpCode{ 0xFFF };
		std::unordered_map<char, VmHashFunc> hashesFunc{};
		std::unordered_set<uint64_t> devCallsNames{};
		std::unordered_map<uint16_t, std::unordered_map<Platform, OPCode>> opcodemap{};
		std::unordered_map<OPCode, std::unordered_map<Platform, uint16_t>> opcodemaplookup{};
		std::unordered_map<Platform, std::unordered_map<OPCode, std::unordered_set<uint16_t>>> opcodemappltlookup{};
		std::unordered_map<uint64_t, GlobalVariableDef> globalvars{};
		std::unordered_map<uint64_t, FunctionOperator> opFuncs{};
		std::unordered_map<uint64_t, const char*> dataType{};
		std::unordered_map<Platform, Platform> sameVmMap{};

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
		constexpr bool HasFlag(uint64_t flag) const {
			return (flags & flag) == flag;
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
		Platform RemapSamePlatform(Platform origin) const;
	};
	const std::unordered_map<byte, VmInfo>& GetVMMaps();
	bool IsValidVmMagic(uint64_t magic, VmInfo*& info, bool registerOpCodes = true);
	bool IsValidVm(byte vm, VmInfo*& info, bool registerOpCodes = true);
	const OPCodeInfo* LookupOpCode(byte vm, Platform platform, uint16_t opcode);
	std::pair<bool, uint16_t> GetOpCodeId(byte vm, Platform platform, OPCode opcode);
	bool HasOpCode(byte vm, Platform plt, OPCode opcode);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	void RegisterVM(byte vm, const char* name, const char* codeName, const char* internalName, uint64_t flags);
	void RegisterVMGlobalVariable(byte vm, const char* name, OPCode getOpCode = OPCODE_Undefined);
	void RegisterVMOperatorFunction(byte vm, const char* name, const char* usage, OPCode opcode, int flags, int minArgs = 0, int maxArgs = 255);
	void RegisterVMPlatform(byte vm, Platform plt);
	void RegisterVMHashOPCode(byte vm, char type, OPCode opCode, int size, std::function<uint64_t(const char*)> hashFunc);
	void RegisterOpCode(byte vm, Platform platform, OPCode enumValue, uint16_t op);
	void RegisterSameCodePlatform(byte vm, Platform main, Platform sub);
	Platform RemapSamePlatform(byte vm, Platform origin);
	void SetMaxOpCode(byte vm, uint16_t maxOpCode);
	void RegisterDevCall(byte vm, const char* devCall);
	void RegisterDatatypeRenamed(byte vm, const char* datatype, const char* trueName);
	void RegisterDatatype(byte vm, const char* datatype);
	void RegisterVmName(byte vm, uint64_t hash);
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

	inline void RegisterVmName(byte vm, const char* name) {
		RegisterVmName(vm, hash::Hash64(name));
	}
	inline void RegisterVmName(byte vm) {}
	template<typename... Names>
	inline void RegisterVmName(byte vm, const char* name, Names... names) {
		RegisterVmName(vm, name);
		RegisterVmName(vm, names...);
	}


	inline void RegisterDatatype(byte vm) {}
	template<typename... Datatypes>
	inline void RegisterDatatype(byte vm, const char* datatype, Datatypes... datatypes) {
		RegisterDatatype(vm, datatype);
		RegisterDatatype(vm, datatypes...);
	}

}