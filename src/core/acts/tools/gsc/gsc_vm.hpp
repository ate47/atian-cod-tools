#pragma once
#include <core/bytebuffer.hpp>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>

namespace tool::gsc::vm {
    class GscGdb;
    class GscVm;

    GscVm* GetGscReader(uint64_t vm);
    GscGdb* GetGdbReader(uint64_t magic);
    GscGdb* GetGdbReader(const char* name);

    std::unordered_map<uint64_t, std::shared_ptr<GscVm>>& GetGscReaders();
    std::unordered_map<uint64_t, std::shared_ptr<GscGdb>>& GetGdbReaders();
    void RegisterVmOpCodes();

    class GscGdb {
      public:
        uint64_t magic;
        const char* id;
        GscGdb(uint64_t magic, const char* id) : magic(magic), id(id) {}

        virtual void DbgLoad(
            GscDecompilerGDBData& gdb, vm::VmInfo* vm, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout,
            bool printInfo
        ) {
            throw std::runtime_error("DbgLoad2 not implemented for this vm");
        }
        void DbgLoad(T8GSCOBJContext& ctx, core::bytebuffer::ByteBuffer& dbgReader, std::ostream& asmout) {
            GscDecompilerGDBData gdb{};
            DbgLoad(gdb, ctx.m_vmInfo, dbgReader, asmout, ctx.opt.m_header);

            uint32_t schk{ ctx.scriptfile->GetChecksum() };
            if (schk && schk != gdb.checksum) {
                LOG_WARNING("Can't use dbg data: unmatching checksums: 0x{:x} != 0x{:x}", schk, gdb.checksum);
                return;
            }

            // the script is saying which platform is was compiled, so we follow it
            if (!ctx.opt.m_ignoreDebugPlatform && gdb.encodedPlatform < Platform::PLATFORM_COUNT) {
                LOG_TRACE("Using debug platform {}", PlatformName(gdb.encodedPlatform));
                ctx.currentPlatform = gdb.encodedPlatform;
            }
            if (gdb.bigEndian) {
                ctx.isBigEndian = true;
            }

            for (const GscDecompilerGDBDataDetour& detour : gdb.detours) {
                GscDetourInfo& det = ctx.m_gsicInfo.detours[detour.location];
                det.name = detour.name;
                det.fixupOffset = detour.location;
                det.fixupSize = detour.size;
                det.replaceFunction = detour.name;
                det.replaceNamespace = detour.name_space;
                det.replaceScript = detour.script;
            }
            if (ctx.gdbData) {
                // clone the new debug information
                ctx.gdbData->checksumOffset = gdb.checksumOffset;
                ctx.gdbData->detours.insert(ctx.gdbData->detours.begin(), gdb.detours.begin(), gdb.detours.end());
            }
        }

        virtual bool DbgSave(GscDecompilerGDBData* gdb, std::string& buffer) {
            throw std::runtime_error("DbgSave not implemented for this vm");
        }
    };
    class GscVm {
      public:
        uint64_t magic;
        GscVm(uint64_t magic) : magic(magic) {}

        virtual std::shared_ptr<GSCOBJHandler> NewHandler(byte* buff, size_t size) const {
            throw std::runtime_error("NewHandler not implemented for this vm");
        }

        inline std::shared_ptr<GSCOBJHandler> operator()(byte* buff, size_t size) { return NewHandler(buff, size); }
    };
} // namespace tool::gsc::vm