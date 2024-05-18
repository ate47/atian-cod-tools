#include "adl/adlBaseVisitor.h"
#include "adl/adlLexer.h"
#include "adl/adlParser.h"
#include "adl/adlVisitor.h"
#include <includes.hpp>
#include "adl/adl.hpp"
#include "compiler/preprocessor.hpp"

namespace acts::compiler::adl {
    using namespace antlr4;
    using namespace antlr4::tree;

#pragma push_macro("ERROR")
#undef ERROR
    constexpr ParseTreeType TREE_ERROR = ParseTreeType::ERROR;
    constexpr ParseTreeType TREE_RULE = ParseTreeType::RULE;
    constexpr ParseTreeType TREE_TERMINAL = ParseTreeType::TERMINAL;
#pragma pop_macro("ERROR")


    class AdlCompilerOption {
    public:
        bool m_help{};
        const char* def{};
        const char* data{};
        const char* output{};
        preprocessor::PreProcessorOption processorOpt{};

        bool Compute(const char** args, INT startIndex, INT endIndex) {
            // default values
            for (size_t i = startIndex; i < endIndex; i++) {
                const char* arg = args[i];

                if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                    m_help = true;
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
                    if (!def) {
                        def = arg;
                    }
                    else if (!data) {
                        data = arg;
                    }
                    else if (!output) {
                        data = arg;
                    }
                }
            }
            if (!def) {

                LOG_ERROR("Missing definition file");
                return false;
            }
            return true;
        }

        void PrintHelp() {
            LOG_INFO("-h --help              : Print help");
            LOG_INFO("-D[name]               : Define variable");
        }
    };

#define IS_RULE_TYPE(rule, index) (rule && rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
#define IS_TERMINAL_TYPE(term, index) (term && term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)
#define IS_IDF(rule) (IS_RULE_TYPE((rule), gscParser::RuleIdf) || IS_TERMINAL_TYPE((rule), gscParser::IDENTIFIER))

    int64_t NumberNodeValue(ParseTree* number) {
        if (IS_RULE_TYPE(number, adlParser::RuleNumber)) {
            return NumberNodeValue(number->children[0]);
        }
        if (!number || number->getTreeType() != TREE_TERMINAL) {
            return 0; // wtf?
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(number);
        switch (term->getSymbol()->getType()) {
        case adlParser::INTEGER10:
            return std::strtoll(term->getText().c_str(), NULL, 10);
        case adlParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 16);
            return neg ? -val : val;
        }
        case adlParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), NULL, 8);
            return neg ? -val : val;
        }
        case adlParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), NULL, 2);
            return neg ? -val : val;
        }
        default:
            return 0;
        }
    }

    class ACTSErrorListener : public ConsoleErrorListener {
        const std::string& filename;
    public:
        ACTSErrorListener(const std::string& filename) : filename(filename) {
        }

        void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine,
            const std::string& msg, std::exception_ptr e) override {
            if (charPositionInLine) {
                LOG_ERROR("{}:{}#{} : {}", filename, line, charPositionInLine, msg);
            }
            else if (line) {
                LOG_ERROR("{}:{} : {}", filename, line, msg);
            }
            else {
                LOG_ERROR("{} : {}", filename, msg);
            }
        }

        void PrintLineMessage(alogs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) {
            if (charPositionInLine) {
                LOG_LVL(lvl, "{}#{}:{} {}", filename, line, charPositionInLine, msg);
            }
            else {
                LOG_LVL(lvl, "{}#{} {}", filename, line, msg);
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


    bool CompileAdl(preprocessor::PreProcessorOption& preproc, const std::string& filename, std::string& data, ADLData& output) {

        auto errList = std::make_unique<ACTSErrorListener>(filename);
        preproc.ApplyPreProcessor(data,
            [&errList](alogs::loglevel lvl, size_t line, const std::string& message) { errList->PrintLineMessage(lvl, line, 0, message); }
        );

        ANTLRInputStream is{ data.data() };

        adlLexer lexer{ &is };
        CommonTokenStream tokens{ &lexer };

        tokens.fill();
        adlParser parser{ &tokens };

        parser.removeErrorListeners();

        parser.addErrorListener(&*errList);

        adlParser::ProgContext* prog = parser.prog();

        size_t error = parser.getNumberOfSyntaxErrors();
        if (error) {
            LOG_ERROR("{} error(s) detected, abort", error);
            return false;
        }

        ParseTree* eof = prog->EOF();

        bool err{};
        for (ParseTree* e : prog->children) {
            if (e == eof) {
                break; // done
            }

            RuleContext* rule = dynamic_cast<RuleContext*>(e->children[0]);

            switch (rule->getRuleIndex()) {
            case adlParser::RuleStruct_def: {

                uint8_t align{ output.align };
                size_t size{};

                size_t i{ 1 };

                if (rule->children[i]->getText() == "align") {
                    int64_t alignVal{ NumberNodeValue(rule->children[i + 2]) };

                    if (alignVal && alignVal != 1 && alignVal != 2 && alignVal != 4 && alignVal != 8 && alignVal != 0x10) {
                        errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Invalid align: 0x{:x}", alignVal));
                        err = true;
                        continue;
                    }

                    align = (uint8_t)alignVal;

                    i += 4;
                }
                if (rule->children[i]->getText() == "size") {
                    int64_t sizeVal{ NumberNodeValue(rule->children[i + 2]) };

                    if (sizeVal < 0) {
                        errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Invalid size: 0x{:x}", sizeVal));
                        err = true;
                        continue;
                    }

                    size = sizeVal;

                    i += 4;
                }

                std::string name{ rule->children[rule->children.size() - 4]->getText() };

                ADLStruct* str{ output.CreateStruct(name, align, size) };

                if (!str) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' already defined", name));
                    err = true;
                    continue;
                }

                // TODO: fill

                break;
            }
            case adlParser::RuleFlag_def: {

                std::string type{ rule->children[3]->getText() };
                std::string name{ rule->children[1]->getText() };

                ADLDataTypeId typeId{ output.IdOfName(type) };

                if (!typeId) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' doesn't exist", type));
                    err = true;
                    continue;
                }

                ADLFlag* str{ output.CreateFlag(name, typeId) };

                if (!str) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' already defined", name));
                    err = true;
                    continue;
                }

                // TODO: fill

                break;
            }
            case adlParser::RuleEnum_def: {

                std::string type{ rule->children[3]->getText() };
                std::string name{ rule->children[1]->getText() };

                ADLDataTypeId typeId{ output.IdOfName(type) };

                if (!typeId) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' doesn't exist", type));
                    err = true;
                    continue;
                }

                ADLEnum* str{ output.CreateEnum(name, typeId) };

                if (!str) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' already defined", name));
                    err = true;
                    continue;
                }

                // TODO: fill

                break;
            }
            case adlParser::RuleRoot_def: {
                if (output.rootName) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, "Root name already defined");
                    err = true;
                    continue;
                }
                std::string root = rule->children[1]->getText();
                output.rootName = hashutils::Hash64(root.c_str());
                break;
            }
            case adlParser::RuleAlign_def: {
                if (output.align) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, "Align already defined");
                    err = true;
                    continue;
                }
                int64_t align = NumberNodeValue(rule->children[1]);
                if (!(!align || align == 1 || align == 2 || align == 4 || align == 8 || align == 0x10)) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Invalid align: 0x{:x}", align));
                    err = true;
                    continue;
                }
                output.align = (uint8_t)align;
                break;
            }
            case adlParser::RuleType_def: {
                std::string origin{ rule->children[1]->getText() };
                std::string dest{ rule->children[2]->getText() };
                ADLDataTypeId def{ output.IdOfName(origin) };
                if (!def) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Invalid type: {}", origin));
                    err = true;
                    continue;
                }
                if (!output.RegisterTypeDef(dest, def)) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type is already defined : {}", dest));
                    err = true;
                    continue;
                }
            }
                break;
            default:
                errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unhandled rule ({}): {}", rule->getRuleIndex(), rule->getText()));
                err = true;
                continue;
            }
        }
        return !err;
    }

    int adlcompiler(Process& proc, int argc, const char* argv[]) {
        AdlCompilerOption opt;
        if (!opt.Compute(argv, 2, argc) || opt.m_help) {
            opt.PrintHelp();
            return 0;
        }

        std::string buff{};

        if (!utils::ReadFile(opt.def, buff)) {
            LOG_ERROR("Can't read {}", opt.def);
            return tool::BASIC_ERROR;
        }

        ADLData output{};

        if (!CompileAdl(opt.processorOpt, opt.def, buff, output)) {
            LOG_ERROR("Error when compiling {}", opt.def);
            return tool::BASIC_ERROR;
        }

        return tool::OK;
    }

    ADD_TOOL("adlcompiler", " [def] [data] [output]", "ADL compiler", nullptr, adlcompiler);
}