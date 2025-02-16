#pragma once
#include <core/config.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::linker::bo4 {
	struct XHash {
		uint64_t hash;
		uint64_t nulled;
	};
	struct XAsset {
		games::bo4::pool::XAssetType type;
		uintptr_t header; // XAssetHeader
	};

	struct ScriptStringList {
		int count;
		uintptr_t strings; // const char**
	};

	struct XAssetList {
		ScriptStringList stringList;
		int assetCount;
		uintptr_t assets; // XAsset*
	};

	struct BO4LinkContext {
		FastFileLinkerContext& linkCtx;
		core::config::Config& ffConfig;
		std::vector<byte> assetData{};
		std::vector<const char*> strings{};
		std::vector<XAsset> assets{};
		bool error{};
	};

	class LinkerWorker {
	public:
		const char* id;
		LinkerWorker(const char* id) : id(id) {}

		virtual void Compute(BO4LinkContext& ctx) = 0;
	};

	uint64_t HashPathName(const std::filesystem::path& path);

	std::vector<LinkerWorker*>& GetWorkers();
}