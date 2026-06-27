#pragma once
#include <core/actsinfo.hpp>
#include <core/async.hpp>
#include <core/updater.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <gsc/gsc_acts_debug.hpp>

namespace tool::gsc::vm {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;
    using namespace tool::gsc::gdb;

    class GscGdbActs : public GscGdb {
      public:
        GscGdbActs() : GscGdb(shared::gsc::acts_debug::MAGIC, "acts") {}

        void DbgLoad(
            GscDecompilerGDBData& gdb, vm::VmInfo* vm, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout,
            bool printInfo
        ) override {
            using namespace shared::gsc::acts_debug;
            // acts compiled file, read data
            GSC_ACTS_DEBUG* dbg = dbgReader.Ptr<GSC_ACTS_DEBUG>();
            LOG_TRACE("Reading ACTS debug data v{:x}", (int)dbg->version);
            asmout << "// ACTS compiled file, file version 0x" << std::hex << (int)dbg->version << ", acts version ";

            asmout << core::updater::GetVersionName((uint32_t)dbg->actsVersion);

            if (core::actsinfo::VERSION_ID != core::actsinfo::DEV_VERSION_ID &&
                dbg->actsVersion == core::actsinfo::VERSION_ID) {
                asmout << "/current";
            }

            asmout << " (0x" << std::hex << core::actsinfo::VERSION_ID << ")";

            asmout << "\n";

            if (dbg->HasFeature(ADF_FLAGS)) {
                asmout << "// flags ....";
                if (!dbg->flags)
                    asmout << " NONE";
                else {
                    // read known flags

                    if (dbg->HasFlag(ADFG_OBFUSCATED))
                        asmout << " OBFUSCATED";
                    if (dbg->HasFlag(ADFG_DEBUG))
                        asmout << " DEBUG";
                    if (dbg->HasFlag(ADFG_CLIENT))
                        asmout << " CLIENT";

                    uint32_t pltFlag = (dbg->flags & ADFG_PLATFORM_MASK) >> ADFG_PLATFORM_SHIFT;
                    if (pltFlag) {
                        Platform nplt{ (Platform)pltFlag };
                        asmout << " PLT(" << utils::MapString(utils::CloneString(PlatformName(nplt)), [](char c) {
                            return std::isspace(c) ? '_' : std::toupper(c);
                        }) << ")";

                        gdb.encodedPlatform = nplt;
                        gdb.bigEndian = vm->IsPlatformBigEndian(nplt);
                    }
                }

                asmout << "\n";
            }

            if (dbg->HasFeature(ADF_CHECKSUM)) {
                asmout << "// dbg crc .. " << "0x" << std::hex << dbg->checksum << "\n";
                gdb.checksum = dbg->checksum;
            }

            if (dbg->HasFeature(ADF_CRC_LOC)) {
                if (dbg->crc_offset) {
                    asmout << "// crc loc .. " << "0x" << std::hex << dbg->crc_offset << "\n";

                    gdb.checksumOffset = dbg->crc_offset;
                }
            }
            if (dbg->HasFeature(ADF_STRING)) {
                dbgReader.Goto(dbg->strings_offset);
                uint32_t* strOffsets = dbgReader.ReadPtr<uint32_t>(dbg->strings_count);
                asmout << "// hashes ... " << std::dec << dbg->strings_count << " (offset: 0x" << std::hex
                       << dbg->strings_offset << ")\n";
                for (size_t i = 0; i < dbg->strings_count; i++) {
                    dbgReader.Goto(strOffsets[i]);
                    const char* str{ dbgReader.ReadString() };

                    uint64_t hashField{ vm->HashField(str) };
                    uint64_t hashFilePath{ vm->HashFilePath(str) };
                    uint64_t hashPath{ vm->HashPath(str) };
                    {
                        core::async::opt_lock_guard hlg{ hashutils::GetMutex(false) };
                        hashutils::AddPrecomputed(hashField, str, true);
                        hashutils::AddPrecomputed(hashFilePath, str, true);
                        hashutils::AddPrecomputed(hashPath, str, true);

                        if (printInfo) {
                            utils::PrintFormattedString(asmout << "// - #\"", str)
                                << "\" (0x" << std::hex << hashField << "/0x" << hashFilePath << "/0x" << hashPath;
                        }
                        // use all the known hashes for this VM
                        for (auto& [k, func] : vm->hashesFunc) {
                            try {
                                int64_t hash = func.hashFunc(str);

                                if (hash) {
                                    if (printInfo) {
                                        asmout << "/" << k << '=' << std::hex << hash;
                                    }
                                    hashutils::AddPrecomputed(hash, str, true);
                                }
                            } catch (std::exception&) {
                                // ignore
                            }
                        }
                    }
                    if (printInfo) {
                        asmout << ")\n";
                    }
                }
                LOG_TRACE("{} hash(es) added", dbg->strings_count);
            }

            if (dbg->HasFeature(ADF_DETOUR)) {
                dbgReader.Goto(dbg->detour_offset);

                const GSC_ACTS_DETOUR* detours{ dbgReader.ReadPtr<GSC_ACTS_DETOUR>(dbg->detour_count) };

                for (size_t i = 0; i < dbg->detour_count; i++) {
                    const GSC_ACTS_DETOUR& detour = detours[i];

                    gdb.detours.emplace_back(
                        GscDecompilerGDBDataDetour{
                            .name_space = detour.name_space,
                            .name = detour.name,
                            .script = detour.script,
                            .location = detour.location,
                            .size = detour.size,
                        }
                    );
                }
            }
            if (dbg->HasFeature(ADF_DEVBLOCK_BEGIN)) {
                // not used by acts decompiler, but can be useful for a vm
                if (printInfo) {
                    dbgReader.Goto(dbg->devblock_offset);
                    uint32_t* dvOffsets = dbgReader.ReadPtr<uint32_t>(dbg->devblock_count);
                    asmout << "// devblock . " << std::dec << dbg->devblock_count << " (offset: 0x" << std::hex
                           << dbg->devblock_offset << ")\n";
                    for (size_t i = 0; i < dbg->devblock_count; i++) {
                        uint32_t off = dvOffsets[i];
                        asmout << "// - " << ctx.GetFLocName(off) << "\n";
                    }
                }
            }
            if (dbg->HasFeature(ADF_LAZYLINK)) {
                // not used by acts decompiler, but can be useful for a vm
                if (printInfo) {
                    asmout << "// lazylink . " << std::dec << dbg->lazylink_count << " (offset: 0x" << std::hex
                           << dbg->lazylink_offset << ")\n";

                    dbgReader.Goto(dbg->lazylink_offset);
                    for (size_t i = 0; i < dbg->lazylink_count; i++) {
                        GSC_ACTS_LAZYLINK* lzOff = dbgReader.ReadPtr<GSC_ACTS_LAZYLINK>();
                        asmout << "// " << hashutils::ExtractTmp("namespace", lzOff->name_space) << "<"
                               << hashutils::ExtractTmpScript(lzOff->script)
                               << ">::" << hashutils::ExtractTmp("function", lzOff->name) << "\n"
                               << "// locs: ";

                        uint32_t* locs = dbgReader.ReadPtr<uint32_t>(lzOff->num_address);
                        for (size_t i = 0; i < lzOff->num_address; i++) {
                            if (i)
                                asmout << ", ";
                            asmout << ctx.GetFLocName(locs[i]);
                        }
                        asmout << "\n";
                    }
                }
            }
            if (dbg->HasFeature(ADF_FILES)) {
                if (printInfo) {
                    asmout << "// files .... " << std::dec << dbg->files_count << " (offset: 0x" << std::hex
                           << dbg->files_offset << ")\n";
                    dbgReader.Goto(dbg->files_offset);
                    GSC_ACTS_FILES* linesOff = dbgReader.ReadPtr<GSC_ACTS_FILES>(dbg->files_count);
                    for (size_t i = 0; i < dbg->files_count; i++) {
                        GSC_ACTS_FILES& l = linesOff[i];

                        dbgReader.Goto(l.filename);
                        const char* filename{ dbgReader.ReadString() };
                        asmout << "// - " << std::dec << filename << " " << l.lineStart << "->" << l.lineEnd << "\n";
                    }
                }
            }
            if (dbg->HasFeature(ADF_LINES)) {
                // not used by acts decompiler, but can be useful for a vm
                if (printInfo) {
                    asmout << "// lines .... " << std::dec << dbg->lines_count << " (offset: 0x" << std::hex
                           << dbg->lines_offset << ")\n";
                    dbgReader.Goto(dbg->lines_offset);
                    GSC_ACTS_LINES* linesOff = dbgReader.ReadPtr<GSC_ACTS_LINES>(dbg->lines_count);
                    for (size_t i = 0; i < dbg->lines_count; i++) {
                        GSC_ACTS_LINES& l = linesOff[i];
                        asmout << "// - " << std::dec << l.lineNum << " " << ctx.GetFLocName(l.start) << "->"
                               << ctx.GetFLocName(l.end) << "\n";
                    }
                }
            }
        }
        bool DbgSave(GscDecompilerGDBData* gdb, std::string& buffer) override {
            using namespace shared::gsc::acts_debug;
            std::vector<byte> data{};

            utils::Allocate<GSC_ACTS_DEBUG>(data);
            uint32_t flags{};

            size_t tableMappedStrings{};
            size_t tableMappedStringsCount{};

            if (!gdb->devStringsLocations.empty()) {
                // we remap the string so the user isn't restricted with the existing sizes and us with the size of the
                // treyarch header.
                flags |= ADFG_MAP_DEBUG_STRING;
                tableMappedStrings =
                    utils::Allocate(data, sizeof(GSC_ACTS_MAPPEDDEVSTRING) * gdb->devStringsLocations.size());

                for (const auto& [floc, string] : gdb->devStringsLocations) {
                    GSC_ACTS_MAPPEDDEVSTRING& map{
                        ((GSC_ACTS_MAPPEDDEVSTRING*)&data[tableMappedStrings])[tableMappedStringsCount++]
                    };
                    map.original = floc;
                    map.address = (uint32_t)data.size();
                    utils::WriteString(data, string ? string : "");
                }
            }

            // gdb->devBlocksLocation
            // gdb->lineInfos
            // gdb->lazyLinks
            // gdb->detours

            GSC_ACTS_DEBUG& header{ *(GSC_ACTS_DEBUG*)data.data() };
            *(uint64_t*)header.magic = shared::gsc::acts_debug::MAGIC;
            header.version = shared::gsc::acts_debug::CURRENT_VERSION;
            header.actsVersion = core::actsinfo::VERSION_ID;
            header.flags = flags;
            header.checksum = static_cast<int32_t>(gdb->checksum);
            header.mappeddevstrings_offset = (int32_t)tableMappedStrings;
            header.mappeddevstrings_count = (int32_t)tableMappedStringsCount;
            header.crc_offset = 0;
            header.detour_offset = 0;
            header.detour_count = 0;
            header.devstrings_offset = 0;
            header.devstrings_count = 0;

            buffer.resize(data.size());
            std::memcpy(buffer.data(), data.data(), data.size());
            return true;
        }
    };
} // namespace tool::gsc::vm