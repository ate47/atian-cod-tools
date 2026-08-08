#pragma once
#include <core/config.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/linkers/linker_memory.hpp>
#include <tools/bo3/pools.hpp>

namespace fastfile::linker::bo3 {

    using XAssetType = ::bo3::pool::T7XAssetType;
    typedef uint32_t ScrString_t;

    enum XFileBlock : __int32 {
        XFILE_BLOCK_TEMP = 0x0,
        XFILE_BLOCK_RUNTIME_VIRTUAL = 0x1,
        XFILE_BLOCK_RUNTIME_PHYSICAL = 0x2,
        XFILE_BLOCK_DELAY_VIRTUAL = 0x3,
        XFILE_BLOCK_DELAY_PHYSICAL = 0x4,
        XFILE_BLOCK_VIRTUAL = 0x5,
        XFILE_BLOCK_PHYSICAL = 0x6,
        XFILE_BLOCK_STREAMER_RESERVED = 0x7,
        XFILE_BLOCK_STREAMER = 0x8,
        XFILE_BLOCK_MEMMAPPED = 0x9,
        XFILE_BLOCK_COUNT = 0xA,
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
        uint32_t unk10;
        void* unk18;
        int assetCount;
        uintptr_t assets; // XAsset*
    };

    class BO3FFContext {
      public:
        BO3FFContext();
        fastfile::linker::memory::XBlockLinker data;
        const char* ffname{};
        std::unordered_map<uint64_t, fastfile::linker::memory::LinkerDataChunk*> allocatedAssets{};
    };
    struct BO3LinkContext;

    BO3LinkContext& GetLinkContext();

    struct BO3LinkContext {
        FastFileLinkerContext& linkCtx;
        BO3FFContext mainFF;
        std::map<std::string, BO3FFContext> ffs{};
        bool error{};

        BO3FFContext& GetFFContext(const char* prefix);

        uint32_t HashScr(const char* str);
        inline uint64_t HashScr(const std::string& str) { return HashScr(str.data()); }

        void LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BO3FFContext* ff);
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count,
            BO3FFContext* ff
        );
        void AddScrString(const char* val, ScrString_t& value, BO3FFContext& ff);
        size_t AddScrStringArray(
            const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BO3FFContext& ff
        );

        template<typename T>
        void LinkAsset(XAssetType type, const char* id, T*& ref, bool addAsset, BO3FFContext* ff) {
            LinkAsset(type, id, *(void**)&ref, addAsset, ff);
        }
        template<typename T>
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T** ref, size_t count,
            BO3FFContext* ff
        ) {
            return LinkAssetArray(type, id, cfg, (void**)ref, count, ff);
        }
        template<typename T, size_t count>
        size_t LinkAssetArray(
            XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, T* (&ref)[count], BO3FFContext* ff
        ) {
            T** p{ ref };
            return LinkAssetArray(type, id, cfg, (void**)p, count, ff);
        }
        template<size_t count>
        size_t AddScrStringArray(
            const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t (&array)[count], BO3FFContext& ff
        ) {
            return AddScrStringArray(id, cfg, array, count, ff);
        }
        inline void AddScrString(const std::string& str, ScrString_t& value, BO3FFContext& ff) {
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

    class BO3TypeGetter : public core::config::BaseTypeGetters {
      public:
        using BaseTypeGetters::Load;

        template<typename CF = core::config::ConfigGeneric<BO3TypeGetter>>
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

        template<typename CF = core::config::ConfigGeneric<BO3TypeGetter>>
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

        template<typename CF = core::config::ConfigGeneric<BO3TypeGetter>>
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

    using BO3Json = core::config::ConfigDocument<BO3TypeGetter>;

    class XAssetLinker {
      public:
        // does LinkAsset add an asset if asked
        bool isGrouped;

        XAssetLinker(bool isGrouped = false) : isGrouped(isGrouped) {}
        virtual void Compute(BO3LinkContext& ctx, const char* id, BO3FFContext& ff) = 0;
        virtual void ComputeFinal(BO3LinkContext& ctx, BO3FFContext& ff) {}
    };

    std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers();
} // namespace fastfile::linker::bo3