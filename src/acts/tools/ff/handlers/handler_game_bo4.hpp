#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::handlers::bo4 {
	typedef uint32_t ScrString_t;

	struct XHash {
		uint64_t hash;
		uint64_t pad;
	};

	enum XFileBlock : int {
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
		XFILE_BLOCK_COUNT2 = 0xB,
	};

	enum XFileBlockMemLocation : int {
		XFILE_BLOCK_LOC_VIRTUAL = 0,
		XFILE_BLOCK_LOC_PHYSICAL = 1,
		XFILE_BLOCK_LOC_STREAM = 2,
		XFILE_BLOCK_LOC_STREAM_CPU = 3,
		XFILE_BLOCK_LOC_STREAM_DEV = 4, // generate 1777362536 by default, should be 0
		XFILE_BLOCK_LOC_STREAM_MMAP = 5,
	};

	struct XAsset_0 {
		games::bo4::pool::XAssetType type;
		void* header;
	}; static_assert(sizeof(XAsset_0) == 0x10);

	struct XAssetList_0 {
		int stringsCount;
		char** strings;
		int assetCount;
		XAsset_0* assets;
	}; static_assert(sizeof(XAssetList_0) == 0x20);

	class Worker {
	public:
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
	};

	std::unordered_map<games::bo4::pool::XAssetType, Worker*>& GetWorkers();
	const char* GetScrString(ScrString_t id);
	void SetAssetList(XAssetList_0* assetList);
	const char* XFileBlockName(XFileBlock id);

	XFileBlockMemLocation GetXFileBlockMemLocation(XFileBlock id);
}