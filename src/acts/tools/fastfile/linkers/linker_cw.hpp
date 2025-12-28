#pragma once
#include <core/config.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/linkers/linker_data.hpp>
#include <tools/cw/cw.hpp>


namespace fastfile::linker::cw {
	using namespace ::cw;
	typedef uint32_t ScrString_t;

	enum XFileBlock : __int32 {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_TEMP_PRELOAD = 0x1,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
		XFILE_BLOCK_VIRTUAL = 0x4,
		XFILE_BLOCK_CPU_GPU_SHARE = 0x5,
		XFILE_BLOCK_PHYSICAL = 0x6,
		XFILE_BLOCK_STREAMER = 0x7,
		XFILE_BLOCK_STREAMER_CPU = 0x8,
		XFILE_BLOCK_MEMMAPPED = 0x9,
		XFILE_BLOCK_LOAD_TIME = 0xA,
		XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR = 0xB,
		XFILE_BLOCK_MESH = 0xC,
		XFILE_BLOCK_COUNT = 0xD,
	};


	struct XAsset {
		::cw::XAssetType type;
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

	struct BOCWLinkContext {
		FastFileLinkerContext& linkCtx;
		fastfile::linker::data::LinkerData data{ XFILE_BLOCK_COUNT, XFILE_BLOCK_TEMP, XFILE_BLOCK_TEMP_PRELOAD };
		//std::unordered_map<::cw::BGCacheTypes, std::unordered_set<uint64_t>> bgcache{};
		bool error{};
		uint64_t ffnameHash{};

		uint32_t HashScr(const char* str);
		uint64_t HashXHash(const char* str);
		inline uint64_t HashXHash(const std::string& str) { return HashXHash(str.data()); }
		inline uint64_t HashScr(const std::string& str) { return HashScr(str.data()); }
		uint64_t HashPathName(const std::filesystem::path& path);
	};

	class LinkerWorker {
	public:
		const char* id;
		int priority;
		LinkerWorker(const char* id, int priority = 0) : id(id), priority(priority) {}

		virtual void Compute(BOCWLinkContext& ctx) = 0;
	};

	std::vector<LinkerWorker*>& GetWorkers();
}