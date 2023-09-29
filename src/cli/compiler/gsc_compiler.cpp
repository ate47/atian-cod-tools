#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include "antlr4-runtime.h"

#include <includes.hpp>

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


class AscmNode {
public:
    UINT32 rloc = 0;

    virtual ~AscmNode() {};

    virtual UINT32 ShiftSize(UINT32 start) const {
        return start; // empty by default
    }

    virtual bool Write(std::vector<BYTE>& data) {
        // nothing by default
        return true;
    }
};

class AscmNodeOpCode : public AscmNode {
public:
    OPCode opcode;

    AscmNodeOpCode(OPCode opcode) : opcode(opcode) {
    }

    UINT32 ShiftSize(UINT32 start) const override {
        return utils::Aligned<UINT16>(start) + sizeof(UINT16);
    }

    bool Write(std::vector<BYTE>& data) override  {
        auto [err, op] = GetOpCodeId(VM_T8, opcode);
        if (err) {
            return false;
        }

        utils::Aligned<UINT16>(data);
        utils::WriteValue(data, op);
        
        // nothing by default
        return true;
    }
};

class GscCompilerOption {
public:
    bool m_help = false;
    std::vector<LPCCH> m_inputFiles{};

    bool Compute(LPCCH* args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            LPCCH arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
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
            m_inputFiles.push_back(".");
        }
        return true;
    }

    void PrintHelp(std::ostream& out) {
        out << "-h --help          : Print help\n";
    }
};  

enum GscFileType {
    FILE_GSC,
    FILE_CSC
};

class GscFile {
public:
    LPCCH filename;
    GscFileType type;
    size_t start;
    size_t startLine;
    LPCH buffer;
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

    std::ostream& PrintLineMessage(std::ostream& out, size_t line, size_t charPositionInLine) {
        const auto& f = FindFile(line);
        
        out
            << f.filename
            << "#" << (f.startLine < line ? (line - f.startLine) : f.sizeLine); 
        
        if (charPositionInLine) {
            out << ":" << charPositionInLine;
        }
        return out << " ";
    }
    inline std::ostream& PrintLineMessage(std::ostream& out, Token* token) {
        return PrintLineMessage(out, token->getLine(), token->getCharPositionInLine());
    }
};

class FunctionObject {
public:
    UINT32 m_name;
    UINT32 m_name_space;
    UINT32 m_data_name;
    BYTE m_params = 0;
    BYTE m_flags = 0;
    std::vector<std::string> m_vars{};
    std::vector<AscmNode*> m_nodes{};
    FunctionObject(
        UINT32 name,
        UINT32 name_space
    ) : m_name(name), m_name_space(name_space), m_data_name(name_space) {
    }
    ~FunctionObject() {
        for (auto* node : m_nodes) {
            delete node;
        }
    }

    /*
     * Compute the nodes relative locations
     */
    void ComputeRelativeLocations() {
        // we start at 0 and we assume that the start location is already aligned
        UINT32 current = 0;

        for (auto node : m_nodes) {
            node->rloc = current;
            current = node->ShiftSize(current);
        }
    }
};


class CompileObject {
public:
    InputInfo& info;
    GscFileType type;
    UINT32 currentNamespace = hashutils::Hash32("");
    std::set<UINT64> includes{};
    std::unordered_map<UINT32, FunctionObject> exports{};

    std::unordered_set<std::string> hashes{};

    CompileObject(GscFileType file, InputInfo& nfo) : type(file), info(nfo) {
    }

    UINT64 GetScPath(std::string& data) {
        hashes.insert(data);

        return 0;
    }
    UINT64 AddInclude(std::string& data) {
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

bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj) {
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
        case gscParser::RuleExpression12:
        case gscParser::RuleSet_expression: {
            if (rule->children.size() == 1) {
                // simple rules recursion
                return ParseExpressionNode(rule->children[0], parser, obj, fobj);
            }
            if (rule->children.size() == 2) {
                // (++|--|~|!) exp


                return true;
            }
            assert(rule->children.size() == 3 && "Expression should have 3 components");

            auto op = rule->children[1]->getText();

            // TODO: bool operator
            if (op == "||") {

            }
            else if (op == "&&") {

            }
            else {
                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj)) {
                    return false;
                }
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj)) {
                    return false;
                }
                // TODO: push operator
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
                    obj.info.PrintLineMessage(std::cerr, 0)
                        << "unhandled operator: " << op << "\n";
                    return false;
                }
                return true;
            }
        }
            break;
        case gscParser::RuleExpression13:
            return ParseExpressionNode(rule->children[rule->children.size() == 3 ? 1 : 0], parser, obj, fobj);
        }

    }

    return true;
}

bool ParseFunction(gscParser::FunctionContext* func, gscParser& parser, CompileObject& obj) {
    if (func->children.size() < 5) { // 0IDF 1( 2params 3) 4block
        obj.info.PrintLineMessage(std::cerr, func->getStart())
            << "Bad function declaration\n";
        return false;
    }

    auto* nameTerm = func->children[(size_t)(func->children.size() - 5)];
    auto* paramsRule = func->children[(size_t)(func->children.size() - 3)];
    auto* blockRule = func->children[(size_t)(func->children.size() - 1)];

    if (!IS_TERMINAL_TYPE(nameTerm, gscParser::IDENTIFIER)) {
        obj.info.PrintLineMessage(std::cerr, func->getStart())
            << "Bad function name declaration\n";
        return false;
    }

    auto* termNode = static_cast<TerminalNode*>(nameTerm);
    
    auto name = termNode->getText();

    obj.hashes.insert(name);
    UINT32 nameHashed = hashutils::Hash32Pattern(name.data());

    auto [res, err] = obj.exports.try_emplace(nameHashed, nameHashed, obj.currentNamespace);

    if (!err) {
        obj.info.PrintLineMessage(std::cerr, func->getStart())
            << "The export " << name << " was defined twice\n";
        return false;
    }

    auto& exp = res->second;

    if (!IS_RULE_TYPE(paramsRule, gscParser::RuleParam_list)) {
        obj.info.PrintLineMessage(std::cerr, func->getStart())
            << "Bad function " << name << " params declaration " << func << "\n";
        return false;
    }
    if (!IS_RULE_TYPE(blockRule, gscParser::RuleStatement_block)) {
        obj.info.PrintLineMessage(std::cerr, func->getStart())
            << "Bad function block declaration " << func << "\n";
        return false;
    }

    // handle modifiers

    for (size_t i = 0; i < func->children.size() - 5; i++) {
        auto* mod = func->children[i];
        if (mod->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(std::cerr, func->getStart())
                << "bad modifier for " << name << "\n";
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
                obj.info.PrintLineMessage(std::cerr, func->getStart())
                    << "bad event for " << name  << "\n";
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
    for (auto* child : params->children) {
        if (index++ % 2) {
            continue; // coma
        }
        assert(IS_RULE_TYPE(child, gscParser::RuleParam_val));
        auto* param = dynamic_cast<gscParser::Param_valContext*>(child);
        assert(IS_TERMINAL_TYPE(param->children[0], gscParser::IDENTIFIER));
        auto* idfNode = dynamic_cast<TerminalNode*>(param->children[0]);
        auto paramIdf = idfNode->getText();
        if (exp.m_params == 256) {
            obj.info.PrintLineMessage(std::cerr, idfNode->getSymbol())
                << "Too many variables\n";
            return false;
        }

        exp.m_params++;


        if (std::find(exp.m_vars.begin(), exp.m_vars.end(), paramIdf) != exp.m_vars.end()) {
            obj.info.PrintLineMessage(std::cerr, idfNode->getSymbol())
                << "The parameter '" << paramIdf << "' was registered twice\n";

            return false;
        }

        exp.m_vars.push_back(paramIdf);

        if (param->children.size() == 3) {
            // default value
            assert(IS_RULE_TYPE(param->children[2], gscParser::RuleExpression));
            auto defaultValueExp = dynamic_cast<gscParser*>(param->children[2]);

            // todo: add default block

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
        obj.info.PrintLineMessage(std::cerr, prog->getStart()) << "Bad prog context\n";
        return false;
    }

    auto* eof = prog->EOF();

    for (auto& e : prog->children) {
        if (e == eof) {
            return true; // done
        }
        if (e->getTreeType() != TREE_RULE) {
            obj.info.PrintLineMessage(std::cerr, prog->getStart()) << "Bad export rule type\n";
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
            obj.info.PrintLineMessage(std::cerr, prog->getStart()) << "Bad export rule\n";
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
        m_info.PrintLineMessage(std::cerr, line, charPositionInLine) << msg << "\n";
    }
};

int compiler(const Process& proc, int argc, const char* argv[]) {
    GscCompilerOption opt;
    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
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
            std::cerr << "Can't read file " << file << "\n";
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


        switch (type)
        {
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
    CompileObject obj{ FILE_GSC, info};

    auto error = parser.getNumberOfSyntaxErrors();
    if (error) {
        std::cerr << std::dec << error << " error(s) detected, abort\n";
        return tool:: BASIC_ERROR;
    }

    if (!ParseProg(prog, parser, obj)) {
        std::cerr << "Error when compiling the object\n";
        return tool::BASIC_ERROR;
    }

    std::cout << "Done\n";


    return 0;
}

#ifdef DEBUG
ADD_TOOL("compiler", " --help", "gsc compiler", false, compiler);
#endif
