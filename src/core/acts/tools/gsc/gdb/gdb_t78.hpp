#pragma once
#include <core/async.hpp>
#include <core/updater.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <gsc/gsc_acts_debug.hpp>

namespace tool::gsc::vm {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;
    using namespace tool::gsc::gdb;

    consteval std::array<char, 7> MakeGscGdbT78Name(uint64_t GDB_MAGIC) {
        constexpr const char* hex = "0123456789abcdef";
        return { 't', '7', '8', '_', hex[(GDB_MAGIC >> 60) & 15], hex[(GDB_MAGIC >> 56) & 15], '\0' };
    }

    template<uint64_t GDB_MAGIC>
    class GscGdbT78 : public GscGdb {
        static constexpr auto name = MakeGscGdbT78Name(GDB_MAGIC);

      public:
        GscGdbT78() : GscGdb(GDB_MAGIC, name.data()) {}

        void DbgLoad(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout) override {
            GSC_GDB* dbg{ dbgReader.ReadPtr<GSC_GDB>() };

            asmout << "// GDB compiled file, file version 0x" << std::hex << (int)dbg->version << ", magic: 0x"
                   << (*(uint64_t*)&dbg->magic[0]) << "\n";

            asmout << "// dbg crc .. " << "0x" << std::hex << dbg->source_crc << "\n";
            if (ctx.scriptfile->GetChecksum() && ctx.scriptfile->GetChecksum() != dbg->source_crc) {
                LOG_WARNING(
                    "Can't use dbg data: unmatching checksums: 0x{:x} != 0x{:x}, magic: 0x{:x}",
                    ctx.scriptfile->GetChecksum(),
                    dbg->source_crc,
                    *(uint64_t*)dbg
                );
                return;
            }

            asmout << "// strings .. " << std::dec << dbg->stringtable_count << " (offset: 0x" << std::hex
                   << dbg->stringtable_offset << ")\n";
            if (dbg->stringtable_count) {
                dbgReader.Goto(dbg->stringtable_offset);
                for (size_t i = 0; i < dbg->stringtable_count; i++) {

                    const char* str = dbgReader.ReadString();

                    uint64_t hashField{ ctx.m_vmInfo->HashField(str) };
                    uint64_t hashFilePath{ ctx.m_vmInfo->HashFilePath(str) };
                    uint64_t hashPath{ ctx.m_vmInfo->HashPath(str) };
                    {
                        core::async::opt_lock_guard hlg{ hashutils::GetMutex(false) };
                        hashutils::AddPrecomputed(hashField, str, true);
                        hashutils::AddPrecomputed(hashFilePath, str, true);
                        hashutils::AddPrecomputed(hashPath, str, true);

                        if (ctx.opt.m_header) {
                            utils::PrintFormattedString(asmout << "// - #\"", str)
                                << "\" (0x" << std::hex << hashField << "/0x" << hashFilePath << "/0x" << hashPath;
                        }
                        // use all the known hashes for this VM
                        for (auto& [k, func] : ctx.m_vmInfo->hashesFunc) {
                            try {
                                int64_t hash = func.hashFunc(str);

                                if (hash) {
                                    if (ctx.opt.m_header) {
                                        asmout << "/" << k << '=' << std::hex << hash;
                                    }
                                    hashutils::AddPrecomputed(hash, str, true);
                                }
                            } catch (std::exception&) {
                                // ignore
                            }
                        }
                    }
                    if (ctx.opt.m_header) {
                        asmout << ")\n";
                    }
                }
            }

            // not used by acts decompiler, but can be useful for a vm
            if (ctx.opt.m_header) {
                asmout << "// lines .... " << std::dec << dbg->lineinfo_count << " (offset: 0x" << std::hex
                       << dbg->lineinfo_offset << ")\n";

                dbgReader.Goto(dbg->lineinfo_offset);
                for (size_t i = 1; i <= dbg->lineinfo_count; i++) {
                    GSC_LINEINFO* linesOff = dbgReader.ReadPtr<GSC_LINEINFO>();
                    asmout << "// - " << std::dec << i << " " << ctx.GetFLocName(linesOff->offset) << "\n";
                }
            }
        }

        bool DbgSave(GscDecompilerGDBData* gdb, std::string& buffer) override {
            std::vector<byte> data{};

            utils::Allocate<GSC_GDB>(data);

            size_t devStringTable{};
            size_t devStringsCount{ gdb->devStringsLocations.size() };

            // know the size for the devstring table
            if (devStringsCount) {
                // sort the strings to have it easy
                std::sort(
                    gdb->devStringsLocations.begin(),
                    gdb->devStringsLocations.end(),
                    [](GscDecompilerGDBDataDevString& a, GscDecompilerGDBDataDevString& b) { return a.rloc < b.rloc; }
                );

                devStringTable = gdb->devStringsLocations[0].rloc;

                if (data.size() > devStringTable) {
                    LOG_ERROR("Dev string in header");
                    return false;
                }

                size_t currentIndex{ devStringTable };
                for (const auto& [floc, string] : gdb->devStringsLocations) {
                    if (floc < currentIndex) {
                        LOG_ERROR("Invalid string ref at 0x{:x}, previous location was 0x{:x}", floc, currentIndex);
                        return false;
                    }

                    size_t len{ (string ? std::strlen(string) : 0) + 1 };
                    currentIndex = floc + len;
                    data.resize(currentIndex);

                    if (string) {
                        // write string if we have it
                        std::memcpy(&data[floc], string, len);
                    }
                }
            }

            // line info are 8 bytes aligned
            utils::Aligned<uint64_t>(data);
            size_t linesInfo{ data.size() };
            size_t linesCount{};
            uint32_t lastOffset{};

            if (!gdb->lineInfos.empty()) {
                auto [cl, off] = *gdb->lineInfos.begin();
                lastOffset = off;
            }

            // write the obtained lines
            for (auto& [cl, off] : gdb->lineInfos) {
                while (linesCount < cl) {
                    GSC_LINEINFO& line{ utils::Allocate<GSC_LINEINFO>(data) };
                    line.offset = lastOffset;
                    linesCount++;
                }
                if (off > lastOffset) {
                    // keep cumulative offsets for the game
                    lastOffset = off;
                }
                GSC_LINEINFO& line{ utils::Allocate<GSC_LINEINFO>(data) };
                line.offset = lastOffset;
                linesCount++;
            }

            // write header data
            GSC_GDB* header{ (GSC_GDB*)data.data() };
            *(uint64_t*)header = gdb->gdb;
            header->source_crc = gdb->checksum;
            header->version = 0;
            header->devblock_stringtable_offset = (uint32_t)devStringTable;
            header->devblock_stringtable_count = (uint32_t)devStringsCount;
            header->lineinfo_offset = (uint32_t)linesInfo;
            header->lineinfo_count = (uint32_t)linesCount;
            // not implemented
            header->stringtable_offset = (uint32_t)data.size();
            header->stringtable_count = (uint32_t)0;

            buffer.resize(data.size());
            std::memcpy(buffer.data(), data.data(), data.size());
            return true;
        }
    };
} // namespace tool::gsc::vm