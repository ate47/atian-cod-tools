#include <includes.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/vm/vm_t6.hpp>


// Call of Duty: Black Ops 2 (T6)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    /*
    class T6GSCOBJHandler : public GSCOBJHandler {
    public:
        T6GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_STRING_NAMES | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T7 | GOHF_SUPPORT_GET_API_SCRIPT | GOHF_SWITCH_TYPE_T7) {}
        
        void SwitchHeaderEndian() override {
            throw std::runtime_error("SwitchHeaderEndian not implemented");
        }
        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<T6GscObj>();

        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<T6GscObj>();
            /*

            auto* fixups = reinterpret_cast<T8GSCFixup*>(&data->magic[data->fixup_offsets]);

            for (size_t i = 0; i < data->fixup_count; i++) {
                const auto& fixup = fixups[i];
                asmout << std::hex << "#fixup 0x" << fixup.offset << " = 0x" << fixup.address << ";\n";
            }

            if (data->fixup_count) {
                asmout << "\n";
            }
            *//*
        }

        uint64_t GetName() override {
            const char* name{ Ptr<const char>(Ptr<T6GscObj>()->name_offset) };
            hashutils::Add(name);
            return hash::Hash64(name);
        }
        uint16_t GetExportsCount() override {
            return Ptr<T6GscObj>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<T6GscObj>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<T6GscObj>()->include_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<T6GscObj>()->include_offset;
        }
        uint16_t GetImportsCount() override {
            return Ptr<T6GscObj>()->import_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<T6GscObj>()->import_offset;
        }
        uint16_t GetGVarsCount() override {
            return 0;
        }
        uint32_t GetGVarsOffset() override {
            return 0;
        }
        uint16_t GetStringsCount() override {
            return Ptr<T6GscObj>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<T6GscObj>()->string_offset;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<T6GscObj>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<T6GscObj>()->devblock_string_offset;
        }
        size_t GetHeaderSize() override {
            return sizeof(T6GscObj);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T6GscObj) && Ref<uint64_t>() == VMI_T6_01;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return (uint16_t)Ptr<T6GscObj>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<T6GscObj>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            throw std::runtime_error("can't get string name for this vm");
        }
        void SetNameString(uint32_t name) override {
            Ptr<T6GscObj>()->name_offset = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = VMI_T6_01;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<T6GscObj>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<T6GscObj>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            if (val > UINT8_MAX) {
                throw std::runtime_error(utils::va("too many includes, max: %d", UINT8_MAX));
            }
            Ptr<T6GscObj>()->include_count = (uint8_t)val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<T6GscObj>()->include_offset = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<T6GscObj>()->import_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<T6GscObj>()->import_offset = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<T6GscObj>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<T6GscObj>()->string_offset = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<T6GscObj>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<T6GscObj>()->devblock_string_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}
        void SetFileSize(uint32_t val) override {
            Ptr<T6GscObj>()->profile_offset = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<T6GscObj>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<T6GscObj>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<T6GscObj>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<T6GscObj>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {}
        void SetAnimTreeSingleOffset(uint32_t val) override {}
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<T6GscObj>()->animtree_count = (uint8_t)val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<T6GscObj>()->animtree_offset = val;
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
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {}
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {}
        int64_t GetDefaultChecksum(bool client) override {
            return 0;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<T6GscObj>()->source_crc = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<T6GscObj>()->source_crc;
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
            return false;
        }
        byte GetVTableImportFlags() override {
            return 0;
        }
    };
    REGISTER_GSC_VM(VMI_T6_01, T6GSCOBJHandler);
    */
}