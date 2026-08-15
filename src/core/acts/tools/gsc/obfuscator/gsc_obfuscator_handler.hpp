#pragma once
#include <tools/gsc/data/gsc_data_t7.hpp>
#include <tools/gsc/data/gsc_data_t8.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_export.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_import.hpp>
#include <tools/gsc/gsc_obfuscator.hpp>
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/gsc_vm.hpp>

namespace tool::gsc::obfuscator::handler {
    using T7GSCExport = tool::gsc::T7GSCExport;
    using T7GSCOBJ = tool::gsc::T7GSCOBJ;
    using T7GSCImport = tool::gsc::T8GSCImport;
    using T7GSCString = tool::gsc::T8GSCString;

    struct NameLocated {
        uint64_t name_space;
        uint64_t name;
        uint64_t script{};
    };
    struct NameLocatedHash {
        size_t operator()(const NameLocated& k) const {
            return k.name_space ^ std::rotl(k.name, 21) ^ std::rotl(k.script, 42);
        }
    };
    struct NameLocatedEquals {
        bool operator()(const NameLocated& a, const NameLocated& b) const {
            return a.name == b.name && a.name_space == b.name_space && a.script == b.script;
        }
    };

    template<typename Value>
    using NameLocatedMap = std::unordered_map<NameLocated, Value, NameLocatedHash, NameLocatedEquals>;

    struct GscExportData {
        void* ref;
        byte* bc{};
        byte flags{};
        size_t len{};
    };

    struct GscImportData {
        void* ref;
        byte flags{};
    };

    struct FreeDataChunk {
        size_t rva;
        size_t len;
    };

    class GscObfuscator {
        tool::gsc::obfuscator::options::GscObfOptions& opt;
        byte* script;
        size_t scriptLen;
        void* scriptEnd;
        bool u16OpCodes;
        bool alignedCode;
        bool modToolOpCodes{};
        size_t trampolineSize;
        std::shared_ptr<GSCOBJHandler> scriptFile{};
        std::unique_ptr<GSCExportReader> exportReader{};
        std::unique_ptr<GSCImportReader> importReader{};
        opcode::VmInfo* vmInfo{};

        std::vector<GscImportData*> imports{};
        std::vector<GscExportData*> exports{};
        NameLocatedMap<std::vector<GscExportData*>> exportsMap{};
        NameLocatedMap<std::vector<GscImportData*>> importsMap{};
        std::vector<FreeDataChunk> freeData{};
        GscDecompilerGlobalContext gdctx{};
        std::unordered_map<uint64_t, std::unordered_set<uint32_t>> opcodesLocsMap{};
        std::unordered_set<uint32_t>* opcodesLocs{};
        GscDecompilerGDBData* gdbData{};

      public:
        GscObfuscator(tool::gsc::obfuscator::options::GscObfOptions& opt, byte* script, size_t scriptLen);

        // init
        void DecompileScript();
        void PatchOpcodes();
        void ApplyPrivateHashes();
        void KillDevStrings();
        void KillPrivateExports();
        void ApplyPrivateStrings();
        void ApplyPrivateScripts();
        void RewriteImports();
        void CreateTrampolines();
        void KillRemainingDevBlocks();
        void ComputeChecksums();
        void RunTasks();

        // utils
        void PrintHeader();
        void ValidateInScript(const void* ptr, const char* descr);
        void ValidateStringInScript(const char* ptr, const char* descr);
        FreeDataChunk* FetchNearFreeDataChunk(size_t loc, size_t maxDist, size_t minSize, size_t align);
        void SetAndPassOpCode(byte*& loc, opcode::OPCode val);
        uint16_t GetAndPassOpCode(byte*& loc);

        // align a location relative to the header (used to avoid aligning the script in memory)
        byte* Aligned(byte* loc, size_t size) { return &script[utils::Aligned<size_t>(loc - script, size)]; }
        template<typename T>
        byte* Aligned(byte* loc) {
            return Aligned(loc, sizeof(T));
        }

        template<typename T>
        T& ReadAlignedRef(byte*& loc, const char* name) {
            if constexpr (sizeof(T) != 1) {
                if (alignedCode) {
                    loc = Aligned<T>(loc);
                }
            }
            ValidateInScript(loc + (sizeof(T) - 1), name);
            T& t{ *(T*)loc };
            loc += sizeof(T);
            return t;
        }
    };
} // namespace tool::gsc::obfuscator::handler