#include <includes.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_compiler.hpp>

namespace {
    using namespace fastfile::linker::cw;
    class XAssetLinkerImpl : public XAssetLinker {
      public:
        using XAssetLinker::XAssetLinker;

        static tool::gsc::opcode::Platform GetGSCPlatform(fastfile::FastFilePlatform plt) {
            switch (plt) {
            case fastfile::XFILE_PC:
                return tool::gsc::opcode::PLATFORM_PC;
            case fastfile::XFILE_PLAYSTATION:
                return tool::gsc::opcode::PLATFORM_PLAYSTATION;
            case fastfile::XFILE_XBOX:
                return tool::gsc::opcode::PLATFORM_XBOX;
            case fastfile::XFILE_DEV:
                return tool::gsc::opcode::PLATFORM_PC_ALPHA;
            default:
                throw std::runtime_error("Invalid pltform for this ff");
            }
        }

        static void AddGscDBGHeader(
            BOCWLinkContext& ctx, std::vector<byte>& buffer, std::string& preproc, std::vector<byte>& dbgbuffer,
            const std::filesystem::path& path
        ) {
            BOCWFFContext& dbgCtx{ ctx.GetFFContext("dbg_") };

            if (buffer.size() < sizeof(tool::gsc::T9GSCOBJ)) {
                LOG_ERROR("Can't read compiled gsc header for {}", path.string());
                return;
            }
            tool::gsc::T9GSCOBJ& obj{ *(tool::gsc::T9GSCOBJ*)buffer.data() };
            struct ScriptParseTreeDBG {
                CWXHash name;
                int32_t gdbLen;
                int32_t srcLen;
                void* gdb;
                const char* src;
            };
            static_assert(sizeof(ScriptParseTreeDBG) == 0x20);

            dbgCtx.data.PushStream(XFILE_BLOCK_TEMP);
            dbgCtx.data.AddAsset(XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG, dbgCtx.data.Align(8));

            ScriptParseTreeDBG& spt{ dbgCtx.data.AllocStreamRef<ScriptParseTreeDBG>() };
            spt.name = obj.name;

            dbgCtx.data.PushStream(XFILE_BLOCK_VIRTUAL);
            if (dbgbuffer.size()) {
                spt.gdb = (void*)fastfile::linker::memory::POINTER_NEXT;
                spt.gdbLen = (int32_t)dbgbuffer.size();
                dbgCtx.data.Align(0x20);
                void* ptr{ dbgCtx.data.AllocStream(dbgbuffer.size() + 1)->As<void>() };
                std::memcpy(ptr, dbgbuffer.data(), dbgbuffer.size());
            }

            if (preproc.size()) {
                spt.src = (const char*)fastfile::linker::memory::POINTER_NEXT;
                spt.srcLen = (int32_t)preproc.size();

                dbgCtx.data.Align<char>();
                void* ptr{ dbgCtx.data.AllocStream(preproc.size() + 1)->As<void>() };
                std::memcpy(ptr, preproc.data(), preproc.size());
            }
            dbgCtx.data.PopStream();

            dbgCtx.data.PopStream();
            LOG_INFO("Added asset scriptparsetreedbg {} (hash_{:x})", path.string(), obj.name);
        }

        static void
        AddUsingHeader(BOCWLinkContext& ctx, BOCWFFContext& ff, CWXHash name, bool client, XAssetType type) {
            struct ScriptUsing {
                CWXHash name;
                CWXHash serverScript;
                CWXHash clientScript;
            };
            static_assert(sizeof(ScriptUsing) == 0x18);

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            ff.data.AddAsset(type, ff.data.Align(8));

            ScriptUsing& spt{ ff.data.AllocStreamRef<ScriptUsing>() };
            spt.name = name;
            if (client) {
                spt.clientScript = name;
            } else {
                spt.serverScript = name;
            }

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
            ff.data.PopStream();

            ff.data.PopStream();
        }

        void Compute(BOCWLinkContext& ctx, const char* id, BOCWFFContext& ff) override {
            bool cfguseModToolOpcodes{ ctx.linkCtx.zone.GetConfigBool("gsc.opModTool", true) };
            bool cfgGenDBG{ ctx.linkCtx.zone.GetConfigBool("gsc.gendbg", false) };
            bool cfgDev{ ctx.linkCtx.zone.GetConfigBool("gsc.dev", false) };
            bool cfgNoDevCallInline{ ctx.linkCtx.zone.GetConfigBool("gsc.noDevCallInline", false) };
            bool genDevBlockAsComment{ ctx.linkCtx.zone.GetConfigBool("gsc.devBlockAsComment", false) };
            const char* preprocDefs{ ctx.linkCtx.zone.GetConfig("gsc.defs", "") };

            const char* scriptparsetreeNameCfg{ id };
            bool forced{ *scriptparsetreeNameCfg == '!' };
            if (forced)
                scriptparsetreeNameCfg++;
            std::filesystem::path scriptName{ scriptparsetreeNameCfg };
            std::filesystem::path path{ ctx.linkCtx.input / scriptparsetreeNameCfg };
            LOG_TRACE("Processing {} ({})", scriptName.string(), path.string());

            std::vector<byte> buffer{};

            bool isCsc;
            // compile file
            tool::gsc::compiler::CompilerConfig cfg{};
            std::string snp{ scriptName.string() };
            cfg.name = hashutils::CleanPath(snp.data());
            ctx.HashPathName(scriptName);
            cfg.platform = GetGSCPlatform(ctx.linkCtx.opt.platform);
            cfg.vm = tool::gsc::opcode::VMI_T9_38; // read cfg?
            cfg.detourType = tool::gsc::compiler::DETOUR_ACTS;
            cfg.computeDevOption = cfgDev;
            std::string preprocOutput{};
            std::vector<byte> dbgdata{};
            if (cfgGenDBG) {
                cfg.preprocOutput = &preprocOutput;
            }
            cfg.dbgoutput = &dbgdata;
            cfg.noDevCallInline = cfgNoDevCallInline;
            cfg.processorOpt.devBlockAsComment = genDevBlockAsComment;
            cfg.processorOpt.cwd = ctx.linkCtx.input;
            cfg.clientScript = scriptName.extension() == ".csc";
            cfg.baseDir = ctx.linkCtx.input;
            isCsc = cfg.clientScript;
            std::unordered_map<std::string, std::vector<std::string>> bgcacheCompiled{};
            cfg.precache = &bgcacheCompiled;
            cfg.useModToolOpCodes = cfguseModToolOpcodes;
            cfg.processorOpt.AddDefine(std::format("_FF_GEN_{}", ff.ffname));
            cfg.processorOpt.AddDefineListConfig(preprocDefs);

            std::unordered_set<std::string> hashes{};
            cfg.hashes = &hashes;

            try {
                tool::gsc::compiler::CompileGsc(path, buffer, cfg);
            } catch (std::runtime_error& re) {
                LOG_ERROR("Can't compile {}: {}", path.string(), re.what());
                ctx.error = true;
                return;
            }

            for (const std::string& str : hashes) {
                char* v{ hashutils::CleanPath(utils::CloneString(str.data())) };
                ctx.linkCtx.RegisterHash(hash::Hash64(str.data()), str);
                ctx.linkCtx.RegisterHash(hash::HashT89Scr(str.data()), str);
            }

            for (auto& [bgtype, entries] : bgcacheCompiled) {
                BGCacheTypes type{ ::cw::BgCacheId(bgtype.c_str()) };
                if (type == BGCacheTypes::BG_CACHE_TYPE_INVALID) {
                    LOG_ERROR("Can't compile {}: Invalid precache asset type {}", path.string(), bgtype);
                    ctx.error = true;
                    continue;
                }

                std::unordered_set<uint64_t>& bg{ ff.bgcache[type] };

                for (const std::string& entry : entries) {
                    uint64_t hash{ ctx.HashXHash(entry.c_str()) };
                    LOG_DEBUG("add {}::{} ({:x}) to bgcache", ::cw::BgCacheName(type), entry, hash);
                    bg.insert(hash);
                }
            }

            LOG_INFO("Compiled {} ({})", path.string(), cfg.name);

            if (buffer.size() < sizeof(tool::gsc::T9GSCOBJ)) {
                LOG_ERROR("Can't read compiled gsc header for {}", path.string());
                return;
            }
            tool::gsc::T9GSCOBJ& obj{ *(tool::gsc::T9GSCOBJ*)buffer.data() };

            struct ScriptParseTree {
                CWXHash name;
                void* buffer;
                uint32_t len;
            };
            static_assert(sizeof(ScriptParseTree) == 0x18);

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            ScriptParseTree& spt{ ff.data.AllocStreamRef<ScriptParseTree>() };
            spt.name = obj.name;
            spt.buffer = (void*)fastfile::linker::memory::POINTER_NEXT;
            spt.len = (uint32_t)buffer.size();

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
            ff.data.Align(0x20);
            void* gscc{ ff.data.AllocStream(buffer.size() + 1)->As<void>() };
            std::memcpy(gscc, buffer.data(), buffer.size());
            ff.data.PopStream();

            ff.data.PopStream();
            LOG_INFO("Added asset scriptparsetree {} (hash_{:x})", path.string(), obj.name);

            if (preprocOutput.size() || dbgdata.size()) {
                AddGscDBGHeader(ctx, buffer, preprocOutput, dbgdata, path);
            }

            if (forced) {
                tool::gsc::T9GSCOBJ& obj{ *(tool::gsc::T9GSCOBJ*)buffer.data() };

                std::string_view sv{ ctx.mainFF.ffname };

                XAssetType usingMode;
                if (sv.starts_with("cp_")) {
                    usingMode = XAssetType::ASSET_TYPE_SCRIPT_USING_CP;
                } else if (sv.starts_with("mp_")) {
                    usingMode = XAssetType::ASSET_TYPE_SCRIPT_USING_MP;
                } else if (sv.starts_with("wz_")) {
                    usingMode = XAssetType::ASSET_TYPE_SCRIPT_USING_WZ;
                } else if (sv.starts_with("zm_")) {
                    usingMode = XAssetType::ASSET_TYPE_SCRIPT_USING_ZM;
                } else {
                    usingMode = XAssetType::ASSET_TYPE_SCRIPT_USING;
                }

                // add forced script
                AddUsingHeader(ctx, ff, obj.name, isCsc, usingMode);
            }
        }
    };

    struct ScriptUsing {
        uint64_t name;
        uint64_t serverScript;
        uint64_t clientScript;
    };

    class XAssetLinkerUsingImpl : public XAssetLinker {
        XAssetType type;

      public:
        XAssetLinkerUsingImpl(XAssetType type) : type(type) {};

        void Compute(BOCWLinkContext& ctx, const char* id, BOCWFFContext& ff) override {
            std::filesystem::path rfpath{ id };
            std::filesystem::path path{ ctx.linkCtx.input / rfpath };
            std::string buffer{};
            if (!utils::ReadFile(path, buffer)) {
                LOG_ERROR("Can't read rawfile {}", path.string());
                ctx.error = true;
                return;
            }

            struct RawFile {
                CWXHash name;
                int32_t len;
                const char* buffer;
            };
            static_assert(sizeof(RawFile) == 0x18);

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            RawFile& rf{ ff.data.AllocStreamRef<RawFile>() };

            rf.name = ctx.HashPathName(rfpath);
            rf.buffer = (const char*)fastfile::linker::memory::POINTER_NEXT;
            rf.len = (uint32_t)buffer.size();

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
            ff.data.Align(0x10);
            ff.data.WriteStream(buffer.data(), buffer.length() + 1); // add \0
            ff.data.PopStream();

            ff.data.PopStream();

            LOG_INFO("Added asset rawfile {} (hash_{:x})", rfpath.string(), rf.name);
        }
    };

    utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(),
                                                                      XAssetType::ASSET_TYPE_SCRIPTPARSETREE };
} // namespace