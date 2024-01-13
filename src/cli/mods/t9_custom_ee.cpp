#include <includes.hpp>
#include "tools/gsc.hpp"

constexpr auto s_assetPools_off = 0x11E50670;

struct XAssetPool {
	uintptr_t pool; // void*
	unsigned int itemSize;
	int itemCount;
	bool isSingleton;
	int itemAllocCount;
	void* freeHead;
};

struct ScriptParseTree {
	uint64_t name;
	uintptr_t buffer; // GSC_OBJ*
	int len;
};

int t9customee(int argc, const char* argv[]) {

	Process proc{ "BlackOpsColdWar.exe" };

	if (!proc || !proc.Open()) {
		std::cerr << "Can't find process\n";
		return tool::BASIC_ERROR;
	}

	XAssetPool sptPool{};

	if (!proc.ReadMemory(&sptPool, proc[s_assetPools_off] + sizeof(sptPool) * 68, sizeof(sptPool))) {
		std::cerr << "Can't read spt\n";
		return tool::BASIC_ERROR;
	}

	auto entries = std::make_unique<ScriptParseTree[]>(sptPool.itemAllocCount);

	if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
		std::cerr << "Can't read SPT pool entries\n";
		return tool::BASIC_ERROR;
	}


	UINT64 nameGsc = hashutils::Hash64("scripts/zm_common/zm_utility.gsc");
	UINT64 nameCsc = hashutils::Hash64("scripts/zm_common/zm_utility.csc");
	UINT32 targetFunction = hashutils::Hash32("is_ee_enabled");

	int patched = 0;

	tool::gsc::T9GSCOBJ header{};
	for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
		auto& entry = entries[i];

		if (entry.name != nameGsc && entry.name != nameCsc) {
			continue;
		}

		if (!proc.ReadMemory(&header, entry.buffer, sizeof(header))) {
			std::cerr << "Can't read header\n";
			return tool::BASIC_ERROR;
		}

		auto exports = std::make_unique<tool::gsc::T8GSCExport[]>(header.exports_count);

		if (!proc.ReadMemory(&exports[0], entry.buffer + header.exports_tables, sizeof(tool::gsc::T8GSCExport) * header.exports_count)) {
			std::cerr << "Can't read header\n";
			return tool::BASIC_ERROR;
		}

		for (size_t j = 0; j < header.exports_count; j++) {
			auto& exp = exports[j];

			if (exp.name != targetFunction) {
				continue;
			}

			// we use a nop because the global variable ref is shifted by 2 bytes compared to bo4
			// CheckClearParams 0x000d
			// Nop 0x0013
			// GetByte 0x00ca 0x01
			// Align 0x00
			// Return 0x1a
			BYTE data[] = { 0x0d, 0x00, 0x13, 0x00, 0xca, 0x00, 0x01, 0x00, 0x1a, 0x00 };

			if (!proc.WriteMemory(entry.buffer + exp.address, data, sizeof(data))) {
				std::cerr << "error when patching zm_utility::is_ee_enabled\n";
				return tool::BASIC_ERROR;
			}

			break;
		}

		if (++patched == 2) {
			break;
		}
	}

	if (patched) {
		std::cerr << "zm_utility::is_ee_enabled patched, leave the zombies mode to erase\n";
		return tool::OK;
	}
	else {
		std::cerr << "Can't find zm_utility::is_ee_enabled, are you in the zombies mode?\n";
		return tool::BASIC_ERROR;
	}
}
ADD_MOD("t9cee", "Enable EEs in offline (cw)", t9customee);
