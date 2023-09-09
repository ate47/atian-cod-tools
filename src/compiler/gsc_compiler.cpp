#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include "antlr4-runtime.h"

#include <includes.hpp>

using namespace antlr4;
using namespace antlr4::tree;

#pragma push_macro("ERROR")
#undef ERROR
constexpr auto TREE_ERROR = ParseTreeType::ERROR;
constexpr auto TREE_RULE = ParseTreeType::RULE;
constexpr auto TREE_TERMINAL = ParseTreeType::TERMINAL;
#pragma pop_macro("ERROR")

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
    LPCH buffer;
    size_t size;

    ~GscFile() {
        std::free(buffer);
    }
};

class FunctionObject {
public:
    UINT32 m_name;
    UINT32 m_name_space;
    UINT32 m_data_name;
    BYTE m_params = 0;
    BYTE m_flags = 0;
    FunctionObject(
        UINT32 name,
        UINT32 name_space
    ) : m_name(name), m_name_space(name_space), m_data_name(name_space) {
    }

};


class CompileObject {
public:
    GscFileType type;
    UINT32 currentNamespace = hashutils::Hash32("");
    std::set<UINT64> includes{};
    std::unordered_map<UINT32, FunctionObject> exports{};

    std::unordered_set<std::string> hashes{};

    CompileObject(GscFileType file) : type(file) {
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


bool ParseFunction(gscParser::FunctionContext* func, gscParser& parser, CompileObject& obj) {
    if (func->children.size() < 5) { // 0IDF 1( 2params 3) 4block
        std::cerr << "Bad function declaration " << func << "\n";
        return false;
    }

    auto* nameTerm = func->children[(size_t)(func->children.size() - 5)];
    auto* paramsRule = func->children[(size_t)(func->children.size() - 3)];
    auto* blockRule = func->children[(size_t)(func->children.size() - 1)];

    if (!IS_TERMINAL_TYPE(nameTerm, gscParser::IDENTIFIER)) {
        std::cerr << "Bad function name declaration " << func << "\n";
        return false;
    }

    auto name = static_cast<TerminalNode*>(nameTerm)->getText();

    obj.hashes.insert(name);
    UINT32 nameHashed = hashutils::Hash32Pattern(name.data());

    auto expRes = obj.exports.try_emplace(nameHashed, nameHashed, obj.currentNamespace);

    if (!expRes.second) {
        std::cerr << "The export " << name << " was defined twice\n";
        return false;
    }

    auto exp = expRes.first->second;

    if (!IS_RULE_TYPE(paramsRule, gscParser::RuleParam_list)) {
        std::cerr << "Bad function " << name << " params declaration " << func << "\n";
        return false;
    }
    if (!IS_RULE_TYPE(blockRule, gscParser::RuleStatement_block)) {
        std::cerr << "Bad function block declaration " << func << "\n";
        return false;
    }

    auto* params = dynamic_cast<gscParser::Param_listContext*>(paramsRule);
    auto* block = dynamic_cast<gscParser::Statement_blockContext*>(blockRule);

    // handle modifiers

    for (size_t i = 0; i < func->children.size() - 5; i++) {
        auto* mod = func->children[i];
        if (mod->getTreeType() != TREE_TERMINAL) {
            std::cerr << "bad modifier for " << name << "\n";
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
                std::cerr << "bad event for " << name  << "\n";
                return false;
            }

            auto evname = static_cast<TerminalNode*>(ev)->getText();

            obj.hashes.insert(evname);
            UINT32 evnameHashed = hashutils::Hash32Pattern(evname.data());
            exp.m_data_name = evnameHashed;
        }
    }

    // handle params

    // handle block

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
        std::cerr << "Bad prog context";
        return false;
    }

    auto* eof = prog->EOF();

    for (auto& e : prog->children) {
        if (e == eof) {
            return true; // done
        }
        if (e->getTreeType() != TREE_RULE) {
            std::cerr << "Bad export rule type " << (size_t)e->getTreeType() << "\n";
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
            std::cerr << "Bad export rule " << rule << "\n";
            return false;
        }
    }

    return true;
}

int gsc::compiler::compiler(const Process& proc, int argc, const char* argv[]) {
    GscCompilerOption opt;
    if (!opt.Compute(argv, 2, argc) || opt.m_help) {
        opt.PrintHelp(std::cout);
        return 0;
    }

    std::vector<GscFile> files{};
    std::string gscData{};
    std::string cscData{};

    for (const auto& file : opt.m_inputFiles) {
        auto s = strlen(file);

        GscFileType type;
        size_t start;
        if (s < 4) {
            continue;
        }
        if (!strncmp(&file[s - 4], ".gsc", 4)) {
            type = FILE_GSC;
            start = gscData.size();
        }
        else if (!strncmp(&file[s - 4], ".csc", 4)) {
            type = FILE_GSC;
            start = cscData.size();
        }
        else {
            continue; // not a known file type, ignore
        }

        auto& dt = files.emplace_back(file, type, start);

        if (!utils::ReadFileNotAlign(file, reinterpret_cast<LPVOID&>(dt.buffer), dt.size, true)) {
            std::cerr << "Can't read file " << file << "\n";
            return tool::BASIC_ERROR;
        }


        switch (type)
        {
        case FILE_GSC:
            gscData = gscData + dt.buffer + "\n";
            break;
        case FILE_CSC:
            cscData = cscData + dt.buffer + "\n";
            break;
        default:
            break;
        }
    }



    ANTLRInputStream is{ gscData };

    gscLexer lexer{ &is };
    CommonTokenStream tokens{ &lexer };

    tokens.fill();
    gscParser parser{ &tokens };

    gscParser::ProgContext* prog = parser.prog();
    CompileObject obj{ FILE_GSC};

    if (!ParseProg(prog, parser, obj)) {
        std::cerr << "Error when compiling the object\n";
        return tool::BASIC_ERROR;
    }

    //std::cout << prog->toStringTree(&parser, true) << std::endl;


    return 0;
}
