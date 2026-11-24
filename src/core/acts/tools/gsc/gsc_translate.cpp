#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <core/memory_allocator.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace {
    using namespace tool::gsc::opcode;
    using namespace tool::gsc;

    struct GscData {
        std::filesystem::path origin;
        std::vector<byte> buffer{};
        std::shared_ptr<GSCOBJHandler> reader{};
        std::unordered_map<uint64_t, std::unordered_set<uint32_t>> opcodesLocs{};
    };

    struct GscExportData {
        NameLocated nl;
        uint32_t start;
        uint32_t size{};
    };

    // Result of a translation between two vms
    struct TranslateContext {
        VmInfo* oldVm{};
        VmInfo* newVm{};
        std::unordered_map<uint64_t, GscData> oldScriptsMap{};
        std::unordered_map<uint64_t, GscData> newScriptsMap{};
        // old opcode -> new values
        std::unordered_map<const OPCodeInfo*, std::set<uint16_t>> opcodeMapping{};
        std::string error{};
    };

    /*
     * Compute the opcode mapping between the compiled scripts of two vms
     * @param ctx translation context, filled with the results
     * @param oldPath directory of the compiled scripts of the old vm
     * @param newPath directory of the compiled scripts of the new vm
     * @param plt platform of the old vm opcodes
     * @param exportConv use the exports to find similar functions
     * @return true if the mapping was computed, false otherwise (ctx.error is set)
     */
    bool TranslateOpcodes(
        TranslateContext& ctx, const std::filesystem::path& oldPath, const std::filesystem::path& newPath,
        opcode::Platform plt, bool exportConv
    ) {
        RegisterOpCodesMap();

        auto LoadVmMap = [](std::unordered_map<uint64_t, GscData>& map,
                            const char* type,
                            const std::filesystem::path& origin,
                            VmInfo*& vm) -> void {
            LOG_INFO("Loading {} path...", type);
            std::vector<std::filesystem::path> paths{};
            utils::GetFileRecurseExt(origin, paths, ".gscc\0.cscc\0.gscbin\0.cscbin\0");

            for (const std::filesystem::path& path : paths) {
                GscData data{ path };
                if (!utils::ReadFile(path, data.buffer) || data.buffer.size() < 8) {
                    LOG_ERROR("Can't read {}", path.string());
                    continue;
                }
                uint64_t magic{ *(uint64_t*)data.buffer.data() };

                if (!vm) {
                    if (!IsValidVmMagic(magic, vm)) {
                        LOG_ERROR("Invalid old vm 0x{:x}", magic);
                        continue;
                    }
                } else if (magic != vm->vmMagic) {
                    throw std::runtime_error(
                        std::format("Multiple magic detected 0x{:x} and 0x{:x}", magic, vm->vmMagic)
                    );
                }

                tool::gsc::vm::GscVm* readerbuilder{ tool::gsc::vm::GetGscReader(magic) };

                if (!readerbuilder) {
                    LOG_ERROR("Missing reader builder");
                    continue;
                }

                data.reader = readerbuilder->NewHandler(data.buffer.data(), data.buffer.size());

                if (!data.reader->IsValidHeader(data.buffer.size())) {
                    continue;
                }

                map[data.reader->GetName()] = std::move(data);
            }
            LOG_INFO("{} scripts loaded", map.size());
        };

        VmInfo*& oldVm{ ctx.oldVm };
        VmInfo*& newVm{ ctx.newVm };
        std::unordered_map<uint64_t, GscData>& oldScriptsMap{ ctx.oldScriptsMap };
        std::unordered_map<uint64_t, GscData>& newScriptsMap{ ctx.newScriptsMap };
        std::unordered_map<const OPCodeInfo*, std::set<uint16_t>>& opcodeMapping{ ctx.opcodeMapping };
        LoadVmMap(oldScriptsMap, "old", oldPath, oldVm);
        LoadVmMap(newScriptsMap, "new", newPath, newVm);

        if (!oldVm || !newVm) {
            ctx.error = "Missing vm";
            return false;
        }

        if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16) != newVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
            ctx.error = "Can't map vm with different op sizes";
            return false;
        }

        bool sameFieldHash = oldVm->HashField("hello") == newVm->HashField("hello");
        bool sameFileHash = oldVm->HashPath("scripts/hello.gsc") == newVm->HashPath("scripts/hello.gsc");

        hashutils::ReadDefaultFile();

        std::unique_ptr<GSCExportReader> oer{ tool::gsc::CreateExportReader(oldVm) };
        std::unique_ptr<GSCExportReader> ner{ tool::gsc::CreateExportReader(newVm) };

        for (auto& [_name, olddata] : oldScriptsMap) {

            uint64_t name;
            // map hash file names
            if (!sameFileHash) {
                const char* mappedName{ hashutils::ExtractPtr(_name) };
                if (!mappedName) {
                    LOG_WARNING("No hash lookup for name {:x}, vm with different hashes", _name);
                    continue;
                }
                name = newVm->HashPath(mappedName);
            } else {
                name = _name;
            }

            auto it{ newScriptsMap.find(name) };
            if (it == newScriptsMap.end()) {
                LOG_TRACE("ignore script {}", hashutils::ExtractTmpScript(name));
                continue;
            }

            GscData& newdata{ it->second };

            LOG_INFO("Translating {}...", hashutils::ExtractTmpScript(name));

            if (!olddata.reader->GetExportsCount() || !newdata.reader->GetExportsCount()) {
                LOG_INFO("No exports");
                continue;
            }

            GscDecompilerGlobalContext gdctx{};

            // disable output file
            gdctx.noDump = true;
            // the decompiler will put everything in this map
            gdctx.opcodesLocs = &newdata.opcodesLocs;
            gdctx.opt.m_platform = plt;

            if (olddata.reader->GetCSEGSize() == newdata.reader->GetCSEGSize()) {
                LOG_INFO("Find similar CSEG");

                int ret{ DecompileGsc(olddata.buffer.data(), olddata.buffer.size(), olddata.origin, gdctx) };

                if (ret) {
                    LOG_ERROR("Decompiler output 0x{:x}", ret);
                    continue;
                }

                LOG_INFO("Adding {} candidates", newdata.opcodesLocs[name].size());
                uint32_t csegStartOld{ olddata.reader->GetCSEGOffset() };
                uint32_t csegStartNew{ newdata.reader->GetCSEGOffset() };
                for (uint32_t loc : newdata.opcodesLocs[name]) {
                    void* opbase{ &olddata.buffer[loc] };
                    void* nopbase{ &newdata.buffer[(size_t)loc - csegStartOld + csegStartNew] };

                    uint16_t op;
                    uint16_t nop;
                    if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                        op = *(uint16_t*)opbase;
                        nop = *(uint16_t*)nopbase;
                    } else {
                        op = (uint16_t)(*(byte*)opbase);
                        nop = (uint16_t)(*(byte*)nopbase);
                    }

                    const OPCodeInfo* oldNfo{ LookupOpCode(oldVm->vmMagic, plt, op) };

                    if (!oldNfo || oldNfo->m_id == OPCODE_Undefined) {
                        continue; // ignore bad ids
                    }

                    opcodeMapping[oldNfo].insert(nop);
                }

                continue;
            }

            if (exportConv) {
                // not working, maybe we can translate checking first same CSEG and then the export codes while ignoring
                // strange results
                std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals> oldExportsMap{};
                std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals> newExportsMap{};
                std::vector<GscExportData> oldExports{};
                std::vector<GscExportData> newExports{};

                auto LoadExports =
                    [](GSCOBJHandler& reader,
                       GSCExportReader& exreader,
                       std::vector<GscExportData>& exports,
                       std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals>& map,
                       VmInfo* vmInfo,
                       bool sameFieldHash) -> void {
                    uint16_t exportsCount{ reader.GetExportsCount() };
                    for (size_t i = 0; i < exportsCount; i++) {
                        exreader.SetHandle(reader.Ptr(reader.GetExportsOffset() + i * reader.GetExportSize()));

                        GscExportData ged{};

                        ged.nl.name = exreader.GetName();
                        ged.nl.name_space = exreader.GetNamespace();
                        ged.start = exreader.GetAddress();

                        // map hash field names
                        if (!sameFieldHash) {
                            const char* mappedName{ hashutils::ExtractPtr(ged.nl.name) };
                            const char* mappedNamespace{ hashutils::ExtractPtr(ged.nl.name_space) };
                            if (!mappedName || !mappedNamespace) {
                                LOG_WARNING(
                                    "No hash lookup for name {}::{}, vm with different hashes",
                                    hashutils::ExtractTmp("namespace", ged.nl.name_space),
                                    hashutils::ExtractTmp("function", ged.nl.name)
                                );
                                ged.nl.name = 0;
                                ged.nl.name_space = 0;
                            } else {
                                ged.nl.name = vmInfo->HashField(mappedName);
                                ged.nl.name_space = vmInfo->HashField(mappedNamespace);
                            }
                        }

                        exports.emplace_back(std::move(ged));
                    }
                    // write end of the cseg to get the size of the last func
                    exports.emplace_back(NameLocated{ 0, 0 }, reader.GetCSEGOffset() + reader.GetCSEGSize(), 0);

                    // sort, shouldn't be required for now, but still good
                    std::sort(exports.begin(), exports.end(), [](GscExportData& a, GscExportData& b) -> bool {
                        return a.start < b.start;
                    });
                    for (size_t i = 0; i < exports.size() - 1; i++) {
                        // compute the size using the delta with the next func
                        exports[i].size = exports[i + 1].start - exports[i].start;

                        if (exports[i].nl.name && exports[i].nl.name_space) {
                            // add the export to the map
                            map[exports[i].nl] = &exports[i];
                        }
                    }
                };

                LoadExports(*olddata.reader, *oer, oldExports, oldExportsMap, oldVm, sameFieldHash);
                LoadExports(*newdata.reader, *ner, newExports, newExportsMap, newVm, true);

                bool isDecompiled{};
                bool anySimilar{};
                for (auto& [nl, oged] : oldExportsMap) {
                    auto ite{ newExportsMap.find(nl) };

                    if (ite == newExportsMap.end()) {
                        LOG_TRACE(
                            "Can't find {}::{} in new dump",
                            hashutils::ExtractTmp("namespace", oged->nl.name_space),
                            hashutils::ExtractTmp("function", oged->nl.name)
                        );
                        continue;
                    }

                    GscExportData* nged{ ite->second };

                    if (oged->size != nged->size) {
                        LOG_TRACE(
                            "Not the same size for {}::{} in new dump",
                            hashutils::ExtractTmp("namespace", oged->nl.name_space),
                            hashutils::ExtractTmp("function", oged->nl.name)
                        );
                        continue;
                    }

                    if (!isDecompiled) {
                        // decompile the script
                        int ret{ DecompileGsc(olddata.buffer.data(), olddata.buffer.size(), olddata.origin, gdctx) };

                        if (ret) {
                            LOG_ERROR("Decompiler output 0x{:x}", ret);
                            break;
                        }
                        isDecompiled = true;
                    }
                    anySimilar = true;

                    for (uint32_t loc : newdata.opcodesLocs[name]) {
                        if (loc < oged->start || loc >= oged->start + oged->size) {
                            continue; // not inside the current export
                        }

                        void* opbase{ &olddata.buffer[loc] };
                        void* nopbase{ &newdata.buffer[(size_t)loc - oged->start + nged->start] };

                        uint16_t op;
                        uint16_t nop;
                        if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                            op = *(uint16_t*)opbase;
                            nop = *(uint16_t*)nopbase;
                        } else {
                            op = (uint16_t)(*(byte*)opbase);
                            nop = (uint16_t)(*(byte*)nopbase);
                        }

                        const OPCodeInfo* oldNfo{ LookupOpCode(oldVm->vmMagic, plt, op) };

                        if (!oldNfo || oldNfo->m_id == OPCODE_Undefined) {
                            continue; // ignore bad ids
                        }

                        opcodeMapping[oldNfo].insert(nop);
                    }
                }

                if (anySimilar)
                    continue;
            }

            // compute exports list
            LOG_WARNING("No similar data to exploit for {}", hashutils::ExtractTmpScript(name));
        }

        return true;
    }

    int gsct(int argc, const char* argv[]) {
        cli::options::CliOptions opts{};

        struct {
            bool help{};
            bool sameVm{};
            const char* testDecompDir{};
            const char* pltName{};
            bool exportConv{};
            opcode::Platform plt{ PLATFORM_PC };
        } opt;

        opts.addOption(&opt.help, "Show help", "--help", "", "-h")
            .addOption(&opt.sameVm, "Use same vm for new", "--sameVm", "", "-s")
            .addOption(&opt.exportConv, "Use exports", "--export", "", "-e")
            .addOption(&opt.testDecompDir, "Start decompiler with new opcodes", "--decomp", "", "-d")
            .addOption(&opt.pltName, "Input platform name", "--plt", " [plt]", "-t");

        if (!opts.ComputeOptions(2, argc, argv) || opt.help || opts.ParamsCount() < 3) {
            LOG_INFO("{} {} [input] [out] [opcodes out]", argv[0], argv[1]);
            opts.PrintOptions();
            return opts.ParamsCount() < 3 ? tool::BAD_USAGE : tool::OK;
        }

        if (opt.pltName) {
            opt.plt = opcode::PlatformOf(opt.pltName);
            if (!opt.plt) {
                LOG_ERROR("Invalid platform name '{}'", opt.pltName);
                return tool::BASIC_ERROR;
            }
            LOG_INFO("use plt: {}", opcode::PlatformName(opt.plt));
        }

        TranslateContext ctx{};
        if (!TranslateOpcodes(ctx, opts[0], opts[1], opt.plt, opt.exportConv)) {
            LOG_ERROR("{}", ctx.error);
            return tool::BASIC_ERROR;
        }

        VmInfo* oldVm{ ctx.oldVm };
        VmInfo* newVm{ ctx.newVm };
        std::unordered_map<uint64_t, GscData>& newScriptsMap{ ctx.newScriptsMap };
        std::unordered_map<const OPCodeInfo*, std::set<uint16_t>>& opcodeMapping{ ctx.opcodeMapping };

        {
            LOG_INFO("Dump into {}", opts[2]);
            utils::OutFileCE os{ opts[2] };

            if (!os) {
                LOG_ERROR("Can't open {}", opts[2]);
                return tool::BASIC_ERROR;
            }

            for (auto& [oldNfo, newVals] : opcodeMapping) {
                os << oldNfo->m_name << " ->";

                if (!oldVm->HasFlag(VmFlags::VMF_OPCODE_U16) && newVals.size() != 1) {
                    LOG_WARNING("Invalid mapping for opcode {}, {} results", oldNfo->m_name, newVals.size());
                }

                for (uint16_t v : newVals) {
                    os << " 0x" << std::hex << (int)v;
                }

                os << "\n";
            }
        }

        if (opt.testDecompDir) {
            // register opcodes
            newVm->AddPlatform(PLATFORM_ACTS_TEST);
            newVm->ClearPlatformOpCode(PLATFORM_ACTS_TEST);

            LOG_INFO("Registering new opcodes...");
            for (auto& [oldNfo, newVals] : opcodeMapping) {
                for (uint16_t v : newVals) {
                    newVm->RegisterOpCode(PLATFORM_ACTS_TEST, oldNfo->m_id, v);
                }
            }

            LOG_INFO("Decompiling...");
            GscDecompilerGlobalContext outdctx{};

            outdctx.opt.m_platform = PLATFORM_ACTS_TEST;
            outdctx.opt.m_outputDir = opt.testDecompDir;
            outdctx.opt.m_dcomp = true;
            outdctx.opt.m_splitByVm = true;

            int r{ tool::OK };
            for (auto& [k, v] : newScriptsMap) {

                int ores{ tool::gsc::DecompileGsc(v.buffer.data(), v.buffer.size(), v.origin, outdctx) };

                if (ores) {
                    LOG_ERROR("Error when decompiling {}", hashutils::ExtractTmpScript(k));
                    r = tool::BASIC_ERROR;
                }
            }

            LOG_INFO("Decompiled test dump into {}", opt.testDecompDir);
            return r;
        }

        return tool::OK;
    }

    // Get a string argument, nullptr if missing
    const char* GetStringArg(const rapidjson::Value& args, const char* key) {
        if (!args.IsObject() || !args.HasMember(key) || !args[key].IsString()) {
            return nullptr;
        }
        return args[key].GetString();
    }

    std::string translate_gsc_opcodes(const rapidjson::Value& args, bool& isError) {
        const char* oldPath{ GetStringArg(args, "old") };
        const char* newPath{ GetStringArg(args, "new") };
        if (!oldPath || !newPath) {
            isError = true;
            return "Missing 'old' or 'new' argument";
        }

        opcode::Platform plt{ PLATFORM_PC };
        const char* pltName{ GetStringArg(args, "platform") };
        if (pltName) {
            plt = opcode::PlatformOf(pltName);
            if (!plt) {
                isError = true;
                return std::format("Invalid platform name '{}'", pltName);
            }
        }
        bool exportConv{ args.IsObject() && args.HasMember("export") && args["export"].IsBool() &&
                         args["export"].GetBool() };

        TranslateContext ctx{};
        try {
            if (!TranslateOpcodes(ctx, oldPath, newPath, plt, exportConv)) {
                isError = true;
                return ctx.error;
            }
        } catch (std::runtime_error& e) {
            isError = true;
            return e.what();
        }

        // sort the results by opcode name
        std::map<std::string, const std::set<uint16_t>*> sorted{};
        for (const std::pair<const OPCodeInfo* const, std::set<uint16_t>>& mapping : ctx.opcodeMapping) {
            // same names as the other opcode tools (list_gsc_opcode_names, set_gsc_vm_opcodes...)
            const char* name{ opcode::OpCodeName(mapping.first->m_id) };
            sorted[name ? name : "<error>"] = &mapping.second;
        }

        bool byteVm{ !ctx.oldVm->HasFlag(VmFlags::VMF_OPCODE_U16) };

        // conflicts: a byte vm uses one value per opcode, and a value can't be used by multiple opcodes
        std::map<uint16_t, std::set<std::string>> valueOwners{};
        for (const std::pair<const std::string, const std::set<uint16_t>*>& mapping : sorted) {
            for (uint16_t value : *mapping.second) {
                valueOwners[value].insert(mapping.first);
            }
        }
        std::set<std::string> conflictOpcodes{};
        for (const std::pair<const std::string, const std::set<uint16_t>*>& mapping : sorted) {
            if (byteVm && mapping.second->size() != 1) {
                conflictOpcodes.insert(mapping.first);
            }
            for (uint16_t value : *mapping.second) {
                if (valueOwners[value].size() > 1) {
                    conflictOpcodes.insert(mapping.first);
                }
            }
        }

        // unmapped: the opcodes defined by the old vm for the platform without any new value
        std::set<std::string> mappedIds{};
        for (const std::pair<const std::string, const std::set<uint16_t>*>& mapping : sorted) {
            mappedIds.insert(mapping.first);
        }
        std::set<std::string> unmapped{};
        std::unordered_map<Platform, std::unordered_map<OPCode, std::unordered_set<uint16_t>>>::const_iterator pltIt{
            ctx.oldVm->opcodemappltlookup.find(ctx.oldVm->RemapSamePlatform(plt))
        };
        if (pltIt != ctx.oldVm->opcodemappltlookup.end()) {
            for (const std::pair<const OPCode, std::unordered_set<uint16_t>>& op : pltIt->second) {
                const char* name{ op.first == OPCODE_Undefined ? nullptr : opcode::OpCodeName(op.first) };
                if (name && !mappedIds.contains(name)) {
                    unmapped.insert(name);
                }
            }
        }

        // apply: register the mappings without conflicts into the new vm for a platform
        size_t applied{};
        const char* applyPltName{ GetStringArg(args, "apply") };
        if (applyPltName) {
            opcode::Platform applyPlt{ opcode::PlatformOf(applyPltName) };
            if (!applyPlt) {
                isError = true;
                return std::format("Invalid platform name '{}' for 'apply'", applyPltName);
            }
            ctx.newVm->AddPlatform(applyPlt);
            for (const std::pair<const std::string, const std::set<uint16_t>*>& mapping : sorted) {
                OPCode op{ opcode::OpCodeFromName(mapping.first.c_str()) };
                if (conflictOpcodes.contains(mapping.first) || op == OPCODE_Undefined) {
                    continue;
                }
                for (uint16_t value : *mapping.second) {
                    if (value < ctx.newVm->maxOpCode) {
                        ctx.newVm->RegisterOpCode(applyPlt, op, value);
                        applied++;
                    }
                }
            }
        }

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        writer.StartObject();
        writer.Key("oldVm");
        writer.String(ctx.oldVm->internalName);
        writer.Key("newVm");
        writer.String(ctx.newVm->internalName);
        writer.Key("oldScripts");
        writer.Uint64(ctx.oldScriptsMap.size());
        writer.Key("newScripts");
        writer.Uint64(ctx.newScriptsMap.size());
        writer.Key("mappings");
        writer.StartArray();
        for (const std::pair<const std::string, const std::set<uint16_t>*>& mapping : sorted) {
            writer.StartObject();
            writer.Key("opcode");
            writer.String(mapping.first.c_str());
            writer.Key("values");
            writer.StartArray();
            for (uint16_t value : *mapping.second) {
                writer.Uint(value);
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("conflicts");
        writer.StartArray();
        for (const std::string& name : conflictOpcodes) {
            writer.StartObject();
            writer.Key("opcode");
            writer.String(name.c_str());
            writer.Key("values");
            writer.StartArray();
            for (uint16_t value : *sorted[name]) {
                writer.Uint(value);
            }
            writer.EndArray();
            // the other opcodes using the same values
            writer.Key("sharedWith");
            writer.StartArray();
            for (uint16_t value : *sorted[name]) {
                for (const std::string& owner : valueOwners[value]) {
                    if (owner != name) {
                        writer.String(std::format("{} (0x{:x})", owner, value).c_str());
                    }
                }
            }
            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("unmapped");
        writer.StartArray();
        for (const std::string& name : unmapped) {
            writer.String(name.c_str());
        }
        writer.EndArray();

        if (applyPltName) {
            writer.Key("applied");
            writer.Uint64(applied);
        }
        writer.EndObject();
        return std::string{ sb.GetString(), sb.GetSize() };
    }

    ADD_MCP_TOOL(
        translate_gsc_opcodes, "gsc",
        "Translate the opcodes of a vm to another by comparing the compiled scripts (.gscc/.cscc/.gscbin/.cscbin) of "
        "two directories (the same scripts compiled for the old and the new vm, with similar code segments). The vm "
        "are read from the magic of the files. 'platform' is the platform of the old vm opcodes (pc by default), "
        "'export' also compares the functions of the scripts with a different code segment (experimental). Returns "
        "{ oldVm, newVm, oldScripts, newScripts, mappings: [ { opcode (old opcode name), values[] (new values) } ], "
        "conflicts (opcodes with several values in a byte vm, or sharing a value with another opcode), unmapped "
        "(opcodes of the old vm without any new value), applied? }. 'apply' is a platform: the mappings without "
        "conflicts are registered into the new vm for this platform (like set_gsc_vm_opcodes), so they don't need "
        "to be typed again.",
        R"({"type":"object","properties":{"old":{"type":"string","description":"directory of the compiled scripts of the old vm"},"new":{"type":"string","description":"directory of the compiled scripts of the new vm"},"platform":{"type":"string"},"export":{"type":"boolean"},"apply":{"type":"string","description":"platform of the new vm to register the mappings without conflicts into"}},"required":["old","new"]})",
        translate_gsc_opcodes
    );

    ADD_TOOL(gsct, "gsc", "[old] [new] [output]", "Translate gsc opcodes from one VM to another", gsct);
} // namespace