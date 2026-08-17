#pragma once
#include <core/config.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/linkers/linker_memory.hpp>
#include <tools/cw/cw.hpp>

namespace fastfile::linker::cw {

    using XAssetType = ::cw::XAssetType;
    using BGCacheTypes = ::cw::BGCacheTypes;
    typedef uint32_t ScrString_t;
    typedef uint64_t CWXHash;

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
        XFILE_BLOCK_COUNT_COD2020 = 0xC,
        XFILE_BLOCK_MESH = 0xC,
        XFILE_BLOCK_COUNT = 0xD,
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
        int* unk10;
        int assetCount;
        uintptr_t assets; // XAsset*
    };

    class BOCWFFContext {
      public:
        BOCWFFContext();
        fastfile::linker::memory::XBlockLinker data;
        std::unordered_map<BGCacheTypes, std::unordered_set<uint64_t>> bgcache{};
        uint64_t ffnameHash{};
        const char* ffname{};
        std::unordered_map<uint64_t, fastfile::linker::memory::LinkerDataChunk*> allocatedAssets{};
    };
    struct BOCWLinkContext;

    BOCWLinkContext& GetLinkContext();

    struct BOCWLinkContext {
        FastFileLinkerContext& linkCtx;
        BOCWFFContext mainFF;
        std::map<std::string, BOCWFFContext> ffs{};
        bool error{};

        BOCWFFContext& GetFFContext(const char* prefix);

        uint32_t HashScr(const char* str);
        uint64_t HashXHash(const char* str, bool ignoreTop = false);
        inline uint64_t HashXHash(const std::string& str, bool ignoreTop = false) {
            return HashXHash(str.data(), ignoreTop);
        }
        inline uint64_t HashScr(const std::string& str) { return HashScr(str.data()); }
        uint64_t HashPathName(const std::filesystem::path& path);

        void LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BOCWFFContext* ff);
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count,
            BOCWFFContext* ff
        );
        void AddXHash(const char* val, CWXHash& value);
        size_t AddXHashArray(const char* id, core::config::RapidJsonGeneric& cfg, CWXHash* array, size_t count);
        void AddScrString(const char* val, ScrString_t& value, BOCWFFContext& ff);
        size_t AddScrStringArray(
            const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BOCWFFContext& ff
        );

        template<typename T>
        void LinkAsset(XAssetType type, const char* id, T*& ref, bool addAsset, BOCWFFContext* ff) {
            LinkAsset(type, id, *(void**)&ref, addAsset, ff);
        }
        template<typename T>
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T** ref, size_t count,
            BOCWFFContext* ff
        ) {
            return LinkAssetArray(type, id, cfg, (void**)ref, count, ff);
        }
        template<typename T, size_t count>
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T* (&ref)[count], BOCWFFContext* ff
        ) {
            T** p{ ref };
            return LinkAssetArray(type, id, cfg, (void**)p, count, ff);
        }
        template<size_t count>
        size_t AddScrStringArray(
            const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t (&array)[count], BOCWFFContext& ff
        ) {
            return AddScrStringArray(id, cfg, array, count, ff);
        }
        inline void AddXHash(const std::string& str, CWXHash& value) { AddXHash(str.data(), value); }
        inline void AddScrString(const std::string& str, ScrString_t& value, BOCWFFContext& ff) {
            AddScrString(str.data(), value, ff);
        }

        bool Assert(bool expr, const char* msg);
        inline bool Assert(bool expr, const std::string& msg) { return Assert(expr, msg.data()); }
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

    class BOCWTypeGetter : public core::config::BaseTypeGetters {
      public:
        using BaseTypeGetters::Load;

        template<typename CF = core::config::ConfigGeneric<BOCWTypeGetter>>
        static void Load(const char* path, CF& base, vec2_t& ref, vec2_t defaultValue) {
            CF sub{ base.GetSubVal(path) };
            if (!sub)
                return;

            if (sub.base.IsString()) {
                if (!base.ScanString(path, "%f, %f", &ref.x, &ref.y)) {
                    LOG_ERROR("Bad format for {} \"123, 123\"", path);
                    GetLinkContext().error = true;
                }
            } else {
                sub.Load("x", ref.x);
                sub.Load("y", ref.y);
            }
        }

        template<typename CF = core::config::ConfigGeneric<BOCWTypeGetter>>
        static void Load(const char* path, CF& base, vec3_t& ref, vec3_t defaultValue) {
            CF sub{ base.GetSubVal(path) };
            if (!sub)
                return;

            if (sub.base.IsString()) {
                if (!base.ScanString(path, "%f, %f, %f", &ref.x, &ref.y, &ref.z)) {
                    LOG_ERROR("Bad format for {} \"123, 123, 123\"", path);
                    GetLinkContext().error = true;
                }
            } else {
                sub.Load("x", ref.x);
                sub.Load("y", ref.y);
                sub.Load("z", ref.z);
            }
        }

        template<typename CF = core::config::ConfigGeneric<BOCWTypeGetter>>
        static void Load(const char* path, CF& base, vec4_t& ref, vec4_t defaultValue) {
            CF sub{ base.GetSubVal(path) };
            if (!sub)
                return;

            if (sub.base.IsString()) {
                if (!base.ScanString(path, "%f, %f, %f, %f", &ref.x, &ref.y, &ref.z, &ref.w)) {
                    LOG_ERROR("Bad format for {} \"123, 123, 123, 123\"", path);
                    GetLinkContext().error = true;
                }
            } else {
                sub.Load("x", ref.x);
                sub.Load("y", ref.y);
                sub.Load("z", ref.z);
                sub.Load("w", ref.w);
            }
        }
    };

    using BOCWJson = core::config::ConfigDocument<BOCWTypeGetter>;

    class XAssetLinker {
      public:
        // does LinkAsset add an asset if asked
        bool isGrouped;

        XAssetLinker(bool isGrouped = false) : isGrouped(isGrouped) {}
        virtual void Compute(BOCWLinkContext& ctx, const char* id, BOCWFFContext& ff) = 0;
        virtual void ComputeFinal(BOCWLinkContext& ctx, BOCWFFContext& ff) {}
    };

    std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers();
} // namespace fastfile::linker::cw