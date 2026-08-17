#include <includes.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>
#include <core/preprocessor.hpp>

namespace fastfile::linker::cw {
    BOCWFFContext::BOCWFFContext() : data{ XFILE_BLOCK_COUNT, XFILE_BLOCK_VIRTUAL } {
        data.SetMode(fastfile::linker::memory::LM_DATA);
        data.SetBlockType(XFILE_BLOCK_TEMP, fastfile::linker::memory::BLOCKTYPE_TEMP);
        data.SetBlockType(XFILE_BLOCK_TEMP_PRELOAD, fastfile::linker::memory::BLOCKTYPE_TEMP);
        data.SetBlockType(XFILE_BLOCK_RUNTIME_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_RUNTIME_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_VIRTUAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_PHYSICAL, fastfile::linker::memory::BLOCKTYPE_VIRTUAL);
        data.SetBlockType(XFILE_BLOCK_STREAMER, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_STREAMER_CPU, fastfile::linker::memory::BLOCKTYPE_RUNTIME);
        data.SetBlockType(XFILE_BLOCK_CPU_GPU_SHARE, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
        data.SetBlockType(XFILE_BLOCK_LOAD_TIME, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
        data.SetBlockType(XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
        data.SetBlockType(XFILE_BLOCK_MESH, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
        data.SetBlockType(XFILE_BLOCK_MEMMAPPED, fastfile::linker::memory::BLOCKTYPE_UNKNOWN);
    }
    std::unordered_map<XAssetType, XAssetLinker*>& GetWorkers() {
        // could be an array like in bo3 linker
        static std::unordered_map<XAssetType, XAssetLinker*> workers{};
        return workers;
    }

    uint32_t BOCWLinkContext::HashScr(const char* str) {
        if (!str) {
            return 0;
        }
        uint64_t r;
        if (hash::TryHashPattern(str, r)) {
            return (uint32_t)r; // nothing to add
        }

        r = hash::HashT89Scr(str);

        linkCtx.RegisterHash(r, str);

        return (uint32_t)r;
    }

    BOCWFFContext& BOCWLinkContext::GetFFContext(const char* prefix) {
        BOCWFFContext& sctx{ ffs[prefix] };
        if (!sctx.ffnameHash) {
            sctx.ffname = this->linkCtx.strs.CloneStr(utils::va("%s%s", prefix, mainFF.ffname));
            sctx.ffnameHash = HashXHash(sctx.ffname);
            sctx.data.SetMode(fastfile::linker::memory::LM_DATA);
        }
        return sctx;
    }

    uint64_t BOCWLinkContext::HashXHash(const char* str, bool ignoreTop) {
        if (!str) {
            return 0;
        }
        if (ignoreTop && *str == '#') {
            str++;
        }
        uint64_t r;
        if (hash::TryHashPattern(str, r)) {
            return r; // nothing to add
        }

        r = hash::Hash64(str);

        linkCtx.RegisterHash(r, str);

        return r;
    }

    uint64_t BOCWLinkContext::HashPathName(const std::filesystem::path& path) {
        std::filesystem::path p{ path };
        if (p.has_extension()) {
            p.replace_extension();
        }
        std::string fn{ p.filename().string() };
        uint64_t r;
        if (!hash::TryHashPattern(fn.data(), r)) {
            fn = path.string();
            r = HashXHash(fn.data());
        }
        LOG_TRACE("Hash path {} -> 0x{:x}", path.string(), r);
        return r;
    }

    static BOCWLinkContext* linkContext{};

    BOCWLinkContext& GetLinkContext() {
        if (!linkContext) {
            throw std::runtime_error("No BOCWLinkContext");
        }
        return *linkContext;
    }

    class FFLinkerBOCW : public FFLinker {
      public:
        FFLinkerBOCW() : FFLinker("CW", "CW fastfile linker") {}

        void Link(FastFileLinkerContext& ctx) override {
            BOCWLinkContext bocwctx{ ctx };
            linkContext = &bocwctx;
            utils::CloseEnd linkContextCE{ [] { linkContext = nullptr; } };
            bocwctx.mainFF.ffname = ctx.mainFFName;
            bocwctx.mainFF.ffnameHash = bocwctx.HashXHash(ctx.mainFFName);

            for (auto& [t, d] : ctx.zone.assets) {
                XAssetType type{ ::cw::PoolId(t.data()) };
                if (type == XAssetType::ASSET_TYPE_COUNT) {
                    bocwctx.error = true;
                    LOG_ERROR("Can't find asset type: {}", t);
                    continue;
                }

                for (fastfile::zone::AssetData& assetData : d) {
                    void* ref{};
                    bocwctx.LinkAsset(type, assetData.value, ref, true, &bocwctx.mainFF);
                    assetData.handled = true;
                }
            }

            // final linking for all the fastfiles
            for (auto& [type, linker] : GetWorkers()) {
                linker->ComputeFinal(bocwctx, bocwctx.mainFF);
                for (auto& [ffname, ff] : bocwctx.ffs) {
                    linker->ComputeFinal(bocwctx, ff);
                }
            }

            if (bocwctx.error) {
                throw std::runtime_error("Error when linking fast file data");
            }

            auto LinkFF = [&bocwctx](BOCWFFContext& ff, std::vector<byte>& data, size_t* blocks) {
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
            LinkFF(bocwctx.mainFF, mff.linkedData, mff.blockSizes);

            for (auto& [k, ff] : bocwctx.ffs) {
                fastfile::FastFile& mff{ ctx.fastfiles.emplace_back() };
                mff.ffname = ff.ffname;
                LinkFF(ff, mff.linkedData, mff.blockSizes);
            }
        }
    };

    void BOCWLinkContext::LinkAsset(XAssetType type, const char* id, void*& ref, bool addAsset, BOCWFFContext* ff) {
        // assign default ff
        if (!ff) {
            ff = &mainFF;
        }

        LOG_DEBUG(
            "BOCWLinkContext::LinkAsset(type={}, id={}, addAsset={}, ff={})",
            ::cw::PoolName(type),
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
                LOG_INFO("Reuse existing {} {}", ::cw::PoolName(type), id);
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
                LOG_ERROR("Can't find asset linker for type {}", ::cw::PoolName(type));
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
                LOG_ERROR("Can't link asset {},{} : {}", ::cw::PoolName(type), id, e.what());
            }
        } else {
            // create empty asset
            LOG_ERROR("empty asset not supported");
            error = true;
            /*
            size_t len{ games::bo4::pool::GetAssetSize(type) };

            if (!len) {
                LOG_ERROR("Can't link empty asset {}: '{}'", ::cw::PoolName(type), real);
                error = true;
                return;
            }

            if (addAsset) {
                asset = ff->data.AddAsset(type, align);
            }

            computedRef = ff->data.AllocStream(len);
            void* emptyAsset{ computedRef->As<void>() };
            XHash* h{ games::bo4::pool::GetAssetName(type, emptyAsset, len) };

            if (!h) {
                LOG_ERROR(
                    "Can't link asset {} with invalid name location: '{}'",
                    ::cw::PoolName(type),
                    real
                );
                error = true;
                return;
            }
            // we add the flag 63 to mark it as default
            h->name = HashPathName(id) | ~hash::MASK63;

            LOG_INFO("Add default asset {} {} (hash_{:x})", ::cw::PoolName(type), id, h->name);
            */
        }
        ff->data.PopStream();
    }

    size_t BOCWLinkContext::LinkAssetArray(
        XAssetType type, const char* id, core::config::RapidJsonGeneric& cfg, void** array, size_t count,
        BOCWFFContext* ff
    ) {
        if (cfg.IsNull()) {
            return 0;
        }
        if (!cfg.IsArray()) {
            LOG_ERROR("BOCWLinkContext::LinkAssetArray: {} isn't an array", id);
            error = true;
            return 0;
        }
        auto arr{ cfg.GetArray() };
        if (arr.Size() > count) {
            LOG_ERROR("BOCWLinkContext::LinkAssetArray: {} can't contain more than {} element(s)", id, count);
            error = true;
            return 0;
        }

        for (rapidjson::Value& v : arr) {
            if (!v.IsString()) {
                LOG_ERROR("BOCWLinkContext::LinkAssetArray: {} can't contain a non string element", id);
                error = true;
                continue;
            }

            const char* str{ v.GetString() };
            LinkAsset(type, str, *array, false, ff);
            array++;
        }

        return arr.Size();
    }

    void BOCWLinkContext::AddXHash(const char* val, CWXHash& value) { value = HashXHash(val, true); }

    size_t
    BOCWLinkContext::AddXHashArray(const char* id, core::config::RapidJsonGeneric& cfg, CWXHash* array, size_t count) {
        if (cfg.IsNull()) {
            return 0;
        }
        if (!cfg.IsArray()) {
            LOG_ERROR("BOCWLinkContext::AddXHashArray: {} isn't an array", id);
            error = true;
            return 0;
        }
        auto arr{ cfg.GetArray() };
        if (arr.Size() > count) {
            LOG_ERROR("BOCWLinkContext::AddXHashArray: {} can't contain more than {} element(s)", id, count);
            error = true;
            return 0;
        }

        for (rapidjson::Value& v : arr) {
            if (!v.IsString()) {
                LOG_ERROR("BOCWLinkContext::AddXHashArray: {} can't contain a non string element", id);
                error = true;
                continue;
            }

            const char* str{ v.GetString() };
            AddXHash(str, *array);
            array++;
        }

        return arr.Size();
    }

    void BOCWLinkContext::AddScrString(const char* val, ScrString_t& value, BOCWFFContext& ff) {
        value = (ScrString_t)ff.data.AddScrString(val);
    }

    size_t BOCWLinkContext::AddScrStringArray(
        const char* id, core::config::RapidJsonGeneric& cfg, ScrString_t* array, size_t count, BOCWFFContext& ff
    ) {
        if (cfg.IsNull()) {
            return 0;
        }
        if (!cfg.IsArray()) {
            LOG_ERROR("BOCWLinkContext::AddXHashArray: {} isn't an array", id);
            error = true;
            return 0;
        }
        auto arr{ cfg.GetArray() };
        if (arr.Size() > count) {
            LOG_ERROR("BOCWLinkContext::AddXHashArray: {} can't contain more than {} element(s)", id, count);
            error = true;
            return 0;
        }

        for (rapidjson::Value& v : arr) {
            if (!v.IsString()) {
                LOG_ERROR("BOCWLinkContext::AddXHashArray: {} can't contain a non string element", id);
                error = true;
                continue;
            }

            const char* str{ v.GetString() };
            AddScrString(str, *array, ff);
            array++;
        }

        return arr.Size();
    }

    bool BOCWLinkContext::Assert(bool expr, const char* msg) {
        if (!expr) {
            LOG_ERROR("BOCWLinkContext::Assert: {}", msg);
            error = true;
        }
        return expr;
    }

    utils::ArrayAdder<FFLinkerBOCW, FFLinker> impl{ GetLinkers() };
} // namespace fastfile::linker::cw