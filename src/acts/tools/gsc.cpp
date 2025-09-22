#include <includes.hpp>
#include <deps/miniz.hpp>
#include <core/async.hpp>
#include <core/bytebuffer.hpp>
#include <utils/decrypt.hpp>
#include <BS_thread_pool.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_decompiler.hpp"
#include "tools/gsc_vm.hpp"
#include "tools/gsc_opcode_nodes.hpp"
#include "tools/cw/cw.hpp"
#include "tools/gsc_acts_debug.hpp"
#include "tools/gsc_gdb.hpp"
#include "tools/gsc_iw.hpp"
#include "actscli.hpp"
#include <acts_vm.hpp>

using namespace tool::gsc;
using namespace tool::gsc::opcode;

enum DumpVTableAnswer : int {
    DVA_OK = 0,
    DVA_NOT,
    DVA_BAD,
};

GscInfoOption::GscInfoOption() {
    // set default formatter
    m_formatter = &tool::gsc::formatter::GetDefaultFormatter();
}

bool tool::gsc::GscDecompilerGlobalContext::WarningType(GscDecompilerGlobalContextWarn warn) {
    core::async::opt_lock_guard lg{ asyncMtx };
    if ((warningOpt & warn)) return false;
    warningOpt |= warn;
    return true;
}

bool GscInfoOption::Compute(const char** args, INT startIndex, INT endIndex) {
    // default values
    for (size_t i = startIndex; i < endIndex; i++) {
        const char* arg = args[i];

        if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
            m_help = true;
        }
        else if (!strcmp("-g", arg) || !_strcmpi("--gsc", arg)) {
            m_dcomp = true;
        }
        else if (!strcmp("-a", arg) || !_strcmpi("--asm", arg)) {
            m_dasm = true;
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
        else if (!_strcmpi("--nofunc", arg)) {
            m_func = false;
        }
        else if (!_strcmpi("--tokens", arg)) {
            m_tokens = true;
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
        else if (!strcmp("-L", arg) || !_strcmpi("--floc", arg)) {
            m_func_floc = true;
        }
        else if (!strcmp("-P", arg) || !_strcmpi("--nopatch", arg)) {
            m_patch = false;
        }
        else if (!strcmp("-V", arg) || !_strcmpi("--vars", arg)) {
            m_show_func_vars = true;
        }
        else if (!strcmp("-d", arg) || !_strcmpi("--gdb", arg)) {
            m_generateGdbData = true;
        }
        else if (!_strcmpi("--gdb-small", arg)) {
            m_generateGdbBaseData = false;
        }
        else if (!_strcmpi("--path-output", arg)) {
            m_usePathOutput = true;
        }
        else if (!_strcmpi("--no-usings-sort", arg)) {
            m_noUsingsSort = true;
        }
        else if (!_strcmpi("--no-str-decrypt", arg)) {
            m_noStrDecrypt = true;
        }
        else if (!_strcmpi("--lc", arg)) {
            m_lineCount = true;
        }
        else if (!strcmp("-s", arg) || !_strcmpi("--skip-data", arg)) {
            m_dumpSkipData = true;
        }
        else if (!_strcmpi("--ignore-dbg-plt", arg)) {
            m_ignoreDebugPlatform = true;
        }
        else if (!_strcmpi("--data-dump", arg)) {
            m_dataDump = true;
        }
        else if (!strcmp("-A", arg) || !_strcmpi("--sync", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            const char* mode{ args[++i] };
            if (!_strcmpi("sync", mode)) {
                m_sync = true;
            } else if (!_strcmpi("async", mode)) {
                m_sync = false;
            }
            else {
                LOG_ERROR("Bad value for param: {}!", arg);
                return false;
            }
        }
        else if (!_strcmpi("--vm-split", arg)) {
            m_splitByVm = true;
        }
        else if (!_strcmpi("--test-header", arg)) {
            m_test_header = true;
        }
        else if (!_strcmpi("--vtable", arg)) {
            m_vtable = true;
        }
        else if (!_strcmpi("--debug-hashes", arg)) {
            m_debugHashes = true;
        }
        else if (!_strcmpi("--vtable-dump", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            vtable_dump = args[++i];
        }
        else if (!strcmp("-t", arg) || !_strcmpi("--type", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_platform = PlatformOf(args[++i]);

            if (!m_platform) {
                LOG_ERROR("Unknown platform: {}!", args[i]);
                return false;
            }
        }
        else if (!strcmp("-f", arg) || !_strcmpi("--format", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_formatter = &formatter::GetFromName(args[++i]);

            if (_strcmpi(m_formatter->name, args[i])) {
                LOG_ERROR("Unknown formatter: {}! Use {}", args[i], m_formatter->name);
                return false;
            }
        }
        else if (!strcmp("-v", arg) || !_strcmpi("--vm", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_vm = VMOf(args[++i]);

            if (!m_vm) {
                LOG_ERROR("Unknown vm: {}!", args[i]);
                return false;
            }
        }
        else if (!_strcmpi("--internalblocks", arg)) {
            m_show_internal_blocks = true;
        }
        else if (!_strcmpi("--internalnames", arg)) {
            m_use_internal_names = true;
        }
        else if (!_strcmpi("--jumpdelta", arg)) {
            m_show_jump_delta = true;
        }
        else if (!_strcmpi("--prestruct", arg)) {
            m_show_pre_dump = true;
        }
        else if (!_strcmpi("--markjump", arg)) {
            m_mark_jump_type = true;
        }
        else if (!_strcmpi("--refcount", arg)) {
            m_show_ref_count = true;
            }
        else if (!_strcmpi("--hideop", arg)) {
            m_show_opcode_values = false;
        }
        else if (!_strcmpi("--rawhash", arg)) {
            m_rawhash = true;
        }
        else if (!_strcmpi("--no-path", arg)) {
            m_noPath = true;
        }
        else if (!_strcmpi("--displaystack", arg)) {
            m_display_stack = true;
        }
        else if (!strcmp("-i", arg) || !_strcmpi("--ignore", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }

            for (const char* param = args[++i]; *param; param++) {
                switch (*param) {
                case 'd':
                    m_stepskip |= STEPSKIP_DEV;
                    break;
                case 'D':
                    m_stepskip |= STEPSKIP_DEVBLOCK_INLINE;
                    break;
                case 's':
                    m_stepskip |= STEPSKIP_SWITCH;
                    break;
                case 'e':
                    m_stepskip |= STEPSKIP_FOREACH;
                    break;
                case 'w':
                    m_stepskip |= STEPSKIP_WHILE;
                    break;
                case 'i':
                    m_stepskip |= STEPSKIP_IF;
                    break;
                case 'f':
                    m_stepskip |= STEPSKIP_FOR;
                    break;
                case 'r':
                    m_stepskip |= STEPSKIP_RETURN;
                    break;
                case 'R':
                    m_stepskip |= STEPSKIP_BOOL_RETURN;
                    break;
                case 'c':
                    m_stepskip |= STEPSKIP_CLASSMEMBER_INLINE;
                    break;
                case 'C':
                    m_stepskip |= STEPSKIP_COMPILER_PATTERNS;
                    break;
                case 'S':
                    m_stepskip |= STEPSKIP_SPECIAL_PATTERN;
                    break;
                case 'a':
                    m_stepskip = ~0;
                    break;
                default:
                    LOG_ERROR("Bad param for {}: '{}'!", arg, *param);
                    return false;
                }
            }
        }
        else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_outputDir = args[++i];
        }
        else if (!strcmp("-O", arg) || !_strcmpi("--output-dbg", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_dbgOutputDir = args[++i];
        }
        else if (!_strcmpi("--input-dbg", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_dbgInputDir = args[++i];
        }
        else if (!_strcmpi("--gdb-dump", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_gdbZipOutputFile = args[++i];
        }
        else if (!strcmp("-m", arg) || !_strcmpi("--hashmap", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_dump_hashmap = args[++i];
        }
        else if (!_strcmpi("--dumpstrings", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_dump_strings = args[++i];
        }
        else if (!strcmp("-C", arg) || !_strcmpi("--copyright", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_copyright = args[++i];
        }
        else if (!strcmp("-r", arg) || !_strcmpi("--rosetta", arg)) {
            if (i + 1 == endIndex) {
                LOG_ERROR("Missing value for param: {}!", arg);
                return false;
            }
            m_rosetta = args[++i];
        }

        else if (*arg == '-') {
            LOG_ERROR("Unknown option: {}!", arg);
            return false;
        }
        else {
            m_inputFiles.push_back(arg);
        }
    }
    if (!m_inputFiles.size()) {
        //m_inputFiles.push_back("scriptparsetree");
        LOG_ERROR("No input param");
        return false; // missing param
    }
    if (!m_dbgOutputDir) {
        m_dbgOutputDir = m_outputDir;
    }
    if (m_func && !(m_dcomp || m_dasm)) {
        m_dcomp = true; // produce gsc by default
    }

    return true;
}

void GscInfoOption::PrintHelp() {
    LOG_INFO("> gscd [options] (input)");
    LOG_INFO("-h --help          : Print help");
    LOG_INFO("-g --gsc           : Produce GSC");
    LOG_INFO("-a --asm           : Produce ASM");
    LOG_INFO("-t --type [t]      : Set type, default PC, values: 'ps', 'xbox', 'pc', 'pc_alpha'");
    LOG_INFO("-o --output [d]    : ASM/GSC output dir, default same.gscasm");
    LOG_INFO("-O --output-dbg [d]: DBG output dir, default same as --output");
    LOG_INFO("--input-dbg [d]    : DBG input directory, default to none");
    LOG_INFO("-v --vm            : Only decompile a particular vm");
    LOG_INFO("-H --header        : Write file header");
    LOG_INFO("-m --hashmap [f]   : Write hashmap in a file f");
    {
        std::ostringstream formats;

        for (const auto* fmt : formatter::GetFormatters()) {

            formats << " '" << fmt->name << "'";
        }

        LOG_INFO("-f --format [f]    : Use formatter, values:{}, default: '{}'", formats.str(), formatter::GetDefaultFormatter().name);
    }
    LOG_INFO("-l --rloc          : Write relative location of the function code");
    LOG_INFO("-L --floc          : Write file location of the function code");
    LOG_INFO("-C --copyright [t] : Set a comment text to put in front of every file");
    LOG_INFO("-d --gdb           : Dump gdb data");
    LOG_INFO("--gdb-small        : Dump only important gdb data");
    LOG_INFO("--dumpstrings [f]  : Dump strings into f");
    LOG_INFO("--vtable-dump [f]  : Dump vtable information into f");
    LOG_INFO("--path-output      : Use the path for the output name");
    LOG_INFO("-s --skip-data     : Dump skip data (gscbin VMs only), requires --path-output");
    // it's not that I don't want them to be known, it's just to avoid having too many of them in the help
    // it's mostly dev tools
    LOG_DEBUG("-G --gvars         : Write gvars");
    LOG_DEBUG("-U --noincludes    : No includes");
    LOG_DEBUG("-X --exptests      : Enable UNK tests");
    LOG_DEBUG("-V --vars          : Show all func vars");
    LOG_DEBUG("-F --nofuncheader  : No function header");
    LOG_DEBUG("--nofunc           : No function write");
    LOG_DEBUG("-p --postfunchead  : Write post function header in ASM mode");
    LOG_DEBUG("-I --imports       : Write imports");
    LOG_DEBUG("-S --strings       : Write strings");
    LOG_DEBUG("-r --rosetta [f]   : Create Rosetta file");
    LOG_DEBUG("--test-header      : Write test header");
    LOG_DEBUG("--internalblocks   : Show internal blocks ");
    LOG_DEBUG("--jumpdelta        : Show jump delta");
    LOG_DEBUG("--hideop           : Hide opcode values in disassembly");
    LOG_DEBUG("--prestruct        : Show prestruct");
    LOG_DEBUG("--refcount         : Show ref count");
    LOG_DEBUG("--markjump         : Show jump type");
    LOG_DEBUG("--displaystack     : Display stack in disassembly");
    LOG_DEBUG("--vm-split         : Split by VM in the output");
    LOG_DEBUG("--internalnames    : Print asm nodes internal names");
    LOG_DEBUG("--rawhash          : Add raw hashes to export dump");
    LOG_DEBUG("--no-path          : No path extraction");
    LOG_DEBUG("--no-usings-sort   : No usings sort");
    LOG_DEBUG("--no-str-decrypt   : No string decrypt");
    LOG_DEBUG("--ignore-dbg-plt   : ignore debug platform info");
    LOG_DEBUG("-A --sync [mode]   : Sync mode: async or sync");
    LOG_DEBUG("--vtable           : Do not hide and decompile vtable functions");
    LOG_DEBUG("--debug-hashes     : Debug hash alogrithm");
    LOG_DEBUG("--data-dump        : Dump data in asm");
    LOG_DEBUG("-i --ignore[t + ]  : ignore step : ");
    LOG_DEBUG("                     a : all, d: devblocks, s : switch, e : foreach, w : while, i : if, f : for, r : return");
    LOG_DEBUG("                     R : bool return, c: class members, D: devblocks inline, S : special patterns");
}

static const char* gDumpStrings{};
static std::unordered_set<std::string> gDumpStringsStore{};
static const char* gRosettaOutput{};
static uint64_t gRosettaCurrent{};
static std::map<uint64_t, RosettaFileData> gRosettaBlocks{};

void tool::gsc::RosettaStartFile(GSCOBJHandler& reader) {
    if (!gRosettaOutput) {
        return;
    }

    
    auto& block = gRosettaBlocks[gRosettaCurrent = reader.GetName()];
    // clone the header for the finder
    block.header.resize(reader.GetHeaderSize());
    memcpy(block.header.data(), reader.Ptr(), reader.GetHeaderSize());
}

void tool::gsc::RosettaAddOpCode(uint32_t loc, uint16_t opcode) {
    if (!gRosettaOutput) {
        return;
    }

    auto& block = gRosettaBlocks[gRosettaCurrent].blocks;

    block.push_back(tool::gsc::RosettaOpCodeBlock{ .location = loc, .opcode = opcode });
}

GSCOBJHandler::GSCOBJHandler(byte* file, uint64_t fileSize, size_t buildFlags) : file(file), buildFlags(buildFlags), fileSize(fileSize) {}

void GSCOBJHandler::SetFile(byte* file, size_t fileSize) {
    this->file = file;
    this->fileSize = fileSize;
}

// by default no remapping
byte GSCOBJHandler::RemapFlagsImport(byte flags) {
    return flags;
}
byte GSCOBJHandler::RemapFlagsExport(byte flags) {
    return flags;
}

byte GSCOBJHandler::MapFlagsImportToInt(byte flags) {
    return flags;
}
byte GSCOBJHandler::MapFlagsExportToInt(byte flags) {
    return flags;
}

void GSCOBJHandler::SetNameString(uint32_t name) {
    throw std::runtime_error("can't set string name for this vm");
}
void GSCOBJHandler::DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt) {};
void GSCOBJHandler::DumpHeader(std::ostream& asmout, const GscInfoOption& opt) {
    uint32_t checksum{ GetChecksum() };

    if (checksum) {
        asmout << "// crc: 0x" << std::hex << checksum << " (" << std::dec << checksum << ")" << std::endl;
    }

    asmout
        << std::left << std::setfill(' ')
        << "// size ...... " << std::dec << std::setw(3) << GetFileSize() << " (0x" << std::hex << GetFileSize() << ")" << std::endl;

    uint32_t includesOffset{ GetIncludesOffset() };
    uint32_t includesCount{ GetIncludesCount() };
    if (includesOffset) {
        asmout << "// includes .. " << std::dec << std::setw(3) << includesCount << " (offset: 0x" << std::hex << includesOffset << ")" << std::endl;
    }

    uint32_t stringsOffset{ GetStringsOffset() };
    uint32_t stringsCount{ GetStringsCount() };
    if (stringsOffset) {
        asmout << "// strings ... " << std::dec << std::setw(3) << stringsCount << " (offset: 0x" << std::hex << stringsOffset << ")" << std::endl;
    }

    uint32_t devStringsOffset{ GetDevStringsOffset() };
    uint32_t devStringsCount{ GetDevStringsCount() };
    if (devStringsOffset) {
        asmout << "// dev strs .. " << std::dec << std::setw(3) << devStringsCount << " (offset: 0x" << std::hex << devStringsOffset << ")" << std::endl;
    }

    uint32_t exportsOffset{ GetExportsOffset() };
    uint32_t exportsCount{ GetExportsCount() };
    if (exportsOffset) {
        asmout << "// exports ... " << std::dec << std::setw(3) << exportsCount << " (offset: 0x" << std::hex << exportsOffset << ")" << std::endl;
    }

    uint32_t csegOffset{ GetCSEGOffset() };
    uint32_t csegSize{ GetCSEGSize() };
    if (csegOffset + csegSize) {
        asmout << "// cseg ...... 0x" << std::hex << csegOffset << " + 0x" << std::hex << csegSize << " (0x" << (csegOffset + csegSize) << ")" << std::endl;
    }

    uint32_t importsOffset{ GetImportsOffset() };
    uint32_t importsCount{ GetImportsCount() };
    if (importsOffset) {
        asmout << "// imports ... " << std::dec << std::setw(3) << importsCount << " (offset: 0x" << std::hex << importsOffset << ")" << std::endl;
    }

    uint32_t animSingleOffset{ GetAnimTreeSingleOffset() };
    uint32_t animSingleCount{ GetAnimTreeSingleCount() };
    if (animSingleOffset) {
        asmout << "// animtree1 . " << std::dec << std::setw(3) << animSingleCount << " (offset: 0x" << std::hex << animSingleOffset << ")" << std::endl;
    }

    uint32_t animDoubleOffset{ GetAnimTreeDoubleOffset() };
    uint32_t animDoubleCount{ GetAnimTreeDoubleCount() };
    if (animDoubleOffset) {
        asmout << "// animtree2 . " << std::dec << std::setw(3) << animDoubleCount << " (offset: 0x" << std::hex << animDoubleOffset << ")" << std::endl;
    } 

    uint32_t globalsOffset{ GetGVarsOffset() };
    uint32_t globalsCount{ GetGVarsCount() };
    if (globalsOffset) {
        asmout << "// globals .. " << std::dec << std::setw(3) << globalsCount << " (offset: 0x" << std::hex << globalsOffset << ")" << std::endl;
    }

    DumpHeaderInternal(asmout, opt);
    asmout << std::right << std::flush;
}

void GSCOBJHandler::SwitchHeaderEndian() {
    throw std::runtime_error("SwitchHeaderEndian not implemented for this vm");
}

uint16_t GSCOBJHandler::GetTokensCount() {
    return 0;
}
uint32_t GSCOBJHandler::GetTokensOffset() {
    return 0;
}

int GSCOBJHandler::PreLoadCode(T8GSCOBJContext& ctx, std::ostream& asmout) {
    return tool::OK;
}
std::pair<const char*, size_t> GSCOBJHandler::GetStringHeader(size_t len) {
    return { "", 0 }; // no encryption header by default
}
int GSCOBJHandler::PatchCode(T8GSCOBJContext& ctx) {
    size_t opcodeSize = ctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16) ? 2 : 1;
    if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_LIKE)) {
        if (GetAnimTreeSingleOffset()) {
            // HAS TO BE DONE FIRST BECAUSE THEY ARE STORED USING 1 byte
            uintptr_t unk2c_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeSingleOffset();
            auto anims_count = (int)GetAnimTreeSingleCount();
            for (size_t i = 0; i < anims_count; i++) {
                const auto* unk2c = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(unk2c_location);

                char* s = Ptr<char>(unk2c->address);

                if (!ctx.opt.m_noStrDecrypt) {
                    s = DecryptString(s);
                }

                if (gDumpStrings) {
                    gDumpStringsStore.insert(s);
                }
                if (ctx.gdctx.stringsLoc) {
                    ctx.gdctx.stringsLoc->push_back(s);
                }
                uint32_t ref = ctx.AddStringValue(s);
                const auto* vars = reinterpret_cast<const uint32_t*>(&unk2c[1]);

                if (ref > 256) {
                    LOG_ERROR("Too many animtrees single usage");
                    return tool::BASIC_ERROR;
                }
                else {
                    for (size_t j = 0; j < unk2c->num_address; j++) {
                        Ref(vars[j]) = (byte)ref;
                        ctx.AddStringRef(vars[j], ref);
                    }
                }
                unk2c_location += sizeof(*unk2c) + sizeof(*vars) * unk2c->num_address;
            }
        }

        if (GetDevStringsOffset()) {
            T8GSCString* val = Ptr<T8GSCString>(GetDevStringsOffset());
            for (size_t i = 0; i < GetDevStringsCount(); i++) {
                const char* str;
                if (val->string) {
                    if (ctx.gdbData) {
                        // add this location as a dev string
                        ctx.gdbData->devStringsLocation.insert(val->string);
                    }
                    if (ctx.dbgData && ctx.dbgSize) {
                        if (val->string >= ctx.dbgSize) {
                            LOG_ERROR("Invalid dev string: location outside of debug file: 0x{:x} >= 0x{:x}", val->string, ctx.dbgSize);
                            str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                        }
                        else {
                            str = (const char*)&ctx.dbgData[val->string];
                        }
                    }
                    else {
                        // no gdb
                        if (ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR)) {
                            break; // nothing
                        }
                        str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                    }
                }
                else {
                    str = "<dev string>";
                }

                uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
                for (size_t j = 0; j < val->num_address; j++) {

                    if (ctx.gdbctx) {
                        // use gdb string
                        auto it = ctx.gdbctx->strings.find(loc[j]);
                        if (it != ctx.gdbctx->strings.end()) {
                            std::string& str = it->second;

                            uint32_t strref = ctx.AddStringValue(str.c_str());
                            Ref<uint32_t>(loc[j]) = strref;
                            ctx.AddStringRef(loc[j], strref);
                            continue;
                        }
                    }
                    ctx.m_unkstrings[str].insert(loc[j]);
                    uint32_t strref = ctx.AddStringValue(str);
                    Ref<uint32_t>(loc[j]) = strref;
                    ctx.AddStringRef(loc[j], strref);
                }
                val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
            }
        }

        uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
        int string_count = (int)GetStringsCount();
        for (size_t i = 0; i < string_count; i++) {

            const auto* str = reinterpret_cast<T8GSCString*>(str_location);
            const char* rcstr;
            if (str->string) {
                char* cstr = Ptr<char>(str->string);

                if (!ctx.opt.m_noStrDecrypt) {
                    cstr = DecryptString(cstr);
                }
                rcstr = cstr;
            }
            else {
                rcstr = "<invalid>";
            }
            if (ctx.gdctx.stringsLoc) {
                ctx.gdctx.stringsLoc->push_back(rcstr);
            }
            if (gDumpStrings) {
                gDumpStringsStore.insert(rcstr);
            }
            uint32_t ref = ctx.AddStringValue(rcstr);

            const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
            for (size_t j = 0; j < str->num_address; j++) {
                if (strings[j] >= fileSize) {
                    LOG_ERROR("Invalid string ref 0x{:x} for {}", strings[j], rcstr);
                    return tool::BASIC_ERROR;
                }
                Ref<uint32_t>(strings[j]) = ref;
                ctx.AddStringRef(strings[j], ref);
            }
            str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
        }

        auto imports_count = (int)GetImportsCount();
        uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + GetImportsOffset();
        for (size_t i = 0; i < imports_count; i++) {

            const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);

            uint8_t mappedImportFlags = RemapFlagsImport(imp->flags);

            size_t delta;
            if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_CALLS)) {
                switch (mappedImportFlags & 0xF) {
                case FUNCTION_THREAD:
                case FUNCTION_CHILDTHREAD:
                case ACTS_CALL_BUILTIN_FUNCTION:
                case METHOD_THREAD:
                case METHOD_CHILDTHREAD:
                case ACTS_CALL_BUILTIN_METHOD:
                    delta = 1;
                    break;
                default:
                    delta = 0;
                    break;
                }
            }
            else {
                delta = 0;
            }

            const auto* imports = reinterpret_cast<const uint32_t*>(&imp[1]);
            for (size_t j = 0; j < imp->num_address; j++) {
                if (delta) {
                    Ref<byte>(imports[j]) = imp->param_count;
                }
                uint16_t* loc = Ptr<uint16_t>(imports[j] + delta);
                auto idx = ctx.m_linkedImports.size();
                ctx.m_linkedImports.push_back(*imp);
                *loc = (uint16_t)idx;
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

                if (!ctx.opt.m_noStrDecrypt) {
                    s1 = DecryptString(s1);
                    s2 = DecryptString(s2);
                }
                if (ctx.gdctx.stringsLoc) {
                    ctx.gdctx.stringsLoc->push_back(s1);
                    ctx.gdctx.stringsLoc->push_back(s2);
                }
                if (gDumpStrings) {
                    gDumpStringsStore.insert(s1);
                    gDumpStringsStore.insert(s2);
                }

                uint32_t ref1 = ctx.AddStringValue(s1);
                uint32_t ref2 = ctx.AddStringValue(s2);

                const auto* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                for (size_t j = 0; j < animt->num_address; j++) {
                    auto* loc = Ptr<uint32_t>(vars[j]);
                    // use strings to link them
                    loc[0] = ref1;
                    loc[1] = ref2;

                    ctx.AddStringRef(vars[j], ref1);
                    ctx.AddStringRef(vars[j] + sizeof(*loc), ref2);
                }
                animt_location += sizeof(*animt) + sizeof(*vars) * animt->num_address;
            }
        }

        if (GetTokensOffset()) {
            auto tokens_count = GetTokensCount();
            GSCBINToken* tokens{ Ptr<GSCBINToken>(GetTokensOffset()) };
            for (size_t i = 0; i < tokens_count; i++) {
                GSCOBJTokenData& tdt{ ctx.m_tokens.emplace_back() };
                tdt.loc = tokens[i].location;
                if (tokens[i].type == GBTT_STRING) {
                    tdt.isString = true;
                    tdt.val.str = Ptr<const char>(tokens[i].val);
                }
                else {
                    tdt.isString = false;
                    tdt.val.id = tokens[i].val;
                }
            }
        }

        return tool::OK;// mwiii
    }
    // patching imports unlink the script refs to write namespace::import_name instead of the address
    auto imports_count = (int)GetImportsCount();
    uint32_t import_offset = GetImportsOffset();
    if (import_offset > fileSize) {
        LOG_ERROR("Invalid import table 0x{:x} ", import_offset);
        return tool::BASIC_ERROR;
    }
    uintptr_t import_location = reinterpret_cast<uintptr_t>(file) + import_offset;
    for (size_t i = 0; i < imports_count; i++) {
        const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
        if (import_location - reinterpret_cast<uintptr_t>(file) + sizeof(uint32_t) * imp->num_address > fileSize) {
            LOG_ERROR("Invalid import 0x{:x} with {} addresses", import_location - reinterpret_cast<uintptr_t>(file), imp->num_address);
            return tool::BASIC_ERROR;
        }

        const auto* imports = reinterpret_cast<const uint32_t*>(&imp[1]);
        for (size_t j = 0; j < imp->num_address; j++) {
            uint32_t* loc;
            auto remapedFlags = RemapFlagsImport(imp->flags);

            if (imports[j] > fileSize) {
                LOG_ERROR("Invalid import {}::{} address 0x{:x} > 0x{:x} for i{}#{}", 
                    hashutils::ExtractTmp("namespace", imp->import_namespace), hashutils::ExtractTmp("function", imp->name),
                    imports[j], fileSize, i, j);
                break;
            }

            switch (remapedFlags & CALLTYPE_MASK) {
            case FUNC_METHOD:
                loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize);
                break;
            case FUNCTION:
            case FUNCTION_THREAD:
            case FUNCTION_CHILDTHREAD:
            case METHOD:
            case METHOD_THREAD:
            case METHOD_CHILDTHREAD: {
                // here the game fix function calls with a bad number of params,
                // but for the decomp/dasm we don't care because we only mind about
                // what we'll find on the stack.
                if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_CALL_NO_PARAMS)) {
                    loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize);
                }
                else {
                    Ref<byte>(imports[j] + opcodeSize) = imp->param_count;
                    loc = PtrAlign<uint64_t, uint32_t>(imports[j] + opcodeSize + 1);
                }

            }
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
        uint16_t ref = ctx.AddGlobalVarName(globalvar->name);

        const auto* vars = reinterpret_cast<const uint32_t*>(&globalvar[1]);
        for (size_t j = 0; j < globalvar->num_address; j++) {
            // no align, no opcode to pass, directly the fucking location, cool.
            if (vars[j] >= GetFileSize() - sizeof(uint16_t)) {
                LOG_ERROR("Invalid global variable: 0x{:x}", vars[j]);
                return tool::BASIC_ERROR;
            }
            else {
                Ref<uint16_t>(vars[j]) = ref;
            }
        }
        gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
    }

    if (GetDevStringsOffset() && !(ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR))) {
        T8GSCString* val = Ptr<T8GSCString>(GetDevStringsOffset());
        for (size_t i = 0; i < GetDevStringsCount(); i++) {
            const char* str;
            if (val->string) {
                if (ctx.gdbData) {
                    // add this location as a dev string
                    ctx.gdbData->devStringsLocation.insert(val->string);
                }
                // the acts compiler uses empty strings location when they're not compiled in the gdb
                if (ctx.dbgData && ctx.dbgSize) {
                    if (val->string >= ctx.dbgSize) {
                        LOG_ERROR("Invalid dev string: location outside of debug file: 0x{:x} >= 0x{:x}", val->string, ctx.dbgSize);
                        str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                    }
                    else {
                        str = (const char*)&ctx.dbgData[val->string];
                    }
                }
                else {
                    // no gdb
                    if (ctx.m_formatter && ctx.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR)) {
                        break; // nothing
                    }
                    str = ctx.CloneString(utils::va("<dev string:x%x>", val->string));
                }
            }
            else {
                str = "<dev string>";
            }

            uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
            for (size_t j = 0; j < val->num_address; j++) {

                if (loc[j] + sizeof(uint32_t) >= GetFileSize()) {
                    LOG_ERROR("Invalid devstring location: 0x{:x}", loc[j]);
                    return tool::BASIC_ERROR;
                }

                if (ctx.gdbctx) {
                    // use gdb string
                    auto it = ctx.gdbctx->strings.find(loc[j]);
                    if (it != ctx.gdbctx->strings.end()) {
                        std::string& str = it->second;

                        uint32_t strref = ctx.AddStringValue(str.c_str());
                        //Ref<uint32_t>(loc[j]) = strref;
                        ctx.AddStringRef(loc[j], strref);
                        continue;
                    }
                }
                ctx.m_unkstrings[str].insert(loc[j]);
                uint32_t strref = ctx.AddStringValue(str);
                //Ref<uint32_t>(loc[j]) = strref;
                ctx.AddStringRef(loc[j], strref);
            }
            val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
        }
    }

    uintptr_t str_location = reinterpret_cast<uintptr_t>(file) + GetStringsOffset();
    int string_count = (int)GetStringsCount();
    for (size_t i = 0; i < string_count; i++) {

        const auto* str = reinterpret_cast<T8GSCString*>(str_location);
        const char* rcstr;
        if (str->string) {
            char* cstr = Ptr<char>(str->string);
            if (!ctx.opt.m_noStrDecrypt) {
                cstr = DecryptString(cstr);
            }
            rcstr = cstr;
        }
        else {
            rcstr = "<invalid>";
        }
        if (ctx.gdctx.stringsLoc) {
            ctx.gdctx.stringsLoc->push_back(rcstr);
        }
        if (gDumpStrings) {
            gDumpStringsStore.insert(rcstr);
        }
        uint32_t ref = ctx.AddStringValue(rcstr);

        const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
        for (size_t j = 0; j < str->num_address; j++) {
            // no align too....
            if (strings[j] + sizeof(uint32_t) >= GetFileSize()) {
                LOG_ERROR("Invalid string location: 0x{:x}", strings[j]);
                break;
            }

            //Ref<uint32_t>(strings[j]) = ref;
            if (str->string || !ctx.GetStringValueByLoc(strings[j])) {
                ctx.AddStringRef(strings[j], ref);
            }
        }
        str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
    }

    if (GetAnimTreeDoubleOffset()) {
        uintptr_t animt_location = reinterpret_cast<uintptr_t>(file) + GetAnimTreeDoubleOffset();
        auto anims_count = (int)GetAnimTreeDoubleCount();
        for (size_t i = 0; i < anims_count; i++) {
            const auto* animt = reinterpret_cast<T7GscAnimTree*>(animt_location);

            auto* s1 = Ptr<char>(animt->name);

            uint32_t ref1 = ctx.AddStringValue(s1);
            if (gDumpStrings) {
                gDumpStringsStore.insert(s1);
            }
            if (ctx.gdctx.stringsLoc) {
                ctx.gdctx.stringsLoc->push_back(s1);
            }

            const uint32_t* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
            for (size_t j = 0; j < animt->num_tree_address; j++) {
                uint32_t rloc = *(vars++);
                uint32_t* loc = Ptr<uint32_t>(rloc);
                // use strings to link them
                ctx.m_animTreeLocations[rloc] = ref1;
                *loc = 666;
            }
            const uint64_t* vars2 = reinterpret_cast<const uint64_t*>(vars);
            for (size_t j = 0; j < animt->num_node_address; j++) {
                if (vars2[0] < fileSize) {
                    const char* v = Ptr<const char>(vars2[0]);
                    uint32_t ref2 = ctx.AddStringValue(v);
                    if (gDumpStrings) {
                        gDumpStringsStore.insert(v);
                    }
                    if (ctx.gdctx.stringsLoc) {
                        ctx.gdctx.stringsLoc->push_back(v);
                    }

                    uint32_t rloc = (uint32_t)vars2[1];
                    uint32_t* loc = Ptr<uint32_t>(rloc);
                    loc[0] = ref1;
                    loc[1] = ref2;
                    ctx.AddStringRef(rloc, ref1);
                    ctx.AddStringRef(rloc + sizeof(*loc), ref2);

                }
                vars2 += 2;
            }
            animt_location += sizeof(*animt) + sizeof(*vars) * (animt->num_tree_address + (size_t)animt->num_node_address * 4);
        }
    }
    return tool::OK;
}

void tool::gsc::GSCOBJHandler::DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx) {
}

std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* tool::gsc::GetGscReader(uint64_t vm) {
    return tool::gsc::vm::GetGscReader(vm); // moved
}

struct H32T7GSCExportReader : GSCExportReader {
    T7GSCExport* exp{};

    void SetHandle(void* handle) override { exp = (T7GSCExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return 0; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};

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
struct BINGSCExportReader : GSCExportReader {
    tool::gsc::iw::BINGSCExport* exp{};

    void SetHandle(void* handle) override { exp = (tool::gsc::iw::BINGSCExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->file_name_space; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    uint32_t GetSize() { return exp->size; };
    size_t SizeOf() override { return sizeof(*exp); };
};

struct H64CERGSCExportReader : GSCExportReader {
    IW24GSCExport* exp{};

    void SetHandle(void* handle) override { exp = (IW24GSCExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->file_name_space; };
    uint64_t GetChecksum() override { return 0; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};

struct ACTSExportReader : GSCExportReader {
    acts::vm::ScriptExport* exp{};

    void SetHandle(void* handle) override { exp = (acts::vm::ScriptExport*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->data; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};

struct H64CER2GSCExportReader : GSCExportReader {
    IW24GSCExport2* exp{};

    void SetHandle(void* handle) override { exp = (IW24GSCExport2*)handle; };
    uint64_t GetName() override { return exp->name; };
    uint64_t GetNamespace() override { return exp->name_space; };
    uint64_t GetFileNamespace() override { return exp->file_name_space; };
    uint64_t GetChecksum() override { return exp->checksum; };
    uint32_t GetAddress() override { return exp->address; };
    uint8_t GetParamCount() override { return exp->param_count; };
    uint8_t GetFlags() override { return exp->flags; };
    size_t SizeOf() override { return sizeof(*exp); };
};



const char* T8GSCOBJContext::GetFLocName(uint32_t floc) const {
    // check the exports to find the right floc name
    uint32_t off = scriptfile->GetExportsOffset();

    byte* exportTable = scriptfile->Ptr(off);

    uint32_t max{};
    void* maxPtr{};
    for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
        void* ptr = exportTable + exp->SizeOf() * i;
        if (scriptfile->GetExportsOffset() + exp->SizeOf() * (i + 1) > scriptfile->GetFileSize()) {
            throw std::runtime_error("Invalid export size");
        }
        exp->SetHandle(ptr);

        uint32_t addr = exp->GetAddress();

        if (addr <= floc && addr > max) {
            maxPtr = ptr;
            max = addr;
        }
    }
    
    if (maxPtr) {
        exp->SetHandle(maxPtr);
        return utils::va(
            "%s::%s@%x",
            hashutils::ExtractTmpPath("namespace", exp->GetNamespace()),
            hashutils::ExtractTmp("function", exp->GetName()),
            floc - exp->GetAddress()
        );
    }

    return utils::va("unk:%lx", floc);
}

int tool::gsc::DecompileGsc(byte* data, size_t size, std::filesystem::path fsPath, GscDecompilerGlobalContext& gdctx, byte* dbgData, size_t dbgSize) {
    std::string pathStr{ fsPath.string() };
    const char* path{ pathStr.data() };
    actslib::profiler::Profiler profiler{ "f" };
    actslib::profiler::ProfiledSection ps{ profiler, path };

    const GscInfoOption& opt = gdctx.opt;
    
    T8GSCOBJContext ctx{ gdctx };
    ctx.m_formatter = opt.m_formatter;
    ctx.dbgData = dbgData;
    ctx.dbgSize = dbgSize;
    auto& gsicInfo = ctx.m_gsicInfo;

    gsicInfo.isGsic = size > 4 && !memcmp(data, "GSIC", 4);
    if (gsicInfo.isGsic) {
        actslib::profiler::ProfiledSection ps{ profiler, "gsic reading"};
        LOG_DEBUG("Reading GSIC Compiled Script data");

        size_t gsicSize = 4; // preamble

        auto numFields = *reinterpret_cast<int32_t*>(&data[gsicSize]);
        gsicSize += 4;

        bool gsicError = false;
        for (size_t i = 0; i < numFields; i++) {
            auto fieldType = *reinterpret_cast<int32_t*>(&data[gsicSize]);
            gsicSize += 4;
            switch (fieldType) {
            case 0: // Detour
            {
                auto detourCount = *reinterpret_cast<int32_t*>(&data[gsicSize]);
                gsicSize += 4;
                for (size_t j = 0; j < detourCount; j++) {
                    GsicDetour* detour = reinterpret_cast<GsicDetour*>(&data[gsicSize]);
                    gsicSize += (size_t)(28 + 256 - 1 - (5 * 4) + 1 - 8);

                    // register detour
                    GscDetourInfo& det = gsicInfo.detours[detour->fixupOffset];
                    det.name = detour->name;
                    det.fixupOffset = detour->fixupOffset;
                    det.fixupSize = detour->fixupSize;
                    det.replaceFunction = detour->replaceFunction;
                    det.replaceNamespace = detour->replaceNamespace;
                    det.replaceScript = *reinterpret_cast<uint64_t*>(&detour->replaceScriptTop);
                }
            }
            break;
            default:
                LOG_ERROR("Bad GSIC field type: {}", fieldType);
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

    if (size < 4) {
        LOG_ERROR("GSC file too small, no magic");
        return tool::BASIC_ERROR;
    }

    uint64_t vmVal;
    if (!memcmp("GSC", data, 4)) {
        vmVal = tool::gsc::opcode::VMI_IW_GSCBIN;

        if (!ctx.opt.m_vm) {
            LOG_INFO("GSCBIN decompiler requires a vm");
            return tool::BASIC_ERROR;
        }

        if (!IsValidVmMagic(opt.m_vm, ctx.m_vmInfo)) {
            LOG_ERROR("Can't find gscbin vm 0x{:x}", (uint64_t)ctx.opt.m_vm);
            return tool::BASIC_ERROR;
        }
    }
    else {
        if (size < 8) {
            LOG_ERROR("GSC file too small, no magic");
            return tool::BASIC_ERROR;
        }
        vmVal = *reinterpret_cast<uint64_t*>(data);

        if (opt.m_vm && vmVal != opt.m_vm) {
            LOG_INFO("Not the wanted vm: 0x{:x} != 0x{:x}", vmVal, (uint64_t)opt.m_vm);
            return tool::OK;
        }

        if (!IsValidVmMagic(vmVal, ctx.m_vmInfo)) {
            LOG_ERROR("Bad vm 0x{:x} for file {}", vmVal, path);
            return tool::BASIC_ERROR;
        }
    }
    hashutils::ReadDefaultFile();

    auto readerBuilder = GetGscReader(vmVal);

    if (!readerBuilder) {
        LOG_ERROR("No handler available for vm 0x{:x} for file {}", vmVal, path);
        return tool::BASIC_ERROR;
    }

    ctx.scriptfile = (*readerBuilder)(data, size);
    ctx.exp = CreateExportReader(ctx.m_vmInfo);

    GSCOBJHandler* scriptfile{ ctx.scriptfile.get() };
    GSCExportReader* exp{ ctx.exp.get() };

    // we keep it because it should also check the size
    if (!scriptfile->IsValidHeader(size)) {
        LOG_ERROR("Bad header 0x{:x} for file {}", scriptfile->Ref<uint64_t>(), path);
        return tool::BASIC_ERROR;
    }
    scriptfile->originalFile = &fsPath;

    utils::LineStreamBuf asmoutFile{};
    std::ostream asmout{ &asmoutFile };
    std::ostream& nullstream{ utils::NullStream() };
    utils::CloseEnd asmoutclose{ asmoutFile };
    ctx.lineBuf = &asmoutFile;

    if (ctx.opt.m_usePathOutput) {
        std::filesystem::path file{ std::filesystem::path{opt.m_outputDir} / fsPath };

        file.replace_extension();
        bool loaded{};
        if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
            std::string fn{ file.filename().string() };

            if (std::find_if(fn.begin(), fn.end(), [](auto c) { return c > '9' || c < '0'; }) == fn.end()) {
                // only number
                try {
                    int id{ (int)std::strtol(fn.data(), nullptr, 10) };
                    const char* opaque{ tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, id, false) };

                    if (opaque) {
                        file = std::filesystem::path{opt.m_outputDir} / opaque;
                        loaded = true;
                    }
                }
                catch (...) {}
            }
        }
        if (!loaded) {
            file.replace_extension(".gsc");
        }

        {
            core::async::opt_lock_guard lg{ gdctx.asyncMtx };
            std::filesystem::create_directories(file.parent_path());
        }
        if (!gdctx.noDump) {
            asmoutFile.in.open(file);

            if (!asmoutFile.in) {
                LOG_ERROR("Can't open path output file {}", file.string());
                return tool::BASIC_ERROR;
            }
        }
        LOG_INFO("Decompiling into '{}'...", file.string());
    }
    
    tool::gsc::RosettaStartFile(*scriptfile);
    ctx.currentPlatform = opt.m_platform;
    ctx.isBigEndian = ctx.m_vmInfo->IsPlatformBigEndian(ctx.currentPlatform);

    bool endianSwapped{};
    if (ctx.SwitchEndian()) {
        scriptfile->SwitchHeaderEndian();
        endianSwapped = true;
    }

    // required for gscbin
    int preloadRet{ scriptfile->PreLoadCode(ctx, opt.m_dumpSkipData ? asmout : nullstream) };
    if (preloadRet) {
        return preloadRet > 0 ? 0 : preloadRet;
    }


    std::stringstream dbgHeader{};

    bool inFileDBG{ !dbgData || !dbgSize };
    if (inFileDBG) {
        // search the debug data inside the script
        dbgData = scriptfile->Ptr(scriptfile->GetHeaderSize());
        dbgSize = scriptfile->GetFileSize() - scriptfile->GetHeaderSize();
    }

    core::bytebuffer::ByteBuffer dbgReader{ dbgData, dbgSize };

    if (dbgReader.CanRead(8)) {
        uint64_t magic{ *dbgReader.Ptr<uint64_t>() };

        tool::gsc::vm::GscGdb* dbgreader{ gsc::vm::GetGdbReader(magic) };

        if (!dbgreader) {
            if (!inFileDBG) {
                LOG_WARNING("No debug handler for magic 0x{:x}", magic);
            }
        }
        else {
            dbgReader.Goto(0);

            try {
                dbgreader->load(ctx, dbgReader, dbgHeader);
            }
            catch (std::runtime_error& err) {
                LOG_WARNING("Can't parse gdb data {}", err.what());
            }
        }
    }

    if (endianSwapped != ctx.SwitchEndian()) {
        scriptfile->SwitchHeaderEndian();
    }

    char asmfnamebuff[1000];

    const char* extractedName{ hashutils::ExtractPtr(scriptfile->GetName()) };

    if (opt.m_debugHashes && extractedName) {
        uint64_t hashPath{ ctx.m_vmInfo->HashPath(extractedName) };

        if (hashPath != scriptfile->GetName() && gdctx.WarningType(GDGCW_BAD_HASH_PATH)) {
            LOG_WARNING("Invalid hash algorithm for extracted name 0x{:x} != 0x{:x} for {}", scriptfile->GetName(), hashPath, extractedName);
        }
    }
    bool isCsc{};
    bool typeSure{};
    if (!scriptfile->HasFlag(GOHF_STRING_NAMES)) {
        const char* scriptNamePtr{ hashutils::ExtractPtr(scriptfile->GetName()) };

        if (scriptNamePtr) {
            std::string_view sw{ scriptNamePtr };
            if (sw.ends_with(".csc")) {
                isCsc = true;
                typeSure = true;
            }
            else if (sw.ends_with(".gsc")) {
                isCsc = false;
                typeSure = true;
            }
            else {
                // the default script doesn't end with .gsc/.csc, it's fine
                // LOG_WARNING("Script's name doesn't end with .gsc or .csc extension, can't extrapolate type: {}", scriptNamePtr);
                goto ignoreCscGsc;
            }
        }

        if (scriptfile->GetIncludesOffset()) {
            uint32_t includeOffset = scriptfile->GetIncludesOffset();
            uint64_t* includes = scriptfile->Ptr<uint64_t>(includeOffset);
            if (!(includeOffset + scriptfile->GetIncludesCount() * sizeof(uint64_t) > scriptfile->GetFileSize())) {
                for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                    const char* incPtr{ hashutils::ExtractPtr(includes[i]) };

                    if (incPtr) {
                        std::string_view sw{ incPtr };
                    
                        if (sw.ends_with(".csc")) {
                            if (!isCsc && typeSure) {
                                typeSure = false;
                                LOG_WARNING("Found csc and gsc includes in the same script, can't extrapolate type");
                                break;
                            }
                            isCsc = true;
                            typeSure = true;
                        }
                        else if (sw.ends_with(".gsc")) {
                            if (isCsc && typeSure) {
                                typeSure = false;
                                LOG_WARNING("Found csc and gsc includes in the same script, can't extrapolate type");
                                break;
                            }
                            isCsc = false;
                            typeSure = true;
                        }
                        else {
                            typeSure = false;
                            LOG_WARNING("Found an include without .gsc or .csc extension, can't extrapolate type: {}", incPtr);
                            break;
                        }

                    }
                }
            }
        }
    }
ignoreCscGsc:

    if (opt.m_outputDir) {
        uint64_t hashname{ scriptfile->GetName() };

        const char* outDir;
        if (opt.m_splitByVm) {
            outDir = utils::va("%s/%s", opt.m_outputDir, tool::gsc::opcode::VMIdFancyName(ctx.m_vmInfo->vmMagic));
        }
        else {
            outDir = opt.m_outputDir;
        }

        if (hashname) {
            const char* name = opt.m_noPath ? nullptr : hashutils::ExtractPtr(hashname);

            if (!name) {
                const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
                if (actscli::options().heavyHashes) {
                    sprintf_s(asmfnamebuff, "%s/%016llX.%s", outDir, hashname, fileExt);
                }
                else {
                    sprintf_s(asmfnamebuff, "%s/hashed/script/script_%llx.%s", outDir, hashname, fileExt);
                }
            }
            else {
                sprintf_s(asmfnamebuff, "%s/%s", outDir, name);

                char* extName{ utils::CloneString(name) };
                std::string_view usingSw{ extName };
                size_t usingSwPath{ usingSw.find_last_of('/') };
                if (usingSw.ends_with(".gsc") || usingSw.ends_with(".csc")) {
                    extName[usingSw.length() - 4] = 0;
                }

                if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                    // IW vm import types
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(extName), extName);
                }
                if (usingSwPath != std::string::npos) {
                    const char* namespaceAdd(&extName[usingSwPath + 1]);
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                    if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                        hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                    }
                }
            }
        }
        else {
            const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
            std::filesystem::path fn = fsPath;

            fn.replace_extension(fileExt);

            std::string name{ fn.string() };
            LOG_WARNING("Can't find script name, using {}", name);
            sprintf_s(asmfnamebuff, "%s/%s", outDir, name.data());
        }
    }
    else {
        sprintf_s(asmfnamebuff, "%sasm", path);
    }
    profiler.GetCurrent().name = asmfnamebuff;

    if (!ctx.opt.m_usePathOutput) {
        std::filesystem::path file{ std::filesystem::absolute(asmfnamebuff) };

        {
            core::async::opt_lock_guard lg{ gdctx.asyncMtx };
            std::filesystem::create_directories(file.parent_path());
        }
        if (!gdctx.noDump) {
            asmoutFile.in.open(file);

            if (!asmoutFile.in) {
                LOG_ERROR("Can't open output file {} ({})", asmfnamebuff, hashutils::ExtractTmpScript(scriptfile->GetName()));
                return tool::BASIC_ERROR;
            }
            LOG_INFO("Decompiling into '{}'{}...", asmfnamebuff, (gsicInfo.isGsic ? " (GSIC)" : ""));
        }
        else {
            LOG_INFO("Decompiling '{}'{}...", hashutils::ExtractTmpScript(scriptfile->GetName()), (gsicInfo.isGsic ? " (GSIC)" : ""));
        }
    }

    if (opt.m_copyright && *opt.m_copyright) {
        std::string_view cv{ opt.m_copyright };
        
        size_t idx{};
        do {
            size_t nl = cv.find('\n', idx);

            if (nl == std::string::npos) {
                nl = cv.size();
            }

            asmout << "// " << cv.substr(idx, nl - idx) << std::endl;

            idx = nl + 1;
        } while (idx < cv.size());

        if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_LINE_AFTER_COPYRIGHT)) {
            asmout << std::endl;
        }
    }

    auto itdbg = gdctx.debugObjects.find(scriptfile->GetName());

    if (itdbg != gdctx.debugObjects.end()) {
        ctx.gdbctx = itdbg->second; // load dbg object
        LOG_DEBUG("Using dbg file {}", ctx.gdbctx->path.string());
    }

    if (opt.m_gdbZipOutputFile) {
        core::async::opt_lock_guard lg{ gdctx.asyncMtx };
        GscDecompilerGDBData*& gdb{ gdctx.gdbData[scriptfile->GetName()] };
        if (gdb) {
            LOG_WARNING("GDB data defined twice for file {}", hashutils::ExtractTmpScript(scriptfile->GetName()));
        }
        else {
            gdb = gdctx.alloc.New<GscDecompilerGDBData>();
            ctx.gdbData = gdb;
            gdb->gdb = ctx.m_vmInfo->gdbMagic;
            gdb->checksum = scriptfile->GetChecksum();
        }
    }

    if (opt.m_header) {
        asmout << "// ";
        if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
            asmout << hashutils::ExtractTmpScript(scriptfile->GetName()) << "  (" << path << ")";
        }
        else {
            asmout << path;
        }
        asmout << " (size: " << size << " Bytes / " << std::hex << "0x" << size;
        
        if (typeSure) {
            asmout << " / " << (isCsc ? "CSC" : "GSC");
        }

        asmout << ")" << std::endl;

        if (gsicInfo.isGsic) {
            asmout << "// GSIC Compiled script" << ", header: 0x" << std::hex << gsicInfo.headerSize << std::endl;
        }
        if (gsicInfo.detours.size()) {
            asmout << "// detours: " << std::dec << gsicInfo.detours.size() << std::endl;
            for (const auto& [key, detour] : gsicInfo.detours) {
                asmout << "// - ";

                if (detour.replaceNamespace) {
                    asmout << hashutils::ExtractTmpPath("namespace", detour.replaceNamespace) << std::flush;
                }
                if (detour.replaceScript) {
                    asmout << "<" << hashutils::ExtractTmpScript(detour.replaceScript) << ">" << std::flush;
                }

                if (detour.replaceNamespace) {
                    asmout << "::";
                }

                asmout
                    << hashutils::ExtractTmp("function", detour.replaceFunction) << std::flush
                    << " offset: 0x" << std::hex << detour.fixupOffset << ", size: 0x" << detour.fixupSize << std::endl;
            }
        }


        if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_VERSION)) {
            if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_MAGIC)) {
                asmout << "// magic .... 0x" << scriptfile->GetMagic();
            }
            else {
                asmout << "//";
            }
            asmout << " vm: " << ctx.m_vmInfo->name << " (" << PlatformName(ctx.currentPlatform) << ")" << std::endl;
        }

        scriptfile->DumpHeader(asmout, opt);

        asmout << dbgHeader.str();
    }

    // write the strings before the patch to avoid reading pre-decrypted strings
    if (opt.m_strings) {
        if (scriptfile->GetDevStringsOffset() && !(opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NOERROR_STR))) {
            T8GSCString* val = scriptfile->Ptr<T8GSCString>(scriptfile->GetDevStringsOffset());
            for (size_t i = 0; i < scriptfile->GetDevStringsCount(); i++) {

                const char* str = utils::va("<dev string:x%x>", val->string); // Ptr<char>(val->string); // no gdb

                asmout << "Dev String: "
                    << "addr:" << std::hex << val->string << ", "
                    << "count:" << std::dec << (int)val->num_address << ", stype:"
                    << (int)val->type << " -> \"" << str << "\"" << std::endl;
                asmout << "loc: ";

                uint32_t* loc = reinterpret_cast<uint32_t*>(val + 1);
                asmout << ctx.GetFLocName(loc[0]);
                for (size_t j = 1; j < val->num_address; j++) {
                    asmout << "," << ctx.GetFLocName(loc[j]);
                }

                asmout << std::endl;
                val = reinterpret_cast<T8GSCString*>(loc + val->num_address);
            }
        }
        if (scriptfile->GetStringsOffset()) {
            uintptr_t str_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetStringsOffset()));

            for (size_t i = 0; i < scriptfile->GetStringsCount(); i++) {

                const auto* str = reinterpret_cast<T8GSCString*>(str_location);

                asmout << std::hex << "String addr:" << str->string << ", count:" << std::dec << (int)str->num_address << ", type:" << (int)str->type << ", loc:0x" << std::hex << (str_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr())) << std::endl;

                if (str->string >= scriptfile->GetFileSize()) {
                    asmout << "bad string location : 0x" << std::hex << str->string << "/0x" << scriptfile->GetFileSize() << std::endl;
                    break;
                }

                char* encryptedString = scriptfile->Ptr<char>(str->string);

                size_t len{ std::strlen(encryptedString) };
                byte type{ (byte)(*reinterpret_cast<byte*>(encryptedString)) };

                if (str->string + len > scriptfile->GetFileSize()) {
                    asmout << "bad string location + len : 0x" << std::hex << str->string << "/0x" << scriptfile->GetFileSize() << std::endl;
                    break;
                }

                asmout << "encryption: ";
                if ((type & 0xC0) != 0x80) {
                    asmout << "(none)";
                }
                else {
                    asmout << "0x" << std::hex << (int)type;
                }
                asmout << " elen: " << std::dec << len << " -> " << std::flush;

                char* cstr = encryptedString;

                if (!ctx.opt.m_noStrDecrypt) {
                    cstr = scriptfile->DecryptString(cstr);
                }

                size_t dlen{ strlen(cstr) };
                asmout << '"' << utils::FormattedString(cstr) << '"' << "(" << std::dec << dlen;
                
                if (dlen > len) {
                    asmout << ",missing";
                }
                if ((type & 0xC0) == 0x80) {
                    byte ntype{ (byte)(*reinterpret_cast<byte*>(encryptedString)) };
                    asmout << ",pt:0x" << std::hex << (int)ntype;
                    if (encryptedString <= cstr) {
                        size_t delta{ (size_t)(cstr - encryptedString) };
                        asmout << ",delta:0x" << delta;
                        if (delta && delta < 5) {
                            asmout << ",data=";
                            for (size_t i = 0; i < delta; i++) {
                                if (i) asmout << ",";
                                asmout << "0x" << std::hex << (int)reinterpret_cast<byte*>(encryptedString)[i];
                            }
                        }
                    }
                    else {
                        asmout << ",delta:-0x" << (encryptedString - cstr);
                    }
                }
                
                asmout << ")" << std::endl;

                asmout << "location(s): ";

                const auto* strings = reinterpret_cast<const uint32_t*>(&str[1]);
                asmout << ctx.GetFLocName(strings[0]);
                for (size_t j = 1; j < str->num_address; j++) {
                    asmout << "," << ctx.GetFLocName(strings[j]);
                }
                asmout << std::endl;
                str_location += sizeof(*str) + sizeof(*strings) * str->num_address;
            }
        }

        if (scriptfile->GetStringsCount() || scriptfile->GetDevStringsCount()) {
            asmout << std::endl;
        }
    }

    int patchCodeResult{};

    if (opt.m_patch) {
        actslib::profiler::ProfiledSection ps{ profiler, "patch linking"};
        // unlink the script and write custom gvar/string ids
        patchCodeResult = scriptfile->PatchCode(ctx);

        {
            core::async::opt_lock_guard hlg{ hashutils::GetMutex(false) };
            for (const auto& [id, str] : ctx.m_stringRefs) {
                hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(str), str, true);
                hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(str), str, true);
                hashutils::AddPrecomputed(ctx.m_vmInfo->HashPath(str), str, true);

                // use all the known hashes for this VM
                for (auto& [k, func] : ctx.m_vmInfo->hashesFunc) {
                    try {
                        int64_t hash = func.hashFunc(str);

                        if (hash) {
                            hashutils::AddPrecomputed(hash, str, true);
                        }
                    }
                    catch (std::exception&) {
                        // ignore
                    }
                }
            }
        }
    }

    if (opt.m_includes && scriptfile->GetIncludesOffset() && scriptfile->GetIncludesCount()) {
        uint32_t includeOffset = scriptfile->GetIncludesOffset();
        std::vector<std::string> usingsList{};
        if (scriptfile->HasFlag(GOHF_STRING_NAMES)) {
            if (includeOffset + scriptfile->GetIncludesCount() * sizeof(uint32_t) > scriptfile->GetFileSize()) {
                LOG_ERROR("Invalid include offset 0x{:x} > 0x{:x}", includeOffset, scriptfile->GetFileSize());
                return tool::BASIC_ERROR;
            }
            uint32_t* includes = scriptfile->Ptr<uint32_t>(includeOffset);

            for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                if (includes[i] >= scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid include string offset 0x{:x} > 0x{:x}", includes[i], scriptfile->GetFileSize());
                    return tool::BASIC_ERROR;
                }
                //asmout << "#using " << scriptfile->Ptr<char>(includes[i]) << ";" << std::endl;
                const char* usingName{ scriptfile->Ptr<char>(includes[i]) };
                usingsList.emplace_back(usingName);

                std::string_view usingSw{ usingName };
                size_t usingSwPath{ usingSw.find_last_of('/') };

                if (usingSwPath != std::string::npos) {
                    const char* namespaceAdd(&usingName[usingSwPath + 1]);
                    hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                    if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                        hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                    }
                }
            }
        }
        else {
            uint64_t* includes = scriptfile->Ptr<uint64_t>(includeOffset);
            if (includeOffset + scriptfile->GetIncludesCount() * sizeof(uint64_t) > scriptfile->GetFileSize()) {
                LOG_ERROR("Invalid include offset 0x{:x} > 0x{:x}", includeOffset, scriptfile->GetFileSize());
                return tool::BASIC_ERROR;
            }

            for (size_t i = 0; i < scriptfile->GetIncludesCount(); i++) {
                auto [isFound, usingName] = hashutils::ExtractTmpPair("script", includes[i]);
                if (isFound) {
                    hashutils::CleanPath(usingName);

                    if (typeSure) {
                        usingName[std::strlen(usingName) - 4] = 0; // remove .csc / .gsc

                        if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                            // IW vm import types
                            hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(usingName), usingName);
                        }
                        std::string_view usingSw{ usingName };
                        size_t usingSwPath{ usingSw.find_last_of('/') };

                        if (usingSwPath != std::string::npos) {
                            const char* namespaceAdd(&usingName[usingSwPath + 1]);
                            hashutils::AddPrecomputed(ctx.m_vmInfo->HashField(namespaceAdd), namespaceAdd);

                            if (!ctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                                hashutils::AddPrecomputed(ctx.m_vmInfo->HashFilePath(namespaceAdd), namespaceAdd);
                            }
                        }
                    }
                }

                usingsList.emplace_back(usingName);
                //asmout << "#using " << usingName << ";" << std::endl;

                if (opt.m_debugHashes) {
                    const char* incExt{ hashutils::ExtractPtr(includes[i]) };
                    if (incExt) {
                        uint64_t hashPath{ ctx.m_vmInfo->HashPath(incExt) };

                        if (hashPath != includes[i] && gdctx.WarningType(GDGCW_BAD_HASH_PATH_INCLUDE)) {
                            LOG_WARNING("Invalid hash algorithm for extracted include 0x{:x} != 0x{:x} for {}", includes[i], hashPath, incExt);
                        }
                    }
                }
            }
        }
        // better rendering, but not really good for debug
        if (!opt.m_noUsingsSort) {
            std::sort(usingsList.begin(), usingsList.end());
        }

        for (const std::string& usingName : usingsList) {
            asmout << "#using " << usingName << ";" << std::endl;
        }

        asmout << std::endl;
    }

    asmout
        << std::flush;

    scriptfile->DumpExperimental(asmout, opt, ctx);

    if (opt.m_tokens && ctx.m_tokens.size()) {
        asmout << "// tokens" << std::endl;
        for (size_t i = 0; i < ctx.m_tokens.size(); i++) {
            GSCOBJTokenData& data{ ctx.m_tokens[i] };

            asmout  << "token 0x" << std::hex << i << " loc:0x" << data.loc << " | ";
            if (data.isString) {
                asmout << "(str) " << data.val.str;
            }
            else {
                asmout << "(id) " << tool::gsc::iw::GetOpaqueStringForVm(ctx.m_vmInfo->vmMagic, data.val.id);
            }
            asmout << std::endl;
        }
        asmout << std::endl;
    }

    if (opt.m_gvars && scriptfile->GetGVarsOffset()) {
        uintptr_t gvars_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetGVarsOffset()));

        for (size_t i = 0; i < scriptfile->GetGVarsCount(); i++) {
            const auto* globalvar = reinterpret_cast<T8GSCGlobalVar*>(gvars_location);
            asmout << std::hex << "Global var " << hashutils::ExtractTmp("var", globalvar->name) << " " << globalvar->num_address << std::endl;

            asmout << "location(s): ";

            const auto* vars = reinterpret_cast<const uint32_t*>(&globalvar[1]);
            asmout << std::hex << ctx.GetFLocName(vars[0]);
            for (size_t j = 1; j < globalvar->num_address; j++) {
                asmout << std::hex << "," << ctx.GetFLocName(vars[j]);
            }
            asmout << std::endl;
            gvars_location += sizeof(*globalvar) + sizeof(*vars) * globalvar->num_address;
        }
        if (scriptfile->GetGVarsCount()) {
            asmout << std::endl;
        }
    }

    if (opt.m_imports) {
        uintptr_t import_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetImportsOffset()));

        for (size_t i = 0; i < scriptfile->GetImportsCount(); i++) {
            uint64_t name_space;
            uint64_t name;
            size_t impSize;
            byte flags;
            byte param_count;
            uint16_t numAddress;

            if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_LIKE)) {
                if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + sizeof(IW23GSCImport) > scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid import {} location", i);
                    return tool::BASIC_ERROR;
                }
                const auto* imp = reinterpret_cast<IW23GSCImport*>(import_location);
                name_space = imp->name_space;
                name = imp->name;
                flags = imp->flags;
                numAddress = imp->num_address;
                param_count = imp->param_count;
                impSize = sizeof(*imp);
            }
            else {
                if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + sizeof(T8GSCImport) > scriptfile->GetFileSize()) {
                    LOG_ERROR("Invalid import {} location", i);
                    return tool::BASIC_ERROR;
                }
                const auto* imp = reinterpret_cast<T8GSCImport*>(import_location);
                name_space = imp->import_namespace;
                name = imp->name;
                flags = imp->flags;
                param_count = imp->param_count;
                numAddress = imp->num_address;
                impSize = sizeof(*imp);
            }
            if (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr<>()) + impSize + sizeof(uint32_t) * numAddress > scriptfile->GetFileSize()) {
                LOG_ERROR("Invalid import {} num address {}", i, numAddress);
                return tool::BASIC_ERROR;
            }

            asmout << std::hex << "import ";

            auto remapedFlags = scriptfile->RemapFlagsImport(flags);

            switch (remapedFlags & T8GSCImportFlags::CALLTYPE_MASK) {
            case FUNC_METHOD: asmout << "get "; break;
            case FUNCTION: asmout << "function "; break;
            case FUNCTION_THREAD: asmout << "function thread "; break;
            case FUNCTION_CHILDTHREAD: asmout << "function childthread "; break;
            case METHOD: asmout << "method "; break;
            case METHOD_THREAD: asmout << "method thread "; break;
            case METHOD_CHILDTHREAD: asmout << "method childthread "; break;
            case ACTS_CALL_BUILTIN_FUNCTION: asmout << "builtin function "; break;
            case ACTS_CALL_BUILTIN_METHOD: asmout << "builtin method "; break;
            case ACTS_GET_BUILTIN_FUNCTION: asmout << "get builtin function "; break;
            case ACTS_GET_BUILTIN_METHOD: asmout << "get builtin method "; break;
            case ACTS_CALL_BUILTIN_FUNCTION_NO_PARAMS: asmout << "builtin function (np) "; break;
            case ACTS_CALL_BUILTIN_METHOD_NO_PARAMS: asmout << "builtin method (np) "; break;
            default:
                asmout << "<errorflag:" << std::hex << (remapedFlags & 0xF) << "> ";
                break;
            }

            if (remapedFlags & T8GSCImportFlags::DEV_CALL) {
                asmout << "devcall ";
            }

            if ((remapedFlags & T8GSCImportFlags::GET_CALL) == 0) {
                // no need for namespace if we are getting the call dynamically (api or inside-code script)
                asmout << "get ";
            }
            asmout << hashutils::ExtractTmpPath("namespace", name_space) << std::flush << "::";

            asmout << std::hex << hashutils::ExtractTmp("function", name);

            if (opt.m_rawhash) {
                asmout << " (" << std::hex << name_space << "::" << name << ")";
            }

            asmout << std::endl;

            asmout << std::hex << "address: " << numAddress
                << ", params: " << std::dec << (int)param_count
                << ", iflags: 0x" << std::hex << (uint16_t)(flags)
                << ", iftype: 0x" << std::hex << (int)(flags & T8GSCImportFlags::CALLTYPE_MASK)
                << ", loc: 0x" << std::hex << (import_location - reinterpret_cast<uintptr_t>(scriptfile->Ptr()))
                << std::endl;

            asmout << "location(s): ";

            const auto* imports = reinterpret_cast<const uint32_t*>(import_location + impSize);
            asmout << std::hex << ctx.GetFLocName(imports[0]);
            for (size_t j = 1; j < numAddress; j++) {
                asmout << std::hex << "," << ctx.GetFLocName(imports[j]) << "(0x" << imports[j] << ")";
            }
            asmout << std::endl;

            asmout << "--------------" << std::endl;

            import_location += impSize + sizeof(*imports) * numAddress;
        }
        if (scriptfile->GetImportsCount()) {
            asmout << std::endl;
        }
    }
    if (opt.m_imports) {
        if (ctx.m_vmInfo->HasFlag(VmFlags::VMF_ANIMTREE_T7)) {
            // in t7 the anims are in a single struct
            if (scriptfile->GetAnimTreeDoubleOffset()) {
                uintptr_t animt_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetAnimTreeDoubleOffset()));
                size_t anims_count = (size_t)scriptfile->GetAnimTreeDoubleCount();

                for (size_t i = 0; i < anims_count; i++) {
                    const auto* animt = reinterpret_cast<T7GscAnimTree*>(animt_location);

                    if (animt->name >= scriptfile->GetFileSize()) {
                        asmout << std::hex << "invalid animtree name 0x" << animt->name << std::endl;
                    } else {
                        char* s = scriptfile->Ptr<char>(animt->name);

                        asmout << std::hex << "animtree " << (s ? s : "<err>") << " : 0x" << ((byte*)animt - scriptfile->Ptr()) << std::endl;

                        const uint32_t* vars = reinterpret_cast<const uint32_t*>(&animt[1]);
                        asmout << "tree address (" << std::dec << animt->num_tree_address << ", 0x" << std::hex << ((byte*)vars - scriptfile->Ptr()) << "):";
                        for (size_t j = 0; j < animt->num_tree_address; j++) {
                            asmout << " " << ctx.GetFLocName(*(vars++));
                        }
                        asmout << std::endl;
                        const uint64_t* vars2 = reinterpret_cast<const uint64_t*>(vars);
                        asmout << "node address (" << std::dec << animt->num_node_address << ", 0x" << std::hex << ((byte*)vars2 - scriptfile->Ptr()) << "):";
                        for (size_t j = 0; j < animt->num_node_address; j++) {

                            if (vars2[0] >= scriptfile->GetFileSize()) {
                                asmout << std::hex << "invalid animtree 2nd name 0x" << animt->name << std::endl;
                            } else {
                                char* v = scriptfile->Ptr<char>(vars2[0]);
                                // why u64?
                                asmout << " " << ctx.GetFLocName((uint32_t)vars2[1]) << ":" << v;
                            }

                            vars2 += 2;
                        }
                        asmout << std::endl;
                    }

                    animt_location += sizeof(*animt) + sizeof(uint32_t) * (animt->num_tree_address + (size_t)animt->num_node_address * 4);
                }
                if (scriptfile->GetAnimTreeDoubleCount()) {
                    asmout << std::endl;
                }
            }
        } else {
            if (scriptfile->GetAnimTreeSingleOffset()) {
                uintptr_t animt_location = reinterpret_cast<uintptr_t>(scriptfile->Ptr(scriptfile->GetAnimTreeSingleOffset()));
                size_t anims_count = (size_t)scriptfile->GetAnimTreeSingleCount();

                for (size_t i = 0; i < anims_count; i++) {
                    const auto* animt = reinterpret_cast<GSC_USEANIMTREE_ITEM*>(animt_location);

                    if (animt->address >= scriptfile->GetFileSize()) {
                        asmout << std::hex << "invalid animtree name 0x" << animt->address << std::endl;
                    } else {
                        char* s = scriptfile->Ptr<char>(animt->address);
                    
                        asmout << "#using_animtree(";

                        if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                            asmout << " ";
                        }

                        asmout << '"' << utils::FormattedString(s) << '"';

                        if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                            asmout << " ";
                        }

                        asmout << ");" << std::endl;
                    }

                    animt_location += sizeof(*animt) + sizeof(uint32_t) * animt->num_address;
                }
                if (scriptfile->GetAnimTreeDoubleCount()) {
                    asmout << std::endl;
                }
            }
        }
    }

    if (patchCodeResult) {
        return patchCodeResult;
    }

    bool dumpAllErrors = actscli::options().debug;
    int exportErrors{};

    if (opt.m_func) {
        actslib::profiler::ProfiledSection ps{ profiler, "decompiling" };
        // current namespace
        uint64_t currentNSP{};
        const char* currentAnimTree{};

        if (scriptfile->GetExportsOffset() + scriptfile->GetExportsCount() * exp->SizeOf() > scriptfile->GetFileSize()) {
            asmout << "// INVALID EXPORT TABLE: 0x" << std::hex << scriptfile->GetExportsOffset() << std::endl;
            return -1;
        }

        for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
            void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset() + i * exp->SizeOf());
            exp->SetHandle(handle);

            // if we aren't dumping the ASM, we compute all the nodes first
            std::ostream& output{ opt.m_dasm ? asmout : nullstream };

            if (exp->GetNamespace() != currentNSP) {
                currentNSP = exp->GetNamespace();

                if (opt.m_dasm) {
                    output << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                }
            }
            else if (!currentNSP && !i) {
                if (opt.m_dasm) {
                    output << std::endl;
                }
            }

            NameLocated rname = { exp->GetNamespace(), exp->GetName() };

            if (exp->GetAddress() > scriptfile->GetFileSize()) {
                asmout << "// INVALID EXPORT ADDRESS: 0x" << std::hex << exp->GetAddress() << std::endl;
                continue;
            }

            auto r = ctx.contextes.try_emplace(rname, scriptfile->Ptr(exp->GetAddress()), *scriptfile, ctx, opt, currentNSP, *exp, handle, ctx.m_vmInfo->vmMagic, ctx.currentPlatform);

            if (!r.second) {
                asmout << "Duplicate node "
                    << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << std::flush << "::"
                    << hashutils::ExtractTmp("function", exp->GetName()) << std::endl;
                continue;
            }

            auto& asmctx = r.first->second;


            if (opt.m_debugHashes) {
                uint64_t name{ exp->GetName() };
                const char* namePtr{ hashutils::ExtractPtr(name) };
                if (namePtr) {
                    uint64_t hashScr{ ctx.m_vmInfo->HashField(namePtr) };

                    if (hashScr != name && gdctx.WarningType(GDGCW_BAD_HASH_FIELD)) {
                        LOG_WARNING("Invalid hash algorithm for extracted field 0x{:x} != 0x{:x} for {}", name, hashScr, namePtr);
                    }
                }
                uint64_t fileNameSpace{ exp->GetFileNamespace() };
                if (fileNameSpace) {

                    const char* fnsPtr{ hashutils::ExtractPtr(fileNameSpace) };
                    if (fnsPtr) {
                        uint64_t hashFSScr{ ctx.m_vmInfo->HashFilePath(fnsPtr) };

                        if (hashFSScr != fileNameSpace && gdctx.WarningType(GDGCW_BAD_HASH_FILE)) {
                            LOG_WARNING("Invalid hash algorithm for extracted field 0x{:x} != 0x{:x} for {}", fileNameSpace, hashFSScr, fnsPtr);
                        }
                    }
                }
            }

            DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);

            if (asmctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                output << std::endl;
            }
            else {
                output << " ";
            }
            output << "gscasm {" << std::endl;

            try {
                tool::gsc::DumpAsm(*exp, output, *scriptfile, ctx, asmctx);
            }
            catch (std::runtime_error& err) {
                output << "FAILURE, " << err.what() << std::endl;
                asmctx.DisableDecompiler(err.what());

                {
                    core::async::opt_lock_guard lg{ gdctx.asyncMtx };
                    gdctx.hardErrors++;
                    if (!(exportErrors++) || dumpAllErrors) {
                        LOG_ERROR("Can't decompile export: {}", err.what());
                    }
                }
            }


            output << "}" << std::endl;

            if (asmctx.m_disableDecompiler) {
                if (opt.m_dasm || opt.m_func_header_post) {
                    DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);
                    output << ";" << std::endl;
                }

                if (opt.m_dasm) {
                    output << "// Can't decompile export " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << asmctx.m_disableDecompilerError << std::endl;
                }
                else if (!opt.m_dcomp) {
                    LOG_WARNING("Can't decompile export {}::{}", hashutils::ExtractTmpPath("namespace", exp->GetNamespace()), hashutils::ExtractTmp("function", exp->GetName()));
                }
            }


            if ((!opt.m_dasm || opt.m_dcomp || opt.m_func_header_post) && !asmctx.m_disableDecompiler) {
                asmctx.ComputeDefaultParamValue();

                if (opt.m_dasm || opt.m_func_header_post) {
                    DumpFunctionHeader(*exp, output, *scriptfile, ctx, asmctx, 0, nullptr, &currentAnimTree);
                }
                output << std::flush;
                DecompContext dctx{ 0, 0, asmctx.m_opt, 0, exp->GetAddress(), ctx.gdbData, ctx.lineBuf };
                if (opt.m_dcomp) {
                    if (!opt.m_vtable && scriptfile->IsVTableImportFlags(exp->GetFlags())) {
                        asmctx.m_bcl = scriptfile->Ptr(exp->GetAddress());
                        int ret{ DumpVTable(*exp, output, *scriptfile, ctx, asmctx, dctx) };
                        asmctx.m_vtable = ret != DVA_NOT;
                        if (ret == DVA_BAD) {
                            output << "// Can't decompile vtable " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << ret << std::endl;
                        }
                    }

                    if (!asmctx.m_vtable) {
                        asmctx.ComputePreSpecialPattern();
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_DEV)) {
                            asmctx.ComputeDevBlocks();

                            if (//(scriptfile->RemapFlagsExport(exp->GetFlags()) & T8GSCExportFlags::PRIVATE) != 0 &&
                                SizeNoEmptyNode(asmctx.m_funcBlock.m_statements) == 2) {
                                auto* node = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 0);
                                auto* ret = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 1);
                                if (ret->node && ret->node->m_type == TYPE_END
                                    && node->node && node->node->m_type == TYPE_BLOCK
                                        && static_cast<ASMContextNodeBlock*>(node->node)->m_blockType == BLOCK_DEV) {
                                    asmctx.m_devFuncCandidate = true;
                                }
                            }
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_SWITCH)) {
                            asmctx.ComputeSwitchBlocks();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOREACH)) {
                            asmctx.ComputeForEachBlocks();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_WHILE)) {
                            asmctx.ComputeWhileBlocks();
                        }
                        asmctx.ComputeSpecialPattern();
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_FOR)) {
                            asmctx.ComputeForBlocks();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_IF)) {
                            asmctx.ComputeIfBlocks();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_RETURN)) {
                            asmctx.ComputeReturnJump();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_BOOL_RETURN)) {
                            asmctx.ComputeBoolReturn();
                        }
                        if (!(asmctx.m_opt.m_stepskip & STEPSKIP_COMPILER_PATTERNS)) {
                            asmctx.ComputeCustomCompilerPattern();
                        }
                        if (opt.m_dasm) {
                            if (asmctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                                output << std::endl;
                            }
                            else {
                                output << " ";
                            }
                            asmctx.Dump(output, dctx);
                        }
                    }
                }
                else {
                    output << ";" << std::endl;
                }
            }
            output << std::endl;
        }

        if (!opt.m_dasm && opt.m_dcomp) {
            // current namespace
            currentNSP = 0;
            currentAnimTree = nullptr;
            int currentPadding{};
            bool inDevBlock{};

            uint64_t constructorName{ ctx.m_vmInfo->HashField("__constructor") };
            uint64_t destructorName{ ctx.m_vmInfo->HashField("__destructor") };

            
            auto handleClsAnimTree = [&ctx](uint64_t name, uint64_t method, gscclass& cls) -> bool {
                NameLocated lname{ name, method };

                auto masmctxit = ctx.contextes.find(lname);

                if (masmctxit == ctx.contextes.end()) {
                    return true;
                }

                const char* atr{ masmctxit->second.useAnimTree };

                if (!atr) {
                    return true; // nothing to check
                }

                if (!cls.animtree) {
                    cls.animtree = atr;
                    cls.hasAnimTrees = true;
                    return true; // default
                }

                if (strcmp(cls.animtree, atr)) {
                    cls.animtree = nullptr;
                    return false; // no common atr, we can't put it at the top level
                }

                return true;
            };

            for (auto& [name, cls] : ctx.m_classes) {
                if (!handleClsAnimTree(name, constructorName, cls)) continue;
                if (!handleClsAnimTree(name, destructorName, cls)) continue;
                for (const auto& method : cls.m_methods) {
                    if (method == constructorName || method == destructorName) continue; // ignore const/destr
                    if (!handleClsAnimTree(name, method, cls)) break;
                }
            }

            for (const auto& [name, cls] : ctx.m_classes) {
                if (cls.name_space != currentNSP) {
                    currentNSP = cls.name_space;

                    asmout << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                }

                if (cls.animtree && (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ANIM_REAL))) {
                    if (!currentAnimTree || std::strcmp(currentAnimTree, cls.animtree)) {
                        currentAnimTree = cls.animtree; // new name
                        // write animtree

                        asmout << "#using_animtree(";

                        if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                            asmout << " ";
                        }

                        asmout << '"' << utils::FormattedString(cls.animtree) << '"';

                        if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                            asmout << " ";
                        }

                        asmout << ");" << std::endl << std::endl;
                    }
                }

                asmout << "// Namespace " << hashutils::ExtractTmpPath("namespace", cls.name_space) << std::endl;
                asmout << "// Method(s) " << std::dec << cls.m_methods.size() << " Total "  << cls.m_vtable.size() << std::endl;
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

                if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                    asmout << std::endl;
                }
                else {
                    asmout << " ";
                }
                asmout << "{" << std::endl << std::endl;

                

                auto handleMethod = [&currentPadding, &opt, &asmout, &scriptfile, name, &ctx, &currentAnimTree](uint64_t method, const char* forceName, bool ignoreEmpty, bool specialMember) -> void {
                    auto lname = NameLocated{ name, method };

                    auto masmctxit = ctx.contextes.find(lname);

                    if (masmctxit == ctx.contextes.end()) {
                        return;
                    }

                    auto& e = masmctxit->second;

                    if (specialMember) e.noFunctionPrefix = true;

                    // set the export handle
                    e.m_exp.SetHandle(e.m_readerHandle);
                    uint32_t floc = e.m_exp.GetAddress();

                    if (e.m_disableDecompiler) {
                        DumpFunctionHeader(e.m_exp, asmout, *scriptfile, ctx, e, 1, forceName, &currentAnimTree);
                        asmout << ";" << std::endl;
                        return;
                    }

                    if (!ignoreEmpty || SizeNoEmptyNode(e.m_funcBlock.m_statements) > 1) {
                        // ignore empty exports (constructor/destructors)
                        

                        DumpFunctionHeader(e.m_exp, asmout, *scriptfile, ctx, e, 1, forceName, &currentAnimTree);
                        DecompContext dctx{ 0, 0, e.m_opt, currentPadding + 1, floc, ctx.gdbData, ctx.lineBuf };
                        if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                            dctx.WritePadding(asmout << std::endl);
                        }
                        else {
                            asmout << " ";
                        }
                        e.Dump(asmout, dctx);
                        asmout << std::endl;
                    }

                    ctx.contextes.erase(masmctxit);
                };

                std::unordered_set<uint64_t> selfmembers{};

                for (const auto& method : cls.m_methods) {
                    auto lname = NameLocated{ name, method };

                    auto masmctxit = ctx.contextes.find(lname);

                    if (masmctxit == ctx.contextes.end()) {
                        LOG_WARNING("Can't find {}", hashutils::ExtractTmp("function", method));
                        continue;
                    }
                    masmctxit->second.ConvertToClassMethod(selfmembers);
                }

                if (selfmembers.size()) {
                    // sort members using string lookup value for a better rendering
                    std::set<std::string> selfMembersSorted{};
                    for (uint64_t field : selfmembers) {
                        selfMembersSorted.insert(hashutils::ExtractTmp("var", field));
                    }

                    for (const std::string& field : selfMembersSorted) {
                        utils::Padding(asmout, 1) << "var " << field << ";" << std::endl;
                    }

                    asmout << std::endl;
                }

                // handle first the constructor/destructor
                handleMethod(constructorName, "constructor", true, true);
                handleMethod(destructorName, "destructor", true, true);

                for (const auto& method : cls.m_methods) {
                    handleMethod(method, nullptr, false, false);
                }

                if (cls.hasAnimTrees && !cls.animtree) {
                    // we need to cleanup the animtree using because it is not scoped anymore
                    currentAnimTree = nullptr;
                }

                asmout << "}" << std::endl << std::endl;
            }

            // compute dev function
            for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
                exp->SetHandle(handle);

                NameLocated lname{ exp->GetNamespace(), exp->GetName() };

                auto f = ctx.contextes.find(lname);

                if (f == ctx.contextes.end()) {
                    continue; // already parsed
                }

                auto& asmctx = f->second;

                if (asmctx.m_disableDecompiler || asmctx.m_vtable) {
                    continue;
                }

                asmctx.ForSubNodes([&ctx, &lname](ASMContextNode*& node, SubNodeContext& sctx) {
                    if (!node) {
                        return;
                    }
                    ASMContextNodeFuncRef* funcRef;
                    if (node->m_type == TYPE_FUNC_CALL) {
                        auto* callFunc = dynamic_cast<ASMContextNodeCallFuncPtr*>(node);

                        if (!callFunc->m_operands.size() || callFunc->m_operands[0]->m_type != TYPE_FUNC_REFNAME) {
                            return; // pointer call
                        }

                        funcRef = dynamic_cast<ASMContextNodeFuncRef*>(callFunc->m_operands[0]);
                    }
                    else if (node->m_type == TYPE_FUNC_REFNAME) {
                        funcRef = dynamic_cast<ASMContextNodeFuncRef*>(node);
                    }
                    else {
                        return;
                    }
                    
                    if (funcRef->m_script) {
                        return; // script call
                    }

                    NameLocated lnames{ funcRef->m_nsp ? funcRef->m_nsp : lname.name_space, funcRef->m_func };

                    auto f = ctx.contextes.find(lnames);

                    if (f == ctx.contextes.end()) {
                        return; // already parsed or builtin?
                    }

                    if (!sctx.devBlockDepth) {
                        f->second.m_devFuncCandidate = false; // called outside a dev block, can't be a canditate
                    }
                });
            }


            for (size_t i = 0; i < scriptfile->GetExportsCount(); i++) {
                void* handle = scriptfile->Ptr(scriptfile->GetExportsOffset()) + i * exp->SizeOf();
                exp->SetHandle(handle);

                NameLocated lname{ exp->GetNamespace(), exp->GetName() };

                auto f = ctx.contextes.find(lname);

                if (f == ctx.contextes.end()) {
                    continue; // already parsed
                }
                auto& asmctx = f->second;

                if (asmctx.m_vtable) {
                    continue;
                }

                if (exp->GetNamespace() != currentNSP) {
                    currentNSP = exp->GetNamespace();

                    asmout << "#namespace " << hashutils::ExtractTmpPath("namespace", currentNSP) << ";" << std::endl << std::endl;
                }
                else if (!currentNSP && !i) {
                    asmout << std::endl;
                }



                if (asmctx.m_devFuncCandidate) {
                    auto* dvb = GetNoEmptyNode(asmctx.m_funcBlock.m_statements, 0);

                    if (dvb->node && dvb->node->m_type == TYPE_BLOCK) {
                        auto* dvbn = dynamic_cast<ASMContextNodeBlock*>(dvb->node);
                        dvbn->m_disabled = true; // disable the dev block 
                    }

                    if (!inDevBlock) {
                        inDevBlock = true;
                        utils::Padding(asmout, currentPadding) << "/#" << std::endl << std::endl;
                        currentPadding++;
                    }

                    {
                        core::async::opt_lock_guard lg{ ctx.gdctx.asyncMtx };

                        asmctx.m_exp.SetHandle(asmctx.m_readerHandle);
                        NameLocated loc{};
                        loc.name = asmctx.m_exp.GetName();
                        loc.name_space = asmctx.m_namespace;

                        ctx.gdctx.exportInfos[loc].devFunc = true;
                    }
                }
                else {
                    if (inDevBlock) {
                        currentPadding--;
                        utils::Padding(asmout, currentPadding) << "#/" << std::endl << std::endl;
                        inDevBlock = false;
                    }
                }
                DecompContext dctx{ 0, 0, asmctx.m_opt, currentPadding, exp->GetAddress(), ctx.gdbData, ctx.lineBuf };

                if (asmctx.m_disableDecompiler) {
                    DumpFunctionHeader(*exp, asmout, *scriptfile, ctx, asmctx, currentPadding, nullptr, &currentAnimTree);
                    if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                        dctx.WritePadding(asmout << std::endl);
                    }
                    else {
                        asmout << " ";
                    }
                    asmout << "{" << std::endl;
                    dctx.padding++;
                    dctx.WritePadding(asmout) << "// Can't decompile export " << hashutils::ExtractTmpPath("namespace", exp->GetNamespace()) << "::" << hashutils::ExtractTmp("function", exp->GetName()) << " " << asmctx.m_disableDecompilerError << std::endl;
                    dctx.padding--;
                    asmout << "}" << std::endl << std::endl;
                    continue;
                }

                DumpFunctionHeader(*exp, asmout, *scriptfile, ctx, asmctx, currentPadding, nullptr, &currentAnimTree);
                if (opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
                    dctx.WritePadding(asmout << std::endl);
                }
                else {
                    asmout << " ";
                }
                asmctx.Dump(asmout, dctx);
                asmout << std::endl;
            }
            if (inDevBlock) {
                currentPadding--;
                utils::Padding(asmout, currentPadding) << "#/" << std::endl;
                inDevBlock = false;
            }
        }
    }

    if (opt.m_generateGdbData && (opt.m_generateGdbBaseData || (ctx.m_unkstrings.size() || ctx.m_devblocks.size() || ctx.m_lazyLinks.size()))) {

        char asmfnamebuffgdb[1000];
        if (opt.m_dbgOutputDir) {
            const char* name = opt.m_noPath ? nullptr : hashutils::ExtractPtr(scriptfile->GetName());

            const char* outDir;
            if (opt.m_splitByVm) {
                outDir = utils::va("%s/%s", opt.m_dbgOutputDir, tool::gsc::opcode::VMIdFancyName(ctx.m_vmInfo->vmMagic));
            }
            else {
                outDir = opt.m_dbgOutputDir;
            }

            if (!name) {
                const char* fileExt{ typeSure && isCsc ? "csc" : "gsc" };
                if (actscli::options().heavyHashes) {
                    sprintf_s(asmfnamebuffgdb, "%s/%016llX.%s", outDir, scriptfile->GetName(), fileExt);
                }
                else {
                    sprintf_s(asmfnamebuffgdb, "%s/hashed/script/script_%llx.%s", outDir, scriptfile->GetName(), fileExt);
                }
            }
            else {
                sprintf_s(asmfnamebuffgdb, "%s/%s", outDir, name);
            }
        }
        else {
            sprintf_s(asmfnamebuffgdb, "%sasm", path);
        }

        std::filesystem::path filedbg(asmfnamebuffgdb);

        std::filesystem::create_directories(filedbg.parent_path());

        const char* gdbFile{ utils::va("%s.gdb", asmfnamebuffgdb) };
        std::ofstream gdbpos{ gdbFile };
        LOG_INFO("Writing GDB into '{}'...", gdbFile);

        if (!gdbpos) {
            LOG_ERROR("Can't open {}", gdbFile);
            return -1;
        }

        // header
        if (opt.m_copyright) {
            gdbpos << "# " << opt.m_copyright << std::endl;
        }

        if (opt.m_header) {
            gdbpos << "# file " << asmfnamebuff << std::endl;
        }

        gdbpos
            << "NAME " << hashutils::ExtractTmpScript(scriptfile->GetName()) << std::endl
            << "VERSION 0" << std::endl
            << "CHECKSUM 0x" << std::hex << scriptfile->GetChecksum() << std::endl
            ;

        // strings
        if (ctx.m_unkstrings.size()) {
            gdbpos
                << "######################################################" << std::endl
                << "####################  DEV STRINGS  ###################" << std::endl
                << "######################################################" << std::endl
                ;
            for (auto& [val, flocs] : ctx.m_unkstrings) {
                gdbpos << "#";

                for (const uint32_t floc : flocs) {
                    gdbpos << " " << ctx.GetFLocName(floc);
                }

                gdbpos
                    << std::endl
                    << "STRING \"";

                utils::PrintFormattedString(gdbpos, val.c_str())
                    << "\""
                    << std::hex
                    ;

                for (const uint32_t floc : flocs) {
                    gdbpos << " 0x" << floc;
                }
                gdbpos << std::endl;
            }
        }
        if (ctx.m_devblocks.size()) {
            gdbpos
                << "######################################################" << std::endl
                << "####################  DEV BLOCKS  ####################" << std::endl
                << "######################################################" << std::endl
                ;
            for (auto& floc : ctx.m_devblocks) {
                gdbpos
                    << "# " << ctx.GetFLocName(floc) << std::endl
                    << "DEVBLOCK 0x" << std::hex << floc << std::endl;
            }
        }
        if (ctx.m_lazyLinks.size()) {
            gdbpos
                << "######################################################" << std::endl
                << "####################  LAZY LINKS  ####################" << std::endl
                << "######################################################" << std::endl
                ;
            for (auto& [val, flocs] : ctx.m_lazyLinks) {
                gdbpos << "#";

                for (const uint32_t floc : flocs) {
                    gdbpos << " " << ctx.GetFLocName(floc);
                }

                gdbpos
                    << std::endl
                    << "LAZYLINK"
                    " \"" << hashutils::ExtractTmp("namespace", val.name_space) << "\""
                    " \"" << hashutils::ExtractTmp("script", val.script) << "\""
                    " \"" << hashutils::ExtractTmp("function", val.name) << "\""
                    << std::hex
                    ;

                for (const uint32_t floc : flocs) {
                    gdbpos << " 0x" << floc;
                }
                gdbpos << std::endl;
            }
        }


        gdbpos.close();
    }

    {
        core::async::opt_lock_guard lg{ gdctx.asyncMtx };

        if (opt.vtable_dump) {
            for (auto& [n, c] : ctx.m_classes) {
                auto& t = gdctx.vtables[scriptfile->GetName()][n];
                for (auto& meth : c.m_vtableMethods) {
                    t.insert(NameLocated{ meth.name_space , meth.name });
                }
            }

        }
        gdctx.decompiledFiles++;
        if (exportErrors) {
            LOG_ERROR("Found {} error(s)", exportErrors);
        }
    }

    return 0;
}

int DumpInfoFileData(tool::gsc::T8GSCOBJ* data, size_t size, const char* path, std::unordered_map<uint64_t, const char*>& dataset) {
    // name
    dataset.insert({ data->name, "script" });


    uint64_t* includes = reinterpret_cast<uint64_t*>(&data->magic[data->include_offset]);
    for (size_t i = 0; i < data->include_count; i++) {
        dataset.insert({ includes[i], "script" });
    }

    uintptr_t gvars_location = reinterpret_cast<uintptr_t>(data->magic) + data->globalvar_offset;
    for (size_t i = 0; i < data->globalvar_count; i++) {
        const auto* globalvar = reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(gvars_location);
        dataset.insert({ globalvar->name, "var" });

        gvars_location += sizeof(*globalvar) + sizeof(uint32_t) * globalvar->num_address;
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

int DumpInfoFile(const std::filesystem::path& path, std::unordered_map<uint64_t, const char*>& dataset) {
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

    LOG_DEBUG("Reading {}", pathname);

    void* buffer{};
    size_t size;
    void* bufferNoAlign{};
    size_t sizeNoAlign;
    if (!utils::ReadFileAlign(path, bufferNoAlign, buffer, sizeNoAlign, size)) {
        LOG_ERROR("Can't read file data for {}", path.string());
        return -1;
    }

    if (size < sizeof(tool::gsc::T8GSCOBJ)) {
        LOG_ERROR("Bad header, file size: {:x}/{:x} for {}", size, sizeof(tool::gsc::T8GSCOBJ), path.string());
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
    const char* inputFile = "scriptparsetree";
    const char* outputFile = "dataset.csv";
    if (argc > 2) {
        inputFile = argv[2];
        if (argc > 3) {
            inputFile = argv[3];
        }
    }

    std::unordered_map<uint64_t, const char*> dataset{};

    int ret = DumpInfoFile(inputFile, dataset);
    if (ret) {
        return ret;
    }

    std::ofstream out{ outputFile };
    if (!out) {
        LOG_ERROR("Can't open output file");
    }

    out << "type,name\n";

    for (const auto& [hash, type] : dataset) {
        out << std::hex << type << "," << hashutils::ExtractTmp(type, hash) << std::endl;
    }
    out.close();
    return 0;
}

tool::gsc::T8GSCOBJContext::T8GSCOBJContext(GscDecompilerGlobalContext& gdctx) : opt(gdctx.opt), gdctx(gdctx) {}

tool::gsc::T8GSCOBJContext::~T8GSCOBJContext() {
    for (auto& [k, string] : m_allocatedStrings) {
        delete[] string;
    }
}

// apply ~ to ref to avoid using 0, 1, 2 which might already be used

uint64_t tool::gsc::T8GSCOBJContext::GetGlobalVarName(uint16_t gvarRef) {
    auto f = m_gvars.find(gvarRef);
    if (f == m_gvars.end()) {
        return 0;
    }
    return f->second;
}

const char* tool::gsc::T8GSCOBJContext::GetStringValue(uint32_t stringRef) {
    auto f = m_stringRefs.find(stringRef);
    if (f == m_stringRefs.end()) {
        return nullptr;
    }
    return f->second;
}
void tool::gsc::T8GSCOBJContext::AddStringRef(uint32_t floc, uint32_t str) {
    m_stringRefsLoc[floc] = str;
}

const char* tool::gsc::T8GSCOBJContext::GetStringValueByLoc(uint32_t floc) {
    auto f = m_stringRefsLoc.find(floc);
    if (f == m_stringRefsLoc.end()) {
        return nullptr;
    }
    return GetStringValue(f->second);
}

const char* tool::gsc::T8GSCOBJContext::GetTokenValue(uint32_t tokenRef) {
    if (tokenRef >= m_tokens.size()) {
        return CloneString(utils::va("<badtoken:%x>", tokenRef));
    }
    GSCOBJTokenData& td{ m_tokens[tokenRef] };
    if (td.isString) return td.val.str;
    const char* extracted{ tool::gsc::iw::GetOpaqueStringForVm(m_vmInfo->vmMagic, td.val.id, false) };
    if (extracted) return extracted;
    return CloneString(utils::va("ref_%x", td.val.id));
}

const char* tool::gsc::T8GSCOBJContext::GetStringValueOrError(uint32_t stringRef, uint32_t floc, const char* errorValue) {
    const char* v = GetStringValue(stringRef);

    if (v) {
        return v;
    }
    return GetStringError(floc, errorValue);
}

const char* tool::gsc::T8GSCOBJContext::GetStringError(uint32_t floc, const char* errorValue) {
    if (errorValue) {
        m_unkstrings[errorValue].insert(floc);
    }

    return errorValue;
}

uint16_t tool::gsc::T8GSCOBJContext::AddGlobalVarName(uint64_t value) {
    uint16_t id = ((uint16_t)m_gvars.size());
    m_gvars[id] = value;
    return id;
}

uint32_t tool::gsc::T8GSCOBJContext::AddStringValue(const char* value) {
    uint32_t id = ((uint32_t)m_stringRefs.size());
    m_stringRefs[id] = value;
    return id;
}
char* tool::gsc::T8GSCOBJContext::CloneString(const char* str) {
    char*& r{ m_allocatedStrings[hash::Hash64(str)] };
    if (r) {
        return r;
    }
    size_t len = std::strlen(str);
    char* ptr = new char[len + 1];

    memcpy(ptr, str, len + 1);

    r = ptr;

    return ptr;
}

int tool::gsc::DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx) {
    uint32_t baseloc{ exp.GetAddress() };
    uint64_t filename{ ctx.m_gscReader.GetName() };
    // main reading loop
    while (ctx.FindNextLocation()) {
        while (true) {
            if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN) && objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                ctx.Aligned<uint16_t>();
            }
            uint32_t floc = ctx.ScriptAbsoluteLocation();
            if (ctx.m_bcl < gscFile.Ptr() || floc >= gscFile.GetFileSize()) {
                throw std::runtime_error(std::format("FIND location after file 0x{:x}", floc));
            }
            byte*& base = ctx.m_bcl;

            // mark the current location as handled
            auto& loc = ctx.PushLocation();
            loc.handled = true;

            if (ctx.m_lastOpCodeBase == -1) {
                ctx.m_lastOpCodeBase = loc.rloc;
            }
            // print the stack and the fields
            auto printStack = [&ctx, &out, &loc](const char* type) {
                if (ctx.m_opt.m_dcomp && ctx.m_opt.m_display_stack) {
                    out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ":"
                        << std::setfill(' ') << std::setw(6) << std::left << " " << std::right
                        << std::setfill(' ') << std::setw(26) << std::left << type << std::right
                        << "stack(" << std::dec << ctx.m_stack.size() << "): "
                        << std::flush;

                    for (const auto* node : ctx.m_stack) {
                        out << "<" << *node << "> ";
                    }
                    out << std::endl;
                    out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ":"
                        << std::setfill(' ') << std::setw(32) << std::left << " " << std::right
                        << "fieldid: <";
                    if (ctx.m_fieldId) {
                        out << *ctx.m_fieldId;
                    }
                    else {
                        out << "none";
                    }
                    out << "> objectid: <";
                    if (ctx.m_objectId) {
                        out << *ctx.m_objectId;
                    }
                    else {
                        out << "none";
                    }
                    out << ">"
                        << std::flush;

                    out << std::endl;
                }

            };

            // compute the late operations (OR|AND)
            for (const auto& lateop : loc.m_lateop) {
                lateop->Run(ctx, objctx);
                printStack(lateop->type);
            }

            uint16_t opCode;

            if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                opCode = ctx.Read<uint16_t>(base);
            }
            else {
                opCode = (uint16_t)ctx.Read<byte>(base);
            }

            //if (objctx.m_vmInfo->modToolFlag && (opCode & objctx.m_vmInfo->modToolFlag)) {
            //    // modtool
            //}
            
            const auto* handler = ctx.LookupOpCode(opCode);

            if (ctx.m_opt.m_func_floc) {
                out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << (baseloc + loc.rloc);
            }

            if (ctx.m_opt.m_show_opcode_values) {
                out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << loc.rloc << ": " << std::flush;
            }

            const char* opcodeName{};
            if (ctx.m_opt.m_use_internal_names) {
                opcodeName = opcode::OpCodeName(handler->m_id);
            }
            else {
                opcodeName = handler->m_name;
            }
            if (!opcodeName) {
                opcodeName = utils::va("NO_NAME_%d", handler->m_id);
            }

            if (opCode > objctx.m_vmInfo->maxOpCode) {
                throw std::runtime_error(std::format("FIND errec {} (0x{:x} > 0x{:x})", opcodeName, opCode, objctx.m_vmInfo->maxOpCode));
                opCode &= objctx.m_vmInfo->maxOpCode;
                break;
            }

            if (ctx.m_opt.m_show_opcode_values) {
                out << std::hex << std::setfill('0') << std::setw(sizeof(int16_t) << 1) << opCode
                    << " ";
            }

            out << std::setfill(' ') << std::setw(25) << std::left
                << opcodeName
                << std::right << " " << std::flush;

            // dump rosetta data
            uint32_t opcodeRloc{ (uint32_t)(reinterpret_cast<uint64_t>(base) - reinterpret_cast<uint64_t>(gscFile.Ptr())) };
            if (ctx.m_objctx.gdctx.opcodesLocs) {
                (*ctx.m_objctx.gdctx.opcodesLocs)[filename].insert(opcodeRloc);
            }
            RosettaAddOpCode(opcodeRloc, handler->m_id);

            // pass the opcode

            byte* opBase{ ctx.m_bcl };

            if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                base += 2;
            }
            else {
                base++;
            }

            // update ASMContext::WritePadding if you change the format

            int ret = handler->Dump(out, opCode, ctx, objctx);

            if (ctx.m_opt.m_dataDump) {
                if (ctx.m_bcl >= opBase) {
                    size_t l{ (size_t)(ctx.m_bcl - opBase) };
                    out << "// data (0x" << std::hex << l << ")";
                    for (size_t i = 0; i < l; i++) {
                        out << " " << std::setfill('0') << std::setw(2) << (int)opBase[i];
                    }

                    out << std::endl;
                }
                else {
                    out << "// ERROR: NEW BASE AFTER OLD BASE" << std::endl;
                }
            }

            if (ret) {
                break;
            }

            printStack("endop");
        }
    }
    // no more location, we can assume the final size
    // maybe we don't end on a return/end, to check?
    out << "// final size: 0x" << std::hex << ctx.FinalSize() << std::endl;

    return 0;
}

int tool::gsc::DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx, DecompContext& dctxt) {
    using namespace tool::gsc::opcode;
    auto FetchCode = [&ctx, &dctxt, &out]() -> const OPCodeInfo* {
        dctxt.rloc = ctx.FunctionRelativeLocation();
        uint16_t code;
        if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
            if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
                ctx.Aligned<uint16_t>();
            }
            code = *(uint16_t*)ctx.m_bcl;
            ctx.m_bcl += 2;
        } else {
            code = (uint16_t)*ctx.m_bcl;
            ctx.m_bcl += 1;
        }
        const OPCodeInfo* ret{ ctx.LookupOpCode(code) };

        if (!ret || ret->m_id == OPCODE_Undefined) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << code << std::endl;
        }

        return ret;
    };

    // main reading loop
    const OPCodeInfo* ccp = FetchCode();

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
    
    if (ccp->m_id != OPCODE_CheckClearParams) {
        // dctxt.WritePadding(out) << "Bad vtable opcode: expected CheckClearParams\n";
        return DVA_NOT;
    }

    const OPCodeInfo* preScriptCall = FetchCode();


    if (preScriptCall->m_id != OPCODE_PreScriptCall) {
        // dctxt.WritePadding(out) << "Bad vtable opcode: expected PreScriptCall\n";
        return DVA_NOT;
    }

    const OPCodeInfo* spawnStruct = FetchCode();
    
    if (spawnStruct->m_id != OPCODE_ScriptFunctionCall && spawnStruct->m_id != OPCODE_CallBuiltinFunction) {
        if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_CRC_DUMP)) {
            return DVA_OK; // crc dump
        }
        // dctxt.WritePadding(out) << "Bad vtable opcode, expected ScriptFunctionCall vm" << std::hex << (int)ctx.m_vm << std::endl;
        return DVA_NOT;
    }

    ctx.m_bcl += 1; // call params
    ctx.Aligned<uint64_t>() += 8; // assume that we have a spawnstruct

    auto AssertOpCode = [&FetchCode, &dctxt, &out](OPCode opc) -> bool {
        const OPCodeInfo* nfo{ FetchCode() };
        if (nfo->m_id != opc) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << nfo->m_name << ", expected " << OpCodeName(opc) << std::endl;
            out << "}" << std::endl;
            return false;
        }
        return true;
    };

    if (!AssertOpCode(OPCODE_GetZero)) return DVA_BAD;

    if (gscFile.GetMagic() > VMI_T8_36) {
        if (!AssertOpCode(OPCODE_T9_EvalFieldVariableFromGlobalObject)) return DVA_BAD;
        ctx.Aligned<uint16_t>() += 2; // - classes
    }
    else if (gscFile.GetMagic() < VMI_T8_34) {
        ctx.Aligned<uint16_t>() += 2; // GetClassesObject

        ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef className
    }
    else {
        ctx.Aligned<uint16_t>() += 2; // GetGlobalObject
        ctx.Aligned<uint16_t>() += 2; // - classes

        ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef
    }

    auto& clsName = ctx.Aligned<uint32_t>();

    uint32_t name = *(uint32_t*)clsName; // __vtable

    auto& cls = objctx.m_classes[name];
    cls.name_space = exp.GetNamespace();

    clsName += 4;
    if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START)) {
        out << std::endl;
    }
    else {
        out << " ";
    }
    out << "{" << std::endl;

    dctxt.WritePadding(out) << "// " << hashutils::ExtractTmp("class", name) << std::endl;

    if (gscFile.GetMagic() > VMI_T8_36) {
        if (!AssertOpCode(OPCODE_T9_SetVariableFieldFromEvalArrayRef)) return DVA_BAD;
    }
    else {
        ctx.Aligned<uint16_t>() += 2; // EvalArrayRef
        if (!AssertOpCode(OPCODE_SetVariableField)) return DVA_BAD;
    }


    while (true) {
        const OPCodeInfo* funcOpCode = FetchCode();

        if (!funcOpCode) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << funcOpCode->m_name << std::endl;
            out << "}" << std::endl;
            return DVA_BAD;
        }
        if (funcOpCode->m_id == OPCODE_End) {
            break; // end
        }
        if (funcOpCode->m_id != OPCODE_GetResolveFunction) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << funcOpCode->m_name << ", excepted GetResolveFunction or End" << std::endl;
            out << "}" << std::endl;
            return DVA_BAD;
        }

        auto& getFuncBase = ctx.Aligned<uint64_t>();
        uint32_t methodName = ((uint32_t*)getFuncBase)[0];
        uint32_t methodClsName = ((uint32_t*)getFuncBase)[1];
        getFuncBase += 8;

        const OPCodeInfo* uidCodeOpCode = FetchCode();

        if (!uidCodeOpCode) {
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter" << std::endl;
            out << "}" << std::endl;
            return DVA_BAD;
        }

        uint64_t uid;

        switch (uidCodeOpCode->m_id) {
        case OPCODE_GetZero: // int32_t
            uid = 0;
            break;
        case OPCODE_GetNegUnsignedInteger: // int32_t
            uid = -*(int32_t*)ctx.Aligned<int32_t>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetNegUnsignedShort: // uint16_t
            uid = -*(uint16_t*)ctx.Aligned<uint16_t>();
            ctx.m_bcl += 2;
            break;
        case OPCODE_GetNegByte: // uint8_t
            uid = -*(uint8_t*)ctx.Aligned<uint8_t>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetByte: // byte
            uid = *(byte*)ctx.Aligned<byte>();
            ctx.m_bcl++;
            break;
        case OPCODE_GetInteger: // int32_t
            uid = *(int32_t*)ctx.Aligned<int32_t>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetLongInteger: // int64_t
            uid = *(int64_t*)ctx.Aligned<int64_t>();
            ctx.m_bcl += 8;
            break;
        case OPCODE_GetUnsignedInteger: // uint32_t
            uid = *(uint32_t*)ctx.Aligned<uint32_t>();
            ctx.m_bcl += 4;
            break;
        case OPCODE_GetUnsignedShort: // uint16_t
            uid = *(uint16_t*)ctx.Aligned<uint16_t>();
            ctx.m_bcl += 2;
            break;
        default:
            dctxt.WritePadding(out) << "Bad vtable opcode: " << std::hex << uidCodeOpCode->m_name << ", excepted Getter" << std::endl;
            out << "}" << std::endl;
            return DVA_BAD;
        }

        if (methodClsName == name) {
            cls.m_methods.push_back(methodName);
        }
        else {
            cls.m_superClass.emplace(methodClsName);
        }
        cls.m_vtableMethods.emplace(NameLocated{ methodClsName, methodName });
        auto& mtd = cls.m_vtable[uid];
        mtd.name = methodName;
        mtd.nsp = methodClsName;
        dctxt.WritePadding(out) << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(uid)) << uid
            << " -> &" << hashutils::ExtractTmp("class", methodClsName) << std::flush 
            << "::" << hashutils::ExtractTmp("function", methodName) << ";" << std::endl;

        if (!AssertOpCode(OPCODE_GetZero)) return DVA_BAD;

        if (gscFile.GetMagic() >= VMI_T8_34) {
            ctx.Aligned<uint16_t>() += 2; // EvalGlobalObjectFieldVariable
            ctx.Aligned<uint16_t>() += 2; // - gvar
        }
        else {
            ctx.Aligned<uint16_t>() += 2; // GetClassesObject
            ctx.Aligned<uint16_t>() += 2; // EvalFieldVariable
        }
        ctx.Aligned<uint32_t>() += 4; // - ref
        ctx.Aligned<uint16_t>() += 2; // EvalArray
        ctx.Aligned<uint16_t>() += 2; // CastFieldObject
        ctx.Aligned<uint16_t>() += 2; // EvalFieldVariableRef
        ctx.Aligned<uint32_t>() += 4; // - ref

        if (gscFile.GetMagic() > VMI_T8_36) {
            ctx.Aligned<uint16_t>() += 2; // SetVariableFieldFromEvalArrayRef
        }
        else {
            ctx.Aligned<uint16_t>() += 2; // EvalArrayRef
            ctx.Aligned<uint16_t>() += 2; // SetVariableField
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
    out << std::endl;
    out << "}" << std::endl;
    return DVA_OK;
}

int tool::gsc::ComputeSize(GSCExportReader& exp, byte* gscFile, Platform plt, VmInfo* vminfo) {
    using namespace opcode;
    byte* loc = gscFile + exp.GetAddress();

    ASMSkipContext ctx{ loc, plt, vminfo };

    while (ctx.FindNextLocation()) {
        while (true) {
            // align to next opcode
            auto& base = ctx.Aligned<uint16_t>();

            // mark the current location as handled
            auto& loc = ctx.PushLocation();
            loc.handled = true;

            uint16_t opCode = *(uint16_t*)base;

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

std::unique_ptr<GSCExportReader> tool::gsc::CreateExportReader(VmInfo* vmInfo) {
    if (vmInfo->HasFlag(VmFlags::VMF_HASH_ACTS)) {
        return std::make_unique<ACTSExportReader>();
    }
    if (vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
        return std::make_unique<BINGSCExportReader>();
    }
    if (vmInfo->HasFlag(VmFlags::VMF_HASH64 | VmFlags::VMF_EXPORT_NOCHECKSUM)) {
        return std::make_unique<H64CERGSCExportReader>();
    }
    else if (vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
        if (vmInfo->HasFlag(VmFlags::VMF_EXPORT_CRC32)) {
            return std::make_unique<H64CER2GSCExportReader>();
        }
        else {
            return std::make_unique<H64GSCExportReader>();
        }
    }
    else if (vmInfo->flags & VmFlags::VMF_NO_FILE_NAMESPACE) {
        return std::make_unique<H32T7GSCExportReader>();
    }
    else {
        return std::make_unique<H32GSCExportReader>();
    }
}

void tool::gsc::DumpFunctionHeader(GSCExportReader& exp, std::ostream& asmout, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, ASMContext& ctx, int padding, const char* forceName, const char** currentAnimTree) {
    auto remapedFlags = gscFile.RemapFlagsExport(exp.GetFlags());
    bool classMember = remapedFlags & (T8GSCExportFlags::CLASS_MEMBER | T8GSCExportFlags::CLASS_DESTRUCTOR);

    auto detourVal = objctx.m_gsicInfo.detours.find(exp.GetAddress());
    bool isDetour = detourVal != objctx.m_gsicInfo.detours.end();

    tool::gsc::formatter::FormatterFlags headerFormat{ ctx.m_opt.m_formatter->GetHeaderFormat() };

    if (ctx.useAnimTree && (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ANIM_REAL))) {
        if (!currentAnimTree || !*currentAnimTree || std::strcmp(*currentAnimTree, ctx.useAnimTree)) {
            if (currentAnimTree) *currentAnimTree = ctx.useAnimTree; // new name
            // write animtree

            utils::Padding(asmout, padding) << "#using_animtree(";

            if (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                asmout << " ";
            }

            asmout << '"' << utils::FormattedString(ctx.useAnimTree) << '"';

            if (objctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
                asmout << " ";
            }

            asmout << ");" << std::endl << std::endl;
        }
    }

    if (ctx.m_opt.m_func_header && headerFormat != tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_NONE) {
        const char* prefix;
        if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ONE_LINE_HEADER_COMMENTS)) {
            utils::Padding(asmout, padding) << "/*" << std::endl;
            padding++;
            prefix = "";
        }
        else {
            prefix = "// ";
        }

        switch (headerFormat) {
        case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_SERIOUS: {
            utils::Padding(asmout, padding) << prefix << "Name: " << hashutils::ExtractTmp("function", exp.GetName()) << std::endl;
            if (exp.GetNamespace()) {
                utils::Padding(asmout, padding) << prefix << "Namespace: " << hashutils::ExtractTmp(classMember ? "class" : "namespace", exp.GetNamespace()) << std::endl;
            }
            // no file namespace in this format, maybe later?
            if (!ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_EXPORT_NOCHECKSUM)) {
                utils::Padding(asmout, padding) << prefix << "Checksum: 0x" << std::hex << std::uppercase << exp.GetChecksum() << std::endl;
            }
            if (!objctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                utils::Padding(asmout, padding) << prefix << "Offset: 0x" << std::hex << std::uppercase << exp.GetAddress() << std::endl;
            }

            uint32_t knownSize{ exp.GetSize() };
            if (knownSize) {
                utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << knownSize << std::endl;
            }
            else {
                UINT size = ctx.FinalSize();
                if (size > 1) { // at least one opcode
                    utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << size << std::endl;
                }
            }


            utils::Padding(asmout, padding) << prefix << "Parameters: " << std::dec << (int)exp.GetParamCount() << std::endl;

            utils::Padding(asmout, padding) << prefix << "Flags: ";

            if (!remapedFlags) {
                asmout << "None";
            }
            else if (gscFile.IsVTableImportFlags(exp.GetFlags())) {
                asmout << "VTable";
            }
            else {
                const struct {
                    T8GSCExportFlags flag;
                    const char* name;
                } knownFlags[]{
                    { T8GSCExportFlags::LINKED, "Linked" },
                    { T8GSCExportFlags::AUTOEXEC, "AutoExec" },
                    { T8GSCExportFlags::PRIVATE, "Private" },
                    { T8GSCExportFlags::EVENT, "Event" },
                    { T8GSCExportFlags::CLASS_LINKED, "Class Linked" },
                    { T8GSCExportFlags::VE, "Variadic" },
                };
                bool def{};
                for (const auto& kf : knownFlags) {
                    if ((remapedFlags & kf.flag) != kf.flag) {
                        continue;
                    }
                    if (def) {
                        asmout << ", ";
                    }
                    else {
                        def = true;
                    }
                    asmout << kf.name;
                }
            }
            asmout << std::nouppercase << std::endl;
            break;
        }
        case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_NONE:
            break;
        case tool::gsc::formatter::FFL_FUNC_HEADER_FORMAT_TYPE_4:
            utils::Padding(asmout, padding) << prefix << "Header format type 4 not implemented!";
            break;
        default: { // ACTS DEFAULT FORMAT
            if (exp.GetNamespace()) {
                utils::Padding(asmout, padding) << prefix << "Namespace "
                    << hashutils::ExtractTmp(classMember ? "class" : "namespace", exp.GetNamespace()) << std::flush;

                uint64_t fileNamespace = exp.GetFileNamespace();

                if (fileNamespace && !objctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_FILE_NAMESPACE)) {
                    // some VMs are only using the filename in the second namespace field, the others are using the full name (without .gsc?)
                    // so it's better to use spaces. A flag was added to keep the same format.
                    if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_FULL_FILE_NAMESPACE)) {
                        asmout << " / ";
                    }
                    else {
                        asmout << "/";
                    }

                    asmout
                        << ((remapedFlags & T8GSCExportFlags::EVENT)
                            ? hashutils::ExtractTmp("event", fileNamespace)
                            : hashutils::ExtractTmpPath("namespace", fileNamespace));
                }
                asmout << std::endl;
            }

            if (isDetour) {
                auto det = detourVal->second;
                utils::Padding(asmout, padding) << prefix
                    << "Detour " << hashutils::ExtractTmp("function", exp.GetName()) << " "
                    << "Offset 0x" << std::hex << det.fixupOffset << "/0x" << det.fixupSize
                    << std::endl
                    ;
            }

            utils::Padding(asmout, padding) << prefix << "Params " << std::dec << (int)exp.GetParamCount();
            
            if (remapedFlags) {
                asmout << ", eflags: 0x" << std::hex << (int)exp.GetFlags();

                if (remapedFlags & T8GSCExportFlags::LINKED) {
                    asmout << " linked";
                }
                if (remapedFlags & T8GSCExportFlags::CLASS_LINKED) {
                    asmout << " class_linked";
                }
                if (remapedFlags & T8GSCExportFlags::VE) {
                    asmout << " variadic";
                }
                if (gscFile.IsVTableImportFlags(exp.GetFlags())) {
                    asmout << " vtable";
                }
            }

            asmout << std::endl;
            if (ctx.m_opt.m_rawhash) {
                utils::Padding(asmout, padding) << prefix
                    << std::hex
                    << "namespace_" << exp.GetNamespace() << "<file_" << exp.GetFileNamespace() << ">::function_" << exp.GetName() << std::endl;
            }

            if (!objctx.m_vmInfo->HasFlag(VmFlags::VMF_GSCBIN)) {
                utils::Padding(asmout, padding) << prefix;
                if (!ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_EXPORT_NOCHECKSUM)) {
                    asmout << "Checksum 0x" << std::hex << exp.GetChecksum() << ", ";
                }
                asmout << "Offset: 0x" << std::hex << exp.GetAddress() << std::endl;
            }

            uint32_t knownSize{ exp.GetSize() };
            bool hasSize{ (bool)knownSize };
            if (knownSize) {
                utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << std::hex << knownSize;
            }
            else {
                auto size = ctx.FinalSize();
                if (size) { // at least one opcode
                    utils::Padding(asmout, padding) << prefix << std::hex << "Size: 0x" << size;
                    hasSize = true;
                }
            }

            if (ctx.m_devFuncCandidate || ctx.m_boolFuncCandidate) {

                if (hasSize) {
                    asmout << "," << " Type:";
                }
                else {
                    utils::Padding(asmout, padding) << prefix << "Type:";
                }

                if (ctx.m_devFuncCandidate) {
                    asmout << " dev";
                }
                if (ctx.m_boolFuncCandidate) {
                    asmout << " bool";
                }
                asmout << std::nouppercase;
            }
            if (hasSize) {
                asmout << std::endl;
            }
            break;
        }
        }
        if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_ONE_LINE_HEADER_COMMENTS)) {
            padding--;
            utils::Padding(asmout, padding) << "*/" << std::endl;
        }
    }

    bool specialClassMember = !ctx.m_opt.m_dasm && classMember &&
        ((remapedFlags & T8GSCExportFlags::CLASS_DESTRUCTOR) || ctx.m_objctx.m_vmInfo->HashField("__constructor") == exp.GetName());

    utils::Padding(asmout, padding);
    
    if (!specialClassMember && !ctx.noFunctionPrefix && !(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_FUNCTION_TITLE))) {
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

    if (isDetour) {
        auto& detour = detourVal->second;

        asmout << "detour ";
        if (detour.replaceNamespace) {
            asmout << hashutils::ExtractTmpPath("namespace", detour.replaceNamespace) << std::flush;
        }

        if (detour.replaceScript) {
            asmout << "<" << hashutils::ExtractTmpScript(detour.replaceScript) << ">" << std::flush;
        }

        if (detour.replaceNamespace) {
            asmout << "::";
        }

        asmout
            << hashutils::ExtractTmp("function", detour.replaceFunction) << std::flush;
    }
    else {
        asmout << (forceName ? forceName : hashutils::ExtractTmp("function", exp.GetName()));
    }


    asmout << std::flush << "(";

    // local var size = <empty>, <params>, <localvars> so we need to check that we have at least param_count + 1
    if (ctx.m_localvars.size() > exp.GetParamCount()) {
        if (exp.GetParamCount() && (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS))) {
            asmout << " ";
        }

        for (size_t i = 0; i < exp.GetParamCount(); i++) {
            // -1 to avoid the <empty> object, -1 because we are in reverse order
            const auto& lvar = ctx.m_localvars[ctx.m_localvars.size() - i - 2];

            if ((lvar.flags & T8GSCLocalVarFlag::IW_VARIADIC_COUNT) && !lvar.defaultValueNode) {
                continue; // ignore if the varargcount is used
            }
            if (i) {
                asmout << ", ";
            }

            if (lvar.flags & T8GSCLocalVarFlag::VARIADIC) {
                asmout << "...";
            }
            else {
                if (lvar.flags & T8GSCLocalVarFlag::ARRAY_REF) {
                    asmout << "&";
                }
                else if (gscFile.GetMagic() != VMI_T8_36 && (lvar.flags & T8GSCLocalVarFlag::T9_VAR_REF)) {
                    asmout << "*";
                }

                asmout << hashutils::ExtractTmp("var", lvar.name) << std::flush;
            }

            byte mask = ~(T8GSCLocalVarFlag::VARIADIC | T8GSCLocalVarFlag::ARRAY_REF);

            if (ctx.m_vm != VMI_T8_36) {
                mask &= ~T8GSCLocalVarFlag::T9_VAR_REF;
            }
            
            if (lvar.flags & mask) {
                asmout << " (unk flags: " << std::hex << (int)lvar.flags << ")";
            }
            if (lvar.defaultValueNode) {
                asmout << " = ";
                DecompContext dctx = { 0, 0, ctx.m_opt, 0 };
                lvar.defaultValueNode->Dump(asmout, dctx);
            }
        }

        if (exp.GetParamCount() && (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS))) {
            asmout << " ";
        }
    }
    asmout << ")";
}

static bool ReadDbgFile(const std::filesystem::path& path, std::string& buffer) {
    std::filesystem::path dbgPath{ path };
    std::string ext{ path.extension().string() };
    
    auto TestLoad = [&buffer, &path](const char* ext) -> bool {
        std::filesystem::path dbgPath{ path };
        dbgPath.replace_extension(ext);
        if (!utils::ReadFile(dbgPath, buffer)) {
            return false;
        }

        LOG_INFO("Loaded debug data {}", dbgPath.string());
        return true;
    };

    if (ext == ".gscc") {
        if (TestLoad(".gsc.gdb") || TestLoad(".gscgdbc")) return true;
    }
    else if (ext == ".cscc") {
        if (TestLoad(".csc.gdb") || TestLoad(".cscgdbc")) return true;
    }
    else if (ext == ".gscbin") {
        if (TestLoad(".gscbin.gdb") || TestLoad(".gscbingdbc")) return true;
    }
    else if (ext == ".gshc") {
        if (TestLoad(".gsh.gdb") || TestLoad(".gshgdbc")) return true;
    }
    else if (ext == ".cshc") {
        if (TestLoad(".csh.gdb") || TestLoad(".cshgdbc")) return true;
    }
    return false;
}

int tool::gsc::gscinfo(Process& proc, int argc, const char* argv[]) {
    GscDecompilerGlobalContext gdctx{};

    if (!gdctx.opt.Compute(argv, 2, argc) || gdctx.opt.m_help) {
        gdctx.opt.PrintHelp();
        return 0;
    }

    if (gdctx.opt.m_dbgInputDir) {
        std::vector<std::filesystem::path> dbgs{};

        utils::GetFileRecurse(gdctx.opt.m_dbgInputDir, dbgs, [](const std::filesystem::path& path) -> bool {
            auto str = path.string();
            return str.ends_with(".gdb");
        });

        for (const std::filesystem::path& dbg : dbgs) {
            tool::gsc::gdb::ACTS_GSC_GDB* gdb = gdctx.alloc.New<tool::gsc::gdb::ACTS_GSC_GDB>(dbg);

            LOG_DEBUG("Loading GDB file {}", dbg.string());

            if (!gdb->ReadFrom(dbg)) {
                gdctx.alloc.Free(gdb);
                continue;
            }

            tool::gsc::gdb::ACTS_GSC_GDB*& ref = gdctx.debugObjects[gdb->name];
            if (ref) {
                LOG_WARNING("Debug object '{}' already defined", gdb->nameStr);
                gdctx.alloc.Free(gdb);
                continue; // already defined
            }

            ref = gdb;
        }

        LOG_INFO("{} gdb file(s) loaded", gdctx.debugObjects.size());
    }

    gRosettaOutput = gdctx.opt.m_rosetta;
    gDumpStrings = gdctx.opt.m_dump_strings;


    const char* globalHM = actscli::options().dumpHashmap;
    if (!globalHM && gdctx.opt.m_dump_hashmap != nullptr) {
        // keep the option for backward compatibility
        hashutils::SaveExtracted(true, false);
    }
    bool computed{};
    int ret{ tool::OK };
    struct FileOrigin {
        std::filesystem::path base;
        bool dir{};
        std::vector<std::filesystem::path> scriptFiles{};
    };
    std::vector<FileOrigin> scriptFiles{};
    for (const auto& file : gdctx.opt.m_inputFiles) {
        scriptFiles.emplace_back(file);
        FileOrigin& orFi{ scriptFiles[scriptFiles.size() - 1] };
        if (std::filesystem::is_directory(orFi.base)) {
            utils::GetFileRecurse(orFi.base, orFi.scriptFiles, [](const std::filesystem::path& path) -> bool {
                std::string pathname = path.string();

                return pathname.ends_with(".gscc") || pathname.ends_with(".cscc")
                    || pathname.ends_with(".gscbin") || pathname.ends_with(".cscbin") // xensik GSCBIN format
                    || pathname.ends_with(".gshc") || pathname.ends_with(".cshc")
                    || pathname.ends_with(".gsic") || pathname.ends_with(".csic") // Serious GSIC format
                    ;
                }, true);
            orFi.dir = true;
        }
        else {
            orFi.scriptFiles.emplace_back(orFi.base.filename());
        }
    }

    if (gdctx.opt.m_sync) {
        std::string buffer{};
        void* bufferAlign{};
        size_t size{};
        for (FileOrigin& pathLoc : scriptFiles) {
            for (std::filesystem::path pathRel : pathLoc.scriptFiles) {
                std::filesystem::path path{ pathLoc.dir ? pathLoc.base / pathRel : pathLoc.base };
                LOG_DEBUG("Reading {} ({})", path.string(), pathRel.string());

                if (!utils::ReadFileAlign(path, buffer, bufferAlign, size)) {
                    LOG_ERROR("Can't read file data for {}", path.string());
                    continue;
                }
                std::string dbgBuffer;

                if (!ReadDbgFile(path, dbgBuffer)) {
                    dbgBuffer = {};
                }

                try {
                    auto lret = DecompileGsc((byte*)bufferAlign, size, pathRel, gdctx, (byte*)dbgBuffer.data(), dbgBuffer.size());
                    if (lret != tool::OK) {
                        ret = lret;
                    }
                }
                catch (std::runtime_error& e) {
                    LOG_ERROR("Exception when reading {}: {}", path.string(), e.what());
                    ret = tool::BASIC_ERROR;
                }
            }
        }
    }
    else {
        LOG_WARNING("Using experimental async mode");
        uint64_t prevAsyncTypes = core::async::GetAsyncTypes();
        core::async::SetAsync(core::async::AT_ALL);

        std::mutex mtx{};
        gdctx.asyncMtx = &mtx;

        BS::thread_pool pool{};

        for (FileOrigin& pathLoc : scriptFiles) {
            for (std::filesystem::path pathRel : pathLoc.scriptFiles) {
                std::filesystem::path path{ pathLoc.dir ? pathLoc.base / pathRel : pathLoc.base };
                pool.detach_task([path, pathRel, &mtx, &ret, &gdctx] {
                    std::string buffer{};
                    void* bufferAlign{};
                    size_t size{};

                    LOG_DEBUG("Reading {} ({})", path.string(), pathRel.string());
                    int lret;
                    if (!utils::ReadFileAlign(path, buffer, bufferAlign, size)) {
                        lret = tool::BASIC_ERROR;
                    }
                    else if (size < 0x8) { // MAGIC (8) or GSC\0 + 3 * uint32_t
                        LOG_ERROR("Invalid header, file size: {:x}/{:x} for {}", size, 0x8, path.string());
                        lret = tool::BASIC_ERROR;
                    }
                    else {
                        lret = DecompileGsc((byte*)bufferAlign, size, pathRel, gdctx);
                    }

                    if (lret != tool::OK) {
                        std::lock_guard lg{ mtx };
                        ret = lret;
                    }
                   });
            }
        }

        pool.wait();

        core::async::SetAsync(prevAsyncTypes);
    }


    if (gdctx.hardErrors) {
        LOG_ERROR("{} (0x{:x}) error(s), are you using the right vm type?", gdctx.hardErrors, gdctx.hardErrors);
    }
    LOG_INFO("{} (0x{:x}) file(s) decompiled.", gdctx.decompiledFiles, gdctx.decompiledFiles);

    if (!globalHM) {
        hashutils::WriteExtracted(gdctx.opt.m_dump_hashmap);
    }

    if (gDumpStrings) {
        std::ofstream os{ gDumpStrings };
        if (!os) {
            LOG_ERROR("Can't open string output");
        }
        else {
            for (const auto& str : gDumpStringsStore) {
                os << str << std::endl;
            }
            os.close();
        }
    }
    if (gdctx.opt.vtable_dump) {
        std::ofstream os{ gdctx.opt.vtable_dump };
        if (!os) {
            LOG_ERROR("Can't open vtable output");
        }
        else {
            os << "script,class,namespace,name";
            for (auto& [script, clss] : gdctx.vtables) {
                for (auto& [cls, ns] : clss) {
                    for (auto& n : ns) {
                        os
                            << std::endl
                            << hashutils::ExtractTmp("script", script) << ","
                            << hashutils::ExtractTmp("class", cls) << ","
                            << hashutils::ExtractTmp("namespace", n.name_space) << ","
                            << hashutils::ExtractTmp("function", n.name)
                            ;
                    }
                }
            }


            os.close();
            LOG_INFO("btable into '{}'", gdctx.opt.vtable_dump);
        }

    }

    if (gdctx.opt.m_gdbZipOutputFile) {
        std::filesystem::path zip{ gdctx.opt.m_gdbZipOutputFile };
        miniz_cpp::zip_file zf{};

        bool any{};
        for (auto& [fn, gdb] : gdctx.gdbData) {
            if (!gdb->gdb) continue;
            tool::gsc::vm::GscGdb *reader{ tool::gsc::vm::GetGdbReader(gdb->gdb) };
            if (!reader) continue; // can't find reader
            std::string bytes{};
            bool r{ reader->saver(gdb, bytes) };
            char* n{ utils::va("%s.gdb", hashutils::ExtractTmp("script", fn))};
            n = utils::MapString(n, [](char c) -> char { return c == '\\' ? '/' : c; });
            if (!r) {
                LOG_ERROR("Can't compile gdb file {}", n);
                continue;
            }
            LOG_DEBUG("write {}: {} bytes", n, bytes.size());
            zf.writestr(n, bytes);
            any = true;
        }
        if (any) {
            zf.save(zip.string());
            LOG_INFO("gdb data saved into {}", zip.string());
        }
        else {
            LOG_WARNING("No gdb data generated");
        }
    }

    if (gRosettaOutput) {
        std::ofstream os{ gRosettaOutput, std::ios::binary };

        if (!os) {
            LOG_ERROR("Can't open rosetta output");
        }
        else {
            os.write("ROS2", 4);

            uint64_t len = (uint64_t)gRosettaBlocks.size();
            os.write(reinterpret_cast<const char*>(&len), sizeof(len));

            for (const auto& [key, data] : gRosettaBlocks) {
                // gsc header
                len = (uint64_t)data.header.length();
                os.write(reinterpret_cast<const char*>(&len), sizeof(len));
                os.write(data.header.data(), data.header.length());
                len = (uint64_t)data.blocks.size();
                os.write(reinterpret_cast<const char*>(&len), sizeof(len));
                for (const auto& block : data.blocks) {
                    os.write(reinterpret_cast<const char*>(&block), sizeof(block));
                }
            }

            // TODO: add crc
            os.write("END", 3);


            os.close();
            LOG_INFO("Rosetta index 2 created into '{}'", gRosettaOutput);
        }
    }

    LOG_INFO("done.");
    return ret;
}

ADD_TOOL(gscinfo, "gsc", "", "GSC decompiler/disassembler", nullptr, gscinfo);
ADD_TOOL(gscd, "gsc", "", "GSC decompiler/disassembler", nullptr, gscinfo);
ADD_TOOL(dds, "gsc", " [input=scriptparsetree] [output=dataset.csv]", "dump dataset from gscinfo", nullptr, dumpdataset);
