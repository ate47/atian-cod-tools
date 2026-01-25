#include <includes.hpp>
#include <actslib/profiler.hpp>
#include <core/async.hpp>
#include <deps/miniz.hpp>
#include <core/bytebuffer.hpp>
#include <utils/decrypt.hpp>
#include <BS_thread_pool.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/gsc_iw.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/gsc_opcodes_load.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm_nodes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_opcodes.hpp>
#include <tools/cw/cw.hpp>
#include <gsc/gsc_acts_debug.hpp>
#include <tools/gsc/gsc_gdb.hpp>
#include <actscli.hpp>


namespace tool::gsc {
    using namespace tool::gsc::opcode;

    bool GscDecompilerGlobalContext::WarningType(GscDecompilerGlobalContextWarn warn) {
        core::async::opt_lock_guard lg{ asyncMtx };
        if ((warningOpt & warn)) return false;
        warningOpt |= warn;
        return true;
    }


    int DecompileGsc(byte* data, size_t size, std::filesystem::path fsPath, GscDecompilerGlobalContext& gdctx, byte* dbgData, size_t dbgSize) {
        std::string pathStr{ fsPath.string() };
        const char* path{ pathStr.data() };
        actslib::profiler::Profiler profiler{ "f" };
        actslib::profiler::ProfiledSection ps{ profiler, path };

        const GscInfoOption& opt = gdctx.opt;

        T8GSCOBJContext ctx{ gdctx };
        ctx.m_formatter = opt.m_formatter;
        ctx.dbgData = dbgData;
        ctx.dbgSize = dbgSize;
        auto& gsicInfo = ctx.m_gsicInfo;

        gsicInfo.isGsic = size > 4 && !memcmp(data, "GSIC", 4);
        if (gsicInfo.isGsic) {
            actslib::profiler::ProfiledSection ps{ profiler, "gsic reading" };
            LOG_DEBUG("Reading GSIC Compiled Script data");

            size_t gsicSize = 4; // preamble

            auto numFields = *reinterpret_cast<int32_t*>(&data[gsicSize]);
            gsicSize += 4;

            bool gsicError = false;
            for (size_t i = 0; i < numFields; i++) {
                auto fieldType = *reinterpret_cast<int32_t*>(&data[gsicSize]);
                gsicSize += 4;
                switch (fieldType) {
                case 0: // Detour
                {
                    auto detourCount = *reinterpret_cast<int32_t*>(&data[gsicSize]);
                    gsicSize += 4;
                    for (size_t j = 0; j < detourCount; j++) {
                        GsicDetour* detour = reinterpret_cast<GsicDetour*>(&data[gsicSize]);
                        gsicSize += (size_t)(28 + 256 - 1 - (5 * 4) + 1 - 8);

                        // register detour
                        GscDetourInfo& det = gsicInfo.detours[detour->fixupOffset];
                        det.name = detour->name;
                        det.fixupOffset = detour->fixupOffset;
                        det.fixupSize = detour->fixupSize;
                        det.replaceFunction = detour->replaceFunction;
                        det.replaceNamespace = detour->replaceNamespace;
                        det.replaceScript = *reinterpret_cast<uint64_t*>(&detour->replaceScriptTop);
                    }
                }
                break;
                default:
                    LOG_ERROR("Bad GSIC field type: {}", fieldType);
                    gsicError = true;
                    break;
                }
                if (gsicError) {
                    break;
                }
            }

            if (gsicError) {
                return tool::BASIC_ERROR;
            }

            // pass the GSIC data
            gsicInfo.headerSize = gsicSize;
            data += gsicSize;
        }

        if (size < 4) {
            LOG_ERROR("GSC file too small, no magic");
            return tool::BASIC_ERROR;
        }

        uint64_t vmVal;
        if (!memcmp("GSC", data, 4)) {
            vmVal = tool::gsc::opcode::VMI_IW_GSCBIN;

            if (!ctx.opt.m_vm) {
                LOG_INFO("GSCBIN decompiler requires a vm");
                return tool::BASIC_ERROR;
            }

            if (!IsValidVmMagic(opt.m_vm, ctx.m_vmInfo)) {
                LOG_ERROR("Can't find gscbin vm 0x{:x}", (uint64_t)ctx.opt.m_vm);
                return tool::BASIC_ERROR;
            }
        }
        else {
            if (size < 8) {
                LOG_ERROR("GSC file too small, no magic");
                return tool::BASIC_ERROR;
            }
            vmVal = *reinterpret_cast<uint64_t*>(data);

            if (opt.m_vm && vmVal != opt.m_vm) {
                LOG_INFO("Not the wanted vm: 0x{:x} != 0x{:x}", vmVal, (uint64_t)opt.m_vm);
                return tool::OK;
            }

            if (!IsValidVmMagic(vmVal, ctx.m_vmInfo)) {
                LOG_ERROR("Bad vm 0x{:x} for file {}", vmVal, path);
                return tool::BASIC_ERROR;
            }
        }
        hashutils::ReadDefaultFile();

        tool::gsc::vm::GscVm* readerBuilder = tool::gsc::vm::GetGscReader(vmVal);

        if (!readerBuilder) {
            LOG_ERROR("No handler available for vm 0x{:x} for file {}", vmVal, path);
            return tool::BASIC_ERROR;
        }

        ctx.scriptfile = readerBuilder->NewHandler(data, size);
        ctx.exp = CreateExportReader(ctx.m_vmInfo);

        GSCOBJHandler* scriptfile{ ctx.scriptfile.get() };
        GSCExportReader* exp{ ctx.exp.get() };

        // we keep it because it should also check the size
        if (!scriptfile->IsValidHeader(size)) {
            LOG_ERROR("Bad header 0x{:x} for file {}", scriptfile->Ref<uint64_t>(), path);
            return tool::BASIC_ERROR;
        }
        scriptfile->originalFile = &fsPath;

        utils::LineStreamBuf asmoutFile{};
        std::ostream asmout{ &asmoutFile };
        std::ostream& nullstream{ utils::NullStream() };
        utils::CloseEnd asmoutclose{ asmoutFile };
        ctx.lineBuf = &asmoutFile;

        if (ctx.opt.m_usePathOutput) {
            std::filesystem::path file{ std::filesystem::path{opt.m_outputDir} / fsPath };

            file.replace_extension();
            bool loaded{};
            if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                std::string fn{ file.filename().string() };

                if (std::find_if(fn.begin(), fn.end(), [](auto c) { return c > '9' || c < '0'; }) == fn.end()) {
                    // only number
                    try {
                        int id{ (int)std::strtol(fn.data(), nullptr, 10) };
                        const char* opaque{ tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, id, false) };

                        if (opaque) {
                            file = std::filesystem::path{ opt.m_outputDir } / opaque;
                            loaded = true;
                        }
                    }
                    catch (...) {}
                }
            }
            if (!loaded) {
                file.replace_extension(".gsc");
            }

            {
                core::async::opt_lock_guard lg{ gdctx.asyncMtx };
                std::filesystem::create_directories(file.parent_path());
            }
            if (!gdctx.noDump) {
                asmoutFile.in.open(file);

                if (!asmoutFile.in) {
                    LOG_ERROR("Can't open path output file {}", file.string());
                    return tool::BASIC_ERROR;
                }
            }
            LOG_INFO("Decompiling into '{}'...", file.string());
        }

        ctx.currentPlatform = opt.m_platform;
        ctx.isBigEndian = ctx.m_vmInfo->IsPlatformBigEndian(ctx.currentPlatform);

        bool endianSwapped{};
        if (ctx.SwitchEndian()) {
            scriptfile->SwitchHeaderEndian();
            endianSwapped = true;
        }

        if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_PLATFORM) && (!ctx.currentPlatform || ctx.currentPlatform == tool::gsc::opcode::PLATFORM_PC)) {
            ctx.currentPlatform = scriptfile->ComputePlatform(ctx);
            if (!ctx.currentPlatform) {
                LOG_INFO("This VM requires a platform which can't be computed");
                return tool::BASIC_ERROR;
            }
        }

        // required for gscbin
        int preloadRet{ scriptfile->PreLoadCode(ctx, opt.m_dumpSkipData ? asmout : nullstream) };
        if (preloadRet) {
            return preloadRet > 0 ? 0 : preloadRet;
        }


        std::stringstream dbgHeader{};

        bool inFileDBG{ !dbgData || !dbgSize };
        if (inFileDBG) {
            // search the debug data inside the script
            dbgData = scriptfile->Ptr(scriptfile->GetHeaderSize());
            dbgSize = scriptfile->GetFileSize() - scriptfile->GetHeaderSize();
        }

        core::bytebuffer::ByteBuffer dbgReader{ dbgData, dbgSize };

        if (dbgReader.CanRead(8)) {
            uint64_t magic{ *dbgReader.Ptr<uint64_t>() };

            tool::gsc::vm::GscGdb* dbgreader{ gsc::vm::GetGdbReader(magic) };

            if (!dbgreader) {
                if (!inFileDBG) {
                    LOG_WARNING("No debug handler for magic 0x{:x}", magic);
                }
            }
            else {
                dbgReader.Goto(0);

                try {
                    dbgreader->DbgLoad(ctx, dbgReader, dbgHeader);
                }
                catch (std::runtime_error& err) {
                    LOG_WARNING("Can't parse gdb data {}", err.what());
                }
            }
        }

        if (endianSwapped != ctx.SwitchEndian()) {
            scriptfile->SwitchHeaderEndian();
        }

        char asmfnamebuff[1000];

        const char* extractedName{ hashutils::ExtractPtr(scriptfile->GetName()) };

        if (opt.m_debugHashes && extractedName) {
            uint64_t hashPath{ ctx.m_vmInfo->HashPath(extractedName) };

            if (hashPath != scriptfile->GetName() && gdctx.WarningType(GDGCW_BAD_HASH_PATH)) {
                LOG_WARNING("Invalid hash algorithm for extracted name 0x{:x} != 0x{:x} for {}", scriptfile->GetName(), hashPath, extractedName);
            }
        }
        bool isCsc{};
        bool typeSure{};
        if (!scriptfile->HasFlag(GOHF_STRING_NAMES)) {
            const char* scriptNamePtr{ hashutils::ExtractPtr(scriptfile->GetName()) };

            if (scriptNamePtr) {
                std::string_view sw{ scriptNamePtr };
                if (sw.ends_with(".csc")) {
                    isCsc = true;
                    typeSure = true;
                }
                else if (sw.ends_with(".gsc")) {
                    isCsc = false;
                    typeSure = true;
                }
                else {
                    // the default script doesn't end with .gsc/.csc, it's fine
                    // LOG_WARNING("Script's name doesn't end with .gsc or .csc extension, can't extrapolate type: {}", scriptNamePtr);
                    goto ignoreCscGsc;
                }
            }

            if (scriptfile->GetIncludesOffset()) {
                uint32_t includeOffset = scriptfile->GetIncludesOffset();
                uint64_t* includes = scriptfile->Ptr<uint64_t>(includeOffset);
                if (!(includeOffset + scriptfile->GetIncludesCount() * sizeof(uint64_t) > scriptfile->GetFileSize())) {
                    for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                        const char* incPtr{ hashutils::ExtractPtr(includes[i]) };

                        if (incPtr) {
                            std::string_view sw{ incPtr };

                            if (sw.ends_with(".csc")) {
                                if (!isCsc && typeSure) {
                                    typeSure = false;
                                    LOG_WARNING("Found csc and gsc includes in the same script, can't extrapolate type");
                                    break;
                                }
                                isCsc = true;
                                typeSure = true;
                            }
                            else if (sw.ends_with(".gsc")) {
                                if (isCsc && typeSure) {
                                    typeSure = false;
                                    LOG_WARNING("Found csc and gsc includes in the same script, can't extrapolate type");
                                    break;
                                }
                                isCsc = false;
                                typeSure = true;
                            }
                            else {
                                typeSure = false;
                                LOG_WARNING("Found an include without .gsc or .csc extension, can't extrapolate type: {}", incPtr);
                                break;
                            }

                        }
                    }
                }
            }
        }
    ignoreCscGsc:

        if (opt.m_outputDir) {
            uint64_t hashname{ scriptfile->GetName() };

            const char* outDir;
            if (opt.m_splitByVm) {
                outDir = utils::va("%s/%s", opt.m_outputDir, tool::gsc::opcode::VMIdFancyName(ctx.m_vmInfo->vmMagic));
            }
            else {
                outDir = opt.m_outputDir;
            }

            if (hashname) {
                const char* name = opt.m_noPath ? nullptr : hashutils::ExtractPtr(hashname);

                if (!name) {
                    const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
                    if (actscli::options().heavyHashes) {
                        sprintf_s(asmfnamebuff, "%s/%016llX.%s", outDir, hashname, fileExt);
                    }
                    else {
                        sprintf_s(asmfnamebuff, "%s/hashed/script/script_%llx.%s", outDir, hashname, fileExt);
                    }
                }
                else {
                    sprintf_s(asmfnamebuff, "%s/%s", outDir, name);

                    char* extName{ utils::CloneString(name) };
                    std::string_view usingSw{ extName };
                    size_t usingSwPath{ usingSw.find_last_of('/') };
                    if (usingSw.ends_with(".gsc") || usingSw.ends_with(".csc")) {
                        extName[usingSw.length() - 4] = 0;
                    }

                    if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                        // IW vm import types
                        hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(extName), extName);
                    }
                    if (usingSwPath != std::string::npos) {
                        const char* namespaceAdd(&extName[usingSwPath + 1]);
                        hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                        if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                            hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                        }
                    }
                }
            }
            else {
                const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
                std::filesystem::path fn = fsPath;

                fn.replace_extension(fileExt);

                std::string name{ fn.string() };
                LOG_WARNING("Can't find script name, using {}", name);
                sprintf_s(asmfnamebuff, "%s/%s", outDir, name.data());
            }
        }
        else {
            sprintf_s(asmfnamebuff, "%sasm", path);
        }
        profiler.GetCurrent().name = asmfnamebuff;

        if (!ctx.opt.m_usePathOutput) {
            std::filesystem::path file{ std::filesystem::absolute(asmfnamebuff) };

            {
                core::async::opt_lock_guard lg{ gdctx.asyncMtx };
                std::filesystem::create_directories(file.parent_path());
            }
            if (!gdctx.noDump) {
                asmoutFile.in.open(file);

                if (!asmoutFile.in) {
                    LOG_ERROR("Can't open output file {} ({})", asmfnamebuff, hashutils::ExtractTmpScript(scriptfile->GetName()));
                    return tool::BASIC_ERROR;
                }
                LOG_INFO("Decompiling into '{}'{}...", asmfnamebuff, (gsicInfo.isGsic ? " (GSIC)" : ""));
            }
            else {
                LOG_INFO("Decompiling '{}'{}...", hashutils::ExtractTmpScript(scriptfile->GetName()), (gsicInfo.isGsic ? " (GSIC)" : ""));
            }
        }

        if (opt.m_copyright && *opt.m_copyright) {
            std::string_view cv{ opt.m_copyright };

            size_t idx{};
            do {
                size_t nl = cv.find('\n', idx);

                if (nl == std::string::npos) {
                    nl = cv.size();
                }

                asmout << "// " << cv.substr(idx, nl - idx) << std::endl;

                idx = nl + 1;
            } while (idx < cv.size());

            if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_LINE_AFTER_COPYRIGHT)) {
                asmout << std::endl;
            }
        }

        auto itdbg = gdctx.debugObjects.find(scriptfile->GetName());

        if (itdbg != gdctx.debugObjects.end()) {
            ctx.gdbctx = itdbg->second; // load dbg object
            LOG_DEBUG("Using dbg file {}", ctx.gdbctx->path.string());
        }

        if (opt.m_gdbZipOutputFile) {
            core::async::opt_lock_guard lg{ gdctx.asyncMtx };
            GscDecompilerGDBData*& gdb{ gdctx.gdbData[scriptfile->GetName()] };
            if (gdb) {
                LOG_WARNING("GDB data defined twice for file {}", hashutils::ExtractTmpScript(scriptfile->GetName()));
            }
            else {
                gdb = gdctx.alloc.New<GscDecompilerGDBData>();
                ctx.gdbData = gdb;
                gdb->gdb = ctx.m_vmInfo->gdbMagic;
                gdb->checksum = scriptfile->GetChecksum();
            }
        }

        if (opt.m_header) {
            asmout << "// ";
            if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                asmout << hashutils::ExtractTmpScript(scriptfile->GetName()) << "  (" << path << ")";
            }
            else {
                asmout << path;
            }
            asmout << " (size: " << size << " Bytes / " << std::hex << "0x" << size;

            if (typeSure) {
                asmout << " / " << (isCsc ? "CSC" : "GSC");
            }

            asmout << ")" << std::endl;

            if (gsicInfo.isGsic) {
                asmout << "// GSIC Compiled script" << ", header: 0x" << std::hex << gsicInfo.headerSize << std::endl;
            }
            if (gsicInfo.detours.size()) {
                asmout << "// detours: " << std::dec << gsicInfo.detours.size() << std::endl;
                for (const auto& [key, detour] : gsicInfo.detours) {
                    asmout << "// - ";

                    if (detour.replaceNamespace) {
                        asmout << hashutils::ExtractTmpPath("namespace", detour.replaceNamespace) << std::flush;
                    }
                    if (detour.replaceScript) {
                        asmout << "<" << hashutils::ExtractTmpScript(detour.replaceScript) << ">" << std::flush;
                    }

                    if (detour.replaceNamespace) {
                        asmout << "::";
                    }

                    asmout
                        << hashutils::ExtractTmp("function", detour.replaceFunction) << std::flush
                        << " offset: 0x" << std::hex << detour.fixupOffset << ", size: 0x" << detour.fixupSize << std::endl;
                }
            }


            if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_VERSION)) {
                if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_MAGIC)) {
                    asmout << "// magic .... 0x" << scriptfile->GetMagic();
                }
                else {
                    asmout << "//";
                }
                asmout << " vm: " << ctx.m_vmInfo->name << " (" << PlatformName(ctx.currentPlatform) << ")" << std::endl;
            }

            scriptfile->DumpHeader(asmout, opt);

            asmout << dbgHeader.str();
        }

        // write the strings before the patch to avoid reading pre-decrypted strings
        if (opt.m_strings) {
            if (scriptfile->GetDevStringsOffset() && !(opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR))) {
                T8GSCString* val = scriptfile->Ptr<T8GSCString>(scriptfile->GetDevStringsOffset());
                for (size_t i = 0; i < scriptfile->GetDevStringsCount(); i++) {

                    const char* str = utils::va("<dev string:x%x>", val->string); // Ptr<char>(val->string); // no gdb

                    asmout << "Dev String: "
                        << "addr:" << std::hex << val->string << ", "
                        << "count:" << std::dec << (int)val->num_address << ", stype:"
                        << (int)val->type << " -> \"" << str << "\"" << std::endl;
                    asmout << "loc: ";

                    uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
                    asmout << ctx.GetFLocName(loc[0]);
                    for (size_t j = 1; j < val->num_address; j++) {
                        asmout << "," << ctx.GetFLocName(loc[j]);
                    }

                    asmout << std::endl;
                    val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
                }
            }
            if (scriptfile->GetStringsOffset()) {
                uintptr_t str_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetStringsOffset()));

                for (size_t i = 0; i < scriptfile->GetStringsCount(); i++) {

                    const auto* str = reinterpret_cast<T8GSCString*>(str_location);

                    asmout << std::hex << "String addr:" << str->string << ", count:" << std::dec << (int)str->num_address << ", type:" << (int)str->type << ", loc:0x" << std::hex << (str_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr())) << std::endl;

                    if (str->string >= scriptfile->GetFileSize()) {
                        asmout << "bad string location : 0x" << std::hex << str->string << "/0x" << scriptfile->GetFileSize() << std::endl;
                        break;
                    }

                    char* encryptedString = scriptfile->Ptr<char>(str->string);

                    size_t len{ std::strlen(encryptedString) };
                    byte type{ (byte)(*reinterpret_cast<byte*>(encryptedString)) };

                    if (str->string + len > scriptfile->GetFileSize()) {
                        asmout << "bad string location + len : 0x" << std::hex << str->string << "/0x" << scriptfile->GetFileSize() << std::endl;
                        break;
                    }

                    asmout << "encryption: ";
                    if ((type & 0xC0) != 0x80) {
                        asmout << "(none)";
                    }
                    else {
                        asmout << "0x" << std::hex << (int)type;
                    }
                    asmout << " elen: " << std::dec << len << " -> " << std::flush;

                    char* cstr = encryptedString;

                    if (!ctx.opt.m_noStrDecrypt) {
                        cstr = scriptfile->DecryptString(cstr);
                    }

                    size_t dlen{ strlen(cstr) };
                    asmout << '"' << utils::FormattedString(cstr) << '"' << "(" << std::dec << dlen;

                    if (dlen > len) {
                        asmout << ",missing";
                    }
                    if ((type & 0xC0) == 0x80) {
                        byte ntype{ (byte)(*reinterpret_cast<byte*>(encryptedString)) };
                        asmout << ",pt:0x" << std::hex << (int)ntype;
                        if (encryptedString <= cstr) {
                            size_t delta{ (size_t)(cstr - encryptedString) };
                            asmout << ",delta:0x" << delta;
                            if (delta && delta < 5) {
                                asmout << ",data=";
                                for (size_t i = 0; i < delta; i++) {
                                    if (i) asmout << ",";
                                    asmout << "0x" << std::hex << (int)reinterpret_cast<byte*>(encryptedString)[i];
                                }
                            }
                        }
                        else {
                            asmout << ",delta:-0x" << (encryptedString - cstr);
                        }
                    }

                    asmout << ")" << std::endl;

                    asmout << "location(s): ";

                    const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
                    asmout << ctx.GetFLocName(strings[0]);
                    for (size_t j = 1; j < str->num_address; j++) {
                        asmout << "," << ctx.GetFLocName(strings[j]);
                    }
                    asmout << std::endl;
                    str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
                }
            }

            if (scriptfile->GetStringsCount() || scriptfile->GetDevStringsCount()) {
                asmout << std::endl;
            }
        }

        int patchCodeResult{};

        if (opt.m_patch) {
            actslib::profiler::ProfiledSection ps{ profiler, "patch linking" };
            // unlink the script and write custom gvar/string ids
            patchCodeResult = scriptfile->PatchCode(ctx);

            {
                core::async::opt_lock_guard hlg{ hashutils::GetMutex(false) };
                for (const auto& [id, str] : ctx.m_stringRefs) {
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(str), str, true);
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(str), str, true);
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashPath(str), str, true);

                    // use all the known hashes for this VM
                    for (auto& [k, func] : ctx.m_vmInfo->hashesFunc) {
                        try {
                            int64_t hash = func.hashFunc(str);

                            if (hash) {
                                hashutils::AddPrecomputed(hash, str, true);
                            }
                        }
                        catch (std::exception&) {
                            // ignore
                        }
                    }
                }
            }
        }

        if (opt.m_includes && scriptfile->GetIncludesOffset() && scriptfile->GetIncludesCount()) {
            uint32_t includeOffset = scriptfile->GetIncludesOffset();
            std::vector<std::string> usingsList{};
            if (scriptfile->HasFlag(GOHF_STRING_NAMES)) {
                if (includeOffset + scriptfile->GetIncludesCount() * sizeof(uint32_t) > scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid include offset 0x{:x} > 0x{:x}", includeOffset, scriptfile->GetFileSize());
                    return tool::BASIC_ERROR;
                }
                uint32_t* includes = scriptfile->Ptr<uint32_t>(includeOffset);

                for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                    if (includes[i] >= scriptfile->GetFileSize()) {
                        LOG_ERROR("Invalid include string offset 0x{:x} > 0x{:x}", includes[i], scriptfile->GetFileSize());
                        return tool::BASIC_ERROR;
                    }
                    //asmout << "#using " << scriptfile->Ptr<char>(includes[i]) << ";" << std::endl;
                    const char* usingName{ scriptfile->Ptr<char>(includes[i]) };
                    usingsList.emplace_back(usingName);

                    std::string_view usingSw{ usingName };
                    size_t usingSwPath{ usingSw.find_last_of('/') };

                    if (usingSwPath != std::string::npos) {
                        const char* namespaceAdd(&usingName[usingSwPath + 1]);
                        hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                        if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                            hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                        }
                    }
                }
            }
            else {
                uint64_t* includes = scriptfile->Ptr<uint64_t>(includeOffset);
                if (includeOffset + scriptfile->GetIncludesCount() * sizeof(uint64_t) > scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid include offset 0x{:x} > 0x{:x}", includeOffset, scriptfile->GetFileSize());
                    return tool::BASIC_ERROR;
                }

                for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                    auto [isFound, usingName] = hashutils::ExtractTmpPair("script", includes[i]);
                    if (isFound) {
                        hashutils::CleanPath(usingName);

                        if (typeSure) {
                            usingName[std::strlen(usingName) - 4] = 0; // remove .csc / .gsc

                            if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                                // IW vm import types
                                hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(usingName), usingName);
                            }
                            std::string_view usingSw{ usingName };
                            size_t usingSwPath{ usingSw.find_last_of('/') };

                            if (usingSwPath != std::string::npos) {
                                const char* namespaceAdd(&usingName[usingSwPath + 1]);
                                hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                                if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                                }
                            }
                        }
                    }

                    usingsList.emplace_back(usingName);
                    //asmout << "#using " << usingName << ";" << std::endl;

                    if (opt.m_debugHashes) {
                        const char* incExt{ hashutils::ExtractPtr(includes[i]) };
                        if (incExt) {
                            uint64_t hashPath{ ctx.m_vmInfo->HashPath(incExt) };

                            if (hashPath != includes[i] && gdctx.WarningType(GDGCW_BAD_HASH_PATH_INCLUDE)) {
                                LOG_WARNING("Invalid hash algorithm for extracted include 0x{:x} != 0x{:x} for {}", includes[i], hashPath, incExt);
                            }
                        }
                    }
                }
            }
            // better rendering, but not really good for debug
            if (!opt.m_noUsingsSort) {
                std::sort(usingsList.begin(), usingsList.end());
            }

            for (const std::string& usingName : usingsList) {
                asmout << "#using " << usingName << ";" << std::endl;
            }

            asmout << std::endl;
        }

        asmout
            << std::flush;

        scriptfile->DumpExperimental(asmout, opt, ctx);

        if (opt.m_tokens && ctx.m_tokens.size()) {
            asmout << "// tokens" << std::endl;
            for (size_t i = 0; i < ctx.m_tokens.size(); i++) {
                GSCOBJTokenData& data{ ctx.m_tokens[i] };

                asmout << "token 0x" << std::hex << i << " loc:0x" << data.loc << " | ";
                if (data.isString) {
                    asmout << "(str) " << data.val.str;
                }
                else {
                    asmout << "(id) " << tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, data.val.id);
                }
                asmout << std::endl;
            }
            asmout << std::endl;
        }

        if (opt.m_gvars && scriptfile->GetGVarsOffset()) {
            uintptr_t gvars_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetGVarsOffset()));

            for (size_t i = 0; i < scriptfile->GetGVarsCount(); i++) {
                const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
                asmout << std::hex << "Global var " << hashutils::ExtractTmp("var", globalvar->name) << " " << globalvar->num_address << std::endl;

                asmout << "location(s): ";

                const auto* vars = reinterpret_cast<const uint32_t*>(&globalvar[1]);
                asmout << std::hex << ctx.GetFLocName(vars[0]);
                for (size_t j = 1; j < globalvar->num_address; j++) {
                    asmout << std::hex << "," << ctx.GetFLocName(vars[j]);
                }
                asmout << std::endl;
                gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
            }
            if (scriptfile->GetGVarsCount()) {
                asmout << std::endl;
            }
        }

        if (opt.m_imports) {
            uintptr_t import_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetImportsOffset()));

            for (size_t i = 0; i < scriptfile->GetImportsCount(); i++) {
                uint64_t name_space;
                uint64_t name;
                size_t impSize;
                byte flags;
                byte param_count;
                uint16_t numAddress;

                if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_LIKE)) {
                    if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + sizeof(IW23GSCImport) > scriptfile->GetFileSize()) {
                        LOG_ERROR("Invalid import {} location", i);
                        return tool::BASIC_ERROR;
                    }
                    const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);
                    name_space = imp->name_space;
                    name = imp->name;
                    flags = imp->flags;
                    numAddress = imp->num_address;
                    param_count = imp->param_count;
                    impSize = sizeof(*imp);
                }
                else {
                    if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + sizeof(T8GSCImport) > scriptfile->GetFileSize()) {
                        LOG_ERROR("Invalid import {} location", i);
                        return tool::BASIC_ERROR;
                    }
                    const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
                    name_space = imp->import_namespace;
                    name = imp->name;
                    flags = imp->flags;
                    param_count = imp->param_count;
                    numAddress = imp->num_address;
                    impSize = sizeof(*imp);
                }
                if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + impSize + sizeof(uint32_t) * numAddress > scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid import {} num address {}", i, numAddress);
                    return tool::BASIC_ERROR;
                }

                asmout << std::hex << "import ";

                auto remapedFlags = scriptfile->RemapFlagsImport(flags);

                switch (remapedFlags & T8GSCImportFlags::CALLTYPE_MASK) {
                case FUNC_METHOD: asmout << "get "; break;
                case FUNCTION: asmout << "function "; break;
                case FUNCTION_THREAD: asmout << "function thread "; break;
                case FUNCTION_CHILDTHREAD: asmout << "function childthread "; break;
                case METHOD: asmout << "method "; break;
                case METHOD_THREAD: asmout << "method thread "; break;
                case METHOD_CHILDTHREAD: asmout << "method childthread "; break;
                case ACTS_CALL_BUILTIN_FUNCTION: asmout << "builtin function "; break;
                case ACTS_CALL_BUILTIN_METHOD: asmout << "builtin method "; break;
                case ACTS_GET_BUILTIN_FUNCTION: asmout << "get builtin function "; break;
                case ACTS_GET_BUILTIN_METHOD: asmout << "get builtin method "; break;
                case ACTS_CALL_BUILTIN_FUNCTION_NO_PARAMS: asmout << "builtin function (np) "; break;
                case ACTS_CALL_BUILTIN_METHOD_NO_PARAMS: asmout << "builtin method (np) "; break;
                default:
                    asmout << "<errorflag:" << std::hex << (remapedFlags & 0xF) << "> ";
                    break;
                }

                if (remapedFlags & T8GSCImportFlags::DEV_CALL) {
                    asmout << "devcall ";
                }

                if ((remapedFlags & T8GSCImportFlags::GET_CALL) == 0) {
                    // no need for namespace if we are getting the call dynamically (api or inside-code script)
                    asmout << "get ";
                }
                asmout << hashutils::ExtractTmpPath("namespace", name_space) << std::flush << "::";

                asmout << std::hex << hashutils::ExtractTmp("function", name);

                if (opt.m_rawhash) {
                    asmout << " (" << std::hex << name_space << "::" << name << ")";
                }

                asmout << std::endl;

                asmout << std::hex << "address: " << numAddress
                    << ", params: " << std::dec << (int)param_count
                    << ", iflags: 0x" << std::hex << (uint16_t)(flags)
                    << ", iftype: 0x" << std::hex << (int)(flags & T8GSCImportFlags::CALLTYPE_MASK)
                    << ", loc: 0x" << std::hex << (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr()))
                    << std::endl;

                asmout << "location(s): ";

                const auto* imports = reinterpret_cast<const uint32_t*>(import_location + impSize);
                asmout << std::hex << ctx.GetFLocName(imports[0]);
                for (size_t j = 1; j < numAddress; j++) {
                    asmout << std::hex << "," << ctx.GetFLocName(imports[j]) << "(0x" << imports[j] << ")";
                }
                asmout << std::endl;

                asmout << "--------------" << std::endl;

                import_location += impSize + sizeof(*imports) * numAddress;
            }
            if (scriptfile->GetImportsCount()) {
                asmout << std::endl;
            }
        }
        if (opt.m_imports) {
            if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_ANIMTREE_T7)) {
                // in t7 the anims are in a single struct
                if (scriptfile->GetAnimTreeDoubleOffset()) {
                    uintptr_t animt_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetAnimTreeDoubleOffset()));
                    size_t anims_count = (size_t)scriptfile->GetAnimTreeDoubleCount();

                    for (size_t i = 0; i < anims_count; i++) {
                        const auto* animt = reinterpret_cast<T7GscAnimTree*>(animt_location);

                        if (animt->name >= scriptfile->GetFileSize()) {
                            asmout << std::hex << "invalid animtree name 0x" << animt->name << std::endl;
                        }
                        else {
                            char* s = scriptfile->Ptr<char>(animt->name);

                            asmout << std::hex << "animtree " << (s ? s : "<err>") << " : 0x" << ((byte*)animt - scriptfile->Ptr()) << std::endl;

                            const uint32_t* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                            asmout << "tree address (" << std::dec << animt->num_tree_address << ", 0x" << std::hex << ((byte*)vars - scriptfile->Ptr()) << "):";
                            for (size_t j = 0; j < animt->num_tree_address; j++) {
                                asmout << " " << ctx.GetFLocName(*(vars++));
                            }
                            asmout << std::endl;
                            const uint64_t* vars2 = reinterpret_cast<const uint64_t*>(vars);
                            asmout << "node address (" << std::dec << animt->num_node_address << ", 0x" << std::hex << ((byte*)vars2 - scriptfile->Ptr()) << "):";
                            for (size_t j = 0; j < animt->num_node_address; j++) {

                                if (vars2[0] >= scriptfile->GetFileSize()) {
                                    asmout << std::hex << "invalid animtree 2nd name 0x" << animt->name << std::endl;
                                }
                                else {
                                    char* v = scriptfile->Ptr<char>(vars2[0]);
                                    // why u64?
                                    asmout << " " << ctx.GetFLocName((uint32_t)vars2[1]) << ":" << v;
                                }

                                vars2 += 2;
                            }
                            asmout << std::endl;
                        }

                        animt_location += sizeof(*animt) + sizeof(uint32_t) * (animt->num_tree_address + (size_t)animt->num_node_address * 4);
                    }
                    if (scriptfile->GetAnimTreeDoubleCount()) {
                        asmout << std::endl;
                    }
                }
            }
            else {
                if (scriptfile->GetAnimTreeSingleOffset()) {
                    uintptr_t animt_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetAnimTreeSingleOffset()));
                    size_t anims_count = (size_t)scriptfile->GetAnimTreeSingleCount();

                    for (size_t i = 0; i < anims_count; i++) {
                        const auto* animt = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(animt_location);

                        if (animt->address >= scriptfile->GetFileSize()) {
                            asmout << std::hex << "invalid animtree name 0x" << animt->address << std::endl;
                        }
                        else {
                            char* s = scriptfile->Ptr<char>(animt->address);

                            asmout << "#using_animtree(";

                            if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                                asmout << " ";
                            }

                            asmout << '"' << utils::FormattedString(s) << '"';

                            if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                                asmout << " ";
                            }

                            asmout << ");" << std::endl;
                        }

                        animt_location += sizeof(*animt) + sizeof(uint32_t) * animt->num_address;
                    }
                    if (scriptfile->GetAnimTreeDoubleCount()) {
                        asmout << std::endl;
                    }
                }
            }
        }

        if (patchCodeResult) {
            return patchCodeResult;
        }

        bool dumpAllErrors = actscli::options().debug;
        int exportErrors{};

        if (opt.m_func) {
            actslib::profiler::ProfiledSection ps{ profiler, "decompiling" };
            // current namespace
            uint64_t currentNSP{};
            const char* currentAnimTree{};

            if (scriptfile->GetExportsOffset() + scriptfile->GetExportsCount() * exp->SizeOf() > scriptfile->GetFileSize()) {
                asmout << "// INVALID EXPORT TABLE: 0x" << std::hex << scriptfile->GetExportsOffset() << std::endl;
                return -1;
            }

            for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset() + i * exp->SizeOf());
                exp->SetHandle(handle);

                // if we aren't dumping the ASM, we compute all the nodes first
                std::ostream& output{ opt.m_dasm ? asmout : nullstream };

                if (exp->GetNamespace() != currentNSP) {
                    currentNSP = exp->GetNamespace();

                    if (opt.m_dasm) {
                        output << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                    }
                }
                else if (!currentNSP && !i) {
                    if (opt.m_dasm) {
                        output << std::endl;
                    }
                }

                NameLocated rname = { exp->GetNamespace(), exp->GetName() };

                if (exp->GetAddress() > scriptfile->GetFileSize()) {
                    asmout << "// INVALID EXPORT ADDRESS: 0x" << std::hex << exp->GetAddress() << std::endl;
                    continue;
                }

                auto r = ctx.contextes.try_emplace(rname, scriptfile->Ptr(exp->GetAddress()), *scriptfile, ctx, opt, currentNSP, *exp, handle, ctx.m_vmInfo->vmMagic, ctx.currentPlatform);

                if (!r.second) {
                    asmout << "Duplicate node "
                        << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << std::flush << "::"
                        << hashutils::ExtractTmp("function", exp->GetName()) << std::endl;
                    continue;
                }

                auto& asmctx = r.first->second;


                if (opt.m_debugHashes) {
                    uint64_t name{ exp->GetName() };
                    const char* namePtr{ hashutils::ExtractPtr(name) };
                    if (namePtr) {
                        uint64_t hashScr{ ctx.m_vmInfo->HashField(namePtr) };

                        if (hashScr != name && gdctx.WarningType(GDGCW_BAD_HASH_FIELD)) {
                            LOG_WARNING("Invalid hash algorithm for extracted field 0x{:x} != 0x{:x} for {}", name, hashScr, namePtr);
                        }
                    }
                    uint64_t fileNameSpace{ exp->GetFileNamespace() };
                    if (fileNameSpace) {

                        const char* fnsPtr{ hashutils::ExtractPtr(fileNameSpace) };
                        if (fnsPtr) {
                            uint64_t hashFSScr{ ctx.m_vmInfo->HashFilePath(fnsPtr) };

                            if (hashFSScr != fileNameSpace && gdctx.WarningType(GDGCW_BAD_HASH_FILE)) {
                                LOG_WARNING("Invalid hash algorithm for extracted field 0x{:x} != 0x{:x} for {}", fileNameSpace, hashFSScr, fnsPtr);
                            }
                        }
                    }
                }

                DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);

                if (asmctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                    output << std::endl;
                }
                else {
                    output << " ";
                }
                output << "gscasm {" << std::endl;

                try {
                    tool::gsc::DumpAsm(*exp, output, *scriptfile, ctx, asmctx);
                }
                catch (std::runtime_error& err) {
                    output << "FAILURE, " << err.what() << std::endl;
                    asmctx.DisableDecompiler(err.what());

                    {
                        core::async::opt_lock_guard lg{ gdctx.asyncMtx };
                        gdctx.hardErrors++;
                        if (!(exportErrors++) || dumpAllErrors) {
                            LOG_ERROR("Can't decompile export: {}", err.what());
                        }
                    }
                }


                output << "}" << std::endl;

                if (asmctx.m_disableDecompiler) {
                    if (opt.m_dasm || opt.m_func_header_post) {
                        DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);
                        output << ";" << std::endl;
                    }

                    if (opt.m_dasm) {
                        output << "// Can't decompile export " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << asmctx.m_disableDecompilerError << std::endl;
                    }
                    else if (!opt.m_dcomp) {
                        LOG_WARNING("Can't decompile export {}::{}", hashutils::ExtractTmpPath("namespace", exp->GetNamespace()), hashutils::ExtractTmp("function", exp->GetName()));
                    }
                }


                if ((!opt.m_dasm || opt.m_dcomp || opt.m_func_header_post) && !asmctx.m_disableDecompiler) {
                    asmctx.ComputeDefaultParamValue();

                    if (opt.m_dasm || opt.m_func_header_post) {
                        DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);
                    }
                    output << std::flush;
                    DecompContext dctx{ 0, 0, asmctx.m_opt, 0, exp->GetAddress(), ctx.gdbData, ctx.lineBuf };
                    if (opt.m_dcomp) {
                        if (!opt.m_vtable && scriptfile->IsVTableImportFlags(exp->GetFlags())) {
                            asmctx.m_bcl = scriptfile->Ptr(exp->GetAddress());
                            int ret{ DumpVTable(*exp, output, *scriptfile, ctx, asmctx, dctx) };
                            asmctx.m_vtable = ret != DVA_NOT;
                            if (ret == DVA_BAD) {
                                output << "// Can't decompile vtable " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << ret << std::endl;
                            }
                        }

                        if (!asmctx.m_vtable) {
                            asmctx.ComputePreSpecialPattern();
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_DEV)) {
                                asmctx.ComputeDevBlocks();

                                if (//(scriptfile->RemapFlagsExport(exp->GetFlags()) & T8GSCExportFlags::PRIVATE) != 0 &&
                                    SizeNoEmptyNode(asmctx.m_funcBlock.m_statements) == 2) {
                                    auto* node = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 0);
                                    auto* ret = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 1);
                                    if (ret->node && ret->node->m_type == TYPE_END
                                        && node->node && node->node->m_type == TYPE_BLOCK
                                        && static_cast<ASMContextNodeBlock*>(node->node)->m_blockType == BLOCK_DEV) {
                                        asmctx.m_devFuncCandidate = true;
                                    }
                                }
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_SWITCH)) {
                                asmctx.ComputeSwitchBlocks();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOREACH)) {
                                asmctx.ComputeForEachBlocks();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_WHILE)) {
                                asmctx.ComputeWhileBlocks();
                            }
                            asmctx.ComputeSpecialPattern();
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOR)) {
                                asmctx.ComputeForBlocks();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_IF)) {
                                asmctx.ComputeIfBlocks();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_RETURN)) {
                                asmctx.ComputeReturnJump();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_BOOL_RETURN)) {
                                asmctx.ComputeBoolReturn();
                            }
                            if (!(asmctx.m_opt.m_stepskip & STEPSKIP_COMPILER_PATTERNS)) {
                                asmctx.ComputeCustomCompilerPattern();
                            }
                            if (opt.m_dasm) {
                                if (asmctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                                    output << std::endl;
                                }
                                else {
                                    output << " ";
                                }
                                asmctx.Dump(output, dctx);
                            }
                        }
                    }
                    else {
                        output << ";" << std::endl;
                    }
                }
                output << std::endl;
            }

            if (!opt.m_dasm && opt.m_dcomp) {
                // current namespace
                currentNSP = 0;
                currentAnimTree = nullptr;
                int currentPadding{};
                bool inDevBlock{};

                uint64_t constructorName{ ctx.m_vmInfo->HashField("__constructor") };
                uint64_t destructorName{ ctx.m_vmInfo->HashField("__destructor") };


                auto handleClsAnimTree = [&ctx](uint64_t name, uint64_t method, gscclass& cls) -> bool {
                    NameLocated lname{ name, method };

                    auto masmctxit = ctx.contextes.find(lname);

                    if (masmctxit == ctx.contextes.end()) {
                        return true;
                    }

                    const char* atr{ masmctxit->second.useAnimTree };

                    if (!atr) {
                        return true; // nothing to check
                    }

                    if (!cls.animtree) {
                        cls.animtree = atr;
                        cls.hasAnimTrees = true;
                        return true; // default
                    }

                    if (strcmp(cls.animtree, atr)) {
                        cls.animtree = nullptr;
                        return false; // no common atr, we can't put it at the top level
                    }

                    return true;
                    };

                for (auto& [name, cls] : ctx.m_classes) {
                    if (!handleClsAnimTree(name, constructorName, cls)) continue;
                    if (!handleClsAnimTree(name, destructorName, cls)) continue;
                    for (const auto& method : cls.m_methods) {
                        if (method == constructorName || method == destructorName) continue; // ignore const/destr
                        if (!handleClsAnimTree(name, method, cls)) break;
                    }
                }

                for (const auto& [name, cls] : ctx.m_classes) {
                    if (cls.name_space != currentNSP) {
                        currentNSP = cls.name_space;

                        asmout << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                    }

                    if (cls.animtree && (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ANIM_REAL))) {
                        if (!currentAnimTree || std::strcmp(currentAnimTree, cls.animtree)) {
                            currentAnimTree = cls.animtree; // new name
                            // write animtree

                            asmout << "#using_animtree(";

                            if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                                asmout << " ";
                            }

                            asmout << '"' << utils::FormattedString(cls.animtree) << '"';

                            if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                                asmout << " ";
                            }

                            asmout << ");" << std::endl << std::endl;
                        }
                    }

                    asmout << "// Namespace " << hashutils::ExtractTmpPath("namespace", cls.name_space) << std::endl;
                    asmout << "// Method(s) " << std::dec << cls.m_methods.size() << " Total " << cls.m_vtable.size() << std::endl;
                    asmout << "class " << hashutils::ExtractTmp("class", name) << std::flush;

                    if (cls.m_superClass.size()) {
                        // write superclasses
                        asmout << " : ";
                        auto it = cls.m_superClass.begin();
                        asmout << hashutils::ExtractTmp("class", *it) << std::flush;
                        it++;

                        while (it != cls.m_superClass.end()) {
                            asmout << ", " << hashutils::ExtractTmp("class", *it) << std::flush;
                            it++;
                        }
                    }

                    if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                        asmout << std::endl;
                    }
                    else {
                        asmout << " ";
                    }
                    asmout << "{" << std::endl << std::endl;



                    auto handleMethod = [&currentPadding, &opt, &asmout, &scriptfile, name, &ctx, &currentAnimTree](uint64_t method, const char* forceName, bool ignoreEmpty, bool specialMember) -> void {
                        auto lname = NameLocated{ name, method };

                        auto masmctxit = ctx.contextes.find(lname);

                        if (masmctxit == ctx.contextes.end()) {
                            return;
                        }

                        auto& e = masmctxit->second;

                        if (specialMember) e.noFunctionPrefix = true;

                        // set the export handle
                        e.m_exp.SetHandle(e.m_readerHandle);
                        uint32_t floc = e.m_exp.GetAddress();

                        if (e.m_disableDecompiler) {
                            DumpFunctionHeader(e.m_exp, asmout, *scriptfile, ctx, e, 1, forceName, &currentAnimTree);
                            asmout << ";" << std::endl;
                            return;
                        }

                        if (!ignoreEmpty || SizeNoEmptyNode(e.m_funcBlock.m_statements) > 1) {
                            // ignore empty exports (constructor/destructors)


                            DumpFunctionHeader(e.m_exp, asmout, *scriptfile, ctx, e, 1, forceName, &currentAnimTree);
                            DecompContext dctx{ 0, 0, e.m_opt, currentPadding + 1, floc, ctx.gdbData, ctx.lineBuf };
                            if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                                dctx.WritePadding(asmout << std::endl);
                            }
                            else {
                                asmout << " ";
                            }
                            e.Dump(asmout, dctx);
                            asmout << std::endl;
                        }

                        ctx.contextes.erase(masmctxit);
                        };

                    std::unordered_set<uint64_t> selfmembers{};

                    for (const auto& method : cls.m_methods) {
                        auto lname = NameLocated{ name, method };

                        auto masmctxit = ctx.contextes.find(lname);

                        if (masmctxit == ctx.contextes.end()) {
                            LOG_WARNING("Can't find {}", hashutils::ExtractTmp("function", method));
                            continue;
                        }
                        masmctxit->second.ConvertToClassMethod(selfmembers);
                    }

                    if (selfmembers.size()) {
                        // sort members using string lookup value for a better rendering
                        std::set<std::string> selfMembersSorted{};
                        for (uint64_t field : selfmembers) {
                            selfMembersSorted.insert(hashutils::ExtractTmp("var", field));
                        }

                        for (const std::string& field : selfMembersSorted) {
                            utils::Padding(asmout, 1) << "var " << field << ";" << std::endl;
                        }

                        asmout << std::endl;
                    }

                    // handle first the constructor/destructor
                    handleMethod(constructorName, "constructor", true, true);
                    handleMethod(destructorName, "destructor", true, true);

                    for (const auto& method : cls.m_methods) {
                        handleMethod(method, nullptr, false, false);
                    }

                    if (cls.hasAnimTrees && !cls.animtree) {
                        // we need to cleanup the animtree using because it is not scoped anymore
                        currentAnimTree = nullptr;
                    }

                    asmout << "}" << std::endl << std::endl;
                }

                // compute dev function
                for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                    void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
                    exp->SetHandle(handle);

                    NameLocated lname{ exp->GetNamespace(), exp->GetName() };

                    auto f = ctx.contextes.find(lname);

                    if (f == ctx.contextes.end()) {
                        continue; // already parsed
                    }

                    auto& asmctx = f->second;

                    if (asmctx.m_disableDecompiler || asmctx.m_vtable) {
                        continue;
                    }

                    asmctx.ForSubNodes([&ctx, &lname](ASMContextNode*& node, SubNodeContext& sctx) {
                        if (!node) {
                            return;
                        }
                        ASMContextNodeFuncRef* funcRef;
                        if (node->m_type == TYPE_FUNC_CALL) {
                            auto* callFunc = dynamic_cast<ASMContextNodeCallFuncPtr*>(node);

                            if (!callFunc->m_operands.size() || callFunc->m_operands[0]->m_type != TYPE_FUNC_REFNAME) {
                                return; // pointer call
                            }

                            funcRef = dynamic_cast<ASMContextNodeFuncRef*>(callFunc->m_operands[0]);
                        }
                        else if (node->m_type == TYPE_FUNC_REFNAME) {
                            funcRef = dynamic_cast<ASMContextNodeFuncRef*>(node);
                        }
                        else {
                            return;
                        }

                        if (funcRef->m_script) {
                            return; // script call
                        }

                        NameLocated lnames{ funcRef->m_nsp ? funcRef->m_nsp : lname.name_space, funcRef->m_func };

                        auto f = ctx.contextes.find(lnames);

                        if (f == ctx.contextes.end()) {
                            return; // already parsed or builtin?
                        }

                        if (!sctx.devBlockDepth) {
                            f->second.m_devFuncCandidate = false; // called outside a dev block, can't be a canditate
                        }
                        });
                }


                for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                    void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
                    exp->SetHandle(handle);

                    NameLocated lname{ exp->GetNamespace(), exp->GetName() };

                    auto f = ctx.contextes.find(lname);

                    if (f == ctx.contextes.end()) {
                        continue; // already parsed
                    }
                    auto& asmctx = f->second;

                    if (asmctx.m_vtable) {
                        continue;
                    }

                    if (exp->GetNamespace() != currentNSP) {
                        currentNSP = exp->GetNamespace();

                        asmout << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                    }
                    else if (!currentNSP && !i) {
                        asmout << std::endl;
                    }



                    if (asmctx.m_devFuncCandidate) {
                        auto* dvb = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 0);

                        if (dvb->node && dvb->node->m_type == TYPE_BLOCK) {
                            auto* dvbn = dynamic_cast<ASMContextNodeBlock*>(dvb->node);
                            dvbn->m_disabled = true; // disable the dev block 
                        }

                        if (!inDevBlock) {
                            inDevBlock = true;
                            utils::Padding(asmout, currentPadding) << "/#" << std::endl << std::endl;
                            currentPadding++;
                        }

                        {
                            core::async::opt_lock_guard lg{ ctx.gdctx.asyncMtx };

                            asmctx.m_exp.SetHandle(asmctx.m_readerHandle);
                            NameLocated loc{};
                            loc.name = asmctx.m_exp.GetName();
                            loc.name_space = asmctx.m_namespace;

                            ctx.gdctx.exportInfos[loc].devFunc = true;
                        }
                    }
                    else {
                        if (inDevBlock) {
                            currentPadding--;
                            utils::Padding(asmout, currentPadding) << "#/" << std::endl << std::endl;
                            inDevBlock = false;
                        }
                    }
                    DecompContext dctx{ 0, 0, asmctx.m_opt, currentPadding, exp->GetAddress(), ctx.gdbData, ctx.lineBuf };

                    if (asmctx.m_disableDecompiler) {
                        DumpFunctionHeader(*exp, asmout, *scriptfile, ctx, asmctx, currentPadding, nullptr, &currentAnimTree);
                        if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                            dctx.WritePadding(asmout << std::endl);
                        }
                        else {
                            asmout << " ";
                        }
                        asmout << "{" << std::endl;
                        dctx.padding++;
                        dctx.WritePadding(asmout) << "// Can't decompile export " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << asmctx.m_disableDecompilerError << std::endl;
                        dctx.padding--;
                        asmout << "}" << std::endl << std::endl;
                        continue;
                    }

                    DumpFunctionHeader(*exp, asmout, *scriptfile, ctx, asmctx, currentPadding, nullptr, &currentAnimTree);
                    if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                        dctx.WritePadding(asmout << std::endl);
                    }
                    else {
                        asmout << " ";
                    }
                    asmctx.Dump(asmout, dctx);
                    asmout << std::endl;
                }
                if (inDevBlock) {
                    currentPadding--;
                    utils::Padding(asmout, currentPadding) << "#/" << std::endl;
                    inDevBlock = false;
                }
            }
        }

        if (opt.m_generateGdbData && (opt.m_generateGdbBaseData || (ctx.m_unkstrings.size() || ctx.m_devblocks.size() || ctx.m_lazyLinks.size()))) {

            char asmfnamebuffgdb[1000];
            if (opt.m_dbgOutputDir) {
                const char* name = opt.m_noPath ? nullptr : hashutils::ExtractPtr(scriptfile->GetName());

                const char* outDir;
                if (opt.m_splitByVm) {
                    outDir = utils::va("%s/%s", opt.m_dbgOutputDir, tool::gsc::opcode::VMIdFancyName(ctx.m_vmInfo->vmMagic));
                }
                else {
                    outDir = opt.m_dbgOutputDir;
                }

                if (!name) {
                    const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
                    if (actscli::options().heavyHashes) {
                        sprintf_s(asmfnamebuffgdb, "%s/%016llX.%s", outDir, scriptfile->GetName(), fileExt);
                    }
                    else {
                        sprintf_s(asmfnamebuffgdb, "%s/hashed/script/script_%llx.%s", outDir, scriptfile->GetName(), fileExt);
                    }
                }
                else {
                    sprintf_s(asmfnamebuffgdb, "%s/%s", outDir, name);
                }
            }
            else {
                sprintf_s(asmfnamebuffgdb, "%sasm", path);
            }

            std::filesystem::path filedbg(asmfnamebuffgdb);

            std::filesystem::create_directories(filedbg.parent_path());

            const char* gdbFile{ utils::va("%s.gdb", asmfnamebuffgdb) };
            std::ofstream gdbpos{ gdbFile };
            LOG_INFO("Writing GDB into '{}'...", gdbFile);

            if (!gdbpos) {
                LOG_ERROR("Can't open {}", gdbFile);
                return -1;
            }

            // header
            if (opt.m_copyright) {
                gdbpos << "# " << opt.m_copyright << std::endl;
            }

            if (opt.m_header) {
                gdbpos << "# file " << asmfnamebuff << std::endl;
            }

            gdbpos
                << "NAME " << hashutils::ExtractTmpScript(scriptfile->GetName()) << std::endl
                << "VERSION 0" << std::endl
                << "CHECKSUM 0x" << std::hex << scriptfile->GetChecksum() << std::endl
                ;

            // strings
            if (ctx.m_unkstrings.size()) {
                gdbpos
                    << "######################################################" << std::endl
                    << "####################  DEV STRINGS  ###################" << std::endl
                    << "######################################################" << std::endl
                    ;
                for (auto& [val, flocs] : ctx.m_unkstrings) {
                    gdbpos << "#";

                    for (const uint32_t floc : flocs) {
                        gdbpos << " " << ctx.GetFLocName(floc);
                    }

                    gdbpos
                        << std::endl
                        << "STRING \"";

                    utils::PrintFormattedString(gdbpos, val.c_str())
                        << "\""
                        << std::hex
                        ;

                    for (const uint32_t floc : flocs) {
                        gdbpos << " 0x" << floc;
                    }
                    gdbpos << std::endl;
                }
            }
            if (ctx.m_devblocks.size()) {
                gdbpos
                    << "######################################################" << std::endl
                    << "####################  DEV BLOCKS  ####################" << std::endl
                    << "######################################################" << std::endl
                    ;
                for (auto& floc : ctx.m_devblocks) {
                    gdbpos
                        << "# " << ctx.GetFLocName(floc) << std::endl
                        << "DEVBLOCK 0x" << std::hex << floc << std::endl;
                }
            }
            if (ctx.m_lazyLinks.size()) {
                gdbpos
                    << "######################################################" << std::endl
                    << "####################  LAZY LINKS  ####################" << std::endl
                    << "######################################################" << std::endl
                    ;
                for (auto& [val, flocs] : ctx.m_lazyLinks) {
                    gdbpos << "#";

                    for (const uint32_t floc : flocs) {
                        gdbpos << " " << ctx.GetFLocName(floc);
                    }

                    gdbpos
                        << std::endl
                        << "LAZYLINK"
                        " \"" << hashutils::ExtractTmp("namespace", val.name_space) << "\""
                        " \"" << hashutils::ExtractTmp("script", val.script) << "\""
                        " \"" << hashutils::ExtractTmp("function", val.name) << "\""
                        << std::hex
                        ;

                    for (const uint32_t floc : flocs) {
                        gdbpos << " 0x" << floc;
                    }
                    gdbpos << std::endl;
                }
            }


            gdbpos.close();
        }

        {
            core::async::opt_lock_guard lg{ gdctx.asyncMtx };

            if (opt.vtable_dump) {
                for (auto& [n, c] : ctx.m_classes) {
                    auto& t = gdctx.vtables[scriptfile->GetName()][n];
                    for (auto& meth : c.m_vtableMethods) {
                        t.insert(NameLocated{ meth.name_space , meth.name });
                    }
                }

            }
            gdctx.decompiledFiles++;
            if (exportErrors) {
                LOG_ERROR("Found {} error(s)", exportErrors);
            }
        }

        return 0;
    }



    GscInfoOption::GscInfoOption() {
        // set default formatter
        m_formatter = &tool::gsc::formatter::GetDefaultFormatter();
    }

    bool GscInfoOption::Compute(const char** args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            const char* arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
            }
            else if (!strcmp("-g", arg) || !_strcmpi("--gsc", arg)) {
                m_dcomp = true;
            }
            else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
                m_dasm = true;
            }
            else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
                m_header = true;
            }
            else if (!strcmp("-I", arg) || !_strcmpi("--imports", arg)) {
                m_imports = true;
            }
            else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
                m_strings = true;
            }
            else if (!strcmp("-G", arg) || !_strcmpi("--gvars", arg)) {
                m_gvars = true;
            }
            else if (!strcmp("-U", arg) || !_strcmpi("--noincludes", arg)) {
                m_includes = false;
            }
            else if (!strcmp("-X", arg) || !_strcmpi("--exptests", arg)) {
                m_exptests = true;
            }
            else if (!_strcmpi("--nofunc", arg)) {
                m_func = false;
            }
            else if (!_strcmpi("--tokens", arg)) {
                m_tokens = true;
            }
            else if (!strcmp("-F", arg) || !_strcmpi("--nofuncheader", arg)) {
                m_func_header = false;
            }
            else if (!strcmp("-p", arg) || !_strcmpi("--postfunchead", arg)) {
                m_func_header_post = true;
            }
            else if (!strcmp("-l", arg) || !_strcmpi("--rloc", arg)) {
                m_func_rloc = true;
            }
            else if (!strcmp("-L", arg) || !_strcmpi("--floc", arg)) {
                m_func_floc = true;
            }
            else if (!strcmp("-P", arg) || !_strcmpi("--nopatch", arg)) {
                m_patch = false;
            }
            else if (!strcmp("-V", arg) || !_strcmpi("--vars", arg)) {
                m_show_func_vars = true;
            }
            else if (!strcmp("-d", arg) || !_strcmpi("--gdb", arg)) {
                m_generateGdbData = true;
            }
            else if (!_strcmpi("--gdb-small", arg)) {
                m_generateGdbBaseData = false;
            }
            else if (!_strcmpi("--path-output", arg)) {
                m_usePathOutput = true;
            }
            else if (!_strcmpi("--no-usings-sort", arg)) {
                m_noUsingsSort = true;
            }
            else if (!_strcmpi("--no-str-decrypt", arg)) {
                m_noStrDecrypt = true;
            }
            else if (!_strcmpi("--lc", arg)) {
                m_lineCount = true;
            }
            else if (!strcmp("-s", arg) || !_strcmpi("--skip-data", arg)) {
                m_dumpSkipData = true;
            }
            else if (!_strcmpi("--ignore-dbg-plt", arg)) {
                m_ignoreDebugPlatform = true;
            }
            else if (!_strcmpi("--data-dump", arg)) {
                m_dataDump = true;
            }
            else if (!strcmp("-A", arg) || !_strcmpi("--sync", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                const char* mode{ args[++i] };
                if (!_strcmpi("sync", mode)) {
                    m_sync = true;
                }
                else if (!_strcmpi("async", mode)) {
                    m_sync = false;
                }
                else {
                    LOG_ERROR("Bad value for param: {}!", arg);
                    return false;
                }
            }
            else if (!_strcmpi("--vm-split", arg)) {
                m_splitByVm = true;
            }
            else if (!_strcmpi("--test-header", arg)) {
                m_test_header = true;
            }
            else if (!_strcmpi("--vtable", arg)) {
                m_vtable = true;
            }
            else if (!_strcmpi("--debug-hashes", arg)) {
                m_debugHashes = true;
            }
            else if (!_strcmpi("--vtable-dump", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                vtable_dump = args[++i];
            }
            else if (!strcmp("-t", arg) || !_strcmpi("--type", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_platform = PlatformOf(args[++i]);

                if (!m_platform) {
                    LOG_ERROR("Unknown platform: {}!", args[i]);
                    return false;
                }
            }
            else if (!strcmp("-f", arg) || !_strcmpi("--format", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                const formatter::FormatterInfo* fmt{ formatter::GetFromName(args[++i]) };

                if (!fmt) {
                    LOG_ERROR("Unknown formatter: {}!", args[i]);
                    return false;
                }

                m_formatter = fmt;
            }
            else if (!strcmp("-v", arg) || !_strcmpi("--vm", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_vm = VMOf(args[++i]);

                if (!m_vm) {
                    LOG_ERROR("Unknown vm: {}!", args[i]);
                    return false;
                }
            }
            else if (!_strcmpi("--internalblocks", arg)) {
                m_show_internal_blocks = true;
            }
            else if (!_strcmpi("--internalnames", arg)) {
                m_use_internal_names = true;
            }
            else if (!_strcmpi("--jumpdelta", arg)) {
                m_show_jump_delta = true;
            }
            else if (!_strcmpi("--prestruct", arg)) {
                m_show_pre_dump = true;
            }
            else if (!_strcmpi("--markjump", arg)) {
                m_mark_jump_type = true;
            }
            else if (!_strcmpi("--refcount", arg)) {
                m_show_ref_count = true;
            }
            else if (!_strcmpi("--hideop", arg)) {
                m_show_opcode_values = false;
            }
            else if (!_strcmpi("--rawhash", arg)) {
                m_rawhash = true;
            }
            else if (!_strcmpi("--no-path", arg)) {
                m_noPath = true;
            }
            else if (!_strcmpi("--displaystack", arg)) {
                m_display_stack = true;
            }
            else if (!strcmp("-i", arg) || !_strcmpi("--ignore", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }

                for (const char* param = args[++i]; *param; param++) {
                    switch (*param) {
                    case 'd':
                        m_stepskip |= STEPSKIP_DEV;
                        break;
                    case 'D':
                        m_stepskip |= STEPSKIP_DEVBLOCK_INLINE;
                        break;
                    case 's':
                        m_stepskip |= STEPSKIP_SWITCH;
                        break;
                    case 'e':
                        m_stepskip |= STEPSKIP_FOREACH;
                        break;
                    case 'w':
                        m_stepskip |= STEPSKIP_WHILE;
                        break;
                    case 'i':
                        m_stepskip |= STEPSKIP_IF;
                        break;
                    case 'f':
                        m_stepskip |= STEPSKIP_FOR;
                        break;
                    case 'r':
                        m_stepskip |= STEPSKIP_RETURN;
                        break;
                    case 'R':
                        m_stepskip |= STEPSKIP_BOOL_RETURN;
                        break;
                    case 'c':
                        m_stepskip |= STEPSKIP_CLASSMEMBER_INLINE;
                        break;
                    case 'C':
                        m_stepskip |= STEPSKIP_COMPILER_PATTERNS;
                        break;
                    case 'S':
                        m_stepskip |= STEPSKIP_SPECIAL_PATTERN;
                        break;
                    case 'a':
                        m_stepskip = ~0;
                        break;
                    default:
                        LOG_ERROR("Bad param for {}: '{}'!", arg, *param);
                        return false;
                    }
                }
            }
            else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_outputDir = args[++i];
            }
            else if (!strcmp("-O", arg) || !_strcmpi("--output-dbg", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_dbgOutputDir = args[++i];
            }
            else if (!_strcmpi("--input-dbg", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_dbgInputDir = args[++i];
            }
            else if (!_strcmpi("--gdb-dump", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_gdbZipOutputFile = args[++i];
            }
            else if (!strcmp("-m", arg) || !_strcmpi("--hashmap", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_dump_hashmap = args[++i];
            }
            else if (!_strcmpi("--dumpstrings", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_dump_strings = args[++i];
            }
            else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_copyright = args[++i];
            }
            else if (*arg == '-') {
                LOG_ERROR("Unknown option: {}!", arg);
                return false;
            }
            else {
                m_inputFiles.push_back(arg);
            }
        }
        if (!m_inputFiles.size()) {
            //m_inputFiles.push_back("scriptparsetree");
            LOG_ERROR("No input param");
            return false; // missing param
        }
        if (!m_dbgOutputDir) {
            m_dbgOutputDir = m_outputDir;
        }
        if (m_func && !(m_dcomp || m_dasm)) {
            m_dcomp = true; // produce gsc by default
        }

        return true;
    }

    void GscInfoOption::PrintHelp() {
        LOG_INFO("> gscd [options] (input)");
        LOG_INFO("-h --help          : Print help");
        LOG_INFO("-g --gsc           : Produce GSC");
        LOG_INFO("-a --asm           : Produce ASM");
        LOG_INFO("-t --type [t]      : Set type, default PC, values: 'ps', 'xbox', 'pc', 'pc_alpha'");
        LOG_INFO("-o --output [d]    : ASM/GSC output dir, default same.gscasm");
        LOG_INFO("-O --output-dbg [d]: DBG output dir, default same as --output");
        LOG_INFO("--input-dbg [d]    : DBG input directory, default to none");
        LOG_INFO("-v --vm            : Only decompile a particular vm");
        LOG_INFO("-H --header        : Write file header");
        LOG_INFO("-m --hashmap [f]   : Write hashmap in a file f");
        {
            std::ostringstream formats;

            for (const auto* fmt : formatter::GetFormatters()) {

                formats << " '" << fmt->name << "'";
            }

            LOG_INFO("-f --format [f]    : Use formatter, values:{}, default: '{}'", formats.str(), formatter::GetDefaultFormatter().name);
        }
        LOG_INFO("-l --rloc          : Write relative location of the function code");
        LOG_INFO("-L --floc          : Write file location of the function code");
        LOG_INFO("-C --copyright [t] : Set a comment text to put in front of every file");
        LOG_INFO("-d --gdb           : Dump gdb data");
        LOG_INFO("--gdb-small        : Dump only important gdb data");
        LOG_INFO("--dumpstrings [f]  : Dump strings into f");
        LOG_INFO("--vtable-dump [f]  : Dump vtable information into f");
        LOG_INFO("--path-output      : Use the path for the output name");
        LOG_INFO("-s --skip-data     : Dump skip data (gscbin VMs only), requires --path-output");
        // it's not that I don't want them to be known, it's just to avoid having too many of them in the help
        // it's mostly dev tools
        LOG_DEBUG("-G --gvars         : Write gvars");
        LOG_DEBUG("-U --noincludes    : No includes");
        LOG_DEBUG("-X --exptests      : Enable UNK tests");
        LOG_DEBUG("-V --vars          : Show all func vars");
        LOG_DEBUG("-F --nofuncheader  : No function header");
        LOG_DEBUG("--nofunc           : No function write");
        LOG_DEBUG("-p --postfunchead  : Write post function header in ASM mode");
        LOG_DEBUG("-I --imports       : Write imports");
        LOG_DEBUG("-S --strings       : Write strings");
        LOG_DEBUG("--test-header      : Write test header");
        LOG_DEBUG("--internalblocks   : Show internal blocks ");
        LOG_DEBUG("--jumpdelta        : Show jump delta");
        LOG_DEBUG("--hideop           : Hide opcode values in disassembly");
        LOG_DEBUG("--prestruct        : Show prestruct");
        LOG_DEBUG("--refcount         : Show ref count");
        LOG_DEBUG("--markjump         : Show jump type");
        LOG_DEBUG("--displaystack     : Display stack in disassembly");
        LOG_DEBUG("--vm-split         : Split by VM in the output");
        LOG_DEBUG("--internalnames    : Print asm nodes internal names");
        LOG_DEBUG("--rawhash          : Add raw hashes to export dump");
        LOG_DEBUG("--no-path          : No path extraction");
        LOG_DEBUG("--no-usings-sort   : No usings sort");
        LOG_DEBUG("--no-str-decrypt   : No string decrypt");
        LOG_DEBUG("--ignore-dbg-plt   : ignore debug platform info");
        LOG_DEBUG("-A --sync [mode]   : Sync mode: async or sync");
        LOG_DEBUG("--vtable           : Do not hide and decompile vtable functions");
        LOG_DEBUG("--debug-hashes     : Debug hash alogrithm");
        LOG_DEBUG("--data-dump        : Dump data in asm");
        LOG_DEBUG("-i --ignore[t + ]  : ignore step : ");
        LOG_DEBUG("                     a : all, d: devblocks, s : switch, e : foreach, w : while, i : if, f : for, r : return");
        LOG_DEBUG("                     R : bool return, c: class members, D: devblocks inline, S : special patterns");
    }

    static bool ReadDbgFile(const std::filesystem::path& path, std::string& buffer) {
        std::filesystem::path dbgPath{ path };
        std::string ext{ path.extension().string() };

        auto TestLoad = [&buffer, &path](const char* ext) -> bool {
            std::filesystem::path dbgPath{ path };
            dbgPath.replace_extension(ext);
            if (!utils::ReadFile(dbgPath, buffer)) {
                return false;
            }

            LOG_INFO("Loaded debug data {}", dbgPath.string());
            return true;
            };

        if (ext == ".gscc") {
            if (TestLoad(".gsc.gdb") || TestLoad(".gscgdbc")) return true;
        }
        else if (ext == ".cscc") {
            if (TestLoad(".csc.gdb") || TestLoad(".cscgdbc")) return true;
        }
        else if (ext == ".gscbin") {
            if (TestLoad(".gscbin.gdb") || TestLoad(".gscbingdbc")) return true;
        }
        else if (ext == ".gshc") {
            if (TestLoad(".gsh.gdb") || TestLoad(".gshgdbc")) return true;
        }
        else if (ext == ".cshc") {
            if (TestLoad(".csh.gdb") || TestLoad(".cshgdbc")) return true;
        }
        return false;
    }

    int gscinfo(int argc, const char* argv[]) {
        GscDecompilerGlobalContext gdctx{};

        if (!gdctx.opt.Compute(argv, 2, argc) || gdctx.opt.m_help) {
            gdctx.opt.PrintHelp();
            return 0;
        }

        if (gdctx.opt.m_dbgInputDir) {
            std::vector<std::filesystem::path> dbgs{};

            utils::GetFileRecurse(gdctx.opt.m_dbgInputDir, dbgs, [](const std::filesystem::path& path) -> bool {
                auto str = path.string();
                return str.ends_with(".gdb");
                });

            for (const std::filesystem::path& dbg : dbgs) {
                tool::gsc::gdb::ACTS_GSC_GDB* gdb = gdctx.alloc.New<tool::gsc::gdb::ACTS_GSC_GDB>(dbg);

                LOG_DEBUG("Loading GDB file {}", dbg.string());

                if (!gdb->ReadFrom(dbg)) {
                    gdctx.alloc.Free(gdb);
                    continue;
                }

                tool::gsc::gdb::ACTS_GSC_GDB*& ref = gdctx.debugObjects[gdb->name];
                if (ref) {
                    LOG_WARNING("Debug object '{}' already defined", gdb->nameStr);
                    gdctx.alloc.Free(gdb);
                    continue; // already defined
                }

                ref = gdb;
            }

            LOG_INFO("{} gdb file(s) loaded", gdctx.debugObjects.size());
        }

        std::unordered_set<std::string> dumpStringsStore{};
        if (gdctx.opt.m_dump_strings) {
            gdctx.dumpStringsStore = &dumpStringsStore;
        }


        const char* globalHM = actscli::options().dumpHashmap;
        if (!globalHM && gdctx.opt.m_dump_hashmap != nullptr) {
            // keep the option for backward compatibility
            hashutils::SaveExtracted(true, false);
        }
        bool computed{};
        int ret{ tool::OK };
        struct FileOrigin {
            std::filesystem::path base;
            bool dir{};
            std::vector<std::filesystem::path> scriptFiles{};
        };
        std::vector<FileOrigin> scriptFiles{};
        for (const auto& file : gdctx.opt.m_inputFiles) {
            scriptFiles.emplace_back(file);
            FileOrigin& orFi{ scriptFiles[scriptFiles.size() - 1] };
            if (std::filesystem::is_directory(orFi.base)) {
                utils::GetFileRecurse(orFi.base, orFi.scriptFiles, [](const std::filesystem::path& path) -> bool {
                    std::string pathname = path.string();

                    return pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
                        || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin") // xensik GSCBIN format
                        || pathname.ends_with(".gshc") || pathname.ends_with(".cshc")
                        || pathname.ends_with(".gsic") || pathname.ends_with(".csic") // Serious GSIC format
                        ;
                    }, true);
                orFi.dir = true;
            }
            else {
                orFi.scriptFiles.emplace_back(orFi.base.filename());
            }
        }

        if (gdctx.opt.m_sync) {
            std::string buffer{};
            void* bufferAlign{};
            size_t size{};
            for (FileOrigin& pathLoc : scriptFiles) {
                for (std::filesystem::path pathRel : pathLoc.scriptFiles) {
                    std::filesystem::path path{ pathLoc.dir ? pathLoc.base / pathRel : pathLoc.base };
                    LOG_DEBUG("Reading {} ({})", path.string(), pathRel.string());

                    if (!utils::ReadFileAlign(path, buffer, bufferAlign, size)) {
                        LOG_ERROR("Can't read file data for {}", path.string());
                        continue;
                    }
                    std::string dbgBuffer;

                    if (!ReadDbgFile(path, dbgBuffer)) {
                        dbgBuffer = {};
                    }

                    try {
                        auto lret = DecompileGsc((byte*)bufferAlign, size, pathRel, gdctx, (byte*)dbgBuffer.data(), dbgBuffer.size());
                        if (lret != tool::OK) {
                            ret = lret;
                        }
                    }
                    catch (std::runtime_error& e) {
                        LOG_ERROR("Exception when reading {}: {}", path.string(), e.what());
                        ret = tool::BASIC_ERROR;
                    }
                }
            }
        }
        else {
            LOG_WARNING("Using experimental async mode");
            uint64_t prevAsyncTypes = core::async::GetAsyncTypes();
            core::async::SetAsync(core::async::AT_ALL);

            std::mutex mtx{};
            gdctx.asyncMtx = &mtx;

            BS::thread_pool pool{};

            for (FileOrigin& pathLoc : scriptFiles) {
                for (std::filesystem::path pathRel : pathLoc.scriptFiles) {
                    std::filesystem::path path{ pathLoc.dir ? pathLoc.base / pathRel : pathLoc.base };
                    pool.detach_task([path, pathRel, &mtx, &ret, &gdctx] {
                        std::string buffer{};
                        void* bufferAlign{};
                        size_t size{};

                        LOG_DEBUG("Reading {} ({})", path.string(), pathRel.string());
                        int lret;
                        if (!utils::ReadFileAlign(path, buffer, bufferAlign, size)) {
                            lret = tool::BASIC_ERROR;
                        }
                        else if (size < 0x8) { // MAGIC (8) or GSC\0 + 3 * uint32_t
                            LOG_ERROR("Invalid header, file size: {:x}/{:x} for {}", size, 0x8, path.string());
                            lret = tool::BASIC_ERROR;
                        }
                        else {
                            lret = DecompileGsc((byte*)bufferAlign, size, pathRel, gdctx);
                        }

                        if (lret != tool::OK) {
                            std::lock_guard lg{ mtx };
                            ret = lret;
                        }
                        });
                }
            }

            pool.wait();

            core::async::SetAsync(prevAsyncTypes);
        }


        if (gdctx.hardErrors) {
            LOG_ERROR("{} (0x{:x}) error(s), are you using the right vm type?", gdctx.hardErrors, gdctx.hardErrors);
        }
        LOG_INFO("{} (0x{:x}) file(s) decompiled.", gdctx.decompiledFiles, gdctx.decompiledFiles);

        if (!globalHM) {
            hashutils::WriteExtracted(gdctx.opt.m_dump_hashmap);
        }

        if (gdctx.opt.m_dump_strings) {
            std::ofstream os{ gdctx.opt.m_dump_strings };
            if (!os) {
                LOG_ERROR("Can't open string output");
            }
            else {
                for (const auto& str : dumpStringsStore) {
                    os << str << std::endl;
                }
                os.close();
            }
        }
        if (gdctx.opt.vtable_dump) {
            std::ofstream os{ gdctx.opt.vtable_dump };
            if (!os) {
                LOG_ERROR("Can't open vtable output");
            }
            else {
                os << "script,class,namespace,name";
                for (auto& [script, clss] : gdctx.vtables) {
                    for (auto& [cls, ns] : clss) {
                        for (auto& n : ns) {
                            os
                                << std::endl
                                << hashutils::ExtractTmp("script", script) << ","
                                << hashutils::ExtractTmp("class", cls) << ","
                                << hashutils::ExtractTmp("namespace", n.name_space) << ","
                                << hashutils::ExtractTmp("function", n.name)
                                ;
                        }
                    }
                }


                os.close();
                LOG_INFO("btable into '{}'", gdctx.opt.vtable_dump);
            }

        }

        if (gdctx.opt.m_gdbZipOutputFile) {
            std::filesystem::path zip{ gdctx.opt.m_gdbZipOutputFile };
            miniz_cpp::zip_file zf{};

            bool any{};
            for (auto& [fn, gdb] : gdctx.gdbData) {
                if (!gdb->gdb) continue;
                tool::gsc::vm::GscGdb* reader{ tool::gsc::vm::GetGdbReader(gdb->gdb) };
                if (!reader) continue; // can't find reader
                std::string bytes{};
                bool r{ reader->DbgSave(gdb, bytes) };
                char* n{ utils::va("%s.gdb", hashutils::ExtractTmp("script", fn)) };
                n = utils::MapString(n, [](char c) -> char { return c == '\\' ? '/' : c; });
                if (!r) {
                    LOG_ERROR("Can't compile gdb file {}", n);
                    continue;
                }
                LOG_DEBUG("write {}: {} bytes", n, bytes.size());
                zf.writestr(n, bytes);
                any = true;
            }
            if (any) {
                zf.save(zip.string());
                LOG_INFO("gdb data saved into {}", zip.string());
            }
            else {
                LOG_WARNING("No gdb data generated");
            }
        }

        LOG_INFO("done.");
        return ret;
    }

    ADD_TOOL(gscinfo, "gsc", "", "GSC decompiler/disassembler", gscinfo);
    ADD_TOOL(gscd, "gsc", "", "GSC decompiler/disassembler", gscinfo);
}