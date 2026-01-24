#include <includes.hpp>
#include <core/async.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_formatter.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm_nodes.hpp>

using namespace tool::gsc::opcode;

namespace {
	// maps to find the opcodes
	
	// VM->vminfo
	std::unordered_map<uint64_t, VmInfo> g_opcodeMap{};
	std::unordered_map<uint64_t, uint64_t> g_vmMap{};


	static struct {
		const char* id;
		const char* name;
	} platformNames[]{
		{"unk", "Unknown"},
		{"pc", "PC"},
		{"ps", "PlayStation"},
		{"xbox", "Xbox"},
		{"pc_alpha", "Alpha"},
		{"test", "Test"},
		{"bo6_v1", "Black Ops 6 (V1)"},
		{"bo6", "Black Ops 6"},
		{"bo7", "Black Ops 7"},
	};
	static_assert(ARRAYSIZE(platformNames) == PLATFORM_COUNT && "platform(s) added without names");
}
namespace tool::gsc::opcode {
	VMId VMOf(const char* name) {
		RegisterOpCodes();
		auto it = g_vmMap.find(hash::Hash64(name));

		if (it != g_vmMap.end()) {
			return (VMId)it->second;
		}
		return VMI_UNKNOWN;
	}
	VMId OldVmOf(byte oldvm) {
		switch (oldvm) {
		case VMOld_T71B: return VMI_T7_1B;
		case VMOld_T7: return VMI_T7_1C;
		case VMOld_T831: return VMI_T8_31;
		case VMOld_T8: return VMI_T8_36;
		case VMOld_T937: return VMI_T9_37;
		case VMOld_T9: return VMI_T9_38;
		case VMOld_MW23: return VMI_JUP_8A;
		case VMOld_MW23B: return VMI_JUP_8B;
		case VMOld_BO6_06: return VMI_T10_06;
		case VMOld_BO6_07: return VMI_T10_07;
		case VMOld_BO6_0B: return VMI_T10_0B;
		case VMOld_BO6_0C: return VMI_T10_0C;
		case VMOld_ACTS_F1: return VMI_ACTS_F1;
		default: return VMI_UNKNOWN;
		}
	}
	byte MapAsOldVM(uint64_t magicVal) {
		if ((magicVal & ~0xFF) == VMI_IW_BASE) {
			return magicVal & 0xFF; // IW
		}
		else if ((magicVal & ~0xFF00000000000000) == VMI_TRE_BASE) {
			return (magicVal >> 56) & 0xFF; // Treyarch
		}
		return 0; // unk
	}

	const char* VMIdFancyName(uint64_t magicVal) {
		uint64_t vm;
		const char* type;
		if ((magicVal & ~0xFF) == VMI_IW_BASE) {
			vm = (magicVal & 0xFF); // IW
			type = "iw";
		}
		else if ((magicVal & ~0xFF00000000000000) == VMI_TRE_BASE) {
			vm = ((magicVal >> 56) & 0xFF); // Treyarch
			type = "t";
		}
		else if ((magicVal & ~0xFF00000000000000) == VMI_DBG_TRE_BASE) {
			vm = ((magicVal >> 56) & 0xFF); // Treyarch dbg
			type = "dbg-t";
		}
		else {
			vm = magicVal;
			type = "";
		}

		return utils::va("vm-%s%02llx", type, vm);
	}

	Platform PlatformOf(const char* name) {
		if (!_strcmpi("pc", name)) {
			return PLATFORM_PC;
		}
		if (!_strcmpi("pc_alpha", name) || !_strcmpi("pca", name) || !_strcmpi("alpha", name) || !_strcmpi("a", name)) {
			return PLATFORM_PC_ALPHA;
		}
		if (!_strcmpi("xbox", name) || !_strcmpi("xb", name)) {
			return PLATFORM_XBOX;
		}
		if (!_strcmpi("ps", name) || !_strcmpi("ps4", name) || !_strcmpi("playstation", name)) {
			return PLATFORM_PLAYSTATION;
		}
		if (!_strcmpi("acts", name) || !_strcmpi("test", name)) {
			return PLATFORM_ACTS_TEST;
		}
		if (!_strcmpi("bo6_v1", name)) {
			return PLATFORM_BO6_V1;
		}
		if (!_strcmpi("bo6", name)) {
			return PLATFORM_BO6;
		}
		if (!_strcmpi("bo7", name)) {
			return PLATFORM_BO7;
		}
		return PLATFORM_UNKNOWN;
	}

	const char* PlatformName(Platform plt) {
		return platformNames[plt < PLATFORM_COUNT ? plt : PLATFORM_UNKNOWN].name;
	}

	const char* PlatformIdName(Platform plt) {
		return platformNames[plt < PLATFORM_COUNT ? plt : PLATFORM_UNKNOWN].id;
	}

	void VmInfo::AddDevCallName(uint64_t name) {
		devCallsNames.insert(name & hash::MASK60);
	}
	void VmInfo::SetCompilerHookFunctionName(const char* name) {
		uint64_t hash{ HashField(name) };
		if (compilerHookFunctionName && compilerHookFunctionName != hash) LOG_WARNING("compiler hook function defined twice for {}", name);
		compilerHookFunctionName = hash;
	}

	uint64_t VmInfo::HashField(const char* value) const {
		uint64_t t;
		if (!value || !*value) {
			return 0;
		}
		if (hash::TryHashPattern(value, t)) {
			return t;
		}
		if (HasFlag(VmFlags::VMF_HASH_ACTS)) {
			return hash::Hash64A(value);
		}
		if (HasFlag(VmFlags::VMF_HASH_CER)) {
			return hash::HashT10Scr(value);
		}
		if (HasFlag(VmFlags::VMF_HASH_CER_SP)) {
			return hash::HashT10ScrSP(value);
		}
		if (HasFlag(VmFlags::VMF_HASH_IW)) {
			return hash::HashJupScr(value);
		}
		if (HasFlag(VmFlags::VMF_HASH_T7)) {
			return hash::HashT7(value);
		}
		return hash::HashT89Scr(value);
	}

	uint64_t VmInfo::HashPath(const char* value) const {
		uint64_t t;
		if (!value || !*value) {
			return 0;
		}
		if (hash::TryHashPattern(value, t)) {
			return t;
		}
		if (HasFlag(VmFlags::VMF_HASH_PATH_IW)) {
			return hash::HashIWAsset(value);
		}
		return hash::Hash64(value);
	}

	uint64_t VmInfo::HashFilePath(const char* value) const {
		return HashField(value);
	}

	bool VmInfo::IsScrHash(uint64_t hash) const {
		if (HasFlag(VMF_UNIQUE_HASH)) return false;
		const char* pt{ hashutils::ExtractPtr(hash) };
		return pt && (HashField(pt) & hash::MASK60) == (hash & hash::MASK60);
	}

	VmInfo* RegisterVM(uint64_t vm, const char* name, const char* codeName, const char* internalName, uint64_t flags) {
		auto it = g_opcodeMap.find(vm);
		if (it != g_opcodeMap.end() && it->second.vmMagic) {
			if (it->second.flags != flags) {
				LOG_WARNING("Registering twice the same vm with different flags, {:x} != {:x} for {}", it->second.flags, flags, name);
			}
			return &it->second; // assuming good name
		}
		VmInfo& nfo = g_opcodeMap[vm];
		nfo = { vm, name, codeName, internalName, flags, {} };
		g_vmMap[hash::Hash64(utils::va("%llx", vm))] = vm;
		byte revVm{ tool::gsc::opcode::MapAsOldVM(vm) };
		if (revVm) {
			g_vmMap[hash::Hash64(utils::va("%02x", revVm))] = vm;
		}
		g_vmMap[hash::Hash64(internalName)] = vm;
		return &nfo;
	}
	VmInfo* GetVm(uint64_t vm) {
		VmInfo& nfo = g_opcodeMap[vm];

		if (!nfo.vmMagic) {
			LOG_WARNING("Can't find vm {:x}", vm);
		}
		return &nfo;
	}

	void VmInfo::RegisterVmName(uint64_t hash) {
		g_vmMap[hash] = vmMagic;
	}

	void VmInfo::RegisterVMOperatorFunction(const char* name, const char* usage, OPCode opcode, int flags, int minArgs, int maxArgs) {
		uint64_t funcHash = HashField(name);

		if (opFuncs.find(funcHash) != opFuncs.end()) {
			LOG_ERROR("Operator function already defined for vm {}, func: {}", name, name);
			return;
		}

		auto& gv = opFuncs[funcHash];
		gv.name = name;
		gv.usage = usage;
		gv.opCode = opcode;
		gv.flags = flags;
		gv.minParam = minArgs;
		gv.maxParam = maxArgs;
	}

	void VmInfo::RegisterVMGlobalVariable(const char* name, OPCode getOpCode) {
		uint64_t hash = HashField(name);

		if (globalvars.find(hash) != globalvars.end()) {
			LOG_ERROR("Global variable already defined for vm {}, gvar: {}", this->name, name);
			return;
		}

		auto& gv = globalvars[hash];
		gv.name = name;
		gv.getOpCode = getOpCode;
	}

	void VmInfo::SetMaxOpCode(uint16_t maxOpCode) {
		this->maxOpCode = maxOpCode;
	}

	void VmInfo::SetModToolFlag(uint16_t flag) {
		this->modToolFlag = flag;
	}

	void VmInfo::SetOpaqueStringCount(size_t opaqueStringCount) {
		this->opaqueStringCount = opaqueStringCount;
	}

	void VmInfo::SetGDBType(uint64_t gdbMagic) {
		this->gdbMagic = gdbMagic;
	}

	void VmInfo::RegisterVMHashOPCode(char type, OPCode opCode, int size, std::function<uint64_t(const char*)> hashFunc) {
		if (!(size == 8 || size == 4)) {
			LOG_ERROR("Invalid size for hash vm {}: '{}' / {} bytes", name, type, size);
			return;
		}

		auto [h, ok] = hashesFunc.try_emplace(type, type, opCode, size, hashFunc);

		if (!ok) {
			LOG_ERROR("Registered existing hash into vm {}: '{}'", name, type);
			return;
		}
	}

	void VmInfo::RegisterOpCode(Platform platform, OPCode enumValue, uint16_t op) {
		opcodemap[op][platform] = enumValue;
		if (op & modToolFlag) {
			opcodemaplookupModTool[enumValue][platform].push_back(op);
		}
		else {
			opcodemaplookup[enumValue][platform].push_back(op);
		}
		opcodemappltlookup[platform][enumValue].insert(op);
	}

	void VmInfo::ClearPlatformOpCode(Platform platform) {
		for (auto& [op, pltop] : opcodemap) {
			auto it{ pltop.find(platform) };
			if (it != pltop.end()) {
				pltop.erase(it);
			}
		}
		for (auto& [op, pltop] : opcodemaplookup) {
			auto it{ pltop.find(platform) };
			if (it != pltop.end()) {
				pltop.erase(it);
			}
		}

		auto it{ opcodemappltlookup.find(platform) };
		if (it != opcodemappltlookup.end()) {
			opcodemappltlookup.erase(it);
		}
	}

	void VmInfo::RegisterSameCodePlatform(Platform main, Platform sub) {
		AddPlatform(sub);

		Platform to = main;

		while (true) {
			auto svmIt = sameVmMap.find(to);

			if (svmIt == sameVmMap.end()) {
				break;
			}

			to = svmIt->second;
		}

		if (to == sub) {
			LOG_ERROR("Trying to register cycling same code platform for vm {} {} -> {}", name, PlatformName(main), PlatformName(sub));
			return;
		}

		sameVmMap[sub] = to;
	}
	Platform VmInfo::RemapSamePlatform(Platform origin) const {
		auto svmIt = sameVmMap.find(origin);

		if (svmIt == sameVmMap.end()) {
			return origin;
		}

		return svmIt->second;
	}
	Platform VmInfo::FindPlatform(VMExpectedValue* values, size_t count) const {
		if (!count || !values) {
			return {};
		}

		size_t valids[opcode::Platform::PLATFORM_COUNT]{};

		for (size_t i = 0; i < count; i++) {
			VMExpectedValue& ev{ values[i] };

			auto it{ opcodemap.find(ev.value) };
			if (it == opcodemap.end()) {
				return {}; // no opcode value
			}

			bool anyMatch{};
			for (auto& [plt, opcode] : it->second) {
				if (opcode != ev.opcode) {
					continue;
				}
				valids[plt]++;
				anyMatch = true;
			}

			if (!anyMatch) {
				return {}; // no platform matching
			}
		}

		Platform res{};

		for (size_t i = 0; i < opcode::Platform::PLATFORM_COUNT; i++) {
			// check that we have only one platform missing
			if (valids[i] == count) {
				if (res) {
					return {}; // duplicated
				}
				res = (opcode::Platform)i;
			}
		}

		return res;
	}

	void VmInfo::RegisterDatatypeRenamed(const char* datatype, const char* trueName) {
		dataType[hash::Hash64(datatype)] = trueName;
	}

	void VmInfo::RegisterDatatype(const char* datatype) {
		dataType[hash::Hash64(datatype)] = datatype;
	}

	void VmInfo::RegisterDevCall(const char* devCall) {
		uint64_t hash = HashField(devCall);

		//LOG_TRACE("add dev call for {} {:x} -> {}", opnfo.internalName, hash, devCall);
		AddDevCallName(hash);
	}

	const std::unordered_map<uint64_t, VmInfo>& GetVMMaps() {
		RegisterOpCodes();
		return g_opcodeMap;
	}


	bool IsValidVmMagic(uint64_t magic, VmInfo*& info, bool registerOpCodes) {
		// same function, before we used the revision, now we use the magic
		return IsValidVm(magic, info, registerOpCodes);
	}

	bool IsValidVm(uint64_t vm, VmInfo*& info, bool registerOpCodes) {
		// build map
		if (registerOpCodes) RegisterOpCodes();
		auto ref = g_opcodeMap.find(vm);
		if (ref == g_opcodeMap.end()) {
			return false;
		}
		info = &ref->second;

		return true;
	}

	std::pair<bool, uint16_t> GetOpCodeId(uint64_t vm, Platform platform, OPCode opcode, bool modTool) {
		RegisterOpCodes();

		VmInfo* info;

		if (!(IsValidVm(vm, info))) {
			return std::make_pair(false, 0);
		}

		if (modTool && info->modToolFlag == 0) {
			return std::make_pair(false, 0); // no mod tool flag
		}

		auto& map{ modTool ? info->opcodemaplookupModTool : info->opcodemaplookup };

		auto ref = map.find(opcode);

		if (ref == map.end()) {
			return std::make_pair(false, 0);
		}

		auto ref2 = ref->second.find(info->RemapSamePlatform(platform));

		if (ref2 == ref->second.end() || ref2->second.empty()) {
			return std::make_pair(false, 0);
		}

		return std::make_pair(true, ref2->second[rand() % ref2->second.size()]);
	}

	bool HasOpCode(uint64_t vm, Platform plt, OPCode opcode, bool modTool) {
		auto [ok, id] = GetOpCodeId(vm, plt, opcode, modTool);
		return ok;
	}

}
