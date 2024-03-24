#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"

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

constexpr uint64_t MAX_JUMP = (1 << (sizeof(uint16_t) << 3));

class AscmCompilerContext {
public:
    const VmInfo* vmInfo;
    Platform plt;
    std::vector<byte>& data;

    AscmCompilerContext(const VmInfo* vmInfo, Platform plt, std::vector<byte>& data) : vmInfo(vmInfo), plt(plt), data(data) {}

    bool HasAlign() {
        return vmInfo->flags & VmFlags::VMF_OPCODE_SHORT;
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
        utils::WriteValue(data, value);
    }
};

class AscmNode {
public:
    int32_t rloc = 0;

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
    }

    uint32_t ShiftSize(uint32_t start, bool aligned) const override {
        if (aligned) {
            return utils::Aligned<uint16_t>(start) + sizeof(uint16_t);
        }
        return start + 1;
    }

    bool Write(AscmCompilerContext& ctx) override {
        // TODO: config platform
        auto [err, op] = GetOpCodeId(ctx.vmInfo->vm, ctx.plt, opcode);
        if (err) {
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

        ctx.Write<byte>((byte)varId);
        return true;
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

    return new AscmNodeData<INT64>((INT64)val, OPCODE_GetLongInteger);
}

class AscmNodeJump : public AscmNodeOpCode {
public:
    AscmNode* location;
    AscmNodeJump(AscmNode* location, OPCode opcode) : AscmNodeOpCode(opcode), location(location) {
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

        auto delta = location->rloc - ShiftSize(rloc, ctx.HasAlign());

        if (delta >= MAX_JUMP) {
            LOG_ERROR("Max delta size");
            return false;
        }
        
        // write jump
        ctx.Align<int16_t>();
        ctx.Write<int16_t>((int16_t)delta);

        return true;
    }
};

class GscCompilerOption {
public:
    bool m_help = false;
    VmInfo* m_vmInfo{};
    Platform m_platform = Platform::PLATFORM_PC;
    std::vector<LPCCH> m_inputFiles{};

    bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            LPCCH arg = args[i];

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
            else if (*arg == '-') {
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
    }
};  

enum GscFileType {
    FILE_GSC,
    FILE_CSC
};

class GscFile {
public:
    const char* filename;
    GscFileType type;
    size_t start;
    size_t startLine;
    char* buffer;
    size_t size;
    size_t sizeLine;

    ~GscFile() {
        std::free(buffer);
    }
};

struct InputInfo {
    std::vector<GscFile> files{};
    std::string gscData{};
    std::string cscData{};


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
            LOG_LVL(lvl, "{}#{}:{} {}", f.filename, (f.startLine < line ? (line - f.startLine) : f.sizeLine), charPositionInLine, msg);
        }
        else {
            LOG_LVL(lvl, "{}#{} {}", f.filename, (f.startLine < line ? (line - f.startLine) : f.sizeLine), msg);
        }
    }
    void PrintLineMessage(alogs::loglevel lvl, ParseTree* tree, const std::string& msg) {
        while (tree && tree->getTreeType() != TREE_TERMINAL && tree->children.size()) {
            tree = tree->children[0];
        }

        if (tree && tree->getTreeType() == TREE_TERMINAL) {
            auto* token = dynamic_cast<TerminalNode*>(tree)->getSymbol();
            PrintLineMessage(lvl, token->getLine(), token->getCharPositionInLine(), msg);
            return;
        }

        alogs::log(lvl, msg);
    }
};

class RefObject {
public:
    uint32_t location = 0;
    std::vector<AscmNode*> nodes{};
};
class ImportObject {
public:
    byte flags;
    std::vector<AscmNode*> nodes{};
};
struct FunctionVar {
    std::string name;
    size_t id;
    byte flags;
};

class FunctionObject {
public:
    uint64_t m_name;
    uint64_t m_name_space;
    uint64_t m_data_name;
    byte m_params = 0;
    byte m_flags = 0;
    size_t location = 0;
    std::vector<FunctionVar> m_vars{};
    std::vector<AscmNode*> m_nodes{};
    VmInfo* m_vmInfo;
    FunctionObject(
        uint32_t name,
        uint32_t name_space,
        VmInfo* vmInfo
    ) : m_name(name), m_name_space(name_space), m_data_name(name_space), m_vmInfo(vmInfo) {
    }
    ~FunctionObject() {
        for (auto* node : m_nodes) {
            delete node;
        }
    }

    decltype(m_vars)::iterator FindVar(const std::string& name) {
        return std::find_if(m_vars.begin(), m_vars.end(), [&name](const FunctionVar& var) -> bool { return name == var.name; });
    }

    std::pair<const char*, FunctionVar*>  RegisterVar(const std::string& name, byte flags = 0) {
        if (FindVar(name) != m_vars.end()) {
            return std::make_pair<>(utils::va("The var '%s' already exists", name.c_str()), nullptr);
        }

        auto& var = m_vars.emplace_back(name, m_vars.size(), flags);

        return std::make_pair<>(nullptr, &var);
    }

    /*
     * Compute the nodes relative locations
     * @return no error
     */
    bool ComputeRelativeLocations() {
        // we start at 0 and we assume that the start location is already aligned
        INT32 current = 0;

        for (auto node : m_nodes) {
            node->rloc = current;
            current = node->ShiftSize(current, m_vmInfo->flags & VmFlags::VMF_OPCODE_SHORT);
            if (node->rloc > current) {
                return false;
            }
        }
        return true;
    }
};


class CompileObject {
public:
    InputInfo& info;
    GscFileType type;
    uint32_t currentNamespace = hashutils::Hash32("");
    std::set<uint64_t> includes{};
    std::unordered_map<uint32_t, FunctionObject> exports{};
    std::unordered_map<std::string, RefObject> strings{};
    std::unordered_map<uint64_t, std::vector<ImportObject>> imports{};
    VmInfo* vmInfo;
    Platform plt;

    std::unordered_set<std::string> hashes{};

    CompileObject(GscFileType file, InputInfo& nfo, VmInfo* vmInfo, Platform plt) : type(file), info(nfo), vmInfo(vmInfo), plt(plt) {
    }

    uint64_t GetScPath(std::string& data) {
        hashes.insert(data);

        return 0;
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
        includes.insert(hashutils::Hash64Pattern(data.data()));
        return 0;
    }
};

#define IS_RULE_TYPE(rule, index) (rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
#define IS_TERMINAL_TYPE(term, index) (term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)

bool ParseFieldNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool withVal) {
    if (exp->getTreeType() == TREE_ERROR) {
        return false;
    }

    return false;
}
bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal) {
    if (exp->getTreeType() == TREE_ERROR) {
        return false;
    }

    if (exp->getTreeType() == TREE_RULE) {
        auto* rule = dynamic_cast<RuleContext*>(exp);

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
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_BoolNot));
                    }
                    else if (op == "~") {
                        if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                            return false;
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_BoolComplement));
                    }
                    else if (op == "++") {
                        // ++var
                        if (!ParseFieldNode(rule->children[1], parser, obj, fobj, false)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Inc));
                    }
                    else if (op == "--") {
                        // --var
                        if (!ParseFieldNode(rule->children[1], parser, obj, fobj, false)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Dec));
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
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Inc));
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                    }
                    else if (op == "--") {
                        // var--
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Dec));
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));

                    }
                    else {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled operator: {}", op));
                        return false;
                    }
                }

                if (!expressVal) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                }

                return true;
            }
            assert(rule->children.size() == 3 && "Expression should have 3 components");

            auto op = rule->children[1]->getText();

            if (op == "||" || op == "&&") {
                // boolean operators are defined using jumps, we need to handle them
                // push op left
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    return false;
                }
                auto* after = new AscmNode();

                fobj.m_nodes.push_back(new AscmNodeJump(after, op == "&&" ? OPCODE_JumpOnFalseExpr : OPCODE_JumpOnTrueExpr));

                // push op right
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    return false;
                }

                // after the operator
                fobj.m_nodes.push_back(after);

                if (!expressVal) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                }
                return true;
            }
            else {
                // push operands
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    return false;
                }
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    return false;
                }

                if (op == "|") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_Or));
                }
                else if (op == "^") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_Xor));
                }
                else if (op == "&") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_And));
                }
                else if (op == "!=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_NotEqual));
                }
                else if (op == "!==") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SuperNotEqual));
                }
                else if (op == "==") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Equal));
                }
                else if (op == "===") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SuperEqual));
                }
                else if (op == "<") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_LessThan));
                }
                else if (op == "<=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_LessThanOrEqualTo));
                }
                else if (op == ">") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_GreaterThan));
                }
                else if (op == ">=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_GreaterThanOrEqualTo));
                }
                else if (op == "+") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Plus));
                }
                else if (op == "-") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Minus));
                }
                else if (op == "*") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Multiply));
                }
                else if (op == "/") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Divide));
                }
                else if (op == "%") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Modulus));
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled operator: {}", op));
                    return false;
                }
                if (!expressVal) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                }
                return true;
            }
        }
            break;
        case gscParser::RuleConst_expr:
        case gscParser::RuleNumber:
        case gscParser::RuleExpression13:
            return ParseExpressionNode(rule->children[rule->children.size() == 3 ? 1 : 0], parser, obj, fobj, expressVal);
        case gscParser::RuleVector_value:
            if (!expressVal) { // no need to create vector
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("useless value: {}", rule->getText()));
                return true;
            }
            if (
                !ParseExpressionNode(rule->children[1], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[3], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[5], parser, obj, fobj, true)
                ) {
                return false;
            }
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Vector));
            return true;
        case gscParser::RuleArray_def: {
            if (!expressVal) { // no need to create array
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("useless value: {}", rule->getText()));
                return true;
            }
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CreateArray));

            // 1 = [ (HASHSTR|'#' number) ':'
            size_t currentKey{};
            for (size_t i = 1; i + 2 < rule->children.size(); i++) {
                if (rule->children[i]->getTreeType() != TREE_RULE || dynamic_cast<RuleContext*>(rule->children[i])->getRuleIndex() != gscParser::RuleExpression) {
                    // '#'? key ':'

                    if (rule->children[i]->getText() == "#") {
                        i++; // skip '#'
                    }

                    if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                        return false;
                    }

                    i++; // ':'
                }
                else {
                    // push current key
                    fobj.m_nodes.push_back(BuildAscmNodeData(currentKey++));
                }

                if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_AddToArray));

                i++; // ',' or ']'
            }

            return true;
        }
        case gscParser::RuleStruct_def:
            if (!expressVal) { // no need to create struct
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("useless value: {}", rule->getText()));
                return true;
            }
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CreateStruct));

            // 1 = [ (HASHSTR|'#' number) ':'
            for (size_t i = 1; i < rule->children.size(); i++) {

                auto* term = rule->children[i];

                auto termStr = term->getText();

                auto sub = termStr.substr(1, termStr.size() - 1);
                fobj.m_nodes.push_back(new AscmNodeData<uint64_t>(hash::Hash32Pattern(sub.c_str()), OPCODE_GetHash));

                i++; // ':'


                if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_AddToStruct));

                i++; // ',' or '}'
            }
            return true;
        case gscParser::RuleSet_expression: {
            std::string opVal = rule->children[1]->getText();

            if (opVal == "=") {
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, expressVal)) {
                    return false;
                }
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj, false)) {
                    return false;
                }
                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
                return true;
            }
            else {
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    return false;
                }
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    return false;
                }
                if (opVal == "+=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Plus));
                }
                else if (opVal == "-=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Minus));
                }
                else if (opVal == "/=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Divide));
                }
                else if (opVal == "*=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Multiply));
                }
                else if (opVal == "%=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Modulus));
                }
                else if (opVal == "&=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_And));
                }
                else if (opVal == "|=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_Or));
                }
                else if (opVal == "^=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Bit_Xor));
                }
                else if (opVal == "<<=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_ShiftLeft));
                }
                else if (opVal == ">>=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_ShiftRight));
                }
                else if (opVal == "~=") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_BoolComplement));
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled set operator: {}", opVal));
                    return false;
                }

                // dogshit way, but it seems to be the same in their compiler...
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
                if (expressVal) {
                    if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
        case gscParser::RuleFunction_ref: {
            if (rule->children.size() == 7) {
                // @nsp<path>::func
                auto nsp = rule->children[1]->getText();
                auto path = rule->children[3]->getText();
                auto funcName = rule->children[6]->getText();

                
                fobj.m_nodes.push_back(new AscmNodeLazyLink(
                    hashutils::Hash64Pattern(path.c_str()),
                    hashutils::Hash32Pattern(nsp.c_str()),
                    hashutils::Hash32Pattern(funcName.c_str())
                ));
                return true;
            }
            // &nsp::func || &func
            auto nsp = obj.currentNamespace;

            if (rule->children.size() == 4) {
                // with nsp
                auto nspStr = rule->children[1]->getText();
                nsp = hashutils::Hash32Pattern(nspStr.c_str());
            }

            assert(rule->children.size());

            auto funcStr = rule->children[rule->children.size() - 1]->getText();
            auto func = hashutils::Hash32Pattern(funcStr.c_str());

            // link by the game, but we write it for test
            auto located = utils::CatLocated(nsp, func);
            auto* asmc = new AscmNodeData<uint64_t>(located, OPCODE_GetFunction);
            fobj.m_nodes.push_back(asmc);

            auto& impList = obj.imports[located];

            BYTE flags = tool::gsc::T8GSCImportFlags::GET_CALL;

            auto it = std::find_if(impList.begin(), impList.end(), [flags](const auto& e) { return e.flags == flags; });

            if (it == impList.end()) {
                // no equivalent, we need to create our own node
                impList.emplace_back(flags).nodes.push_back(asmc);
            }
            else {
                // same local/flags, we can add our node
                it->nodes.push_back(asmc);
            }

            return true;
        }
        case gscParser::RuleLeft_value: {
            if (rule->children.size() == 1) {
                // IDENTIFIER
                return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
            }
            if (rule->children[0]->getTreeType() == TREE_TERMINAL && rule->children[0]->getText() == "[[") {
                // | '[[' expression ']]' '->' IDENTIFIER

                // todo: implement this
                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Class objects not implemented");
                return false;

            }
            else if (rule->children[0]->getTreeType() == TREE_RULE) {
                auto* first = dynamic_cast<RuleContext*>(rule->children[0]);

                if (first->getRuleIndex() == gscParser::RuleConst_expr) {
                    /*
                        | (const_expr | ('(' expression ')')) '.' (IDENTIFIER | ('(' expression ')'))
                        -> 
                        | (const_expr | ('(' expression ')')) '[' expression ']'
                    */


                }
                else if (first->getRuleIndex() == gscParser::RuleLeft_value) {
                    /*
                        | left_value '.' (IDENTIFIER | ('(' expression ')'))
                        -> test.test2 test.("test2")
                        | left_value '[' expression ']'
                        -> test[...]
                    */

                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown rule type: {}", exp->getText()));
                    return false;
                }
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown rule type: {}", exp->getText()));
                return false;
            }
            obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown rule type: {}", exp->getText()));
            return false;
        }
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("unhandled rule: {}", rule->getText()));
        return false;
    }

    if (exp->getTreeType() != TREE_TERMINAL) {
        obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("unknown tree type: {}", exp->getText()));
        return false;
    }

    auto* term = dynamic_cast<TerminalNode*>(exp);

    if (!expressVal) {
        obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("useless value: {}", term->getText()));
        return true;
    }

    auto len = term->getText().size();

    switch (term->getSymbol()->getType()) {
    case gscParser::IDENTIFIER: {
        std::string varName = term->getText();

        auto varIt = fobj.FindVar(varName);


        if (varIt == fobj.m_vars.end()) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, term, std::format("unknown variable: {}", varName));
            return false;
        }

        fobj.m_nodes.push_back(new AscmNodeVariable(varIt->id, OPCODE_EvalLocalVariableCached));
        return true;
    }
    case gscParser::UNDEFINED_VALUE:
        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_GetUndefined));
        return true;
    case gscParser::BOOL_VALUE:
        fobj.m_nodes.push_back(BuildAscmNodeData(term->getText() == "true"));
        return true;
    case gscParser::FLOATVAL:
        fobj.m_nodes.push_back(new AscmNodeData<FLOAT>((FLOAT)std::strtof(term->getText().c_str(), NULL), OPCODE_GetFloat));
        return true;
    case gscParser::INTEGER10:
        fobj.m_nodes.push_back(BuildAscmNodeData(std::strtoll(term->getText().c_str(), NULL, 10)));
        return true;
    case gscParser::INTEGER16: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 16);
        fobj.m_nodes.push_back(BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::INTEGER8: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), NULL, 8);
        fobj.m_nodes.push_back(BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::INTEGER2: {
        bool neg = term->getText()[0] == '-';
        auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 2);
        fobj.m_nodes.push_back(BuildAscmNodeData(neg ? -val : val));
        return true;
    }
    case gscParser::HASHSTRING: {
        auto sub = term->getText().substr(2, len - 3);
        fobj.m_nodes.push_back(new AscmNodeData<uint64_t>(hash::Hash64Pattern(sub.c_str()), OPCODE_GetHash));
        return true;
    }
    case gscParser::STRING: {
        auto node = term->getText();
        auto newStr = std::make_unique<char[]>(node.length() + 1);
        auto* newStrWriter = &newStr[0];

        // format string
        for (size_t i = 0; i < node.length(); i++) {
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
        auto* asmc = new AscmNodeData<uint32_t>(0, OPCODE_GetString);
        fobj.m_nodes.push_back(asmc);

        auto& str = obj.strings[&newStr[0]];
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
    UINT32 nameHashed = hashutils::Hash32Pattern(name.data());

    auto [res, err] = obj.exports.try_emplace(nameHashed, nameHashed, obj.currentNamespace, obj.vmInfo);

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
        }
        else if (txt == "event_handler") {
            exp.m_flags |= tool::gsc::T8GSCExportFlags::EVENT;
            auto* ev = func->children[i += 2];
            i++; // ']'
            if (ev->getTreeType() != TREE_TERMINAL) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, ev, std::format("Bad event for {}", name));
                return false;
            }

            auto evname = static_cast<TerminalNode*>(ev)->getText();

            obj.hashes.insert(evname);
            UINT32 evnameHashed = hashutils::Hash32Pattern(evname.data());
            exp.m_data_name = evnameHashed;
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
            // todo: add param modifier

            if (param->children.size() == 1) {
                // '...'
                exp.m_flags |= tool::gsc::T8GSCExportFlags::VE;

                std::string paramIdf = "vararg";


                if (exp.m_params == 256) {
                    return utils::va("", name.c_str());
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, std::format("Can't register param '{}': too many params", paramIdf));
                }

                exp.m_params++;

                auto [err, vardef] = exp.RegisterVar(paramIdf, 1);
                if (err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, err);
                    return false;
                }

                return true;
            }

            // skip modifier
            idfNode = dynamic_cast<TerminalNode*>(param->children[1]);
            auto modifier = param->children[0]->getText();
            //if (modifier == "*") {
            //    // ptr (T9)
            //}
            //else if (modifier == "&") {
            //    // ref (T8)
            //}
            //else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not implemented: {}", modifier));
                return false;
            //}
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

        auto [err, vardef] = exp.RegisterVar(paramIdf, 1);

        if (err) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, err);
            return false;
        }

        if (param->children.size() >= 3) {
            // default value
            assert(IS_RULE_TYPE(param->children[param->children.size() - 1], gscParser::RuleExpression));
            auto defaultValueExp = dynamic_cast<gscParser*>(param->children[param->children.size() - 1]);

            // todo: add default block vardef->id
            

            obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, std::format("default block not implemented yet"));
            return false;
        }

    }

    // handle block

    auto* block = dynamic_cast<gscParser::Statement_blockContext*>(blockRule);

    return true;
}

bool ParseInclude(gscParser::IncludeContext* nsp, gscParser& parser, CompileObject& obj) {
    if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
        return false; // bad
    }

    auto txt = dynamic_cast<TerminalNode*>(nsp->children[1])->getText();

    // add the include/using into the includes
    obj.AddInclude(txt);

    return true;
}

bool ParseNamespace(gscParser::NamespaceContext* nsp, gscParser& parser, CompileObject& obj) {
    if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
        return false; // bad
    }

    auto txt = dynamic_cast<TerminalNode*>(nsp->children[1])->getText();

    // set the current namespace to the one specified

    obj.hashes.insert(txt);
    obj.currentNamespace = hashutils::Hash32Pattern(txt.data());

    return true;
}

bool ParseProg(gscParser::ProgContext* prog, gscParser& parser, CompileObject& obj) {
    if (prog->getTreeType() == TREE_ERROR) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad prog context");
        return false;
    }

    auto* eof = prog->EOF();

    for (auto& e : prog->children) {
        if (e == eof) {
            return true; // done
        }
        if (e->getTreeType() != TREE_RULE) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad export rule type");
            return false;
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

int compiler(Process& proc, int argc, const char* argv[]) {
    GscCompilerOption opt;
    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp();
        return 0;
    }

    InputInfo info{};
    size_t lineGsc = 0;
    size_t lineCsc = 0;

    for (const auto& file : opt.m_inputFiles) {
        auto s = strlen(file);

        GscFileType type;
        size_t start;
        size_t startLine;
        if (s < 4) {
            continue;
        }
        if (!strncmp(&file[s - 4], ".gsc", 4)) {
            type = FILE_GSC;
            start = info.gscData.size();
            startLine = lineGsc;
        }
        else if (!strncmp(&file[s - 4], ".csc", 4)) {
            type = FILE_GSC;
            start = info.cscData.size();
            startLine = lineCsc;
        }
        else {
            continue; // not a known file type, ignore
        }

        auto& dt = info.files.emplace_back(file, type, start, startLine);

        if (!utils::ReadFileNotAlign(file, reinterpret_cast<LPVOID&>(dt.buffer), dt.size, true)) {
            LOG_ERROR("Can't read file {}", file);
            return tool::BASIC_ERROR;
        }

        size_t lineCount = 1; // 1 for the one we'll add at the end

        LPCCH b = dt.buffer;
        while (*b) {
            if (*(b++) == '\n') {
                lineCount++;
            }
        }

        dt.sizeLine = lineCount;


        switch (type) {
        case FILE_GSC:
            info.gscData = info.gscData + dt.buffer + "\n";
            lineGsc += lineCount;
            break;
        case FILE_CSC:
            info.cscData = info.cscData + dt.buffer + "\n";
            lineCsc += lineCount;
            break;
        default:
            break;
        }
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
    CompileObject obj{ FILE_GSC, info, opt.m_vmInfo, opt.m_platform };

    auto error = parser.getNumberOfSyntaxErrors();
    if (error) {
        LOG_ERROR("{} error(s) detected, abort", error);
        return tool:: BASIC_ERROR;
    }

    if (!ParseProg(prog, parser, obj)) {
        LOG_ERROR("Error when compiling the object");
        return tool::BASIC_ERROR;
    }

    LOG_INFO("Done");


    return 0;
}

#ifndef CI_BUILD
ADD_TOOL("compiler", " --help", "gsc compiler", nullptr, compiler);
#endif
