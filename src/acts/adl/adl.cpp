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

                ParseTree* members{ rule->children[rule->children.size() - 2] };

                for (size_t i = 0; i < members->children.size(); i += 2) {
                    RuleContext* structMember{ dynamic_cast<RuleContext*>(members->children[i]->children[0])};

                    if (structMember->getRuleIndex() == adlParser::RuleData_member) {
                        ADLStructField& nfield{ str->fields.emplace_back() };

                        std::string typeName{ structMember->children[0]->getText() };
                        nfield.type = output.IdOfName(typeName);

                        size_t idx{ 1 };
                        if (structMember->children[idx]->getTreeType() == TREE_TERMINAL && structMember->children[idx]->getText() == "*") {
                            nfield.pointer = true;
                            idx++;
                        }
                        else {
                            nfield.pointer = false;
                        }

                        std::string fieldName{ structMember->children[idx]->getText() };
                        uint64_t fieldHash{ hash::Hash64(fieldName.c_str()) };

                        for (size_t j = 0; j < str->fields.size(); j++) {
                            if (str->fields[j].name == fieldHash) {
                                errList->PrintLineMessage(alogs::LVL_ERROR, structMember->children[idx], std::format("Field already defined for this structure {}", fieldName));
                                err = true;
                                break;
                            }
                        }
                        nfield.name = fieldHash;

                        if (structMember->children.size() > ++idx) {
                            int64_t arraySize{ NumberNodeValue(structMember->children[++idx]) };

                            if (arraySize < 0) {
                                errList->PrintLineMessage(alogs::LVL_ERROR, structMember->children[idx], std::format("Can't have negative array size {}", arraySize));
                                err = true;
                                arraySize = 0;
                            }
                            nfield.arraySize = (size_t)arraySize;
                        }
                        else {
                            nfield.arraySize = 1;
                        }
                    }
                    else if (structMember->getRuleIndex() == adlParser::RuleData_operator) {
                        std::string oper{ structMember->children[0]->getText() };
                        int64_t arraySize{ NumberNodeValue(structMember->children[1]) };

                        if (oper == "$padding") {
                            if (arraySize < 0) {
                                errList->PrintLineMessage(alogs::LVL_ERROR, structMember->children[1], std::format("Padding to a negative size {}", arraySize));
                                err = true;
                                arraySize = 0;
                            }
                            ADLStructField& nfield{ str->fields.emplace_back() };
                            nfield.name = PADDING_FIELD_HASH;
                            nfield.type = ADD_PADDING;
                            nfield.arraySize = 1;
                            nfield.pointer = false;
                            nfield.offset = (size_t)arraySize;
                        }
                        //else if (oper == "$assert_offset") {
                        // TODO: fill
                        //}
                        else {
                            errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Operator not handler : {}", oper));
                            err = true;
                            continue;
                        }

                    }
                    else {
                        errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Struct member not handler ({}): {}", structMember->getRuleIndex(), structMember->getText()));
                        err = true;
                        continue;
                    }
                }

                break;
            }
            case adlParser::RuleFlag_def: {

                std::string type{ rule->children[3]->getText() };
                std::string name{ rule->children[1]->getText() };

                ADLDataTypeId typeId{ output.IdOfName(type) };

                if (!IsDefaultType(typeId) || !IsIntType(typeId)) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' doesn't exist or isn't an int type", type));
                    err = true;
                    continue;
                }

                size_t sizeOf{ output.SizeOf(typeId) };
                int64_t maxValue, minValue;
                if (IsUnsignedType(typeId)) {
                    if (sizeOf == 8) {
                        maxValue = INT64_MAX;
                    }
                    else {
                        maxValue = (1ull << (sizeOf << 3));
                    }
                    minValue = 0;
                }
                else {
                    if (sizeOf == 8) {
                        maxValue = INT64_MAX;
                        minValue = INT64_MIN;
                    }
                    else {
                        maxValue = (1ull << (sizeOf << 3)) - 1;
                        minValue = -(int64_t)(1ull << (sizeOf << 3));
                    }
                }
                ADLFlag* str{ output.CreateFlag(name, typeId) };

                if (!str) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' already defined", name));
                    err = true;
                    continue;
                }

                ParseTree* members{ rule->children[rule->children.size() - 2] };

                for (size_t i = 0; i < members->children.size(); i += 2) {
                    RuleContext* enumMember{ dynamic_cast<RuleContext*>(members->children[i]) };

                    ParseTree* enumVal{ enumMember->children[0] };
                    int64_t value{};
                    if (enumMember->children.size() < 2) {
                        value = (int64_t)i / 2;
                    }
                    else {
                        value = NumberNodeValue(enumMember->children[2]);
                    }

                    if (value >= maxValue || value < minValue) {
                        errList->PrintLineMessage(alogs::LVL_ERROR, enumMember, std::format("Out of bounds value for '{}', {} not in [{},{}[", name, value, minValue, maxValue));
                        err = true;
                    }

                    std::string fieldName;
                    if (IS_TERMINAL_TYPE(enumVal, adlParser::STRING)) {

                        auto node = enumVal->getText();
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

                        fieldName = &newStr[0];
                    }
                    else if (IS_RULE_TYPE(enumVal, adlParser::RuleIdf)) {
                        fieldName = enumVal->getText();
                    }
                    else {
                        fieldName = "";
                        errList->PrintLineMessage(alogs::LVL_ERROR, enumVal, std::format("Unhandled flag val {}", enumVal->getText()));
                        err = true;
                    }

                    uint64_t fieldHash{ hash::Hash64(fieldName.c_str()) };

                    for (size_t j = 0; j < str->fields.size(); j++) {
                        if (str->fields[j].name == fieldHash) {
                            errList->PrintLineMessage(alogs::LVL_ERROR, enumVal, std::format("Field already defined for this structure {}", fieldName));
                            err = true;
                            break;
                        }
                    }
                    ADLEnumField& field{ str->fields.emplace_back() };
                    field.name = fieldHash;
                    field.value = value;
                }

                break;
            }
            case adlParser::RuleEnum_def: {

                std::string type{ rule->children[3]->getText() };
                std::string name{ rule->children[1]->getText() };

                ADLDataTypeId typeId{ output.IdOfName(type) };

                if (!IsDefaultType(typeId) || !IsIntType(typeId)) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' doesn't exist or isn't an int type", type));
                    err = true;
                    continue;
                }

                size_t sizeOf{ output.SizeOf(typeId) };
                int64_t maxValue, minValue;
                if (IsUnsignedType(typeId)) {
                    if (sizeOf == 8) {
                        // we do not support uint64
                        maxValue = INT64_MAX;
                    }
                    else {
                        maxValue = (1ull << (sizeOf << 3)) - 1;
                    }
                    minValue = 0;
                }
                else {
                    if (sizeOf == 8) {
                        maxValue = INT64_MAX;
                        minValue = INT64_MIN;
                    }
                    else {
                        maxValue = (1ull << ((sizeOf << 3) - 1)) - 1;
                        minValue = -(maxValue + 1);
                    }
                }

                ADLEnum* str{ output.CreateEnum(name, typeId) };

                if (!str) {
                    errList->PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Type '{}' already defined", name));
                    err = true;
                    continue;
                }

                ParseTree* members{ rule->children[rule->children.size() - 2] };

                for (size_t i = 0; i < members->children.size(); i += 2) {
                    RuleContext* enumMember{ dynamic_cast<RuleContext*>(members->children[i]) };

                    ParseTree* enumVal{ enumMember->children[0] };
                    int64_t value{};
                    if (enumMember->children.size() < 2) {
                        value = (int64_t)i / 2;
                    }
                    else {
                        value = NumberNodeValue(enumMember->children[2]);
                    }

                    if (value > maxValue || value < minValue) {
                        errList->PrintLineMessage(alogs::LVL_ERROR, enumMember, std::format("Out of bounds value for '{}', {} not in [{},{}]", name, value, minValue, maxValue));
                        err = true;
                    }

                    std::string fieldName;
                    if (IS_TERMINAL_TYPE(enumVal, adlParser::STRING)) {

                        auto node = enumVal->getText();
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

                        fieldName = &newStr[0];
                    }
                    else if (IS_RULE_TYPE(enumVal, adlParser::RuleIdf)) {
                        fieldName = enumVal->getText();
                    }
                    else {
                        fieldName = "";
                        errList->PrintLineMessage(alogs::LVL_ERROR, enumVal, std::format("Unhandled flag val {}", enumVal->getText()));
                        err = true;
                    }

                    uint64_t fieldHash{ hash::Hash64(fieldName.c_str()) };

                    for (size_t j = 0; j < str->fields.size(); j++) {
                        if (str->fields[j].name == fieldHash) {
                            errList->PrintLineMessage(alogs::LVL_ERROR, enumVal, std::format("Field already defined for this structure {}", fieldName));
                            err = true;
                            break;
                        }
                    }
                    ADLEnumField& field{ str->fields.emplace_back() };
                    field.name = fieldHash;
                    field.value = value;
                }

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
        if (err) {
            return false;
        }

        // parsing done, we can sync the offsets



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