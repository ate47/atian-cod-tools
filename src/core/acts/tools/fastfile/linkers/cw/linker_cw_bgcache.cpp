#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/fastfile/linkers/linker_cw.hpp>

namespace {
    using namespace fastfile::linker::cw;
    struct BGCacheInfoDef {
        BGCacheTypes type;
        CWXHash name;
        void* asset;
    };
    static_assert(sizeof(BGCacheInfoDef) == 0x18);

    struct BGCacheInfo {
        CWXHash name;
        BGCacheInfoDef* def;
        int defCount;
    };
    static_assert(sizeof(BGCacheInfo) == 0x18);

    class XAssetLinkerImpl : public XAssetLinker {
      public:
        using XAssetLinker::XAssetLinker;

        void ComputeFinal(BOCWLinkContext& ctx, BOCWFFContext& ff) override {
            size_t entries{};
            for (auto& [_, v] : ff.bgcache) {
                for (auto& vv : v) {
                    entries++;
                }
            }

            if (!entries) {
                return; // nothing to build
            }

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            ff.data.AddAsset(XAssetType::ASSET_TYPE_BGCACHE, ff.data.Align(8));

            BGCacheInfo& bg{ ff.data.AllocStreamRef<BGCacheInfo>() };

            bg.name = ff.ffnameHash;
            bg.def = (BGCacheInfoDef*)fastfile::linker::memory::POINTER_NEXT;
            bg.defCount = (uint32_t)entries;

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
            ff.data.Align(8);
            BGCacheInfoDef* defs{ ff.data.AllocStreamPtr<BGCacheInfoDef>(entries) };

            for (auto& [type, v] : ff.bgcache) {
                for (auto& name : v) {
                    defs->name = name;
                    defs->type = type;
                    defs++;
                }
            }
            ff.data.PopStream();

            ff.data.PopStream();

            LOG_INFO("Added asset bgcache {} (hash_{:x})", ff.ffname, ff.ffnameHash);
        }

        void Compute(BOCWLinkContext& ctx, const char* id, BOCWFFContext& ff) override {
            std::filesystem::path path{ ctx.linkCtx.input / id };
            utils::InFileCE is{ path };
            if (!is) {
                LOG_ERROR("Can't read {}", path.string());
                ctx.error = true;
                return;
            }

            rapidcsv::Document doc{};

            doc.Load(is, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

            LOG_INFO("Loaded bgcache data {}", id);

            if (!doc.GetRowCount())
                return; // empty

            if (doc.GetColumnCount() < 2) {
                LOG_ERROR("Can't read {}: Invalid BGCache structure", path.string());
                ctx.error = true;
                return;
            }

            for (size_t i = 0; i < doc.GetRowCount(); i++) {
                const std::string typeStr{ doc.GetCell<std::string>(0, i) };
                const std::string nameStr{ doc.GetCell<std::string>(1, i) };

                BGCacheTypes type{ ::cw::BgCacheId(typeStr.data()) };

                if (type == BGCacheTypes::BG_CACHE_TYPE_INVALID) {

                    LOG_ERROR("Can't read {}: Unknown bgcache type {}", path.string(), typeStr);
                    ctx.error = true;
                    continue;
                }

                ff.bgcache[type].insert(ctx.HashXHash(nameStr.c_str()));
            }
        }
    };

    utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(),
                                                                      XAssetType::ASSET_TYPE_BGCACHE,
                                                                      true };
} // namespace