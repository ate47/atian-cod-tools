#include <includes.hpp>
#include <decryptutils.hpp>
#include <tools/gsc_vm.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>


// Call of Duty: Modern Warfare III (JUP)

namespace {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;

    class MW23GSCOBJHandler : public GSCOBJHandler {
    public:
        MW23GSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_FILENAMESPACE | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<GscObj23>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk22 :" << std::dec << std::setw(3) << (int)data->unk22 << " (0x" << std::hex << data->unk22 << ")\n"
                    << "unk26 :" << std::dec << std::setw(3) << (int)data->unk26 << " (0x" << std::hex << data->unk26 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    << "unk54 :" << std::dec << std::setw(3) << (int)data->size2 << " (0x" << std::hex << data->size2 << ")\n"
                    << "unk5C :" << std::dec << std::setw(3) << (int)data->unk5C << " (0x" << std::hex << data->unk5C << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj23>();

            if (opt.m_test_header) {
                uintptr_t unk2c_location = reinterpret_cast<uintptr_t>(data->magic) + data->animtree_use_offset;
                for (size_t i = 0; i < data->animtree_use_count; i++) {
                    const auto* unk2c = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(unk2c_location);

                    auto* s = Ptr<char>(unk2c->address);

                    asmout << std::hex << "animtree #" << s << std::endl;

                    hashutils::Add(s, true, true);

                    const auto* vars = reinterpret_cast<const uint32_t*>(&unk2c[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < unk2c->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<uint16_t>(vars[j]) = ref;
                        if (j) asmout << ",";
                        asmout << std::hex << vars[j];
                    }
                    asmout << "\n";
                    unk2c_location += sizeof(*unk2c) + sizeof(*vars) * unk2c->num_address;
                }
                if (data->animtree_use_count) {
                    asmout << "\n";
                }

                uintptr_t animt_location = reinterpret_cast<uintptr_t>(data->magic) + data->animtree_offset;
                for (size_t i = 0; i < data->animtree_count; i++) {
                    const auto* animt = reinterpret_cast<GSC_ANIMTREE_ITEM*>(animt_location);

                    auto* s1 = Ptr<char>(animt->address_str1);
                    auto* s2 = Ptr<char>(animt->address_str2);

                    hashutils::Add(s1, true, true);
                    hashutils::Add(s2, true, true);

                    asmout << std::hex << "animtree " << s1 << "%" << s2 << std::endl;

                    const auto* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < animt->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<uint16_t>(vars[j]) = ref;
                        if (j) asmout << ",";
                        asmout << std::hex << vars[j];
                    }
                    asmout << "\n";
                    animt_location += sizeof(*animt) + sizeof(*vars) * animt->num_address;
                }
                if (data->animtree_count) {
                    asmout << "\n";
                }
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj23>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj23>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj23>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj23>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj23>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj23>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj23>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj23>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj23>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj23>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj23>()->string_table;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj23>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj23>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj23>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj23);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj23) && Ref<uint64_t>() == 0xa0d4353478a;
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
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }
            if (flags & 0x40) {
                nflags |= T8GSCExportFlags::VE;
            }

            return nflags;
        }

        byte MapFlagsExportToInt(byte flags) override {
            byte nflags = 0;

            if (flags & AUTOEXEC) nflags |= 1;
            if (flags & LINKED) nflags |= 2;
            if (flags & PRIVATE) nflags |= 4;
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj23>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj23>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj23>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj23>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            Ptr<GscObj23>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d4353478a;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj23>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj23>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj23>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj23>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj23>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj23>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj23>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj23>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj23>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj23>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj23>()->size1 = val;
            Ptr<GscObj23>()->size2 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj23>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj23>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj23>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj23>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj23>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj23>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj23>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj23>()->animtree_offset = val;
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
            Ptr<GscObj23>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj23>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            return ""; // idc
        }
        bool IsVTableImportFlags(byte flags) override {
            return false;
        }
        byte GetVTableImportFlags() override {
            return 0;
        }
    };


    /*****************************************************************************************************************************/

    class MW23BGSCOBJHandler : public GSCOBJHandler {
    public:
        MW23BGSCOBJHandler(byte* file, size_t fileSize) : GSCOBJHandler(file, fileSize, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP | GOHF_NOTIFY_CRC_STRING | GOHF_FILENAMESPACE | GOHF_SWITCH_TYPE_IW) {}

        void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) override {
            auto* data = Ptr<GscObj23>();
            if (opt.m_test_header) {
                // fillme
                asmout
                    << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                    << "unk22 :" << std::dec << std::setw(3) << (int)data->unk22 << " (0x" << std::hex << data->unk22 << ")\n"
                    << "unk26 :" << std::dec << std::setw(3) << (int)data->unk26 << " (0x" << std::hex << data->unk26 << ")\n"
                    << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                    << "unk54 :" << std::dec << std::setw(3) << (int)data->size2 << " (0x" << std::hex << data->size2 << ")\n"
                    << "unk5C :" << std::dec << std::setw(3) << (int)data->unk5C << " (0x" << std::hex << data->unk5C << ")\n"
                    ;
            }
        }
        void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) override {
            auto* data = Ptr<GscObj23>();

            if (opt.m_test_header) {
                uintptr_t unk2c_location = reinterpret_cast<uintptr_t>(data->magic) + data->animtree_use_offset;
                for (size_t i = 0; i < data->animtree_use_count; i++) {
                    const auto* unk2c = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(unk2c_location);

                    auto* s = DecryptString(Ptr<char>(unk2c->address));

                    asmout << std::hex << "animtree #" << s << std::endl;

                    hashutils::Add(s, true, true);

                    const auto* vars = reinterpret_cast<const uint32_t*>(&unk2c[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < unk2c->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<uint16_t>(vars[j]) = ref;
                        if (j) asmout << ",";
                        asmout << std::hex << vars[j];
                    }
                    asmout << "\n";
                    unk2c_location += sizeof(*unk2c) + sizeof(*vars) * unk2c->num_address;
                }
                if (data->animtree_use_count) {
                    asmout << "\n";
                }

                uintptr_t animt_location = reinterpret_cast<uintptr_t>(data->magic) + data->animtree_offset;
                for (size_t i = 0; i < data->animtree_count; i++) {
                    const auto* animt = reinterpret_cast<GSC_ANIMTREE_ITEM*>(animt_location);

                    auto* s1 = DecryptString(Ptr<char>(animt->address_str1));
                    auto* s2 = DecryptString(Ptr<char>(animt->address_str2));

                    hashutils::Add(s1, true, true);
                    hashutils::Add(s2, true, true);

                    asmout << std::hex << "animtree " << s1 << "%" << s2 << std::endl;

                    const auto* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < animt->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<uint16_t>(vars[j]) = ref;
                        if (j) asmout << ",";
                        asmout << std::hex << vars[j];
                    }
                    asmout << "\n";
                    animt_location += sizeof(*animt) + sizeof(*vars) * animt->num_address;
                }
                if (data->animtree_count) {
                    asmout << "\n";
                }
            }
        }

        uint64_t GetName() override {
            return Ptr<GscObj23>()->name;
        }
        uint16_t GetExportsCount() override {
            return Ptr<GscObj23>()->export_count;
        }
        uint32_t GetExportsOffset() override {
            return Ptr<GscObj23>()->export_offset;
        }
        uint16_t GetIncludesCount() override {
            return Ptr<GscObj23>()->includes_count;
        }
        uint32_t GetIncludesOffset() override {
            return Ptr<GscObj23>()->include_table;
        }
        uint16_t GetImportsCount() override {
            return Ptr<GscObj23>()->imports_count;
        }
        uint32_t GetImportsOffset() override {
            return Ptr<GscObj23>()->import_table;
        }
        uint16_t GetGVarsCount() override {
            return 0; //return Ptr<GscObj23>()->globalvar_count;
        }
        uint32_t GetGVarsOffset() override {
            return 0; //return Ptr<GscObj23>()->globalvar_offset;
        }
        uint16_t GetStringsCount() override {
            return Ptr<GscObj23>()->string_count;
        }
        uint32_t GetStringsOffset() override {
            return Ptr<GscObj23>()->string_table;
        }
        uint16_t GetDevStringsCount() override {
            return Ptr<GscObj23>()->devblock_string_count;
        }
        uint32_t GetDevStringsOffset() override {
            return Ptr<GscObj23>()->devblock_string_offset;
        }
        uint32_t GetFileSize() override {
            return Ptr<GscObj23>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj23);
        }
        char* DecryptString(char* str) override {
            return acts::decryptutils::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj23) && Ref<uint64_t>() == 0xa0d4353478b;
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
            if (flags & VE) nflags |= 0x40;

            return nflags;
        }
        uint16_t GetAnimTreeSingleCount() override {
            return Ptr<GscObj23>()->animtree_use_count;
        };
        uint32_t GetAnimTreeSingleOffset() override {
            return Ptr<GscObj23>()->animtree_use_offset;
        };
        uint16_t GetAnimTreeDoubleCount() override {
            return Ptr<GscObj23>()->animtree_count;
        };
        uint32_t GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj23>()->animtree_offset;
        };

        void SetName(uint64_t name) override {
            Ptr<GscObj23>()->name = name;
        }
        void SetHeader() override {
            Ref<uint64_t>() = 0xa0d4353478b;
        }
        void SetExportsCount(uint16_t val) override {
            Ptr<GscObj23>()->export_count = val;
        }
        void SetExportsOffset(uint32_t val) override {
            Ptr<GscObj23>()->export_offset = val;
        }
        void SetIncludesCount(uint16_t val) override {
            Ptr<GscObj23>()->includes_count = val;
        }
        void SetIncludesOffset(uint32_t val) override {
            Ptr<GscObj23>()->include_table = val;
        }
        void SetImportsCount(uint16_t val) override {
            Ptr<GscObj23>()->imports_count = val;
        }
        void SetImportsOffset(uint32_t val) override {
            Ptr<GscObj23>()->import_table = val;
        }
        void SetStringsCount(uint16_t val) override {
            Ptr<GscObj23>()->string_count = val;
        }
        void SetStringsOffset(uint32_t val) override {
            Ptr<GscObj23>()->string_table = val;
        }
        void SetDevStringsCount(uint16_t val) override {
            Ptr<GscObj23>()->devblock_string_count = val;
        }
        void SetDevStringsOffset(uint32_t val) override {
            Ptr<GscObj23>()->devblock_string_offset = val;
        }
        void SetFileSize(uint32_t val) override {
            // idk
            Ptr<GscObj23>()->size1 = val;
        }
        void SetCSEGOffset(uint32_t val) override {
            Ptr<GscObj23>()->cseg_offset = val;
        }
        void SetCSEGSize(uint32_t val) override {
            Ptr<GscObj23>()->cseg_size = val;
        }
        uint32_t GetCSEGOffset() override {
            return Ptr<GscObj23>()->cseg_offset;
        }
        uint32_t GetCSEGSize() override {
            return Ptr<GscObj23>()->cseg_size;
        }
        void SetAnimTreeSingleCount(uint16_t val) override {
            Ptr<GscObj23>()->animtree_use_count = val;
        }
        void SetAnimTreeSingleOffset(uint32_t val) override {
            Ptr<GscObj23>()->animtree_use_offset = val;
        }
        void SetAnimTreeDoubleCount(uint16_t val) override {
            Ptr<GscObj23>()->animtree_count = val;
        }
        void SetAnimTreeDoubleOffset(uint32_t val) override {
            Ptr<GscObj23>()->animtree_offset = val;
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
            Ptr<GscObj23>()->checksum = (uint32_t)val;
        }
        uint32_t GetChecksum() override {
            return Ptr<GscObj23>()->checksum;
        }
        const char* GetDefaultName(bool client) override {
            return ""; // idc
        }
        bool IsVTableImportFlags(byte flags) override {
            return flags == 0x15;
        }
        byte GetVTableImportFlags() override {
            return 0x15;
        }
    };
}
REGISTER_GSC_VM(VMI_JUP_8A, MW23GSCOBJHandler);
REGISTER_GSC_VM(VMI_JUP_8B, MW23BGSCOBJHandler);