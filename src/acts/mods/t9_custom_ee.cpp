#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/cw/cw.hpp"

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
		LOG_ERROR("Can't find process");
		return tool::BASIC_ERROR;
	}

	XAssetPool sptPool{};

	if (!proc.ReadMemory(&sptPool, cw::ScanPool(proc) + sizeof(sptPool) * 68, sizeof(sptPool))) {
		LOG_ERROR("Can't read spt");
		return tool::BASIC_ERROR;
	}

	auto entries = std::make_unique<ScriptParseTree[]>(sptPool.itemAllocCount);

	if (!proc.ReadMemory(&entries[0], sptPool.pool, sptPool.itemAllocCount * sizeof(entries[0]))) {
		LOG_ERROR("Can't read SPT pool entries");
		return tool::BASIC_ERROR;
	}


	int patched = 0;

	tool::gsc::T9GSCOBJ header{};
	for (size_t i = 0; i < sptPool.itemAllocCount; i++) {
		auto& entry = entries[i];

		if (entry.name != hashutils::Hash64("scripts/zm_common/zm_utility.gsc") 
			&& entry.name != hashutils::Hash64("scripts/zm_common/zm_utility.csc")) {
			continue;
		}

		if (!proc.ReadMemory(&header, entry.buffer, sizeof(header))) {
			LOG_ERROR("Can't read header");
			return tool::BASIC_ERROR;
		}

		auto exports = std::make_unique<tool::gsc::T8GSCExport[]>(header.exports_count);

		if (!proc.ReadMemory(&exports[0], entry.buffer + header.exports_tables, sizeof(tool::gsc::T8GSCExport) * header.exports_count)) {
			LOG_ERROR("Can't read header");
			return tool::BASIC_ERROR;
		}

		for (size_t j = 0; j < header.exports_count; j++) {
			auto& exp = exports[j];

			if (exp.name != hashutils::Hash32("is_ee_enabled")) {
				continue;
			}

			// we use a nop because the global variable ref is shifted by 2 bytes compared to bo4
			// CheckClearParams 0x000d
			// Nop 0x0013
			// GetByte 0x00ca 0x01
			// Align 0x00
			// Return 0x1a
			byte data[] = { 0x0d, 0x00, 0x13, 0x00, 0xca, 0x00, 0x01, 0x00, 0x1a, 0x00 };

			if (!proc.WriteMemory(entry.buffer + exp.address, data, sizeof(data))) {
				LOG_ERROR("error when patching zm_utility::is_ee_enabled");
				return tool::BASIC_ERROR;
			}

			break;
		}

		if (++patched == 2) {
			break;
		}
	}

	if (patched) {
		LOG_INFO("zm_utility::is_ee_enabled patched, leave the zombies mode to erase");
		return tool::OK;
	}
	else {
		LOG_ERROR("Can't find zm_utility::is_ee_enabled, are you in the zombies mode?");
		return tool::BASIC_ERROR;
	}
}
ADD_MOD("t9cee", "Enable EEs in offline (cw)", t9customee);
