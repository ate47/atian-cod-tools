#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>


// Call of Duty: Black Ops 3 (T7)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class T7GSCOBJHandler : public GSCOBJHandler {
    public:
        T7GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_STRING_NAMES | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T7 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_SWITCH_TYPE_T7) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T7GSCOBJ>();
            asmout
                << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offsets << ")\n";

            if (opt.m_test_header) {
                asmout
                    << "// unk40 .... " << std::dec << (int)data->unk40 << " / 0x" << std::hex << (int)data->unk40 << "\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T7GSCOBJ>();

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offsets]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
        }

        uint32_t GetFileSize() override {
            return Ptr<T7GSCOBJ>()->script_size;
        }
        uint64_t GetName() override {
            const char* name{ Ptr<T7GSCOBJ>()->GetName() };
            hashutils::Add(name);
            return hash::Hash64(name);
        }
        uint16_t GetExportsCount() override {
            return Ptr<T7GSCOBJ>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T7GSCOBJ>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T7GSCOBJ>()->include_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T7GSCOBJ>()->include_offset;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T7GSCOBJ>()->import_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T7GSCOBJ>()->import_offset;
        }
        uint16_t GetGVarsCount() override {
            return 0;
        }
        uint32_t GetGVarsOffset() override {
            return 0;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T7GSCOBJ>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T7GSCOBJ>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T7GSCOBJ>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T7GSCOBJ>()->devblock_string_offset;
        }
        size_t GetHeaderSize() override {
            return sizeof(T7GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T7GSCOBJ) && Ref<uint64_t>() == 0x1c000a0d43534780;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return (uint16_t)Ptr<T7GSCOBJ>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<T7GSCOBJ>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            throw std::runtime_error("can't get string name for this vm");
        }
        void SetNameString(uint32_t name) override {
            Ptr<T7GSCOBJ>()->name_offset = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x1c000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            if (val > UINT8_MAX) {
                throw std::runtime_error(utils::va("too many includes, max: %d", UINT8_MAX));
            }
            Ptr<T7GSCOBJ>()->include_count = (uint8_t)val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->include_offset = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->import_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->import_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
        }
        void SetGVarsOffset(uint32_t val) override {
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T7GSCOBJ>()->script_size = val; // not available?
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T7GSCOBJ>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<T7GSCOBJ>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<T7GSCOBJ>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->animtree_count = (uint8_t)val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->animtree_offset = val;
        }
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T7GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::T7GscAnimTree);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T7GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            return 0x1234567;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T7GSCOBJ>()->source_crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<T7GSCOBJ>()->source_crc;
        }
        const char* GetDefaultName(bool client) override {
            return client ? "" : "scripts/shared/duplicaterender_mgr.gsc";
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags & 1) nflags |= T8GSCExportFlags::LINKED;
            if (flags & 2) nflags |= T8GSCExportFlags::AUTOEXEC;
            if (flags & 4) nflags |= T8GSCExportFlags::PRIVATE;
            if (flags & 0x20) nflags |= T8GSCExportFlags::VE;

            return nflags;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= T8GSCImportFlags::FUNC_METHOD; break;
            case 2: nflags |= T8GSCImportFlags::FUNCTION; break;
            case 3: nflags |= T8GSCImportFlags::FUNCTION_THREAD; break;
            case 4: nflags |= T8GSCImportFlags::METHOD; break;
            case 5: nflags |= T8GSCImportFlags::METHOD_THREAD; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }

            if (flags & 0x10) nflags |= T8GSCImportFlags::DEV_CALL;
            if (flags & 0x20) nflags |= T8GSCImportFlags::GET_CALL;


            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags{};
            if (flags == tool::gsc::CLASS_VTABLE) return 0x6;
            if (flags & T8GSCExportFlags::LINKED) nflags |= 1;
            if (flags & T8GSCExportFlags::AUTOEXEC) nflags |= 2;
            if (flags & T8GSCExportFlags::PRIVATE) nflags |= 4;
            if (flags & T8GSCExportFlags::VE) nflags |= 0x20;
            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 1; break;
            case FUNCTION: nflags |= 2; break;
            case FUNCTION_THREAD:nflags |= 3; break;
            case METHOD: nflags |= 4; break;
            case METHOD_THREAD:  nflags |= 5; break;
            default: nflags |= (flags & 0xF); break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x6;
        }
        byte GetVTableImportFlags() override {
            return 0x6;
        }
    };
    /*****************************************************************************************************************************/

    class T71BGSCOBJHandler : public GSCOBJHandler {
    public:
        T71BGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_STRING_NAMES | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T7 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_SWITCH_TYPE_T7) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T7GSCOBJ>();
            asmout
                << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offsets << ")\n";

            if (opt.m_test_header) {
                asmout
                    << "// unk40 .... " << std::dec << (int)data->unk40 << " / 0x" << std::hex << (int)data->unk40 << "\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T7GSCOBJ>();

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offsets]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
        }

        uint32_t GetFileSize() override {
            return Ptr<T7GSCOBJ>()->script_size;
        }
        uint64_t GetName() override {
            const char* name{ Ptr<T7GSCOBJ>()->GetName() };
            hashutils::Add(name);
            return hash::Hash64(name);
        }
        uint16_t GetExportsCount() override {
            return Ptr<T7GSCOBJ>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T7GSCOBJ>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T7GSCOBJ>()->include_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T7GSCOBJ>()->include_offset;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T7GSCOBJ>()->import_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T7GSCOBJ>()->import_offset;
        }
        uint16_t GetGVarsCount() override {
            return 0;
        }
        uint32_t GetGVarsOffset() override {
            return 0;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T7GSCOBJ>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T7GSCOBJ>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T7GSCOBJ>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T7GSCOBJ>()->devblock_string_offset;
        }
        size_t GetHeaderSize() override {
            return sizeof(T7GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T7GSCOBJ) && Ref<uint64_t>() == 0x1b000a0d43534780;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return (uint16_t)Ptr<T7GSCOBJ>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<T7GSCOBJ>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            throw std::runtime_error("can't get string name for this vm");
        }
        void SetNameString(uint32_t name) override {
            Ptr<T7GSCOBJ>()->name_offset = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x1b000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            if (val > UINT8_MAX) {
                throw std::runtime_error(utils::va("too many includes, max: %d", UINT8_MAX));
            }
            Ptr<T7GSCOBJ>()->include_count = (uint8_t)val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->include_offset = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->import_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->import_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
        }
        void SetGVarsOffset(uint32_t val) override {
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T7GSCOBJ>()->script_size = val; // not available?
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T7GSCOBJ>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<T7GSCOBJ>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<T7GSCOBJ>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<T7GSCOBJ>()->animtree_count = (uint8_t)val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<T7GSCOBJ>()->animtree_offset = val;
        }
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T7GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::T7GscAnimTree);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T7GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCImport*>(data);
            imp.name = (uint32_t)item.name;
            imp.import_namespace = (uint32_t)item.name_space;
            imp.flags = item.flags;
            imp.num_address = item.num_address;
            imp.param_count = item.param_count;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            return 0x1234567;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T7GSCOBJ>()->source_crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<T7GSCOBJ>()->source_crc;
        }
        const char* GetDefaultName(bool client) override {
            return client ? "" : "scripts/shared/duplicaterender_mgr.gsc";
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags & 1) nflags |= T8GSCExportFlags::LINKED;
            if (flags & 2) nflags |= T8GSCExportFlags::AUTOEXEC;
            if (flags & 4) nflags |= T8GSCExportFlags::PRIVATE;
            if (flags & 0x20) nflags |= T8GSCExportFlags::VE;

            return nflags;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= T8GSCImportFlags::FUNC_METHOD; break;
            case 2: nflags |= T8GSCImportFlags::FUNCTION; break;
            case 3: nflags |= T8GSCImportFlags::FUNCTION_THREAD; break;
            case 4: nflags |= T8GSCImportFlags::METHOD; break;
            case 5: nflags |= T8GSCImportFlags::METHOD_THREAD; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }

            if (flags & 0x10) nflags |= T8GSCImportFlags::DEV_CALL;
            if (flags & 0x20) nflags |= T8GSCImportFlags::GET_CALL;


            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags{};
            if (flags == tool::gsc::CLASS_VTABLE) return 0x6;
            if (flags & T8GSCExportFlags::LINKED) nflags |= 1;
            if (flags & T8GSCExportFlags::AUTOEXEC) nflags |= 2;
            if (flags & T8GSCExportFlags::PRIVATE) nflags |= 4;
            if (flags & T8GSCExportFlags::VE) nflags |= 0x20;
            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 1; break;
            case FUNCTION: nflags |= 2; break;
            case FUNCTION_THREAD:nflags |= 3; break;
            case METHOD: nflags |= 4; break;
            case METHOD_THREAD:  nflags |= 5; break;
            default: nflags |= (flags & 0xF); break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x6;
        }
        byte GetVTableImportFlags() override {
            return 0x6;
        }
    };

}
REGISTER_GSC_VM(VMI_T7, T7GSCOBJHandler);
REGISTER_GSC_VM(VMI_T71B, T71BGSCOBJHandler);