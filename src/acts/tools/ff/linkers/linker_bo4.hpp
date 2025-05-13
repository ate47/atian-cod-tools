#pragma once
#include <core/config.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/linkers/linker_data.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::linker::bo4 {
	typedef uint32_t ScrString_t;
	enum XFileBlock : __int32 {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_TEMP_PRELOAD = 0x1,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
		XFILE_BLOCK_VIRTUAL = 0x4,
		XFILE_BLOCK_PHYSICAL = 0x5,
		XFILE_BLOCK_STREAMER = 0x6,
		XFILE_BLOCK_STREAMER_CPU = 0x7,
		XFILE_BLOCK_MEMMAPPED = 0x8,
		XFILE_BLOCK_COUNT = 0x9,
		XFILE_BLOCK_MEMMAPPED2 = 0xA,
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
		fastfile::linker::data::LinkerData data{ XFILE_BLOCK_COUNT, XFILE_BLOCK_TEMP, XFILE_BLOCK_TEMP_PRELOAD };
		bool error{};
		uint64_t ffnameHash{};
	};

	class LinkerWorker {
	public:
		const char* id;
		int priority;
		LinkerWorker(const char* id, int priority = 0) : id(id), priority(priority) {}

		virtual void Compute(BO4LinkContext& ctx) = 0;
	};

	uint64_t HashPathName(const std::filesystem::path& path);

	std::vector<LinkerWorker*>& GetWorkers();
}