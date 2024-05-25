#include <includes.hpp>
#include "pools.hpp"
#include "tools/gsc.hpp"

namespace {
	constexpr size_t xassetpools = 0x94073F0;
	constexpr size_t gscobjinfo = 0x50DC2E0;
	constexpr size_t gscobjinfocount = 0x50EFB60;

	struct objFileInfo_t {
		uintptr_t activeVersion; // GSC_OBJ* 
		uintptr_t unk8; // GSC_OBJ* 
		uintptr_t name; // const char* 
		uintptr_t cseg_start; // byte* 
		uintptr_t cseg_end; // byte* 
		uint64_t unk28;
		int32_t unk30;
		uint32_t unk34;
		uint64_t unk38;
		uint32_t unk40;
		uint32_t unk44;
		uint64_t unk48;
	};

	

	int t7poolscripts(Process& proc, int argc, const char* argv[]) {
		std::filesystem::path outDir;
		if (argc == 2) {
			outDir = "scriptparsetree_t7";
		}
		else {
			outDir = argv[2];
		}

		std::filesystem::create_directories(outDir);
		size_t added{};

		LOG_INFO("Read linked infos...");


		auto [gObjFileInfoCount, ok3] = proc.ReadMemoryArray<int32_t>(proc[gscobjinfocount], 2);
		auto [gObjFileInfo, ok4] = proc.ReadMemoryArray<objFileInfo_t[500]>(proc[gscobjinfo], 2);

		if (!ok3 || !ok4) {
			LOG_ERROR("Error when reading gObjFileInfo");
			return tool::BASIC_ERROR;
		}

		tool::gsc::T7GSCOBJ header{};

		for (size_t inst = 0; inst < 2; inst++) {
			auto& gObjFileInfoI{ gObjFileInfo[inst] };
			for (size_t i = 0; i < gObjFileInfoCount[inst]; i++) {
				auto& entry{ gObjFileInfoI[i] };

				if (!entry.activeVersion || !entry.name) {
					continue;
				}

				const char* name{ proc.ReadStringTmp(entry.name, nullptr) };

				if (!name) {
					continue; // can't read name
				}

				if (!proc.ReadMemory(&header, entry.activeVersion, sizeof(header))) {
					continue; // can't read header
				}

				// probably profile or another shit, but this isn't used by the game
				if (header.script_size < sizeof(header) || header.script_size < header.cseg_offset + header.cseg_size) {
					continue; // bad size, end before cseg
				}
				auto [buffer, okb] = proc.ReadMemoryArray<byte>(entry.activeVersion, header.script_size);

				if (!okb) {
					LOG_ERROR("Can't read script {}", name);
					continue;
				}
				uint64_t magic = *reinterpret_cast<uint64_t*>(&buffer[0]);
				if (magic != 0x1c000a0d43534780 && magic != 0x1b000a0d43534780) {
					LOG_ERROR("Can't read script {}: invalid magic", name);
					continue;
				}

				std::filesystem::path outFile{ outDir / utils::va("%sc", name) }; // add c to do gscc/cscc
				bool isNew{ !std::filesystem::exists(outFile) };
				if (isNew) {
					std::filesystem::create_directories(outFile.parent_path());
					added++;
				}
				if (!utils::WriteFile(outFile, &buffer[0], header.script_size)) {
					LOG_ERROR("Error when dumping {} bytes into {}", header.script_size, outFile.string());
					continue;
				}
				LOG_INFO("Dump into {} ({}){}", outFile.string(), header.script_size, isNew ? " (new)" : "");
			}
		}

		LOG_INFO("For now {} file(s) added", added);

		LOG_INFO("Read SPT pool...");
		const uintptr_t spt = proc[xassetpools] + sizeof(bo3::pool::T7XAssetPool) * bo3::pool::T7_ASSET_TYPE_SCRIPTPARSETREE;
		auto [pool, ok] = proc.ReadMemoryObject<bo3::pool::T7XAssetPool>(spt);

		if (!ok) {
			LOG_ERROR("Can't read SPT pool: {}", spt);
			return tool::BASIC_ERROR;
		}

		auto [entries, ok2] = proc.ReadMemoryArray<bo3::pool::T7ScriptParseTree>(pool->pool, pool->itemAllocCount);

		if (!ok2) {
			LOG_ERROR("Can't read SPT pool data");
			return tool::BASIC_ERROR;
		}

		for (size_t i = 0; i < pool->itemAllocCount; i++) {
			bo3::pool::T7ScriptParseTree& entry{ entries[i] };

			const char* name{ proc.ReadStringTmp(entry.name, nullptr) };

			if (!name) {
				continue;
			}

			auto [buffer, okb] = proc.ReadMemoryArray<byte>(entry.script, entry.scriptSize);

			if (!okb) {
				LOG_ERROR("Can't read script {}", name);
				continue;
			}
			if (*reinterpret_cast<uint64_t*>(&buffer[0]) != 0x1c000a0d43534780) {
				LOG_ERROR("Can't read script {}: invalid magic", name);
				continue;
			}

			std::filesystem::path outFile{ outDir / utils::va("%sc", name)}; // add c to do gscc/cscc
			bool isNew{ !std::filesystem::exists(outFile) };
			if (isNew) {
				std::filesystem::create_directories(outFile.parent_path());
				added++;
			}
			if (!utils::WriteFile(outFile, &buffer[0], entry.scriptSize)) {
				LOG_ERROR("Error when dumping {} bytes into {}", entry.scriptSize, outFile.string());
				continue;
			}
			LOG_INFO("Dump into {} ({}){}", outFile.string(), entry.scriptSize, isNew ? " (new)" : "");
		}

		LOG_INFO("Done, {} file(s) added", added);

		return tool::OK;
	}



	ADD_TOOL("wpst7", "bo3", " [output=scriptparsetree_t7]", "dump pooled scripts (bo3)", L"BlackOps3.exe", t7poolscripts);
}
