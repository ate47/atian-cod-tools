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

struct FunctionVar {
    std::string name;
    size_t id;
    byte flags;
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
            LOG_ERROR("Can't find opcode {} for vm {}/{}", (int)opcode, ctx.vmInfo->name, PlatformName(ctx.plt));
            
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
    uint64_t clsName;
    int flags;
    byte params;

    AscmNodeFunctionCall(OPCode opcode, int flags, byte params, uint64_t clsName) : AscmNodeOpCode(opcode), flags(flags), params(params), clsName(clsName) {
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
            ctx.Write<uint64_t>(0x1234567887654321ull);
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
            ctx.Write<uint32_t>(hashutils::Hash32Pattern(var.name.c_str()));
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
    std::set<std::string> defines{};
};

class GscCompilerOption {
public:
    bool m_help = false;
    VmInfo* m_vmInfo{};
    Platform m_platform = Platform::PLATFORM_PC;
    std::vector<const char*> m_inputFiles{};
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
    std::pair<const char*, FunctionVar*> RegisterVar(const std::string& name, bool allowExisting, byte flags = 0) {
        auto it = FindVar(name);
        if (it != VarEnd()) {
            if (allowExisting) {
                return std::make_pair<>(nullptr, &*it);
            }
            return std::make_pair<>(utils::va("The var '%s' already exists", name.c_str()), nullptr);
        }

        if (m_allocatedVar >= ARRAYSIZE(m_vars)) {
            return std::make_pair<>(utils::va("Can't create var '%s': too much variable for function", name.c_str()), nullptr);
        }

        auto& var = m_vars[m_allocatedVar] = { name, m_allocatedVar, flags };
        m_allocatedVar++;

        return std::make_pair<>(nullptr, &var);
    }

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
        bool align = m_vmInfo->flags & VmFlags::VMF_OPCODE_SHORT;

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
    InputInfo& info;
    GscFileType type;
    uint32_t currentNamespace = hashutils::Hash32("");
    std::set<uint64_t> includes{};
    std::unordered_map<uint32_t, FunctionObject> exports{};
    std::unordered_map<std::string, RefObject> strings{};
    std::unordered_map<uint64_t, std::vector<ImportObject>> imports{};
    std::unordered_map<std::string, GlobalVarObject> globals{};
    VmInfo* vmInfo;
    Platform plt;

    std::unordered_set<std::string> hashes{};

    CompileObject(GscFileType file, InputInfo& nfo, VmInfo* vmInfo, Platform plt) : type(file), info(nfo), vmInfo(vmInfo), plt(plt) {
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

    bool Compile(std::vector<byte>& data) {
        utils::Allocate(data, sizeof(tool::gsc::T8GSCOBJ));

        LOG_TRACE("Compile {} include(s)...", includes.size());
        size_t incTable = utils::Allocate(data, sizeof(uint64_t) * includes.size());

        uint64_t* tab = reinterpret_cast<uint64_t*>(&data[incTable]);

        for (uint64_t i : includes) {
            *tab = i;
            tab++;
        }

        size_t expTable = utils::Allocate(data, sizeof(tool::gsc::T8GSCExport) * exports.size());

        size_t csegOffset = data.size();

        LOG_TRACE("Compile {} export(s)...", exports.size());

        size_t exportIndex{};
        for (auto& [name, exp] : exports) {
            if (exp.m_nodes.empty()) {
                LOG_ERROR("No nodes for {:x}", exp.m_name);
                return false;
            }

            struct PreExp { uint64_t top; uint64_t bottom; };

            utils::Aligned<PreExp>(data);
            utils::Allocate(data, sizeof(PreExp));

            int32_t len = exp.ComputeRelativeLocations((int32_t)data.size());
            if (len < 0) {
                LOG_ERROR("Error when allocating relative locations");
                return false;
            }

            tool::gsc::T8GSCExport& e = reinterpret_cast<tool::gsc::T8GSCExport*>(&data[expTable])[exportIndex++];

            exp.location = data.size();

            e.name = (uint32_t)exp.m_name;
            e.name_space = (uint32_t)exp.m_name_space;
            e.callback_event = (uint32_t)exp.m_data_name;
            e.flags = exp.m_flags;
            e.address = (int32_t)exp.location;
            e.param_count = exp.m_params;
            e.checksum = 0x12345678;


            AscmCompilerContext cctx{ vmInfo, plt, exp.m_allocatedVar, data };

            for (auto* node : exp.m_nodes) {
                if (!node->Write(cctx)) {
                    return false;
                }
            }
        }
        size_t csegSize = data.size() - csegOffset;

        LOG_TRACE("Compile {} strings(s)...", strings.size());

        // compile strings

        for (auto& [key, strobj] : strings) {
            // TODO: check vm, in mwiii it's not the same
            strobj.location = (uint32_t)data.size();
            data.push_back(0x9f);
            data.push_back((byte)(key.length() + 1));
            //data.push_back(0x92);
            utils::WriteString(data, key.c_str());
        }

        size_t stringRefs = data.size();
        size_t stringCount{};

        for (auto& [key, strobj] : strings) {
            size_t w{};
            while (w < strobj.nodes.size()) {
                if (w % 0xFF == 0) {
                    size_t buff = utils::Allocate(data, sizeof(tool::gsc::T8GSCString));
                    tool::gsc::T8GSCString* str = reinterpret_cast<tool::gsc::T8GSCString*>(&data[buff]);
                    str->string = strobj.location;
                    str->type = 0;
                    str->num_address = (byte)((strobj.nodes.size() - w) > 0xFF ? 0xFF : (strobj.nodes.size() - w));
                    stringCount++;
                }
                utils::WriteValue<uint32_t>(data, strobj.nodes[w++]->GetDataFLoc(vmInfo->flags & VmFlags::VMF_OPCODE_SHORT));
            }
        }

        size_t gvarRefs = data.size();
        size_t gvarCount{};

        for (auto& [key, gvobj] : globals) {
            size_t w{};
            while (w < gvobj.nodes.size()) {
                if (w % 0xFF == 0) {
                    size_t buff = utils::Allocate(data, sizeof(tool::gsc::T8GSCGlobalVar));
                    tool::gsc::T8GSCGlobalVar* gv = reinterpret_cast<tool::gsc::T8GSCGlobalVar*>(&data[buff]);
                    gv->name = (uint32_t)vmInfo->HashField(key.c_str());
                    gv->num_address = (byte)((gvobj.nodes.size() - w) > 0xFF ? 0xFF : (gvobj.nodes.size() - w));
                    gvarCount++;
                }
                utils::WriteValue<uint32_t>(data, gvobj.nodes[w++]->GetDataFLoc(vmInfo->flags & VmFlags::VMF_OPCODE_SHORT));
            }
        }

        size_t implRefs = data.size();
        size_t implCount{};

        for (auto& [key, imobj] : imports) {
            auto [nsp, func] = utils::UnCatLocated(key);
            for (ImportObject& implData : imobj) {
                size_t w{};
                while (w < implData.nodes.size()) {
                    if (w % 0xFFFF == 0) {
                        size_t buff = utils::Allocate(data, sizeof(tool::gsc::T8GSCImport));
                        tool::gsc::T8GSCImport* gv = reinterpret_cast<tool::gsc::T8GSCImport*>(&data[buff]);
                        gv->import_namespace = nsp;
                        gv->name = func;
                        gv->param_count = implData.params;
                        gv->flags = implData.flags;
                        gv->num_address = (byte)((implData.nodes.size() - w) > 0xFFFF ? 0xFFFF : (implData.nodes.size() - w));
                        implCount++;
                    }
                    utils::WriteValue<uint32_t>(data, implData.nodes[w++]->floc);
                }
            }
        }

        // compile header

        auto* prime_obj = reinterpret_cast<tool::gsc::T8GSCOBJ*>(data.data());
        *reinterpret_cast<uint64_t*>(prime_obj->magic) = 0x36000a0d43534780;

        prime_obj->include_offset = (int32_t)incTable;
        prime_obj->include_count = (int16_t)includes.size();

        prime_obj->string_offset = (int32_t)stringRefs;
        prime_obj->string_count = (int16_t)stringCount;

        prime_obj->globalvar_offset = (int32_t)gvarRefs;
        prime_obj->globalvar_count = (int16_t)gvarCount;

        prime_obj->export_table_offset = (int32_t)expTable;
        prime_obj->exports_count = (int16_t)exports.size();
        prime_obj->cseg_offset = (int32_t)csegOffset;
        prime_obj->cseg_size = (int32_t)csegSize;

        prime_obj->imports_offset = (int32_t)implRefs;
        prime_obj->imports_count = (int16_t)implCount;

        prime_obj->script_size = (int32_t)data.size();

        return true;
    }
};

#define IS_RULE_TYPE(rule, index) (rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
#define IS_TERMINAL_TYPE(term, index) (term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)

bool ParseFieldNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool withVal) {
    if (!exp) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "empty tree error");
        return false;
    }
    if (exp->getTreeType() == TREE_ERROR) {
        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "detected tree error, bad syntax?");
        return false;
    }

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

                fobj.m_nodes.push_back(loc.node);
            }

            return ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, expressVal);
        }
        case gscParser::RuleStatement_if: {
            bool ok{ true };
            auto* elseStart = new AscmNode();
            if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.m_nodes.push_back(new AscmNodeJump(elseStart, OPCODE_JumpOnFalse));

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                ok = false;
            }


            if (rule->children.size() > 5) { // else
                auto* endElse = new AscmNode();
                fobj.m_nodes.push_back(new AscmNodeJump(endElse, OPCODE_Jump));
                fobj.m_nodes.push_back(elseStart);

                if (!ParseExpressionNode(rule->children[6], parser, obj, fobj, false)) {
                    ok = false;
                }

                fobj.m_nodes.push_back(endElse);
            }
            else {
                fobj.m_nodes.push_back(elseStart);
            }

            return ok;
        }
        case gscParser::RuleStatement_while: {
            bool ok{ true };
            auto* loopBreak = new AscmNode();
            auto* loopContinue = new AscmNode();

            fobj.PushContinueNode(loopContinue);
            fobj.PushBreakNode(loopBreak);

            fobj.m_nodes.push_back(loopContinue);

            if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.m_nodes.push_back(new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                ok = false;
            }

            fobj.m_nodes.push_back(loopBreak);

            fobj.PopContinueNode();
            fobj.PopBreakNode();

            return ok;
        }
        case gscParser::RuleStatement_dowhile: {
            bool ok{ true };
            auto* loopStart = new AscmNode();
            auto* loopBreak = new AscmNode();
            auto* loopContinue = new AscmNode();

            fobj.PushContinueNode(loopContinue);
            fobj.PushBreakNode(loopBreak);

            fobj.m_nodes.push_back(loopStart);

            if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, false)) {
                ok = false;
            }

            fobj.m_nodes.push_back(loopContinue);

            if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                ok = false;
            }

            fobj.m_nodes.push_back(new AscmNodeJump(loopStart, OPCODE_JumpOnTrue));

            fobj.m_nodes.push_back(loopBreak);

            fobj.PopContinueNode();
            fobj.PopBreakNode();

            return ok;
        }
        case gscParser::RuleStatement_for: {
            // todo
            bool err{};
            size_t i = 2;

            // init expression
            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                err = true;
            }

            i++; // skip ';'
            auto* loopStart = new AscmNode();
            auto* loopBreak = new AscmNode();
            auto* loopContinue = new AscmNode();

            fobj.m_nodes.push_back(loopStart);


            // if expression
            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression)) {
                if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                    err = true;
                }
                else {
                    fobj.m_nodes.push_back(new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));
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
            fobj.m_nodes.push_back(loopContinue);

            if (IS_RULE_TYPE(rule->children[i], gscParser::RuleExpression) && !ParseExpressionNode(rule->children[i++], parser, obj, fobj, false)) {
                err = true;
            }

            fobj.m_nodes.push_back(new AscmNodeJump(loopStart, OPCODE_Jump));
            fobj.m_nodes.push_back(loopBreak);

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

            fobj.m_nodes.push_back(new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableRefCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));


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
                    auto* jmpNode = new AscmNode();

                    if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                        ok = false;
                    }
                    fobj.m_nodes.push_back(new AscmNodeVariable(var->id, OPCODE_EvalLocalVariableCached));
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Equal));


                    fobj.m_nodes.push_back(new AscmNodeJump(jmpNode, OPCODE_JumpOnTrue));

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
            auto* endSwitchNode = new AscmNode();
            fobj.PushBreakNode(endSwitchNode);

            if (defaultCase.jmpNode) {
                fobj.m_nodes.push_back(new AscmNodeJump(defaultCase.jmpNode, OPCODE_Jump));
            }
            else {
                fobj.m_nodes.push_back(new AscmNodeJump(endSwitchNode, OPCODE_Jump));
            }

            for (auto& caseElem : jmpTable) {
                fobj.m_nodes.push_back(caseElem.jmpNode);

                for (size_t stmt = caseElem.startStmt; stmt < caseElem.endStmt; stmt++) {
                    if (!ParseExpressionNode(rule->children[stmt], parser, obj, fobj, true)) {
                        ok = false;
                    }
                }
            }

            fobj.m_nodes.push_back(endSwitchNode);

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

            if (!ParseExpressionNode(rule->children[rule->children.size() - 3], parser, obj, fobj, true)) {
                ok = false;
            }
            // array = ...;
            fobj.m_nodes.push_back(new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableRefCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
            // key = firstarray(array);
            fobj.m_nodes.push_back(new AscmNodeVariable(arrayVal->id, OPCODE_FirstArrayKeyCached));
            fobj.m_nodes.push_back(new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));

            auto* loopIt = new AscmNode();
            auto* loopBreak = new AscmNode();
            auto* loopContinue = new AscmNode();

            fobj.PushBreakNode(loopBreak);
            fobj.PushContinueNode(loopContinue);

            fobj.m_nodes.push_back(loopIt);
            // jumpiffalse(isdefined(key)) loopBreak
            // todo: check if we can use EvalLocalVariableDefined
            fobj.m_nodes.push_back(new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IsDefined));
            fobj.m_nodes.push_back(new AscmNodeJump(loopBreak, OPCODE_JumpOnFalse));

            // value = array[key];
            fobj.m_nodes.push_back(new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
            fobj.m_nodes.push_back(new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_EvalArray));
            fobj.m_nodes.push_back(new AscmNodeVariable(valueVar->id, OPCODE_SetNextArrayKeyCached));

            // nextarray(array, key, iterator)
            fobj.m_nodes.push_back(new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
            fobj.m_nodes.push_back(new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
            fobj.m_nodes.push_back(new AscmNodeVariable(iteratorVal->id, OPCODE_SetNextArrayKeyCached));

            if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                ok = false;
            }

            fobj.m_nodes.push_back(loopContinue);
            // key = iterator;
            fobj.m_nodes.push_back(new AscmNodeVariable(iteratorVal->id, OPCODE_EvalLocalVariableCached));
            fobj.m_nodes.push_back(new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableRefCached));
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));

            // loop back
            fobj.m_nodes.push_back(new AscmNodeJump(loopIt, OPCODE_Jump));

            fobj.PopBreakNode();
            fobj.PopContinueNode();
            // end
            fobj.m_nodes.push_back(loopBreak);

            return ok;
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

                auto* loc = fobj.PeekBreakNode();

                if (!loc) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "not in a break context");
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeJump(loc, OPCODE_Jump));
                return true;
            }

            if (idf == "continue") {
                if (rule->children.size() > 1) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "can't specify jump location with continue");
                    return false;
                }

                auto* loc = fobj.PeekContinueNode();

                if (!loc) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "not in a continue context");
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeJump(loc, OPCODE_Jump));
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

                fobj.m_nodes.push_back(new AscmNodeJump(loc.node, OPCODE_Jump));
                return true;
            }

            if (idf == "return") {
                if (rule->children.size() <= 1) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
                    return true;
                }

                if (!ParseExpressionNode(rule->children[1], parser, obj, fobj, true)) {
                    return false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Return));
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

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Wait));
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

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_WaitFrame));
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
                std::string funcName = functionComp->children[0]->getText();

                funcHash = obj.vmInfo->HashField(funcName.c_str());

                auto funcIt = obj.vmInfo->opFuncs.find(funcHash);

                if (funcIt != obj.vmInfo->opFuncs.end()) {
                    // internal function call
                    auto& f = funcIt->second;
                    if (f.HasFlag(tool::gsc::opcode::VPFD_SELF_PARAM) && !(flags & FCF_METHOD)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator should have a caller {}, Usage: {}", funcName, f.usage));
                        return false;
                    }
                    if (expressVal && !f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator doesn't return a value {}, Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (f.HasFlag(tool::gsc::opcode::VPFD_USE_PRE_SCRIPT_CALL)) {
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));
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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Not enought params for operator {}, Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (paramCount > f.maxParam) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Too many params for operator {}, Usage: {}", funcName, f.usage));
                        return false;
                    }

                    if (f.HasFlag(tool::gsc::opcode::VPFD_USE_COUNT)) {
                        fobj.m_nodes.push_back(new AscmNodeData<byte>((byte)paramCount, f.opCode));
                    }
                    else {
                        fobj.m_nodes.push_back(new AscmNodeOpCode(f.opCode));
                    }

                    if (!expressVal && f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                    }

                    return true;
                }
            }
            else if (functionComp->children.size() == 3) {
                if (IS_TERMINAL_TYPE(functionComp->children[0], gscParser::IDENTIFIER)) {
                    // namespace::func

                    std::string funcNspName = functionComp->children[0]->getText();
                    std::string funcName = functionComp->children[2]->getText();

                    funcNspHash = obj.vmInfo->HashField(funcNspName.c_str());
                    funcHash = obj.vmInfo->HashField(funcName.c_str());
                }
                else {
                    // [[ espression ]]
                    ptrTree = functionComp->children[1];
                    flags |= FCF_POINTER;
                }
            }
            else if (functionComp->children.size() == 5) {
                if (flags & FCF_METHOD) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, "A class call can't have a self caller");
                    return false;
                }
                // [[ espression ]] -> func
                std::string funcName = functionComp->children[4]->getText();
                funcHash = obj.vmInfo->HashField(funcName.c_str());

                ptrTree = functionComp->children[1];
                flags |= FCF_POINTER_CLASS | FCF_POINTER;
            }
            else {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Function call not implemented {}", functionComp->getText()));
                return false;
            }

            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));
            
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

            // TODO: add opcode/ref
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
                fobj.m_nodes.push_back(new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash));
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

                auto* funcCall = new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash);

                // link by the game, but we write it for test
                auto located = utils::CatLocated((uint32_t)funcNspHash, (uint32_t)funcHash);

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

                fobj.m_nodes.push_back(funcCall);
            }


            if (!expressVal) {
                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
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
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj, expressVal)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Inc));
                        if (expressVal) {
                            // convert to ++var when we do not need the expressed val
                            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                        }
                    }
                    else if (op == "--") {
                        // var--
                        if (!ParseFieldNode(rule->children[0], parser, obj, fobj, expressVal)) {
                            return false; // can't parse field
                        }
                        fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Dec));
                        if (expressVal) {
                            // convert to --var when we do not need the expressed val
                            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
                        }
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
            if (rule->children.size() == 5) {
                // e ? e : e
                bool ok{ true };

                if (!ParseExpressionNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }

                auto* caseNo = new AscmNode();
                fobj.m_nodes.push_back(new AscmNodeJump(caseNo, OPCODE_JumpOnFalse));

                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                auto* caseEnd = new AscmNode();
                fobj.m_nodes.push_back(new AscmNodeJump(caseEnd, OPCODE_Jump));

                fobj.m_nodes.push_back(caseNo);

                if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.m_nodes.push_back(caseEnd);

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
                auto* after = new AscmNode();

                fobj.m_nodes.push_back(new AscmNodeJump(after, op == "&&" ? OPCODE_JumpOnFalseExpr : OPCODE_JumpOnTrueExpr));

                // push op right
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                // after the operator
                fobj.m_nodes.push_back(after);

                if (!expressVal) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
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
                else if (op == "<<") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_ShiftLeft));
                }
                else if (op == ">>") {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_ShiftRight));
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("unhandled operator: {}", op));
                    ok = false;
                }
                if (!expressVal) {
                    fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_DecTop));
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
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("useless value: {}", rule->getText()));
                return true;
            }
            if (
                !ParseExpressionNode(rule->children[5], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[3], parser, obj, fobj, true),
                !ParseExpressionNode(rule->children[1], parser, obj, fobj, true)
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
            bool ok{ true };
            size_t currentKey{};
            for (size_t i = 1; i + 2 < rule->children.size(); i++) {
                if (rule->children[i]->getTreeType() != TREE_RULE || dynamic_cast<RuleContext*>(rule->children[i])->getRuleIndex() != gscParser::RuleExpression) {
                    // '#'? key ':'

                    if (rule->children[i]->getText() == "#") {
                        i++; // skip '#'
                    }

                    if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    i++; // ':'
                }
                else {
                    // push current key
                    fobj.m_nodes.push_back(BuildAscmNodeData(currentKey++));
                }

                if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_AddToArray));

                i++; // ',' or ']'
            }

            return ok;
        }
        case gscParser::RuleStruct_def: {
            if (!expressVal) { // no need to create struct
                obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("useless value: {}", rule->getText()));
                return true;
            }
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CreateStruct));

            bool ok{ true };
            // 1 = [ (HASHSTR|'#' number) ':'
            for (size_t i = 1; i < rule->children.size(); i++) {

                auto* term = rule->children[i];

                auto termStr = term->getText();

                auto sub = termStr.substr(1, termStr.size() - 1);
                fobj.m_nodes.push_back(new AscmNodeData<uint64_t>(obj.vmInfo->HashField(sub.c_str()), OPCODE_GetHash));

                i++; // ':'


                if (!ParseExpressionNode(rule->children[i], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_AddToStruct));

                i++; // ',' or '}'
            }
            return ok;
        }
        case gscParser::RuleSet_expression: {
            std::string opVal = rule->children[1]->getText();

            bool ok{ true };
            if (opVal == "=") {
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, expressVal)) {
                    ok = false;
                }
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj, false)) {
                    ok = false;
                }
                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
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
                    ok = false;
                }

                // dogshit way, but it seems to be the same in their compiler...
                if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
                if (expressVal) {
                    if (!ParseFieldNode(rule->children[0], parser, obj, fobj, true)) {
                        ok = false;
                    }
                }
                return ok;
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

            byte flags = tool::gsc::T8GSCImportFlags::FUNC_METHOD;

            if (rule->children.size() == 4) {
                // with nsp
                auto nspStr = rule->children[1]->getText();
                nsp = hashutils::Hash32Pattern(nspStr.c_str());
            }
            else {
                flags |= tool::gsc::T8GSCImportFlags::GET_CALL;
            }

            assert(rule->children.size());

            auto funcStr = rule->children[rule->children.size() - 1]->getText();
            auto func = hashutils::Hash32Pattern(funcStr.c_str());

            // link by the game, but we write it for test
            auto located = utils::CatLocated(nsp, func);
            auto* asmc = new AscmNodeData<uint64_t>(located, OPCODE_GetFunction);
            fobj.m_nodes.push_back(asmc);

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

        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("unhandled rule: {} ({})", rule->getText(), rule->getRuleIndex()));
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

        if (varName == "self") {
            fobj.m_nodes.push_back(new AscmNodeOpCode(OPCODE_GetSelf));
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
            fobj.m_nodes.push_back(gvar);
            return true;
        }

        auto varIt = fobj.FindVar(varName);

        if (varIt == fobj.VarEnd()) {
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
        fobj.m_nodes.push_back(asmc);

        std::string key{ &newStr[0]};

        LOG_TRACE("Allocate str {}", key);

        if (key.length() >= 256) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("string too long: {}", term->getText()));
            return false;
        }

        auto& str = obj.strings[key];
        str.nodes.push_back(asmc);
        LOG_TRACE("Done str {}", key);
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
    uint32_t nameHashed = hashutils::Hash32Pattern(name.data());

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
            uint32_t evnameHashed = hashutils::Hash32Pattern(evname.data());
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
                if (obj.vmInfo->vm < 0x37) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not implemented: {}", modifier));
                    return false;
                }
                idfFlags = tool::gsc::opcode::T9_VAR_REF;
            }
            else if (modifier == "&") {
                // ref (T8)
                if (obj.vmInfo->vm < 0x36) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("modifier not implemented: {}", modifier));
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
            auto defaultValueExp = param->children[param->children.size() - 1];
            assert(IS_RULE_TYPE(defaultValueExp, gscParser::RuleExpression));

            /*
                if (!isdefined(param)) {
                    param = defaultValueExp;
                }
             */
            exp.m_nodes.push_back(new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableCached));
            exp.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IsDefined));
            auto* afterNode = new AscmNode();
            exp.m_nodes.push_back(new AscmNodeJump(afterNode, OPCODE_JumpOnTrue));
            if (!ParseExpressionNode(defaultValueExp, parser, obj, exp, true)) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, defaultValueExp, std::format("can't create expression node for variable {}", paramIdf));
                return false;
            }
            exp.m_nodes.push_back(new AscmNodeVariable(vardef->id, OPCODE_EvalLocalVariableRefCached));
            exp.m_nodes.push_back(new AscmNodeOpCode(OPCODE_SetVariableField));
            exp.m_nodes.push_back(afterNode);
        }

    }

    // handle block

    // weirdly, their gsc compiler is converting top level breaks to jump to the end of the function
    auto* endNode = new AscmNodeOpCode(OPCODE_End);
    exp.PushBreakNode(endNode);

    if (!ParseExpressionNode(blockRule, parser, obj, exp, false)) {
        return false;
    }

    exp.PopBreakNode();

    exp.m_nodes.push_back(endNode);

    for (auto& [name, loc] : exp.m_jumpLocs) {
        if (loc.defined) {
            continue;
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, loc.def ? loc.def : blockRule, std::format("the location {} was used, but isn't declared", name));

        if (loc.node) {
            delete loc.node; // free loc
            loc.node = nullptr;
        }
    }
    exp.m_nodes.insert(exp.m_nodes.begin(), exp.CreateParamNode());

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

size_t FindEndLineDelta(const char* d) {
    const char* s = d;
    while (*d && *d != '\n') {
        d++;
    }
    return d - s;
}

bool ApplyPreProcessor(std::string& str, PreProcessorOption& opt) {
    const char* start = str.data();
    char* d = str.data();

    // todo
    return true;
}

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

        if (!utils::ReadFileNotAlign(file, reinterpret_cast<void*&>(dt.buffer), dt.size, true)) {
            LOG_ERROR("Can't read file {}", file);
            return tool::BASIC_ERROR;
        }

        size_t lineCount = 1; // 1 for the one we'll add at the end

        const char* b = dt.buffer;
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

    if (!ApplyPreProcessor(info.gscData, opt.processorOpt)) {
        LOG_ERROR("Error when applying preprocessor on GSC data");
        return tool::BASIC_ERROR;
    }
    if (!ApplyPreProcessor(info.cscData, opt.processorOpt)) {
        LOG_ERROR("Error when applying preprocessor on CSC data");
        return tool::BASIC_ERROR;
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

    utils::WriteFile("compiled.gscc", (const void*)data.data(), data.size());

    LOG_INFO("Done into compiled.gscc");


    return 0;
}

#ifndef CI_BUILD
ADD_TOOL("gscc", " --help", "gsc compiler", nullptr, compiler);
#endif
