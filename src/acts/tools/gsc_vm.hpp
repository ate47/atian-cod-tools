#include "gsc.hpp"
#include "gsc_opcodes.hpp"

/*****************************************************************************************************************************/

class T8GSCOBJHandler : public GSCOBJHandler {
public:
    T8GSCOBJHandler(byte* file) : GSCOBJHandler(file, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_FOREACH_TYPE_T8) {}

    void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
        auto* data = Ptr<T8GSCOBJ>();
        asmout
            << std::hex
            << "// crc: 0x" << std::hex << data->crc << "\n"
            << std::left << std::setfill(' ')
            << "// size ..... " << std::dec << std::setw(3) << data->script_size << "\n"
            << "// includes . " << std::dec << std::setw(3) << data->include_count << " (offset: 0x" << std::hex << data->include_offset << ")\n"
            << "// strings .. " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_offset << ")\n"
            << "// exports .. " << std::dec << std::setw(3) << data->exports_count << " (offset: 0x" << std::hex << data->export_table_offset << ")\n"
            << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->imports_offset << ")\n"
            << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
            << "// fixups ... " << std::dec << std::setw(3) << data->fixup_count << " (offset: 0x" << std::hex << data->fixup_offset << ")\n"
            << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << "\n"
            << std::right
            << std::flush;

        if (opt.m_test_header) {
            asmout
                << "// ukn0c .... " << std::dec << data->pad << " / 0x" << std::hex << data->pad << "\n"
                << "// ukn2c .... " << std::dec << data->ukn2c << " / 0x" << std::hex << data->ukn2c << "\n"
                << "// ukn34 .... " << std::dec << data->ukn34 << " / 0x" << std::hex << data->ukn34 << "\n"
                << "// ukn50 .... " << std::dec << data->ukn50 << " / 0x" << std::hex << data->ukn50 << "\n"
                << "// ukn5a .... " << std::dec << (int)data->ukn5a << " / 0x" << std::hex << (int)data->ukn5a << "\n"
                ;
        }
    }
    void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt) override {
        auto* data = Ptr<T8GSCOBJ>();

        // no clue what this thing is doing
        uint64_t* requires_implements_table = reinterpret_cast<uint64_t*>(&data->magic[data->requires_implements_offset]);

        for (size_t i = 0; i < data->requires_implements_count; i++) {
            asmout << "#precache(\"requires_implements\" #\"" << hashutils::ExtractTmp("hash", requires_implements_table[i]) << "\");\n";
        }

        if (data->requires_implements_count) {
            asmout << "\n";
        }

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
        return Ptr<T8GSCOBJ>()->name;
    }
    uint16_t GetExportsCount() override {
        return Ptr<T8GSCOBJ>()->exports_count;
    }
    uint32_t GetExportsOffset() override {
        return Ptr<T8GSCOBJ>()->export_table_offset;
    }
    uint16_t GetIncludesCount() override {
        return Ptr<T8GSCOBJ>()->include_count;
    }
    uint32_t GetIncludesOffset() override {
        return Ptr<T8GSCOBJ>()->include_offset;
    }
    uint16_t GetImportsCount() override {
        return Ptr<T8GSCOBJ>()->imports_count;
    }
    uint32_t GetImportsOffset() override {
        return Ptr<T8GSCOBJ>()->imports_offset;
    }
    uint16_t GetGVarsCount() override {
        return Ptr<T8GSCOBJ>()->globalvar_count;
    }
    uint32_t GetGVarsOffset() override {
        return Ptr<T8GSCOBJ>()->globalvar_offset;
    }
    uint16_t GetStringsCount() override {
        return Ptr<T8GSCOBJ>()->string_count;
    }
    uint32_t GetStringsOffset() override {
        return Ptr<T8GSCOBJ>()->string_offset;
    }
    uint32_t GetFileSize() override {
        return Ptr<T8GSCOBJ>()->script_size;
    }
    size_t GetHeaderSize() override {
        return sizeof(T8GSCOBJ);
    }
    char* DecryptString(char* str) override {
        return decrypt::DecryptString(str);
    }
    bool IsValidHeader(size_t size) override {
        return size >= sizeof(T8GSCOBJ) && *reinterpret_cast<uint64_t*>(file) == 0x36000a0d43534780;
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
        Ptr<T8GSCOBJ>()->name = name;
    }
    void SetHeader() override {
        Ref<uint64_t>() = 0x36000a0d43534780;
    }
    void SetExportsCount(uint16_t val) override {
        Ptr<T8GSCOBJ>()->exports_count = val;
    }
    void SetExportsOffset(uint32_t val) override {
        Ptr<T8GSCOBJ>()->export_table_offset = val;
    }
    void SetIncludesCount(uint16_t val) override {
        Ptr<T8GSCOBJ>()->include_count = val;
    }
    void SetIncludesOffset(uint32_t val) override {
        Ptr<T8GSCOBJ>()->include_offset = val;
    }
    void SetImportsCount(uint16_t val) override {
        Ptr<T8GSCOBJ>()->imports_count = val;
    }
    void SetImportsOffset(uint32_t val) override {
        Ptr<T8GSCOBJ>()->imports_offset = val;
    }
    void SetStringsCount(uint16_t val) override {
        Ptr<T8GSCOBJ>()->string_count = val;
    }
    void SetStringsOffset(uint32_t val) override {
        Ptr<T8GSCOBJ>()->string_offset = val;
    }
    void SetGVarsCount(uint16_t val) override {
        Ptr<T8GSCOBJ>()->globalvar_count = val;
    }
    void SetGVarsOffset(uint32_t val) override {
        Ptr<T8GSCOBJ>()->globalvar_offset = val;
    }
    void SetFileSize(uint32_t val) override {
        Ptr<T8GSCOBJ>()->script_size = val;
    }
    void SetCSEGOffset(uint16_t val) override {
        Ptr<T8GSCOBJ>()->cseg_offset = val;
    }
    void SetCSEGSize(uint32_t val) override {
        Ptr<T8GSCOBJ>()->cseg_size = val;
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
        constexpr int32_t t = 0x636e9d38;
        return t;
    }
    void SetChecksum(uint64_t val) override {
        Ptr<T8GSCOBJ>()->crc = (uint32_t)val;
    }
    const char* GetDefaultName(bool client) override {
        if (client) {
            return "";
        }
        return "scripts/core_common/clientids_shared.gsc";
    }
};


/*****************************************************************************************************************************/

class T937GSCOBJHandler : public GSCOBJHandler {
public:
    T937GSCOBJHandler(byte* file) : GSCOBJHandler(file, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_NOTIFY_CRC | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_SUPPORT_VAR_PTR | GOHF_FOREACH_TYPE_T9) {}

    void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
        auto* data = Ptr<T937GSCOBJ>();
        asmout
            << std::hex
            << "// crc: 0x" << std::hex << data->crc << "\n"
            << std::left << std::setfill(' ')
            << "// size ..... " << std::dec << std::setw(3) << data->file_size << " (0x" << std::hex << data->file_size << ")" << "\n"
            << "// includes . " << std::dec << std::setw(3) << data->includes_count << " (offset: 0x" << std::hex << data->includes_table << ")\n"
            << "// strings .. " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_offset << ")\n"
            << "// exports .. " << std::dec << std::setw(3) << data->export_count << " (offset: 0x" << std::hex << data->exports_tables << ")\n"
            << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->imports_offset << ")\n"
            << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
            << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << "\n"
            << std::right
            << std::flush;

        if (opt.m_test_header) {
            asmout
                << "// ukn0c .... " << std::dec << data->pad0c << " / 0x" << std::hex << data->pad0c << "\n"
                << "// unk2c .... " << std::dec << data->unk2c << " / 0x" << std::hex << data->unk2c << "\n"
                << "// unk3a .... " << std::dec << data->unk3a << " / 0x" << std::hex << data->unk3a << "\n"
                << "// unk48 .... " << std::dec << data->unk48 << " / 0x" << std::hex << data->unk48 << "\n"
                << "// unk52 .... " << std::dec << data->unk52 << " / 0x" << std::hex << data->unk52 << "\n"
                << "// unk54 .... " << std::dec << data->unk54 << " / 0x" << std::hex << data->unk54 << "\n"
                ;
        }
    }
    void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt) override {
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
    uint32_t GetFileSize() override {
        return Ptr<T937GSCOBJ>()->file_size;
    }
    size_t GetHeaderSize() override {
        return sizeof(T937GSCOBJ);
    }
    char* DecryptString(char* str) override {
        return cw::DecryptString(str);
    }
    bool IsValidHeader(size_t size) override {
        return size >= sizeof(T937GSCOBJ) && *reinterpret_cast<uint64_t*>(file) == 0x37000a0d43534780;
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
    void SetGVarsCount(uint16_t val) override {
        Ptr<T937GSCOBJ>()->globalvar_count = val;
    }
    void SetGVarsOffset(uint32_t val) override {
        Ptr<T937GSCOBJ>()->globalvar_offset = val;
    }
    void SetFileSize(uint32_t val) override {
        Ptr<T937GSCOBJ>()->file_size = val;
    }
    void SetCSEGOffset(uint16_t val) override {
        Ptr<T937GSCOBJ>()->cseg_offset = val;
    }
    void SetCSEGSize(uint32_t val) override {
        Ptr<T937GSCOBJ>()->cseg_size = val;
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
    const char* GetDefaultName(bool client) override {
        if (client) {
            return "";
        }
        return "scripts/core_common/clientids_shared.gsc";
    }
};


/*****************************************************************************************************************************/

class T9GSCOBJHandler : public GSCOBJHandler {
public:
    T9GSCOBJHandler(byte* file) : GSCOBJHandler(file, GOHF_GLOBAL | GOHF_INLINE_FUNC_PTR | GOHF_NOTIFY_CRC | GOHF_SUPPORT_EV_HANDLER | GOHF_SUPPORT_VAR_VA | GOHF_SUPPORT_VAR_REF | GOHF_SUPPORT_VAR_PTR | GOHF_FOREACH_TYPE_T9) {}

    void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
        auto* data = Ptr<T9GSCOBJ>();
        asmout
            << std::hex
            << "// crc: 0x" << std::hex << data->crc << "\n"
            << std::left << std::setfill(' ')
            << "// size ..... " << std::dec << std::setw(3) << data->file_size << " (0x" << std::hex << data->file_size << ")" << "\n"
            << "// includes . " << std::dec << std::setw(3) << data->includes_count << " (offset: 0x" << std::hex << data->includes_table << ")\n"
            << "// strings .. " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_offset << ")\n"
            << "// exports .. " << std::dec << std::setw(3) << data->exports_count << " (offset: 0x" << std::hex << data->exports_tables << ")\n"
            << "// imports .. " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->import_tables << ")\n"
            << "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
            << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << "\n"
            << std::right
            << std::flush;

        if (opt.m_test_header) {
            asmout
                << "// ukn0c .... " << std::dec << data->pad0c << " / 0x" << std::hex << data->pad0c << "\n"
                << "// unk1e .... " << std::dec << data->unk1e << " / 0x" << std::hex << data->unk1e << "\n"
                << "// unk22 .... " << std::dec << data->unk22 << " / 0x" << std::hex << data->unk22 << "\n"
                << "// unk26 .... " << std::dec << data->unk26 << " / 0x" << std::hex << data->unk26 << "\n"
                << "// unk28 .... " << std::dec << data->unk28 << " / 0x" << std::hex << data->unk28 << "\n" // offset
                << "// unk40 .... " << std::dec << data->unk40 << " / 0x" << std::hex << data->unk40 << "\n" // offset
                << "// unk4c .... " << std::dec << data->unk4c << " / 0x" << std::hex << data->unk4c << "\n"
                << "// unk54 .... " << std::dec << data->unk54 << " / 0x" << std::hex << data->unk54 << "\n"
                ;
        }
    }
    void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt) override {
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
    uint32_t GetFileSize() override {
        return Ptr<T9GSCOBJ>()->file_size;
    }
    size_t GetHeaderSize() override {
        return sizeof(T9GSCOBJ);
    }
    char* DecryptString(char* str) override {
        return cw::DecryptString(str);
    }
    bool IsValidHeader(size_t size) override {
        return size >= sizeof(T9GSCOBJ) && *reinterpret_cast<uint64_t*>(file) == 0x38000a0d43534780;
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

    byte MapFlagsImportToInt(byte flags) {
        byte nflags = 0;

        switch (flags & T9_IF_CALLTYPE_MASK) {
        case METHOD_CHILDTHREAD: nflags |= T9_IF_METHOD_CHILDTHREAD; break;
        case METHOD_THREAD: nflags |= T9_IF_METHOD_THREAD; break;
        case FUNCTION_CHILDTHREAD: nflags |= T9_IF_FUNCTION_CHILDTHREAD; break;
        case FUNCTION: nflags |= T9_IF_FUNCTION; break;
        case FUNC_METHOD: nflags |= T9_IF_FUNC_METHOD; break;
        case FUNCTION_THREAD: nflags |= T9_IF_FUNCTION_THREAD; break;
        case METHOD: nflags |= T9_IF_METHOD; break;
        default: nflags |= flags & 0xF; // wtf?
        }

        if (flags & DEV_CALL) nflags |= T9_IF_DEV_CALL;
        if (flags & GET_CALL) nflags |= T9_IF_GET_CALL;

        return nflags;
    }

    byte MapFlagsExportToInt(byte flags) {
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
    void SetGVarsCount(uint16_t val) override {
        Ptr<T9GSCOBJ>()->globalvar_count = val;
    }
    void SetGVarsOffset(uint32_t val) override {
        Ptr<T9GSCOBJ>()->globalvar_offset = val;
    }
    void SetFileSize(uint32_t val) override {
        Ptr<T9GSCOBJ>()->file_size = val;
    }
    void SetCSEGOffset(uint16_t val) override {
        Ptr<T9GSCOBJ>()->cseg_offset = val;
    }
    void SetCSEGSize(uint32_t val) override {
        Ptr<T9GSCOBJ>()->cseg_size = val;
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
    const char* GetDefaultName(bool client) override {
        if (client) {
            return "";
        }
        return "scripts/core_common/clientids_shared.gsc";
    }
};


/*****************************************************************************************************************************/

class MW23GSCOBJHandler : public GSCOBJHandler {
public:
    MW23GSCOBJHandler(byte* file) : GSCOBJHandler(file, GOHF_ANIMTREE | GOHF_ANIMTREE_DOUBLE | GOHF_FOREACH_TYPE_JUP) {}

    void DumpHeader(std::ostream& asmout, const GscInfoOption& opt) override {
        auto* data = Ptr<GscObj23>();
        asmout
            << std::left << std::setfill(' ')
            << "// size ...... " << std::dec << std::setw(3) << data->size1 << " (0x" << std::hex << data->size1 << ")" << "\n"
            << "// includes .. " << std::dec << std::setw(3) << data->includes_count << " (offset: 0x" << std::hex << data->include_table << ")\n"
            << "// strings ... " << std::dec << std::setw(3) << data->string_count << " (offset: 0x" << std::hex << data->string_table << ")\n"
            << "// exports ... " << std::dec << std::setw(3) << data->export_count << " (offset: 0x" << std::hex << data->export_offset << ")\n"
            << "// imports ... " << std::dec << std::setw(3) << data->imports_count << " (offset: 0x" << std::hex << data->import_table << ")\n"
            << "// animtree1 . " << std::dec << std::setw(3) << data->animtree_use_count << " (offset: 0x" << std::hex << data->animtree_use_offset << ")\n"
            << "// animtree2 . " << std::dec << std::setw(3) << data->animtree_count << " (offset: 0x" << std::hex << data->animtree_offset << ")\n"
            //<< "// globals .. " << std::dec << std::setw(3) << data->globalvar_count << " (offset: 0x" << std::hex << data->globalvar_offset << ")\n"
            << "// cseg ..... 0x" << std::hex << data->cseg_offset << " + 0x" << std::hex << data->cseg_size << "\n"
            << std::right
            << std::flush;

        if (opt.m_test_header) {
            // fillme
            asmout
                << "unk16 :" << std::dec << std::setw(3) << (int)data->unk16 << " (0x" << std::hex << data->unk16 << ")\n"
                << "unk1C :" << std::dec << std::setw(3) << (int)data->unk1C << " (0x" << std::hex << data->unk1C << ")\n"
                << "unk22 :" << std::dec << std::setw(3) << (int)data->unk22 << " (0x" << std::hex << data->unk22 << ")\n"
                << "unk26 :" << std::dec << std::setw(3) << (int)data->unk26 << " (0x" << std::hex << data->unk26 << ")\n"
                << "unk28 :" << std::dec << std::setw(3) << (int)data->unk28 << " (0x" << std::hex << data->unk28 << ")\n"
                << "unk2A :" << std::dec << std::setw(3) << (int)data->unk2A << " (0x" << std::hex << data->unk2A << ")\n"
                << "unk3C :" << std::dec << std::setw(3) << (int)data->unk3C << " (0x" << std::hex << data->unk3C << ")\n"
                << "unk48 :" << std::dec << std::setw(3) << (int)data->size1 << " (0x" << std::hex << data->size1 << ")\n"
                << "unk54 :" << std::dec << std::setw(3) << (int)data->size2 << " (0x" << std::hex << data->size2 << ")\n"
                << "unk5C :" << std::dec << std::setw(3) << (int)data->unk5C << " (0x" << std::hex << data->unk5C << ")\n"
                ;
        }
    }
    void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt) override {
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
    uint32_t GetFileSize() override {
        return Ptr<GscObj23>()->size1;
    }
    size_t GetHeaderSize() override {
        return sizeof(GscObj23);
    }
    char* DecryptString(char* str) override {
        return str; // iw
    }
    bool IsValidHeader(size_t size) override {
        return size >= sizeof(GscObj23) && *reinterpret_cast<uint64_t*>(file) == 0xa0d4353478a;
    }
    byte RemapFlagsImport(byte flags) override {
        byte nflags = 0;

        switch (flags & 0xF) {
        case 5: nflags |= FUNC_METHOD; break;
        case 4: nflags |= FUNCTION; break;
        case 2: nflags |= FUNCTION_THREAD; break;
        case 1: nflags |= FUNCTION_CHILDTHREAD; break;

        case 3:
        case 6:
        case 7: nflags |= FUNCTION; break; // TODO: unk script calls
        case 8:
        case 0xA:nflags |= FUNCTION; break; // api call
        case 9:
        case 0xB:nflags |= METHOD; break; // api call
        default: nflags |= flags & 0xF; // wtf?
        }

        // 0x10: Dev import
        // 0x20: use file namespace
        nflags |= flags & ~0xF;

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

    byte MapFlagsImportToInt(byte flags) {
        return flags; // TODO
    }

    byte MapFlagsExportToInt(byte flags) {
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
    void SetFileSize(uint32_t val) override {
        // idk
        Ptr<GscObj23>()->size1 = val;
        Ptr<GscObj23>()->size2 = val;
    }
    void SetCSEGOffset(uint16_t val) override {
        Ptr<GscObj23>()->cseg_offset = val;
    }
    void SetCSEGSize(uint32_t val) override {
        Ptr<GscObj23>()->cseg_size = val;
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
    void SetChecksum(uint64_t val) override { }
    const char* GetDefaultName(bool client) override {
        return ""; // idc
    }
};
