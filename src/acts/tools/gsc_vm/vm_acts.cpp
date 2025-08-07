#include <includes.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <acts_vm.hpp>


// Acts VM

namespace {
    using namespace acts::vm;
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class ACTSF1GSCOBJHandler : public GSCOBJHandler {
    public:
        ACTSF1GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_FOREACH_TYPE_5) {}
        
        uint64_t GetName() override {
            return Ptr<ActScript>()->name;
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

        void SetName(uint64_t name) override {
            Ptr<ActScript>()->name = name;
        }
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
            Ptr<ActScript>()->fileSize = val;
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
            return sizeof(tool::gsc::IW23GSCImport);
        }
        size_t GetExportSize() override {
            return sizeof(tool::gsc::IW23GSCExport);
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
            *reinterpret_cast<tool::gsc::IW23GSCExport*>(data) = item;
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
            return 0; // no checksum
        }
        void SetChecksum(uint64_t val) override {
            // Ptr<ActScript>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return 0; // Ptr<GscObj23>()->checksum;
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