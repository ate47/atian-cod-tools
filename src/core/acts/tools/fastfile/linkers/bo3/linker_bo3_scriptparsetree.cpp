#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo3.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_compiler.hpp>

namespace {
    using namespace fastfile::linker::bo3;
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

        void Compute(BO3LinkContext& ctx, const char* id, BO3FFContext& ff) override {
            bool cfguseModToolOpcodes{ ctx.linkCtx.zone.GetConfigBool("gsc.opModTool", true) };
            bool cfgGenDBG{ ctx.linkCtx.zone.GetConfigBool("gsc.gendbg", false) };
            bool cfgDev{ ctx.linkCtx.zone.GetConfigBool("gsc.dev", false) };
            bool cfgNoDevCallInline{ ctx.linkCtx.zone.GetConfigBool("gsc.noDevCallInline", false) };
            bool genDevBlockAsComment{ ctx.linkCtx.zone.GetConfigBool("gsc.devBlockAsComment", false) };
            const char* preprocDefs{ ctx.linkCtx.zone.GetConfig("gsc.defs", "") };

            const char* scriptparsetreeNameCfg{ id };
            std::filesystem::path scriptName{ scriptparsetreeNameCfg };
            std::filesystem::path path{ ctx.linkCtx.input / scriptparsetreeNameCfg };
            LOG_TRACE("Processing {} ({})", scriptName.string(), path.string());

            std::vector<byte> buffer{};

            bool isCsc;
            // compile file
            tool::gsc::compiler::CompilerConfig cfg{};
            std::string snp{ scriptName.string() };
            cfg.name = hashutils::CleanPath(snp.data());
            cfg.platform = GetGSCPlatform(ctx.linkCtx.opt.platform);
            cfg.vm = tool::gsc::opcode::VMI_T7_1C; // read cfg?
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
                ctx.linkCtx.RegisterHash(hash::HashT7(str.data()), str);
            }

            if (!bgcacheCompiled.empty()) {
                LOG_ERROR("Can't compile bg cache header for {} (not implemented)", path.string());
                ctx.error = true;
                return;
            }

            LOG_INFO("Compiled {} ({})", path.string(), cfg.name);

            if (buffer.size() < sizeof(tool::gsc::T8GSCOBJ)) {
                LOG_ERROR("Can't read compiled gsc header for {}", path.string());
                ctx.error = true;
                return;
            }
            tool::gsc::T8GSCOBJ& obj{ *(tool::gsc::T8GSCOBJ*)buffer.data() };

            struct ScriptParseTree {
                XString name;
                uint32_t len;
                void* buffer;
            };
            static_assert(sizeof(ScriptParseTree) == 0x18);

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            ScriptParseTree& spt{ ff.data.AllocStreamRef<ScriptParseTree>() };
            spt.name = (XString)fastfile::linker::memory::POINTER_NEXT;
            spt.buffer = (void*)fastfile::linker::memory::POINTER_NEXT;
            spt.len = (uint32_t)buffer.size();

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

            ff.data.WriteStream(snp);
            ff.data.Align(0x20);
            void* gscc{ ff.data.AllocStream(buffer.size() + 1)->As<void>() };
            std::memcpy(gscc, buffer.data(), buffer.size());
            ff.data.PopStream();

            ff.data.PopStream();
            LOG_INFO("Added asset scriptparsetree {} (hash_{:x})", path.string(), obj.name);

            // if (preprocOutput.size() || dbgdata.size()) {} // <- write that somewhere because bo3 doesn't have dbg
            // header
        }
    };

    utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(),
                                                                      XAssetType::T7_ASSET_TYPE_SCRIPTPARSETREE };
} // namespace