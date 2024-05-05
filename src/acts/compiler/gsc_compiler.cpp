#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_acts_debug.hpp"

using namespace antlr4;
using namespace antlr4::tree;
using namespace tool::gsc::opcode;

#pragma push_macro("ERROR")
#undef ERROR
constexpr auto TREE_ERROR = ParseTreeType::ERROR;
constexpr auto TREE_RULE = ParseTreeType::RULE;
constexpr auto TREE_TERMINAL = ParseTreeType::TERMINAL;
#pragma pop_macro("ERROR")

class GscCompilerOption;
class FunctionObject;
class CompileObject;
class ACTSErrorListener;
struct InputInfo;

struct FunctionVar {
    std::string name;
    size_t id;
    byte flags;
};

struct Located {
    uint64_t name_space;
    uint64_t name;
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

template<typename Type>
constexpr size_t maxNumberSize() {
    return (1ull << (sizeof(Type) << 3));
}

constexpr int64_t MAX_JUMP = maxNumberSize<uint16_t>();

class AscmCompilerContext {
public:
    const VmInfo* vmInfo;
    Platform plt;
    std::vector<byte>& data;
    size_t lvars;

    AscmCompilerContext(const VmInfo* vmInfo, Platform plt, size_t lvars, std::vector<byte>& data) : vmInfo(vmInfo), plt(plt), data(data), lvars(lvars) {}

    bool HasAlign() const {
        return vmInfo->HasFlag(VmFlags::VMF_OPCODE_SHORT);
    }

    template<typename Type>
    void Align() {
        if (HasAlign()) {
            utils::Aligned<Type>(data);
        }
        // not required
    }
    template<typename Type>
    void Write(Type value) {
        utils::WriteValue<Type>(data, value);
    }
};

enum AscmNodeType {
    ASCMNT_UNKNOWN = 0,
    ASCMNT_OPCODE = 1,
};

class AscmNode {
public:
    int32_t rloc{};
    int32_t floc{};
    size_t line{};
    AscmNodeType nodetype{ ASCMNT_UNKNOWN };

    virtual ~AscmNode() {};

    virtual uint32_t ShiftSize(uint32_t start, bool aligned) const {
        return start; // empty by default
    }

    virtual bool Write(AscmCompilerContext& ctx) {
        // nothing by default
        return true;
    }
};

class AscmNodeOpCode : public AscmNode {
public:
    OPCode opcode;

    AscmNodeOpCode(OPCode opcode) : opcode(opcode) {
        nodetype = ASCMNT_OPCODE;
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            return utils::Aligned<uint16_t>(start) + sizeof(uint16_t);
        }
        return start + 1;
    }

    bool Write(AscmCompilerContext& ctx) override {
        auto [ok, op] = GetOpCodeId(ctx.vmInfo->vm, ctx.plt, opcode);
        if (!ok) {
            LOG_ERROR("Can't find opcode {} ({}) for vm {}/{}", OpCodeName(opcode), (int)opcode, ctx.vmInfo->name, PlatformName(ctx.plt));
            
            return false;
        }

        ctx.Align<uint16_t>();
        if (ctx.HasAlign()) {
            ctx.Write<uint16_t>(op);
        }
        else {
            ctx.Write<byte>((byte)op);
        }

        return true;
    }
};

template<typename Type>
class AscmNodeData : public AscmNodeOpCode {
public:
    Type val;

    AscmNodeData(Type val, OPCode opcode) : AscmNodeOpCode(opcode), val(val) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            return utils::Aligned<Type>(AscmNodeOpCode::ShiftSize(start, aligned)) + sizeof(Type);
        }
        return AscmNodeOpCode::ShiftSize(start, aligned) + sizeof(Type);
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Align<Type>();
        ctx.Write<Type>(val);
        return true;
    }

    uint32_t GetDataFLoc(bool aligned) const {
        return ShiftSize(floc, aligned) - sizeof(Type);
    }
};

enum FuncCallFlags {
    FCF_THREAD = 0x1,
    FCF_CHILDTHREAD = 0x2,
    FCF_METHOD = 0x4,
    FCF_POINTER = 0x8,
    FCF_POINTER_CLASS = 0x10,
};

class AscmNodeFunctionCall : public AscmNodeOpCode {
public:
    uint64_t nameSpace;
    uint64_t clsName;
    int flags;
    byte params;

    AscmNodeFunctionCall(OPCode opcode, int flags, byte params, uint64_t clsName, uint64_t nameSpace) 
        : AscmNodeOpCode(opcode), flags(flags), params(params), clsName(clsName), nameSpace(nameSpace) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            if (flags & FCF_POINTER_CLASS) {
                return utils::Aligned<uint32_t>(AscmNodeOpCode::ShiftSize(start, aligned) + 1) + sizeof(uint32_t);
            }
            if (flags & FCF_POINTER) {
                return AscmNodeOpCode::ShiftSize(start, aligned) + 1;
            }
            return utils::Aligned<uint64_t>(AscmNodeOpCode::ShiftSize(start, aligned) + 1) + sizeof(uint64_t);
        }
        throw std::runtime_error("later");
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Write<byte>(params);

        if (flags & FCF_POINTER_CLASS) {
            ctx.Align<uint32_t>();
            ctx.Write<uint32_t>((uint32_t)clsName);
        }
        else if (!(flags & FCF_POINTER)) {
            // replaced by the linker
            ctx.Align<uint64_t>();
            ctx.Write<uint64_t>(utils::CatLocated((uint32_t)nameSpace, (uint32_t)clsName));
        }

        return true;
    }
};

class AscmNodeVariable : public AscmNodeOpCode {
public:
    size_t varId;

    AscmNodeVariable(size_t varId, OPCode opcode) : AscmNodeOpCode(opcode), varId(varId) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        return AscmNodeOpCode::ShiftSize(start, aligned) + 1;
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Write<byte>((byte)(ctx.lvars - varId - 1));
        return true;
    }
};

class AscmNodeCreateLocalVariables : public AscmNodeOpCode {
    std::vector<FunctionVar> vars{};
public:
    AscmNodeCreateLocalVariables(const FunctionVar* lvars, size_t count) : AscmNodeOpCode(OPCODE_SafeCreateLocalVariables) {
        vars.reserve(count);
        for (size_t i = 0; i < count; i++) {
            vars.push_back(lvars[i]);
        }
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        uint32_t e = AscmNodeOpCode::ShiftSize(start, aligned) + 1;
        for (size_t i = 0; i < vars.size(); i++) {
            e = utils::Aligned<uint32_t>(e) + 4 + 1;
        }
        return e;
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Write<byte>((byte)vars.size());
        for (FunctionVar& var : vars) {
            ctx.Align<uint32_t>();
            ctx.Write<uint32_t>((uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
            ctx.Write<byte>(var.flags);
        }

        return true;
    }
};

class AscmNodeGlobalVariable : public AscmNodeOpCode {
public:
    GlobalVariableDef* def;

    AscmNodeGlobalVariable(GlobalVariableDef* def, bool ref) : 
        AscmNodeOpCode(ref ? (def->getRefOpCode ? def->getRefOpCode : OPCODE_GetGlobalObject)
                        : (def->getOpCode ? def->getOpCode : OPCODE_GetGlobal)), def(def) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        return AscmNodeOpCode::ShiftSize(start, aligned) + 2;
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Write<uint16_t>(0); // added by the linker, remove if def using opcode todo??
        return true;
    }

    uint32_t GetDataFLoc(bool aligned) const {
        return ShiftSize(floc, aligned) - sizeof(uint16_t);
    }
};

class AscmNodeLazyLink : public AscmNodeOpCode {
public:
    uint64_t path;
    uint32_t nsp;
    uint32_t func;
    AscmNodeLazyLink(uint64_t path, uint32_t nsp, uint32_t func) : AscmNodeOpCode(OPCode::OPCODE_T8C_GetLazyFunction), path(path), func(func), nsp(nsp) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            return utils::Aligned<uint32_t>(AscmNodeOpCode::ShiftSize(start, aligned)) + 16;
        }
        return AscmNodeOpCode::ShiftSize(start, aligned) + 16;
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        ctx.Align<uint32_t>();
        ctx.Write<uint32_t>(nsp);
        ctx.Write<uint32_t>(func);
        ctx.Write<uint64_t>(path);
        return true;
    }
};

/*
 * Compute the node using the minimum amount of bits
 * @return node
 */
AscmNodeOpCode* BuildAscmNodeData(int64_t val) {
    if (val == 0) {
        return new AscmNodeOpCode(OPCODE_GetZero);
    }
    if (val > 0) {
        if (val < 256) {
            return new AscmNodeData<uint8_t>((uint8_t)val, OPCODE_GetByte);
        }
        if (val < 65536) {
            return new AscmNodeData<uint16_t>((uint16_t)val, OPCODE_GetUnsignedShort);
        }
        if (val < 4294967295) {
            return new AscmNodeData<uint32_t>((uint32_t)val, OPCODE_GetUnsignedInteger);
        }
    } else {
        if (val > -256) {
            return new AscmNodeData<uint8_t>((uint8_t)(-val), OPCODE_GetNegByte);
        }
        if (val > -65536) {
            return new AscmNodeData<uint16_t>((uint16_t)(-val), OPCODE_GetNegUnsignedShort);
        }
        if (val >= -4294967295) {
            return new AscmNodeData<uint32_t>((uint32_t)(-val), OPCODE_GetNegUnsignedInteger);
        }
    }

    return new AscmNodeData<int64_t>((int64_t)val, OPCODE_GetLongInteger);
}

#define IS_RULE_TYPE(rule, index) (rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
#define IS_TERMINAL_TYPE(term, index) (term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)

int64_t NumberNodeValue(ParseTree* number) {
    if (IS_RULE_TYPE(number, gscParser::RuleNumber)) {
        return NumberNodeValue(number->children[0]);
    }
    if (number->getTreeType() != TREE_TERMINAL) {
        return 0; // wtf?
    }

    TerminalNode* term = dynamic_cast<TerminalNode*>(number);
    switch (term->getSymbol()->getType()) {
    case gscParser::INTEGER10:
        return std::strtoll(term->getText().c_str(), NULL, 10);
    case gscParser::INTEGER16: {
        bool neg = term->getText()[0] == '-';
        int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 16);
        return neg ? -val : val;
    }
    case gscParser::INTEGER8: {
        bool neg = term->getText()[0] == '-';
        int64_t val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), NULL, 8);
        return neg ? -val : val;
    }
    case gscParser::INTEGER2: {
        bool neg = term->getText()[0] == '-';
        int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 2);
        return neg ? -val : val;
    }
    default:
        return 0;
    }
}

class AscmNodeJump : public AscmNodeOpCode {
public:
    const AscmNode* location;
    AscmNodeJump(const AscmNode* location, OPCode opcode) : AscmNodeOpCode(opcode), location(location) {
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            return utils::Aligned<int16_t>(AscmNodeOpCode::ShiftSize(start, aligned)) + sizeof(int16_t);
        }
        return AscmNodeOpCode::ShiftSize(start, aligned) + sizeof(int16_t);
    }

    bool Write(AscmCompilerContext& ctx) override {
        if (!AscmNodeOpCode::Write(ctx)) {
            return false;
        }

        int32_t delta = location->rloc - (int32_t)ShiftSize(rloc, ctx.HasAlign());

        if (delta >= MAX_JUMP || delta <= -MAX_JUMP) {
            LOG_ERROR("Max delta size");
            return false;
        }
        
        // write jump
        ctx.Align<int16_t>();
        ctx.Write<int16_t>((int16_t)delta);

        return true;
    }
};

struct PreProcessorOption {
    std::unordered_set<std::string> defines{};
};

class GscCompilerOption {
public:
    bool m_help{};
    bool m_computeDevOption{};
    const char* m_preproc{};
    VmInfo* m_vmInfo{};
    Platform m_platform{ Platform::PLATFORM_PC };
    const char* m_outFileName{ "compiled"};
    std::vector<const char*> m_inputFiles{};
    bool m_client{};
    int64_t crcServer{};
    int64_t crcClient{};
    const char* nameServer{ "" };
    const char* nameClient{ "" };
    PreProcessorOption processorOpt{};

    bool Compute(const char** args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            const char* arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
            }
            else if (!strcmp("-p", arg) || !_strcmpi("--platform", arg)) {
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
            else if (!strcmp("-g", arg) || !_strcmpi("--game", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                VmInfo* out{};

                if (!IsValidVm(VMOf(args[++i]), out)) {
                    LOG_ERROR("Unknown game: {}!", args[i]);
                    return false;
                }

                m_vmInfo = out;
            }
            else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_outFileName = args[++i];
            }
            else if (!strcmp("-d", arg) || !_strcmpi("--dbg", arg)) {
                m_computeDevOption = true;
            }
            else if (!_strcmpi("--preproc", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                m_preproc = args[++i];
            }
            else if (!strcmp("-c", arg) || !_strcmpi("--csc", arg)) {
                m_client = true;
            }
            else if (!_strcmpi("--crc", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                try {
                    crcServer = std::strtoll(args[++i], nullptr, 16);
                }
                catch (std::exception& e) {
                    LOG_ERROR("Invalid crc for {}: {} / {}!", arg, args[i], e.what());
                    return false;
                }
            }
            else if (!_strcmpi("--crc-client", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                try {
                    crcClient = std::strtoll(args[++i], nullptr, 16);
                }
                catch (std::exception& e) {
                    LOG_ERROR("Invalid crc for {}: {} / {}!", arg, args[i], e.what());
                    return false;
                }
            }
            else if (!_strcmpi("--name", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                nameServer = args[++i];
            }
            else if (!_strcmpi("--name-client", arg)) {
                if (i + 1 == endIndex) {
                    LOG_ERROR("Missing value for param: {}!", arg);
                    return false;
                }
                nameClient = args[++i];
            }
            else if (*arg == '-') {
                if (arg[1] == 'D' && arg[2]) {
                    processorOpt.defines.insert(arg + 2);
                    continue;
                }

                LOG_ERROR("Unknown option: {}!", arg);
                return false;
            }
            else {
                m_inputFiles.push_back(arg);
            }
        }
        if (!m_inputFiles.size()) {
            m_inputFiles.push_back(".");
        }
        if (!m_vmInfo) {
            LOG_WARNING("No game set, please set a game using --game [game]");
            return false;
        }
        return true;
    }

    void PrintHelp() {
        LOG_INFO("-h --help          : Print help");
        LOG_INFO("-g --game [g]      : Set game");
        LOG_INFO("-p --platform [p]  : Set platform");
        LOG_INFO("-d --dbg           : Add dev options");
        LOG_INFO("-o --output [f]    : Set output file (without extension), default: 'compiled'");
        LOG_INFO("-D[name]           : Define variable");
        LOG_INFO("-c --csc           : Build client script with csc files");
        LOG_INFO("--crc [c]          : Set the crc for the server script");
        LOG_INFO("--crc-client [c]   : Set the crc for the client script");
        LOG_INFO("--name [n]         : Set the name for the server script");
        LOG_INFO("--name-client [n]  : Set the name for the client script");
        LOG_DEBUG("--preproc [f]      : Export preproc result into f");
    }
};  

enum GscFileType {
    FILE_GSC,
    FILE_CSC
};

class GscFile {
public:
    const std::filesystem::path& filename;
    size_t start;
    size_t startLine;
    size_t sizeLine;
    std::string buffer{};
};

struct InputInfo {
    std::vector<GscFile> files{};
    std::string gscData{};


    const GscFile& FindFile(size_t line) {
        for (auto& f : files) {
            if (line >= f.startLine && line < f.startLine + f.sizeLine) {
                return f;
            }
        }
        return files[files.size() - 1];
    }

    void PrintLineMessage(alogs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) {
        const auto& f = FindFile(line);
        

        if (charPositionInLine) {
            LOG_LVL(lvl, "{}#{}:{} {}", f.filename.string(), (f.startLine < line ? (line - f.startLine) : f.sizeLine), charPositionInLine, msg);
        }
        else {
            LOG_LVL(lvl, "{}#{} {}", f.filename.string(), (f.startLine < line ? (line - f.startLine) : f.sizeLine), msg);
        }
    }
    Token* GetToken(ParseTree* tree) {
        while (tree && tree->getTreeType() != TREE_TERMINAL && tree->children.size()) {
            tree = tree->children[0];
        }

        if (tree && tree->getTreeType() == TREE_TERMINAL) {
            return dynamic_cast<TerminalNode*>(tree)->getSymbol();
        }
        return nullptr;
    }
    void PrintLineMessage(alogs::loglevel lvl, ParseTree* tree, const std::string& msg) {
        Token* token = GetToken(tree);
        if (token) {
            PrintLineMessage(lvl, token->getLine(), token->getCharPositionInLine(), msg);
            return;
        }

        alogs::log(lvl, msg);
    }
};

class RefObject {
public:
    uint32_t location{};
    std::vector<AscmNodeData<uint32_t>*> nodes{};
};
class ImportObject {
public:
    byte flags;
    byte params;
    std::vector<AscmNode*> nodes{};
};
class GlobalVarObject {
public:
    tool::gsc::opcode::GlobalVariableDef* def{};
    std::vector<AscmNodeGlobalVariable*> nodes{};
};

struct FunctionJumpLoc {
    AscmNode* node{};
    ParseTree* def{};
    bool defined{};
};

class FunctionObject {
public:
    CompileObject& obj;
    int64_t autoexecOrder{};
    uint64_t m_name;
    uint64_t m_name_space;
    uint64_t m_data_name;
    byte m_params{};
    byte m_flags{};
    size_t location{};
    size_t rndVarStart{};
    FunctionVar m_vars[256]{};
    size_t m_allocatedVar{};
    std::vector<AscmNode*> m_nodes{};
    std::stack<AscmNode*> m_jumpBreak{};
    std::stack<AscmNode*> m_jumpContinue{};
    std::unordered_map<std::string, FunctionJumpLoc> m_jumpLocs{};
    VmInfo* m_vmInfo;
    FunctionObject(
        CompileObject& obj,
        uint64_t name,
        uint64_t name_space,
        VmInfo* vmInfo
    ) : obj(obj), m_name(name), m_name_space(name_space), m_data_name(name_space), m_vmInfo(vmInfo) {
    }
    ~FunctionObject() {
        for (auto* node : m_nodes) {
            delete node;
        }
    }

    AscmNode* CreateParamNode() const {
        if (!m_allocatedVar) {
            return new AscmNodeOpCode(OPCODE_CheckClearParams);
        }

        return new AscmNodeCreateLocalVariables(m_vars, m_allocatedVar);
    }

    const AscmNode* PeekBreakNode() const {
        if (m_jumpBreak.size()) {
            return m_jumpBreak.top();
        }
        return nullptr;
    }

    const AscmNode* PeekContinueNode() const {
        if (m_jumpContinue.size()) {
            return m_jumpContinue.top();
        }
        return nullptr;
    }

    void PushBreakNode(AscmNode* node) {
        m_jumpBreak.push(node);
    }

    void PopBreakNode() {
        m_jumpBreak.pop();
    }

    void PushContinueNode(AscmNode* node) {
        m_jumpContinue.push(node);
    }

    void PopContinueNode() {
        m_jumpContinue.pop();
    }

    inline FunctionVar* VarEnd() {
        return &m_vars[m_allocatedVar];
    }

    void AddNode(ParseTree* tree, AscmNode* node);
    void AddNode(decltype(m_nodes)::iterator it, ParseTree* tree, AscmNode* node);


    /*
     * Find a variable with its name
     * @param name name
     * @return iterator from m_vars
     */
    FunctionVar* FindVar(const std::string& name) {
        return std::find_if(std::begin(m_vars), VarEnd(), [&name](const FunctionVar& var) -> bool { return name == var.name; });
    }


    /*
     * Register a variable with a name
     * @param name name
     * @param allowExisting return existing variables
     * @param flags flags
     * @return pair with an error message (for errors) or the variable
     */
    std::pair<const char*, FunctionVar*> RegisterVar(const std::string& name, bool allowExisting, byte flags = 0);

    /*
     * Register a variable with a random name
     * @return same as RegisterVar
     */
    std::pair<const char*, FunctionVar*> RegisterVarRnd() {
        return RegisterVar(std::format("$$v{:x}", rndVarStart++), false);
    }

    /*
     * Compute the nodes relative locations
     * @return -1 in case of error, the size otherwise
     */
    int32_t ComputeRelativeLocations(int32_t floc) {
        // we start at 0 and we assume that the start location is already aligned
        int32_t current{};
        bool align = m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_SHORT);

        for (auto node : m_nodes) {
            node->rloc = current;
            node->floc = align ? utils::Aligned<uint16_t>(floc + current) : (floc + current);
            current = node->ShiftSize(current, align);
            if (node->rloc > current) {
                return -1;
            }
        }
        return current;
    }
};


class CompileObject {
public:
    GscCompilerOption& opt;
    InputInfo& info;
    GscFileType type;
    uint64_t fileName;
    uint64_t currentNamespace;
    int64_t crc;
    std::set<uint64_t> includes{};
    std::unordered_map<uint64_t, FunctionObject> exports{};
    std::unordered_map<std::string, RefObject> strings{};
    std::unordered_map<Located, std::vector<ImportObject>, LocatedHash, LocatedEquals> imports{};
    std::unordered_map<std::string, GlobalVarObject> globals{};
    VmInfo* vmInfo;
    Platform plt;
    int64_t autoexecOrder{};
    std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler;
    std::unordered_set<std::string> hashes{};

    CompileObject(GscCompilerOption& opt, GscFileType file, InputInfo& nfo, VmInfo* vmInfo, Platform plt, std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler) 
        : opt(opt), type(file), info(nfo), vmInfo(vmInfo), plt(plt), gscHandler(gscHandler) {
        currentNamespace = vmInfo->HashField("");
    }

    uint64_t AddInclude(std::string& data) {
        if (!(data.ends_with(".gsc") || data.ends_with(".csc")) && !(data.starts_with("script_"))) {
            switch (type) {
            case FILE_CSC:
                data += ".csc";
                break;
            case FILE_GSC:
                data += ".gsc";
                break;
            }
        }
        hashes.insert(data);
        includes.insert(vmInfo->HashPath(data.data()));
        return 0;
    }

    bool HasOpCode(OPCode opcode) {
        auto [ok, op] = GetOpCodeId(vmInfo->vm, plt, opcode);
        return ok;
    }

    bool Compile(std::vector<byte>& data) {
        if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
            constexpr const char* name = "$notif_checkum";
            uint64_t nameHashed = vmInfo->HashField(name);
            hashes.insert(name);
            auto [res, err] = exports.try_emplace(nameHashed, *this, nameHashed, currentNamespace, vmInfo);

            if (!err) {
                LOG_ERROR("Can't register notif checksum export: {}", name);
                return false;
            }

            FunctionObject& f = res->second;
            f.autoexecOrder = INT64_MIN; // first
            f.m_flags = tool::gsc::CLASS_VTABLE;
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CheckClearParams));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));
            f.m_nodes.push_back(BuildAscmNodeData(crc));

            auto gvarIt = vmInfo->globalvars.find(vmInfo->HashField("level"));

            if (gvarIt == vmInfo->globalvars.end()) {
                LOG_ERROR("Can't find level def for checksum export: {}", name);
                return false;
            }

            auto& gv = gvarIt->second;

            auto& decl = globals[gv.name];

            if (!decl.def) {
                decl.def = &gv;
            }

            auto* gvar = new AscmNodeGlobalVariable(decl.def, false);
            decl.nodes.emplace_back(gvar);
            f.m_nodes.push_back(gvar);
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Notify));
            f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
        }


        utils::Allocate(data, gscHandler->GetHeaderSize());
        size_t afterHeaderStart{ data.size() };
        if (opt.m_computeDevOption) {
            utils::Allocate(data, sizeof(tool::gsc::acts_debug::GSC_ACTS_DEBUG));
        }

        LOG_TRACE("Compile {} include(s)...", includes.size());
        size_t incTable = utils::Allocate(data, sizeof(uint64_t) * includes.size());

        uint64_t* tab = reinterpret_cast<uint64_t*>(&data[incTable]);

        for (uint64_t i : includes) {
            *tab = i;
            tab++;
        }

        size_t expTable = utils::Allocate(data, gscHandler->GetExportSize() * exports.size());

        size_t csegOffset = data.size();

        LOG_TRACE("Compile {} export(s)...", exports.size());

        size_t exportIndex{};

        auto& that = *this;
        auto writeExport = [&data, &that, &expTable, &exportIndex](FunctionObject& exp) -> bool {
            if (exp.m_nodes.empty()) {
                LOG_ERROR("No nodes for {:x}", exp.m_name);
                return false;
            }

            struct PreExp { uint64_t top; uint64_t bottom; };

            utils::Aligned<PreExp>(data);
            utils::Allocate(data, sizeof(uint64_t));

            int32_t len = exp.ComputeRelativeLocations((int32_t)data.size());
            if (len < 0) {
                LOG_ERROR("Error when allocating relative locations");
                return false;
            }

            exp.location = data.size();
            tool::gsc::IW23GSCExport e{};
            e.name = exp.m_name;
            e.name_space = exp.m_name_space;
            e.file_name_space = exp.m_data_name;
            e.flags = that.gscHandler->MapFlagsExportToInt(exp.m_flags);
            e.address = (int32_t)exp.location;
            e.param_count = exp.m_params;
            e.checksum = 0x12345678;
            that.gscHandler->WriteExport(&data[expTable + that.gscHandler->GetExportSize() * exportIndex++], e);

            AscmCompilerContext cctx{ that.vmInfo, that.plt, exp.m_allocatedVar, data };

            for (auto* node : exp.m_nodes) {
                if (!node->Write(cctx)) {
                    return false;
                }
            }
            return true;
        };

        std::vector<FunctionObject*> autoexecs{};

        for (auto& [name, exp] : exports) {
            if (exp.m_flags & tool::gsc::T8GSCExportFlags::AUTOEXEC) {
                autoexecs.push_back(&exp);
            }
        }

        // sort the autoexecs by ids and write them first

        std::sort(autoexecs.begin(), autoexecs.end(), [](auto& f1, auto& f2) -> bool { return f1->autoexecOrder < f2->autoexecOrder; });

        bool exportsOk{ true };
        for (auto* exp : autoexecs) {
            if (!writeExport(*exp)) {
                exportsOk = false;
            }
        }

        for (auto& [name, exp] : exports) {
            if (exp.m_flags & tool::gsc::T8GSCExportFlags::AUTOEXEC) {
                continue; // skip autoexecs
            }

            if (!writeExport(exp)) {
                exportsOk = false;
            }
        }

        if (!exportsOk) {
            return false;
        }

        size_t csegSize = data.size() - csegOffset;

        LOG_TRACE("Compile {} strings(s)...", strings.size());

        // compile strings

        for (auto& [key, strobj] : strings) {
            // TODO: check vm, in mwiii it's not the same
            strobj.location = (uint32_t)data.size();
            if (vmInfo->vm == VM_T8) {
                data.push_back(0x9f);
                data.push_back((byte)(key.length() + 1));
            }
            utils::WriteString(data, key.c_str());
        }

        size_t stringRefs = data.size();
        size_t stringCount{};

        for (auto& [key, strobj] : strings) {
            size_t w{};
            while (w < strobj.nodes.size()) {
                if (w % 0xFF == 0) {
                    size_t buff = utils::Allocate(data, gscHandler->GetStringSize());
                    tool::gsc::T8GSCString str{};
                    str.string = strobj.location;
                    str.type = 0;
                    str.num_address = (byte)((strobj.nodes.size() - w) > 0xFF ? 0xFF : (strobj.nodes.size() - w));
                    gscHandler->WriteString(&data[buff], str);
                    stringCount++;
                }
                utils::WriteValue<uint32_t>(data, strobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_OPCODE_SHORT)));
            }
        }

        size_t gvarRefs = data.size();
        size_t gvarCount{};
        if (gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
            LOG_TRACE("Compile {} global(s)...", globals.size());

            for (auto& [key, gvobj] : globals) {
                size_t w{};
                while (w < gvobj.nodes.size()) {
                    if (w % 0xFF == 0) {
                        size_t buff = utils::Allocate(data, gscHandler->GetGVarSize());
                        tool::gsc::T8GSCGlobalVar gv{};
                        hashes.insert(key);
                        gv.name = (uint32_t)vmInfo->HashField(key.c_str());
                        gv.num_address = (byte)((gvobj.nodes.size() - w) > 0xFF ? 0xFF : (gvobj.nodes.size() - w));
                        gscHandler->WriteGVar(&data[buff], gv);
                        gvarCount++;
                    }
                    utils::WriteValue<uint32_t>(data, gvobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_OPCODE_SHORT)));
                }
            }
        }

        LOG_TRACE("Compile {} import(s)...", imports.size());
        size_t implRefs = data.size();
        size_t implCount{};

        for (auto& [key, imobj] : imports) {
            uint64_t nsp = key.name_space;
            uint64_t func = key.name;
            for (ImportObject& implData : imobj) {
                size_t w{};
                while (w < implData.nodes.size()) {
                    if (w % 0xFFFF == 0) {
                        size_t buff = utils::Allocate(data, gscHandler->GetImportSize());
                        tool::gsc::IW23GSCImport gv{};
                        gv.name_space = nsp;
                        gv.name = func;
                        gv.param_count = implData.params;
                        gv.flags = gscHandler->MapFlagsImportToInt(implData.flags);
                        gv.num_address = (byte)((implData.nodes.size() - w) > 0xFFFF ? 0xFFFF : (implData.nodes.size() - w));
                        gscHandler->WriteImport(&data[buff], gv);
                        implCount++;
                    }
                    utils::WriteValue<uint32_t>(data, implData.nodes[w++]->floc);
                }
            }
        }


        if (opt.m_computeDevOption) {
            LOG_TRACE("Add {} hashe(s)...", hashes.size());
            size_t hashesLoc = utils::Allocate(data, sizeof(uint32_t) * hashes.size());

            size_t hashesIdx{};

            for (const auto& h : hashes) {
                reinterpret_cast<uint32_t*>(&data[hashesLoc])[hashesIdx++] = (uint32_t)data.size();
                utils::WriteString(data, h.c_str());
            }

            auto* debug_obj = reinterpret_cast<tool::gsc::acts_debug::GSC_ACTS_DEBUG*>(data.data() + afterHeaderStart);

            *reinterpret_cast<uint64_t*>(debug_obj->magic) = tool::gsc::acts_debug::MAGIC;
            debug_obj->version = tool::gsc::acts_debug::CURRENT_VERSION;
            debug_obj->actsVersion = (uint64_t) actsinfo::VERSION_ID;
            debug_obj->strings_count = (uint32_t)hashes.size();
            debug_obj->strings_offset = (uint32_t)hashesLoc;
        }

        // compile header
        gscHandler->file = data.data();
        gscHandler->SetName(fileName);
        gscHandler->SetChecksum(crc);
        gscHandler->SetHeader();

        gscHandler->SetIncludesCount((int16_t)includes.size());
        gscHandler->SetIncludesOffset((int32_t)incTable);

        gscHandler->SetStringsCount((int16_t)stringCount);
        gscHandler->SetStringsOffset((int32_t)stringRefs);

        if (gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
            gscHandler->SetGVarsCount((int16_t)gvarCount);
            gscHandler->SetGVarsOffset((int32_t)gvarRefs);
        }

        gscHandler->SetExportsCount((int16_t)exports.size());
        gscHandler->SetExportsOffset((int32_t)expTable);
        gscHandler->SetCSEGOffset((int32_t)csegOffset);
        gscHandler->SetCSEGSize((int32_t)csegSize);

        gscHandler->SetImportsCount((int16_t)implCount);
        gscHandler->SetImportsOffset((int32_t)implRefs);

        gscHandler->SetFileSize((int32_t)data.size());

        return true;
    }
};


std::pair<const char*, FunctionVar*> FunctionObject::RegisterVar(const std::string& name, bool allowExisting, byte flags) {
    static FunctionVar badVar{ "$BAD_VAR", 0, 0 };
    auto it = FindVar(name);
    if (it != VarEnd()) {
        if (allowExisting) {
            return std::make_pair<>(nullptr, &*it);
        }
        return std::make_pair<>(utils::va("The var '%s' already exists", name.c_str()), &badVar);
    }

    if (m_allocatedVar >= ARRAYSIZE(m_vars)) {
        return std::make_pair<>(utils::va("Can't create var '%s': too much variable for function", name.c_str()), &badVar);
    }

    auto& var = m_vars[m_allocatedVar] = { name, m_allocatedVar, flags };
    m_allocatedVar++;
    obj.hashes.insert(var.name);

    return std::make_pair<>(nullptr, &var);
}

void FunctionObject::AddNode(ParseTree* tree, AscmNode* node) {
    Token* token = obj.info.GetToken(tree);
    if (token) {
        node->line = token->getLine();
    }
    m_nodes.push_back(node);
}

void FunctionObject::AddNode(decltype(m_nodes)::iterator it, ParseTree* tree, AscmNode* node) {
    Token* token = obj.info.GetToken(tree);
    if (token) {
        node->line = token->getLine();
    }
    m_nodes.insert(it, node);
}

bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal);

bool ParseFieldNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj) {
    if (!exp) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "empty tree error");
        return false;
    }
    if (exp->getTreeType() == TREE_ERROR) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "detected tree error, bad syntax?");
        return false;
    }
    if (exp->getTreeType() == TREE_RULE) {
        RuleContext* rule = dynamic_cast<RuleContext*>(exp);



        // search that we don't have a children in error, after that we'll assume the tree is right for a depth of 2
        if (std::find_if(rule->children.begin(), rule->children.end(), [](const ParseTree* tree) -> bool { return tree->getTreeType() == TREE_ERROR; })
            != rule->children.end()) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "detected tree children error, bad syntax?");
            return false;
        }

        switch (rule->getRuleIndex()) {
        case gscParser::RuleExpression:
        case gscParser::RuleExpression1:
        case gscParser::RuleExpression2:
        case gscParser::RuleExpression3:
        case gscParser::RuleExpression4:
        case gscParser::RuleExpression5:
        case gscParser::RuleExpression6:
        case gscParser::RuleExpression7:
        case gscParser::RuleExpression8:
        case gscParser::RuleExpression9:
        case gscParser::RuleExpression10:
        case gscParser::RuleExpression11:
        case gscParser::RuleExpression12:
        case gscParser::RuleExpression13: {
            if (rule->children.size() == 1) {
                return ParseFieldNode(rule->children[0], parser, obj, fobj);
            }
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }
        case gscParser::RuleExpression14: {
            if (rule->children.size() == 1) { // exp
                return ParseFieldNode(rule->children[0], parser, obj, fobj);
            }
            if (rule->children.size() == 3) { // ( exp )
                return ParseFieldNode(rule->children[1], parser, obj, fobj);
            }
            obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown expr14 rule type: {}", exp->getText()));
            return false;
        }
        case gscParser::RuleObject_left_value:
        case gscParser::RuleArray_left_value: {
            ParseTree* value;
            size_t startOp;
            if (IS_RULE_TYPE(rule->children[0], gscParser::RuleConst_expr)) {
                value = rule->children[0];
                startOp = 1;
            }
            else if (IS_RULE_TYPE(rule->children[1], gscParser::RuleExpression)) {
                value = rule->children[1];
                startOp = 3;
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown object/array type: {}", exp->getText()));
                return false;
            }

            if (rule->getRuleIndex() == gscParser::RuleObject_left_value) {
                // object
                if (IS_TERMINAL_TYPE(rule->children[startOp + 1], gscParser::IDENTIFIER)) {
                    if (!ParseExpressionNode(value, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object id: {}", exp->getText()));
                        return false;
                    }
                    std::string fieldText = rule->children[startOp + 1]->getText();
                    obj.hashes.insert(fieldText);

                    if (fieldText == "size") {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format(".size can't be used as a lvalue: {}", exp->getText()));
                        return false;
                    }
                    else {
                        fobj.AddNode(rule->children[startOp + 1], new AscmNodeOpCode(OPCODE_CastFieldObject));
                        // use identifier
                        uint64_t hash = obj.vmInfo->HashField(fieldText);
                        fobj.AddNode(rule->children[startOp + 1], new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_EvalFieldVariableRef));
                    }
                }
                else {
                    if (!ParseExpressionNode(rule->children[startOp + 2], parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object canon id: {}", exp->getText()));
                        return false;
                    }
                    fobj.AddNode(rule->children[startOp + 2], new AscmNodeOpCode(OPCODE_CastCanon));

                    if (!ParseExpressionNode(value, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object id (from canon): {}", exp->getText()));
                        return false;
                    }
                    fobj.AddNode(value, new AscmNodeOpCode(OPCODE_CastFieldObject));
                    fobj.AddNode(value, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStackRef));
                }
                return true;

            }
            if (rule->getRuleIndex() == gscParser::RuleArray_left_value) {
                // array 
                if (!ParseExpressionNode(rule->children[startOp + 1], parser, obj, fobj, true)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse array key: {}", exp->getText()));
                    return false;
                }

                if (!ParseFieldNode(value, parser, obj, fobj)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse array node: {}", exp->getText()));
                    return false;
                }

                fobj.AddNode(value, new AscmNodeOpCode(OPCODE_EvalArrayRef));
                return true;
            }

            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown object/array rule: {} ({})", exp->getText(), rule->getRuleIndex()));
            return false;
        }
        case gscParser::RuleLeft_value: {
            if (rule->children.size() == 1) {
                return ParseFieldNode(rule->children[0], parser, obj, fobj);
            }

            if (rule->children[0]->getTreeType() == TREE_RULE) {
                RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                if (first->getRuleIndex() == gscParser::RuleLeft_value) {
                    /*
                        | left_value '.' (IDENTIFIER | ('(' expression ')')) -> test.test2 test.("test2")
                        | left_value '[' expression ']'
                        -> test[...]
                    */
                    std::string second = rule->children[1]->getText();

                    if (second == ".") {
                        // object access
                        if (IS_TERMINAL_TYPE(rule->children[2], gscParser::IDENTIFIER)) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();
                            obj.hashes.insert(fieldText);

                            if (fieldText == "size") {
                                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format(".size can't be used as a lvalue: {}", exp->getText()));
                                return false;
                            }
                            else {
                                // use identifier
                                uint64_t hash = obj.vmInfo->HashField(fieldText);
                                fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_CastFieldObject));
                                fobj.AddNode(rule->children[2], new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_EvalFieldVariableRef));
                            }
                        }
                        else {
                            if (!ParseExpressionNode(rule->children[3], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule->children[3], new AscmNodeOpCode(OPCODE_CastCanon));

                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(first, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(first, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStackRef));
                        }
                        return true;
                    }
                    else if (second == "[") {
                        // array access

                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            return false;
                        }

                        if (!ParseFieldNode(first, parser, obj, fobj)) {
                            return false;
                        }

                        fobj.AddNode(first, new AscmNodeOpCode(OPCODE_EvalArrayRef));
                        return true;
                    }
                    else {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown lvalue type: {}", exp->getText()));
                    return false;
                }
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown lvalue tree type: {}", exp->getText()));
                return false;
            }
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unhandled lvalue type: {}", exp->getText()));
            return false;
        }
        case gscParser::RuleConst_expr:
        case gscParser::RuleNumber:
        case gscParser::RuleVector_value:
        case gscParser::RuleSet_expression:
        case gscParser::RuleArray_def:
        case gscParser::RuleStruct_def:
        case gscParser::RuleFunction_ref:
        case gscParser::RuleStatement:
        case gscParser::RuleStatement_if:
        case gscParser::RuleStatement_while:
        case gscParser::RuleStatement_dowhile:
        case gscParser::RuleStatement_for:
        case gscParser::RuleStatement_switch:
        case gscParser::RuleStatement_foreach:
        case gscParser::RuleStatement_inst:
        case gscParser::RuleNop_def:
        case gscParser::RuleStatement_block:
        case gscParser::RuleOperator_inst:
        case gscParser::RuleFunction_call:
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }


        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("unhandled lvalue rule: {} ({})", rule->getText(), rule->getRuleIndex()));
        return false;
    }

    if (exp->getTreeType() != TREE_TERMINAL) {
        obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown tree type: {}", exp->getText()));
        return false;
    }
    TerminalNode* term = dynamic_cast<TerminalNode*>(exp);


    auto len = term->getText().size();

    switch (term->getSymbol()->getType()) {
    case gscParser::IDENTIFIER: {
        std::string varName = term->getText();

        if (varName == "self") {
            obj.info.PrintLineMessage(alogs::LVL_WARNING, term, "self can't be used as a lvalue");
            return false;
        }

        auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

        if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
            auto& gv = gvarIt->second;

            obj.info.PrintLineMessage(alogs::LVL_WARNING, term, std::format("the {} global variable can't be used as a lvalue", gv.name));
            return false;
        }
        auto [varerr, keyVarL] = fobj.RegisterVar(varName, true, 0);

        if (varerr) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, term, varerr);
            return false;
        }

        fobj.AddNode(term, new AscmNodeVariable(keyVarL->id, OPCODE_EvalLocalVariableRefCached));
        return true;
    }
    case gscParser::UNDEFINED_VALUE:
    case gscParser::BOOL_VALUE:
    case gscParser::FLOATVAL:
    case gscParser::INTEGER10:
    case gscParser::INTEGER16:
    case gscParser::INTEGER8:
    case gscParser::INTEGER2:
    case gscParser::HASHSTRING:
    case gscParser::STRING:
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("not a valid lvalue: {}", term->getText()));
        return false;
    }

    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unhandled lvalue terminal: {}", term->getText()));
    return false;
}
bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal) {
    if (!exp) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "empty tree error");
        return false;
    }
    if (exp->getTreeType() == TREE_ERROR) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "detected tree error, bad syntax?");
        return false;
    }

    if (exp->getTreeType() == TREE_RULE) {
        auto* rule = dynamic_cast<RuleContext*>(exp);

        // search that we don't have a children in error, after that we'll assume the tree is right for a depth of 2
        if (std::find_if(rule->children.begin(), rule->children.end(), [](const ParseTree* tree) -> bool { return tree->getTreeType() == TREE_ERROR; })
            != rule->children.end()) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "detected tree children error, bad syntax?");
            return false;
        }

        switch (rule->getRuleIndex()) {
        case gscParser::RuleStatement: {
            if (expressVal) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't express a statement");
                return false;
            }

            if (rule->children.size() > 1) {
                // define location

                std::string locName = rule->children[0]->getText();

                auto& loc = fobj.m_jumpLocs[locName];

                if (loc.defined) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("the location {} was defined twice!", locName));
                    return false;
                }

                loc.defined = true;

                if (!loc.node) {
                    loc.node = new AscmNode();
                }

                fobj.AddNode(rule, loc.node);
            }

            return ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, expressVal);
        }
        case gscParser::RuleStatement_if: {
            bool ok{ true };
            auto* elseStart = new AscmNode();
            if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.AddNode(rule->children[2], new AscmNodeJump(elseStart, OPCODE_JumpOnFalse));

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                ok = false;
            }


            if (rule->children.size() > 5) { // else
                auto* endElse = new AscmNode();
                fobj.AddNode(rule->children[5], new AscmNodeJump(endElse, OPCODE_Jump));
                fobj.AddNode(rule->children[6], elseStart);

                if (!ParseExpressionNode(rule->children[6], parser, obj, fobj, false)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[rule->children.size() - 1], endElse);
            }
            else {
                fobj.AddNode(rule->children[rule->children.size() - 1], elseStart);
            }

            return ok;
        }
        case gscParser::RuleStatement_while: {
            bool ok{ true };
            AscmNode* loopBreak = new AscmNode();
            AscmNode* loopContinue = new AscmNode();

            fobj.PushContinueNode(loopContinue);
            fobj.PushBreakNode(loopBreak);

            fobj.AddNode(rule, loopContinue);

            if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.AddNode(rule->children[2], new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                ok = false;
            }

            // TODO: add next node
            fobj.AddNode(rule->children[4], new AscmNodeJump(loopContinue, OPCODE_Jump));

            fobj.AddNode(rule->children[4], loopBreak);

            fobj.PopContinueNode();
            fobj.PopBreakNode();

            return ok;
        }
        case gscParser::RuleStatement_dowhile: {
            bool ok{ true };
            AscmNode* loopStart = new AscmNode();
            AscmNode* loopBreak = new AscmNode();
            AscmNode* loopContinue = new AscmNode();

            fobj.PushContinueNode(loopContinue);
            fobj.PushBreakNode(loopBreak);

            fobj.AddNode(rule, loopStart);

            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, false)) {
                ok = false;
            }

            fobj.AddNode(rule, loopContinue);

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.AddNode(rule->children[4], new AscmNodeJump(loopStart, OPCODE_JumpOnTrue));

            // TODO: add next node
            fobj.AddNode(rule->children[4], loopBreak);

            fobj.PopContinueNode();
            fobj.PopBreakNode();

            return ok;
        }
        case gscParser::RuleStatement_for: {
            bool err{};
            size_t i = 2;

            // init expression
            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                err = true;
            }

            i++; // skip ';'
            AscmNode* loopStart = new AscmNode();
            AscmNode* loopBreak = new AscmNode();
            AscmNode* loopContinue = new AscmNode();

            fobj.AddNode(rule, loopStart);


            // if expression
            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression)) {
                if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                    err = true;
                }
                else {
                    fobj.AddNode(rule->children[i - 1], new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));
                }
            }

            i++; // skip ';'

            fobj.PushContinueNode(loopContinue);
            fobj.PushBreakNode(loopBreak);

            if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                err = true;
            }

            fobj.PopContinueNode();
            fobj.PopBreakNode();

            // delta expression
            fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);

            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                err = true;
            }

            fobj.AddNode(rule->children[i - 1], new AscmNodeJump(loopStart, OPCODE_Jump));
            fobj.AddNode(rule->children[i - 1], loopBreak);

            return !err;
        }
        case gscParser::RuleStatement_switch: {
            auto [err, var] = fobj.RegisterVarRnd();

            if (err) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering switch variable: {}", err));
                return false;
            }

            /*
              var = expression1

              if (var == case1) goto LOC_1
              if (var == case2) goto LOC_2
              ...
              goto LOC_DEF
              LOC_1:
                ...
              LOC_2:
                ...
                ...
              LOC_DEF:
                ...
              LOC_ENDSWITCH:
                ...


             */
            bool ok{ true };

            if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.AddNode(rule->children[2], new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
            fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_SetVariableField));


            // statement_switch: 'switch' '(' expression ')' '{' (('case' const_expr | 'default') ':' (statement)*)+'}';

            struct JumpData {
                size_t startStmt;
                size_t endStmt;
                AscmNode* jmpNode;
            };

            // build jump table
            std::vector<JumpData> jmpTable{};
            JumpData defaultCase{ 0, 0, nullptr };

            bool findDefault{};
            for (size_t i = 5; i < rule->children.size() - 1;) {
                auto caseType = rule->children[i]->getText();

                if (caseType == "case") {
                    i++; // 'case'
                    AscmNode* jmpNode = new AscmNode();

                    if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    fobj.AddNode(rule->children[i], new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(rule->children[i], new AscmNodeOpCode(OPCODE_Equal));


                    fobj.AddNode(rule->children[i], new AscmNodeJump(jmpNode, OPCODE_JumpOnTrue));

                    i += 2; // const_expr ':'

                    size_t start = i;
                    while (IS_RULE_TYPE(rule->children[i], gscParser::RuleStatement)) {
                        i++;
                    }
                    jmpTable.emplace_back(start, i, jmpNode);

                    continue;
                }

                if (caseType == "default") {
                    if (defaultCase.jmpNode) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[i], "A switch can't have more than one default block");
                        ok = false;
                    }
                    else {
                        defaultCase.jmpNode = new AscmNode();
                    }
                    i += 2; // 'default' ':'
                    defaultCase.startStmt = i;
                    while (IS_RULE_TYPE(rule->children[i], gscParser::RuleStatement)) {
                        i++;
                    }
                    defaultCase.endStmt = i;
                    jmpTable.emplace_back(defaultCase);
                    continue;
                }

                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[i], std::format("Unknown case type: {}/{}", caseType, i));
                ok = false;
                i++;
            }

            // push content
            AscmNode* endSwitchNode = new AscmNode();
            fobj.PushBreakNode(endSwitchNode);

            if (defaultCase.jmpNode) {
                fobj.AddNode(rule, new AscmNodeJump(defaultCase.jmpNode, OPCODE_Jump));
            }
            else {
                fobj.AddNode(rule, new AscmNodeJump(endSwitchNode, OPCODE_Jump));
            }

            for (auto& caseElem : jmpTable) {
                fobj.AddNode(rule->children[caseElem.startStmt], caseElem.jmpNode);

                for (size_t stmt = caseElem.startStmt; stmt < caseElem.endStmt; stmt++) {
                    if (!ParseExpressionNode(rule->children[stmt], parser, obj, fobj, false)) {
                        ok = false;
                    }
                }
            }

            fobj.AddNode(rule, endSwitchNode);

            fobj.PopBreakNode();

            return ok;
        }
        case gscParser::RuleStatement_foreach: {
            auto [var1err, arrayVal] = fobj.RegisterVarRnd();
            auto [var2err, iteratorVal] = fobj.RegisterVarRnd();

            if (var1err) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach variable: {}", var1err));
                return false;
            }

            if (var2err) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach iterator variable: {}", var2err));
                return false;
            }

            // foreach (key, value in level.var_4afb8f5a) {

            FunctionVar* keyVar;
            FunctionVar* valueVar;

            if (IS_TERMINAL_TYPE(rule->children[4], gscParser::IDENTIFIER)) {
                // key,value in
                auto [var3err, keyVarL] = fobj.RegisterVar(rule->children[2]->getText(), true);
                auto [var4err, valueVarL] = fobj.RegisterVar(rule->children[4]->getText(), true);

                if (var3err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach key variable: {}", var3err));
                    return false;
                }
                if (var4err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach value variable: {}", var4err));
                    return false;
                }

                keyVar = keyVarL;
                valueVar = valueVarL;
            }
            else {
                // value in 
                auto [var3err, keyVarL] = fobj.RegisterVarRnd();
                auto [var4err, valueVarL] = fobj.RegisterVar(rule->children[2]->getText(), true);

                if (var3err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach key variable: {}", var3err));
                    return false;
                }
                if (var4err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach value variable: {}", var4err));
                    return false;
                }

                keyVar = keyVarL;
                valueVar = valueVarL;
            }

            bool ok{ true };

            ParseTree* arrVal = rule->children[rule->children.size() - 3];

            if (!ParseExpressionNode(arrVal, parser, obj, fobj, true)) {
                ok = false;
            }

            uint64_t forEachType = obj.gscHandler->buildFlags & tool::gsc::GOHF_FOREACH_TYPE_MASK;

            switch (forEachType) {
            case tool::gsc::GOHF_FOREACH_TYPE_T8: {
                // array = ...;
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));
                // key = firstarray(array);
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_FirstArrayKeyCached));
                fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));

                AscmNode* loopBreak = new AscmNode();
                AscmNode* loopContinue = new AscmNode();
                AscmNode* loopIt = new AscmNode();

                fobj.PushBreakNode(loopBreak);
                fobj.PushContinueNode(loopContinue);

                fobj.AddNode(arrVal, loopIt);
                // jumpiffalse(isdefined(key)) loopBreak
                fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_IsDefined));
                fobj.AddNode(arrVal, new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

                // value = array[key];
                fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_EvalArray));
                fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));

                // nextarray(array, key, iterator)
                fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_SetNextArrayKeyCached));

                if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                // key = iterator;
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeOpCode(OPCODE_SetVariableField));

                // loop back
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                fobj.PopBreakNode();
                fobj.PopContinueNode();
                // end
                fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);
            }
                break;
            case tool::gsc::GOHF_FOREACH_TYPE_T9: {
                auto [varnexterr, nextVar] = fobj.RegisterVarRnd();

                if (varnexterr) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("error when registering foreach next variable: {}", varnexterr));
                    ok = false;
                }

                // array = ...;
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_SetVariableField));
                // iterator = firstarray(array);
                fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_FirstArrayKey));
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_SetLocalVariableCached));

                AscmNode* loopBreak = new AscmNode();
                AscmNode* loopContinue = new AscmNode();
                AscmNode* loopIt = new AscmNode();

                fobj.PushBreakNode(loopBreak);
                fobj.PushContinueNode(loopContinue);

                fobj.AddNode(arrVal, loopIt);
                // jumpiffalse(isdefined(iterator)) loopBreak
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_IsDefined));
                fobj.AddNode(arrVal, new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));
                
                // key = iteratorkey(iterator);
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorKey));
                fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

                // val = iteratorval(iterator);
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorVal));
                fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_SetLocalVariableCached));

                // next = iteratornext(iterator);
                fobj.AddNode(arrVal, new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_T9_IteratorNext));
                fobj.AddNode(arrVal, new AscmNodeVariable(nextVar->id, OPCODE_SetLocalVariableCached));

                if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                // key = iterator;
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(nextVar->id, OPCODE_EvalLocalVariableCached));
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableRefCached));
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeOpCode(OPCODE_SetVariableField));

                // loop back
                fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                fobj.PopBreakNode();
                fobj.PopContinueNode();
                // end
                fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);
            }
                break;
            default:
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("foreach not implemented for vm {} (0x{:x})", obj.vmInfo->name, forEachType));
                ok = false;
                break;
            }

            return ok;
        }
        case gscParser::RuleNop_def: {
            if (expressVal) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't express a nop value");
                return false;
            }
            size_t count{ 1 };
            if (rule->children.size() > 1) {
                int64_t val{ NumberNodeValue(rule->children[2]) };
                if (val < 0) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't define a negative amount of nop");
                    return false;
                }
                count = (size_t)val;
            }

            for (size_t i = 0; i < count; i++) {
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Nop));
            }
            
            return true;
        }
        case gscParser::RuleStatement_inst: {
            if (rule->children.size() == 1) {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't express this value");
                    return false;
                }
                return true; // empty instruction
            }
            return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
        }
        case gscParser::RuleStatement_block: {
            if (expressVal) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't express this value");
                return false;
            }
            bool ok{ true };
            for (size_t i = 1; i < rule->children.size() - 1; i++) {
                auto* stmt = rule->children[i];

                if (!ParseExpressionNode(stmt, parser, obj, fobj, false)) {
                    ok = false; // do not return false so we can have multiple issues
                }
            }
            return ok;
        }
        case gscParser::RuleOperator_inst: {
            if (expressVal) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't express this value");
                return false;
            }
            auto idf = rule->children[0]->getText();

            if (idf == "break") {
                if (rule->children.size() > 1) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't specify jump location with break");
                    return false;
                }

                const AscmNode* loc = fobj.PeekBreakNode();

                if (!loc) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "not in a break context");
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                return true;
            }

            if (idf == "continue") {
                if (rule->children.size() > 1) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't specify jump location with continue");
                    return false;
                }

                const AscmNode* loc = fobj.PeekContinueNode();

                if (!loc) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "not in a continue context");
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                return true;
            }

            if (idf == "goto") {
                if (rule->children.size() <= 1 && !IS_TERMINAL_TYPE(rule->children[1], gscParser::IDENTIFIER)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "goto should be used with a jump location");
                    return false;
                }

                auto& loc = fobj.m_jumpLocs[rule->children[1]->getText()];
                if (!loc.node) {
                    loc.node = new AscmNode();
                }
                if (!loc.def) {
                    loc.def = rule;
                }

                fobj.AddNode(rule, new AscmNodeJump(loc.node, OPCODE_Jump));
                return true;
            }

            if (idf == "return") {
                if (rule->children.size() <= 1) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_End));
                    return true;
                }

                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Return));
                return true;
            }
            
            if (idf == "wait") {
                if (rule->children.size() <= 1) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "wait should be used with a value");
                    return false;
                }

                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Wait));
                return true;
            }
            if (idf == "waitframe") {
                if (rule->children.size() <= 1) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "wait should be used with a value");
                    return false;
                }

                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_WaitFrame));
                return true;
            }

            obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unknown operator type {}", idf));
            return false;
        }
        case gscParser::RuleFunction_call: {
            uint64_t funcNspHash{};
            uint64_t funcHash{};
            ParseTree* ptrTree{};
            ParseTree* selfTree{};

            int flags{};
            byte importFlags{};
            size_t idx{};
            if (rule->children[idx]->getTreeType() == TREE_RULE && !IS_RULE_TYPE(rule->children[idx], gscParser::RuleFunction_component)) {
                selfTree = rule->children[idx];
                flags |= FCF_METHOD;
                idx++;
            }
            if (rule->children[idx]->getTreeType() == TREE_TERMINAL) {
                std::string callModifier = rule->children[idx]->getText();
                if (callModifier == "thread") {
                    flags |= FCF_THREAD;
                }
                else if (callModifier == "childthread") {
                    flags |= FCF_CHILDTHREAD;
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[idx], std::format("Unknown call modifier {}", callModifier));
                    return false;
                }
                idx++;
            }
            ParseTree* functionComp = rule->children[idx];

            if (!IS_RULE_TYPE(functionComp, gscParser::RuleFunction_component)) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Not a function component {}", functionComp->getText()));
                return false;
            }

            auto paramsList = rule->children[rule->children.size() - 2];
            if (!IS_RULE_TYPE(paramsList, gscParser::RuleExpression_list)) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Not a params list {}", paramsList->getText()));
                return false;
            }

            if (functionComp->children.size() == 1) {
                // func
                importFlags |= tool::gsc::T8GSCImportFlags::GET_CALL;
                funcNspHash = obj.currentNamespace;
                std::string funcName = functionComp->children[0]->getText();

                funcHash = obj.vmInfo->HashField(funcName.c_str());

                auto funcIt = obj.vmInfo->opFuncs.find(funcHash);

                if (funcIt != obj.vmInfo->opFuncs.end()) {
                    // internal function call
                    auto& f = funcIt->second;
                    if (f.HasFlag(tool::gsc::opcode::VPFD_SELF_PARAM) && !(flags & FCF_METHOD)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' should have a caller, Usage: {}", funcName, f.usage));
                        return false;
                    }
                    if (expressVal && !f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' doesn't return a value, Usage: {}", funcName, f.usage));
                        return false;
                    }
                    if (flags & (FCF_THREAD | FCF_CHILDTHREAD)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' can't have a thread modifier, Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (f.HasFlag(tool::gsc::opcode::VPFD_USE_PRE_SCRIPT_CALL)) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_PreScriptCall));
                    }

                    bool paramError{};
                    int paramCount{};
                    for (int i = (int)paramsList->children.size() - 1; i >= 0; i -= 2) {
                        if (!ParseExpressionNode(paramsList->children[i], parser, obj, fobj, true)) {
                            paramError = true;
                        }
                        paramCount++;
                    }

                    // add self
                    if (flags & FCF_METHOD) {
                        if (!ParseExpressionNode(selfTree, parser, obj, fobj, true)) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, "Error when parsing caller");
                            return false;
                        }
                    }

                    if (paramError) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "Error when parsing param list");
                        return false;
                    }

                    if (paramCount < f.minParam) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Not enought params for operator '{}', Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (paramCount > f.maxParam) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Too many params for operator '{}', Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (f.HasFlag(tool::gsc::opcode::VPFD_USE_COUNT)) {
                        fobj.AddNode(rule, new AscmNodeData<byte>((byte)paramCount, f.opCode));
                    }
                    else {
                        fobj.AddNode(rule, new AscmNodeOpCode(f.opCode));
                    }

                    if (!expressVal && f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                    }

                    return true;
                }
                obj.hashes.insert(funcName);
            }
            else if (functionComp->children.size() == 3) {
                    // namespace::func

                    std::string funcNspName = functionComp->children[0]->getText();
                    std::string funcName = functionComp->children[2]->getText();

                    obj.hashes.insert(funcNspName);
                    obj.hashes.insert(funcName);
                    funcNspHash = obj.vmInfo->HashField(funcNspName.c_str());
                    funcHash = obj.vmInfo->HashField(funcName.c_str());
            }
            else if (functionComp->children.size() == 5) {
                // [ [ espression ] ]
                ptrTree = functionComp->children[2];
                flags |= FCF_POINTER;
            }
            else if (functionComp->children.size() == 7) {
                if (flags & FCF_METHOD) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, "A class call can't have a self caller");
                    return false;
                }
                // [ [ espression ] ] -> func
                std::string funcName = functionComp->children[6]->getText();
                obj.hashes.insert(funcName);
                funcHash = obj.vmInfo->HashField(funcName.c_str());

                ptrTree = functionComp->children[2];
                flags |= FCF_POINTER_CLASS | FCF_POINTER;
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Function call not implemented {}", functionComp->getText()));
                return false;
            }

            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_PreScriptCall));
            
            // push params
            bool paramError{};
            int paramCount{};
            for (int i = (int)paramsList->children.size() - 1; i >= 0; i -= 2) {
                if (!ParseExpressionNode(paramsList->children[i], parser, obj, fobj, true)) {
                    paramError = true;
                }
                paramCount++;
            }

            // add self
            if (flags & FCF_METHOD) {
                if (!ParseExpressionNode(selfTree, parser, obj, fobj, true)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, "Error when parsing caller");
                    return false;
                }
            }

            // add ptr
            if (flags & FCF_POINTER) {
                if (!ParseExpressionNode(ptrTree, parser, obj, fobj, true)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, "Error when parsing pointer");
                    return false;
                }
            }

            if (paramError) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "Error when parsing param list");
                return false;
            }

            if (paramCount >= 256) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "Too many parameters for call");
                return false;
            }

            OPCode opcode;
            if (flags & FCF_POINTER) {
                if (flags & FCF_POINTER_CLASS) {

                    if (flags & FCF_THREAD) {
                        opcode = OPCODE_ClassFunctionThreadCall;
                    }
                    else if (flags & FCF_CHILDTHREAD) {
                        opcode = OPCODE_ClassFunctionThreadCallEndOn;
                    }
                    else {
                        opcode = OPCODE_ClassFunctionCall;
                    }
                }
                else {
                    if (flags & FCF_METHOD) {
                        if (flags & FCF_THREAD) {
                            opcode = OPCODE_ScriptMethodThreadCallPointer;
                        }
                        else if (flags & FCF_CHILDTHREAD) {
                            opcode = OPCODE_ScriptMethodThreadCallPointerEndOn;
                        }
                        else {
                            opcode = OPCODE_ScriptMethodCallPointer;
                        }
                    }
                    else {
                        if (flags & FCF_THREAD) {
                            opcode = OPCODE_ScriptThreadCallPointer;
                        }
                        else if (flags & FCF_CHILDTHREAD) {
                            opcode = OPCODE_ScriptThreadCallPointerEndOn;
                        }
                        else {
                            opcode = OPCODE_ScriptFunctionCallPointer;
                        }
                    }
                }
                fobj.AddNode(rule, new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash));
            }
            else {
                if (flags & FCF_METHOD) {
                    if (flags & FCF_THREAD) {
                        opcode = OPCODE_ScriptMethodThreadCall;
                        importFlags |= tool::gsc::T8GSCImportFlags::METHOD_THREAD;
                    }
                    else if (flags & FCF_CHILDTHREAD) {
                        opcode = OPCODE_ScriptMethodThreadCallEndOn;
                        importFlags |= tool::gsc::T8GSCImportFlags::METHOD_CHILDTHREAD;
                    }
                    else {
                        opcode = OPCODE_ScriptMethodCall;
                        importFlags |= tool::gsc::T8GSCImportFlags::METHOD;
                    }
                }
                else {
                    if (flags & FCF_THREAD) {
                        opcode = OPCODE_ScriptThreadCall;
                        importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION_THREAD;
                    }
                    else if (flags & FCF_CHILDTHREAD) {
                        opcode = OPCODE_ScriptThreadCallEndOn;
                        importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION_CHILDTHREAD;
                    }
                    else {
                        opcode = OPCODE_ScriptFunctionCall;
                        importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION;
                    }
                }

                auto* funcCall = new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash);

                // link by the game, but we write it for test
                Located located{ funcNspHash, funcHash };

                auto& impList = obj.imports[located];

                auto it = std::find_if(impList.begin(), impList.end(), [flags](const auto& e) { return e.flags == flags; });

                if (it == impList.end()) {
                    // no equivalent, we need to create our own node
                    impList.emplace_back(importFlags, (byte)paramCount).nodes.push_back(funcCall);
                }
                else {
                    // same local/flags, we can add our node
                    it->nodes.push_back(funcCall);
                }

                fobj.AddNode(rule, funcCall);
            }


            if (!expressVal) {
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
            }
            return true;
        }
        case gscParser::RuleExpression:
        case gscParser::RuleExpression1:
        case gscParser::RuleExpression2:
        case gscParser::RuleExpression3:
        case gscParser::RuleExpression4:
        case gscParser::RuleExpression5:
        case gscParser::RuleExpression6:
        case gscParser::RuleExpression7:
        case gscParser::RuleExpression8:
        case gscParser::RuleExpression9:
        case gscParser::RuleExpression10:
        case gscParser::RuleExpression11:
        case gscParser::RuleExpression12: {
            if (rule->children.size() == 1) {
                // simple rules recursion
                return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
            }
            if (rule->children.size() == 2) {
                // (++|--|~|!) exp
                if (rule->children[0]->getTreeType() == TREE_TERMINAL) {
                    // ++/--/~/!

                    auto op = rule->children[0]->getText();
                    if (op == "!") {
                        if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                            return false;
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolNot));

                        if (!expressVal) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                        }
                    }
                    else if (op == "~") {
                        if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                            return false;
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolComplement));

                        if (!expressVal) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                        }
                    }
                    else if (op == "++") {
                        // ++var
                        if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                            return false; // can't parse field
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Inc));
                        if (expressVal) {
                            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                return false;
                            }
                        }
                    }
                    else if (op == "--") {
                        // --var
                        if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                            return false; // can't parse field
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Dec));
                        if (expressVal) {
                            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                                return false;
                            }
                        }
                    }
                    else {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[0], std::format("unhandled operator: {}", op));
                        return false;
                    }
                }
                else {
                    // ++/--

                    auto op = rule->children[1]->getText();
                    if (op == "++") {
                        // var++
                        if (expressVal) {
                            if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                                return false; // can't parse field
                            }
                        }
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                            return false; // can't parse field
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Inc));
                    }
                    else if (op == "--") {
                        // var--
                        if (expressVal) {
                            if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                                return false; // can't parse field
                            }
                        }
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                            return false; // can't parse field
                        }
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Dec));
                    }
                    else {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled operator: {}", op));
                        return false;
                    }
                }

                return true;
            }
            if (rule->children.size() == 5) {
                // e ? e : e
                bool ok{ true };

                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }

                AscmNode* caseNo = new AscmNode();
                fobj.AddNode(rule, new AscmNodeJump(caseNo, OPCODE_JumpOnFalse));

                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                AscmNode* caseEnd = new AscmNode();
                fobj.AddNode(rule, new AscmNodeJump(caseEnd, OPCODE_Jump));

                fobj.AddNode(rule->children[3], caseNo);

                if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[4], caseEnd);

                return ok;
            }
            if (rule->children.size() != 3) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("unknown expression, excepted 3 children: {}", rule->getText()));
                return false;
            }

            auto op = rule->children[1]->getText();

            if (op == "||" || op == "&&") {
                // boolean operators are defined using jumps, we need to handle them
                // push op left
                bool ok{ true };
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }
                AscmNode* after = new AscmNode();

                fobj.AddNode(rule, new AscmNodeJump(after, op == "&&" ? OPCODE_JumpOnFalseExpr : OPCODE_JumpOnTrueExpr));

                // push op right
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                // after the operator
                fobj.AddNode(rule->children[2], after);

                if (!expressVal) {
                    fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_DecTop));
                }
                return ok;
            }
            else {
                // push operands
                bool ok{ true };
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                if (op == "|") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Or));
                }
                else if (op == "^") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Xor));
                }
                else if (op == "&") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_And));
                }
                else if (op == "!=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_NotEqual));
                }
                else if (op == "!==") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SuperNotEqual));
                }
                else if (op == "==") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Equal));
                }
                else if (op == "===") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SuperEqual));
                }
                else if (op == "<") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_LessThan));
                }
                else if (op == "<=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_LessThanOrEqualTo));
                }
                else if (op == ">") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GreaterThan));
                }
                else if (op == ">=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_GreaterThanOrEqualTo));
                }
                else if (op == "+") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Plus));
                }
                else if (op == "-") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Minus));
                }
                else if (op == "*") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Multiply));
                }
                else if (op == "/") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Divide));
                }
                else if (op == "%") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Modulus));
                }
                else if (op == "<<") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftLeft));
                }
                else if (op == ">>") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftRight));
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled operator: {}", op));
                    ok = false;
                }
                if (!expressVal) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                }
                return ok;
            }
        }
            break;
        case gscParser::RuleConst_expr:
        case gscParser::RuleNumber:
        case gscParser::RuleExpression13:
        case gscParser::RuleExpression14:
            return ParseExpressionNode(rule->children[rule->children.size() == 3 ? 1 : 0], parser, obj, fobj, expressVal);
        case gscParser::RuleVector_value:
            if (!expressVal) { // no need to create vector
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("ignored useless value: {}", rule->getText()));
                return true;
            }
            if (
                !ParseExpressionNode(rule->children[5], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[3], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[1], parser, obj, fobj, true)
                ) {
                return false;
            }
            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Vector));
            return true;
        case gscParser::RuleArray_def: {
            if (!expressVal) { // no need to create array
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("ignored useless value: {}", rule->getText()));
                return true;
            }
            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateArray));

            // 1 = [ (HASHSTR|'#' number) ':'
            bool ok{ true };
            size_t currentKey{};
            for (size_t i = 1; i + 1 < rule->children.size();) {
                if (rule->children[i + 1]->getText() == ":") {
                    // key ':'

                    auto* expTree = rule->children[i++];

                    i++; // ':'

                    if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    if (!ParseExpressionNode(expTree, parser, obj, fobj, true)) {
                        ok = false;
                    }
                }
                else {
                    if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    // push current key
                    fobj.AddNode(rule->children[i - 1], BuildAscmNodeData(currentKey++));
                }

                fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToArray));

                i++; // ',' or ']'
            }

            return ok;
        }
        case gscParser::RuleStruct_def: {
            if (!expressVal) { // no need to create struct
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("ignored useless value: {}", rule->getText()));
                return true;
            }
            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateStruct));

            bool ok{ true };
            for (size_t i = 1; i < rule->children.size() - 1;) {

                ParseTree* term = rule->children[i++];

                std::string termStr = term->getText();

                std::string sub = termStr.substr(1, termStr.size() - 1);

                i++; // ':'


                if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[i - 1], new AscmNodeData<uint64_t>(obj.vmInfo->HashField(sub.c_str()), OPCODE_GetHash));

                fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToStruct));

                i++; // ',' or '}'
            }
            return ok;
        }
        case gscParser::RuleSet_expression: {
            std::string opVal = rule->children[1]->getText();

            bool ok{ true };
            if (opVal == "=") {
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                    ok = false;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));
                if (expressVal) {
                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                }
                return ok;
            }
            else {
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }
                if (opVal == "+=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Plus));
                }
                else if (opVal == "-=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Minus));
                }
                else if (opVal == "/=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Divide));
                }
                else if (opVal == "*=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Multiply));
                }
                else if (opVal == "%=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Modulus));
                }
                else if (opVal == "&=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_And));
                }
                else if (opVal == "|=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Or));
                }
                else if (opVal == "^=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Bit_Xor));
                }
                else if (opVal == "<<=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftLeft));
                }
                else if (opVal == ">>=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ShiftRight));
                }
                else if (opVal == "~=") {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_BoolComplement));
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled set operator: {}", opVal));
                    ok = false;
                }

                // dogshit way, but it seems to be the same in their compiler...
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj)) {
                    ok = false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SetVariableField));
                if (expressVal) {
                    if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                }
                return ok;
            }
            return false;
        }
        case gscParser::RuleFunction_ref: {
            if (rule->children.size() == 2) {
                if (IS_TERMINAL_TYPE(rule->children[1], gscParser::IDENTIFIER) ||
                    (IS_RULE_TYPE(rule->children[1], gscParser::RuleLeft_value) && IS_TERMINAL_TYPE(rule->children[1]->children[0], gscParser::IDENTIFIER))
                    ) {
                    if (obj.HasOpCode(OPCODE_T9_GetVarRef)) {
                        // single idf ref
                        // &var ?
                        std::string varName = rule->children[1]->getText();

                        FunctionVar* var = fobj.FindVar(varName);

                        if (var != fobj.VarEnd()) {
                            fobj.AddNode(rule, new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T9_GetVarRef));
                            return true;
                        }
                    }
                }
                else if (IS_RULE_TYPE(rule->children[1], gscParser::RuleLeft_value)) {
                    if (!ParseFieldNode(rule->children[1], parser, obj, fobj)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], "Can't express field ref");
                        return false;
                    }
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_T9_GetVarRef));
                    return true;
                }
            }
            if (rule->children.size() == 7) {
                // @nsp<path>::func
                std::string nsp = rule->children[1]->getText();
                std::string path = rule->children[3]->getText();
                std::string funcName = rule->children[6]->getText();

                obj.hashes.insert(nsp);
                obj.hashes.insert(path);
                obj.hashes.insert(funcName);
                
                fobj.AddNode(rule, new AscmNodeLazyLink(
                    obj.vmInfo->HashPath(path.c_str()),
                    (uint32_t)obj.vmInfo->HashField(nsp.c_str()),
                    (uint32_t)obj.vmInfo->HashField(funcName.c_str())
                ));
                return true;
            }
            // &nsp::func || &func
            uint64_t nsp = obj.currentNamespace;

            byte flags = tool::gsc::T8GSCImportFlags::FUNC_METHOD;

            if (rule->children.size() == 4) {
                // with nsp
                std::string nspStr = rule->children[1]->getText();
                obj.hashes.insert(nspStr);
                nsp = obj.vmInfo->HashField(nspStr.c_str());
            }
            else {
                flags |= tool::gsc::T8GSCImportFlags::GET_CALL;
            }

            assert(rule->children.size());

            std::string funcStr = rule->children[rule->children.size() - 1]->getText();
            obj.hashes.insert(funcStr);
            uint64_t func = obj.vmInfo->HashField(funcStr.c_str());

            // link by the game, but we write it for test
            AscmNode* asmc;
            if (obj.gscHandler->HasFlag(tool::gsc::GOHF_INLINE_FUNC_PTR)) {
                asmc = new AscmNodeData<uint64_t>(0, OPCODE_GetResolveFunction);
                fobj.AddNode(rule, asmc);
            }
            else {
                // in mwiii, unlike in t8 where the pointers are inlined into the bytecode, 
                // the game links resolved functions using 4 bytes for script functions or 
                // 2 bytes for builtin functions so we use nops so the game does whatever it want
                asmc = new AscmNodeOpCode(OPCODE_GetResolveFunction);
                fobj.AddNode(rule, asmc);

                for (size_t i = 0; i < 4; i++) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Nop));
                }
            }

            Located located{ nsp, func };
            auto& impList = obj.imports[located];

            auto it = std::find_if(impList.begin(), impList.end(), [flags](const auto& e) { return e.flags == flags; });

            if (it == impList.end()) {
                // no equivalent, we need to create our own node
                impList.emplace_back(flags, 0).nodes.push_back(asmc);
            }
            else {
                // same local/flags, we can add our node
                it->nodes.push_back(asmc);
            }

            return true;
        }
        case gscParser::RuleObject_left_value:
        case gscParser::RuleArray_left_value: {
            ParseTree* value;
            size_t startOp;
            if (IS_RULE_TYPE(rule->children[0], gscParser::RuleConst_expr)) {
                value = rule->children[0];
                startOp = 1;
            }
            else if (IS_RULE_TYPE(rule->children[1], gscParser::RuleExpression)) {
                value = rule->children[1];
                startOp = 3;
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown object/array type: {}", exp->getText()));
                return false;
            }

            if (rule->getRuleIndex() == gscParser::RuleObject_left_value) {
                // object
                if (IS_TERMINAL_TYPE(rule->children[startOp + 1], gscParser::IDENTIFIER)) {
                    if (!ParseExpressionNode(value, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object id: {}", exp->getText()));
                        return false;
                    }
                    std::string fieldText = rule->children[startOp + 1]->getText();
                    if (fieldText == "size") {
                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SizeOf));
                    }
                    else {
                        obj.hashes.insert(fieldText);
                        // use identifier
                        uint64_t hash = obj.vmInfo->HashField(fieldText);
                        if (obj.HasOpCode(OPCODE_CastAndEvalFieldVariable)) {
                            fobj.AddNode(rule, new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_CastAndEvalFieldVariable));
                        }
                        else {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(rule, new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_EvalFieldVariable));
                        }
                    }
                }
                else {
                    if (!ParseExpressionNode(rule->children[startOp + 2], parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object canon id: {}", exp->getText()));
                        return false;
                    }
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastCanon));

                    if (!ParseExpressionNode(value, parser, obj, fobj, true)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse object id (from canon): {}", exp->getText()));
                        return false;
                    }
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStack));
                }
                return true;

            }
            if (rule->getRuleIndex() == gscParser::RuleArray_left_value) {
                // array 
                if (!ParseExpressionNode(rule->children[startOp + 1], parser, obj, fobj, true)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse array key: {}", exp->getText()));
                    return false;
                }

                if (!ParseExpressionNode(value, parser, obj, fobj, true)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("can't parse array node: {}", exp->getText()));
                    return false;
                }

                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalArray));
                return true;
            }

            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown object/array rule: {} ({})", exp->getText(), rule->getRuleIndex()));
            return false;
        }
        case gscParser::RuleLeft_value: {
            if (!expressVal) {
                obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("ignored useless value: {}", rule->getText()));
                return true;
            }
            if (rule->children.size() == 1) {
                return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
            }
            else if (rule->children[0]->getTreeType() == TREE_RULE) {
                RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                if (first->getRuleIndex() == gscParser::RuleLeft_value) {
                    /*
                        | left_value '.' (IDENTIFIER | ('(' expression ')')) -> test.test2 test.("test2")
                        | left_value '[' expression ']'
                        -> test[...]
                    */
                    std::string second = rule->children[1]->getText();

                    if (second == ".") {
                        // object access
                        if (IS_TERMINAL_TYPE(rule->children[2], gscParser::IDENTIFIER)) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SizeOf));
                            }
                            else {
                                obj.hashes.insert(fieldText);
                                // use identifier
                                uint64_t hash = obj.vmInfo->HashField(fieldText);
                                if (obj.HasOpCode(OPCODE_CastAndEvalFieldVariable)) {
                                    fobj.AddNode(rule, new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_CastAndEvalFieldVariable));
                                }
                                else {
                                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                                    fobj.AddNode(rule, new AscmNodeData<uint32_t>((uint32_t)hash, OPCODE_EvalFieldVariable));
                                }
                            }
                        }
                        else {
                            if (!ParseExpressionNode(rule->children[3], parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastCanon));

                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalFieldVariableOnStack));
                        }
                        return true;
                    }
                    else if (second == "[") {
                        // array access

                        if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                            return false;
                        }

                        if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                            return false;
                        }

                        fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_EvalArray));
                        return true;
                    }
                    else {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown lvalue type: {}", exp->getText()));
                    return false;
                }
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown lvalue tree type: {}", exp->getText()));
                return false;
            }
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unhandled lvalue type: {}", exp->getText()));
            return false;
        }
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("unhandled rule: {} ({})", rule->getText(), rule->getRuleIndex()));
        return false;
    }

    if (exp->getTreeType() != TREE_TERMINAL) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unknown tree type: {}", exp->getText()));
        return false;
    }

    TerminalNode* term = dynamic_cast<TerminalNode*>(exp);

    if (!expressVal) {
        obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("ignored useless value: {}", term->getText()));
        return true;
    }

    auto len = term->getText().size();

    switch (term->getSymbol()->getType()) {
    case gscParser::IDENTIFIER: {
        std::string varName = term->getText();

        if (varName == "self") {
            fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetSelf));
            return true;
        }

        auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

        if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
            auto& gv = gvarIt->second;
            
            if (gv.getOpCode) {
                obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("opcode gvar not implemented: {}", gv.name));
                return false;
            }

            auto& decl = obj.globals[gv.name];

            if (!decl.def) {
                decl.def = &gv;
            }

            auto* gvar = new AscmNodeGlobalVariable(decl.def, false);
            decl.nodes.emplace_back(gvar);
            fobj.AddNode(term, gvar);
            return true;
        }

        auto varIt = fobj.FindVar(varName);

        if (varIt == fobj.VarEnd()) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, term, std::format("unknown variable: {}", varName));
            return false;
        }

        fobj.AddNode(term, new AscmNodeVariable(varIt->id, OPCODE_EvalLocalVariableCached));
        return true;
    }
    case gscParser::UNDEFINED_VALUE:
        fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetUndefined));
        return true;
    case gscParser::BOOL_VALUE:
        fobj.AddNode(term, BuildAscmNodeData(term->getText() == "true"));
        return true;
    case gscParser::FLOATVAL:
        fobj.AddNode(term, new AscmNodeData<FLOAT>((FLOAT)std::strtof(term->getText().c_str(), NULL), OPCODE_GetFloat));
        return true;
    case gscParser::INTEGER10:
        fobj.AddNode(term, BuildAscmNodeData(std::strtoll(term->getText().c_str(), NULL, 10)));
        return true;
    case gscParser::INTEGER16: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 16);
        fobj.AddNode(term, BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::INTEGER8: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), NULL, 8);
        fobj.AddNode(term, BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::INTEGER2: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 2);
        fobj.AddNode(term, BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::HASHSTRING: {
        auto sub = term->getText().substr(2, len - 3);
        obj.hashes.insert(sub);
        fobj.AddNode(term, new AscmNodeData<uint64_t>(hash::Hash64Pattern(sub.c_str()), OPCODE_GetHash));
        return true;
    }
    case gscParser::STRING: {
        auto node = term->getText();
        auto newStr = std::make_unique<char[]>(node.length() - 1);
        auto* newStrWriter = &newStr[0];

        // format string
        for (size_t i = 1; i < node.length() - 1; i++) {
            if (node[i] != '\\') {
                *(newStrWriter++) = node[i];
                continue; // default case
            }

            i++;

            assert(i < node.length() && "bad format, \\ before end");

            switch (node[i]) {
            case 'n':
                *(newStrWriter++) = '\n';
                break;
            case 't':
                *(newStrWriter++) = '\t';
                break;
            case 'r':
                *(newStrWriter++) = '\r';
                break;
            case 'b':
                *(newStrWriter++) = '\b';
                break;
            default:
                *(newStrWriter++) = node[i];
                break;
            }
        }
        *(newStrWriter++) = 0; // end char

        // link by the game
        auto* asmc = new AscmNodeData<uint32_t>(0x12345678, OPCODE_GetString);
        fobj.AddNode(term, asmc);

        std::string key{ &newStr[0]};

        if (key.length() >= 256) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("string too long: {}", term->getText()));
            return false;
        }

        auto& str = obj.strings[key];
        str.nodes.push_back(asmc);
        return true;
    }
    }

    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("unhandled terminal: {}", term->getText()));
    return false;
}

bool ParseFunction(gscParser::FunctionContext* func, gscParser& parser, CompileObject& obj) {
    if (func->children.size() < 5) { // 0IDF 1( 2params 3) 4block
        obj.info.PrintLineMessage(alogs::LVL_ERROR, func, "Bad function declaration");
        return false;
    }

    auto* nameTerm = func->children[(size_t)(func->children.size() - 5)];
    auto* paramsRule = func->children[(size_t)(func->children.size() - 3)];
    auto* blockRule = func->children[(size_t)(func->children.size() - 1)];

    if (!IS_TERMINAL_TYPE(nameTerm, gscParser::IDENTIFIER)) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, func, "Bad function name declaration");
        return false;
    }

    auto* termNode = static_cast<TerminalNode*>(nameTerm);
    
    auto name = termNode->getText();

    obj.hashes.insert(name);
    uint64_t nameHashed = obj.vmInfo->HashField(name.data());

    auto [res, err] = obj.exports.try_emplace(nameHashed, obj, nameHashed, obj.currentNamespace, obj.vmInfo);

    if (!err) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("The export {} was defined twice", name));
        return false;
    }

    auto& exp = res->second;

    if (!IS_RULE_TYPE(paramsRule, gscParser::RuleParam_list)) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("Bad function {} params declaration {}", name, func->getText()));
        return false;
    }
    if (!IS_RULE_TYPE(blockRule, gscParser::RuleStatement_block)) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("Bad function {} block declaration {}", name, func->getText()));
        return false;
    }

    // handle modifiers

    for (size_t i = 0; i < func->children.size() - 5; i++) {
        auto* mod = func->children[i];
        if (mod->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, mod, std::format("Bad modifier for {}", name));
            return false;
        }

        auto* term = dynamic_cast<TerminalNode*>(mod);

        auto txt = term->getText();

        if (txt == "function") {
            continue; // don't care
        }
        if (txt == "private") {
            exp.m_flags |= tool::gsc::T8GSCExportFlags::PRIVATE;
        }
        else if (txt == "autoexec") {
            exp.m_flags |= tool::gsc::T8GSCExportFlags::AUTOEXEC;
            
            if (IS_RULE_TYPE(func->children[i + 2], gscParser::RuleNumber)) {
                // use user order
                exp.autoexecOrder = NumberNodeValue(func->children[i + 2]);
                i += 3;
            }
            else {
                // use natural order
                exp.autoexecOrder = obj.autoexecOrder++;
            }
        }
        else if (txt == "event_handler") {
            if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_EV_HANDLER)) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, func, "event_handler functions not available for this vm");
                return false;
            }
            exp.m_flags |= tool::gsc::T8GSCExportFlags::EVENT;
            auto* ev = func->children[i += 2];
            i++; // ']'
            if (ev->getTreeType() != TREE_TERMINAL) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, ev, std::format("Bad event for {}", name));
                return false;
            }

            auto evname = static_cast<TerminalNode*>(ev)->getText();

            obj.hashes.insert(evname);
            exp.m_data_name = obj.vmInfo->HashField(evname.data());
        }
    }

    // handle params

    auto* params = dynamic_cast<gscParser::Param_listContext*>(paramsRule);

    size_t index = 0;
    bool varargDetected{};
    for (auto* child : params->children) {
        if (index++ % 2) {
            continue; // coma
        }

        assert(IS_RULE_TYPE(child, gscParser::RuleParam_val));
        auto* param = dynamic_cast<gscParser::Param_valContext*>(child);

        if (varargDetected) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, child, "Can't register param after a vararg");
            return false;
        }

        TerminalNode* idfNode;
        byte idfFlags{};
        if (!IS_TERMINAL_TYPE(param->children[0], gscParser::IDENTIFIER)) {
            if (param->children.size() == 1) {
                // '...'

                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_VA)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, "modifier not available for this vm: vararg...");
                    return false;
                }
                exp.m_flags |= tool::gsc::T8GSCExportFlags::VE;

                std::string paramIdf = "vararg";


                if (exp.m_params == 256) {
                    return utils::va("", name.c_str());
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, std::format("Can't register param '{}': too many params", paramIdf));
                }

                exp.m_params++;

                auto [err, vardef] = exp.RegisterVar(paramIdf, false, tool::gsc::opcode::VARIADIC);
                if (err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, err);
                    return false;
                }

                continue;
            }

            // skip modifier
            idfNode = dynamic_cast<TerminalNode*>(param->children[1]);
            auto modifier = param->children[0]->getText();
            if (modifier == "*") {
                // ptr (T9)
                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_PTR)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not available for this vm: {}", modifier));
                    return false;
                }
                idfFlags = tool::gsc::opcode::T9_VAR_REF;
            }
            else if (modifier == "&") {
                // ref (T8)
                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_REF)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not available for this vm: {}", modifier));
                    return false;
                }
                idfFlags = tool::gsc::opcode::ARRAY_REF;
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not implemented: {}", modifier));
                return false;
            }
        }
        else {
            idfNode = dynamic_cast<TerminalNode*>(param->children[0]);
        }
        auto paramIdf = idfNode->getText();

        if (exp.m_params == 256) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, std::format("Can't register param '{}': too many params", paramIdf));
            return false;
        }

        exp.m_params++;

        auto [err, vardef] = exp.RegisterVar(paramIdf, false, idfFlags);

        if (err) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, err);
            return false;
        }

        if (param->children.size() >= 3) {
            // default value
            ParseTree* defaultValueExp = param->children[param->children.size() - 1];
            assert(IS_RULE_TYPE(defaultValueExp, gscParser::RuleExpression));

            /*
                if (!isdefined(param)) {
                    param = defaultValueExp;
                }
             */
            if (obj.HasOpCode(OPCODE_EvalLocalVariableDefined)) {
                exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableDefined));
            }
            else {
                exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableCached));
                exp.AddNode(defaultValueExp, new AscmNodeOpCode(OPCODE_IsDefined));
            }
            auto* afterNode = new AscmNode();
            exp.AddNode(defaultValueExp, new AscmNodeJump(afterNode, OPCODE_JumpOnTrue));
            if (!ParseExpressionNode(defaultValueExp, parser, obj, exp, true)) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, defaultValueExp, std::format("can't create expression node for variable {}", paramIdf));
                return false;
            }
            exp.AddNode(defaultValueExp, new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableRefCached));
            exp.AddNode(defaultValueExp, new AscmNodeOpCode(OPCODE_SetVariableField));
            exp.AddNode(defaultValueExp, afterNode);
        }

    }

    // handle block

    // weirdly, their gsc compiler is converting top level breaks to jump to the end of the function
    AscmNode* endNode = new AscmNodeOpCode(OPCODE_End);
    exp.PushBreakNode(endNode);

    if (!ParseExpressionNode(blockRule, parser, obj, exp, false)) {
        return false;
    }

    exp.PopBreakNode();

    exp.AddNode(func, endNode);

    bool badRef{};
    for (auto& [name, loc] : exp.m_jumpLocs) {
        if (loc.defined) {
            continue;
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, loc.def ? loc.def : blockRule, std::format("the location {} was used, but isn't declared", name));

        badRef = true;

        if (loc.node) {
            delete loc.node; // free loc
            loc.node = nullptr;
        }
    }
    exp.AddNode(exp.m_nodes.begin(), func, exp.CreateParamNode());

    return !badRef;
}

bool ParseInclude(gscParser::IncludeContext* nsp, gscParser& parser, CompileObject& obj) {
    if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
        return false; // bad
    }

    std::string txt = nsp->children[1]->getText();

    // add the include/using into the includes
    obj.AddInclude(txt);

    return true;
}

bool ParseNamespace(gscParser::NamespaceContext* nsp, gscParser& parser, CompileObject& obj) {
    if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
        return false; // bad
    }

    std::string txt = nsp->children[1]->getText();

    // set the current namespace to the one specified

    obj.hashes.insert(txt);
    obj.currentNamespace = obj.vmInfo->HashField(txt.data());

    return true;
}

bool ParseProg(gscParser::ProgContext* prog, gscParser& parser, CompileObject& obj) {
    if (prog->getTreeType() == TREE_ERROR) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad prog context");
        return false;
    }

    auto* eof = prog->EOF();

    // find the first namespace (used for multifile inputs)
    for (auto& e : prog->children) {
        if (e == eof) {
            break;
        }
        if (e->getTreeType() != TREE_RULE) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad export rule type");
            return false;
        }

        if (dynamic_cast<RuleContext&>(*e).getRuleIndex() == gscParser::RuleNamespace) {
            if (!ParseNamespace(dynamic_cast<gscParser::NamespaceContext*>(e), parser, obj)) {
                return false;
            }
            break;
        }
    }

    for (auto& e : prog->children) {
        if (e == eof) {
            return true; // done
        }

        auto rule = dynamic_cast<RuleContext&>(*e).getRuleIndex();

        switch (rule) {
        case gscParser::RuleInclude:
            if (!ParseInclude(dynamic_cast<gscParser::IncludeContext*>(e), parser, obj)) {
                return false;
            }
            break;
        case gscParser::RuleNamespace:
            if (!ParseNamespace(dynamic_cast<gscParser::NamespaceContext*>(e), parser, obj)) {
                return false;
            }
            break; 
        case gscParser::RuleFunction:
            if (!ParseFunction(dynamic_cast<gscParser::FunctionContext*>(e), parser, obj)) {
                return false;
            }
            break;
        default:
            obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad export rule");
            return false;
        }
    }

    return true;
}

class ACTSErrorListener : public ConsoleErrorListener {
    InputInfo& m_info;
public:
    ACTSErrorListener(InputInfo& info) : m_info(info) {
    }

    void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine,
        const std::string& msg, std::exception_ptr e) override {
        m_info.PrintLineMessage(alogs::LVL_ERROR, line, charPositionInLine, msg);
    }
};

size_t FindEndLineDelta(const char* d) {
    const char* s = d;
    while (*d && *d != '\n') {
        d++;
    }
    return d - s;
}

bool TrimDefineVal(std::string& val) {
    if (val.empty()) {
        return false;
    }
    size_t start{};
    size_t end{ val.length() };

    while (start < val.length() && isspace(val[start])) {
        start++;
    }

    while (end > start && isspace(val[end - 1])) {
        end--;
    }

    if (end > start) {
        val = val.substr(start, end - start);
        for (size_t i = 0; i < val.length(); i++) {
            if (isspace(val[i])) {
                return false;
            }
        }
        return true;
    }

    return false;
}

bool HasOnlySpaceAfter(const std::string_view& val, size_t idx) {
    for (size_t i = idx; i < val.length(); i++) {
        if (!isspace(val[i])) {
            return false;
        }
    }
    return true;
}

inline void SetBlankChar(char& c) {
    if (!isspace(c)) {
        c = ' '; // keep same struct
    }
}

bool ApplyPreProcessorComments(InputInfo& info, std::string& str, PreProcessorOption& opt) {
    size_t idx{};
    char* data = str.data();
    char* dataEnd = data + str.length();

    while (*data) {
        char c = data[0];

        if (c == '"') {
            // skip string
            data++;
            while (data != dataEnd) {
                if (data[0] == '\\') {
                    data++;
                    if (data == dataEnd) {
                        break; // invalid \?
                    }
                }
                else if (data[0] == '"') {
                    data++;
                    break;
                }
                data++;
            }
        }
        else if (c == '/' && data[1] == '/') {
            // skip line comment

            SetBlankChar(*(data++)); // /
            do {
                SetBlankChar(*(data++));
            } while (data != dataEnd && *data != '\n');
        }
        else if (c == '/' && data[1] == '*') {
            // skip comment
            SetBlankChar(*(data++)); // /

            do {
                SetBlankChar(*(data++));
            } while (data != dataEnd && !(data[0] == '*' && data[1] == '/'));

            if (data == dataEnd) {
                info.PrintLineMessage(alogs::LVL_ERROR, nullptr, "No end for multiline comments");
                return false;
            }
            SetBlankChar(*(data++));
            SetBlankChar(*(data++)); // */
        }
        else {
            data++;
        }
    }

    return true;
}

bool ApplyPreProcessor(InputInfo& info, std::string& str, PreProcessorOption& opt) {
    if (!ApplyPreProcessorComments(info, str, opt)) {
        return false;
    }
    size_t lineStart{};
    size_t lineIdx{};
    bool err{};

    std::stack<bool> eraseCtx{};

    while (lineStart < str.length()) {
        lineIdx++;
        size_t next = str.find("\n", lineStart);

        if (next == std::string::npos) {
            next = str.length(); // last line
        }

        std::string_view line{ str.data() + lineStart, str.data() + next };
        if (line.starts_with("#ifdef")) {
            std::string define{ line.substr(6) };
            if (define.length() < 1 || !isspace(define[0])) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#ifdef should be used with a parameter");
                err = true;
            }
            else if (!TrimDefineVal(define)) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#ifdef should be used with one valid parameter");
                err = true;
            }
            else {
                bool forceDel = eraseCtx.size() && eraseCtx.top();

                if (!forceDel) {
                    forceDel = !opt.defines.contains(define);
                }
                eraseCtx.push(forceDel);
            }
        }
        else if (line.starts_with("#ifndef")) {
            std::string define{ line.substr(7) };
            if (define.length() < 1 || !isspace(define[0])) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#ifndef should be used with a parameter");
                err = true;
            }
            else if (!TrimDefineVal(define)) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#ifndef should be used with one valid parameter");
                err = true;
            }
            else {
                bool del = eraseCtx.size() && eraseCtx.top();

                if (!del) {
                    del = opt.defines.contains(define);
                }
                eraseCtx.push(del);
            }
        }
        else if (line.starts_with("#else")) {
            if (!HasOnlySpaceAfter(line, 5)) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#else can't have parameters");
                err = true;
            }
            else if (eraseCtx.empty()) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "usage of #else without start if");
                err = true;
            }
            else {
                bool curr = eraseCtx.top();
                eraseCtx.pop();

                if (!eraseCtx.empty()) {
                    // at least 2, we need to check the parent ctx
                    if (!eraseCtx.top()) {
                        eraseCtx.push(false);
                    }
                    else {
                        eraseCtx.push(!curr);
                    }
                }
                else {
                    eraseCtx.push(!curr);
                }
            }
        }
        else if (line.starts_with("#endif")) {
            if (!HasOnlySpaceAfter(line, 6)) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#endif can't have parameters");
                err = true;
            }
            else if (eraseCtx.empty()) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "usage of #endif without start if");
                err = true;
            }
            else {
                eraseCtx.pop();
            }
        }
        else if (line.starts_with("#define")) {
            std::string define{ line.substr(7) };
            if (define.length() < 1 || !isspace(define[0])) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#define should be used with a parameter");
                err = true;
            }
            else if (!TrimDefineVal(define)) {
                info.PrintLineMessage(alogs::LVL_ERROR, lineIdx, 0, "#define should be used with one valid parameter");
                err = true;
            }
            else {
                opt.defines.insert(define);
            }
        }
        else {
            if (eraseCtx.empty() || !eraseCtx.top()) {
                lineStart = next + 1;
                continue;
            }
        }


        for (size_t i = lineStart; i < next; i++) {
            SetBlankChar(str[i]);
        }

        lineStart = next + 1;
    }

    return !err;
}

int compiler(Process& proc, int argc, const char* argv[]) {
    GscCompilerOption opt;
    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp();
        return 0;
    }

    auto* readerBuilder = tool::gsc::GetGscReader(opt.m_vmInfo->vm);

    if (!readerBuilder) {
        LOG_ERROR("No GSC handler available for {}", opt.m_vmInfo->name);
        return tool::BASIC_ERROR;
    }

    std::shared_ptr<tool::gsc::GSCOBJHandler> handler {(*readerBuilder)(nullptr)};

    std::vector<std::filesystem::path> inputs{};

    for (const char* file : opt.m_inputFiles) {
        utils::GetFileRecurse(file, inputs);
    }

    auto produceFile = [&opt, &inputs, &handler](bool client) -> int {
        InputInfo info{};
        size_t lineGsc{};

        for (const std::filesystem::path& file : inputs) {
            auto ext = file.extension();
            if (client) {
                if (ext != ".csc" && ext != ".gcsc") {
                    continue;
                }
            }
            else {
                if (ext != ".gsc" && ext != ".gcsc") {
                    continue;
                }
            }

            size_t start;
            size_t startLine;

            start = info.gscData.size();
            startLine = lineGsc;

            auto& dt = info.files.emplace_back(file, start, startLine, 0);

            if (!utils::ReadFile(file, dt.buffer)) {
                LOG_ERROR("Can't read file {}", file.string());
                return tool::BASIC_ERROR;
            }

            size_t lineCount{ 1 }; // 1 for the one we'll add at the end

            const char* b = dt.buffer.data();
            while (*b) {
                if (*(b++) == '\n') {
                    lineCount++;
                }
            }

            dt.sizeLine = lineCount;
            info.gscData = info.gscData + dt.buffer + "\n";
        }

        PreProcessorOption popt = opt.processorOpt;
        popt.defines.insert("_SUPPORTS_GCSC");
        popt.defines.insert(utils::UpperCase(utils::va("_%s", opt.m_vmInfo->codeName)));
        popt.defines.insert(utils::UpperCase(utils::va("_%s", PlatformName(opt.m_platform))));

        if (client) {
            popt.defines.insert("_CSC");
        }
        else {
            popt.defines.insert("_GSC");
        }

        if (!ApplyPreProcessor(info, info.gscData, popt)) {
            LOG_ERROR("Error when applying preprocessor on data");
            return tool::BASIC_ERROR;
        }

        if (opt.m_preproc) {
            utils::WriteFile(opt.m_preproc, info.gscData);
        }

        ANTLRInputStream is{ info.gscData };

        gscLexer lexer{ &is };
        CommonTokenStream tokens{ &lexer };

        tokens.fill();
        gscParser parser{ &tokens };

        auto errList = std::make_unique<ACTSErrorListener>(info);

        parser.removeErrorListeners();

        parser.addErrorListener(&*errList);

        gscParser::ProgContext* prog = parser.prog();
        CompileObject obj{ opt, client ? FILE_CSC : FILE_GSC, info, opt.m_vmInfo, opt.m_platform, handler };

        obj.crc = client ? opt.crcClient : opt.crcServer;

        if (!obj.crc) {
            obj.crc = handler->GetDefaultChecksum(client);
        }

        const char* fn = client ? opt.nameClient : opt.nameServer;
        obj.fileName = fn && *fn ? obj.vmInfo->HashPath(fn) : 0;

        if (!obj.fileName) {
            obj.fileName = obj.vmInfo->HashPath(handler->GetDefaultName(client));
        }

        auto error = parser.getNumberOfSyntaxErrors();
        if (error) {
            LOG_ERROR("{} error(s) detected, abort", error);
            return tool::BASIC_ERROR;
        }

        LOG_TRACE("Parse tree");

        if (!ParseProg(prog, parser, obj)) {
            LOG_ERROR("Error when parsing the object");
            return tool::BASIC_ERROR;
        }


        RegisterOpCodesMap();
        std::vector<byte> data{};

        LOG_TRACE("Compile tree");
        if (!obj.Compile(data)) {
            LOG_ERROR("Error when compiling the object");
            return tool::BASIC_ERROR;
        }

        const char* outFile{ utils::va("%s.%s", opt.m_outFileName, client ? "cscc" : "gscc")};
        utils::WriteFile(outFile, (const void*)data.data(), data.size());

        LOG_INFO("Done into {}", outFile);
        return tool::OK;
    };

    int ret = produceFile(false);

    if (ret) {
        return ret;
    }

    if (opt.m_client) {
        if (!opt.m_vmInfo->HasFlag(VmFlags::VMF_CLIENT_VM)) {
            LOG_ERROR("The vm {} doesn't support client scripts", opt.m_vmInfo->name);
            return tool::BASIC_ERROR;
        }
        return produceFile(true);
    }

    return tool::OK;
}

ADD_TOOL("gscc", " --help", "gsc compiler (Alpha)", nullptr, compiler);
