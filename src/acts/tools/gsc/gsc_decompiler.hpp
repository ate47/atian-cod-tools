#pragma once
#include <tools/gsc/gsc.hpp>

namespace tool::gsc {

    struct NameLocated {
        uint64_t name_space;
        uint64_t name;
        uint64_t script{};
    };
    struct NameLocatedHash {
        size_t operator()(const NameLocated& k) const {
            return k.name_space ^ RotateLeft64(k.name, 21) ^ RotateLeft64(k.script, 42);
        }
    };
    struct NameLocatedEquals {
        bool operator()(const NameLocated& a, const NameLocated& b) const {
            return a.name == b.name && a.name_space == b.name_space && a.script == b.script;
        }
    };

    enum GscInfoOptionStepSkip {
        STEPSKIP_DEV = 1,
        STEPSKIP_SWITCH = 2,
        STEPSKIP_FOREACH = 4,
        STEPSKIP_WHILE = 8,
        STEPSKIP_IF = 0x10,
        STEPSKIP_FOR = 0x20,
        STEPSKIP_RETURN = 0x40,
        STEPSKIP_BOOL_RETURN = 0x80,
        STEPSKIP_DEVBLOCK_INLINE = 0x100,
        STEPSKIP_CLASSMEMBER_INLINE = 0x200,
        STEPSKIP_SPECIAL_PATTERN = 0x400,
        STEPSKIP_COMPILER_PATTERNS = 0x800,
    };

    // cli options
    class GscInfoOption {
    public:
        bool m_dcomp{};
        bool m_dasm{};
        bool m_help{};
        bool m_header{};
        bool m_imports{};
        bool m_strings{};
        bool m_gvars{};
        bool m_includes{ true };
        bool m_exptests{};
        bool m_patch{ true };
        bool m_func{ true };
        bool m_func_rloc{};
        bool m_func_floc{};
        bool m_func_header{ true };
        bool m_func_header_post{};
        bool m_show_jump_delta{};
        bool m_show_pre_dump{};
        bool m_show_ref_count{};
        bool m_show_opcode_values{ true };
        bool m_tokens{};
        bool m_test_header{};
        bool m_dataDump{};
        const char* m_dump_hashmap{};
        const char* m_dump_strings{};
        const char* m_outputDir{};
        const char* m_dbgOutputDir{};
        const char* m_copyright{};
        const char* m_dbgInputDir{};
        const char* m_gdbZipOutputFile{};
        bool m_show_internal_blocks{};
        bool m_show_func_vars{};
        bool m_mark_jump_type{};
        bool m_display_stack{};
        bool m_use_internal_names{};
        bool m_generateGdbData{};
        bool m_generateGdbBaseData{ true };
        bool m_splitByVm{};
        bool m_rawhash{};
        bool m_noPath{};
        bool m_ignoreDebugPlatform{};
        bool m_sync{ true };
        bool m_vtable{};
        bool m_debugHashes{};
        bool m_usePathOutput{};
        bool m_dumpSkipData{};
        bool m_noUsingsSort{};
        bool m_noStrDecrypt{};
        bool m_lineCount{};
        const char* vtable_dump{};
        uint32_t m_stepskip{};
        opcode::Platform m_platform{ opcode::Platform::PLATFORM_PC };
        opcode::VMId m_vm{ opcode::VMId::VMI_UNKNOWN };
        const formatter::FormatterInfo* m_formatter{};
        const char* (*LookupLocalizedFunc)(uint64_t hash) {};


        std::vector<const char*> m_inputFiles{};

        GscInfoOption();
        /*
         * Compute options
         * @param args Arguments
         * @param startIndex Start index in args
         * @param startIndex End index in args (exclusive)
         * @return if the arguments were correctly computed
         */
        bool Compute(const char** args, INT startIndex, INT endIndex);
        /*
         * Print help in a stream
         */
        void PrintHelp();
    };

    enum GscDecompilerGlobalContextWarn : uint64_t {
        GDGCW_BAD_HASH_PATH = 1,
        GDGCW_BAD_HASH_FIELD = 1 << 1,
        GDGCW_BAD_HASH_FILE = 1 << 2,
        GDGCW_BAD_HASH_PATH_INCLUDE = 1 << 3,
    };

    struct GscExportInformation {
        bool boolFunc{};
        bool devFunc{};
    };

    struct GscDecompilerGDBData {
        uint64_t gdb{};
        uint32_t checksum{};
        std::set<uint32_t> devStringsLocation{};
        std::map<size_t, uint32_t> fileOffsets{};
        std::map<size_t, uint32_t> lineInfos{};
    };

    struct GscDecompilerGlobalContext {
        core::memory_allocator::MemoryAllocator alloc{};
        std::mutex* asyncMtx{};
        GscInfoOption opt{};
        bool noDump{};
        uint64_t warningOpt{};
        std::unordered_map<uint64_t, tool::gsc::gdb::ACTS_GSC_GDB*> debugObjects{};
        std::unordered_map<uint64_t, GscDecompilerGDBData*> gdbData{};
        size_t decompiledFiles{};
        size_t hardErrors{};
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::unordered_set<NameLocated, NameLocatedHash, NameLocatedEquals>>> vtables{};
        std::unordered_map<NameLocated, GscExportInformation, NameLocatedHash, NameLocatedEquals> exportInfos{};
        std::unordered_map<uint64_t, std::unordered_set<uint32_t>>* opcodesLocs{};
        std::unordered_set<std::string>* dumpStringsStore{};
        std::vector<const char*>* stringsLoc{};

        bool WarningType(GscDecompilerGlobalContextWarn warn);
    };

    int DecompileGsc(byte* data, size_t size, std::filesystem::path fsPath, GscDecompilerGlobalContext& gdctx, byte* dbgData = nullptr, size_t dbgSize = 0);

}