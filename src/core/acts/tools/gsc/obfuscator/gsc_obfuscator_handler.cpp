#include <includes.hpp>
#include <span>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/obfuscator/gsc_obfuscator_handler.hpp>
#include <utils/data_utils.hpp>
#include <core/bytebuffer.hpp>
// crc_cpp stuff
#undef small
#include <crc_cpp.h>

namespace tool::gsc::obfuscator::handler {
    namespace {
        uint32_t ComputeCRC32(void* data, size_t len) {
            byte* b{ (byte*)data };

            crc_cpp::crc32 crc{};

            for (size_t i = 0; i < len; i++) {
                crc.update(b[i]);
            }
            return crc.final();
        }

        size_t FindExportSizeByCrc(byte* bytecode, void* end, uint32_t crc) {
            for (size_t len = 1; bytecode + len <= end; len++) {
                if (ComputeCRC32(bytecode, len) == crc) {
                    return len;
                }
            }
            return 0;
        }
    } // namespace

    GscObfuscator::GscObfuscator(tool::gsc::obfuscator::options::GscObfOptions& opt, byte* script, size_t scriptLen)
        : opt(opt), script(script), scriptLen(scriptLen), scriptEnd(&script[scriptLen]) {

        if (scriptLen < sizeof(uint64_t)) {
            throw std::runtime_error("Invalid size");
        }

        uint64_t magic{ *(uint64_t*)script };

        tool::gsc::vm::GscVm* vm{ tool::gsc::vm::GetGscReader(magic) };

        if (!vm || !opcode::IsValidVmMagic(magic, vmInfo)) {
            throw std::runtime_error(std::format("Unknown GSC vm magic 0x{:x}", magic));
        }

        scriptFile = vm->NewHandler(script, scriptLen);

        if (!scriptFile->IsValidHeader(scriptLen)) {
            throw std::runtime_error(std::format("Invalid gsc header 0x{:x}", magic));
        }

        exportReader = CreateExportReader(vmInfo);
        importReader = CreateImportReader(vmInfo);

        u16OpCodes = vmInfo->HasFlag(opcode::VmFlags::VMF_OPCODE_U16);
        alignedCode = vmInfo->HasFlag(opcode::VmFlags::VMF_ALIGN);

        DecompileScript();
    }

    void GscObfuscator::DecompileScript() {
        // read basic tables
        size_t exportsCount{ scriptFile->GetExportsCount() };
        if (exportsCount) {
            size_t exportSize{ scriptFile->GetExportSize() };
            byte* exports{ scriptFile->Ptr(scriptFile->GetExportsOffset()) };
            ValidateInScript(exports + exportsCount * exportSize, "export table");

            for (size_t i = 0; i < exportsCount; i++) {
                void* handle{ &exports[i * exportSize] };
                exportReader->SetHandle(handle);

                byte* bc{ scriptFile->Ptr(exportReader->GetAddress()) };
                // the minimum is createparam + end (4 bytes)
                ValidateInScript(Aligned<uint16_t>(bc) + 4, "invalid exports address");
                size_t len{ FindExportSizeByCrc(bc, scriptEnd, exportReader->GetChecksum()) };

                byte flags{ scriptFile->RemapFlagsExport(exportReader->GetFlags()) };

                GscExportData* expData{
                    gdctx.alloc.New<GscExportData>(GscExportData{ .ref = handle, .bc = bc, .flags = flags, .len = len })
                };

                exportsMap[NameLocated{
                               .name_space = exportReader->GetNamespace(),
                               .name = exportReader->GetName(),
                           }]
                    .emplace_back(expData);
                this->exports.emplace_back(expData);
            }
        }

        size_t importsCount{ scriptFile->GetImportsCount() };
        if (importsCount) {
            size_t importsSize{ scriptFile->GetImportSize() };
            byte* imports{ scriptFile->Ptr(scriptFile->GetImportsOffset()) };

            for (size_t i = 0; i < importsCount; i++) {
                void* handle{ imports };
                importReader->SetHandle(handle);
                ValidateInScript(imports + importsSize, "imports table");
                imports += importsSize + sizeof(uint32_t) * importReader->GetNumAddresses();
                byte flags{ scriptFile->RemapFlagsImport(importReader->GetFlags()) };

                uint32_t* addrs{ importReader->GetAddresses() };
                ValidateInScript(addrs + importReader->GetNumAddresses(), "imports table addresses");

                GscImportData* impData{ gdctx.alloc.New<GscImportData>(GscImportData{ .ref = handle,
                                                                                      .flags = flags }) };

                importsMap[NameLocated{
                               .name_space = importReader->GetNamespace(),
                               .name = importReader->GetName(),
                           }]
                    .emplace_back(impData);
                this->imports.emplace_back(impData);
            }
        }

        // decompile opcodes

        // disable output file
        if (opt.dumpDecompiledScripts) {
            gdctx.noDump = false;
            gdctx.opt.m_dasm = true;
            gdctx.opt.m_dcomp = true;
            gdctx.opt.m_header = true;
            gdctx.opt.m_imports = true;
            gdctx.opt.m_strings = true;
            gdctx.opt.m_gvars = true;
            gdctx.opt.m_func_rloc = true;
            gdctx.opt.m_func_floc = true;
            gdctx.opt.m_noUsingsSort = true;
        } else {
            gdctx.noDump = true;
        }

        // the decompiler will put everything in this map
        gdctx.opcodesLocs = &opcodesLocsMap;
        std::filesystem::path outputPath{ std::filesystem::path{ opt.output } / "decomp" };
        std::string outputDir{ outputPath.string() };

        const char* name{ hashutils::ExtractPtr(scriptFile->GetName()) };
        std::filesystem::path outputFile{ outputPath /
                                          (name ? name : utils::va("script_%llx.gsc", scriptFile->GetName())) };
        gdctx.opt.m_outputDir = outputDir.data();
        gdctx.opt.m_noLogs = true;
        gdctx.opt.m_generateDbgData = true;
        gdctx.opt.m_platform = opt.plt;

        LOG_DEBUG("decomp in {}", outputDir);

        int ret{ DecompileGsc(script, scriptLen, outputFile, gdctx) };

        if (ret) {
            throw std::runtime_error(std::format("Failed to decompile script 0x{:x}", ret));
        }

        auto it{ opcodesLocsMap.begin() };
        if (it != opcodesLocsMap.end()) {
            opcodesLocs = &it->second;

            if (vmInfo->modToolFlag) {
                // has a modtool flag, we can search to fill the value
                auto opit{ opcodesLocs->begin() };
                if (opit != opcodesLocs->end()) {
                    byte* firstOpCode{ scriptFile->Ptr(*opit) };
                    modToolOpCodes = (GetAndPassOpCode(firstOpCode) & vmInfo->modToolFlag) != 0;
                    LOG_DEBUG("use modtool opcodes: {}", modToolOpCodes ? "true" : "false");
                }
            }
        } else if (exportsCount) {
            LOG_WARNING("Can't find any opcodes, but the script contains {} function(s)", exportsCount);
        }
        auto itdbg{ gdctx.gdbData.begin() };
        if (itdbg != gdctx.gdbData.end()) {
            gdbData = itdbg->second;
        } else if (exportsCount) {
            LOG_WARNING("Can't find any debug data, but the script contains {} function(s)", exportsCount);
        }
    }

    void GscObfuscator::PrintHeader() {
        // LOG_INFO("source_crc 0x{:x}", (uint32_t)header.source_crc);
        // LOG_INFO("include_offset 0x{:x}", (uint32_t)header.include_offset);
        // LOG_INFO("animtree_offset 0x{:x}", (uint32_t)header.animtree_offset);
        // LOG_INFO("cseg_offset 0x{:x}", (uint32_t)header.cseg_offset);
        // LOG_INFO("string_offset 0x{:x}", (uint32_t)header.string_offset);
        // LOG_INFO("devblock_string_offset 0x{:x}", (uint32_t)header.devblock_string_offset);
        // LOG_INFO("export_offset 0x{:x}", (uint32_t)header.export_offset);
        // LOG_INFO("import_offset 0x{:x}", (uint32_t)header.import_offset);
        // LOG_INFO("fixup_offsets 0x{:x}", (uint32_t)header.fixup_offsets);
        // LOG_INFO("profile_offset 0x{:x}", (uint32_t)header.profile_offset);
        // LOG_INFO("cseg_size 0x{:x}", (uint32_t)header.cseg_size);
        // LOG_INFO("name_offset 0x{:x}", (uint32_t)header.name_offset);
        // LOG_INFO("string_count 0x{:x}", (uint32_t)header.string_count);
        // LOG_INFO("export_count 0x{:x}", (uint32_t)header.export_count);
        // LOG_INFO("import_count 0x{:x}", (uint32_t)header.import_count);
        // LOG_INFO("fixup_count 0x{:x}", (uint32_t)header.fixup_count);
        // LOG_INFO("profile_count 0x{:x}", (uint32_t)header.profile_count);
        // LOG_INFO("devblock_string_count 0x{:x}", (uint32_t)header.devblock_string_count);
        // LOG_INFO("include_count 0x{:x}", (uint32_t)header.include_count);
        // LOG_INFO("animtree_count 0x{:x}", (uint32_t)header.animtree_count);
        // LOG_INFO("flags 0x{:x}", (uint32_t)header.flags);
    }

    void GscObfuscator::ValidateInScript(const void* ptr, const char* descr) {
        if (ptr > scriptEnd) {
            throw std::runtime_error(std::format("Invalid {} ({} > {})", descr, ptr, scriptEnd));
        }
    };
    void GscObfuscator::ValidateStringInScript(const char* ptr, const char* descr) {
        ValidateInScript(ptr, descr);
        while (*(ptr++)) {
            ValidateInScript(ptr, descr);
        }
    }
    FreeDataChunk* GscObfuscator::FetchNearFreeDataChunk(size_t loc, size_t maxDist, size_t minSize, size_t align) {
        if (freeData.empty()) {
            return {};
        }

        int64_t smaxDist{ (int64_t)maxDist };
        for (auto it = freeData.begin(); it != freeData.end(); it++) {
            FreeDataChunk& chunk{ *it };
            size_t skipedSize{};

            if (align > 1 && alignedCode) {
                size_t origin{ chunk.rva };
                size_t alignedOrigin{ utils::Aligned(origin, align) };
                skipedSize = alignedOrigin - origin;
            }

            if (chunk.len < minSize + skipedSize) {
                LOG_TRACE("freedata too small: 0x{:x} < 0x{:x} + 0x{:x}", chunk.len, minSize, skipedSize);
                continue; // too small
            }
            int64_t delta{ ((int64_t)chunk.rva - (int64_t)loc) + (int64_t)chunk.len };

            if (delta > smaxDist || -delta > smaxDist) {
                LOG_TRACE("freedata too far: |0x{:x}| > 0x{}", delta < 0 ? -delta : delta, smaxDist);
                continue; // too far
            }

            chunk.rva += skipedSize;
            chunk.len -= skipedSize;
            return &chunk;
        }

        return {};
    }
    void GscObfuscator::SetAndPassOpCode(byte*& loc, opcode::OPCode op) {
        auto [ok, vl] = vmInfo->GetOpCodeId(opt.plt, op, modToolOpCodes);
        if (!ok) {
            throw std::runtime_error(std::format("Can't find opcode {}", opcode::OpCodeName(op)));
        }
        if (u16OpCodes) {
            ReadAlignedRef<uint16_t>(loc, "set opcode u16") = vl;
        } else {
            ReadAlignedRef<uint8_t>(loc, "set opcode u8") = (uint8_t)vl;
        }
    }

    uint16_t GscObfuscator::GetAndPassOpCode(byte*& loc) {
        if (u16OpCodes) {
            return ReadAlignedRef<uint16_t>(loc, "opcode u16");
        } else {
            return ReadAlignedRef<uint8_t>(loc, "opcode u8");
        }
    }

    void GscObfuscator::PatchOpcodes() {
        if (!opcodesLocs) {
            return;
        }
        byte* scriptBase{ scriptFile->Ptr() };

        size_t varCount{ 1 };
        for (uint32_t rva : *opcodesLocs) {
            byte* loc{ scriptFile->Ptr(rva) };

            byte* baseOpcode{ loc };
            uint16_t val{ GetAndPassOpCode(loc) };

            const opcode::OPCodeInfo* op{ vmInfo->LookupOpCode(opt.plt, val) };

            if (op->m_id == opcode::OPCODE_DevblockBegin) {
                int16_t delta{ ReadAlignedRef<int16_t>(loc, "DevblockBegin::delta") };

                if (delta <= 0) {
                    LOG_WARNING("Found invalid dev block with {} jump", delta);
                    continue;
                }

                byte* endDevBlock{ loc + delta };
                ValidateInScript(endDevBlock, "DevblockBegin::end");
                // we replace it with a jump, some decompilers doesn't like it
                if (!opt.noDevOpCodeReplace) {
                    SetAndPassOpCode(baseOpcode, opcode::OPCODE_Jump);
                }

                freeData.emplace_back(
                    FreeDataChunk{ .rva = (size_t)(loc - scriptBase), .len = (size_t)(endDevBlock - loc) }
                );
                continue;
            }

            if (!opt.noRemoveLocals) {
                switch (op->m_id) {
                case opcode::OPCODE_SafeCreateLocalVariables: {
                    if (vmInfo->HasFlag(opcode::VmFlags::VMF_VAR_ID)) {
                        continue; // no var names
                    }

                    uint64_t hashVarArg{ vmInfo->HashField("vararg") };
                    uint64_t hashVarArgCount{ vmInfo->HashField("varargcount") };

                    byte count{ ReadAlignedRef<byte>(loc, "SafeCreateLocalVariables::count") };

                    for (size_t i = 0; i < count; i++) {
                        if (vmInfo->HasFlag(opcode::VmFlags::VMF_HASH64)) {
                            uint64_t& n{ ReadAlignedRef<uint64_t>(loc, "SafeCreateLocalVariables::name64") };
                            if (n != hashVarArg && n != hashVarArgCount) {
                                n = opt.privateFileData.EncodeValNumber<uint64_t>(varCount++);
                            }
                        } else {
                            uint32_t& n{ ReadAlignedRef<uint32_t>(loc, "SafeCreateLocalVariables::name32") };
                            if (n != hashVarArg && n != hashVarArgCount) {
                                n = opt.privateFileData.EncodeValNumber<uint32_t>(varCount++);
                            }
                        }

                        if (!vmInfo->HasFlag(opcode::VmFlags::VMF_NO_PARAM_FLAGS)) {
                            ReadAlignedRef<byte>(loc, "SafeCreateLocalVariables::flags");
                        }
                    }
                    break;
                }
                case opcode::OPCODE_IW_RegisterVariable:
                    if (vmInfo->HasFlag(opcode::VmFlags::VMF_VAR_ID)) {
                        continue; // no var names
                    }
                    // only h64
                    ReadAlignedRef<uint64_t>(loc, "IW_RegisterVariable::name") =
                        opt.privateFileData.EncodeValNumber<uint64_t>(varCount++);
                    break;
                case opcode::OPCODE_IW_RegisterMultipleVariables: {
                    if (vmInfo->HasFlag(opcode::VmFlags::VMF_VAR_ID)) {
                        continue; // no var names
                    }
                    byte count{ ReadAlignedRef<byte>(loc, "IW_RegisterMultipleVariables::count") };
                    for (size_t i = 0; i < count; i++) {
                        ReadAlignedRef<uint64_t>(loc, "IW_RegisterMultipleVariables::name") =
                            opt.privateFileData.EncodeValNumber<uint64_t>(varCount++);
                    }
                    break;
                }
                }
            }
        }
    }

    void GscObfuscator::ApplyPrivateHashes() {
        core::bytebuffer::ByteBuffer bb{ script, scriptLen };
        for (auto& [str, val] : opt.privateFileData.GetHashes()) {
            // Why would the user has hashed values here, it's its strings
            for (auto& [t, func] : vmInfo->hashesFunc) {
                union {
                    uint32_t h32;
                    uint64_t h64;
                } hashBytes{ .h64 = func.hashFunc(str.data()) };

                if (func.size == 4) {
                    hashBytes.h32 = (uint32_t)hashBytes.h64;
                } else if (func.size != 8) {
                    LOG_WARNING("Invalid hash size type '{}': {}", t, func.size);
                    continue;
                }

                size_t off;

                while ((off = bb.Find((byte*)&hashBytes, func.size)) != std::string::npos) {
                    if (func.size == 8) {
                        *(uint64_t*)&script[off] = val;
                    } else if (func.size == 4) {
                        *(uint32_t*)&script[off] = val;
                    }
                }
            }
        }
    }

    void GscObfuscator::KillDevStrings() {
        size_t devStringsCount{ scriptFile->GetDevStringsCount() };
        if (devStringsCount) {
            T8GSCString* strings{ scriptFile->Ptr<T8GSCString>(scriptFile->GetDevStringsOffset()) };
            ValidateInScript(strings + 1, "dev strings table");

            for (size_t i = 0; i < devStringsCount; i++) {
                T8GSCString& str{ *strings };
                uint32_t* offsets{ (uint32_t*)&strings[1] };
                strings = (T8GSCString*)&offsets[strings->num_address];
                ValidateInScript(strings, "dev strings table");
                // kill dev block string information
                utils::data::FillRandomBuffer(offsets, sizeof(*offsets) * str.num_address);
                utils::data::FillRandomBuffer(&str, sizeof(str));
            }
            scriptFile->SetDevStringsCount(0);
            LOG_TRACE("removed {} dev strings data", devStringsCount);
        }
        scriptFile->SetDevStringsOffset(0);
    }

    void GscObfuscator::RewriteImports() {
        // rewrite imports without dev calls, we know we have enough memory
        byte* writer{ scriptFile->Ptr<byte>(scriptFile->GetImportsOffset()) };
        size_t ignored{};

        IW23GSCImport generic{};
        uint16_t count{};
        for (GscImportData* imp : imports) {
            importReader->SetHandle(imp->ref);
            if (imp->flags & T8GSCImportFlags::DEV_CALL) {
                ignored += importReader->GetNumAddresses() * sizeof(uint32_t) + importReader->SizeOf();
                continue; // we ignore it
            }

            LOG_TRACE(
                "remove dev call {}::{} (flags:0x{:x})",
                hashutils::ExtractTmp("namespace", importReader->GetNamespace()),
                hashutils::ExtractTmp("function", importReader->GetName()),
                imp->flags
            );

            importReader->ReadGeneric(generic);
            uint32_t* addrs{ importReader->GetAddresses() };
            size_t addrsSize{ sizeof(addrs[0]) * importReader->GetNumAddresses() };

            // rewrite it, knowing we read them in order, we can only be at the same location or before
            // so the addresses are known to be readable.
            scriptFile->WriteImport(writer, generic);
            writer += importReader->SizeOf();
            std::memmove(writer, addrs, addrsSize);
            writer += addrsSize;
            count++;
        }
        // write junk inside the old removed imports
        if (ignored) {
            utils::data::FillRandomBuffer(writer, ignored);
        }
        scriptFile->SetImportsCount(count);
    }

    void GscObfuscator::ApplyPrivateStrings() {
        size_t stringsCount{ scriptFile->GetStringsCount() };
        if (stringsCount) {
            T8GSCString* strings{ scriptFile->Ptr<T8GSCString>(scriptFile->GetStringsOffset()) };
            ValidateInScript(strings + 1, "strings table");

            for (size_t i = 0; i < stringsCount; i++) {
                T8GSCString& str{ *strings };
                uint32_t* offsets{ (uint32_t*)&strings[1] };
                strings = (T8GSCString*)&offsets[strings->num_address];

                ValidateInScript(strings, "strings table");

                char* s{ scriptFile->Ptr<char>(str.string) };
                ValidateStringInScript(s, "string ref");
                opt.privateFileData.RenamedString(s);
            }
        }
    }

    void GscObfuscator::ApplyPrivateScripts() {
        if (scriptFile->HasFlag(GOHF_STRING_NAMES)) {
            char* name{ scriptFile->Ptr<char>(scriptFile->GetNameOffset()) };
            ValidateStringInScript(name, "script name");
            std::string_view sw{ name };

            bool client;
            if (sw.ends_with(".csc") || sw.ends_with(".csh")) {
                client = true;
            } else if (sw.ends_with(".gsc") || sw.ends_with(".gsh")) {
                client = false;
            } else {
                LOG_WARNING("Invalid script name: {}", name);
                return;
            }

            opt.privateFileData.RenamedScript(name);

            size_t includesCount{ scriptFile->GetIncludesCount() };
            if (includesCount) {
                uint32_t* includes{ scriptFile->Ptr<uint32_t>(scriptFile->GetIncludesOffset()) };
                ValidateInScript(&includes[includesCount], "includes");
                for (size_t i = 0; i < includesCount; i++) {
                    char* inc{ scriptFile->Ptr<char>(includes[i]) };
                    ValidateStringInScript(inc, "include value");
                    opt.privateFileData.RenamedScriptExt(inc, client);
                }
            }
        } else {
            uint64_t fileName{ scriptFile->GetName() };
            if (opt.privateFileData.RenamedScriptHashed(fileName)) {
                scriptFile->SetName(fileName);
            }

            size_t includesCount{ scriptFile->GetIncludesCount() };
            if (includesCount) {
                uint64_t* includes{ scriptFile->Ptr<uint64_t>(scriptFile->GetIncludesOffset()) };
                ValidateInScript(&includes[includesCount], "includes hashed");
                for (size_t i = 0; i < includesCount; i++) {
                    opt.privateFileData.RenamedScriptHashed(includes[i]);
                }
            }
        }
    }

    void GscObfuscator::KillPrivateExports() {
        uint32_t privateCount{};
        for (auto& [nl, exps] : exportsMap) {
            if (exps.size() != 1) {
                LOG_WARNING(
                    "export {}::{} defined multiple times",
                    hashutils::ExtractTmp("namespace", nl.name_space),
                    hashutils::ExtractTmp("function", nl.name)
                );
                continue;
            }

            GscExportData& exp{ *exps[0] };
            exportReader->SetHandle(exp.ref);

            if ((exp.flags & T8GSCExportFlags::PRIVATE) != 0) {
                // private export, we can remove its name

                uint64_t newName{ vmInfo->HashField(opt.privateFileData.EncodeValTmp(++privateCount)) };

                exportReader->SetName(newName);

                auto iit{ importsMap.find(nl) };
                if (iit == importsMap.end()) {
                    continue;
                }

                for (GscImportData* imp : iit->second) {
                    importReader->SetHandle(imp->ref);
                    importReader->SetName(newName);
                }
            }
        }
    }

    void GscObfuscator::CreateTrampolines() {
        // sort free data by locations
        std::sort(freeData.begin(), freeData.end(), [](FreeDataChunk& a, FreeDataChunk& b) { return a.rva < b.rva; });

        byte* scriptBase{ scriptFile->Ptr() };

        for (GscExportData* exp : exports) {
            if (freeData.empty()) {
                LOG_DEBUG("no enough trampolines");
                break; // no more locations
            }
            FreeDataChunk* chunk{
                FetchNearFreeDataChunk(exp->bc - scriptBase, INT16_MAX, (!alignedCode && !u16OpCodes) ? 3 : 4, 2)
            }; // 3/4 = jmp+delta

            exportReader->SetHandle(exp->ref);

            if (!chunk) {
                LOG_DEBUG(
                    "no close trampoline for {}::{}",
                    hashutils::ExtractTmp("namespace", exportReader->GetNamespace()),
                    hashutils::ExtractTmp("function", exportReader->GetName())
                );
                continue; // no close location for this export
            }

            uint32_t exportLoc{ (uint32_t)chunk->rva };
            uint32_t oldExportLoc{ exportReader->GetAddress() };
            byte* base{ &scriptBase[exportLoc] };
            SetAndPassOpCode(base, opcode::OPCODE_Jump);
            int16_t delta{ (int16_t)(exp->bc - (base + 2)) };
            ReadAlignedRef<int16_t>(base, "trampoline jump") = delta;
            exportReader->SetAddress(exportLoc);
            exp->bc = base;

            uint32_t newChunkRva{ (uint32_t)(base - scriptBase) };
            size_t usedSpace{ newChunkRva - chunk->rva };
            // we update the chunk data
            chunk->len -= usedSpace;
            chunk->rva = newChunkRva;
            exp->len = usedSpace;

            LOG_TRACE(
                "create trampoline {}::{} at 0x{:x} -> 0x{:x} ({}0x{:x})",
                hashutils::ExtractTmp("namespace", exportReader->GetNamespace()),
                hashutils::ExtractTmp("function", exportReader->GetName()),
                exportLoc,
                oldExportLoc,
                (delta < 0 ? "-" : ""),
                (delta < 0 ? -delta : delta)
            );
        }
    }
    void GscObfuscator::KillRemainingDevBlocks() {
        for (FreeDataChunk& chunk : freeData) {
            if (chunk.len) {
                utils::data::FillRandomBuffer(scriptFile->Ptr(chunk.rva), chunk.len);
            }
        }
    }

    void GscObfuscator::ComputeChecksums() {
        for (GscExportData* data : exports) {
            exportReader->SetHandle(data->ref);
            if (!opt.recomputeCRC) {
                // we can remove the checksum by computing only one byte, it kills cerberus
                exportReader->SetChecksum(ComputeCRC32(data->bc, 1));
            } else if (data->len) {
                // we recompute the export checksum, like that cerberus can accept the data
                exportReader->SetChecksum(ComputeCRC32(data->bc, data->len));
            }
        }
    }

    void GscObfuscator::RunTasks() {
        if (opt.printData) {
            PrintHeader();
        }
        PatchOpcodes();
        if (!opt.noRemovePrivateExports) {
            KillPrivateExports();
        }
        ApplyPrivateStrings();
        ApplyPrivateScripts();
        ApplyPrivateHashes();
        if (!opt.noDebugKill) {
            RewriteImports();
            KillDevStrings();
        }
        if (!opt.noTrampoline) {
            CreateTrampolines();
        }
        if (!opt.noDebugKill) {
            KillRemainingDevBlocks();
        }
        ComputeChecksums();
    }

} // namespace tool::gsc::obfuscator::handler