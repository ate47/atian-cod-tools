#include <includes.hpp>
#include "pools.hpp"

namespace {
	constexpr size_t xassetpools = 0x94073F0;

	int t7poolscripts(Process& proc, int argc, const char* argv[]) {

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
		
		std::filesystem::path outDir;
		if (argc == 2) {
			outDir = "scriptparsetree_t7";
		}
		else {
			outDir = argv[2];
		}

		std::filesystem::create_directories(outDir);
		size_t added{};

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
