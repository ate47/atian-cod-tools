#include <includes.hpp>
#include <decryptutils.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>


// Call of Duty: Black Ops Cold War (T9)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class T937GSCOBJHandler : public GSCOBJHandler {
    public:
        T937GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_SUPPORT_VAR_PTR | GOHF_FOREACH_TYPE_T9 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_SWITCH_TYPE_T89) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T937GSCOBJ>();
            if (opt.m_test_header) {
                asmout
                    << "// ukn0c .... " << std::dec << data->pad0c << " / 0x" << std::hex << data->pad0c << "\n"
                    << "// unk3a .... " << std::dec << data->unk3a << " / 0x" << std::hex << data->unk3a << "\n"
                    << "// unk48 .... " << std::dec << data->unk48 << " / 0x" << std::hex << data->unk48 << "\n"
                    << "// unk52 .... " << std::dec << data->unk52 << " / 0x" << std::hex << data->unk52 << "\n"
                    << "// unk54 .... " << std::dec << data->unk54 << " / 0x" << std::hex << data->unk54 << "\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T937GSCOBJ>();

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offset]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
        }

        uint64_t GetName() override {
            return Ptr<T937GSCOBJ>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<T937GSCOBJ>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T937GSCOBJ>()->exports_tables;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T937GSCOBJ>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T937GSCOBJ>()->includes_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T937GSCOBJ>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T937GSCOBJ>()->imports_offset;
        }
        uint16_t GetGVarsCount() override {
            return Ptr<T937GSCOBJ>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return Ptr<T937GSCOBJ>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T937GSCOBJ>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T937GSCOBJ>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T937GSCOBJ>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T937GSCOBJ>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<T937GSCOBJ>()->file_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T937GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        std::pair<const char*, size_t> GetStringHeader(size_t len) override {
            static thread_local byte str[3]{ 0x8b, 0xFF, 0 };
            str[1] = (byte)(len + 1);
            return { reinterpret_cast<const char*>(&str[0]), sizeof(str) };
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T937GSCOBJ) && Ref<uint64_t>() == 0x37000a0d43534780;
        }

        byte RemapFlagsImport(byte flags) override {
            return flags; // same as bo4?
        }

        byte RemapFlagsExport(byte flags) override {
            return flags;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return 0;
        };

        void SetName(uint64_t name) override {
            Ptr<T937GSCOBJ>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x37000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->exports_tables = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->includes_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->imports_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
            Ptr<T937GSCOBJ>()->globalvar_count = val;
        }
        void SetGVarsOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->globalvar_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T937GSCOBJ>()->file_size = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<T937GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T937GSCOBJ>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<T937GSCOBJ>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<T937GSCOBJ>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {}
        void SetAnimTreeDoubleOffset(uint32_t val) override {}
        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T8GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return sizeof(tool::gsc::T8GSCGlobalVar);
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return 0;
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.callback_event = (uint32_t)item.file_name_space;
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
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {
            *reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(data) = item;
        }
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            // todo: check client
            constexpr int32_t t = 0xefc1b08d;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T937GSCOBJ>()->crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return (uint32_t)Ptr<T937GSCOBJ>()->crc;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/core_common/clientids_shared.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == CLASS_VTABLE;
        }
        byte GetVTableImportFlags() override {
            return CLASS_VTABLE;
        }
    };


    /*****************************************************************************************************************************/

    class T9GSCOBJHandler : public GSCOBJHandler {
    public:
        T9GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_NOTIFY_CRC | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_SUPPORT_VAR_PTR | GOHF_FOREACH_TYPE_T9 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_SWITCH_TYPE_T89) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            if (opt.m_test_header) {
                auto* data = Ptr<T9GSCOBJ>();
                asmout
                    << "// ukn0c .... " << std::dec << data->pad0c << " / 0x" << std::hex << data->pad0c << "\n"
                    << "// unk1e .... " << std::dec << data->unk1e << " / 0x" << std::hex << data->unk1e << "\n"
                    << "// unk22 .... " << std::dec << data->unk22 << " / 0x" << std::hex << data->unk22 << "\n"
                    << "// unk40 .... " << std::dec << data->unk40 << " / 0x" << std::hex << data->unk40 << "\n" // offset
                    << "// unk4c .... " << std::dec << data->unk4c << " / 0x" << std::hex << data->unk4c << "\n"
                    << "// unk54 .... " << std::dec << data->unk54 << " / 0x" << std::hex << data->unk54 << "\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
        }

        uint64_t GetName() override {
            return Ptr<T9GSCOBJ>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<T9GSCOBJ>()->exports_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T9GSCOBJ>()->exports_tables;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T9GSCOBJ>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T9GSCOBJ>()->includes_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T9GSCOBJ>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T9GSCOBJ>()->import_tables;
        }
        uint16_t GetGVarsCount() override {
            return Ptr<T9GSCOBJ>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return Ptr<T9GSCOBJ>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T9GSCOBJ>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T9GSCOBJ>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T9GSCOBJ>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T9GSCOBJ>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<T9GSCOBJ>()->file_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T9GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        std::pair<const char*, size_t> GetStringHeader(size_t len) override {
            static thread_local byte str[3]{ 0x8b, 0xFF, 0 };
            str[1] = (byte)(len + 1);
            return { reinterpret_cast<const char*>(&str[0]), sizeof(str) };
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T9GSCOBJ) && Ref<uint64_t>() == 0x38000a0d43534780;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags = 0;

            switch (flags & T9_IF_CALLTYPE_MASK) {
            case T9_IF_METHOD_CHILDTHREAD: nflags |= METHOD_CHILDTHREAD; break;
            case T9_IF_METHOD_THREAD: nflags |= METHOD_THREAD; break;
            case T9_IF_FUNCTION_CHILDTHREAD: nflags |= FUNCTION_CHILDTHREAD; break;
            case T9_IF_FUNCTION: nflags |= FUNCTION; break;
            case T9_IF_FUNC_METHOD: nflags |= FUNC_METHOD; break;
            case T9_IF_FUNCTION_THREAD: nflags |= FUNCTION_THREAD; break;
            case T9_IF_METHOD: nflags |= METHOD; break;
            default: nflags |= flags & 0xF; // wtf?
            }

            if (flags & T9_IF_DEV_CALL) nflags |= DEV_CALL;
            if (flags & T9_IF_GET_CALL) nflags |= GET_CALL;

            return nflags;
        }

        byte RemapFlagsExport(byte flags) override {
            if (flags == T9_EF_CLASS_VTABLE) {
                return CLASS_VTABLE;
            }
            byte nflags = 0;

            if (flags & T9_EF_AUTOEXEC) nflags |= AUTOEXEC;
            if (flags & T9_EF_LINKED) nflags |= LINKED;
            if (flags & T9_EF_PRIVATE) nflags |= PRIVATE;
            if (flags & T9_EF_CLASS_MEMBER) nflags |= CLASS_MEMBER;
            if (flags & T9_EF_EVENT) nflags |= EVENT;
            if (flags & T9_EF_VE) nflags |= VE;
            if (flags & T9_EF_CLASS_LINKED) nflags |= CLASS_LINKED;
            if (flags & T9_EF_CLASS_DESTRUCTOR) nflags |= CLASS_DESTRUCTOR;

            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags = 0;

            switch (flags & T9_IF_CALLTYPE_MASK) {
            case METHOD_CHILDTHREAD: nflags |= T9_IF_METHOD_CHILDTHREAD; break;
            case METHOD_THREAD: nflags |= T9_IF_METHOD_THREAD; break;
            case FUNCTION_CHILDTHREAD: nflags |= T9_IF_FUNCTION_CHILDTHREAD; break;
            case FUNCTION: nflags |= T9_IF_FUNCTION; break;
            case FUNC_METHOD: nflags |= T9_IF_FUNC_METHOD; break;
            case FUNCTION_THREAD: nflags |= T9_IF_FUNCTION_THREAD; break;
            case METHOD: nflags |= T9_IF_METHOD; break;
            default: nflags |= (flags & 0xF); break;// wtf?
            }

            if (flags & DEV_CALL) nflags |= T9_IF_DEV_CALL;
            if (flags & GET_CALL) nflags |= T9_IF_GET_CALL;

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            if (flags == CLASS_VTABLE) {
                return T9_EF_CLASS_VTABLE;
            }
            byte nflags = 0;

            if (flags & AUTOEXEC) nflags |= T9_EF_AUTOEXEC;
            if (flags & LINKED) nflags |= T9_EF_LINKED;
            if (flags & PRIVATE) nflags |= T9_EF_PRIVATE;
            if (flags & CLASS_MEMBER) nflags |= T9_EF_CLASS_MEMBER;
            if (flags & EVENT) nflags |= T9_EF_EVENT;
            if (flags & VE) nflags |= T9_EF_VE;
            if (flags & CLASS_LINKED) nflags |= T9_EF_CLASS_LINKED;
            if (flags & CLASS_DESTRUCTOR) nflags |= T9_EF_CLASS_DESTRUCTOR;

            return nflags;
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == T9_EF_CLASS_VTABLE;
        }
        byte GetVTableImportFlags() override {
            return T9_EF_CLASS_VTABLE;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return 0;
        };

        void SetName(uint64_t name) override {
            Ptr<T9GSCOBJ>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0x38000a0d43534780;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->exports_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->exports_tables = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->includes_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->import_tables = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {
            Ptr<T9GSCOBJ>()->globalvar_count = val;
        }
        void SetGVarsOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->globalvar_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            Ptr<T9GSCOBJ>()->file_size = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<T9GSCOBJ>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T9GSCOBJ>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<T9GSCOBJ>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<T9GSCOBJ>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {}
        void SetAnimTreeDoubleOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(tool::gsc::T8GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::T8GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return sizeof(tool::gsc::T8GSCGlobalVar);
        }
        size_t GetAnimTreeSingleSize() override {
            return 0;
        }
        size_t GetAnimTreeDoubleSize() override {
            return 0;
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::T8GSCExport*>(data);
            imp.name = (uint32_t)item.name;
            imp.name_space = (uint32_t)item.name_space;
            imp.callback_event = (uint32_t)item.file_name_space;
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
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {
            *reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(data) = item;
        }
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override { }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override { }
        int64_t GetDefaultChecksum(bool client) override {
            // todo: check client
            constexpr int32_t t = 0xc97916a2;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T9GSCOBJ>()->crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return (uint32_t)Ptr<T9GSCOBJ>()->crc;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/core_common/clientids_shared.gsc";
        }
    };

}
REGISTER_GSC_VM(VMI_T9, T9GSCOBJHandler);
REGISTER_GSC_VM(VMI_T937, T937GSCOBJHandler);