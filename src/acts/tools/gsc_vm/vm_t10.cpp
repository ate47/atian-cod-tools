#include <includes.hpp>
#include <decryptutils.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>


// Call of Duty: Black Ops 6 (T10)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class T1006GSCOBJHandler : public GSCOBJHandler {
    public:
        T1006GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_NOTIFY_CRC_STRING | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_VAR_VA_COUNT | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            GscObj24* data = Ptr<GscObj24>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk28 :" << std::dec << std::setw(3) << (int)data->unk28 << " (0x" << std::hex << data->unk28 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj24>();

            if (opt.m_test_header) {
                // nothing
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj24>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj24>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj24>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj24>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj24>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj24>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj24>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj24>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj24>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj24>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj24>()->string_table;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj24>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj24>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj24>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj24>()->animtree_offset;
        };
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj24>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj24>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj24>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj24);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj24) && Ref<uint64_t>() == 0xa0d43534706;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptMethodThreadCallEndOn / ScriptThreadCallEndOn (same script?)
            case 2: nflags |= METHOD_THREAD; break; // ScriptMethodThreadCall
            case 3: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptThreadCallEndOn (file namespace)
            case 6: nflags |= FUNCTION_THREAD; break; // ScriptThreadCall (namspace?)
            case 4: nflags |= FUNCTION; break; // ScriptFunctionCall (same script? / file namespace)
            case 7: nflags |= FUNCTION; break; // ScriptFunctionCall (namespace?)
            case 5: nflags |= FUNC_METHOD; break; // GetFunction (same namespace?)
            case 8: // params(1) + CallBuiltinFunction
            case 0xA:nflags |= FUNCTION; break; // api call / GetBuiltinFunction
            case 9: // params(1) + CallBuiltinMethod
            case 0xB:nflags |= METHOD; break; // api call / GetBuiltinMethod
            default: nflags |= flags & 0xF; break; // wtf?
            }

            if (flags & 0x10) nflags |= DEV_CALL;
            if (flags & 0x20) nflags |= ACTS_USE_FULL_NAMESPACE;

            /*
            0x10: Dev import
            0x20: use file namespace
            GetFunction = 0x5
            ScriptFunctionCall = 0x4, 0x7
            ScriptThreadCall = 0x2, 0x6
            ScriptThreadCallEndOn = 0x3, 0x1
            ScriptMethodCall = 0x7
            ScriptMethodThreadCall = 0x2
            ScriptMethodThreadCallEndOn = 0x1
            CallBuiltinFunction = 0x8
            CallBuiltinMethod = 0x9
            GetBuiltinFunction = 0xa
            GetBuiltinMethod = 0xb
            */

            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 0x5; break;
            case FUNCTION: nflags |= 0x4; break;
            case METHOD: nflags |= 0x7; break;
            case METHOD_CHILDTHREAD:
            case FUNCTION_CHILDTHREAD: nflags |= 0x1; break;
            case FUNCTION_THREAD:
            case METHOD_THREAD: nflags |= 0x2; break;
            case ACTS_CALL_BUILTIN_FUNCTION: nflags |= 0x8; break;
            case ACTS_CALL_BUILTIN_METHOD: nflags |= 0x9; break;
            case ACTS_GET_BUILTIN_FUNCTION: nflags |= 0xa; break;
            case ACTS_GET_BUILTIN_METHOD: nflags |= 0xb; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags == 0x15) return tool::gsc::CLASS_VTABLE;
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }
            if (flags & 0x20) {
                nflags |= T8GSCExportFlags::EVENT;
            }
            if (flags & 0x40) {
                nflags |= T8GSCExportFlags::VE;
            }

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags = 0;
            if (flags == tool::gsc::CLASS_VTABLE) return 0x15;

            if (flags & AUTOEXEC) nflags |= 1;
            if (flags & LINKED) nflags |= 2;
            if (flags & PRIVATE) nflags |= 4;
            if (flags & EVENT) nflags |= 0x20;
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }

        void SetName(uint64_t name) override {
            Ptr<GscObj24>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d43534706;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj24>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj24>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj24>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj24>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj24>()->size1 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj24>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj24>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj24>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj24>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(tool::gsc::IW23GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::IW24GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return sizeof(tool::gsc::GSC_USEANIMTREE_ITEM);
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::GSC_ANIMTREE_ITEM);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::IW24GSCExport*>(data);
            imp.name = item.name;
            imp.name_space = item.name_space;
            imp.file_name_space = item.file_name_space;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            *reinterpret_cast<tool::gsc::IW23GSCImport*>(data) = item;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(data) = item;
        }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(data) = item;
        }
        int64_t GetDefaultChecksum(bool client) override {
            constexpr int32_t t = 0x2F6BD24A;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<GscObj24>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj24>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/mp/art.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x15;
        }
        byte GetVTableImportFlags() override {
            return 0x15;
        }
    };



    /*****************************************************************************************************************************/

    class T1007GSCOBJHandler : public GSCOBJHandler {
    public:
        T1007GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_NOTIFY_CRC_STRING | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_VAR_VA_COUNT | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            GscObj24* data = Ptr<GscObj24>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk28 :" << std::dec << std::setw(3) << (int)data->unk28 << " (0x" << std::hex << data->unk28 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj24>();

            if (opt.m_test_header) {
                // nothing
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj24>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj24>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj24>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj24>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj24>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj24>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj24>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj24>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj24>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj24>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj24>()->string_table;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj24>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj24>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj24>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj24>()->animtree_offset;
        };
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj24>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj24>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj24>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj24);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj24) && Ref<uint64_t>() == 0xa0d43534707;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptMethodThreadCallEndOn / ScriptThreadCallEndOn (same script?)
            case 2: nflags |= METHOD_THREAD; break; // ScriptMethodThreadCall
            case 3: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptThreadCallEndOn (file namespace)
            case 6: nflags |= FUNCTION_THREAD; break; // ScriptThreadCall (namspace?)
            case 4: nflags |= FUNCTION; break; // ScriptFunctionCall (same script? / file namespace)
            case 7: nflags |= FUNCTION; break; // ScriptFunctionCall (namespace?)
            case 5: nflags |= FUNC_METHOD; break; // GetFunction (same namespace?)
            case 8: // params(1) + CallBuiltinFunction
            case 0xA:nflags |= FUNCTION; break; // api call / GetBuiltinFunction
            case 9: // params(1) + CallBuiltinMethod
            case 0xB:nflags |= METHOD; break; // api call / GetBuiltinMethod
            default: nflags |= (flags & 0xF); break; // wtf?
            }

            if (flags & 0x10) nflags |= DEV_CALL;
            if (flags & 0x20) nflags |= ACTS_USE_FULL_NAMESPACE;

            /*
            0x10: Dev import
            0x20: use file namespace
            GetFunction = 0x5
            ScriptFunctionCall = 0x4, 0x7
            ScriptThreadCall = 0x2, 0x6
            ScriptThreadCallEndOn = 0x3, 0x1
            ScriptMethodCall = 0x7
            ScriptMethodThreadCall = 0x2
            ScriptMethodThreadCallEndOn = 0x1
            CallBuiltinFunction = 0x8
            CallBuiltinMethod = 0x9
            GetBuiltinFunction = 0xa
            GetBuiltinMethod = 0xb
            */

            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 0x5; break;
            case FUNCTION: nflags |= 0x4; break;
            case METHOD: nflags |= 0x7; break;
            case METHOD_CHILDTHREAD:
            case FUNCTION_CHILDTHREAD: nflags |= 0x1; break;
            case FUNCTION_THREAD:
            case METHOD_THREAD: nflags |= 0x2; break;
            case ACTS_CALL_BUILTIN_FUNCTION: nflags |= 0x8; break;
            case ACTS_CALL_BUILTIN_METHOD: nflags |= 0x9; break;
            case ACTS_GET_BUILTIN_FUNCTION: nflags |= 0xa; break;
            case ACTS_GET_BUILTIN_METHOD: nflags |= 0xb; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags == 0x15) return tool::gsc::CLASS_VTABLE;
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }
            if (flags & 0x20) {
                nflags |= T8GSCExportFlags::EVENT;
            }
            if (flags & 0x40) {
                nflags |= T8GSCExportFlags::VE;
            }

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags = 0;
            if (flags == tool::gsc::CLASS_VTABLE) return 0x15;

            if (flags & AUTOEXEC) nflags |= 1;
            if (flags & LINKED) nflags |= 2;
            if (flags & PRIVATE) nflags |= 4;
            if (flags & EVENT) nflags |= 0x20;
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }

        void SetName(uint64_t name) override {
            Ptr<GscObj24>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d43534707;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj24>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj24>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj24>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj24>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj24>()->size1 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj24>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj24>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj24>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj24>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(tool::gsc::IW23GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::IW24GSCExport);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return sizeof(tool::gsc::GSC_USEANIMTREE_ITEM);
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::GSC_ANIMTREE_ITEM);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::IW24GSCExport*>(data);
            imp.name = item.name;
            imp.name_space = item.name_space;
            imp.file_name_space = item.file_name_space;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            *reinterpret_cast<tool::gsc::IW23GSCImport*>(data) = item;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(data) = item;
        }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(data) = item;
        }
        int64_t GetDefaultChecksum(bool client) override {
            constexpr int32_t t = 0x2F6BD24A;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<GscObj24>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj24>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/mp/art.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x15;
        }
        byte GetVTableImportFlags() override {
            return 0x15;
        }
    };


    /*****************************************************************************************************************************/

    class T100CGSCOBJHandler : public GSCOBJHandler {
    public:
        T100CGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_NOTIFY_CRC_STRING | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_VAR_VA_COUNT | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            GscObj24* data = Ptr<GscObj24>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk28 :" << std::dec << std::setw(3) << (int)data->unk28 << " (0x" << std::hex << data->unk28 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj24>();

            if (opt.m_test_header) {
                // nothing
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj24>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj24>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj24>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj24>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj24>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj24>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj24>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj24>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj24>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj24>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj24>()->string_table;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj24>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj24>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj24>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj24>()->animtree_offset;
        };
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj24>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj24>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj24>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj24);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj24) && Ref<uint64_t>() == 0xa0d4353470C;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptMethodThreadCallEndOn / ScriptThreadCallEndOn (same script?)
            case 2: nflags |= METHOD_THREAD; break; // ScriptMethodThreadCall
            case 3: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptThreadCallEndOn (file namespace)
            case 6: nflags |= FUNCTION_THREAD; break; // ScriptThreadCall (namspace?)
            case 4: nflags |= FUNCTION; break; // ScriptFunctionCall (same script? / file namespace)
            case 7: nflags |= FUNCTION; break; // ScriptFunctionCall (namespace?)
            case 5: nflags |= FUNC_METHOD; break; // GetFunction (same namespace?)
            case 8: // params(1) + CallBuiltinFunction
            case 0xA:nflags |= FUNCTION; break; // api call / GetBuiltinFunction
            case 9: // params(1) + CallBuiltinMethod
            case 0xB:nflags |= METHOD; break; // api call / GetBuiltinMethod
            default: nflags |= flags & 0xF; break; // wtf?
            }

            if (flags & 0x10) nflags |= DEV_CALL;
            if (flags & 0x20) nflags |= ACTS_USE_FULL_NAMESPACE;

            /*
            0x10: Dev import
            0x20: use file namespace
            GetFunction = 0x5
            ScriptFunctionCall = 0x4, 0x7
            ScriptThreadCall = 0x2, 0x6
            ScriptThreadCallEndOn = 0x3, 0x1
            ScriptMethodCall = 0x7
            ScriptMethodThreadCall = 0x2
            ScriptMethodThreadCallEndOn = 0x1
            CallBuiltinFunction = 0x8
            CallBuiltinMethod = 0x9
            GetBuiltinFunction = 0xa
            GetBuiltinMethod = 0xb
            */

            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 0x5; break;
            case FUNCTION: nflags |= 0x4; break;
            case METHOD: nflags |= 0x7; break;
            case METHOD_CHILDTHREAD:
            case FUNCTION_CHILDTHREAD: nflags |= 0x1; break;
            case FUNCTION_THREAD:
            case METHOD_THREAD: nflags |= 0x2; break;
            case ACTS_CALL_BUILTIN_FUNCTION: nflags |= 0x8; break;
            case ACTS_CALL_BUILTIN_METHOD: nflags |= 0x9; break;
            case ACTS_GET_BUILTIN_FUNCTION: nflags |= 0xa; break;
            case ACTS_GET_BUILTIN_METHOD: nflags |= 0xb; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags == 0x15) return tool::gsc::CLASS_VTABLE;
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }
            if (flags & 0x20) {
                nflags |= T8GSCExportFlags::EVENT;
            }
            if (flags & 0x40) {
                nflags |= T8GSCExportFlags::VE;
            }

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags = 0;
            if (flags == tool::gsc::CLASS_VTABLE) return 0x15;

            if (flags & AUTOEXEC) nflags |= 1;
            if (flags & LINKED) nflags |= 2;
            if (flags & PRIVATE) nflags |= 4;
            if (flags & EVENT) nflags |= 0x20;
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }

        void SetName(uint64_t name) override {
            Ptr<GscObj24>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d4353470C;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj24>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj24>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj24>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj24>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj24>()->size1 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj24>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj24>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj24>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj24>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(tool::gsc::IW23GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::IW24GSCExport2);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return sizeof(tool::gsc::GSC_USEANIMTREE_ITEM);
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::GSC_ANIMTREE_ITEM);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::IW24GSCExport2*>(data);
            imp.name = item.name;
            imp.name_space = item.name_space;
            imp.file_name_space = item.file_name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            *reinterpret_cast<tool::gsc::IW23GSCImport*>(data) = item;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(data) = item;
        }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(data) = item;
        }
        int64_t GetDefaultChecksum(bool client) override {
            constexpr int32_t t = 0x2F6BD24A;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<GscObj24>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj24>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/mp/art.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x15;
        }
        byte GetVTableImportFlags() override {
            return 0x15;
        }
    };


    
    /*****************************************************************************************************************************/

    class T100BGSCOBJHandler : public GSCOBJHandler {
    public:
        T100BGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_NOTIFY_CRC_STRING | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_VAR_VA_COUNT | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            GscObj24* data = Ptr<GscObj24>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk28 :" << std::dec << std::setw(3) << (int)data->unk28 << " (0x" << std::hex << data->unk28 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj24>();

            if (opt.m_test_header) {
                // nothing
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj24>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj24>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj24>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj24>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj24>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj24>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj24>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj24>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj24>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj24>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj24>()->string_table;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj24>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj24>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj24>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj24>()->animtree_offset;
        };
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj24>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj24>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj24>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj24);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj24) && Ref<uint64_t>() == 0xa0d4353470B;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case 1: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptMethodThreadCallEndOn / ScriptThreadCallEndOn (same script?)
            case 2: nflags |= METHOD_THREAD; break; // ScriptMethodThreadCall
            case 3: nflags |= FUNCTION_CHILDTHREAD; break; // ScriptThreadCallEndOn (file namespace)
            case 6: nflags |= FUNCTION_THREAD; break; // ScriptThreadCall (namspace?)
            case 4: nflags |= FUNCTION; break; // ScriptFunctionCall (same script? / file namespace)
            case 7: nflags |= FUNCTION; break; // ScriptFunctionCall (namespace?)
            case 5: nflags |= FUNC_METHOD; break; // GetFunction (same namespace?)
            case 8: // params(1) + CallBuiltinFunction
            case 0xA:nflags |= FUNCTION; break; // api call / GetBuiltinFunction
            case 9: // params(1) + CallBuiltinMethod
            case 0xB:nflags |= METHOD; break; // api call / GetBuiltinMethod
            default: nflags |= flags & 0xF; break; // wtf?
            }

            if (flags & 0x10) nflags |= DEV_CALL;
            if (flags & 0x20) nflags |= ACTS_USE_FULL_NAMESPACE;

            /*
            0x10: Dev import
            0x20: use file namespace
            GetFunction = 0x5
            ScriptFunctionCall = 0x4, 0x7
            ScriptThreadCall = 0x2, 0x6
            ScriptThreadCallEndOn = 0x3, 0x1
            ScriptMethodCall = 0x7
            ScriptMethodThreadCall = 0x2
            ScriptMethodThreadCallEndOn = 0x1
            CallBuiltinFunction = 0x8
            CallBuiltinMethod = 0x9
            GetBuiltinFunction = 0xa
            GetBuiltinMethod = 0xb
            */

            return nflags;
        }

        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};
            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= 0x5; break;
            case FUNCTION: nflags |= 0x4; break;
            case METHOD: nflags |= 0x7; break;
            case METHOD_CHILDTHREAD:
            case FUNCTION_CHILDTHREAD: nflags |= 0x1; break;
            case FUNCTION_THREAD:
            case METHOD_THREAD: nflags |= 0x2; break;
            case ACTS_CALL_BUILTIN_FUNCTION: nflags |= 0x8; break;
            case ACTS_CALL_BUILTIN_METHOD: nflags |= 0x9; break;
            case ACTS_GET_BUILTIN_FUNCTION: nflags |= 0xa; break;
            case ACTS_GET_BUILTIN_METHOD: nflags |= 0xb; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }
            if (flags & DEV_CALL) nflags |= 0x10;
            if (flags & ACTS_USE_FULL_NAMESPACE) nflags |= 0x20;

            return nflags;
        }

        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags == 0x15) return tool::gsc::CLASS_VTABLE;
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }
            if (flags & 0x20) {
                nflags |= T8GSCExportFlags::EVENT;
            }
            if (flags & 0x40) {
                nflags |= T8GSCExportFlags::VE;
            }

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags = 0;
            if (flags == tool::gsc::CLASS_VTABLE) return 0x15;

            if (flags & AUTOEXEC) nflags |= 1;
            if (flags & LINKED) nflags |= 2;
            if (flags & PRIVATE) nflags |= 4;
            if (flags & EVENT) nflags |= 0x20;
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }

        void SetName(uint64_t name) override {
            Ptr<GscObj24>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d4353470B;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj24>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj24>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj24>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj24>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj24>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj24>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj24>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj24>()->size1 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj24>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj24>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj24>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj24>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj24>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj24>()->animtree_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(tool::gsc::IW23GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::IW24GSCExport2);
        }
        size_t GetStringSize() override {
            return sizeof(tool::gsc::T8GSCString);
        }
        size_t GetGVarSize() override {
            return 0;
        }
        size_t GetAnimTreeSingleSize() override {
            return sizeof(tool::gsc::GSC_USEANIMTREE_ITEM);
        }
        size_t GetAnimTreeDoubleSize() override {
            return sizeof(tool::gsc::GSC_ANIMTREE_ITEM);
        }
        void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) override {
            auto& imp = *reinterpret_cast<tool::gsc::IW24GSCExport2*>(data);
            imp.name = item.name;
            imp.name_space = item.name_space;
            imp.file_name_space = item.file_name_space;
            imp.checksum = (uint32_t)item.checksum;
            imp.flags = item.flags;
            imp.address = item.address;
            imp.param_count = item.param_count;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            *reinterpret_cast<tool::gsc::IW23GSCImport*>(data) = item;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            *reinterpret_cast<tool::gsc::T8GSCString*>(data) = item;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(data) = item;
        }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(data) = item;
        }
        int64_t GetDefaultChecksum(bool client) override {
            constexpr int32_t t = 0x2F6BD24A;
            return t;
        }
        void SetChecksum(uint64_t val) override {
            Ptr<GscObj24>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj24>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "";
            }
            return "scripts/mp/art.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x15;
        }
        byte GetVTableImportFlags() override {
            return 0x15;
        }
    };

}
REGISTER_GSC_VM(VMI_T10_06, T1006GSCOBJHandler);
REGISTER_GSC_VM(VMI_T10_07, T1007GSCOBJHandler);
REGISTER_GSC_VM(VMI_T10_0B, T100BGSCOBJHandler);
REGISTER_GSC_VM(VMI_T10_0C, T100CGSCOBJHandler);
