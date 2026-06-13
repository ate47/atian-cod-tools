#pragma once
#include <core/actsinfo.hpp>
#include <core/async.hpp>
#include <core/updater.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <gsc/gsc_acts_addons.hpp>

namespace tool::gsc::vm {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;
    using namespace tool::gsc::gdb;

    class GscGdbActsAddon : public GscGdb {
    public:
        GscGdbActsAddon() : GscGdb(shared::gsc::acts_addons::MAGIC) {}

        void DbgLoad(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout) override {
            using namespace shared::gsc::acts_addons;
            // acts compiled file, read data
            GSC_ACTS_ADDONS* dbg = dbgReader.Ptr<GSC_ACTS_ADDONS>();
            LOG_TRACE("Reading ACTS addon data v{:x} at 0x{:x}", (int)dbg->version, dbgReader.Loc());
            asmout << "// ACTS addon compiled file, file version 0x" << std::hex << (int)dbg->version << "\n";

            asmout << "// flags ....";
            if (!dbg->flags) asmout << " NONE";
            else {
                // read known flags

            }

            asmout << "\n";

            if (dbg->HasFeature(AAF_CRC_OFFSET)) {
                if (dbg->crc_offset) {
                    asmout << "// crc loc .. " << "0x" << std::hex << dbg->crc_offset << " ";

                    if (ctx.scriptfile->HasFlag(GOHF_NOTIFY_CRC_STRING)) {
                        if (dbg->crc_offset > ctx.scriptfile->GetFileSize()) {
                            asmout << "INVALID LOC";
                        }
                        else {
                            utils::PrintFormattedString(asmout << "\"", ctx.scriptfile->Ptr<const char>(dbg->crc_offset)) << "\"";
                        }
                    }
                    else if (ctx.scriptfile->HasFlag(GOHF_NOTIFY_CRC)) {
                        asmout << ctx.GetFLocName(dbg->crc_offset);
                    }
                    else {
                        asmout << "USELESS"; // why?
                    }

                    asmout << "\n";
                }
            }

            if (dbg->HasFeature(AAF_DETOUR)) {
                dbgReader.Goto(dbg->detour_offset);
                const GSC_ACTS_DETOUR* detours = dbgReader.ReadPtr<GSC_ACTS_DETOUR>(dbg->detour_count);

                for (size_t i = 0; i < dbg->detour_count; i++) {
                    const GSC_ACTS_DETOUR& detour = detours[i];

                    GscDetourInfo& det = ctx.m_gsicInfo.detours[detour.location];
                    det.name = detour.name;
                    det.fixupOffset = detour.location;
                    det.fixupSize = detour.size;
                    det.replaceFunction = detour.name;
                    det.replaceNamespace = detour.name_space;
                    det.replaceScript = detour.script;
                }
            }
        }
    };
}