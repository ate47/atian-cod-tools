#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/cw/cw.hpp"

using namespace tool::gsc;

constexpr auto g_constructorName = hashutils::Hash32("__constructor");
constexpr auto g_destructorName = hashutils::Hash32("__destructor");

bool GscInfoOption::Compute(LPCCH* args, INT startIndex, INT endIndex) {
    // default values
    for (size_t i = startIndex; i < endIndex; i++) {
        LPCCH arg = args[i];

        if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
            m_help = true;
        }
        else if (!strcmp("-g", arg) || !_strcmpi("--gsc", arg)) {
            m_dcomp = true;
        }
        else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
            m_dasm = true;
        }
        else if (!strcmp("-s", arg) || !_strcmpi("--silent", arg)) {
            m_verbose = false;
        }
        else if (!strcmp("-H", arg) || !_strcmpi("--header", arg)) {
            m_header = true;
        }
        else if (!strcmp("-I", arg) || !_strcmpi("--imports", arg)) {
            m_imports = true;
        }
        else if (!strcmp("-S", arg) || !_strcmpi("--strings", arg)) {
            m_strings = true;
        }
        else if (!strcmp("-G", arg) || !_strcmpi("--gvars", arg)) {
            m_gvars = true;
        }
        else if (!strcmp("-U", arg) || !_strcmpi("--noincludes", arg)) {
            m_includes = false;
        }
        else if (!strcmp("-X", arg) || !_strcmpi("--exptests", arg)) {
            m_exptests = true;
        }
        else if (!strcmp("-f", arg) || !_strcmpi("--nofunc", arg)) {
            m_func = false;
        }
        else if (!strcmp("-F", arg) || !_strcmpi("--nofuncheader", arg)) {
            m_func_header = false;
        }
        else if (!strcmp("-p", arg) || !_strcmpi("--postfunchead", arg)) {
            m_func_header_post = true;
        }
        else if (!strcmp("-l", arg) || !_strcmpi("--rloc", arg)) {
            m_func_rloc = true;
        }
        else if (!strcmp("-P", arg) || !_strcmpi("--nopatch", arg)) {
            m_patch = false;
        }
        else if (!strcmp("-V", arg) || !_strcmpi("--vars", arg)) {
            m_show_func_vars = true;
        }
        else if (!_strcmpi("--test-header", arg)) {
            m_test_header = true;
        }
        else if (!strcmp("-t", arg) || !_strcmpi("--type", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_platform = opcode::PlatformOf(args[++i]);

            if (!m_platform) {
                std::cerr << "Unknown platform: " << args[i] << "!\n";
                return false;
            }
        }
        else if (!strcmp("-v", arg) || !_strcmpi("--vm", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_vm = opcode::VMOf(args[++i]);

            if (!m_vm) {
                std::cerr << "Unknown vm: " << args[i] << "!\n";
                return false;
            }
        }
        else if (!_strcmpi("--internalblocks", arg)) {
            m_show_internal_blocks = true;
        }
        else if (!_strcmpi("--jumpdelta", arg)) {
            m_show_jump_delta = true;
        }
        else if (!_strcmpi("--prestruct", arg)) {
            m_show_pre_dump = true;
        }
        else if (!_strcmpi("--refcount", arg)) {
            m_show_ref_count = true;
        }
        else if (!strcmp("-i", arg) || !_strcmpi("--ignore", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }

            for (const char* param = args[++i]; *param; param++) {
                switch (*param) {
                case 'd':
                case 'D':
                    m_stepskip |= STEPSKIP_DEV;
                    break;
                case 's':
                case 'S':
                    m_stepskip |= STEPSKIP_SWITCH;
                    break;
                case 'e':
                case 'E':
                    m_stepskip |= STEPSKIP_FOREACH;
                    break;
                case 'w':
                case 'W':
                    m_stepskip |= STEPSKIP_WHILE;
                    break;
                case 'i':
                case 'I':
                    m_stepskip |= STEPSKIP_IF;
                    break;
                case 'f':
                case 'F':
                    m_stepskip |= STEPSKIP_FOR;
                    break;
                default:
                    std::cerr << "Bad param for " << arg << ": '" << *param << "'!\n";
                    return false;
                }
            }
        }
        else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_outputDir = args[++i];
        }
        else if (!strcmp("-m", arg) || !_strcmpi("--hashmap", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_dump_hashmap = args[++i];
        }
        else if (!_strcmpi("--dumpstrings", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_dump_strings = args[++i];
        }
        else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_copyright = args[++i];
        }
        else if (!strcmp("-r", arg) || !_strcmpi("--rosetta", arg)) {
            if (i + 1 == endIndex) {
                std::cerr << "Missing value for param: " << arg << "!\n";
                return false;
            }
            m_rosetta = args[++i];
        }

        else if (*arg == '-') {
            std::cerr << "Unknown option: " << arg << "!\n";
            return false;
        }
        else {
            m_inputFiles.push_back(arg);
        }
    }
    if (!m_inputFiles.size()) {
        m_inputFiles.push_back("scriptparsetree");
    }
    return true;
}

void GscInfoOption::PrintHelp(std::ostream& out) {
    out << "-h --help          : Print help\n"
        << "-g --gsc           : Produce GSC\n"
        << "-a --asm           : Produce ASM\n"
        << "-t --type [t]      : Set type, default PC, values: 'ps', 'xbox', 'pc'\n"
        << "-o --output [d]    : ASM/GSC output dir, default same.gscasm\n"
        << "-v --vm            : Set vm, useless for Treyarch VM, values: mw23\n"
        << "-H --header        : Write file header\n"
        << "-m --hashmap [f]   : Write hashmap in a file f\n"
        << "--dumpstrings [f]  : Dump strings in f\n"
        << "-l --rloc          : Write relative location of the function code\n"
        << "-f --nofunc        : No function write\n"
        << "-C --copyright [t] : Set a comment text to put in front of every file\n"
#ifndef CI_BUILD
        // it's not that I don't want them to be known, it's just to avoid having too many of them in the help
        // it's mostly dev tools
        << "-G --gvars         : Write gvars\n"
        << "-U --noincludes    : No includes\n"
        << "-X --exptests      : Enable UNK tests\n"
        << "-V --vars          : Show all func vars\n"
        << "-F --nofuncheader  : No function header\n"
        << "-p --postfunchead  : Write post function header in ASM mode\n"
        << "-I --imports       : Write imports\n"
        << "-S --strings       : Write strings\n"
        << "-r --rosetta [f]   : Create Rosetta file\n"
        << "--test-header      : Write test header\n"
        << "--internalblocks   : Show internal blocks \n"
        << "--jumpdelta        : Show jump delta\n"
        << "--prestruct        : Show prestruct\n"
        << "--refcount         : Show ref count\n"
        << "-i --ignore[t + ]  : ignore step (d: dev, s: switch, e: foreach, w: while, i: if, f: for)\n"
#endif
        ;
}

static LPCCH gDumpStrings = NULL;
static std::unordered_set<std::string> gDumpStringsStore{};
static LPCCH gRosettaOutput = NULL;
static UINT64 gRosettaCurrent = 0;
static std::map<UINT64, RosettaFileData> gRosettaBlocks{};

void tool::gsc::RosettaStartFile(GSCOBJReader& reader) {
    if (!gRosettaOutput) {
        return;
    }

    
    auto& block = gRosettaBlocks[gRosettaCurrent = reader.GetName()];
    // clone the header for the finder
    memcpy(&block.header, reader.Ptr(), reader.GetHeaderSize());
}

void tool::gsc::RosettaAddOpCode(UINT32 loc, UINT16 opcode) {
    if (!gRosettaOutput) {
        return;
    }

    auto& block = gRosettaBlocks[gRosettaCurrent].blocks;

    block.push_back(tool::gsc::RosettaOpCodeBlock{ .location = loc, .opcode = opcode });
}

GSCOBJReader::GSCOBJReader(BYTE* file, const GscInfoOption& opt) : file(file), opt(opt) {}

// by default no remapping
BYTE GSCOBJReader::RemapFlagsImport(BYTE flags) {
    return flags;
}
BYTE GSCOBJReader::RemapFlagsExport(BYTE flags) {
    return flags;
}

struct GSC_USEANIMTREE_ITEM {
    UINT32 num_address;
    UINT32 address;
};
struct GSC_ANIMTREE_ITEM {
    UINT32 num_address;
    UINT32 address_str1;
    UINT32 address_str2;
};

void GSCOBJReader::PatchCode(T8GSCOBJContext& ctx) {
    if (ctx.m_vmInfo->flags & opcode::VmFlags::VMF_HASH64) {
        if (GetAnimTreeSingleOffset()) {
            // HAS TO BE DONE FIRST BECAUSE THEY ARE STORED USING 1 BYTE
            uintptr_t unk2c_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeSingleOffset();
            auto anims_count = (int)GetAnimTreeSingleCount();
            for (size_t i = 0; i < anims_count; i++) {
                const auto* unk2c = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(unk2c_location);

                auto* s = Ptr<char>(unk2c->address);

                UINT32 ref = ctx.AddStringValue(s);
                const auto* vars = reinterpret_cast<const UINT32*>(&unk2c[1]);

                if (ref > 256) {
                    std::cerr << "too many animtrees single usage\n";
                }
                else {
                    for (size_t j = 0; j < unk2c->num_address; j++) {
                        Ref(vars[j]) = (BYTE)ref;
                    }
                }
                unk2c_location += sizeof(*unk2c) + sizeof(*vars) * unk2c->num_address;
            }
        }

        uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
        auto string_count = (int)GetStringsCount();
        for (size_t i = 0; i < string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);
            LPCH cstr = DecryptString(Ptr<CHAR>(str->string));
            if (gDumpStrings) {
                gDumpStringsStore.insert(cstr);
            }
            hashutils::Add(cstr, false, false);
            hashutils::Add(cstr, false, true);
            UINT32 ref = ctx.AddStringValue(cstr);

            const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
            for (size_t j = 0; j < str->num_address; j++) {
                Ref<UINT32>(strings[j]) = ref;
            }
            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }

        auto imports_count = (int)GetImportsCount();
        uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + GetImportsOffset();
        for (size_t i = 0; i < imports_count; i++) {

            const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);

            const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
            for (size_t j = 0; j < imp->num_address; j++) {
                UINT16* loc = Ptr<UINT16>(imports[j]);
                auto idx = ctx.m_linkedImports.size();
                ctx.m_linkedImports.push_back(*imp);
                *loc = (UINT16)idx;
            }
            import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
        }

        if (GetAnimTreeDoubleOffset()) {
            uintptr_t animt_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeDoubleOffset();
            auto anims_count = (int)GetAnimTreeDoubleCount();
            for (size_t i = 0; i < anims_count; i++) {
                const auto* animt = reinterpret_cast<GSC_ANIMTREE_ITEM*>(animt_location);

                auto* s1 = Ptr<char>(animt->address_str1);
                auto* s2 = Ptr<char>(animt->address_str2);

                hashutils::Add(s1, true, true);
                hashutils::Add(s2, true, true);
                UINT32 ref1 = ctx.AddStringValue(s1);
                UINT32 ref2 = ctx.AddStringValue(s2);

                const auto* vars = reinterpret_cast<const UINT32*>(&animt[1]);
                for (size_t j = 0; j < animt->num_address; j++) {
                    auto* loc = Ptr<UINT32>(vars[j]);
                    // use strings to link them
                    loc[0] = ref1;
                    loc[1] = ref2;
                }
                animt_location += sizeof(*animt) + sizeof(*vars) * animt->num_address;
            }
        }

        return; // mwiii
    }
    // patching imports unlink the script refs to write namespace::import_name instead of the address
    auto imports_count = (int)GetImportsCount();
    uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + GetImportsOffset();
    for (size_t i = 0; i < imports_count; i++) {

        const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);

        const auto* imports = reinterpret_cast<const UINT32*>(&imp[1]);
        for (size_t j = 0; j < imp->num_address; j++) {
            UINT32* loc;
            auto remapedFlags = RemapFlagsImport(imp->flags);

            switch (remapedFlags & CALLTYPE_MASK) {
            case FUNC_METHOD:
                loc = PtrAlign<UINT64, UINT32>(imports[j] + 2ull);
                break;
            case FUNCTION:
            case FUNCTION_THREAD:
            case FUNCTION_CHILDTHREAD:
            case METHOD:
            case METHOD_THREAD:
            case METHOD_CHILDTHREAD:
                // here the game fix function calls with a bad number of params,
                // but for the decomp/dasm we don't care because we only mind about
                // what we'll find on the stack.
                Ref<BYTE>(imports[j] + 2ull) = imp->param_count;
                loc = PtrAlign<UINT64, UINT32>(imports[j] + 2ull + 1);
                break;
            default:
                loc = nullptr;
                break;
            }
            if (loc) {
                loc[0] = imp->name;

                if (remapedFlags & T8GSCImportFlags::GET_CALL) {
                    // no need for namespace if we are getting the call dynamically (api or inside-code script)
                    loc[1] = 0xc1243180; // ""
                }
                else {
                    loc[1] = imp->import_namespace;
                }

            }
        }
        import_location += sizeof(*imp) + sizeof(*imports) * imp->num_address;
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(file) + GetGVarsOffset();
    auto globalvar_count = (int)GetGVarsCount();
    for (size_t i = 0; i < globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
        UINT16 ref = ctx.AddGlobalVarName(globalvar->name);

        const auto* vars = reinterpret_cast<const UINT32*>(&globalvar[1]);
        for (size_t j = 0; j < globalvar->num_address; j++) {
            // no align, no opcode to pass, directly the fucking location, cool.
            Ref<UINT16>(vars[j]) = ref;
        }
        gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
    }

    uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
    auto string_count = (int)GetStringsCount();
    for (size_t i = 0; i < string_count; i++) {

        const auto* str = reinterpret_cast<T8GSCString*>(str_location);
        LPCH cstr = DecryptString(Ptr<CHAR>(str->string));
        if (gDumpStrings) {
            gDumpStringsStore.insert(cstr);
        }
        UINT32 ref = ctx.AddStringValue(cstr);

        const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
        for (size_t j = 0; j < str->num_address; j++) {
            // no align too....
            Ref<UINT32>(strings[j]) = ref;
        }
        str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
    }
}

void tool::gsc::GSCOBJReader::DumpExperimental(std::ostream& asmout, const GscInfoOption& opt) {
}

namespace {

    class T8GSCOBJReader : public GSCOBJReader {
        using GSCOBJReader::GSCOBJReader;

        void DumpHeader(std::ostream& asmout) override {
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
            UINT64* requires_implements_table = reinterpret_cast<UINT64*>(&data->magic[data->requires_implements_offset]);

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

        UINT64 GetName() override {
            return Ptr<T8GSCOBJ>()->name;
        }
        UINT16 GetExportsCount() override {
            return Ptr<T8GSCOBJ>()->exports_count;
        }
        UINT32 GetExportsOffset() override {
            return Ptr<T8GSCOBJ>()->export_table_offset;
        }
        UINT16 GetIncludesCount() override {
            return Ptr<T8GSCOBJ>()->include_count;
        }
        UINT32 GetIncludesOffset() override {
            return Ptr<T8GSCOBJ>()->include_offset;
        }
        UINT16 GetImportsCount() override {
            return Ptr<T8GSCOBJ>()->imports_count;
        }
        UINT32 GetImportsOffset() override {
            return Ptr<T8GSCOBJ>()->imports_offset;
        }
        UINT16 GetGVarsCount() override {
            return Ptr<T8GSCOBJ>()->globalvar_count;
        }
        UINT32 GetGVarsOffset() override {
            return Ptr<T8GSCOBJ>()->globalvar_offset;
        }
        UINT16 GetStringsCount() override {
            return Ptr<T8GSCOBJ>()->string_count;
        }
        UINT32 GetStringsOffset() override {
            return Ptr<T8GSCOBJ>()->string_offset;
        }
        UINT32 GetFileSize() override {
            return Ptr<T8GSCOBJ>()->script_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T8GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return decrypt::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T8GSCOBJ) && *reinterpret_cast<UINT64*>(file) == 0x36000a0d43534780;
        }
        UINT16 GetAnimTreeSingleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeSingleOffset() override {
            return 0;
        };
        UINT16 GetAnimTreeDoubleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeDoubleOffset() override {
            return 0;
        };
    };
    
    class T937GSCOBJReader : public GSCOBJReader {
        using GSCOBJReader::GSCOBJReader;

        void DumpHeader(std::ostream& asmout) override {
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

        UINT64 GetName() override {
            return Ptr<T937GSCOBJ>()->name;
        }
        UINT16 GetExportsCount() override {
            return Ptr<T937GSCOBJ>()->export_count;
        }
        UINT32 GetExportsOffset() override {
            return Ptr<T937GSCOBJ>()->exports_tables;
        }
        UINT16 GetIncludesCount() override {
            return Ptr<T937GSCOBJ>()->includes_count;
        }
        UINT32 GetIncludesOffset() override {
            return Ptr<T937GSCOBJ>()->includes_table;
        }
        UINT16 GetImportsCount() override {
            return Ptr<T937GSCOBJ>()->imports_count;
        }
        UINT32 GetImportsOffset() override {
            return Ptr<T937GSCOBJ>()->imports_offset;
        }
        UINT16 GetGVarsCount() override {
            return Ptr<T937GSCOBJ>()->globalvar_count;
        }
        UINT32 GetGVarsOffset() override {
            return Ptr<T937GSCOBJ>()->globalvar_offset;
        }
        UINT16 GetStringsCount() override {
            return Ptr<T937GSCOBJ>()->string_count;
        }
        UINT32 GetStringsOffset() override {
            return Ptr<T937GSCOBJ>()->string_offset;
        }
        UINT32 GetFileSize() override {
            return Ptr<T937GSCOBJ>()->file_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T937GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return cw::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T937GSCOBJ) && *reinterpret_cast<UINT64*>(file) == 0x37000a0d43534780;
        }

        BYTE RemapFlagsImport(BYTE flags) override {
            return flags; // same as bo4?
        }

        BYTE RemapFlagsExport(BYTE flags) override {
            return flags;
        }
        UINT16 GetAnimTreeSingleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeSingleOffset() override {
            return 0;
        };
        UINT16 GetAnimTreeDoubleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeDoubleOffset() override {
            return 0;
        };
    };

    class T9GSCOBJReader : public GSCOBJReader {
        using GSCOBJReader::GSCOBJReader;

        void DumpHeader(std::ostream& asmout) override {
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

        UINT64 GetName() override {
            return Ptr<T9GSCOBJ>()->name;
        }
        UINT16 GetExportsCount() override {
            return Ptr<T9GSCOBJ>()->exports_count;
        }
        UINT32 GetExportsOffset() override {
            return Ptr<T9GSCOBJ>()->exports_tables;
        }
        UINT16 GetIncludesCount() override {
            return Ptr<T9GSCOBJ>()->includes_count;
        }
        UINT32 GetIncludesOffset() override {
            return Ptr<T9GSCOBJ>()->includes_table;
        }
        UINT16 GetImportsCount() override {
            return Ptr<T9GSCOBJ>()->imports_count;
        }
        UINT32 GetImportsOffset() override {
            return Ptr<T9GSCOBJ>()->import_tables;
        }
        UINT16 GetGVarsCount() override {
            return Ptr<T9GSCOBJ>()->globalvar_count;
        }
        UINT32 GetGVarsOffset() override {
            return Ptr<T9GSCOBJ>()->globalvar_offset;
        }
        UINT16 GetStringsCount() override {
            return Ptr<T9GSCOBJ>()->string_count;
        }
        UINT32 GetStringsOffset() override {
            return Ptr<T9GSCOBJ>()->string_offset;
        }
        UINT32 GetFileSize() override {
            return Ptr<T9GSCOBJ>()->file_size;
        }
        size_t GetHeaderSize() override {
            return sizeof(T9GSCOBJ);
        }
        char* DecryptString(char* str) override {
            return cw::DecryptString(str);
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(T9GSCOBJ) && *reinterpret_cast<UINT64*>(file) == 0x38000a0d43534780;
        }
        BYTE RemapFlagsImport(BYTE flags) override {
            BYTE nflags = 0;

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

            nflags |= flags & ~T9_IF_CALLTYPE_MASK;

            return nflags;
        }

        BYTE RemapFlagsExport(BYTE flags) override {
            if (flags == T9_EF_CLASS_VTABLE) {
                return CLASS_VTABLE;
            }
            BYTE nflags = 0;

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
        UINT16 GetAnimTreeSingleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeSingleOffset() override {
            return 0;
        };
        UINT16 GetAnimTreeDoubleCount() override {
            return 0;
        };
        UINT32 GetAnimTreeDoubleOffset() override {
            return 0;
        };
    };

    class MW23GSCOBJReader : public GSCOBJReader {
        using GSCOBJReader::GSCOBJReader;

        void DumpHeader(std::ostream& asmout) override {
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

                    const auto* vars = reinterpret_cast<const UINT32*>(&unk2c[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < unk2c->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<UINT16>(vars[j]) = ref;
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

                    const auto* vars = reinterpret_cast<const UINT32*>(&animt[1]);
                    asmout << "location(s): ";
                    for (size_t j = 0; j < animt->num_address; j++) {
                        // no align, no opcode to pass, directly the fucking location, cool.
                        //Ref<UINT16>(vars[j]) = ref;
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

        UINT64 GetName() override {
            return Ptr<GscObj23>()->name;
        }
        UINT16 GetExportsCount() override {
            return Ptr<GscObj23>()->export_count;
        }
        UINT32 GetExportsOffset() override {
            return Ptr<GscObj23>()->export_offset;
        }
        UINT16 GetIncludesCount() override {
            return Ptr<GscObj23>()->includes_count;
        }
        UINT32 GetIncludesOffset() override {
            return Ptr<GscObj23>()->include_table;
        }
        UINT16 GetImportsCount() override {
            return Ptr<GscObj23>()->imports_count;
        }
        UINT32 GetImportsOffset() override {
            return Ptr<GscObj23>()->import_table;
        }
        UINT16 GetGVarsCount() override {
            return 0; //return Ptr<GscObj23>()->globalvar_count;
        }
        UINT32 GetGVarsOffset() override {
            return 0; //return Ptr<GscObj23>()->globalvar_offset;
        }
        UINT16 GetStringsCount() override {
            return Ptr<GscObj23>()->string_count;
        }
        UINT32 GetStringsOffset() override {
            return Ptr<GscObj23>()->string_table;
        }
        UINT32 GetFileSize() override {
            return Ptr<GscObj23>()->size1;
        }
        size_t GetHeaderSize() override {
            return sizeof(GscObj23);
        }
        char* DecryptString(char* str) override {
            return str; // iw
        }
        bool IsValidHeader(size_t size) override {
            return size >= sizeof(GscObj23) && *reinterpret_cast<UINT64*>(file) == 0xa0d4353478a;
        }
        BYTE RemapFlagsImport(BYTE flags) override {
            BYTE nflags = 0;

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

        BYTE RemapFlagsExport(BYTE flags) override {
            BYTE nflags{};
            if (flags & 1) {
                nflags |= T8GSCExportFlags::AUTOEXEC;
            }
            if (flags & 2) {
                nflags |= T8GSCExportFlags::LINKED;
            }
            if (flags & 4) {
                nflags |= T8GSCExportFlags::PRIVATE;
            }

            return nflags;
        }
        UINT16 GetAnimTreeSingleCount() override {
            return Ptr<GscObj23>()->animtree_use_count;
        };
        UINT32 GetAnimTreeSingleOffset() override {
            return Ptr<GscObj23>()->animtree_use_offset;
        };
        UINT16 GetAnimTreeDoubleCount() override {
            return Ptr<GscObj23>()->animtree_count;
        };
        UINT32 GetAnimTreeDoubleOffset() override {
            return Ptr<GscObj23>()->animtree_offset;
        };
    };

    std::unordered_map<BYTE, std::function<std::shared_ptr<GSCOBJReader> (BYTE*, const GscInfoOption&)>> gscReaders = {
        { tool::gsc::opcode::VM_T8,[](BYTE* file, const GscInfoOption& opt) { return std::make_shared<T8GSCOBJReader>(file, opt); }},
        { tool::gsc::opcode::VM_T937,[](BYTE* file, const GscInfoOption& opt) { return std::make_shared<T937GSCOBJReader>(file, opt); }},
        { tool::gsc::opcode::VM_T9,[](BYTE* file, const GscInfoOption& opt) { return std::make_shared<T9GSCOBJReader>(file, opt); }},
        { tool::gsc::opcode::VM_MW23,[](BYTE* file, const GscInfoOption& opt) { return std::make_shared<MW23GSCOBJReader>(file, opt); }},
    };
}


struct H32GSCExportReader : GSCExportReader {
    T8GSCExport* exp{};

    void SetHandle(void* handle) override { exp = (T8GSCExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->callback_event; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};
struct H64GSCExportReader : GSCExportReader {
    IW23GSCExport* exp{};

    void SetHandle(void* handle) override { exp = (IW23GSCExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->file_name_space; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};

int GscInfoHandleData(BYTE* data, size_t size, const char* path, const GscInfoOption& opt) {


    T8GSCOBJContext ctx{};
    auto& gsicInfo = ctx.m_gsicInfo;

    gsicInfo.isGsic = size > 4 && !memcmp(data, "GSIC", 4);
    if (gsicInfo.isGsic) {
        std::cout << "Reading GSIC Compiled Script data\n";

        size_t gsicSize = 4; // preamble

        auto numFields = *reinterpret_cast<INT32*>(&data[gsicSize]);
        gsicSize += 4;

        bool gsicError = false;
        for (size_t i = 0; i < numFields; i++) {
            auto fieldType = *reinterpret_cast<INT32*>(&data[gsicSize]);
            gsicSize += 4;
            switch (fieldType) {
            case 0: // Detour
            {
                auto detourCount = *reinterpret_cast<INT32*>(&data[gsicSize]);
                gsicSize += 4;
                for (size_t j = 0; j < detourCount; j++) {
                    GsicDetour* detour = reinterpret_cast<GsicDetour*>(&data[gsicSize]);
                    gsicSize += (size_t)(28 + 256 - 1 - (5 * 4) + 1 - 8);

                    // register detour
                    gsicInfo.detours[detour->fixupOffset] = detour;
                }
            }
            break;
            default:
                std::cerr << "Bad GSIC field type: " << std::dec << fieldType << "\n";
                gsicError = true;
                break;
            }
            if (gsicError) {
                break;
            }
        }

        if (gsicError) {
            return tool::BASIC_ERROR;
        }

        // pass the GSIC data
        gsicInfo.headerSize = gsicSize;
        data += gsicSize;
    }
    BYTE vm;
    bool iw;

    auto magicVal = *reinterpret_cast<UINT64*>(data) & ~0xFF00000000000000;
    if (magicVal == 0xa0d4353478a) {
        // IW GSC file, use user input
        if (opt.m_vm == opcode::VM_UNKNOWN) {
            std::cerr << "VM type needed with IW GSC file, please use --vm [vm] to set it\n";
            return tool::BASIC_ERROR;
        }
        vm = opt.m_vm;
        iw = true;
    } 
    else if (magicVal == 0xa0d43534780) {
        // Treyarch GSC file, use revision
        vm = data[7];
        iw = false;
    }
    else {
        std::cerr << "Bad magic 0x" << std::hex << *reinterpret_cast<UINT64*>(data) << "\n";
        return tool::BASIC_ERROR;
    }
    hashutils::ReadDefaultFile(true, iw);


    opcode::VmInfo* vmInfo;
    if (!opcode::IsValidVm(vm, vmInfo)) {
        std::cerr << "Bad vm 0x" << std::hex << (int)vm << " for file " << path << "\n";
        return -1;
    }
    ctx.m_vmInfo = vmInfo;

    auto readerBuilder = gscReaders.find(vm);

    if (readerBuilder == gscReaders.end()) {
        std::cerr << "No reader available for vm 0x" << std::hex << (int)vm << " for file " << path << "\n";
        return -1;
    }

    std::shared_ptr<GSCOBJReader> scriptfile = readerBuilder->second(data, opt);

    // we keep it because it should also check the size
    if (!scriptfile->IsValidHeader(size)) {
        std::cerr << "Bad header 0x" << std::hex << scriptfile->Ref<UINT64>() << " for file " << path << "\n";
        return -1;
    }

    tool::gsc::RosettaStartFile(*scriptfile);

    char asmfnamebuff[1000];

    if (opt.m_outputDir) {
        LPCCH name = hashutils::ExtractPtr(scriptfile->GetName());
        if (!name) {
            sprintf_s(asmfnamebuff, "%s/hashed/script/script_%llx.gsc", opt.m_outputDir, scriptfile->GetName());
        }
        else {
            sprintf_s(asmfnamebuff, "%s/%s", opt.m_outputDir, name);
        }
    }
    else {
        sprintf_s(asmfnamebuff, "%sasm", path);
    }

    std::filesystem::path file(asmfnamebuff);

    std::filesystem::create_directories(file.parent_path());

    std::ofstream asmout{ file };

    if (!asmout) {
        std::cerr << "Can't open output file " << asmfnamebuff << "\n";
        return -1;
    }
    std::cout << "Decompiling into '" << asmfnamebuff << (gsicInfo.isGsic ? " (GSIC)" : "") << "'...\n";
    if (opt.m_copyright) {
        asmout << "// " << opt.m_copyright << "\n";
    }

    if (opt.m_header) {
        asmout
            << "// " << hashutils::ExtractTmpScript(scriptfile->GetName()) << " (" << path << ")" << " (size: " << size << " Bytes / " << std::hex << "0x" << size << ")\n";

        if (gsicInfo.isGsic) {
            asmout
                << "// GSIC Compiled script" << ", header: 0x" << std::hex << gsicInfo.headerSize << "\n"
                << "// detours: " << std::dec << gsicInfo.detours.size() << "\n";
            for (const auto& [key, detour] : gsicInfo.detours) {
                asmout << "// - ";

                if (detour->replaceNamespace) {
                    asmout << hashutils::ExtractTmp("namespace", detour->replaceNamespace) << std::flush;
                }
                auto replaceScript = *reinterpret_cast<UINT64*>(&detour->replaceScriptTop);
                if (replaceScript) {
                    asmout << "<" << hashutils::ExtractTmpScript(replaceScript) << ">" << std::flush;
                }

                if (detour->replaceNamespace) {
                    asmout << "::";
                }

                asmout
                    << hashutils::ExtractTmp("function", detour->replaceFunction) << std::flush
                    << " offset: 0x" << std::hex << detour->fixupOffset << ", size: 0x" << detour->fixupSize << "\n";
            }
        }

        asmout
            << "// magic .... 0x" << scriptfile->Ref<UINT64>()
            << " vm: ";
        
        if (vmInfo->flags & opcode::VmFlags::VMF_NO_VERSION) {
            asmout << vmInfo->name;
        }
        else {
            asmout << (UINT32)vmInfo->vm << " (" << vmInfo->name << ")";
        }
        asmout << "\n";

        scriptfile->DumpHeader(asmout);
    }

    // write the strings before the patch to avoid reading pre-decrypted strings
    if (opt.m_strings && scriptfile->GetStringsOffset()) {
        uintptr_t str_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetStringsOffset()));

        for (size_t i = 0; i < scriptfile->GetStringsCount(); i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);

            asmout << std::hex << "String addr:" << str->string << ", count:" << (int)str->num_address << ", type:" << (int)str->type << std::endl;

            LPCH encryptedString = scriptfile->Ptr<CHAR>(str->string);

            size_t len{};
            BYTE type{};
            if (scriptfile->GetVM() == opcode::VM_T8) {
                len = (size_t)reinterpret_cast<BYTE*>(encryptedString)[1] - 1;
                type = *reinterpret_cast<BYTE*>(encryptedString);

                if (str->string + len + 1 > scriptfile->GetFileSize()) {
                    asmout << "bad string location\n";
                    break;
                }

                asmout << "encryption: ";
                asmout << "0x" << std::hex << (int)type;
                if ((type & 0xC0) == 0x80) {
                    asmout << "(none)";
                }
                asmout << " len: " << std::dec << len << " -> " << std::flush;

            }
            else {
                auto* ess = reinterpret_cast<BYTE*>(encryptedString);
                type = ess[0];
                len = (size_t)ess[2] - 1;

                if (str->string + len + 3 > scriptfile->GetFileSize()) {
                    asmout << "bad string location\n";
                    break;
                }

                asmout << "encryption: ";
                asmout << "0x" << std::hex << (int)type;
                if ((type & 0xC0) == 0x80) {
                    asmout << "(none)";
                }
                asmout << " len: " << std::dec << len << ", unk1: 0x" << std::hex << (int)ess[1] << " -> " << std::flush;
            }
            LPCH cstr = scriptfile->DecryptString(encryptedString);

            asmout << '"' << cstr << "\"" << std::flush;

            if (scriptfile->GetVM() == opcode::VM_T8) {
                size_t lenAfterDecrypt = strnlen_s(cstr, len + 2);

                if (lenAfterDecrypt != len) {
                    asmout << " ERROR LEN (" << std::dec << lenAfterDecrypt << " != " << len << " for type 0x" << std::hex << (int)type << ")";
                    assert(false);
                }
            }

            asmout << "\n";

            asmout << "location(s): ";

            const auto* strings = reinterpret_cast<const UINT32*>(&str[1]);
            asmout << std::hex << strings[0];
            for (size_t j = 1; j < str->num_address; j++) {
                asmout << std::hex << "," << strings[j];
            }
            asmout << "\n";
            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }
        if (scriptfile->GetStringsCount()) {
            asmout << "\n";
        }
    }

    if (opt.m_patch) {
        // unlink the script and write custom gvar/string ids
        scriptfile->PatchCode(ctx);
    }

    if (opt.m_includes && scriptfile->GetIncludesOffset()) {
        auto* includes = scriptfile->Ptr<UINT64>(scriptfile->GetIncludesOffset());

        for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
            asmout << "#using " << hashutils::ExtractTmpScript(includes[i]) << ";\n";
        }
        if (scriptfile->GetIncludesCount()) {
            asmout << "\n";
        }
    }

    scriptfile->DumpExperimental(asmout, opt);

    if (opt.m_gvars && scriptfile->GetGVarsOffset()) {
        uintptr_t gvars_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetGVarsOffset()));

        for (size_t i = 0; i < scriptfile->GetGVarsCount(); i++) {
            const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
            asmout << std::hex << "Global var " << hashutils::ExtractTmp("var", globalvar->name) << " " << globalvar->num_address << "\n";

            asmout << "location(s): ";

            const auto* vars = reinterpret_cast<const UINT32*>(&globalvar[1]);
            asmout << std::hex << vars[0];
            for (size_t j = 1; j < globalvar->num_address; j++) {
                asmout << std::hex << "," << vars[j];
            }
            asmout << "\n";
            gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
        }
        if (scriptfile->GetGVarsCount()) {
            asmout << "\n";
        }
    }

    if (opt.m_imports) {

        uintptr_t import_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetImportsOffset()));

        for (size_t i = 0; i < scriptfile->GetImportsCount(); i++) {
            UINT64 name_space;
            UINT64 name;
            size_t impSize;
            BYTE flags;
            BYTE param_count;
            UINT16 numAddress;

            if (ctx.m_vmInfo->flags & opcode::VmFlags::VMF_HASH64) {
                const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);
                name_space = imp->name_space;
                name = imp->name;
                flags = imp->flags;
                numAddress = imp->num_address;
                param_count = imp->param_count;
                impSize = sizeof(*imp);
            }
            else {
                const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
                name_space = imp->import_namespace;
                name = imp->name;
                flags = imp->flags;
                param_count = imp->param_count;
                numAddress = imp->num_address;
                impSize = sizeof(*imp);
            }

            asmout << std::hex << "import ";

            auto remapedFlags = scriptfile->RemapFlagsImport(flags);

            switch (remapedFlags & T8GSCImportFlags::CALLTYPE_MASK) {
            case FUNC_METHOD: asmout << "funcmethod "; break;
            case FUNCTION: asmout << "function "; break;
            case FUNCTION_THREAD: asmout << "function thread "; break;
            case FUNCTION_CHILDTHREAD: asmout << "function childthread "; break;
            case METHOD: asmout << "method "; break;
            case METHOD_THREAD: asmout << "method thread "; break;
            case METHOD_CHILDTHREAD: asmout << "method childthread "; break;
            default:
                asmout << "<errorflag:" << std::hex << (remapedFlags & 0xF) << "> ";
                break;
            }

            if (remapedFlags & T8GSCImportFlags::DEV_CALL) {
                asmout << "devcall ";
            }

            // they both seem unused
            if (remapedFlags & T8GSCImportFlags::UKN40) {
                asmout << "ukn40 ";
            }
            if (remapedFlags & T8GSCImportFlags::UKN80) {
                asmout << "ukn80 ";
            }

            if ((remapedFlags & T8GSCImportFlags::GET_CALL) == 0) {
                // no need for namespace if we are getting the call dynamically (api or inside-code script)
                asmout << hashutils::ExtractTmp("namespace", name_space) << std::flush << "::";
            }

            asmout << std::hex << hashutils::ExtractTmp("function", name) << "\n";

            asmout << std::hex << "address: " << numAddress <<
                ", params: " << (int)param_count <<
                ", iflags: 0x" << std::hex << (UINT16)(flags) << "\n";

            asmout << "location(s): ";

            const auto* imports = reinterpret_cast<const UINT32*>(import_location + impSize);
            asmout << std::hex << imports[0];
            for (size_t j = 1; j < numAddress; j++) {
                asmout << std::hex << "," << imports[j];
            }
            asmout << "\n";

            asmout << "--------------\n";

            import_location += impSize + sizeof(*imports) * numAddress;
        }
        if (scriptfile->GetImportsCount()) {
            asmout << "\n";
        }
    }

    if (opt.m_func) {
        // current namespace
        UINT64 currentNSP = 0;

        struct Located {
            UINT64 name_space;
            UINT64 name;
        };
        struct LocatedHash {
            size_t operator()(const Located& k) const {
                return k.name_space ^ RotateLeft64(k.name, 32);
            }
        };
        struct LocatedEquals {
            bool operator()(const Located& a, const Located& b) const {
                return a.name == b.name && a.name_space == b.name_space;
            }
        };

        std::unordered_map<Located, opcode::ASMContext, LocatedHash, LocatedEquals> contextes{};

        std::unique_ptr<GSCExportReader> exp;
        if (ctx.m_vmInfo->flags & opcode::VmFlags::VMF_HASH64) {
            exp = std::make_unique<H64GSCExportReader>();
        }
        else {
            exp = std::make_unique<H32GSCExportReader>();
        }

        for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
            void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
            exp->SetHandle(handle);

            std::ofstream nullstream;
            nullstream.setstate(std::ios_base::badbit);

            // if we aren't dumping the ASM, we compute all the nodes first
            std::ostream& output = opt.m_dasm ? asmout : nullstream;

            if (exp->GetNamespace() != currentNSP) {
                currentNSP = exp->GetNamespace();

                if (opt.m_dasm) {
                    output << "#namespace " << hashutils::ExtractTmp("namespace", currentNSP) << ";\n" << std::endl;
                }
            }

            Located rname = { exp->GetNamespace(), exp->GetName() };

            auto r = contextes.try_emplace(rname, scriptfile->Ptr(exp->GetAddress()), *scriptfile, ctx, opt, currentNSP, *exp, handle, vm, opt.m_platform);

            if (!r.second) {
                asmout << "Duplicate node "
                    << hashutils::ExtractTmp("namespace", exp->GetNamespace()) << std::flush << "::"
                    << hashutils::ExtractTmp("function", exp->GetName()) << std::endl;
                continue;
            }

            auto& asmctx = r.first->second;

            DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx);

            output << " gscasm {\n";

            DumpAsm(*exp, output, *scriptfile, ctx, asmctx);

            output << "}\n";

            if (asmctx.m_disableDecompiler) {
                output << "// decompiler disabled for method\n";
            }


            if ((!opt.m_dasm || opt.m_dcomp || opt.m_func_header_post) && !asmctx.m_disableDecompiler) {
                asmctx.ComputeDefaultParamValue();
                if (opt.m_dasm || opt.m_func_header_post) {
                    DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx);
                }
                output << std::flush;
                opcode::DecompContext dctx{ 0, 0, asmctx };
                if (opt.m_dcomp) {
                    if (scriptfile->RemapFlagsExport(exp->GetFlags()) == T8GSCExportFlags::CLASS_VTABLE) {
                        asmctx.m_bcl = scriptfile->Ptr(exp->GetAddress());
                        output << " {\n";
                        DumpVTable(*exp, output, *scriptfile, ctx, asmctx, dctx);
                        output << "}\n";
                    }
                    else {
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_DEV)) {
                            asmctx.ComputeDevBlocks(asmctx);
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_SWITCH)) {
                            asmctx.ComputeSwitchBlocks(asmctx);
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOREACH)) {
                            asmctx.ComputeForEachBlocks(asmctx);
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_WHILE)) {
                            asmctx.ComputeWhileBlocks(asmctx);
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_IF)) {
                            asmctx.ComputeIfBlocks(asmctx);
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOR)) {
                            asmctx.ComputeForBlocks(asmctx);
                        }
                        if (opt.m_dasm) {
                            output << " ";
                            asmctx.Dump(output, dctx);
                        }
                    }
                }
                else {
                    output << ";\n";
                }
            }
            output << "\n";
        }

        if (!opt.m_dasm && opt.m_dcomp) {
            // current namespace
            currentNSP = 0;

            for (const auto& [name, cls] : ctx.m_classes) {
                if (cls.name_space != currentNSP) {
                    currentNSP = cls.name_space;

                    asmout << "#namespace " << hashutils::ExtractTmp("namespace", currentNSP) << ";\n" << std::endl;
                }

                asmout << "// Namespace " << hashutils::ExtractTmp("namespace", cls.name_space) << std::endl;
                asmout << "// Method(s) " << std::dec << cls.m_methods.size() << " Total "  << cls.m_vtable.size() << "\n";
                asmout << "class " << hashutils::ExtractTmp("class", name) << std::flush;

                if (cls.m_superClass.size()) {
                    // write superclasses
                    asmout << " : ";
                    auto it = cls.m_superClass.begin();
                    asmout << hashutils::ExtractTmp("class", *it) << std::flush;
                    it++;

                    while (it != cls.m_superClass.end()) {
                        asmout << ", " << hashutils::ExtractTmp("class", *it) << std::flush;
                        it++;
                    }
                }
                asmout << " {\n\n";

                

                auto handleMethod = [&contextes, &asmout, &scriptfile, name, &ctx](UINT64 method) -> void {
                    auto lname = Located{ name, method };

                    auto masmctxit = contextes.find(lname);

                    if (masmctxit == contextes.end()) {
                        return;
                    }

                    auto& e = masmctxit->second;

                    if (e.m_disableDecompiler) {
                        return;
                    }

                    // set the export handle
                    e.m_exp.SetHandle(e.m_readerHandle);

                    DumpFunctionHeader(e.m_exp, asmout, *scriptfile, ctx, e, 1);
                    asmout << " ";
                    opcode::DecompContext dctx{ 1, 0, e };
                    e.Dump(asmout, dctx);
                    asmout << "\n";

                    contextes.erase(masmctxit);
                };

                // handle first the constructor/destructor
                handleMethod(g_constructorName);
                handleMethod(g_destructorName);

                for (const auto& method : cls.m_methods) {
                    handleMethod(method);
                }

                asmout << "}\n\n";
            }

            for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
                exp->SetHandle(handle);

                if (scriptfile->RemapFlagsExport(exp->GetFlags()) == T8GSCExportFlags::CLASS_VTABLE) {
                    continue;
                }

                Located lname = Located{ exp->GetNamespace(), exp->GetName() };

                auto f = contextes.find(lname);

                if (f == contextes.end()) {
                    continue; // already parsed
                }

                if (exp->GetNamespace() != currentNSP) {
                    currentNSP = exp->GetNamespace();

                    asmout << "#namespace " << hashutils::ExtractTmp("namespace", currentNSP) << ";\n" << std::endl;
                }

                auto& asmctx = f->second;

                if (asmctx.m_disableDecompiler) {
                    continue;
                }

                DumpFunctionHeader(*exp, asmout, *scriptfile, ctx, asmctx);
                asmout << " ";
                opcode::DecompContext dctx{ 0, 0, asmctx };
                asmctx.Dump(asmout, dctx);
                asmout << "\n";
            }
        }
    }

    asmout.close();

    return 0;
}

int GscInfoFile(const std::filesystem::path& path, const GscInfoOption& opt) {
    if (std::filesystem::is_directory(path)) {
        // directory
        auto ret = 0;
        for (const auto& sub : std::filesystem::directory_iterator{path}) {
            auto lret = GscInfoFile(sub.path(), opt);
            if (!ret) {
                ret = lret;
            }
        }
        return ret;
    }
    auto pathname = path.generic_string();

    if (!(
        pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
        || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin")
        || pathname.ends_with(".gsic") || pathname.ends_with(".csic") // Serious GSIC format
        )) {
        return 0;
    }
    if (opt.m_verbose) {
        std::cout << "Reading " << pathname << "\n";
    }

    LPVOID buffer = NULL;
    size_t size;
    LPVOID bufferNoAlign = NULL;
    size_t sizeNoAlign;
    if (!utils::ReadFileAlign(path, bufferNoAlign, buffer, sizeNoAlign, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < 0x18) { // MAGIC (8), crc(4), pad(4) name(8)
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << 0x18 << " for " << path << "\n";
        std::free(bufferNoAlign);
        return -1;
    }

    auto ret = GscInfoHandleData(reinterpret_cast<BYTE*>(buffer), size, pathname.c_str(), opt);
    std::free(bufferNoAlign);
    return ret;
}

int DumpInfoFileData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path, std::unordered_map<UINT64, LPCCH>& dataset) {
    // name
    dataset.insert({ data->name, "script" });


    UINT64* includes = reinterpret_cast<UINT64*>(&data->magic[data->include_offset]);
    for (size_t i = 0; i < data->include_count; i++) {
        dataset.insert({ includes[i], "script" });
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(data->magic) + data->globalvar_offset;
    for (size_t i = 0; i < data->globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(gvars_location);
        dataset.insert({ globalvar->name, "var" });

        gvars_location += sizeof(*globalvar) + sizeof(UINT32) * globalvar->num_address;
    }


    auto* exports = reinterpret_cast<tool::gsc::T8GSCExport*>(&data->magic[data->export_table_offset]);
    for (size_t i = 0; i < data->exports_count; i++) {
        const auto& exp = exports[i];
        dataset.insert({ exp.name_space, "namespace" });
        dataset.insert({ exp.name, "function" });

        if (exp.flags & tool::gsc::T8GSCExportFlags::EVENT) {
            dataset.insert({ exp.callback_event, "event" });
        }
    }

    return 0;
}

int DumpInfoFile(const std::filesystem::path& path, std::unordered_map<UINT64, LPCCH>& dataset) {
    if (std::filesystem::is_directory(path)) {
        // directory
        auto ret = 0;
        for (const auto& sub : std::filesystem::directory_iterator{ path }) {
            auto lret = DumpInfoFile(sub.path(), dataset);
            if (!ret) {
                ret = lret;
            }
        }
        return ret;
    }
    auto pathname = path.generic_string();

    if (!(
        pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
        || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin")
        )) {
        return 0;
    }

    std::cout << "Reading " << pathname << "\n";

    LPVOID buffer = NULL;
    size_t size;
    LPVOID bufferNoAlign = NULL;
    size_t sizeNoAlign;
    if (!utils::ReadFileAlign(path, bufferNoAlign, buffer, sizeNoAlign, size)) {
        std::cerr << "Can't read file data for " << path << "\n";
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        std::cerr << "Bad header, file size: " << std::hex << size << "/" << sizeof(tool::gsc::T8GSCOBJ) << " for " << path << "\n";
        std::free(bufferNoAlign);
        return -1;
    }

    auto ret = DumpInfoFileData(reinterpret_cast<tool::gsc::T8GSCOBJ*>(buffer), size, pathname.c_str(), dataset);
    std::free(bufferNoAlign);
    return ret;
}

int dumpdataset(Process& proc, int argc, const char* argv[]) {
    hashutils::ReadDefaultFile();
    // scriptparsetree] [output=dataset.txt]
    LPCCH inputFile = "scriptparsetree";
    LPCCH outputFile = "dataset.csv";
    if (argc > 2) {
        inputFile = argv[2];
        if (argc > 3) {
            inputFile = argv[3];
        }
    }

    std::unordered_map<UINT64, LPCCH> dataset{};

    int ret = DumpInfoFile(inputFile, dataset);
    if (ret) {
        return ret;
    }

    std::ofstream out{ outputFile };
    if (!out) {
        std::cerr << "Can't open output file\n";
    }

    out << "type,name\n";

    for (const auto& [hash, type] : dataset) {
        out << std::hex << type << "," << hashutils::ExtractTmp(type, hash) << "\n";
    }
    out.close();
    return 0;
}

tool::gsc::T8GSCOBJContext::T8GSCOBJContext() {}

// apply ~ to ref to avoid using 0, 1, 2 which might already be used

UINT64 tool::gsc::T8GSCOBJContext::GetGlobalVarName(UINT16 gvarRef) {
    auto f = m_gvars.find(gvarRef);
    if (f == m_gvars.end()) {
        return 0;
    }
    return f->second;
}

LPCCH tool::gsc::T8GSCOBJContext::GetStringValue(UINT32 stringRef) {
    auto f = m_stringRefs.find(stringRef);
    if (f == m_stringRefs.end()) {
        return NULL;
    }
    return f->second;
}

UINT16 tool::gsc::T8GSCOBJContext::AddGlobalVarName(UINT64 value) {
    UINT16 id = ((UINT16)m_gvars.size());
    m_gvars[id] = value;
    return id;
}

UINT32 tool::gsc::T8GSCOBJContext::AddStringValue(LPCCH value) {
    UINT32 id = ((UINT32)m_stringRefs.size());
    m_stringRefs[id] = value;
    return id;
}

int tool::gsc::DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx) {
    // main reading loop
    while (ctx.FindNextLocation()) {
        while (true) {
            if (objctx.m_vmInfo->flags & opcode::VmFlags::VMF_OPCODE_SHORT) {
                ctx.Aligned<UINT16>();
            }
            BYTE*& base = ctx.m_bcl;

            // mark the current location as handled
            auto& loc = ctx.PushLocation();
            loc.handled = true;

            if (ctx.m_lastOpCodeBase == -1) {
                ctx.m_lastOpCodeBase = loc.rloc;
            }

            // compute the late operations (OR|AND)
            for (const auto& lateop : loc.m_lateop) {
                lateop->Run(ctx, objctx);
            }

            UINT16 opCode;

            if (objctx.m_vmInfo->flags & opcode::VmFlags::VMF_OPCODE_SHORT) {
                opCode = *(UINT16*)base;
            }
            else {
                opCode = (UINT16)*base;
            }

            const auto* handler = ctx.LookupOpCode(opCode);


            out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << loc.rloc << ": " << std::flush;

            if (opCode & ~0xFFF) {
                out << std::hex << "FAILURE, FIND errec: " << handler->m_name << "(" << opCode << ")" << "\n";
                opCode &= 0xFFF;
                ctx.m_disableDecompiler = true;
                break;
            }

            out << std::hex << std::setfill('0') << std::setw(sizeof(INT16) << 1) << opCode
                << " "
                << std::setfill(' ') << std::setw(25) << std::left << handler->m_name << std::right
                << " " << std::flush;

            // dump rosetta data
            RosettaAddOpCode((UINT32)(reinterpret_cast<UINT64>(base) - reinterpret_cast<UINT64>(gscFile.Ptr())), handler->m_id);

            // pass the opcode

            if (objctx.m_vmInfo->flags & opcode::VmFlags::VMF_OPCODE_SHORT) {
                base += 2;
            }
            else {
                base++;
            }

            // update ASMContext::WritePadding if you change the format

            auto ret = handler->Dump(out, opCode, ctx, objctx);

            if (ret) {
                break;
            }
        }
    }
    // no more location, we can assume the final size
    // maybe we don't end on a return/end, to check?
    out << "// final size: 0x" << std::hex << ctx.FinalSize() << "\n";

    return 0;
}


int tool::gsc::DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, opcode::DecompContext& dctxt) {
    using namespace tool::gsc::opcode;
    UINT16 code = *(UINT16*)ctx.Aligned<UINT16>();
    // main reading loop
    const auto* ccp = ctx.LookupOpCode(code);

/*
 * Start
 .00000000: 000d CheckClearParams
 .00000002: 000e PreScriptCall
 .00000004: 0eca ScriptFunctionCall        params: 0 spawnstruct
 .00000010: 0b16 GetZero
 .00000012: 0eef GetGlobalObject           classes
 .00000016: 05c0 EvalFieldVariableRef      cct_shared_warning
 .0000001c: 04ce EvalArrayRef
 .0000001e: 0c18 SetVariableField
*/
    
    if (!ccp || ccp->m_id != OPCODE_CheckClearParams) {
        dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << ", expected CheckClearParams\n";
        return -1;
    }

    ctx.m_bcl += 2;

    const auto* preScriptCall = ctx.LookupOpCode(code = *(UINT16*)ctx.Aligned<UINT16>());


    if (!preScriptCall || preScriptCall->m_id != OPCODE_PreScriptCall) {
        dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << ", expected PreScriptCall\n";
        return -1;
    }

    ctx.m_bcl += 2;

    const auto* spawnStruct = ctx.LookupOpCode(code = *(UINT16*)ctx.Aligned<UINT16>());

    
    if (!spawnStruct) {
        dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << "\n";
        return -1;
    }

    if (spawnStruct->m_id != OPCODE_ScriptFunctionCall && spawnStruct->m_id != OPCODE_CallBuiltinFunction) {
        if (gscFile.GetVM() == VM_T9) {
            return 0; // crc dump
        }
        dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << ", expected ScriptFunctionCall\n";
        return -1;
    }

    ctx.m_bcl += 2 + 1;
    ctx.Aligned<UINT64>() += 8; // assume that we have a spawnstruct

    ctx.Aligned<UINT16>() += 2; // GetZero


    if (gscFile.GetVM() != VM_T8) {
        ctx.Aligned<UINT16>() += 2; // EvalFieldVariableFromGlobalObject
        ctx.Aligned<UINT16>() += 2; // - classes
    }
    else {

        ctx.Aligned<UINT16>() += 2; // GetGlobalObject
        ctx.Aligned<UINT16>() += 2; // - classes

        ctx.Aligned<UINT16>() += 2; // EvalFieldVariableRef
    }

    auto& clsName = ctx.Aligned<UINT32>();

    UINT32 name = *(UINT32*)clsName; // __vtable

    auto& cls = objctx.m_classes[name];
    cls.name_space = exp.GetNamespace();

    clsName += 4;

    if (gscFile.GetVM() != VM_T8) {
        ctx.Aligned<UINT16>() += 2; // SetVariableFieldFromEvalArrayRef
    }
    else {
        ctx.Aligned<UINT16>() += 2; // EvalArrayRef
        ctx.Aligned<UINT16>() += 2; // SetVariableField
    }

    while (true) {
        auto& func = ctx.Aligned<UINT16>();

        UINT16 opcode = *(UINT16*)func;
        func += 2;

        const auto* funcOpCode = ctx.LookupOpCode(opcode);

        if (!funcOpCode) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << opcode << "\n";
            return -1;
        }
        if (funcOpCode->m_id == OPCODE_End) {
            break; // end
        }
        if (funcOpCode->m_id != OPCODE_GetResolveFunction) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << opcode << ", excepted GetResolveFunction or End\n";
            return -1;
        }

        auto& getFuncBase = ctx.Aligned<UINT64>();
        UINT32 methodName = ((UINT32*)getFuncBase)[0];
        UINT32 methodClsName = ((UINT32*)getFuncBase)[1];
        getFuncBase += 8;

        auto& uidCodeBase = ctx.Aligned<UINT16>();

        UINT16 uidCodeOp = *(UINT16*)uidCodeBase;

        const auto* uidCodeOpCode = ctx.LookupOpCode(uidCodeOp);

        uidCodeBase += 2;

        if (!uidCodeOpCode) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter\n";
            return -1;
        }

        UINT64 uid;

        switch (uidCodeOpCode->m_id) {
        case OPCODE_GetZero: // INT32
            uid = 0;
            break;
        case OPCODE_GetNegUnsignedInteger: // INT32
            uid = -*(INT32*)ctx.Aligned<INT32>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetNegUnsignedShort: // UINT16
            uid = -*(UINT16*)ctx.Aligned<UINT16>();
            ctx.m_bcl += 2;
            break;
        case OPCODE_GetNegByte: // UINT8
            uid = -*(UINT8*)ctx.Aligned<UINT8>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetByte: // BYTE
            uid = *(BYTE*)ctx.Aligned<BYTE>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetInteger: // INT32
            uid = *(INT32*)ctx.Aligned<INT32>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetLongInteger: // INT64
            uid = *(INT64*)ctx.Aligned<INT64>();
            ctx.m_bcl += 8;
            break;
        case OPCODE_GetUnsignedInteger: // UINT32
            uid = *(UINT32*)ctx.Aligned<UINT32>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetUnsignedShort: // UINT16
            uid = *(UINT16*)ctx.Aligned<UINT16>();
            ctx.m_bcl += 2;
            break;
        default:
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter\n";
            return -1;
        }

        if (methodClsName == name) {
            cls.m_methods.push_back(methodName);
        }
        else {
            cls.m_superClass.emplace(methodClsName);
        }
        auto& mtd = cls.m_vtable[uid];
        mtd.name = methodName;
        mtd.nsp = methodClsName;
        dctxt.WritePadding(out) << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(uid)) << uid
            << " -> &" << hashutils::ExtractTmp("class", methodClsName) << std::flush 
            << "::" << hashutils::ExtractTmp("function", methodName) << ";" << std::endl;

        ctx.Aligned<UINT16>() += 2; // GetZero

        ctx.Aligned<UINT16>() += 2; // EvalGlobalObjectFieldVariable
        ctx.Aligned<UINT16>() += 2; // - gvar
        ctx.Aligned<UINT32>() += 4; // - ref
        ctx.Aligned<UINT16>() += 2; // EvalArray
        ctx.Aligned<UINT16>() += 2; // CastFieldObject
        ctx.Aligned<UINT16>() += 2; // EvalFieldVariableRef
        ctx.Aligned<UINT32>() += 4; // - ref

        if (gscFile.GetVM() != VM_T8) {
            ctx.Aligned<UINT16>() += 2; // SetVariableFieldFromEvalArrayRef
        }
        else {
            ctx.Aligned<UINT16>() += 2; // EvalArrayRef
            ctx.Aligned<UINT16>() += 2; // SetVariableField
        }
    }
/*
* Field
.000002f4: 086c GetResolveFunction        &cct_shared_warning::__constructor
.00000300: 0a9a GetInteger                674154906
.00000308: 056d GetZero
.0000030a: 07f7 EvalGlobalObjectFieldVariable classes.cct_shared_warning
.00000314: 09e4 EvalArray
.00000316: 05e3 CastFieldObject
.00000318: 0af5 EvalFieldVariableRef      __vtable
.00000320: 0887 EvalArrayRef
.00000322: 00f3 SetVariableField
* End
End
*/
    dctxt.WritePadding(out) << "// class " << hashutils::ExtractTmp("class", name) << std::flush;
    if (cls.m_superClass.size()) {
        out << " : ";
        for (auto it = cls.m_superClass.begin(); it != cls.m_superClass.end(); it++) {
            if (it != cls.m_superClass.begin()) {
                out << ", ";
            }
            out << hashutils::ExtractTmp("class", *it) << std::flush;
        }
    }
    out << "\n";
    return 0;
}

int tool::gsc::ComputeSize(GSCExportReader& exp, BYTE* gscFile, gsc::opcode::Platform plt, gsc::opcode::VmInfo* vminfo) {
    using namespace opcode;
    BYTE* loc = gscFile + exp.GetAddress();

    ASMSkipContext ctx{ loc, plt, vminfo };

    while (ctx.FindNextLocation()) {
        while (true) {
            // align to next opcode
            auto& base = ctx.Aligned<UINT16>();

            // mark the current location as handled
            auto& loc = ctx.PushLocation();
            loc.handled = true;

            UINT16 opCode = *(UINT16*)base;

            const auto* handler = ctx.LookupOpCode(opCode);

            if (opCode & 0x1000) {
                return 0; // bad code
            }

            // pass the opcode
            base += 2;

            if (handler->Skip(opCode, ctx)) {
                break;
            }
        }
    }

    int max = 0;
    for (auto& [loc, ref] : ctx.m_locs) {
        if (max < ref.rloc) {
            max = ref.rloc;
        }
    }
    return max + 2; // +1 for the Return/End operator
}


void tool::gsc::DumpFunctionHeader(GSCExportReader& exp, std::ostream& asmout, GSCOBJReader& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, int padding) {
    auto remapedFlags = gscFile.RemapFlagsExport(exp.GetFlags());
    bool classMember = remapedFlags & (T8GSCExportFlags::CLASS_MEMBER | T8GSCExportFlags::CLASS_DESTRUCTOR);

    if (ctx.m_opt.m_func_header) {
        utils::Padding(asmout, padding) << "// Namespace "
            << hashutils::ExtractTmp(classMember ? "class" : "namespace", exp.GetNamespace()) << std::flush << "/"
            << hashutils::ExtractTmp((remapedFlags & T8GSCExportFlags::EVENT) ? "event" : "namespace", exp.GetFileNamespace()) << std::endl;
        utils::Padding(asmout, padding) << "// Params " << (int)exp.GetParamCount() << ", eflags: 0x" << std::hex << (int)exp.GetFlags();

        if (remapedFlags == T8GSCExportFlags::CLASS_VTABLE) {
            asmout << " vtable";
        }
        else {
            if (remapedFlags & T8GSCExportFlags::LINKED) {
                asmout << " linked";
            }
            if (remapedFlags & T8GSCExportFlags::CLASS_LINKED) {
                asmout << " class_linked";
            }
        }

        asmout << std::endl;
        utils::Padding(asmout, padding) << std::hex << "// Checksum 0x" << exp.GetChecksum() << ", Offset: 0x" << exp.GetAddress() << std::endl;

        auto size = ctx.FinalSize();
        if (size > 2) { // at least one opcode
            utils::Padding(asmout, padding) << std::hex << "// Size: 0x" << size << "\n";
        }
    }

    if (remapedFlags == T8GSCExportFlags::CLASS_VTABLE) {
        utils::Padding(asmout, padding) << "vtable " << hashutils::ExtractTmp("class", exp.GetName());
    }
    else {

        bool specialClassMember = !ctx.m_opt.m_dasm && classMember &&
            ((remapedFlags & T8GSCExportFlags::CLASS_DESTRUCTOR) || g_constructorName == exp.GetName());

        utils::Padding(asmout, padding);

        if (!specialClassMember) {
            asmout << "function ";
        }
        if (remapedFlags & T8GSCExportFlags::PRIVATE) {
            asmout << "private ";
        }
        if (remapedFlags & T8GSCExportFlags::AUTOEXEC) {
            asmout << "autoexec ";
        }
        if (remapedFlags & T8GSCExportFlags::EVENT) {
            asmout << "event_handler[" << hashutils::ExtractTmp("event", exp.GetFileNamespace()) << "] " << std::flush;
        }

        if (ctx.m_opt.m_dasm && (classMember || (remapedFlags & T8GSCExportFlags::CLASS_DESTRUCTOR))) {
            asmout << hashutils::ExtractTmp("class", exp.GetNamespace())
                << std::flush << "::";

            if (exp.GetFlags() & T8GSCExportFlags::CLASS_DESTRUCTOR) {
                asmout << "~";
            }
        }
        auto detourVal = objctx.m_gsicInfo.detours.find(exp.GetAddress());

        if (detourVal != objctx.m_gsicInfo.detours.end()) {
            auto* detour = detourVal->second;

            asmout << "detour ";
            if (detour->replaceNamespace) {
                asmout << hashutils::ExtractTmp("namespace", detour->replaceNamespace) << std::flush;
            }
            auto replaceScript = *reinterpret_cast<UINT64*>(&detour->replaceScriptTop);
            if (replaceScript) {
                asmout << "<" << hashutils::ExtractTmpScript(replaceScript) << ">" << std::flush;
            }

            if (detour->replaceNamespace) {
                asmout << "::";
            }

            asmout
                << hashutils::ExtractTmp("function", detour->replaceFunction) << std::flush;
        }
        else {
            asmout << hashutils::ExtractTmp("function", exp.GetName());
        }

    }


    asmout << std::flush << "(";

    // local var size = <empty>, <params>, <localvars> so we need to check that we have at least param_count + 1
    if (ctx.m_localvars.size() > exp.GetParamCount()) {
        for (size_t i = 0; i < exp.GetParamCount(); i++) {
            if (i) {
                asmout << ", ";
            }

            // -1 to avoid the <empty> object, -1 because we are in reverse order
            const auto& lvar = ctx.m_localvars[ctx.m_localvars.size() - i - 2];

            if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC) {
                asmout << "...";
            }
            else {
                if (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF) {
                    asmout << "&";
                }
                else if (gscFile.GetVM() != tool::gsc::opcode::VM_T8 && (lvar.flags & tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF)) {
                    asmout << "*";
                }

                asmout << hashutils::ExtractTmp("var", lvar.name) << std::flush;
            }

            BYTE mask = ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF);

            if (ctx.m_vm != tool::gsc::opcode::VM_T8) {
                mask &= ~tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF;
            }
            
            if (lvar.flags & mask) {
                asmout << " (unk flags: " << std::hex << (int)lvar.flags << ")";
            }
            if (lvar.defaultValueNode) {
                asmout << " = ";
                opcode::DecompContext dctx = { 0, 0, ctx };
                lvar.defaultValueNode->Dump(asmout, dctx);
            }
        }
    }
    asmout << ")";
}

int tool::gsc::gscinfo(Process& proc, int argc, const char* argv[]) {
    GscInfoOption opt{};

    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
        return 0;
    }

    gRosettaOutput = opt.m_rosetta;
    gDumpStrings = opt.m_dump_strings;

    hashutils::SaveExtracted(opt.m_dump_hashmap != NULL);
    bool computed = false;
    auto ret = 0;
    for (const auto& file : opt.m_inputFiles) {
        auto lret = GscInfoFile(file, opt);
        if (!ret) {
            ret = lret;
        }
    }
    hashutils::WriteExtracted(opt.m_dump_hashmap);

    if (gDumpStrings) {
        std::ofstream os{ gDumpStrings };
        if (!os) {
            std::cerr << "Can't open string output\n";
        }
        for (const auto& str : gDumpStringsStore) {
            os << str << "\n";
        }
        os.close();
    }
    if (gRosettaOutput) {
        std::ofstream os{ gRosettaOutput, std::ios::binary };

        if (!os) {
            std::cerr << "Can't open rosetta output\n";
        }
        else {
            os.write("ROSE", 4);

            auto len = gRosettaBlocks.size();
            os.write(reinterpret_cast<const char*>(&len), sizeof(len));

            for (const auto& [key, data] : gRosettaBlocks) {
                // gsc header
                os.write(reinterpret_cast<const char*>(&data.header), sizeof(data.header));
                len = data.blocks.size();
                os.write(reinterpret_cast<const char*>(&len), sizeof(len));
                for (const auto& block : data.blocks) {
                    os.write(reinterpret_cast<const char*>(&block), sizeof(block));
                }
            }

            // TODO: add crc
            os.write("END", 3);


            os.close();
            std::cerr << "Rosetta index created into '" << gRosettaOutput << "'\n";
        }
    }

    std::cout << "done.\n";
    return ret;
}

ADD_TOOL("gscinfo", " (intput)*", "write info about a script in asm file", nullptr, gscinfo);
ADD_TOOL("dds", " [input=scriptparsetree] [output=dataset.csv]", "dump dataset from gscinfo", nullptr, dumpdataset);
