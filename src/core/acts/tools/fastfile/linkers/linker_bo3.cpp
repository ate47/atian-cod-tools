#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo3.hpp>
#include <core/preprocessor.hpp>

namespace fastfile::linker::bo3 {
    BO3FFContext::BO3FFContext() : data{ XFILE_BLOCK_COUNT, XFILE_BLOCK_VIRTUAL } {
        data.SetMode(fastfile::linker::memory::LM_DATA);
        data.SetBlockType(XFILE_BLOCK_TEMP, fastfile::linker::memory::BLOCKTYPE_TEMP);
        data.SetBlockType(XFILE_BLOCK_RUNTIME_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_RUNTIME_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_DELAY_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_DELAY_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_STREAMER_RESERVED, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_STREAMER, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_MEMMAPPED, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
    }
    std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers() {
        // could be an array like in bo3 linker
        static std::unordered_map<XAssetType, XAssetLinker*> workers{};
        return workers;
    }

    uint32_t BO3LinkContext::HashScr(const char* str) {
        if (!str) {
            return 0;
        }
        uint64_t r;
        if (hash::TryHashPattern(str, r)) {
            return (uint32_t)r; // nothing to add
        }

        r = hash::HashT7(str);

        linkCtx.RegisterHash(r, str);

        return (uint32_t)r;
    }

    BO3FFContext& BO3LinkContext::GetFFContext(const char* prefix) {
        BO3FFContext& sctx{ ffs[prefix] };
        if (!sctx.ffname) {
            sctx.ffname = this->linkCtx.strs.CloneStr(utils::va("%s%s", prefix, mainFF.ffname));
            sctx.data.SetMode(fastfile::linker::memory::LM_DATA);
        }
        return sctx;
    }

    static BO3LinkContext* linkContext{};

    BO3LinkContext& GetLinkContext() {
        if (!linkContext) {
            throw std::runtime_error("No BO3LinkContext");
        }
        return *linkContext;
    }

    class FFLinkerBO3 : public FFLinker {
      public:
        FFLinkerBO3() : FFLinker("Bo3", "Bo3 fastfile linker") {}

        void Link(FastFileLinkerContext& ctx) override {
            BO3LinkContext bo3ctx{ ctx };
            linkContext = &bo3ctx;
            utils::CloseEnd linkContextCE{ [] { linkContext = nullptr; } };
            bo3ctx.mainFF.ffname = ctx.mainFFName;

            for (auto& [t, d] : ctx.zone.assets) {
                XAssetType type{ ::bo3::pool::T7XAssetIdFromName(t.data()) };
                if (type == XAssetType::T7_ASSET_TYPE_COUNT) {
                    bo3ctx.error = true;
                    LOG_ERROR("Can't find asset type: {}", t);
                    continue;
                }

                for (fastfile::zone::AssetData& assetData : d) {
                    void* ref{};
                    bo3ctx.LinkAsset(type, assetData.value, ref, true, &bo3ctx.mainFF);
                    assetData.handled = true;
                }
            }

            // final linking for all the fastfiles
            for (auto& [type, linker] : GetWorkers()) {
                linker->ComputeFinal(bo3ctx, bo3ctx.mainFF);
                for (auto& [ffname, ff] : bo3ctx.ffs) {
                    linker->ComputeFinal(bo3ctx, ff);
                }
            }

            if (bo3ctx.error) {
                throw std::runtime_error("Error when linking fast file data");
            }

            auto LinkFF = [&bo3ctx](BO3FFContext& ff, std::vector<byte>& data, size_t* blocks) {
                ff.data.SetMode(fastfile::linker::memory::LM_HEADER);

                // add to ctx.data the assets/strings headers
                XAssetList assetlist{};

                // write header
                assetlist.stringList.count = (int)ff.data.scrStrings.size() + 1; // +1 for the null
                assetlist.stringList.strings = fastfile::linker::memory::POINTER_NEXT;

                if (ff.data.assets.size()) {
                    assetlist.assetCount = (int)ff.data.assets.size();
                    assetlist.assets = fastfile::linker::memory::POINTER_NEXT;
                }
                ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

                ff.data.WriteStream(assetlist);

                ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
                // write string ref array
                ff.data.Align<void*>();

                ff.data.WriteStream<void*>(nullptr); // empty str
                for (size_t i = 0; i < ff.data.scrStrings.size(); i++) {
                    ff.data.WriteStream<void*>((void*)fastfile::linker::memory::POINTER_NEXT);
                }

                // write strings
                for (const char* str : ff.data.scrStrings) {
                    ff.data.Align<char>();
                    ff.data.WriteStream(str);
                }
                ff.data.PopStream();

                if (ff.data.assets.size()) {
                    ff.data.Align(8); // GetAlignment_XAsset

                    struct XAsset {
                        XAssetType type;
                        uintptr_t header; // XAssetHeader
                    };
                    // write asset array

                    XAsset tmpAsset{};
                    for (const fastfile::linker::memory::AssetData* asset : ff.data.assets) {
                        tmpAsset.type = (XAssetType)asset->type;
                        tmpAsset.header = (uintptr_t)asset->header;
                        ff.data.WriteStream(tmpAsset);
                    }
                }
                ff.data.PopStream();

                ff.data.Link(blocks);

                ff.data.WriteLinkedData(data);

                LOG_INFO(
                    "Fastfile {} data linked with {} asset(s) and {} string(s)",
                    ff.ffname,
                    assetlist.assetCount,
                    assetlist.stringList.count
                );
            };

            fastfile::FastFile& mff{ ctx.fastfiles.emplace_back() };
            mff.ffname = ctx.mainFFName;
            LinkFF(bo3ctx.mainFF, mff.linkedData, mff.blockSizes);

            for (auto& [k, ff] : bo3ctx.ffs) {
                fastfile::FastFile& mff{ ctx.fastfiles.emplace_back() };
                mff.ffname = ff.ffname;
                LinkFF(ff, mff.linkedData, mff.blockSizes);
            }
        }
    };

    void BO3LinkContext::LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BO3FFContext* ff) {
        // assign default ff
        if (!ff) {
            ff = &mainFF;
        }

        LOG_DEBUG(
            "BO3LinkContext::LinkAsset(type={}, id={}, addAsset={}, ff={})",
            ::bo3::pool::T7XAssetName(type),
            !id ? "nullptr" : id,
            addAsset ? "true" : "false",
            ff->ffname
        );
        if (!id) {
            ref = nullptr;
            return; // empty id = no asset to link
        }

        // ignore hash identifier
        const char* real{ id };
        if (*id == '#') {
            id++;
        }
        bool defaultAsset{ *id == ',' };
        if (defaultAsset) {
            id++;
        }

        uint64_t hash{ hash::Hash64(id) };
        /*
        auto ait{ ff->allocatedAssets.find(hash) };

        if (ait != ff->allocatedAssets.end()) {
                // we already know this asset, we can link it there

                // ait->second->refs.push_back(&ref); // fixme
                LOG_INFO("Reuse existing {} {}", ::bo3::pool::T7XAssetName(type), id);
                return;
        }
        */

        ref = (void*)fastfile::linker::memory::POINTER_NEXT;
        ff->data.PushStream(XFILE_BLOCK_TEMP);

        fastfile::linker::memory::LinkerDataChunk* align{ ff->data.Align(8) }; // GetAlignment_XAsset

        fastfile::linker::memory::LinkerDataChunk* computedRef{};
        fastfile::linker::memory::AssetData* asset{};
        if (!defaultAsset) {
            std::unordered_map<XAssetType, XAssetLinker*>& workers{ GetWorkers() };
            auto it{ workers.find(type) };

            if (it == workers.end()) {
                LOG_ERROR("Can't find asset linker for type {}", ::bo3::pool::T7XAssetName(type));
                error = true;
                return;
            }

            // add the asset to the list if required
            if (addAsset && !it->second->isGrouped) {
                asset = ff->data.AddAsset(type, align);
            }
            try {
                it->second->Compute(*this, id, *ff);

                if (computedRef) {
                    ff->allocatedAssets[hash] = computedRef;
                }
            } catch (std::runtime_error& e) {
                error = true;
                LOG_ERROR("Can't link asset {},{} : {}", ::bo3::pool::T7XAssetName(type), id, e.what());
            }
        } else {
            // create empty asset
            LOG_ERROR("empty asset not supported");
            error = true;
        }
        ff->data.PopStream();
    }

    size_t BO3LinkContext::LinkAssetArray(
        XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count,
        BO3FFContext* ff
    ) {
        if (cfg.IsNull()) {
            return 0;
        }
        if (!cfg.IsArray()) {
            LOG_ERROR("BO3LinkContext::LinkAssetArray: {} isn't an array", id);
            error = true;
            return 0;
        }
        auto arr{ cfg.GetArray() };
        if (arr.Size() > count) {
            LOG_ERROR("BO3LinkContext::LinkAssetArray: {} can't contain more than {} element(s)", id, count);
            error = true;
            return 0;
        }

        for (rapidjson::Value& v : arr) {
            if (!v.IsString()) {
                LOG_ERROR("BO3LinkContext::LinkAssetArray: {} can't contain a non string element", id);
                error = true;
                continue;
            }

            const char* str{ v.GetString() };
            LinkAsset(type, str, *array, false, ff);
            array++;
        }

        return arr.Size();
    }

    void BO3LinkContext::AddScrString(const char* val, ScrString_t& value, BO3FFContext& ff) {
        value = (ScrString_t)ff.data.AddScrString(val);
    }

    size_t BO3LinkContext::AddScrStringArray(
        const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BO3FFContext& ff
    ) {
        if (cfg.IsNull()) {
            return 0;
        }
        if (!cfg.IsArray()) {
            LOG_ERROR("BO3LinkContext::AddXHashArray: {} isn't an array", id);
            error = true;
            return 0;
        }
        auto arr{ cfg.GetArray() };
        if (arr.Size() > count) {
            LOG_ERROR("BO3LinkContext::AddXHashArray: {} can't contain more than {} element(s)", id, count);
            error = true;
            return 0;
        }

        for (rapidjson::Value& v : arr) {
            if (!v.IsString()) {
                LOG_ERROR("BO3LinkContext::AddXHashArray: {} can't contain a non string element", id);
                error = true;
                continue;
            }

            const char* str{ v.GetString() };
            AddScrString(str, *array, ff);
            array++;
        }

        return arr.Size();
    }

    bool BO3LinkContext::Assert(bool expr, const char* msg) {
        if (!expr) {
            LOG_ERROR("BO3LinkContext::Assert: {}", msg);
            error = true;
        }
        return expr;
    }

    utils::ArrayAdder<FFLinkerBO3, FFLinker> impl{ GetLinkers() };
} // namespace fastfile::linker::bo3