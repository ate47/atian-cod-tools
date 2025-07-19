#pragma once
#include <Windows.h>
#include "gsc_opcodes_load.hpp"
#include <unordered_map>

namespace tool::gsc::opcode {
	enum VmFlags : uint64_t {
		VMF_NONE = 0,
		VMF_ALIGN = 1,
		VMF_HASH64 = 1 << 1,
		VMF_NO_VERSION = 1 << 2,
		VMF_NO_PARAM_FLAGS = 1 << 3,
		VMF_FULL_FILE_NAMESPACE = 1 << 4,
		VMF_INV_ADD_TO_OBJECT = 1 << 5,
		VMF_HASH_IW = 1 << 6,
		VMF_CLIENT_VM = 1 << 7,
		VMF_NO_FILE_NAMESPACE = 1 << 8,
		VMF_OPCODE_U16 = 1 << 9,
		VMF_CALL_NO_PARAMS = 1 << 10,
		VMF_IW_CALLS = 1 << 11,
		VMF_EXPORT_NOCHECKSUM = 1 << 12,
		VMF_HASH_CER = 1 << 13,
		VMF_ANIMTREE_T7 = 1 << 14,
		VMF_HASH_T7 = 1 << 15,
		VMF_FOREACH_IW = 1 << 16,
		VMF_CRC_DUMP = 1 << 17,
		VMF_EXPORT_CRC32 = 1 << 18,
		VMF_HASH_CER_SP = 1 << 19,
		VMF_HASH_PATH_IW = 1 << 20,
		VMF_GSCBIN = 1 << 21,
		VMF_NO_MAGIC = 1 << 22,
		VMF_VAR_ID = 1 << 23,
		VMF_IW_LIKE = 1 << 24,
		VMF_FOREACH_IW_KEYS = 1 << 25,
		VMF_ISTRING = 1 << 26,
		VMF_ISTRING_HASHED = 1 << 27,
		VMF_FUNC_REF_24 = 1 << 28,
		VMF_SWITCH_32 = 1 << 29,
		VMF_UNIQUE_HASH = 1 << 30,
	};
	enum VmOperatorFunctionData : uint64_t {
		VPFD_NONE = 0,
		VPFD_USE_COUNT = 1,
		VPFD_USE_PRE_SCRIPT_CALL = 1 << 1,
		VPFD_RETURN_VALUE = 1 << 2,
		VPFD_SELF_PARAM = 1 << 3,
		VPFD_HASH_PARAM = 1 << 4,
		VPFD_UNPACK = 1 << 5,
		VPFD_STRING_PARAM = 1 << 6,
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
		uint64_t vmMagic{};
		const char* name{ "unknown" };
		const char* codeName{ "unk" };
		const char* internalName{ "unk" };
		uint64_t flags{};
		uint64_t compilerHookFunctionName{};
		byte platforms{};
		uint16_t maxOpCode{ 0xFFF };
		uint16_t modToolFlag{};
		size_t opaqueStringCount{};
		uint64_t gdbMagic{};
		std::unordered_map<char, VmHashFunc> hashesFunc{};
		std::unordered_set<uint64_t> devCallsNames{};
		std::unordered_map<uint16_t, std::unordered_map<Platform, OPCode>> opcodemap{};
		std::unordered_map<OPCode, std::unordered_map<Platform, std::vector<uint16_t>>> opcodemaplookup{};
		std::unordered_map<OPCode, std::unordered_map<Platform, std::vector<uint16_t>>> opcodemaplookupModTool{};
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
		 * Set the compiler:: hook function name
		 */
		void SetCompilerHookFunctionName(const char* name);

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
		bool IsScrHash(uint64_t hash) const;

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

		void RegisterVMGlobalVariable(const char* name, OPCode getOpCode = OPCODE_Undefined);
		void RegisterVMOperatorFunction(const char* name, const char* usage, OPCode opcode, int flags, int minArgs = 0, int maxArgs = 255);
		void RegisterVMHashOPCode(char type, OPCode opCode, int size, std::function<uint64_t(const char*)> hashFunc);
		void RegisterOpCode(Platform platform, OPCode enumValue, uint16_t op);
		void RegisterSameCodePlatform(Platform main, Platform sub);
		void ClearPlatformOpCode(Platform platform);
		void SetMaxOpCode(uint16_t maxOpCode);
		void SetModToolFlag(uint16_t flag);
		void SetOpaqueStringCount(size_t opaqueStringCount);
		void SetGDBType(uint64_t gdbMagic);
		void RegisterDevCall(const char* devCall);
		void RegisterDatatypeRenamed(const char* datatype, const char* trueName);
		void RegisterDatatype(const char* datatype);
		void RegisterVmName(uint64_t hash);
		inline void RegisterOpCode(Platform platform, OPCode enumValue) {}
		template<typename... OpTypes>
		inline void RegisterOpCode(Platform platform, OPCode enumValue, uint16_t op, OpTypes... ops) {
			RegisterOpCode(platform, enumValue, op);
			RegisterOpCode(platform, enumValue, ops...);
		}


		inline void RegisterDevCall() {}
		template<typename... DevCalls>
		inline void RegisterDevCall(const char* devCall, DevCalls... calls) {
			RegisterDevCall(devCall);
			RegisterDevCall(calls...);
		}

		inline void RegisterVmName(const char* name) {
			RegisterVmName(hash::Hash64(name));
		}
		inline void RegisterVmName() {}
		template<typename... Names>
		inline void RegisterVmName(const char* name, Names... names) {
			RegisterVmName(name);
			RegisterVmName(names...);
		}


		inline void RegisterDatatype() {}
		template<typename... Datatypes>
		inline void RegisterDatatype(const char* datatype, Datatypes... datatypes) {
			RegisterDatatype(datatype);
			RegisterDatatype(datatypes...);
		}

	};
	const std::unordered_map<uint64_t, VmInfo>& GetVMMaps();
	bool IsValidVmMagic(uint64_t magic, VmInfo*& info, bool registerOpCodes = true);
	bool IsValidVm(uint64_t vm, VmInfo*& info, bool registerOpCodes = true);
	const OPCodeInfo* LookupOpCode(uint64_t vm, Platform platform, uint16_t opcode);
	std::pair<bool, uint16_t> GetOpCodeId(uint64_t vm, Platform platform, OPCode opcode, bool modTool = false);
	bool HasOpCode(uint64_t vm, Platform plt, OPCode opcode, bool modTool = false);
	void RegisterOpCodeHandler(const OPCodeInfo* info);
	VmInfo* RegisterVM(uint64_t vm, const char* name, const char* codeName, const char* internalName, uint64_t flags);
	VmInfo* GetVm(uint64_t vm);
	void RegisterOpCodes();

}