#include <includes.hpp>
#include "ddlBaseVisitor.h"
#include "ddlLexer.h"
#include "ddlParser.h"
#include "ddlVisitor.h"
#include "compiler/preprocessor.hpp"

using namespace antlr4;
using namespace antlr4::tree;

constexpr ParseTreeType TREE_ERROR = ParseTreeType::ERROR;
constexpr ParseTreeType TREE_RULE = ParseTreeType::RULE;
constexpr ParseTreeType TREE_TERMINAL = ParseTreeType::TERMINAL;

// not the same as in game, for them see pool.cpp
enum ACTSDDLType {
    DLL_INVALID = 0xFF,
    DDL_BYTE_TYPE = 0,
    DDL_SHORT_TYPE,
    DDL_UINT_TYPE,
    DDL_INT_TYPE,
    DDL_UINT64_TYPE,
    DDL_HASH_TYPE,
    DDL_FLOAT_TYPE,
    DDL_FIXEDPOINT_TYPE,
    DDL_STRING_TYPE,
    DDL_PAD_TYPE,
    DDL_CLASS_TYPE,
};

class DDLCompilerOption {
public:
    bool m_help{};
    const char* m_ddl{};
    const char* m_bin{};
    acts::compiler::preprocessor::PreProcessorOption m_processorOpt{};

    bool Compute(const char** args, INT startIndex, INT endIndex) {
        // default values
        for (size_t i = startIndex; i < endIndex; i++) {
            const char* arg = args[i];

            if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
                m_help = true;
            }
            else if (*arg == '-') {
                if (arg[1] == 'D' && arg[2]) {
                    m_processorOpt.defines.insert(arg + 2);
                    continue;
                }

                LOG_ERROR("Unknown option: {}!", arg);
                return false;
            }
            else {
                if (!m_ddl) {
                    m_ddl = arg;
                }
                else if (!m_bin) {
                    m_bin = arg;
                }
                // don't care
            }
        }
        return m_bin != NULL;
    }

    void PrintHelp() {
        LOG_INFO("-h --help              : Print help");
        LOG_INFO("-D[name]               : Define variable");
    }


    std::ostream& PrintLineMessage(std::ostream& out, size_t line, size_t charPositionInLine) {
        out << m_ddl << "#" << line;
        if (charPositionInLine) {
            out << ":" << charPositionInLine;
        }
        return out << " ";
    }
    inline std::ostream& PrintLineMessage(std::ostream& out, Token* token) {
        return PrintLineMessage(out, token->getLine(), token->getCharPositionInLine()) << "(near \"" << token->getText() << "\") ";
    }
    std::ostream& PrintLineMessage(std::ostream& out, ParseTree* token) {
        if (token->getTreeType() == TREE_TERMINAL) {
            return PrintLineMessage(out, dynamic_cast<TerminalNode*>(token)->getSymbol());
        }
        if (token->getTreeType() == TREE_RULE && token->children.size()) {
            // recurse on the first token
            return PrintLineMessage(out, token->children[0]);
        }
        return PrintLineMessage(out, 1, 0);
    }
};

union DDLData {
    double doubleValue;
    uint64_t uintValue;
    int64_t intValue;
};

DDLData ReadDDLMember(uint64_t offset, ACTSDDLType type, uint64_t size, byte* raw) {
    DDLData data{};

    assert(size <= 64 && "reading complex objects");

    byte buffer[8]{};

    // reading raw

    if ((offset & 7) == 0 && (size & 7) == 0) {
        // byte aligned, no need to work
        memcpy(buffer, raw + (offset >> 3), ((size - 1) >> 3) + 1);
    }
    else {
        // TODO: directly copy by chunk instead of copy by bit
        // or maybe it's reversed?
        for (size_t i = 0; i < size; i++) {
            auto idx = offset + i;
            buffer[i >> 3] |= ((raw[idx >> 3] >> (idx & 7)) & 1) << (i & 7);
        }
    }
 
    switch (type) {
    case DDL_BYTE_TYPE:
    case DDL_UINT_TYPE:
    case DDL_UINT64_TYPE:
    case DDL_HASH_TYPE:
        data.uintValue = *(uint64_t*)buffer;
        break;
    case DDL_SHORT_TYPE:
    case DDL_INT_TYPE: {
        // we need to check the higher bit to check the sign
        auto val = *(uint64_t*)buffer;
        if (data.intValue & (1ull << (size - 1))) {
            // negative, we need to add 1 bit at the end
            data.uintValue = val | (~0ull << size);
        }
        else {
            data.intValue = (int64_t)val;
        }
    }
        break;
    case DDL_FLOAT_TYPE:
    case DDL_FIXEDPOINT_TYPE:
        data.doubleValue = *(FLOAT*)buffer;
        break;
    default:
        data.intValue = 0;
        break;
    }

    return data;
}


class DDLMember {
public:
    uint64_t name;
    uint64_t typeName;
    ACTSDDLType type{ DLL_INVALID };
    uint64_t offset{};
    uint64_t size{};
    bool isArray{};
    uint64_t arraySize{};

    DDLMember(uint64_t name, uint64_t typeName) : name(name), typeName(typeName) {
        struct TmpTypeInfo {
            uint64_t tname{};
            ACTSDDLType val{};
            uint64_t size{};
        };

        static TmpTypeInfo tmp[DDL_CLASS_TYPE + 64 + 64 + 64 + 2]{};
        static std::once_flag tmpFlag{};

        std::call_once(tmpFlag, []() {
            tmp[DDL_BYTE_TYPE] = { hash::Hash64("byte"), DDL_BYTE_TYPE, 8 };
            tmp[DDL_SHORT_TYPE] = { hash::Hash64("short"), DDL_SHORT_TYPE, 16 };
            tmp[DDL_UINT_TYPE] = { hash::Hash64("uint"), DDL_UINT_TYPE, 32 };
            tmp[DDL_INT_TYPE] = { hash::Hash64("int"), DDL_INT_TYPE, 32 };
            tmp[DDL_UINT64_TYPE] = { hash::Hash64("uint64"), DDL_UINT64_TYPE, 64 };
            tmp[DDL_HASH_TYPE] = { hash::Hash64("hash"), DDL_HASH_TYPE, 64 };
            tmp[DDL_FLOAT_TYPE] = { hash::Hash64("float"), DDL_FLOAT_TYPE, 32 };
            tmp[DDL_FIXEDPOINT_TYPE] = { hash::Hash64("fixed"), DDL_FIXEDPOINT_TYPE, 32 };
            tmp[DDL_STRING_TYPE] = { hash::Hash64("char"), DDL_STRING_TYPE, 8 };
            tmp[DDL_PAD_TYPE] = { hash::Hash64("pad"), DDL_PAD_TYPE, 1 };

            size_t idx = DDL_CLASS_TYPE;
            
            tmp[idx++] = { hash::Hash64("bool"), DDL_UINT_TYPE, 1 };
            tmp[idx++] = { hash::Hash64("double"), DDL_FLOAT_TYPE, 64 };

            for (size_t i = 1; i <= 64; i++) {
                tmp[idx] = { hash::Hash64(std::format("uint:{}", i)), DDL_UINT_TYPE, i };
                tmp[idx + 1] = { hash::Hash64(std::format("int:{}", i)), DDL_INT_TYPE, i };
                tmp[idx + 2] = { hash::Hash64(std::format("fixed<{}>", i)), DDL_FIXEDPOINT_TYPE, i };

                idx += 3;
            }
        });

        auto it = std::find_if(std::begin(tmp), std::end(tmp), [typeName](const TmpTypeInfo& nfo) { return nfo.tname == typeName; });

        if (it != std::end(tmp)) {
            type = it->val;
            size = it->size;
        }
        else {
            type = DDL_CLASS_TYPE;
            // size is computed during the checks
        }
    }
};

class DDLStruct {
public:
    uint64_t size{};
    std::vector<DDLMember> values{};
};

class DDLEnum {
public:
    std::vector<uint64_t> values{};

    uint64_t GetSize() const {
        // too lazy to write a fast log
        if (values.size() < (1ull << 8)) {
            return 8;
        }
        if (values.size() < (1ull << 16)) {
            return 16;
        }
        if (values.size() < (1ull << 32)) {
            return 32;
        }
        return 64;
    }
};

class DDLCompiled {
public:
    uint64_t name;
    uint32_t version{};
    uint64_t metatable{};
    std::unordered_map<uint64_t, DDLEnum> enums{};
    std::unordered_map<uint64_t, DDLStruct> structs{};


    DDLCompiled(uint64_t name) : name(name) {}

private:
    bool CompleteStruct(DDLStruct& current, std::vector<uint64_t>& types) {
        if (current.size) {
            return true; // already computed
        }
        current.size = 0;
        for (auto& sub : current.values) {
            sub.offset = current.size;
            if (sub.type != DDL_CLASS_TYPE) {
                if (sub.isArray) {
                    current.size += sub.size * sub.arraySize;
                }
                else {
                    current.size += sub.size;
                }
                continue; // by default ok
            }

            auto ite = enums.find(sub.typeName);
            if (ite != enums.end()) {
                sub.size = ite->second.GetSize();
                if (sub.isArray) {
                    current.size += sub.size * sub.arraySize;
                }
                else {
                    current.size += sub.size;
                }
                continue; // ok
            }

            auto it = structs.find(sub.typeName);

            if (it == structs.end()) {
                LOG_ERROR("The type hash_{:x} doesn't exist", sub.typeName);
                return false;
            }

            if (std::find(types.begin(), types.end(), sub.typeName) != types.end()) {
                LOG_ERROR("Recursion on the type hash_{:x}", sub.typeName);
                return false;
            }

            types.push_back(sub.typeName);
            if (!CompleteStruct(it->second, types)) {
                return false;
            }
            sub.size = it->second.size;
            types.pop_back();
            if (sub.isArray) {
                current.size += sub.size * sub.arraySize;
            }
            else {
                current.size += sub.size;
            }
        }
        return true;
    }
public:
    bool StructEnumExists(uint64_t name) const {
        return enums.contains(name) || structs.contains(name);
    }

    bool CompleteStruct() {
        std::vector<uint64_t> types{};
        static auto rootHash = hash::Hash64("root");

        // check that root exists

        if (structs.find(rootHash) == structs.end()) {
            LOG_ERROR("Can't find root structure");
            return false;
        }

        // check loops
        return CompleteStruct(GetRoot(), types);
    }

    DDLStruct& GetRoot() {
        static auto rootHash = hash::Hash64("root");
        return structs[rootHash];
    }
};

class FullDDLCompiled {
public:
    std::vector<DDLCompiled> compiled{};

    bool HasFirst() {
        return compiled.size();
    }

    DDLCompiled& Last() {
        return compiled[compiled.size() - 1];
    }
};

namespace {

    std::string ReadString(ParseTree* pt) {
        auto str = pt->getText();

        return str.substr(1, str.length() - 2);
    }
    int64_t ReadInt(ParseTree* pt) {
        auto str = pt->getText();

        if (str.starts_with("-0x")) return -std::strtoll(str.c_str() + 3, nullptr, 16);
        if (str.starts_with("0x")) return std::strtoll(str.c_str() + 2, nullptr, 16);
        if (str.starts_with("-0b")) return -std::strtoll(str.c_str() + 3, nullptr, 2);
        if (str.starts_with("0b")) return std::strtoll(str.c_str() + 2, nullptr, 2);
        if (str.starts_with("-0")) return -std::strtoll(str.c_str() + 2, nullptr, 8);
        if (str.starts_with("0")) return std::strtoll(str.c_str() + 1, nullptr, 8);
        return std::strtoll(str.c_str(), nullptr, 10);
    }

    void ClearDDLComments(char* ddlText) {
        int comment{}; // 1 = // 2 = /*

        for (; *ddlText; ddlText++) {

            if (comment) {
                if (comment == 1) {
                    if (*ddlText == '\n') {
                        comment = 0;
                        continue;
                    }
                }
                else if (comment == 2) {
                    if (ddlText[0] == '*' && ddlText[1] == '/') {
                        comment = 0;
                        ddlText[0] = ' ';
                        ddlText[1] = ' ';
                        ddlText++;
                        continue;
                    }
                }

                *ddlText = ' ';
                continue;
            }
            if (ddlText[0] == '/') {
                if (ddlText[1] == '/') {
                    comment = 1;
                    ddlText[0] = ' ';
                    ddlText[1] = ' ';
                    ddlText++;
                    continue;
                }

                if (ddlText[1] == '*') {
                    comment = 2;
                    ddlText[0] = ' ';
                    ddlText[1] = ' ';
                    ddlText++;
                    continue;
                }
            }
        }
    }

    class ACTSErrorListener : public ConsoleErrorListener {
        DDLCompilerOption& m_info;
    public:
        ACTSErrorListener(DDLCompilerOption& info) : m_info(info) {
        }

        void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine,
            const std::string& msg, std::exception_ptr e) override {
            if (charPositionInLine) {
                LOG_ERROR("{}:{}#{} : {}", m_info.m_ddl, line, charPositionInLine, msg);
            }
            else if (line) {
                LOG_ERROR("{}:{} : {}", m_info.m_ddl, line, msg);
            }
            else {
                LOG_ERROR("{} : {}", m_info.m_ddl, msg);
            }
        }
    };

    bool ComputeDDLCheck(DDLCompilerOption& opt, std::string& ddlText, byte* binary, size_t binarySize, FullDDLCompiled& ddl) {
        LOG_INFO("Compiling DDL file...");

        opt.m_processorOpt.ApplyPreProcessor(ddlText, 
            [&opt](core::logs::loglevel lvl, size_t line, const std::string& message) { LOG_LVLF(lvl, "{}:{} : {}", opt.m_ddl, line, message); }
        );

        ANTLRInputStream is{ ddlText.data() };

        ddlLexer lexer{ &is };
        CommonTokenStream tokens{ &lexer };

        tokens.fill();
        ddlParser parser{ &tokens };

        auto errList = std::make_unique<ACTSErrorListener>(opt);

        parser.removeErrorListeners();

        parser.addErrorListener(&*errList);

        ddlParser::ProgContext* prog = parser.prog();

        auto error = parser.getNumberOfSyntaxErrors();
        if (error) {
            LOG_ERROR("{} error(s) detected, abort", error);
            return false;
        }


        auto* eof = prog->EOF();

        for (auto& e : prog->children) {
            if (e == eof) {
                break; // done
            }
            if (e->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode&>(*e).getText() == ";") {
                continue; // ignore that
            }

            if (e->getTreeType() != TREE_RULE) {
                opt.PrintLineMessage(std::cerr, e) << "Bad export rule type\n";
                return false;
            }

            auto rule = dynamic_cast<RuleContext&>(*e).getRuleIndex();
            /*
            switch (rule) {
            case ddlParser::RuleData: {
                auto idf = e->children[0]->getText();

                if (idf == "begin") {
                    if (e->children[1]->getTreeType() != TREE_TERMINAL || dynamic_cast<TerminalNode*>(e->children[1])->getSymbol()->getType() != ddlParser::STRING) {
                        opt.PrintLineMessage(std::cerr, e) << "begin should be set using a string value\n";
                        return false;
                    }
                    auto name = ReadString(e->children[1]);
                    ddl.compiled.emplace_back(hash::Hash64Pattern(name.c_str()));
                }
                else if (idf == "version") {
                    if (!ddl.HasFirst()) {
                        opt.PrintLineMessage(std::cerr, e) << "Can't set version without begin\n";
                        return false;
                    }
                    if (e->children[1]->getTreeType() == TREE_TERMINAL) {
                        opt.PrintLineMessage(std::cerr, e) << "version should be set using a number value\n";
                        return false;
                    }
                    ddl.Last().version = (uint32_t)ReadInt(e->children[1]);
                }
                else if (idf == "metatable") {
                    if (!ddl.HasFirst()) {
                        opt.PrintLineMessage(std::cerr, e) << "Can't set metatable without begin\n";
                        return false;
                    }
                    if (e->children[1]->getTreeType() != TREE_TERMINAL || dynamic_cast<TerminalNode*>(e->children[1])->getSymbol()->getType() != ddlParser::STRING) {
                        opt.PrintLineMessage(std::cerr, e) << "metatable should be set using a string value\n";
                        return false;
                    }
                    auto metatable = ReadString(e->children[1]);
                    ddl.Last().metatable = hash::Hash64Pattern(metatable.c_str());
                }
                else {
                    opt.PrintLineMessage(std::cerr, e) << "Invalid data rule: " << idf << ", ignored\n";
                }

            }
                break;
            case ddlParser::RuleEnum: {
                if (!ddl.HasFirst()) {
                    opt.PrintLineMessage(std::cerr, e) << "Can't add enum without begin\n";
                    return false;
                }

                auto name = e->children[1]->getText();

                auto nameHashed = hash::Hash64Pattern(name.c_str());

                if (ddl.Last().StructEnumExists(nameHashed)) {
                    opt.PrintLineMessage(std::cerr, e->children[1]) << "The type " << name << " was registered twice\n";
                    return false;
                }

                auto& enumData = ddl.Last().enums[nameHashed];

                for (size_t i = 3; i < e->children.size() - 1; i++)
                {

                    auto* child = e->children[i];
                    if (child->getText() == "=") {
                        i += 3; // = number, (ignored)
                        continue;
                    }
                    if (child->getText() == ",") {
                        continue;
                    }

                    assert(child->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode&>(*child).getSymbol()->getType() == ddlParser::STRING);

                    auto enumVal = ReadString(child);
                    enumData.values.emplace_back(hash::Hash64Pattern(enumVal.c_str()));
                }
            }
                break;
            case ddlParser::RuleStruct: {
                if (!ddl.HasFirst()) {
                    opt.PrintLineMessage(std::cerr, e) << "Can't add struct without begin\n";
                    return false;
                }

                auto name = e->children[1]->getText();

                auto nameHashed = hash::Hash64Pattern(name.c_str());

                if (ddl.Last().StructEnumExists(nameHashed)) {
                    opt.PrintLineMessage(std::cerr, e->children[1]) << "The type " << name << " was registered twice\n";
                    return false;
                }

                auto& structData = ddl.Last().structs[nameHashed];

                for (size_t i = 3; i < e->children.size() - 1; i++) {
                    auto* def = e->children[i];

                    auto fieldType = def->children[0]->getText();
                    auto fieldName = def->children[1]->getText();

                    ACTSDDLType fieldTypeDDL{};

                    auto fieldTypeHashed = hash::Hash64Pattern(fieldType.c_str());


                    auto& fieldDef = structData.values.emplace_back(hash::Hash64Pattern(name.c_str()), fieldTypeHashed);

                    if (def->children.size() > 5) { // + [ N ] (3)

                        if (def->children.size() > 8) {
                            if (fieldDef.type != DDL_STRING_TYPE) {
                                opt.PrintLineMessage(std::cerr, def->children[5]) << "Can't define matrix\n";
                                return false;
                            }
                            // char str[array_size][string_len];

                            auto strSize = ReadInt(def->children[6]);

                            if (strSize < 0) {
                                opt.PrintLineMessage(std::cerr, def->children[6]) << "String with negative size\n";
                                return false;
                            }

                            fieldDef.size = strSize * 8;

                            fieldDef.isArray = true;
                            auto arrSize = ReadInt(def->children[3]);

                            if (arrSize < 0) {
                                opt.PrintLineMessage(std::cerr, def->children[3]) << "Array with negative size\n";
                                return false;
                            }
                            fieldDef.arraySize = (size_t)arrSize;
                        }
                        else if (fieldDef.type == DDL_STRING_TYPE) {
                            // char str[string_len];


                            auto strSize = ReadInt(def->children[3]);

                            if (strSize < 0) {
                                opt.PrintLineMessage(std::cerr, def->children[3]) << "String with negative size\n";
                                return false;
                            }

                            fieldDef.size = strSize * 8;
                        }
                        else {
                            // or type arr[array_size];

                            fieldDef.isArray = true;
                            auto arrSize = ReadInt(def->children[3]);

                            if (arrSize < 0) {
                                opt.PrintLineMessage(std::cerr, def->children[3]) << "Array with negative size\n";
                                return false;
                            }
                            fieldDef.arraySize = (size_t)arrSize;
                        }

                    }
                }
            }
            break;
            default:
                opt.PrintLineMessage(std::cerr, e) << "Bad export rule\n";
                return false;
            }
            */
        }

        LOG_INFO("Checking DDL structure...");

        bool errorStruct = false;
        for (auto& ddlVersion : ddl.compiled) {
            if (!ddlVersion.CompleteStruct()) {
                errorStruct = true;
            }
        }
        if (errorStruct) {
            opt.PrintLineMessage(std::cerr, prog) << "Error in the structure\n";
            return false;
        }
        LOG_INFO("DDL file compiled.");
        LOG_INFO("Versions: {}.", ddl.compiled.size());

        for (auto& ver : ddl.compiled) {
            auto& root = ver.GetRoot();


            LOG_INFO("Vers: {}", ver.version);
            LOG_INFO("Size: 0x{:x} bits ({})", root.size, root.size);
            LOG_INFO("    | 0x{:x} bytes ({})", (root.size >> 3), (root.size >> 3));

            uLong len = (uLong)((root.size >> 3) + 0x100000);
            auto decompiledBuffer = std::make_unique<byte[]>(len);

            LOG_INFO("len: {}", len);

            if (binarySize >= 2 && binary[0] == 0x78 && binary[1] == 0x9C) {
                // ZLIB encoded
                uLong binSizeUL = (uLong)binarySize;
                auto res = uncompress2(&decompiledBuffer[0], &len, binary, &binSizeUL);
                if (res != Z_OK) {
                    const char* ermsg;
                    switch (res) {
                        case Z_MEM_ERROR: ermsg = "Not enough memory"; break;
                        case Z_BUF_ERROR: ermsg = "Not enough room in the output buffer"; break;
                        case Z_DATA_ERROR: ermsg = "Corrupted data"; break;
                        default: ermsg = "unknown"; break;
                    }
                    LOG_ERROR("Can't uncompress zlib buffer: {}", ermsg);
                    return false;
                }
                LOG_INFO("zlib decompressed");
            }
            else {
                memcpy(&decompiledBuffer[0], binary, binarySize);
            }
            LOG_INFO("len: {}", len);
            break; // only use first version

        }
        return true;
    }

	int ddlcheck(Process& proc, int argc, const char* argv[]) {
        DDLCompilerOption opt{};

        if (!opt.Compute(argv, 2, argc) || opt.m_help) {
            opt.PrintHelp();
            return 0;
        }

        std::string ddl{};
        std::string bin{};

        if (!utils::ReadFile(opt.m_ddl, ddl)) {
            LOG_ERROR("Can't read {}", opt.m_ddl);
            return tool::BASIC_ERROR;
        }

        if (!utils::ReadFile(opt.m_bin, bin)) {
            LOG_ERROR("Can't read {}", opt.m_bin);
            return tool::BASIC_ERROR;
        }
        FullDDLCompiled ddlCompiled{};

        auto res = ComputeDDLCheck(opt, ddl, reinterpret_cast<byte*>(bin.data()), bin.size(), ddlCompiled);

		return res ? tool::OK : tool::BASIC_ERROR;
	}


}
ADD_TOOL(ddlcheck, "common", "", "ddl check", nullptr, ddlcheck);