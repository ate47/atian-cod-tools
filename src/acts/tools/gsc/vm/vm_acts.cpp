#include <includes.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <acts_vm.hpp>


// Acts VM

namespace {
    using namespace acts::vm;
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class ACTSF1GSCOBJHandler : public GSCOBJHandler {
    public:
        ACTSF1GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_FOREACH_TYPE_5 | GOHF_STRING_NAMES) {}

        uint64_t GetName() override {
            const char* name{ Ptr<ActScript>()->GetName() };
            hashutils::Add(name);
            return hash::Hash64(name);
        }
        void SetName(uint64_t name) override {
            throw std::runtime_error("can't get string name for this vm");
        }
        void SetNameString(uint32_t name) override {
            Ptr<ActScript>()->name_offset = name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<ActScript>()->exports_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<ActScript>()->exports_table;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<ActScript>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<ActScript>()->includes_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<ActScript>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<ActScript>()->imports_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj23>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj23>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<ActScript>()->strings_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<ActScript>()->strings_table;
        }
        uint16_t GetDevStringsCount() override {
            return 0; // return Ptr<ActScript>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return 0; // return Ptr<ActScript>()->devblock_string_offset;
        }
        size_t GetHeaderSize() override {
            return sizeof(ActScript);
        }
        char* DecryptString(char* str) override {
            return str;
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(ActScript) && Ref<uint64_t>() == ACTSCRIPT_MAGIC;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return 0; // Ptr<ActScript>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return 0; // Ptr<ActScript>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return 0; // Ptr<ActScript>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return 0; // Ptr<ActScript>()->animtree_offset;
        };

        void SetHeader() override {
            Ref<uint64_t>() = ACTSCRIPT_MAGIC;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<ActScript>()->exports_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<ActScript>()->exports_table = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<ActScript>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<ActScript>()->includes_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<ActScript>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<ActScript>()->imports_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<ActScript>()->strings_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<ActScript>()->strings_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            //Ptr<ActScript>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            //Ptr<ActScript>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            Ptr<ActScript>()->file_size = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<ActScript>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<ActScript>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<ActScript>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<ActScript>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            //Ptr<ActScript>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            //Ptr<ActScript>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            //Ptr<ActScript>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            //Ptr<ActScript>()->animtree_offset = val;
        }
        void SetGVarsCount(uint16_t val) override {}
        void SetGVarsOffset(uint32_t val) override {}

        size_t GetImportSize() override {
            return sizeof(ScriptImport);
        }
        size_t GetExportSize() override {
            return sizeof(ScriptExport);
        }
        size_t GetStringSize() override {
            return sizeof(ScriptString);
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
            ScriptExport* exp{ (ScriptExport*)data };
            exp->address = item.address;
            exp->checksum = (uint32_t)item.checksum;
            exp->name = item.name;
            exp->name_space = item.name_space;
            exp->data = item.file_name_space;
            exp->param_count = item.param_count;
            exp->flags = item.flags;
        }
        void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) override {
            ScriptImport* imp{ (ScriptImport*)data };
            imp->name = item.name;
            imp->name_space = item.name_space;
            imp->num_address = item.num_address;
            imp->param_count = item.param_count;
            imp->flags = item.flags;
        }
        byte RemapFlagsImport(byte flags) override {
            byte nflags{};
            switch (flags & ScriptImportFlags::SIF_TYPE_MASK) {
            case ScriptImportFlags::SIF_GET: nflags |= FUNC_METHOD; break;
            case ScriptImportFlags::SIF_FUNCTION: nflags |= FUNCTION; break;
            case ScriptImportFlags::SIF_FUNCTION_THREAD: nflags |= FUNCTION_THREAD; break;
            case ScriptImportFlags::SIF_FUNCTION_CHILDTHREAD: nflags |= FUNCTION_CHILDTHREAD; break;
            case ScriptImportFlags::SIF_METHOD: nflags |= METHOD; break;
            case ScriptImportFlags::SIF_METHOD_THREAD: nflags |= METHOD_THREAD; break;
            case ScriptImportFlags::SIF_METHOD_CHILDTHREAD: nflags |= METHOD_CHILDTHREAD; break;
            case ScriptImportFlags::SIF_FUNCTION_BUILTIN: nflags |= ACTS_CALL_BUILTIN_FUNCTION; break;
            case ScriptImportFlags::SIF_METHOD_BUILTIN: nflags |= ACTS_CALL_BUILTIN_METHOD; break;
            case ScriptImportFlags::SIF_GET_BUILTIN: nflags |= ACTS_GET_BUILTIN_FUNCTION; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }
            return nflags;
        }
        byte MapFlagsImportToInt(byte flags) override {
            byte nflags{};

            switch (flags & 0xF) {
            case FUNC_METHOD: nflags |= ScriptImportFlags::SIF_GET; break;
            case FUNCTION: nflags |= ScriptImportFlags::SIF_FUNCTION; break;
            case FUNCTION_THREAD: nflags |= ScriptImportFlags::SIF_FUNCTION_THREAD; break;
            case FUNCTION_CHILDTHREAD: nflags |= ScriptImportFlags::SIF_FUNCTION_CHILDTHREAD; break;
            case METHOD: nflags |= ScriptImportFlags::SIF_METHOD; break;
            case METHOD_THREAD: nflags |= ScriptImportFlags::SIF_METHOD_THREAD; break;
            case METHOD_CHILDTHREAD: nflags |= ScriptImportFlags::SIF_METHOD_CHILDTHREAD; break;
            case ACTS_CALL_BUILTIN_FUNCTION: nflags |= ScriptImportFlags::SIF_FUNCTION_BUILTIN; break;
            case ACTS_CALL_BUILTIN_METHOD: nflags |= ScriptImportFlags::SIF_METHOD_BUILTIN; break;
            case ACTS_GET_BUILTIN_FUNCTION:
            case ACTS_GET_BUILTIN_METHOD: nflags |= ScriptImportFlags::SIF_GET_BUILTIN; break;
            default: nflags |= flags & 0xF; break; // wtf?
            }

            return nflags;
        }
        byte RemapFlagsExport(byte flags) override {
            byte nflags{};
            if (flags & ScriptExportFlags::SEF_AUTOEXEC) nflags |= T8GSCExportFlags::AUTOEXEC;
            if (flags & ScriptExportFlags::SEF_PRIVATE) nflags |= T8GSCExportFlags::PRIVATE;
            return nflags;
        }
        byte MapFlagsExportToInt(byte flags) override {
            byte nflags{};
            if (flags & T8GSCExportFlags::AUTOEXEC) nflags |= ScriptExportFlags::SEF_AUTOEXEC;
            if (flags & T8GSCExportFlags::PRIVATE) nflags |= ScriptExportFlags::SEF_PRIVATE;
            return nflags;
        }
        void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) override {}
        void WriteString(byte* data, const tool::gsc::T8GSCString& item) override {
            ScriptString* str{ (ScriptString*)data };
            str->address = item.string;
            str->num_address = item.num_address;
        }
        void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(data) = item;
        }
        void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) override {
            *reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(data) = item;
        }
        int64_t GetDefaultChecksum(bool client) override {
            return 0; // no checksum
        }
        void SetChecksum(uint64_t val) override {
            Ptr<ActScript>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<ActScript>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            if (client) {
                return "acts/default.csc";
            }
            return "acts/default.gsc";
        }
        bool IsVTableImportFlags(byte flags) override {
            return false;
        }
        byte GetVTableImportFlags() override {
            return 0;
        }
    };

}

REGISTER_GSC_VM(VMI_ACTS_F1, ACTSF1GSCOBJHandler);