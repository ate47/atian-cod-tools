#pragma once
#include <core/config.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/linkers/linker_memory.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::linker::bo4 {

	using XAssetType = games::bo4::pool::XAssetType;
	using BGCacheTypes = games::bo4::pool::BGCacheTypes;
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
		XAssetType type;
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

	class BO4FFContext {
	public:
		BO4FFContext();
		fastfile::linker::memory::XBlockLinker data;
		std::unordered_map<BGCacheTypes, std::unordered_set<uint64_t>> bgcache{};
		std::vector<XHash> forcedServerScripts{};
		std::vector<XHash> forcedClientScripts{};
		uint64_t ffnameHash{};
		const char* ffname{};
	};

	struct BO4LinkContext {
		FastFileLinkerContext& linkCtx;
		BO4FFContext mainFF;
		std::map<std::string, BO4FFContext> ffs{};
		bool error{};

		BO4FFContext& GetFFContext(const char* prefix);

		uint32_t HashScr(const char* str);
		uint64_t HashXHash(const char* str, bool ignoreTop = false);
		inline uint64_t HashXHash(const std::string& str, bool ignoreTop = false) { return HashXHash(str.data(), ignoreTop); }
		inline uint64_t HashScr(const std::string& str) { return HashScr(str.data()); }
		uint64_t HashPathName(const std::filesystem::path& path);

		void LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset = false, BO4FFContext* ff = nullptr);

		template<typename T>
		void LinkAsset(XAssetType type, const char* id, T*& ref, bool addAsset = false, BO4FFContext* ff = nullptr) {
			LinkAsset(type, id, *(void**)&ref, addAsset, ff);
		}
	};

	struct GfxImage;
	typedef float vec3_t[3];

	class XAssetLinker {
	public:
		// does LinkAsset add an asset if asked
		bool isGrouped;

		XAssetLinker(bool isGrouped = false) : isGrouped(isGrouped) {
		}
		virtual void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) = 0;
		virtual void ComputeFinal(BO4LinkContext& ctx, BO4FFContext& ff) {}
	};

	std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers();
}