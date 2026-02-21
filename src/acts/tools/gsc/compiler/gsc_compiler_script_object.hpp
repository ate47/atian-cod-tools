#pragma once
#include <tools/gsc/compiler/gsc_compiler_grammar.hpp>
#include <tools/gsc/compiler/gsc_compiler_asm.hpp>
#include <tools/gsc/compiler/gsc_compiler_function_object.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>

namespace tool::gsc::compiler {
    class CompileObject;

    enum GscFileType {
        FILE_GSC,
        FILE_CSC
    };

    struct NumberOpCodesS {
        OPCode opcode;
        int64_t minValue;
        int64_t maxValue;
        size_t sizeOf;
        bool negat;
        bool unsign;
    };

    class StringObject {
    public:
        uint32_t location{};
        size_t forceLen{};
        bool devString{};
        std::vector<uint32_t*> listeners{};
        std::vector<AscmNodeData<uint32_t>*> nodes{};
    };
    struct AnimObject {
        uint32_t location{};
        std::vector<AscmNodeOpCode*> anim{};
    };
    struct AnimTreeObject {
        uint32_t location{};
        std::vector<AscmNodeOpCode*> animtree{};
        std::unordered_map<std::string, AnimObject> anims{};
    };
    class ImportObject {
    public:
        byte flags;
        byte params;
        std::vector<AscmNodeFunctionCall*> nodes{};
    };
    class GlobalVarObject {
    public:
        tool::gsc::opcode::GlobalVariableDef* def{};
        std::vector<AscmNodeGlobalVariable*> nodes{};
    };

    struct ClassCompileContext {
        uint64_t name;
        std::unordered_map<uint64_t, ParseTree*> vars{};
        std::unordered_set<uint64_t> funcs{};
    };

    class CompileObject {
    public:
        union {
            AscmNode* opcode;
            uint32_t strlistener;
        } crcData{};
        size_t devBlockDepth{};
        CompilerConfig& config;
        InputInfo& info;
        GscFileType type;
        uint64_t currentNamespace;
        uint64_t fileName{};
        const char* fileNameStr{};
        uint64_t fileNameSpace{};
        std::string lastAnimTree{};
        std::set<std::string> includes{};
        std::vector<AscmNode*> m_devBlocks{};
        std::unordered_map<uint64_t, FunctionObject> exports{};
        std::unordered_map<std::string, StringObject> strings{};
        std::unordered_map<std::string, AnimTreeObject> animtrees{};

        std::unordered_map<Located, std::vector<AscmNodeLazyLink*>, LocatedHash, LocatedEquals> lazyimports{};
        std::unordered_map<Located, std::vector<ImportObject>, LocatedHash, LocatedEquals> imports{};
        std::unordered_map<std::string, GlobalVarObject> globals{};
        std::unordered_map<uint64_t, ParseTree*> constexprs{};
        VmInfo* vmInfo;
        int64_t autoexecOrder{};
        std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler;
        std::unordered_set<std::string> hashes{};
        size_t emptyNameInc{};
        ClassCompileContext* clsCtx{};

        CompileObject(CompilerConfig& config, GscFileType file, InputInfo& nfo, std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler);

        StringObject& RegisterString(const char* str);

        StringObject& RegisterString(const std::string& str);

        void AddHash(const char* str);

        void AddHash(const std::string& str);


        void AddInclude(const std::string& data);

        bool HasOpCode(OPCode opcode);

        TerminalNode* GetUniqueNode(ParseTree* tree, bool error = true);

        float FloatNumberNodeValue(ParseTree* number, bool error = true);

        int64_t NumberNodeValue(ParseTree* number, bool error = true, bool* extracted = nullptr);

        AscmNodeOpCode* BuildAscmNodeData(const NumberOpCodesS& opcode, int64_t val) const;

        /*
         * Compute the node using the minimum amount of bits
         * @param val value to encode
         * @return node
         */
        AscmNodeOpCode* BuildAscmNodeData(int64_t val);

        AscmNodeOpCode* CreateBiggestValue(int64_t val);

        bool TryStringNodeValue(ParseTree* hashNode, std::string& output);

        bool TryHashNodeValue(ParseTree* hashNode, uint64_t& output);

        std::string GetIncludeName(const std::string& origin);

        bool Compile(std::vector<byte>& data, std::vector<byte>* pdbgdata);

        void AddImport(AscmNodeFunctionCall* funcCall, uint64_t funcNspHash, uint64_t funcHash, size_t paramCount, uint8_t importFlags);

        void AddLazy(AscmNodeLazyLink* lazyLink);
    };


}