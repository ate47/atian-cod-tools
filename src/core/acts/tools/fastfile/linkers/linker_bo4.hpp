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
		std::unordered_map<uint64_t, fastfile::linker::memory::LinkerDataChunk*> allocatedAssets{};
	};
	struct BO4LinkContext;

	BO4LinkContext& GetLinkContext();

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

		void LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BO4FFContext* ff);
		size_t LinkAssetArray(XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count, BO4FFContext* ff);
		void AddXHash(const char* val, XHash& value);
		size_t AddXHashArray(const char* id, core::config::RapidJsonGeneric& cfg, XHash* array, size_t count);
		void AddScrString(const char* val, ScrString_t& value, BO4FFContext& ff);
		size_t AddScrStringArray(const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BO4FFContext& ff);

		template<typename T>
		void LinkAsset(XAssetType type, const char* id, T*& ref, bool addAsset, BO4FFContext* ff) {
			LinkAsset(type, id, *(void**)&ref, addAsset, ff);
		}
		template<typename T>
		size_t LinkAssetArray(XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T** ref, size_t count, BO4FFContext* ff) {
			return LinkAssetArray(type, id, cfg, (void**)ref, count, ff);
		}
		template<typename T, size_t count>
		size_t LinkAssetArray(XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T*(&ref)[count], BO4FFContext* ff) {
			T** p{ ref };
			return LinkAssetArray(type, id, cfg, (void**)p, count, ff);
		}
		template<size_t count>
		size_t AddScrStringArray(const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t(&array)[count], BO4FFContext& ff) {
			return AddScrStringArray(id, cfg, array, count, ff);
		}
		inline void AddXHash(const std::string& str, XHash& value) {
			AddXHash(str.data(), value);
		}
		inline void AddScrString(const std::string& str, ScrString_t& value, BO4FFContext& ff) {
			AddScrString(str.data(), value, ff);
		}

		bool Assert(bool expr, const char* msg);
		inline bool Assert(bool expr, const std::string& msg) {
			return Assert(expr, msg.data());
		}
	};

	struct GfxImage;
	typedef const char* XString;
	struct vec2_t {
		float x;
		float y;
	};
	struct vec3_t {
		float x;
		float y;
		float z;
	};
	struct vec4_t {
		float x;
		float y;
		float z;
		float w;
	};

	class BO4TypeGetter : public core::config::BaseTypeGetters {
	public:
		using BaseTypeGetters::Load;
		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, XHash& ref, XHash defaultValue) {
			GetLinkContext().AddXHash(base.GetCString(path), ref);
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, vec2_t& ref, vec2_t defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;

			if (sub.base.IsString()) {
				if (!base.ScanString(path, "%f, %f", &ref.x, &ref.y)) {
					LOG_ERROR("Bad format for {} \"123, 123\"", path);
					GetLinkContext().error = true;
				}
			}
			else {
				sub.Load("x", ref.x);
				sub.Load("y", ref.y);
			}
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, vec3_t& ref, vec3_t defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;

			if (sub.base.IsString()) {
				if (!base.ScanString(path, "%f, %f, %f", &ref.x, &ref.y, &ref.z)) {
					LOG_ERROR("Bad format for {} \"123, 123, 123\"", path);
					GetLinkContext().error = true;
				}
			}
			else {
				sub.Load("x", ref.x);
				sub.Load("y", ref.y);
				sub.Load("z", ref.z);
			}
		}

		template<typename CF = core::config::ConfigGeneric<BO4TypeGetter>>
		static void Load(const char* path, CF& base, vec4_t& ref, vec4_t defaultValue) {
			CF sub{ base.GetSubVal(path) };
			if (!sub) return;

			if (sub.base.IsString()) {
				if (!base.ScanString(path, "%f, %f, %f, %f", &ref.x, &ref.y, &ref.z, &ref.w)) {
					LOG_ERROR("Bad format for {} \"123, 123, 123, 123\"", path);
					GetLinkContext().error = true;
				}
			}
			else {
				sub.Load("x", ref.x);
				sub.Load("y", ref.y);
				sub.Load("z", ref.z);
				sub.Load("w", ref.w);
			}
		}
	};

	using BO4Json = core::config::ConfigDocument<BO4TypeGetter>;

	class XAssetLinker {
	public:
		// does LinkAsset add an asset if asked
		bool isGrouped;

		XAssetLinker(bool isGrouped = false) : isGrouped(isGrouped) {
		}
		virtual void Compute(BO4LinkContext& ctx, const char* id, BO4FFContext& ff) = 0;
		virtual void ComputeFinal(BO4LinkContext& ctx, BO4FFContext& ff) {}
	};

	std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers();
}