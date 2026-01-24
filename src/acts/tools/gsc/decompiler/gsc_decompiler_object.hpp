#pragma once
#include <tools/gsc/gsc_decompiler.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <tools/gsc/vm/vm_jup.hpp>

namespace tool::gsc {
    struct T8GSCExport;
    struct IW23GSCImport;
    class T8GSCOBJContext;
    class GSCOBJHandler;
    struct GSCExportReader;
    struct GscDecompilerGDBData;

    struct GSCOBJTokenData {
        uint32_t loc;
        bool isString;
        union {
            uint32_t id;
            const char* str;
        } val;
    };

    struct asmcontext_func {
        uint64_t name;
        uint64_t nsp;
    };

    struct gscclass {
        uint64_t name_space = 0;
        bool hasAnimTrees{};
        const char* animtree{};
        std::set<uint64_t> m_superClass{};
        std::vector<uint64_t> m_methods{};
        std::unordered_set<NameLocated, NameLocatedHash, NameLocatedEquals> m_vtableMethods{};
        std::unordered_map<uint64_t, asmcontext_func> m_vtable{};
    };

    struct GscDetourInfo {
        uint64_t name;
        uint64_t replaceNamespace;
        uint64_t replaceFunction;
        int32_t fixupOffset;
        int32_t fixupSize;
        int64_t replaceScript;
    };

    struct GsicInfo {
        bool isGsic = false;
        size_t headerSize = 0;
        std::unordered_map<uint64_t, GscDetourInfo> detours{};
    };



    // Result context for T8GSCOBJ::PatchCode
    class T8GSCOBJContext {
    private:
        std::unordered_map<uint64_t, char*> m_allocatedStrings{};
    public:
        std::unordered_map<NameLocated, opcode::ASMContext, NameLocatedHash, NameLocatedEquals> contextes{};
        std::unordered_map<uint16_t, uint64_t> m_gvars{};
        std::unordered_map<uint32_t, const char*> m_stringRefs{};
        std::unordered_map<uint32_t, uint32_t> m_stringRefsLoc{};
        std::vector<GSCOBJTokenData> m_tokens{};
        std::vector<IW23GSCImport> m_linkedImports{};
        // getnumber hack
        std::unordered_map<uint32_t, uint32_t> m_animTreeLocations{};
        std::map<std::string, std::set<uint32_t>> m_unkstrings{};
        std::unordered_map<NameLocated, std::vector<uint32_t>, NameLocatedHash, NameLocatedEquals> m_lazyLinks{};
        std::set<uint32_t> m_devblocks{};
        GsicInfo m_gsicInfo{};
        opcode::VmInfo* m_vmInfo{};
        bool isBigEndian{};
        byte* dbgData{};
        size_t dbgSize{};
        GscDecompilerGDBData* gdbData{};
        utils::LineStreamBuf* lineBuf{};
        tool::gsc::opcode::Platform currentPlatform{};
        std::shared_ptr<GSCOBJHandler> scriptfile{};
        std::unique_ptr<GSCExportReader> exp{};
        std::unordered_map<uint64_t, gscclass> m_classes{};
        tool::gsc::gdb::ACTS_GSC_GDB* gdbctx{};
        const tool::gsc::formatter::FormatterInfo* m_formatter{};
        const GscInfoOption& opt;
        GscDecompilerGlobalContext& gdctx;
        T8GSCOBJContext(GscDecompilerGlobalContext& gdctx);
        ~T8GSCOBJContext();

        constexpr bool SwitchEndian() const {
            if (isBigEndian) {
                return std::endian::native != std::endian::big;
            }
            else {
                return std::endian::native != std::endian::little;
            }
        }

        /*
         * Get a name for a global var ref
         * @param gvarRef ref
         * @return name or 0
         */
        uint64_t GetGlobalVarName(uint16_t gvarRef);
        /*
         * Get a string for a string ref
         * @param stringRef ref
         * @return string or null
         */
        const char* GetStringValue(uint32_t stringRef);
        /*
         * Get a string for a string floc
         * @param floc loc
         * @return string or null
         */
        const char* GetStringValueByLoc(uint32_t floc);
        /*
         * Get a token for a floc
         * @param floc loc
         * @return string or null
         */
        const char* GetTokenValue(uint32_t tokenRef);
        /*
         * Get a string for a string ref, return errorValue in case of error
         * @param stringRef string ref
         * @param errorValue returned value in case of bad ref
         * @return string or error value
         */
        const char* GetStringValueOrError(uint32_t stringRef, uint32_t floc, const char* errorValue);
        /*
         * Get a string for a string ref, return errorValue in case of error
         * @param errorValue returned value in case of bad ref
         * @return string or error value
         */
        const char* GetStringError(uint32_t floc, const char* errorValue);
        /*
         * Add a global var
         * @param value name
         * @return var ref
         */
        uint16_t AddGlobalVarName(uint64_t value);
        /*
         * Add a string
         * @param value string
         * @return new string ref
         */
        uint32_t AddStringValue(const char* value);
        /*
         * Add a string
         * @param floc location
         * @param str string ref
         */
        void AddStringRef(uint32_t floc, uint32_t str);
        /*
         * Clone a string inside this context
         * @param str the string
         * @return allocated string
         */
        char* CloneString(const char* str);
        /*
         * Get file location name
         * @param floc file location
         * @return render name
         */
        const char* GetFLocName(uint32_t floc) const;
    };

    enum DumpVTableAnswer : int {
        DVA_OK = 0,
        DVA_NOT,
        DVA_BAD,
    };

    void DumpFunctionHeader(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx, int padding, const char* forceName, const char** currentAnimTree);
    int DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx);
    DumpVTableAnswer DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJHandler& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, opcode::DecompContext& dctxt);



    enum GscObjHandlerBuildFlags : uint64_t {
        GOHF_GLOBAL = 0x1,
        GOHF_ANIMTREE = 0x2,
        GOHF_ANIMTREE_DOUBLE = 0x4,
        GOHF_NOTIFY_CRC = 0x8,
        GOHF_SUPPORT_VAR_VA = 0x10,
        GOHF_SUPPORT_VAR_REF = 0x20,
        GOHF_SUPPORT_VAR_PTR = 0x40,
        GOHF_SUPPORT_EV_HANDLER = 0x80,
        GOHF_INLINE_FUNC_PTR = 0x100,
        GOHF_FOREACH_TYPE_V1 = 0x200,
        GOHF_FOREACH_TYPE_V2 = 0x400,
        GOHF_FOREACH_TYPE_V3 = 0x800,

        GOHF_FOREACH_TYPE_T8 = GOHF_FOREACH_TYPE_V1, // 100
        GOHF_FOREACH_TYPE_T9 = GOHF_FOREACH_TYPE_V2, // 010
        GOHF_FOREACH_TYPE_JUP = GOHF_FOREACH_TYPE_V1 | GOHF_FOREACH_TYPE_V2, // 110
        GOHF_FOREACH_TYPE_T7 = GOHF_FOREACH_TYPE_V3, // 001
        // for later
        GOHF_FOREACH_TYPE_5 = GOHF_FOREACH_TYPE_V1 | GOHF_FOREACH_TYPE_V3, // 101
        GOHF_FOREACH_TYPE_6 = GOHF_FOREACH_TYPE_V2 | GOHF_FOREACH_TYPE_V3, // 110
        GOHF_FOREACH_TYPE_7 = GOHF_FOREACH_TYPE_V1 | GOHF_FOREACH_TYPE_V2 | GOHF_FOREACH_TYPE_V3, // 111
        GOHF_FOREACH_TYPE_MASK = GOHF_FOREACH_TYPE_V1 | GOHF_FOREACH_TYPE_V2 | GOHF_FOREACH_TYPE_V3,
        GOHF_SUPPORT_GET_API_SCRIPT = 0x1000,
        GOHF_STRING_NAMES = 0x2000,
        GOHF_NOTIFY_CRC_STRING = 0x4000,
        GOHF_FILENAMESPACE = 0x8000,
        GOHF_VAR_VA_COUNT = 0x10000,
        GOHF_IW_BIN = 0x20000,
        GOHF_SWITCH_TYPE_V1 = 0x40000,
        GOHF_SWITCH_TYPE_V2 = 0x80000,
        GOHF_SWITCH_TYPE_V3 = 0x100000,

        GOHF_SWITCH_TYPE_IFTABLE = 0, // 000 default value
        GOHF_SWITCH_TYPE_T89 = GOHF_SWITCH_TYPE_V1, // 100
        GOHF_SWITCH_TYPE_T7 = GOHF_SWITCH_TYPE_V2, // 010
        GOHF_SWITCH_TYPE_IW = GOHF_SWITCH_TYPE_V1 | GOHF_SWITCH_TYPE_V2, // 110
        GOHF_SWITCH_TYPE_4 = GOHF_SWITCH_TYPE_V3, // 001
        GOHF_SWITCH_TYPE_5 = GOHF_SWITCH_TYPE_V1 | GOHF_SWITCH_TYPE_V3, // 101
        GOHF_SWITCH_TYPE_6 = GOHF_SWITCH_TYPE_V2 | GOHF_SWITCH_TYPE_V3, // 110
        GOHF_SWITCH_TYPE_7 = GOHF_SWITCH_TYPE_V1 | GOHF_SWITCH_TYPE_V2 | GOHF_SWITCH_TYPE_V3, // 111
        GOHF_SWITCH_TYPE_MASK = GOHF_SWITCH_TYPE_V1 | GOHF_SWITCH_TYPE_V2 | GOHF_SWITCH_TYPE_V3,
    };

    struct GsicDetour {
        uint32_t name;
        uint32_t replaceNamespace;
        uint32_t replaceFunction;
        int32_t fixupOffset;
        int32_t fixupSize;
        int32_t replaceScriptTop;
        int32_t replaceScriptBottom;
    };

    enum GSCBinTokenType : uint32_t {
        GBTT_INVALID = 0,
        GBTT_FIELD = 1,
        GBTT_STRING = 4,
    };

    struct GSCBINToken {
        GSCBinTokenType type;
        uint32_t val;
        uint32_t location;
    };

    enum ACTSGSCImportFlags : uint8_t {
        ACTS_GET_BUILTIN_FUNCTION = 0x8,
        ACTS_CALL_BUILTIN_FUNCTION = 0x9,
        ACTS_GET_BUILTIN_METHOD = 0xa,
        ACTS_CALL_BUILTIN_METHOD = 0xb,
        ACTS_CALL_BUILTIN_FUNCTION_NO_PARAMS = 0xc,
        ACTS_CALL_BUILTIN_METHOD_NO_PARAMS = 0xd,
        ACTS_USE_FULL_NAMESPACE = 0x40
    };

    class GSCOBJHandler {
        byte* file;
        size_t fileSize;
    public:
        std::filesystem::path* originalFile{};
        uint64_t buildFlags;

        GSCOBJHandler(byte* file, uint64_t fileSize, size_t buildFlags);

        /*
         * Test if the handler has a flag
         * @param flag flag
         * @return if the flag is present
         */
        constexpr bool HasFlag(GscObjHandlerBuildFlags flag) {
            return (buildFlags & flag) != 0;
        }

        virtual void SetFile(byte* file, size_t fileSize);

        /*
         * Return a pointer at a particular shift
         * @param shift in the file
         * @param T align type
         * @return Pointer
         */
        template<typename T = byte>
        constexpr T* Ptr(size_t shift = 0) {
            return reinterpret_cast<T*>(file + shift);
        }

        /*
         * Return a pointer at a particular shift aligned to a type
         * @param shift in the file
         * @param T align type
         * @param R return type
         * @return Pointer
         */
        template<typename T = byte, typename R = T>
        constexpr R* PtrAlign(size_t shift = 0) {
            return reinterpret_cast<R*>(utils::Aligned<T>(file + shift));
        }

        /*
         * Return a reference to a particular shift
         * @param shift in the file
         * @param T align type
         * @return Ref
         */
        template<typename T = byte>
        constexpr T& Ref(size_t shift = 0) {
            return *Ptr<T>(shift);
        }

        /*
         * Return a reference to a particular shift aligned to a type
         * @param shift in the file
         * @param T align type
         * @param R return type
         * @return Ref
         */
        template<typename T = byte, typename R = T>
        constexpr R& RefAlign(size_t shift = 0) {
            return *PtrAlign<T, R>(shift);
        }

        constexpr uint64_t GetMagic() {
            return Ref<uint64_t>();
        }

        // Read functions
        virtual uint64_t GetName() = 0;
        virtual bool IsValidHeader(size_t size) = 0;
        virtual void SwitchHeaderEndian();
        virtual uint16_t GetExportsCount() = 0;
        virtual uint32_t GetExportsOffset() = 0;
        virtual uint16_t GetIncludesCount() = 0;
        virtual uint32_t GetIncludesOffset() = 0;
        virtual uint16_t GetImportsCount() = 0;
        virtual uint32_t GetImportsOffset() = 0;
        virtual uint16_t GetStringsCount() = 0;
        virtual uint32_t GetStringsOffset() = 0;
        virtual uint16_t GetDevStringsCount() = 0;
        virtual uint32_t GetDevStringsOffset() = 0;
        virtual uint16_t GetGVarsCount() = 0;
        virtual uint32_t GetGVarsOffset() = 0;
        virtual uint16_t GetTokensCount();
        virtual uint32_t GetTokensOffset();
        uint32_t GetFileSize() const {
            return (uint32_t)fileSize;
        };
        virtual int64_t GetDefaultChecksum(bool client) = 0;
        virtual const char* GetDefaultName(bool client) = 0;

        virtual size_t GetHeaderSize() = 0;
        virtual size_t GetImportSize() = 0;
        virtual size_t GetExportSize() = 0;
        virtual size_t GetStringSize() = 0;
        virtual size_t GetGVarSize() = 0;
        virtual size_t GetAnimTreeSingleSize() = 0;
        virtual size_t GetAnimTreeDoubleSize() = 0;
        virtual void WriteExport(byte* data, const tool::gsc::IW23GSCExport& item) = 0;
        virtual void WriteImport(byte* data, const tool::gsc::IW23GSCImport& item) = 0;
        virtual void WriteGVar(byte* data, const tool::gsc::T8GSCGlobalVar& item) = 0;
        virtual void WriteString(byte* data, const tool::gsc::T8GSCString& item) = 0;
        virtual void WriteAnimTreeSingle(byte* data, const tool::gsc::GSC_USEANIMTREE_ITEM& item) = 0;
        virtual void WriteAnimTreeDouble(byte* data, const tool::gsc::GSC_ANIMTREE_ITEM& item) = 0;

        // decrypt a string
        virtual char* DecryptString(char* str) = 0;
        // header to encrypt a non ascii string
        virtual std::pair<const char*, size_t> GetStringHeader(size_t len);
        virtual byte RemapFlagsImport(byte flags);
        virtual byte RemapFlagsExport(byte flags);
        virtual uint16_t GetAnimTreeSingleCount() = 0;
        virtual uint32_t GetAnimTreeSingleOffset() = 0;
        virtual uint16_t GetAnimTreeDoubleCount() = 0;
        virtual uint32_t GetAnimTreeDoubleOffset() = 0;
        virtual uint32_t GetCSEGOffset() = 0;
        virtual uint32_t GetCSEGSize() = 0;

        // Write functions
        virtual void SetName(uint64_t name) = 0;
        virtual void SetNameString(uint32_t name);
        virtual void SetHeader() = 0;
        virtual void SetChecksum(uint64_t val) = 0;
        virtual uint32_t GetChecksum() = 0;
        virtual void SetExportsCount(uint16_t val) = 0;
        virtual void SetExportsOffset(uint32_t val) = 0;
        virtual void SetIncludesCount(uint16_t val) = 0;
        virtual void SetIncludesOffset(uint32_t val) = 0;
        virtual void SetImportsCount(uint16_t val) = 0;
        virtual void SetImportsOffset(uint32_t val) = 0;
        virtual void SetStringsCount(uint16_t val) = 0;
        virtual void SetStringsOffset(uint32_t val) = 0;
        virtual void SetDevStringsCount(uint16_t val) = 0;
        virtual void SetDevStringsOffset(uint32_t val) = 0;
        virtual void SetGVarsCount(uint16_t val) = 0;
        virtual void SetGVarsOffset(uint32_t val) = 0;
        virtual void SetFileSize(uint32_t val) = 0;

        virtual void SetAnimTreeSingleCount(uint16_t val) = 0;
        virtual void SetAnimTreeSingleOffset(uint32_t val) = 0;
        virtual void SetAnimTreeDoubleCount(uint16_t val) = 0;
        virtual void SetAnimTreeDoubleOffset(uint32_t val) = 0;

        virtual void SetCSEGOffset(uint32_t val) = 0;
        virtual void SetCSEGSize(uint32_t val) = 0;

        virtual byte MapFlagsImportToInt(byte flags);
        virtual byte MapFlagsExportToInt(byte flags);
        virtual bool IsVTableImportFlags(byte flags) = 0;
        virtual byte GetVTableImportFlags() = 0;

        // Dump header
        virtual void DumpHeaderInternal(std::ostream& asmout, const GscInfoOption& opt);
        void DumpHeader(std::ostream& asmout, const GscInfoOption& opt);
        virtual void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt, T8GSCOBJContext& ctx);
        // Patch script to prepare disasm
        virtual int PatchCode(T8GSCOBJContext& ctx);
        virtual int PreLoadCode(T8GSCOBJContext& ctx, std::ostream& asmout);
        virtual opcode::Platform ComputePlatform(T8GSCOBJContext& ctx);
    };

    std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* GetGscReader(uint64_t vm);

}