#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/gsc_opcodes_load.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include <tools/gsc_decompiler.hpp>

namespace {

    class GscFFHandler : public fastfile::FFHandler {
        size_t count{};
        bool anyDbg{};
    public:
        GscFFHandler() : fastfile::FFHandler("GSC", "Dump gsc scripts") {
        }
        void Init(fastfile::FastFileOption& opt) override {
            count = 0;
            anyDbg = false;
        }

        void Cleanup() override {
            if (count) LOG_INFO("{} file(s) dumped", count);
            if (anyDbg) LOG_WARNING("Debug file found");
        }

        void DecompFile(fastfile::FastFileOption& opt, byte* file, size_t len, const std::filesystem::path& path) {
            if (opt.disableScriptsDecomp) return; // nothing to decomp
            tool::gsc::GscDecompilerGlobalContext gdctx{};

            gdctx.opt.m_platform = fastfile::GetCurrentContext().gscPlatform;
            gdctx.opt.m_dcomp = true;
            gdctx.opt.m_splitByVm = true;
            std::filesystem::path outSource{ opt.m_output / "gsc_decomp" };
            std::string outSourceStr{ outSource.string() };
            gdctx.opt.m_outputDir = outSourceStr.data();

            std::vector<byte> scriptData;
            byte* alignedAlloc{ utils::Aligned(file, 0x20) };

            if (file != alignedAlloc) {
                // we clone the buffer to align it
                utils::Allocate(scriptData, len + 0x20);
                alignedAlloc = utils::Aligned(scriptData.data(), 0x20);
                std::memcpy(alignedAlloc, file, len);
            }

            int r{ tool::gsc::DecompileGsc(alignedAlloc, len, path, gdctx) };

            if (r) {
                LOG_ERROR("Error when decompiling script {}", path.string());
            }
        }

        bool HandleScriptFile(fastfile::FastFileOption & opt, core::bytebuffer::ByteBuffer & buff, fastfile::FastFileContext & ctx) {
            struct ScriptFile {
                uint64_t name;
                int32_t compressedLen;
                int32_t len;
                int32_t bytecodeLen;
                uintptr_t buffer;
                uintptr_t bytecode;
            };
            static_assert(sizeof(ScriptFile) == 0x28);

            ScriptFile sfSearch{};
            ScriptFile sfMask{};

            // use -2 to match both types
            sfSearch.buffer = 0xFFFFFFFFFFFFFFFE;
            sfMask.buffer = 0xFFFFFFFFFFFFFFFE;
            sfSearch.bytecode = 0xFFFFFFFFFFFFFFFE;
            sfMask.bytecode = 0xFFFFFFFFFFFFFFFE;

            std::filesystem::path outDir{ opt.m_output / "scriptfile" };

            bool any{};
            size_t off{};
            while (true) {
                buff.Goto(off);
                if (buff.CanRead(8)) buff.Skip(8);

                if ((off = buff.FindMasked(&sfSearch, &sfMask, sizeof(sfSearch))) == std::string::npos) {
                    break;
                }
                buff.Goto(off);
                off++;
                ScriptFile& sf{ *buff.ReadPtr<ScriptFile>() };

                if (sf.name < 0x1000000000 || sf.compressedLen <= 0 || sf.len <= 0 || sf.bytecodeLen <= 0
                    || !buff.CanRead((size_t)sf.compressedLen + sf.bytecodeLen)) {
                    continue; // bad header
                }

                const char* sfname;
                if (sf.name == 0xFFFFFFFFFFFFFFFF) {
                    // xstring?
                    sfname = buff.ReadString();
                }
                else {
                    // xhash?
                    sfname = utils::va("script_%llx", sf.name);
                }

                compatibility::xensik::gscbin::GscBinHeader header{};

                header.Magic() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
                header.bytecodeLen = sf.bytecodeLen;
                header.compressedLen = sf.compressedLen;
                header.len = sf.len;
                byte* compressedData{ buff.ReadPtr<byte>((size_t)sf.compressedLen) };
                byte* bytecodeData{ buff.ReadPtr<byte>((size_t)sf.bytecodeLen) };

                if (*compressedData != 0x78) {
                    continue; // the compressed data is zlib compressed, which is 99% of the time described by the magic 78 DA
                }

                std::filesystem::path outFile{ outDir / sfname };
                outFile.replace_extension(".gscbin");
                std::filesystem::create_directories(outFile.parent_path());
                {
                    utils::OutFileCE os{ outFile, false, std::ios::binary };
                    if (!os) {
                        LOG_ERROR("Error when dumping {}", outFile.string());
                        continue;
                    }


                    utils::WriteValue(os, &header, sizeof(header));
                    utils::WriteValue(os, compressedData, sf.compressedLen);
                    utils::WriteValue(os, bytecodeData, sf.bytecodeLen);
                }

                LOG_INFO("Dump {}", outFile.string());
                any = true;
            }
            return any;
        }

        bool HandleScriptParseTree(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) {
            bool any{};

            { // spt string search
                struct ScriptParseTree {
                    int64_t name;
                    int32_t len;
                    int32_t pad;
                    int64_t buffer;
                }; static_assert(sizeof(ScriptParseTree) == 0x18);
                // dump spt dbg
                ScriptParseTree sptSearch{};
                ScriptParseTree sptMask{};
                memset(&sptMask, 0xff, sizeof(sptMask));
                sptMask.len = 0;
                sptSearch.buffer = 0xFFFFFFFFFFFFFFFF;
                sptSearch.name = 0xFFFFFFFFFFFFFFFF;

                std::filesystem::path outDir{ opt.m_output / "spt" };

                size_t off{};
                while (true) {
                    if ((off = buff.FindMasked(&sptSearch, &sptMask, sizeof(sptSearch))) == std::string::npos) {
                        break;
                    }
                    buff.Goto(off);
                    off++;
                    ScriptParseTree& spt{ *buff.ReadPtr<ScriptParseTree>() };

                    const char* name{ buff.ReadString() };

                    std::string_view sw{ name };
                    if (spt.len < 8 || !(sw.ends_with(".gsc") || sw.ends_with(".csc") || sw.ends_with(".gsh") || sw.ends_with(".csh"))) {
                        continue; // not a candidate
                    }


                    LOG_TRACE("spt {} -> 0x{:x}", name, spt.len);

                    if (!buff.CanRead(spt.len)) {
                        continue; // bad size
                    }
                    byte* data{ buff.ReadPtr<byte>(spt.len + 1) };

                    if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_TRE_BASE) {
                        if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_DBG_TRE_BASE) {
                            //LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data); // not a gscc file
                        }
                        continue; // bad magic
                    }

                    std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}c", *(uint64_t*)data, name) };
                    std::filesystem::create_directories(outFile.parent_path());
                    if (!utils::WriteFile(outFile, data, spt.len)) {
                        LOG_ERROR("Error when dumping");
                    }
                    else {
                        LOG_INFO("Dump into {}", outFile.string());
                    }
                    any = true;
                    DecompFile(opt, data, spt.len, outFile);
                }
            }
            {
                struct ScriptParseTreeDBG
                {
                    uintptr_t name;
                    int gdbLen;
                    int srcLen;
                    uintptr_t gdb;
                    uintptr_t src;
                }; static_assert(sizeof(ScriptParseTreeDBG) == 0x20);

                // dump spt
                ScriptParseTreeDBG sptSearch{};
                ScriptParseTreeDBG sptMask{};
                memset(&sptMask, 0xff, sizeof(sptMask));
                sptMask.gdbLen = 0;
                sptMask.srcLen = 0;
                sptSearch.name = 0xFFFFFFFFFFFFFFFF;
                sptSearch.gdb = 0xFFFFFFFFFFFFFFFF;
                sptSearch.src = 0xFFFFFFFFFFFFFFFF;

                std::filesystem::path outDir{ opt.m_output / "gsc" };

                size_t off{};
                while (true) {
                    if ((off = buff.FindMasked(&sptSearch, &sptMask, sizeof(sptSearch))) == std::string::npos) {
                        break;
                    }
                    buff.Goto(off);
                    off++;
                    ScriptParseTreeDBG& spt{ *buff.ReadPtr<ScriptParseTreeDBG>() };

                    const char* name{ buff.ReadString() };

                    std::string_view sw{ name };
                    if (spt.gdbLen < 8 || !(sw.ends_with(".gsc") || sw.ends_with(".csc") || sw.ends_with(".gsh") || sw.ends_with(".csh"))) {
                        continue; // not a candidate
                    }


                    LOG_INFO("dbg {} -> 0x{:x}", name, spt.gdbLen);

                    if (!buff.CanRead(spt.gdbLen)) {
                        continue; // bad size
                    }
                    byte* data{ buff.ReadPtr<byte>(spt.gdbLen + 1) };

                    if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_DBG_TRE_BASE) {
                        // bad magic
                        if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != tool::gsc::opcode::VMI_TRE_BASE) {
                            //LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data); // not a dbg file
                        }
                        continue;
                    }
                    {
                        std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}.gdb", *(uint64_t*)data, name) };
                        std::filesystem::create_directories(outFile.parent_path());
                        if (!utils::WriteFile(outFile, data, spt.gdbLen)) {
                            LOG_ERROR("Error when dumping");
                        }
                        else {
                            LOG_INFO("Dump into {}", outFile.string());
                        }
                    }

                    if (!buff.CanRead(spt.srcLen)) {
                        continue; // bad size
                    }
                    {
                        byte* datasrc{ buff.ReadPtr<byte>(spt.srcLen + 1) };

                        std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}", *(uint64_t*)data, name) };
                        std::filesystem::create_directories(outFile.parent_path());
                        if (!utils::WriteFile(outFile, datasrc, spt.srcLen)) {
                            LOG_ERROR("Error when dumping");
                        }
                        else {
                            LOG_INFO("Dump into {}", outFile.string());
                        }
                        any = true;
                    }
                }
            }
            if (any) return true; // found

            // search scriptparsetreedbg
            uint64_t dbgMagic{ 0xA0D42444780 };
            uint64_t dbgMagicMask{ 0xFFFFFFFFFFFFF };
            if (buff.FindMasked(&dbgMagic, &dbgMagicMask, sizeof(dbgMagic)) != std::string::npos) {
                LOG_WARNING("FIND A DBG SPT");
                anyDbg = true;
            }

            // search scriptparsetree
            {
                uint64_t magic{ tool::gsc::opcode::VMI_TRE_BASE };
                uint64_t magicMask{ 0x00FFFFFFFFFFFFFF };

                size_t loc{};
                std::filesystem::path out{ opt.m_output / "gsc" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };
                while (true) {
                    loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
                    if (loc == std::string::npos) break;
                    struct T8GSCOBJ {
                        byte magic[8];
                        int32_t crc;
                        int32_t pad;
                        uint64_t name;
                    };

                    buff.Goto(loc);

                    if (!buff.CanRead(sizeof(T8GSCOBJ))) {
                        break; // can't read buffer
                    }

                    byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

                    T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

                    uint64_t name{ obj->name };

                    while (*(uint64_t*)sptCan != name) {
                        sptCan--;
                        if (start == sptCan) {
                            break;
                        }
                    }
                    if (start == sptCan) {
                        loc++;
                        continue;
                    }


                    uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

                    size_t size;

                    struct T8SPT {
                        uint64_t name;
                        uint64_t pad0;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };

                    struct T8SPTOld {
                        uint64_t name;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };

                    // Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
                    // to the buffer will constantly be -1 because it is not linked yet
                    if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPT*)sptCan)->size & 0x7FFFFFFF;
                    }
                    else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPTOld*)sptCan)->size & 0x7FFFFFFF;
                    }
                    else {
                        LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
                        loc++;
                        continue;
                    }

                    LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


                    if (!buff.CanRead(size)) {
                        loc++;
                        LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
                        continue;
                    }
                    buff.Skip(size);

                    std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
                    std::filesystem::create_directories(outFile.parent_path());

                    if (!utils::WriteFile(outFile, obj->magic, size)) {
                        LOG_ERROR("Can't write {}", outFile.string());
                    }
                    else {
                        LOG_INFO("Dump {} ({})", outFile.string(), hashutils::ExtractTmpScript(obj->name));
                        count++;
                    }
                    any = true;
                    DecompFile(opt, (byte*)obj, size, outFile);

                    loc++;
                }

            }
            // search gscobj
            {
                uint64_t magic{ tool::gsc::opcode::VMI_IW_BASE };
                uint64_t magicMask{ 0xFFFFFFFFFFFFFF00 };

                size_t loc{};
                std::filesystem::path out{ opt.m_output / "gsc" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };
                while (true) {
                    loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
                    if (loc == std::string::npos) break;
                    struct GscObjEntry {
                        uint64_t name;
                        uint32_t len;
                        void* obj;
                    };

                    buff.Goto(loc);
                    uint64_t smagic{ buff.Read<uint64_t>() };

                    if (loc < sizeof(GscObjEntry)) {
                        continue; // not enough data
                    }
                    buff.Goto(loc - sizeof(GscObjEntry));

                    GscObjEntry* entry{ buff.ReadPtr<GscObjEntry>() };

                    if (!entry->name) {
                        LOG_ERROR("0x{:x} INVALID {}/{} for 0x{:x}", smagic, entry->obj, hashutils::ExtractTmpScript(entry->name), loc);
                        continue;
                    }


                    if (!buff.CanRead(entry->len)) {
                        loc++;
                        LOG_ERROR("Bad size 0x{:x} for loc 0x{:x}", entry->len, loc);
                        continue;
                    }

                    LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, entry->len, hashutils::ExtractTmpScript(entry->name));

                    byte* obj{ buff.ReadPtr<byte>(entry->len) };

                    std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, entry->name) };
                    std::filesystem::create_directories(outFile.parent_path());

                    if (!utils::WriteFile(outFile, obj, entry->len)) {
                        LOG_ERROR("Can't write {}", outFile.string());
                    }
                    else {
                        LOG_INFO("Dump {} ({})", outFile.string(), hashutils::ExtractTmpScript(entry->name));
                        count++;
                    }

                    any = true;
                    DecompFile(opt, obj, entry->len, outFile);

                    loc++;
                }

            }
            return any;
        }

        void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) override {
            if (HandleScriptParseTree(opt, buff, ctx)) return;
            if (HandleScriptFile(opt, buff, ctx)) return;
        }
    };

    utils::ArrayAdder<GscFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}