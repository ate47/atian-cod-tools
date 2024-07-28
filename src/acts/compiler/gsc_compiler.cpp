#include "gscBaseVisitor.h"
#include "gscLexer.h"
#include "gscParser.h"
#include "gscVisitor.h"
#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_acts_debug.hpp"
#include "compiler/preprocessor.hpp"

namespace acts::compiler {
    using namespace antlr4;
    using namespace antlr4::tree;
    using namespace tool::gsc::opcode;

    #pragma push_macro("ERROR")
    #undef ERROR
    constexpr ParseTreeType TREE_ERROR = ParseTreeType::ERROR;
    constexpr ParseTreeType TREE_RULE = ParseTreeType::RULE;
    constexpr ParseTreeType TREE_TERMINAL = ParseTreeType::TERMINAL;
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
        uint64_t script{};
    };
    struct LocatedHash {
        size_t operator()(const Located& k) const {
            return k.name_space ^ RotateLeft64(k.name, 21) ^ RotateLeft64(k.script, 42);
        }
    };
    struct LocatedEquals {
        bool operator()(const Located& a, const Located& b) const {
            return a.name == b.name && a.name_space == b.name_space && a.script == b.script;
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
            return vmInfo->HasFlag(VmFlags::VMF_ALIGN);
        }

        void Align(size_t len) {
            if (HasAlign()) {
                size_t pre = data.size();
                size_t post = (data.size() + (len - 1)) & ~(len - 1);

                for (size_t i = pre; i < post; i++) {
                    data.push_back(0);
                }
            }
            // not required
        }

        template<typename Type>
        void Align() {
            Align(sizeof(Type));
        }
        template<typename Type>
        void Write(Type value) {
            utils::WriteValue<Type>(data, value);
        }
    };

    enum AscmNodeType {
        ASCMNT_UNKNOWN = 0,
        ASCMNT_OPCODE = 1,
        ASCMNT_OPCODE_RAW = 1,
    };

    class AscmNodeJump;
    class AscmNodeOpCode;

    class AscmNode {
    public:
        int32_t rloc{};
        int32_t floc{};
        size_t line{};
        std::vector<AscmNodeJump*> refs{};
        AscmNodeType nodetype{ ASCMNT_UNKNOWN };

        virtual ~AscmNode() {};

        virtual uint32_t ShiftSize(uint32_t start, bool aligned) const {
            return start; // empty by default
        }

        virtual bool Write(AscmCompilerContext& ctx) {
            // nothing by default
            return true;
        }

        bool HasRef() const {
            return refs.size();
        }

        bool IsOpCode() const {
            return nodetype == ASCMNT_OPCODE;
        }

        AscmNodeOpCode* AsOpCode();
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
                LOG_ERROR("Can't find opcode {} ({}) for vm {}/{}", utils::PtrOrElse(OpCodeName(opcode), "null"), (int)opcode, ctx.vmInfo->name, PlatformName(ctx.plt));

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

    AscmNodeOpCode* AscmNode::AsOpCode() {
        if (nodetype == ASCMNT_OPCODE) {
            return dynamic_cast<AscmNodeOpCode*>(this);
        }
        return nullptr;
    }

    class AscmNodeRawOpCode : public AscmNode {
    public:
        int16_t opcode;

        AscmNodeRawOpCode(int16_t opcode) : opcode(opcode) {
            nodetype = ASCMNT_OPCODE_RAW;
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            if (aligned) {
                return utils::Aligned<uint16_t>(start) + sizeof(uint16_t);
            }
            return start + 1;
        }

        bool Write(AscmCompilerContext& ctx) override {
            ctx.Align<uint16_t>();
            if (ctx.HasAlign()) {
                ctx.Write<uint16_t>(opcode);
            }
            else {
                ctx.Write<byte>((byte)opcode);
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

    class AscmNodeHash : public AscmNodeOpCode {
    public:
        uint64_t val;
        uint32_t hashSize;

        AscmNodeHash(uint64_t val, OPCode opcode, uint32_t hashSize) : AscmNodeOpCode(opcode), val(val), hashSize(hashSize) {
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            if (aligned) {
                return ((AscmNodeOpCode::ShiftSize(start, aligned) + (hashSize - 1)) & ~(hashSize - 1)) + hashSize;
            }
            return AscmNodeOpCode::ShiftSize(start, aligned) + hashSize;
        }

        bool Write(AscmCompilerContext& ctx) override {
            if (!AscmNodeOpCode::Write(ctx)) {
                return false;
            }

            if (hashSize == 8) {
                ctx.Align<uint64_t>();
                ctx.Write<uint64_t>(val);
            }
            else if (hashSize == 4) {
                ctx.Align<uint32_t>();
                ctx.Write<uint32_t>((uint32_t)val);
            }
            else {
                throw std::runtime_error(utils::va("hash%d not implemented in AscmNodeHash", hashSize * 8));
            }
            return true;
        }

        uint32_t GetDataFLoc(bool aligned) const {
            return ShiftSize(floc, aligned) - hashSize;
        }
    };

    enum FuncCallFlags {
        FCF_THREAD = 0x1,
        FCF_CHILDTHREAD = 0x2,
        FCF_METHOD = 0x4,
        FCF_POINTER = 0x8,
        FCF_POINTER_CLASS = 0x10,
        FCF_BUILTIN = 0x20,
        FCF_GETTER = 0x40,
    };

    class AscmNodeFunctionCall : public AscmNodeOpCode {
    public:
        uint64_t nameSpace;
        uint64_t clsName;
        // linked by the compiler after the parsing
        bool isScriptCall{};
        int flags;
        byte params;
        bool inlineCall;

        AscmNodeFunctionCall(OPCode opcode, int flags, byte params, uint64_t clsName, uint64_t nameSpace, VmInfo* vm)
            : AscmNodeOpCode(opcode), flags(flags), params(params), clsName(clsName), nameSpace(nameSpace) {
            inlineCall = vm->HasFlag(VmFlags::VMF_ALIGN);
        }

        void SetScriptCall(CompileObject& obj, bool scriptCall);

        void LoadData(bool& useParams, uint32_t& dataSize) const {
            if (inlineCall) {
                switch (opcode) {
                case OPCODE_GetFunction:
                case OPCODE_IW_GetBuiltinFunction:
                case OPCODE_IW_GetBuiltinMethod:
                case OPCODE_GetResolveFunction:
                    useParams = false;
                    break;
                default:
                    useParams = true;
                    break;
                }
                dataSize = (flags & FCF_POINTER) ? 0 : 8;
            }
            else {
                switch (opcode) {
                case OPCODE_ScriptThreadCallPointerEndOn:
                case OPCODE_ScriptThreadCallPointer:
                case OPCODE_ScriptMethodThreadCallPointer:
                case OPCODE_ScriptMethodThreadCallPointerEndOn:
                case OPCODE_IW_BuiltinFunctionCallPointer:
                case OPCODE_IW_BuiltinMethodCallPointer:
                    dataSize = 0;
                    useParams = true;
                    break;
                case OPCODE_ScriptFunctionCallPointer:
                case OPCODE_ScriptMethodCallPointer:
                    dataSize = 0;
                    useParams = false;
                    break;
                case OPCODE_ScriptThreadCallEndOn:
                case OPCODE_ScriptThreadCall:
                case OPCODE_ScriptMethodThreadCallEndOn:
                case OPCODE_ScriptMethodThreadCall:
                    dataSize = 4;
                    useParams = true;
                    break;
                case OPCODE_ScriptFunctionCall:
                case OPCODE_ScriptMethodCall:
                    dataSize = 4;
                    useParams = false;
                    break;
                case OPCODE_CallBuiltinFunction:
                case OPCODE_CallBuiltinMethod:
                    dataSize = 2;
                    useParams = true;
                    break;
                case OPCODE_GetFunction:
                case OPCODE_IW_GetBuiltinFunction:
                case OPCODE_IW_GetBuiltinMethod:
                    dataSize = 2;
                    useParams = false;
                    break;
                case OPCODE_GetResolveFunction:
                    dataSize = 4;
                    useParams = false;
                    break;
                default: {
                    const char* opcodeName{ tool::gsc::opcode::OpCodeName(opcode) };
                    throw std::runtime_error(utils::va("invalid opcode for func call %s", opcodeName ? opcodeName : "null"));
                }
                }
            }
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            bool useParams;
            uint32_t dataSize;
            LoadData(useParams, dataSize);
            if (aligned) {
                if (flags & FCF_POINTER_CLASS) {
                    return utils::Aligned<uint32_t>(AscmNodeOpCode::ShiftSize(start, aligned) + 1) + (uint32_t)sizeof(uint32_t);
                }
                if (flags & FCF_POINTER) {
                    return AscmNodeOpCode::ShiftSize(start, aligned) + 1;
                }
                return utils::Aligned<uint64_t>(AscmNodeOpCode::ShiftSize(start, aligned) + (useParams ? 1 : 0)) + dataSize;
            }

            start = AscmNodeOpCode::ShiftSize(start, aligned);
            if (useParams) {
                start++;
            }
            start += dataSize;

            return start;
        }

        bool Write(AscmCompilerContext& ctx) override {
            if (!AscmNodeOpCode::Write(ctx)) {
                return false;
            }

            bool useParams;
            uint32_t dataSize{};
            LoadData(useParams, dataSize);

            if (useParams) {
                ctx.Write<byte>(params);
            }

            if (flags & FCF_POINTER_CLASS) {
                ctx.Align<uint32_t>();
                ctx.Write<uint32_t>((uint32_t)clsName);
            }
            else if (!(flags & FCF_POINTER)) {
                // replaced by the linker
                ctx.Align(dataSize);
                if (dataSize == 8) {
                    ctx.Write<uint64_t>(utils::CatLocated((uint32_t)nameSpace, (uint32_t)clsName));
                }
                else {
                    for (size_t i = 0; i < dataSize; i++) {
                        ctx.Write<byte>((byte)i);
                    }
                }
            }

            return true;
        }

        uint32_t GetFLoc() const {
            if (inlineCall) {
                return floc;
            }

            if (opcode == OPCODE_CallBuiltinFunction || opcode == OPCODE_CallBuiltinMethod) {
                return floc + 2;
            }
            return floc + 1;
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

    class AscmNodeCreateLocalVariables : public AscmNode {
        std::vector<FunctionVar> vars{};
        size_t params{};
        bool hasRegister;
        bool hasRegisters;
        bool hasFlag;
        uint32_t hashSize;
    public:
        AscmNodeCreateLocalVariables(const FunctionVar* lvars, size_t count, size_t params, const FunctionObject& fobj);

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            static AscmNodeOpCode opCodeUndef{ OPCODE_Undefined };
            // opcode
            uint32_t e = opCodeUndef.ShiftSize(start, aligned);

            if (hasRegister || hasRegisters) {
                if (params) {
                    e++; // add params count

                    for (size_t i = 0; i < params; i++) {
                        if (aligned) {
                            e = (e + (hashSize - 1)) & ~(hashSize - 1);
                        }

                        e += hashSize;

                        if (hasFlag) {
                            e++;
                        }
                    }
                }
                if (hasRegister) {
                    for (size_t i = params; i < vars.size(); i++) {
                        // register opcode
                        e = opCodeUndef.ShiftSize(e, aligned);
                        if (aligned) {
                            e = (e + (hashSize - 1)) & ~(hashSize - 1);
                        }

                        e += hashSize;

                        if (hasFlag) {
                            e++;
                        }
                    }
                }
                else if (hasRegisters) {
                    if (vars.size() - params) {
                        e = opCodeUndef.ShiftSize(e, aligned);
                        e++; // add var count

                        if (aligned) {
                            e = (e + (hashSize - 1)) & ~(hashSize - 1);
                        }

                        e += hashSize;

                        if (hasFlag) {
                            e++;
                        }
                    }
                }
            }
            else if (vars.size()) {
                // count
                e += 1;
                for (size_t i = 0; i < vars.size(); i++) {
                    if (aligned) {
                        e = (e + (hashSize - 1)) & ~(hashSize - 1);
                    }

                    e += hashSize;

                    if (hasFlag) {
                        e++;
                    }
                }
            }
            // else we only have the CheckClearParams
            return e;
        }

        bool Write(AscmCompilerContext& ctx) override {
            if (hasRegister || hasRegisters) {
                if (params) {
                    // add params
                    AscmNodeOpCode opCode{ OPCODE_SafeCreateLocalVariables };
                    if (!opCode.Write(ctx)) {
                        return false;
                    }

                    ctx.Write<byte>((byte)params);

                    for (size_t i = 0; i < params; i++) {
                        FunctionVar& var = vars[i];

                        if (ctx.vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
                            ctx.Align<uint64_t>();
                            ctx.Write<uint64_t>(ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        else {
                            ctx.Align<uint32_t>();
                            ctx.Write<uint32_t>((uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        if (hasFlag) {
                            ctx.Write<byte>(var.flags);
                        }
                    }
                }
                else {
                    // check no params
                    AscmNodeOpCode opCode{ OPCODE_CheckClearParams };
                    if (!opCode.Write(ctx)) {
                        return false;
                    }
                }

                // register the variables
                if (hasRegister) {
                    AscmNodeOpCode opCode{ OPCODE_IW_RegisterVariable };
                    for (size_t i = params; i < vars.size(); i++) {
                        FunctionVar& var = vars[i];

                        if (!opCode.Write(ctx)) {
                            return false;
                        }

                        if (ctx.vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
                            ctx.Align<uint64_t>();
                            ctx.Write<uint64_t>(ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        else {
                            ctx.Align<uint32_t>();
                            ctx.Write<uint32_t>((uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        if (hasFlag) {
                            ctx.Write<byte>(var.flags);
                        }
                    }
                }
                else if (hasRegisters) {
                    if (vars.size() - params) {
                        AscmNodeOpCode opCode{ OPCODE_IW_RegisterMultipleVariables };
                        if (!opCode.Write(ctx)) {
                            return false;
                        }
                        ctx.Write<byte>((byte)(vars.size() - params));
                        for (size_t i = params; i < vars.size(); i++) {
                            FunctionVar& var = vars[i];

                            if (ctx.vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
                                ctx.Align<uint64_t>();
                                ctx.Write<uint64_t>(ctx.vmInfo->HashField(var.name.c_str()));
                            }
                            else {
                                ctx.Align<uint32_t>();
                                ctx.Write<uint32_t>((uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
                            }
                            if (hasFlag) {
                                ctx.Write<byte>(var.flags);
                            }
                        }
                    }
                }

                return true;
            }
        
            if (vars.size()) {
                AscmNodeOpCode opCode{ OPCODE_SafeCreateLocalVariables };
                if (!opCode.Write(ctx)) {
                    return false;
                }

                ctx.Write<byte>((byte)vars.size());

                for (FunctionVar& var : vars) {
                    if (ctx.vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
                        ctx.Align<uint64_t>();
                        ctx.Write<uint64_t>(ctx.vmInfo->HashField(var.name.c_str()));
                    }
                    else {
                        ctx.Align<uint32_t>();
                        ctx.Write<uint32_t>((uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
                    }
                    if (hasFlag) {
                        ctx.Write<byte>(var.flags);
                    }
                }

                return true;
            }


            AscmNodeOpCode opCode{ OPCODE_CheckClearParams };
            return opCode.Write(ctx);
        }
    };

    class AscmNodeGlobalVariable : public AscmNodeOpCode {
    public:
        GlobalVariableDef* def;

        AscmNodeGlobalVariable(GlobalVariableDef* def, bool ref) : 
            AscmNodeOpCode(ref ? OPCODE_GetGlobalObject : OPCODE_GetGlobal), def(def) {
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            if (aligned) {
                return utils::Aligned<uint16_t>(AscmNodeOpCode::ShiftSize(start, aligned)) + sizeof(uint16_t);
            }
            return AscmNodeOpCode::ShiftSize(start, aligned) + 2;
        }

        bool Write(AscmCompilerContext& ctx) override {
            if (!AscmNodeOpCode::Write(ctx)) {
                return false;
            }

            ctx.Align<uint16_t>(); // not really required, but still good
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

    #define IS_RULE_TYPE(rule, index) (rule && rule->getTreeType() == TREE_RULE && dynamic_cast<RuleContext*>(rule)->getRuleIndex() == index)
    #define IS_TERMINAL_TYPE(term, index) (term && term->getTreeType() == TREE_TERMINAL && dynamic_cast<TerminalNode*>(term)->getSymbol()->getType() == index)
    #define IS_IDF(rule) (IS_RULE_TYPE((rule), gscParser::RuleIdf) || IS_TERMINAL_TYPE((rule), gscParser::IDENTIFIER))

    int64_t NumberNodeValue(ParseTree* number) {
        if (IS_RULE_TYPE(number, gscParser::RuleNumber)) {
            return NumberNodeValue(number->children[0]);
        }
        if (!number || number->getTreeType() != TREE_TERMINAL) {
            return 0; // wtf?
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(number);
        switch (term->getSymbol()->getType()) {
        case gscParser::INTEGER10:
            return std::strtoll(term->getText().c_str(), nullptr, 10);
        case gscParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 16);
            return neg ? -val : val;
        }
        case gscParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), nullptr, 8);
            return neg ? -val : val;
        }
        case gscParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            int64_t val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 2);
            return neg ? -val : val;
        }
        default:
            return 0;
        }
    }

    class AscmNodeJump : public AscmNodeOpCode {
    public:
        AscmNode* const location;
        AscmNodeJump(AscmNode* location, OPCode opcode) : AscmNodeOpCode(opcode), location(location) {
            location->refs.push_back(this);
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

    enum DetourType {
        DETOUR_UNKNOWN = 0,
        DETOUR_GSIC,
        DETOUR_ACTS
    };

    class GscCompilerOption {
    public:
        bool m_help{};
        bool m_computeDevOption{};
        const char* m_preproc{};
        VmInfo* m_vmInfo{};
        Platform m_platform{ Platform::PLATFORM_PC };
        const char* m_outFileName{ "compiled" };
        std::vector<const char*> m_inputFiles{};
        bool m_client{};
        int64_t crcServer{};
        int64_t crcClient{};
        const char* nameServer{ "" };
        const char* nameClient{ "" };
        const char* fileNameSpaceServer{ "" };
        const char* fileNameSpaceClient{ "" };
        DetourType detourType{ DETOUR_UNKNOWN };
        preprocessor::PreProcessorOption processorOpt{};

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
                else if (!_strcmpi("--dev-block-as-comment", arg)) {
                    processorOpt.devBlockAsComment = true;
                    LOG_WARNING("{} used, this message is just here to remind you that you're stupid.", arg);
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
                else if (!_strcmpi("--namespace", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    fileNameSpaceServer = args[++i];
                }
                else if (!_strcmpi("--namespace-client", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    fileNameSpaceClient = args[++i];
                }
                else if (!_strcmpi("--detour", arg)) {
                    if (i + 1 == endIndex) {
                        LOG_ERROR("Missing value for param: {}!", arg);
                        return false;
                    }
                    const char* dt = args[++i];
                    if (!_strcmpi(dt, "acts")) {
                        detourType = DETOUR_ACTS;
                    }
                    else if (!_strcmpi(dt, "gsic")) {
                        detourType = DETOUR_GSIC;
                    }
                    else if (_strcmpi(dt, "none")) {
                        LOG_ERROR("Invalid value value for param '{}': {}!", arg, dt);
                        return false;
                    }
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
            LOG_INFO("-h --help              : Print help");
            {
                std::ostringstream games;

                size_t idx{};
                for (const auto& [vm, vmInfo] : tool::gsc::opcode::GetVMMaps()) {
                    if (idx++) games << ",";
                    games << " '" << vmInfo.internalName << "'";
                }

                LOG_INFO("-g --game [g]          : Set game, values:{}.", games.str());
            }
            {
                std::ostringstream platforms;

                size_t idx{};

                for (size_t i = 1; i < tool::gsc::opcode::PLATFORM_COUNT; i++) {
                    const char* plt = tool::gsc::opcode::PlatformIdName((tool::gsc::opcode::Platform)i);
                    if (i) platforms << ",";
                    platforms << " '" << plt << "'";
                }

                LOG_INFO("-p --platform [p]      : Set platform, values:{}.", platforms.str());
            }
            LOG_INFO("-d --dbg               : Add dev options");
            LOG_INFO("-o --output [f]        : Set output file (without extension), default: 'compiled'");
            LOG_INFO("-D[name]               : Define variable");
            LOG_INFO("-c --csc               : Build client script with csc files");
            LOG_INFO("--detour [t]           : Set the detour compilation type ('none' / 'acts' / 'gsic') default: 'none'");
            LOG_INFO("--crc [c]              : Set the crc for the server script");
            LOG_INFO("--crc-client [c]       : Set the crc for the client script");
            LOG_INFO("--name [n]             : Set the name for the server script");
            LOG_INFO("--name-client [n]      : Set the name for the client script");
            LOG_INFO("--namespace [n]        : Set the file namespace for the server script");
            LOG_INFO("--namespace-client [n] : Set the file namespace for the client script");
            LOG_INFO("--dev-block-as-comment : Consider /# #/ as comment markers");
            LOG_DEBUG("--preproc [f]         : Export preproc result into f");
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


        const GscFile& FindFile(size_t line) const {
            for (const GscFile& f : files) {
                if (line >= f.startLine && line < f.startLine + f.sizeLine) {
                    return f;
                }
            }
            return files[files.size() - 1];
        }

        void PrintLineMessage(alogs::loglevel lvl, size_t line, size_t charPositionInLine, const std::string& msg) const {
            const GscFile& f = FindFile(line);
        
            size_t localLine;

            if (line > f.startLine) {
                localLine = line - f.startLine;
            }
            else {
                localLine = f.sizeLine;
            }

            if (charPositionInLine) {
                LOG_LVL(lvl, "{}#{}:{} {}", f.filename.string(), localLine, charPositionInLine, msg);
            }
            else {
                LOG_LVL(lvl, "{}#{} {}", f.filename.string(), localLine, msg);
            }
        }
        Token* GetToken(ParseTree* tree) const {
            while (tree && tree->getTreeType() != TREE_TERMINAL && tree->children.size()) {
                tree = tree->children[0];
            }

            if (tree && tree->getTreeType() == TREE_TERMINAL) {
                return dynamic_cast<TerminalNode*>(tree)->getSymbol();
            }
            return nullptr;
        }
        void PrintLineMessage(alogs::loglevel lvl, ParseTree* tree, const std::string& msg) const {
            Token* token = GetToken(tree);
            if (token) {
                PrintLineMessage(lvl, token->getLine(), token->getCharPositionInLine(), msg);
                return;
            }

            alogs::log(lvl, msg);
        }
    };

    class StringObject {
    public:
        uint32_t location{};
        size_t forceLen{};
        std::vector<uint32_t*> listeners{};
        std::vector<AscmNodeData<uint32_t>*> nodes{};
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

    struct FunctionJumpLoc {
        AscmNode* node{};
        ParseTree* def{};
        bool defined{};
    };

    struct DetourData {
        uint64_t nsp{};
        uint64_t script{};
        uint64_t func{};
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
        size_t size{};
        size_t rndVarStart{};
        FunctionVar m_vars[256]{};
        size_t m_allocatedVar{};
        std::vector<AscmNode*> m_nodes{};
        std::stack<AscmNode*> m_jumpBreak{};
        std::stack<AscmNode*> m_jumpContinue{};
        std::unordered_map<std::string, FunctionJumpLoc> m_jumpLocs{};
        VmInfo* m_vmInfo;
        DetourData detour{};
        FunctionObject(
            CompileObject& obj,
            uint64_t name,
            uint64_t name_space,
            uint64_t file_name_space,
            VmInfo* vmInfo
        ) : obj(obj), m_name(name), m_name_space(name_space), m_data_name(file_name_space), m_vmInfo(vmInfo) {
        }
        ~FunctionObject() {
            for (AscmNode* node : m_nodes) {
                delete node;
            }
        }

        bool IsDetour() const {
            return detour.func || detour.nsp || detour.script;
        }

        AscmNode* CreateParamNode() const {
            return new AscmNodeCreateLocalVariables(m_vars, m_allocatedVar, (size_t)m_params, *this);
        }

        AscmNode* CreateFieldHash(uint64_t v, OPCode op) const {
            return new AscmNodeHash(v, op, m_vmInfo->HasFlag(VmFlags::VMF_HASH64) ? 8 : 4);
        }

        AscmNode* CreateFieldHash(const char* v, OPCode op) const;

        AscmNode* CreateFieldHash(const std::string& v, OPCode op) const;

        AscmNode* PeekBreakNode() const {
            if (m_jumpBreak.size()) {
                return m_jumpBreak.top();
            }
            return nullptr;
        }

        AscmNode* PeekContinueNode() const {
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
            bool align = m_vmInfo->HasFlag(VmFlags::VMF_ALIGN) && m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16);

            for (AscmNode* node : m_nodes) {
                node->rloc = current;
                node->floc = align ? utils::Aligned<uint16_t>(floc + current) : (floc + current);
                current = node->ShiftSize(current, align);
                if (node->rloc > current) {
                    return -1;
                }
            }
            return current;
        }

        void OptimizeNodes() {
            // todo: find something to do?
        }
    };


    class CompileObject {
    public:
        size_t devBlockDepth;
        GscCompilerOption& opt;
        InputInfo& info;
        GscFileType type;
        uint64_t currentNamespace;
        uint64_t fileName{};
        const char* fileNameStr{};
        uint64_t fileNameSpace{};
        int64_t crc{};
        std::set<std::string> includes{};
        std::vector<AscmNode*> m_devBlocks{};
        std::unordered_map<uint64_t, FunctionObject> exports{};
        std::unordered_map<std::string, StringObject> strings{};
        
        std::unordered_map<Located, std::vector<AscmNodeLazyLink*>, LocatedHash, LocatedEquals> lazyimports{};
        std::unordered_map<Located, std::vector<ImportObject>, LocatedHash, LocatedEquals> imports{};
        std::unordered_map<std::string, GlobalVarObject> globals{};
        VmInfo* vmInfo;
        Platform plt;
        int64_t autoexecOrder{};
        std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler;
        std::unordered_set<std::string> hashes{};
        size_t emptyNameInc{};

        CompileObject(GscCompilerOption& opt, GscFileType file, InputInfo& nfo, VmInfo* vmInfo, Platform plt, std::shared_ptr<tool::gsc::GSCOBJHandler> gscHandler) 
            : opt(opt), type(file), info(nfo), vmInfo(vmInfo), plt(plt), gscHandler(gscHandler) {
            bool client = type == FILE_CSC;
            crc = client ? opt.crcClient : opt.crcServer;

            if (!crc) {
                crc = gscHandler->GetDefaultChecksum(client);
            }

            const char* fn = client ? opt.nameClient : opt.nameServer;
            fileNameStr = fn && *fn ? fn : nullptr;

            if (!fileNameStr) {
                fileNameStr = gscHandler->GetDefaultName(client);
            }

            fileName = vmInfo->HashPath(fileNameStr);
            const char* fns = client ? opt.fileNameSpaceClient : opt.fileNameSpaceServer;
            fileNameSpace = fns && *fns ? vmInfo->HashFilePath(opt.fileNameSpaceClient) : 0;

            currentNamespace = fileNameSpace;
        }

        void AddHash(const std::string& str) {
            const char* strc{ str.c_str() };
            if (!hash::HashPattern(strc)) {
                hashes.insert(str);
                hashutils::Add(strc);
            }
        }

        void AddHash(const char* str) {
            if (!hash::HashPattern(str)) {
                hashes.insert(str);
                hashutils::Add(str);
            }
        }

        void AddInclude(std::string& data) {
            includes.insert(data);
        }

        bool HasOpCode(OPCode opcode) {
            auto [ok, op] = GetOpCodeId(vmInfo->vm, plt, opcode);
            return ok;
        }

        /*
         * Compute the node using the minimum amount of bits
         * @param val value to encode
         * @return node
         */
        AscmNodeOpCode* BuildAscmNodeData(int64_t val) {
            if (val == 0) {
                return new AscmNodeOpCode(OPCODE_GetZero);
            }
            if (val > 0) {
                if (val <= 0x7F && HasOpCode(OPCODE_GetSignedByte)) {
                    return new AscmNodeData<int8_t>((int8_t)val, OPCODE_GetSignedByte);
                }
                if (val <= 0xFF && HasOpCode(OPCODE_GetByte)) {
                    return new AscmNodeData<uint8_t>((uint8_t)val, OPCODE_GetByte);
                }
                if (val <= 0x7FFF && HasOpCode(OPCODE_GetShort)) {
                    return new AscmNodeData<int16_t>((int16_t)val, OPCODE_GetShort);
                }
                if (val <= 0xFFFF && HasOpCode(OPCODE_GetUnsignedShort)) {
                    return new AscmNodeData<uint16_t>((uint16_t)val, OPCODE_GetUnsignedShort);
                }
                if (val <= 0x7FFFFFFF && HasOpCode(OPCODE_GetInteger)) {
                    return new AscmNodeData<int32_t>((int32_t)val, OPCODE_GetInteger);
                }
                if (val <= 0xFFFFFFFF && HasOpCode(OPCODE_GetUnsignedInteger)) {
                    return new AscmNodeData<uint32_t>((uint32_t)val, OPCODE_GetUnsignedInteger);
                }
            }
            else {
                if (val >= -0x7F && HasOpCode(OPCODE_GetSignedByte)) {
                    return new AscmNodeData<int8_t>((int8_t)(val), OPCODE_GetSignedByte);
                }
                if (val >= -0xFF && HasOpCode(OPCODE_GetNegByte)) {
                    return new AscmNodeData<uint8_t>((uint8_t)(-val), OPCODE_GetNegByte);
                }
                if (val >= -0x7FFF && HasOpCode(OPCODE_GetShort)) {
                    return new AscmNodeData<int16_t>((int16_t)(val), OPCODE_GetShort);
                }
                if (val >= -0xFFFF && HasOpCode(OPCODE_GetNegUnsignedShort)) {
                    return new AscmNodeData<uint16_t>((uint16_t)(-val), OPCODE_GetNegUnsignedShort);
                }
                if (val >= -0x7FFFFFFF && HasOpCode(OPCODE_GetInteger)) {
                    return new AscmNodeData<int32_t>((int32_t)val, OPCODE_GetInteger);
                }
                if (val >= -0xFFFFFFFFLL && HasOpCode(OPCODE_GetNegUnsignedInteger)) {
                    return new AscmNodeData<uint32_t>((uint32_t)(-val), OPCODE_GetNegUnsignedInteger);
                }
            }

            return new AscmNodeData<int64_t>(val, OPCODE_GetLongInteger);
        }

        bool TryHashNodeValue(ParseTree* hashNode, uint64_t& output) {
            if (!hashNode) {
                return false; // wtf?
            }
            while (hashNode->getTreeType() != TREE_TERMINAL) {
                if (hashNode->getTreeType() == TREE_ERROR) {
                    info.PrintLineMessage(alogs::LVL_ERROR, hashNode, "Tree error");
                    return false;
                }
                if (hashNode->children.size() != 1) {
                    info.PrintLineMessage(alogs::LVL_ERROR, hashNode, "Not a hash expression");
                    return false;
                }

                hashNode = hashNode->children[0];
            }

            TerminalNode* term = dynamic_cast<TerminalNode*>(hashNode);
            switch (term->getSymbol()->getType()) {
            case gscParser::HASHSTRING: {
                std::string hash = term->getText();
                char type = hash[0];
                std::string sub = hash.substr(2, hash.length() - 3);
                auto ith = vmInfo->hashesFunc.find(type);

                if (ith == vmInfo->hashesFunc.end()) {
                    info.PrintLineMessage(alogs::LVL_ERROR, hashNode, std::format("Hash type not available for this vm: {}", type));
                    return false;
                }

                const char* ss = sub.c_str();

                if (!hash::TryHashPattern(ss, output)) {
                    output = ith->second.hashFunc(ss);
                    if (!output) {
                        info.PrintLineMessage(alogs::LVL_ERROR, hashNode, std::format("Can't hash the string '{}' with the type {}", sub, type));
                        return false;
                    }
                    AddHash(sub);
                }
                return true;
            }
            case gscParser::STRING: {
                std::string node = term->getText();
                auto newStr = std::make_unique<char[]>(node.length() - 1);
                char* newStrWriter = &newStr[0];

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

                constexpr char type = '#'; // default type

                std::string key{ &newStr[0] };
                auto ith = vmInfo->hashesFunc.find(type);

                if (ith == vmInfo->hashesFunc.end()) {
                    info.PrintLineMessage(alogs::LVL_ERROR, hashNode, std::format("Hash type not available for this vm: {}", type));
                    return false;
                }

                const char* ss = key.c_str();

                if (!hash::TryHashPattern(ss, output)) {
                    output = ith->second.hashFunc(ss);
                    if (!output) {
                        info.PrintLineMessage(alogs::LVL_ERROR, hashNode, std::format("Can't hash the string '{}' with the type {}", node, type));
                        return false;
                    }
                    AddHash(key);
                }
                return true;
            }
            default:
                info.PrintLineMessage(alogs::LVL_ERROR, hashNode, "Not a hash expression, only string or hash node available");
                return false;
            }
        }

        bool Compile(std::vector<byte>& data) {
            union {
                AscmNode* opcode;
                uint32_t strlistener;
                
            } crcData{};
            bool forceDebugHeader{};
            if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
                // add the notify crc function for T9 38 vm
                constexpr const char* name = "$notif_checkum";
                uint64_t nameHashed = vmInfo->HashField(name);
                AddHash(name);
                auto [res, err] = exports.try_emplace(nameHashed, *this, nameHashed, currentNamespace, fileNameSpace, vmInfo);

                if (!err) {
                    LOG_ERROR("Can't register notif checksum export: {}", name);
                    return false;
                }

                FunctionObject& f = res->second;
                f.autoexecOrder = INT64_MIN; // first
                f.m_flags = tool::gsc::CLASS_VTABLE;
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CheckClearParams));
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));
                if (crc < 0) {
                    f.m_nodes.push_back(crcData.opcode = new AscmNodeData<uint32_t>((uint32_t)(-crc), OPCODE_GetNegUnsignedInteger));
                }
                else {
                    f.m_nodes.push_back(crcData.opcode = new AscmNodeData<uint32_t>((uint32_t)(crc), OPCODE_GetUnsignedInteger));
                }
                forceDebugHeader = true;

                auto gvarIt = vmInfo->globalvars.find(vmInfo->HashField("level"));

                if (gvarIt == vmInfo->globalvars.end()) {
                    LOG_ERROR("Can't find level def for checksum export: {}", name);
                    return false;
                }

                GlobalVariableDef& gv = gvarIt->second;

                GlobalVarObject& decl = globals[gv.name];

                if (!decl.def) {
                    decl.def = &gv;
                }

                AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(decl.def, false);
                decl.nodes.emplace_back(gvar);
                f.m_nodes.push_back(gvar);
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_Notify));
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
            }
            else if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
                // add the notify crc function for JUP B vm

                constexpr const char* name = "$notif_checkum";
                uint64_t nameHashed = vmInfo->HashField(name);
                AddHash(name);
                auto [res, err] = exports.try_emplace(nameHashed, *this, nameHashed, currentNamespace, fileNameSpace, vmInfo);

                if (!err) {
                    LOG_ERROR("Can't register notif checksum export: {}", name);
                    return false;
                }

                FunctionObject& f = res->second;
                f.autoexecOrder = INT64_MIN; // first
                f.m_flags = tool::gsc::CLASS_VTABLE;
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_CheckClearParams));
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_PreScriptCall));

                const char* crcStr = utils::va("%lld", crc);

                StringObject& strdef = strings[crcStr];
                auto* getstr = new AscmNodeData<uint32_t>(0xFFFFFFFF, OPCODE_GetString);
                strdef.nodes.push_back(getstr);
                // add some padding so we can patch the crc at runtime
                strdef.forceLen = 0x20;
                strdef.listeners.push_back(&crcData.strlistener);
                forceDebugHeader = true;
                f.m_nodes.push_back(getstr);
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IW_GetLevel));
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_IW_Notify));
                f.m_nodes.push_back(new AscmNodeOpCode(OPCODE_End));
            }

            // set builtin call types for jup VM
            if (!gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_GET_API_SCRIPT)) {
                // we need to compute if an import is from a builtin or a script
                for (auto& [idx, imps] : imports) {
                
                    uint64_t name = idx.name;
                    uint64_t nsp = idx.name_space;

                    auto it = exports.find(name);

                    bool scriptCall{ it != exports.end() && it->second.m_name_space == nsp };

                    for (ImportObject& imp : imps) {
                        if (imp.flags & tool::gsc::GET_CALL) {
                            int funcType{ imp.flags & 0xF };
                            if (funcType == tool::gsc::FUNC_METHOD) {
                                // ref
                                if (!scriptCall) {
                                    LOG_ERROR("Getter for builtin methods not implemented in this vm yet");
                                    return false;
                                }
                            }

                            if (!scriptCall) {
                                if (funcType == tool::gsc::FUNCTION_CHILDTHREAD
                                    || funcType == tool::gsc::FUNCTION_THREAD
                                    || funcType == tool::gsc::METHOD_CHILDTHREAD
                                    || funcType == tool::gsc::METHOD_THREAD) {
                                    LOG_ERROR("Usage of thread modifier on a builtin call {}", hashutils::ExtractTmp("function", idx.name));
                                    return false;
                                }
                                if (funcType == tool::gsc::METHOD) {
                                    imp.flags = (imp.flags & ~0xF) | tool::gsc::ACTS_CALL_BUILTIN_METHOD;
                                }
                                else if (funcType == tool::gsc::FUNCTION) {
                                    imp.flags = (imp.flags & ~0xF) | tool::gsc::ACTS_CALL_BUILTIN_FUNCTION;
                                }
                                else {
                                    LOG_ERROR("Unknown builtin call {} {}", funcType, hashutils::ExtractTmp("function", idx.name));
                                    return false;
                                }
                            }

                            for (AscmNodeFunctionCall* node : imp.nodes) {
                                node->SetScriptCall(*this, scriptCall);
                            }
                        }
                        else {
                            for (AscmNodeFunctionCall* node : imp.nodes) {
                                node->SetScriptCall(*this, true);
                            }
                        }
                    }
                }

            }

            utils::Allocate(data, gscHandler->GetHeaderSize());
            size_t actsDebugHeader{};
            if (forceDebugHeader || opt.m_computeDevOption || opt.detourType == DETOUR_ACTS) {
                actsDebugHeader = utils::Allocate(data, sizeof(tool::gsc::acts_debug::GSC_ACTS_DEBUG));
            }

            LOG_TRACE("Compile {} include(s)...", includes.size());
            size_t incTable{};
            if (includes.size()) {
                if (gscHandler->HasFlag(tool::gsc::GOHF_STRING_NAMES)) {
                    // store the offsets
                    incTable = utils::Allocate(data, sizeof(uint32_t) * includes.size());

                    size_t idx{};

                    for (const std::string& i : includes) {
                        reinterpret_cast<uint32_t*>(&data[incTable])[idx++] = (uint32_t)data.size();
                        utils::WriteString(data, i.data());
                    }
                }
                else {
                    // store the hashes
                    incTable = utils::Allocate(data, sizeof(uint64_t) * includes.size());
                    uint64_t* tab = reinterpret_cast<uint64_t*>(&data[incTable]);

                    for (const std::string& i : includes) {
                        std::string data{ i };
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
                        AddHash(data);

                        *tab = vmInfo->HashPath(data.data());
                        tab++;
                    }
                }
            }

            size_t expTable = utils::Allocate(data, gscHandler->GetExportSize() * exports.size());

            size_t csegOffset = data.size();

            LOG_TRACE("Compile {} export(s)...", exports.size());

            size_t exportIndex{};

            std::vector<FunctionObject*> detourObjs{};

            CompileObject& that = *this;
            auto writeExport = [&data, &that, &expTable, &exportIndex, &detourObjs](FunctionObject& exp) -> bool {
                exp.OptimizeNodes();
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

                for (AscmNode* node : exp.m_nodes) {
                    if (!node->Write(cctx)) {
                        return false;
                    }
                }
                // add size for detours
                exp.size = data.size() - exp.location;

                if (exp.IsDetour()) {
                    detourObjs.push_back(&exp);
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
            for (FunctionObject* exp : autoexecs) {
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
            if (!detourObjs.empty() && !opt.detourType) {
                LOG_ERROR("Detour parsed, but no --detour has been specified, they will be ignored.");
            }

            size_t csegSize = data.size() - csegOffset;

            LOG_TRACE("Compile {} strings(s)...", strings.size());

            // compile strings

            for (auto& [key, strobj] : strings) {
                // TODO: check vm, in mwiii it's not the same
                strobj.location = (uint32_t)data.size();
                for (uint32_t* lis : strobj.listeners) {
                    *lis = strobj.location;
                }
                if (vmInfo->vm == VM_T8) {
                    data.push_back(0x9f);
                    data.push_back((byte)(key.length() + 1));
                }
                utils::WriteString(data, key.c_str());
                // add bytes to the string
                if (key.length() < strobj.forceLen) {
                    for (size_t i = key.length(); i < strobj.forceLen; i++) {
                        data.push_back(0);
                    }
                }
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
                    utils::WriteValue<uint32_t>(data, strobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
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
                            AddHash(key);
                            gv.name = (uint32_t)vmInfo->HashField(key.c_str());
                            gv.num_address = (byte)((gvobj.nodes.size() - w) > 0xFF ? 0xFF : (gvobj.nodes.size() - w));
                            gscHandler->WriteGVar(&data[buff], gv);
                            gvarCount++;
                        }
                        utils::WriteValue<uint32_t>(data, gvobj.nodes[w++]->GetDataFLoc(vmInfo->HasFlag(VmFlags::VMF_ALIGN)));
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
                            gv.num_address = (uint16_t)((implData.nodes.size() - w) > 0xFFFF ? 0xFFFF : (implData.nodes.size() - w));
                            gscHandler->WriteImport(&data[buff], gv);
                            implCount++;
                        }
                        utils::WriteValue<uint32_t>(data, implData.nodes[w++]->GetFLoc());
                    }
                }
            }

            if (actsDebugHeader) {
                size_t hashesLoc{};
                size_t hashesIdx{};
                size_t devBlocksLoc{};
                size_t devBlocksIdx{};
                size_t lazyLinksLoc{};
                size_t lazyLinksIdx{};
                if (opt.m_computeDevOption) {
                    LOG_TRACE("Compile {} hash(es)...", hashes.size());
                    hashesLoc = utils::Allocate(data, sizeof(uint32_t) * hashes.size());

                    for (const std::string& h : hashes) {
                        reinterpret_cast<uint32_t*>(&data[hashesLoc])[hashesIdx++] = (uint32_t)data.size();
                        utils::WriteString(data, h.c_str());
                    }

                    LOG_TRACE("Compile {} dev block(s)...", m_devBlocks.size());
                    devBlocksLoc = utils::Allocate(data, sizeof(uint32_t) * m_devBlocks.size());
                    for (const AscmNode* node : m_devBlocks) {
                        reinterpret_cast<uint32_t*>(&data[devBlocksLoc])[devBlocksIdx++] = node->floc;
                    }

                    LOG_TRACE("Compile {} lazy link(s)...", lazyimports.size());
                    for (auto& [loc, lz] : lazyimports) {
                        size_t lzi = utils::Allocate(data, sizeof(tool::gsc::acts_debug::GSC_ACTS_LAZYLINK) + sizeof(uint32_t) * lz.size());
                        lazyLinksIdx++;

                        if (!lazyLinksLoc) {
                            lazyLinksLoc = lzi;
                        }


                        tool::gsc::acts_debug::GSC_ACTS_LAZYLINK* lzd = reinterpret_cast<tool::gsc::acts_debug::GSC_ACTS_LAZYLINK*>(&data[lzi]);

                        lzd->name = loc.name;
                        lzd->name_space = loc.name_space;
                        lzd->script = loc.script;
                        lzd->num_address = (uint32_t)lz.size();

                        uint32_t* locs = reinterpret_cast<uint32_t*>(&lzd[1]);
                        for (AscmNodeLazyLink* node : lz) {
                            *(locs++) = node->floc;
                        }
                    }
                }

                uint32_t detoursLoc{};
                uint32_t detoursCount{};
                if (!detourObjs.empty() && opt.detourType == DETOUR_ACTS) {
                    LOG_TRACE("Compile {} detour(s)...", detourObjs.size());
                    detoursCount = (uint32_t)detourObjs.size();
                    detoursLoc = (uint32_t)utils::Allocate(data, sizeof(tool::gsc::acts_debug::GSC_ACTS_DETOUR) * detourObjs.size());
                    tool::gsc::acts_debug::GSC_ACTS_DETOUR* detours = reinterpret_cast<tool::gsc::acts_debug::GSC_ACTS_DETOUR*>(&data[detoursLoc]);

                    for (FunctionObject* objexp : detourObjs) {
                        detours->location = (uint32_t)objexp->location;
                        detours->size = (uint32_t)objexp->size;

                        detours->name = objexp->detour.func;
                        detours->name_space = objexp->detour.nsp;
                        detours->script = objexp->detour.script;
                        detours++;
                    }
                }

                tool::gsc::acts_debug::GSC_ACTS_DEBUG* debug_obj = reinterpret_cast<tool::gsc::acts_debug::GSC_ACTS_DEBUG*>(data.data() + actsDebugHeader);

                *reinterpret_cast<uint64_t*>(debug_obj->magic) = tool::gsc::acts_debug::MAGIC;
                debug_obj->version = tool::gsc::acts_debug::CURRENT_VERSION;
                debug_obj->actsVersion = (uint64_t) actsinfo::VERSION_ID;
                debug_obj->strings_count = (uint32_t)hashesIdx;
                debug_obj->strings_offset = (uint32_t)hashesLoc;
                debug_obj->detour_count = (uint32_t)detoursCount;
                debug_obj->detour_offset = (uint32_t)detoursLoc;
                debug_obj->devblock_offset = (uint32_t)devBlocksLoc;
                debug_obj->devblock_count = (uint32_t)devBlocksIdx;
                debug_obj->lazylink_offset = (uint32_t)lazyLinksLoc;
                debug_obj->lazylink_count = (uint32_t)lazyLinksIdx;
                // add crc location
                if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC)) {
                    debug_obj->crc_offset = (uint32_t)crcData.opcode->floc;
                }
                else if (gscHandler->HasFlag(tool::gsc::GOHF_NOTIFY_CRC_STRING)) {
                    debug_obj->crc_offset = (uint32_t)crcData.strlistener;
                }
                else {
                    debug_obj->crc_offset = 0;
                }
            }

            uint32_t nameOffSet{};
            if (gscHandler->HasFlag(tool::gsc::GOHF_STRING_NAMES)) {
                nameOffSet = (uint32_t)data.size();
                utils::WriteString(data, fileNameStr);
            }

            // compile header
            gscHandler->file = data.data();

            if (nameOffSet) {
                gscHandler->SetNameString(nameOffSet);
            }
            else {
                gscHandler->SetName(fileName);
            }
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

        void AddImport(AscmNodeFunctionCall* funcCall, uint64_t funcNspHash, uint64_t funcHash, size_t paramCount, uint8_t importFlags) {
            // link by the game, but we write it for test
            Located located{ funcNspHash, funcHash };

            if (devBlockDepth) {
                // dev call
                importFlags |= tool::gsc::T8GSCImportFlags::DEV_CALL;
            }

            std::vector<ImportObject>& impList = imports[located];

            bool useParams = funcCall && (funcCall->flags & FCF_GETTER) == 0;

            auto it = std::find_if(impList.begin(), impList.end(), [importFlags, paramCount, useParams](const ImportObject& e) { 
                if (useParams && paramCount != e.params) {
                    return false;
                }
                return e.flags == importFlags;
            });

            if (it == impList.end()) {
                // no equivalent, we need to create our own node
                impList.emplace_back(importFlags, (byte)paramCount).nodes.push_back(funcCall);
            }
            else {
                // same local/flags, we can add our node
                it->nodes.push_back(funcCall);
            }
        }

        void AddLazy(AscmNodeLazyLink* lazyLink) {
            Located located{ lazyLink->nsp, lazyLink->func, lazyLink->path };

            lazyimports[located].push_back(lazyLink);
        }
    };


    std::pair<const char*, FunctionVar*> FunctionObject::RegisterVar(const std::string& name, bool allowExisting, byte flags) {
        static FunctionVar badVar{ "$BAD_VAR", 0, 0 };
        FunctionVar* it = FindVar(name);
        if (it != VarEnd()) {
            if (allowExisting) {
                return std::make_pair<>(nullptr, &*it);
            }
            return std::make_pair<>(utils::va("The var '%s' already exists", name.c_str()), &badVar);
        }

        if (m_allocatedVar >= ARRAYSIZE(m_vars)) {
            return std::make_pair<>(utils::va("Can't create var '%s': too much variable for function", name.c_str()), &badVar);
        }

        FunctionVar& var = m_vars[m_allocatedVar] = { name, m_allocatedVar, flags };
        m_allocatedVar++;
        obj.AddHash(var.name);

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

    AscmNode* FunctionObject::CreateFieldHash(const char* v, OPCode op) const {
        obj.AddHash(v);
        return CreateFieldHash(m_vmInfo->HashField(v), op);
    }

    AscmNode* FunctionObject::CreateFieldHash(const std::string& v, OPCode op) const {
        obj.AddHash(v);
        return CreateFieldHash(m_vmInfo->HashField(v), op);
    }

    void AscmNodeFunctionCall::SetScriptCall(CompileObject& obj, bool scriptCall) {
        isScriptCall = scriptCall;

        if (flags & FCF_GETTER) {
            if (scriptCall || !obj.HasOpCode(OPCODE_IW_GetBuiltinFunction)) {
                opcode = OPCODE_GetResolveFunction;
            }
            else {
                throw std::runtime_error("builtin getters not implemented for this vm");
            }
            return;
        }

        if (scriptCall) {
            if (flags & FCF_METHOD) {
                if (flags & FCF_CHILDTHREAD) {
                    opcode = OPCODE_ScriptMethodThreadCallEndOn;
                }
                else if (flags & FCF_THREAD) {
                    opcode = OPCODE_ScriptMethodThreadCall;
                }
                else {
                    opcode = OPCODE_ScriptMethodCall;
                }
            }
            else {
                // func
                if (flags & FCF_CHILDTHREAD) {
                    opcode = OPCODE_ScriptThreadCallEndOn;
                }
                else if (flags & FCF_THREAD) {
                    opcode = OPCODE_ScriptThreadCall;
                }
                else {
                    opcode = OPCODE_ScriptFunctionCall;
                }
            }
        }
        else {
            // builtin
            if (flags & FCF_METHOD) {
                opcode = OPCODE_CallBuiltinMethod;
            }
            else {
                // func
                opcode = OPCODE_CallBuiltinFunction;
            }
        }
    }

    AscmNodeCreateLocalVariables::AscmNodeCreateLocalVariables(const FunctionVar* lvars, size_t count, size_t params, const FunctionObject& fobj) : params(params) {
        hasRegister = fobj.obj.HasOpCode(OPCODE_IW_RegisterVariable);
        hasRegisters = fobj.obj.HasOpCode(OPCODE_IW_RegisterMultipleVariables);
        hashSize = fobj.obj.vmInfo->HasFlag(VMF_HASH64) ? 8 : 4;
        hasFlag = !fobj.obj.vmInfo->HasFlag(VMF_NO_PARAM_FLAGS);

        vars.reserve(count);
        for (size_t i = 0; i < count; i++) {
            vars.push_back(lvars[i]);
        }
    }

    FunctionObject* ParseFunction(RuleContext* func, gscParser& parser, CompileObject& obj, byte forceFlags = 0);
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
            case gscParser::RuleExpression13:
            case gscParser::RuleExpression14:
            case gscParser::RuleIdf: {
                if (rule->children.size() == 1) {
                    return ParseFieldNode(rule->children[0], parser, obj, fobj);
                }
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
                return false;
            }
            case gscParser::RuleExpression15:
                return ParseFieldNode(rule->children[1], parser, obj, fobj);
            case gscParser::RuleLeft_value: {
                if (rule->children.size() == 1) {
                    return ParseFieldNode(rule->children[0], parser, obj, fobj);
                }

                if (rule->children[0]->getTreeType() == TREE_RULE) {
                    RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                    std::string second = rule->children[1]->getText();

                    if (second == ".") {
                        // object access
                        if (IS_IDF(rule->children[2])) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format(".size can't be used as a lvalue: {}", exp->getText()));
                                return false;
                            }
                            else {
                                // use identifier
                                fobj.AddNode(rule->children[2], new AscmNodeOpCode(OPCODE_CastFieldObject));
                                fobj.AddNode(rule->children[2], fobj.CreateFieldHash(fieldText, OPCODE_EvalFieldVariableRef));
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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unknown lvalue tree type: {}", exp->getText()));
                    return false;
                }
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unhandled lvalue type: {}", exp->getText()));
                return false;
            }
            case gscParser::RuleConst_expr:
            case gscParser::RuleNumber:
            case gscParser::RuleVector_value:
            case gscParser::RuleSet_expression:
            case gscParser::RuleArray_def:
            case gscParser::RuleStruct_def:
            case gscParser::RuleFunction:
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
            case gscParser::RuleDevop_def:
            case gscParser::RuleStatement_block:
            case gscParser::RuleStatement_dev_block:
            case gscParser::RuleOperator_inst:
            case gscParser::RuleFunction_call:
            case gscParser::RuleFunction_call_exp:
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Not a valid lvalue: {} ({})", rule->getText(), rule->getRuleIndex()));
                return false;
            }


            obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unhandled lvalue rule: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }

        if (exp->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("Unknown tree type: {}", exp->getText()));
            return false;
        }
        TerminalNode* term = dynamic_cast<TerminalNode*>(exp);


        size_t len = term->getText().size();

        switch (term->getSymbol()->getType()) {
        case gscParser::IDENTIFIER: {
            std::string varName = term->getText();

            if (varName == "self") {
                obj.info.PrintLineMessage(alogs::LVL_WARNING, term, "self can't be used as a lvalue");
                return false;
            }

            auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

            if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
                GlobalVariableDef& gv = gvarIt->second;

                obj.info.PrintLineMessage(alogs::LVL_WARNING, term, std::format("The {} global variable can't be used as a lvalue", gv.name));
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
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Not a valid lvalue: {}", term->getText()));
            return false;
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unhandled lvalue terminal: {}", term->getText()));
        return false;
    }
    bool ParseExpressionNode(ParseTree* exp, gscParser& parser, CompileObject& obj, FunctionObject& fobj, bool expressVal) {
        if (!exp) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "Empty tree error");
            return false;
        }
        if (exp->getTreeType() == TREE_ERROR) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "Detected tree error, bad syntax?");
            return false;
        }

        if (exp->getTreeType() == TREE_RULE) {
            RuleContext* rule = dynamic_cast<RuleContext*>(exp);

            // search that we don't have a children in error, after that we'll assume the tree is right for a depth of 2
            if (std::find_if(rule->children.begin(), rule->children.end(), [](const ParseTree* tree) -> bool { return tree->getTreeType() == TREE_ERROR; })
                != rule->children.end()) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, "Detected tree children error, bad syntax?");
                return false;
            }

            switch (rule->getRuleIndex()) {
            case gscParser::RuleStatement: {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express a statement");
                    return false;
                }

                if (rule->children.size() > 1) {
                    // define location

                    std::string locName = rule->children[0]->getText();

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[locName];

                    if (loc.defined) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("The location {} was defined twice!", locName));
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
                AscmNode* elseStart = new AscmNode();
                if (!ParseExpressionNode(rule->children[2], parser, obj, fobj, true)) {
                    ok = false;
                }

                fobj.AddNode(rule->children[2], new AscmNodeJump(elseStart, OPCODE_JumpOnFalse));

                if (!ParseExpressionNode(rule->children[4], parser, obj, fobj, false)) {
                    ok = false;
                }


                if (rule->children.size() > 5) { // else
                    AscmNode* endElse = new AscmNode();
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
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering switch variable: {}", err));
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
                    std::string caseType = rule->children[i]->getText();

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

                for (JumpData& caseElem : jmpTable) {
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

                if (var1err) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach variable: {}", var1err));
                    return false;
                }

                // foreach (key, value in level.var_4afb8f5a) {

                FunctionVar* keyVar;
                FunctionVar* valueVar;

                if (IS_IDF(rule->children[4])) {
                    // key,value in
                    auto [var3err, keyVarL] = fobj.RegisterVar(rule->children[2]->getText(), true);
                    auto [var4err, valueVarL] = fobj.RegisterVar(rule->children[4]->getText(), true);

                    if (var3err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach key variable: {}", var3err));
                        return false;
                    }
                    if (var4err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach value variable: {}", var4err));
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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach key variable: {}", var3err));
                        return false;
                    }
                    if (var4err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach value variable: {}", var4err));
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
                    auto [var2err, iteratorVal] = fobj.RegisterVarRnd();
                    if (var2err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach iterator variable: {}", var2err));
                        return false;
                    }

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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach next variable: {}", varnexterr));
                        ok = false;
                    }

                    auto [var2err, iteratorVal] = fobj.RegisterVarRnd();
                    if (var2err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Error when registering foreach iterator variable: {}", var2err));
                        return false;
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
                case tool::gsc::GOHF_FOREACH_TYPE_JUP: {
                    // array = ...;
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_SetLocalVariableCached));
                    // key = getfirstarraykey(array);
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_PreScriptCall));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    uint64_t fakHash = obj.vmInfo->HashField("getfirstarraykey");
                    AscmNodeFunctionCall* fakNode = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 1, fakHash, obj.currentNamespace, obj.vmInfo);
                    obj.AddImport(fakNode, obj.currentNamespace, fakHash, obj.currentNamespace, tool::gsc::FUNCTION | tool::gsc::GET_CALL);
                    fobj.AddNode(arrVal, fakNode);
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

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

                    // val = array[key];
                    fobj.AddNode(arrVal, new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_EvalArray));
                    fobj.AddNode(arrVal, new AscmNodeVariable(valueVar->id, OPCODE_SetLocalVariableCached));


                    if (!ParseExpressionNode(rule->children[rule->children.size() - 1], parser, obj, fobj, false)) {
                        ok = false;
                    }

                    fobj.AddNode(rule->children[rule->children.size() - 1], loopContinue);
                    // key = getnextarraykey(array, key);
                    fobj.AddNode(arrVal, new AscmNodeOpCode(OPCODE_PreScriptCall));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_EvalLocalVariableCached));
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(arrayVal->id, OPCODE_EvalLocalVariableCached));

                    uint64_t gakHash = obj.vmInfo->HashField("getnextarraykey");
                    AscmNodeFunctionCall* gakNode = new AscmNodeFunctionCall(OPCODE_CallBuiltinFunction, 0, 2, gakHash, obj.currentNamespace, obj.vmInfo);
                    obj.AddImport(gakNode, obj.currentNamespace, gakHash, obj.currentNamespace, tool::gsc::FUNCTION | tool::gsc::GET_CALL);
                    fobj.AddNode(arrVal, gakNode);
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_SetLocalVariableCached));

                    // loop back
                    fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeJump(loopIt, OPCODE_Jump));

                    fobj.PopBreakNode();
                    fobj.PopContinueNode();
                    // end
                    fobj.AddNode(rule->children[rule->children.size() - 1], loopBreak);

                    //fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(keyVar->id, OPCODE_IW_ClearFieldVariableRef));
                    //fobj.AddNode(rule->children[rule->children.size() - 1], new AscmNodeVariable(arrayVal->id, OPCODE_IW_ClearFieldVariableRef));
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
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express a nop value");
                    return false;
                }
                size_t count{ 1 };
                if (rule->children.size() > 1) {
                    int64_t val{ NumberNodeValue(rule->children[2]) };
                    if (val < 0) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't define a negative amount of nop");
                        return false;
                    }
                    count = (size_t)val;
                }

                for (size_t i = 0; i < count; i++) {
                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_Nop));
                }
            
                return true;
            }
            case gscParser::RuleDevop_def: {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express a devop value");
                    return false;
                }
                int64_t val{ NumberNodeValue(rule->children[2]) };
                if (val < 0) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Invalid devop value (Negative)");
                    return false;
                }
                if (obj.vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) {
                    if (val > 0xFFFF) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Invalid devop value (Too large)");
                        return false;
                    }
                }
                else {
                    if (val > 0xFF) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Invalid devop value (Too large)");
                        return false;
                    }
                }

                fobj.AddNode(rule, new AscmNodeRawOpCode((uint16_t)val));

                return true;
            }
            case gscParser::RuleStatement_inst: {
                if (rule->children.size() == 1) {
                    if (expressVal) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express this value");
                        return false;
                    }
                    return true; // empty instruction
                }
                return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
            }
            case gscParser::RuleStatement_block: {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                bool ok{ true };
                for (size_t i = 1; i < rule->children.size() - 1; i++) {
                    ParseTree* stmt = rule->children[i];

                    if (!ParseExpressionNode(stmt, parser, obj, fobj, false)) {
                        ok = false; // do not return false so we can have multiple issues
                    }
                }
                return ok;
            }
            case gscParser::RuleStatement_dev_block: {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                bool ok{ true };
                AscmNode* endBlock = new AscmNode();
                AscmNode* djmp = new AscmNodeJump(endBlock, OPCODE_DevblockBegin);
                obj.m_devBlocks.push_back(djmp);
                fobj.AddNode(rule->children[0], djmp);
                obj.devBlockDepth++;
                for (size_t i = 1; i < rule->children.size() - 1; i++) {
                    ParseTree* stmt = rule->children[i];

                    if (!ParseExpressionNode(stmt, parser, obj, fobj, false)) {
                        ok = false;
                    }
                }
                obj.devBlockDepth--;
                fobj.AddNode(rule->children[rule->children.size() - 1], endBlock);
                return ok;
            }
            case gscParser::RuleOperator_inst: {
                if (expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't express this value");
                    return false;
                }
                std::string idf = rule->children[0]->getText();

                if (idf == "break") {
                    if (rule->children.size() > 1) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't specify jump location with break");
                        return false;
                    }

                    AscmNode* loc = fobj.PeekBreakNode();

                    if (!loc) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Not in a break context");
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                    return true;
                }

                if (idf == "continue") {
                    if (rule->children.size() > 1) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't specify jump location with continue");
                        return false;
                    }

                    AscmNode* loc = fobj.PeekContinueNode();

                    if (!loc) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Not in a continue context");
                        return false;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc, OPCODE_Jump));
                    return true;
                }

                if (idf == "goto") {
                    if (rule->children.size() <= 1 && !IS_IDF(rule->children[1])) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "goto should be used with a jump location");
                        return false;
                    }

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[rule->children[1]->getText()];
                    if (!loc.node) {
                        loc.node = new AscmNode();
                    }
                    if (!loc.def) {
                        loc.def = rule;
                    }

                    fobj.AddNode(rule, new AscmNodeJump(loc.node, OPCODE_Jump));
                    return true;
                }

                if (idf == "jumpdev") {
                    if (rule->children.size() <= 1 && !IS_IDF(rule->children[1])) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "jumpdev should be used with a jump location");
                        return false;
                    }

                    FunctionJumpLoc& loc = fobj.m_jumpLocs[rule->children[1]->getText()];
                    if (!loc.node) {
                        loc.node = new AscmNode();
                    }
                    if (!loc.def) {
                        loc.def = rule;
                    }

                    AscmNode* djmp = new AscmNodeJump(loc.node, OPCODE_DevblockBegin);
                    obj.m_devBlocks.push_back(djmp);
                    fobj.AddNode(rule, djmp);
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

                obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unknown operator type {}", idf));
                return false;
            }
            case gscParser::RuleFunction_call:
            case gscParser::RuleFunction_call_exp: {
                if (rule->children.size() == 1) {
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
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
                    else if (callModifier == "childthread" || callModifier == "threadendon") { // old compatibility
                        flags |= FCF_CHILDTHREAD;
                    }
                    else if (callModifier == "builtin") {
                        flags |= FCF_BUILTIN;
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

                ParseTree* paramsList = rule->children[rule->children.size() - 2];
                if (!IS_RULE_TYPE(paramsList, gscParser::RuleExpression_list)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, std::format("Not a params list {}", paramsList->getText()));
                    return false;
                }

                if (functionComp->children.size() == 1) {
                    // func
                    importFlags |= tool::gsc::GET_CALL;
                    funcNspHash = obj.currentNamespace;
                    std::string funcName = functionComp->children[0]->getText();

                    funcHash = obj.vmInfo->HashField(funcName.c_str());

                    auto funcIt = obj.vmInfo->opFuncs.find(funcHash);

                    if (funcIt != obj.vmInfo->opFuncs.end()) {
                        // internal function call
                        FunctionOperator& f = funcIt->second;
                        if (f.HasFlag(tool::gsc::opcode::VPFD_SELF_PARAM) && !(flags & FCF_METHOD)) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' should have a caller, Usage: {}", funcName, f.usage));
                            return false;
                        }
                        if (expressVal && !f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' doesn't return a value, Usage: {}", funcName, f.usage));
                            return false;
                        }
                        if (flags & (FCF_THREAD | FCF_CHILDTHREAD | FCF_BUILTIN)) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' can't have a call modifier, Usage: {}", funcName, f.usage));
                            return false;
                        }

                        if (f.HasFlag(tool::gsc::opcode::VPFD_USE_PRE_SCRIPT_CALL)) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_PreScriptCall));
                        }

                        bool paramError{};
                        int paramCount{};
                        uint64_t hashVal{};

                        if (f.HasFlag(tool::gsc::opcode::VPFD_UNPACK)) {
                            if (expressVal) {
                                obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' can't express a value, Usage: {}", funcName, f.usage));
                                paramError = true;
                            }
                            else if (!paramsList->children.size()) {
                                obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' needs at least one param, Usage: {}", funcName, f.usage));
                                paramError = true;
                            }
                            else {
                                if (!ParseExpressionNode(paramsList->children[0], parser, obj, fobj, true)) {
                                    paramError = true;
                                }

                                paramCount++;

                                for (int i = (int)paramsList->children.size() - 1; i > 1; i -= 2) {
                                    ParseTree* pt = paramsList->children[i];
                                    while (pt && pt->getTreeType() == TREE_RULE && pt->children.size() == 1) {
                                        pt = pt->children[0];
                                    }
                                    if (!IS_IDF(pt)) {
                                        obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' needs to be unpacked with variables, Usage: {}", funcName, f.usage));
                                        return false;
                                    }
                                    paramCount++;
                                }
                            }
                        }
                        else {
                            int removedStart{};
                            if (f.HasFlag(VPFD_HASH_PARAM)) {
                                if (paramsList->children.empty()) {
                                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' needs to have at least one param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                ParseTree* hashParam{ paramsList->children[0] };

                                if (!obj.TryHashNodeValue(hashParam, hashVal)) {
                                    obj.info.PrintLineMessage(alogs::LVL_ERROR, functionComp, std::format("Operator '{}' should start with a valid hash param, Usage: {}", funcName, f.usage));
                                    return false;
                                }

                                removedStart += 2; // use the first param as hash
                            }
                            for (int i = (int)paramsList->children.size() - 1; i >= removedStart; i -= 2) {
                                if (!ParseExpressionNode(paramsList->children[i], parser, obj, fobj, true)) {
                                    paramError = true;
                                }
                                paramCount++;
                            }
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
                        else if (f.HasFlag(VPFD_HASH_PARAM)) {
                            fobj.AddNode(rule, new AscmNodeData<uint64_t>(hashVal, f.opCode));
                        }
                        else {
                            fobj.AddNode(rule, new AscmNodeOpCode(f.opCode));
                        }

                        if (!expressVal && f.HasFlag(tool::gsc::opcode::VPFD_RETURN_VALUE)) {
                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_DecTop));
                        }

                        if (f.HasFlag(tool::gsc::opcode::VPFD_UNPACK)) {
                            for (size_t i = 1; i < paramCount; i++) {
                                ParseTree* pnode = paramsList->children[i * 2];
                                auto [errVar, var] = fobj.RegisterVar(pnode->getText(), true);
                                if (errVar) {
                                    obj.info.PrintLineMessage(alogs::LVL_ERROR, pnode, std::format("Can't register variable: {}", errVar));
                                    return false;
                                }

                                // register var
                                fobj.AddNode(pnode, new AscmNodeVariable(var->id, OPCODE_IW_SetWaittillVariableFieldCached));
                            }

                            fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_ClearParams));
                        }

                        return true;
                    }
                    obj.AddHash(funcName);
                }
                else if (functionComp->children.size() == 3) {
                        // namespace::func

                        std::string funcNspName = functionComp->children[0]->getText();
                        std::string funcName = functionComp->children[2]->getText();

                        obj.AddHash(funcNspName);
                        obj.AddHash(funcName);
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
                    obj.AddHash(funcName);
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

                LOG_TRACE("add param {} {}", hashutils::ExtractTmp("function", funcHash), paramCount);

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
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "Class pointer can't be used with builtin calls");
                            return false;
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
                            else if (flags & FCF_BUILTIN) {
                                opcode = OPCODE_IW_BuiltinMethodCallPointer;
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
                            else if (flags & FCF_BUILTIN) {
                                opcode = OPCODE_IW_BuiltinFunctionCallPointer;
                            }
                            else {
                                opcode = OPCODE_ScriptFunctionCallPointer;
                            }
                        }
                    }
                    fobj.AddNode(rule, new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash, obj.vmInfo));
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
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "builtin modifier can only be used with pointer calls");
                            return false;
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
                        else if (flags & FCF_BUILTIN) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, paramsList, "builtin modifier can only be used with pointer calls");
                            return false;
                        }
                        else {
                            opcode = OPCODE_ScriptFunctionCall;
                            importFlags |= tool::gsc::T8GSCImportFlags::FUNCTION;
                        }
                    }

                    AscmNodeFunctionCall* funcCall = new AscmNodeFunctionCall(opcode, flags, (byte)paramCount, funcHash, funcNspHash, obj.vmInfo);

                    obj.AddImport(funcCall, funcNspHash, funcHash, paramCount, importFlags);

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
            case gscParser::RuleExpression12:
            case gscParser::RuleIdf: {
                if (rule->children.size() == 1) {
                    // simple rules recursion
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
                if (rule->children.size() == 2) {
                    // (++|--|~|!) exp
                    if (rule->children[0]->getTreeType() == TREE_TERMINAL) {
                        // ++/--/~/!

                        std::string op = rule->children[0]->getText();
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
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[0], std::format("Unhandled operator: {}", op));
                            return false;
                        }
                    }
                    else {
                        // ++/--

                        std::string op = rule->children[1]->getText();
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
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("Unhandled operator: {}", op));
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
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unknown expression, excepted 3 children: {}", rule->getText()));
                    return false;
                }

                std::string op = rule->children[1]->getText();

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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("Unhandled operator: {}", op));
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
            case gscParser::RuleExpression15:
                return ParseExpressionNode(rule->children[rule->children.size() == 3 ? 1 : 0], parser, obj, fobj, expressVal);
            case gscParser::RuleVector_value:
                if (!expressVal) { // no need to create vector
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
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
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateArray));

                // 1 = [ (HASHSTR|'#' number) ':'
                bool ok{ true };
                size_t currentKey{};
                for (size_t i = 1; i + 1 < rule->children.size();) {
                    if (rule->children[i + 1]->getText() == ":") {
                        // key ':'

                        ParseTree* expTree = rule->children[i++];

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
                        fobj.AddNode(rule->children[i - 1], obj.BuildAscmNodeData(currentKey++));
                    }

                    fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToArray));

                    i++; // ',' or ']'
                }

                return ok;
            }
            case gscParser::RuleStruct_def: {
                if (!expressVal) { // no need to create struct
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CreateStruct));

                bool ok{ true };
                for (size_t i = 1; i < rule->children.size() - 1;) {

                    ParseTree* term = rule->children[i++];

                    i++; // ':'


                    if (!ParseExpressionNode(rule->children[i++], parser, obj, fobj, true)) {
                        ok = false;
                    }

                    if (obj.HasOpCode(OPCODE_IW_AddToStruct)) {
                        if (!IS_TERMINAL_TYPE(term, gscParser::STRUCT_IDENTIFIER)) {
                            obj.info.PrintLineMessage(alogs::LVL_ERROR, term, std::format("Can't use expression to define structure names in this vm: {}", term->getText()));
                            ok = false;
                        }
                        else {
                            std::string termStr = term->getText();

                            std::string sub = termStr.substr(1, termStr.size() - 1);
                            fobj.AddNode(rule->children[i - 1], fobj.CreateFieldHash(sub, OPCODE_IW_AddToStruct));
                        }
                    }
                    else {
                        if (!IS_TERMINAL_TYPE(term, gscParser::STRUCT_IDENTIFIER)) {
                            if (!ParseExpressionNode(term, parser, obj, fobj, true)) {
                                ok = false;
                            }
                            fobj.AddNode(term, new AscmNodeOpCode(OPCODE_CastCanon));
                        }
                        else {
                            std::string termStr = term->getText();
                            
                            std::string sub = termStr.substr(1, termStr.size() - 1);

                            // we can't use CreateField hash because the hash is on 64 bits, but 32 in reality
                            obj.AddHash(sub);
                            fobj.AddNode(rule->children[i - 1], new AscmNodeData<uint64_t>(obj.vmInfo->HashField(sub.c_str()), OPCODE_GetHash));
                        }

                        fobj.AddNode(rule->children[i - 1], new AscmNodeOpCode(OPCODE_AddToStruct));
                    }

                    i++; // ',' or '}'
                }
                return ok;
            }
            case gscParser::RuleFunction: {
                FunctionObject* sfobj = ParseFunction(rule, parser, obj, tool::gsc::T8GSCExportFlags::PRIVATE);

                if (!sfobj) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, "Can't parse sub function");
                    return false;
                }

                if (expressVal) {
                    AscmNodeFunctionCall* asmc = new AscmNodeFunctionCall(OPCODE_GetResolveFunction, FCF_GETTER, 0, 0, 0, obj.vmInfo);

                    obj.AddImport(asmc, sfobj->m_name_space, sfobj->m_name, 0, tool::gsc::T8GSCImportFlags::FUNC_METHOD | tool::gsc::T8GSCImportFlags::GET_CALL);
                    fobj.AddNode(rule, asmc);
                }

                return true;
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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, rule->children[1], std::format("Unhandled set operator: {}", opVal));
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
                if (!expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, rule, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                if (rule->children.size() == 2) {
                    if (IS_IDF(rule->children[1]) ||
                        (IS_RULE_TYPE(rule->children[1], gscParser::RuleLeft_value) && IS_IDF(rule->children[1]->children[0]))
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

                    obj.AddHash(nsp);
                    obj.AddHash(path);
                    obj.AddHash(funcName);

                    AscmNodeLazyLink* lazy = new AscmNodeLazyLink(
                        obj.vmInfo->HashPath(path.c_str()),
                        (uint32_t)obj.vmInfo->HashField(nsp.c_str()),
                        (uint32_t)obj.vmInfo->HashField(funcName.c_str())
                    );

                    obj.AddLazy(lazy);
                
                    fobj.AddNode(rule, lazy);
                    return true;
                }
                // &nsp::func || &func
                uint64_t nsp = obj.currentNamespace;

                byte flags = tool::gsc::T8GSCImportFlags::FUNC_METHOD;

                if (rule->children.size() == 4) {
                    // with nsp
                    std::string nspStr = rule->children[1]->getText();
                    obj.AddHash(nspStr);
                    nsp = obj.vmInfo->HashField(nspStr.c_str());
                }
                else {
                    flags |= tool::gsc::T8GSCImportFlags::GET_CALL;
                }

                assert(rule->children.size());

                std::string funcStr = rule->children[rule->children.size() - 1]->getText();
                obj.AddHash(funcStr);
                uint64_t func = obj.vmInfo->HashField(funcStr.c_str());

                // link by the game, but we write it for test
                AscmNodeFunctionCall* asmc = new AscmNodeFunctionCall(OPCODE_GetResolveFunction, FCF_GETTER, 0, 0, 0, obj.vmInfo);
                obj.AddImport(asmc, nsp, func, 0, flags);
                fobj.AddNode(rule, asmc);

                return true;
            }
            case gscParser::RuleLeft_value: {
                if (!expressVal) {
                    obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("Ignored useless value: {}", rule->getText()));
                    return true;
                }
                if (rule->children.size() == 1) {
                    return ParseExpressionNode(rule->children[0], parser, obj, fobj, expressVal);
                }
                else if (rule->children[0]->getTreeType() == TREE_RULE) {
                    RuleContext* first = dynamic_cast<RuleContext*>(rule->children[0]);

                    std::string second = rule->children[1]->getText();

                    if (second == ".") {
                        // object access
                        if (IS_IDF(rule->children[2])) {
                            if (!ParseExpressionNode(first, parser, obj, fobj, true)) {
                                return false;
                            }
                            std::string fieldText = rule->children[2]->getText();

                            if (fieldText == "size") {
                                fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_SizeOf));
                            }
                            else {
                                // use identifier
                                if (obj.HasOpCode(OPCODE_CastAndEvalFieldVariable)) {
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_CastAndEvalFieldVariable));
                                }
                                else {
                                    fobj.AddNode(rule, new AscmNodeOpCode(OPCODE_CastFieldObject));
                                    fobj.AddNode(rule, fobj.CreateFieldHash(fieldText, OPCODE_EvalFieldVariable));
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
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unknown left value type: {}", second));
                        return false;
                    }
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unknown lvalue tree type: {}", exp->getText()));
                    return false;
                }
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unhandled lvalue type: {}", exp->getText()));
                return false;
            }
            }

            obj.info.PrintLineMessage(alogs::LVL_ERROR, rule, std::format("Unhandled rule: {} ({})", rule->getText(), rule->getRuleIndex()));
            return false;
        }

        if (exp->getTreeType() != TREE_TERMINAL) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unknown tree type: {}", exp->getText()));
            return false;
        }

        TerminalNode* term = dynamic_cast<TerminalNode*>(exp);

        if (!expressVal) {
            obj.info.PrintLineMessage(alogs::LVL_WARNING, exp, std::format("Ignored useless value: {}", term->getText()));
            return true;
        }

        size_t len = term->getText().size();

        switch (term->getSymbol()->getType()) {
        case gscParser::IDENTIFIER: {
            std::string varName = term->getText();

            if (varName == "self") {
                fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetSelf));
                return true;
            }

            auto gvarIt = fobj.m_vmInfo->globalvars.find(obj.vmInfo->HashField(varName));

            if (gvarIt != fobj.m_vmInfo->globalvars.end()) {
                GlobalVariableDef& gv = gvarIt->second;
            
                if (gv.getOpCode) {
                    fobj.AddNode(term, new AscmNodeOpCode(gv.getOpCode));
                    return true;
                }

                if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_GLOBAL)) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, term, std::format("{} is defined as a global, but the vm doesn't support globals", varName));
                    return false;
                }

                GlobalVarObject& decl = obj.globals[gv.name];

                if (!decl.def) {
                    decl.def = &gv;
                }

                AscmNodeGlobalVariable* gvar = new AscmNodeGlobalVariable(&gv, false);
                decl.nodes.emplace_back(gvar);
                fobj.AddNode(term, gvar);
                return true;
            }

            FunctionVar* varIt = fobj.FindVar(varName);

            if (varIt == fobj.VarEnd()) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, term, std::format("Unknown variable: {}", varName));
                return false;
            }

            fobj.AddNode(term, new AscmNodeVariable(varIt->id, OPCODE_EvalLocalVariableCached));
            return true;
        }
        case gscParser::UNDEFINED_VALUE:
            fobj.AddNode(term, new AscmNodeOpCode(OPCODE_GetUndefined));
            return true;
        case gscParser::BOOL_VALUE:
            fobj.AddNode(term, obj.BuildAscmNodeData(term->getText() == "true"));
            return true;
        case gscParser::FLOATVAL:
            fobj.AddNode(term, new AscmNodeData<FLOAT>((FLOAT)std::strtof(term->getText().c_str(), nullptr), OPCODE_GetFloat));
            return true;
        case gscParser::INTEGER10:
            fobj.AddNode(term, obj.BuildAscmNodeData(std::strtoll(term->getText().c_str(), nullptr, 10)));
            return true;
        case gscParser::INTEGER16: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 16);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::INTEGER8: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 2 : 1), nullptr, 8);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::INTEGER2: {
            bool neg = term->getText()[0] == '-';
            auto val = std::strtoll(term->getText().c_str() + (neg ? 3 : 2), nullptr, 2);
            fobj.AddNode(term, obj.BuildAscmNodeData(neg ? -val : val));
            return true;
        }
        case gscParser::HASHSTRING: {
            std::string hash = term->getText();
            char type = hash[0];
            std::string sub = hash.substr(2, len - 3);
            obj.AddHash(sub);
            auto ith = obj.vmInfo->hashesFunc.find(type);

            if (ith == obj.vmInfo->hashesFunc.end()) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Hash type not available for this vm: {}", type));
                return false;
            }

            const char* ss = sub.c_str();

            uint64_t val;

            if (!hash::TryHashPattern(ss, val)) {
                val = ith->second.hashFunc(ss);
                if (!val) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Can't hash the string '{}' with the type {}", sub, type));
                    return false;
                }
            }

            switch (ith->second.size) {
            case 8: fobj.AddNode(term, new AscmNodeData<uint64_t>((uint64_t)val, ith->second.opCode)); break;
            case 4: fobj.AddNode(term, new AscmNodeData<uint32_t>((uint32_t)val, ith->second.opCode)); break;
            case 2: fobj.AddNode(term, new AscmNodeData<uint16_t>((uint16_t)val, ith->second.opCode)); break;
            case 1: fobj.AddNode(term, new AscmNodeData<uint8_t>((uint8_t)val, ith->second.opCode)); break;
            default: {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Invalid hash size definition: {} / {} bytes", type, ith->second.size));
                return false;
            }
            }

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
                obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("String too long: {}", term->getText()));
                return false;
            }

            auto& str = obj.strings[key];
            str.nodes.push_back(asmc);
            return true;
        }
        }

        obj.info.PrintLineMessage(alogs::LVL_ERROR, exp, std::format("Unhandled terminal: {}", term->getText()));
        return false;
    }

    FunctionObject* ParseFunction(RuleContext* func, gscParser& parser, CompileObject& obj, byte forceFlags) {
        bool hasName = func->children.size() > 4 && IS_IDF(func->children[(size_t)(func->children.size() - 5)]);

        auto* paramsRule = func->children[(size_t)(func->children.size() - 3)];
        auto* blockRule = func->children[(size_t)(func->children.size() - 1)];
    
        std::string name = hasName ? func->children[(size_t)(func->children.size() - 5)]->getText() : utils::va("$nameless_%llx", obj.emptyNameInc++);

        obj.AddHash(name);
        uint64_t nameHashed = obj.vmInfo->HashField(name.data());

        auto [res, err] = obj.exports.try_emplace(nameHashed, obj, nameHashed, obj.currentNamespace, obj.fileNameSpace, obj.vmInfo);

        if (!err) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("The export {} was defined twice", name));
            return nullptr;
        }

        FunctionObject& exp = res->second;

        exp.m_flags |= forceFlags;

        if (!IS_RULE_TYPE(paramsRule, gscParser::RuleParam_list)) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("Bad function {} params declaration {}", name, func->getText()));
            return nullptr;
        }
        if (!IS_RULE_TYPE(blockRule, gscParser::RuleStatement_block)) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("Bad function {} block declaration {}", name, func->getText()));
            return nullptr;
        }

        // handle modifiers

        size_t end = hasName ? 5 : 4;

        for (size_t i = 0; i < func->children.size() - end; i++) {
            ParseTree* mod = func->children[i];

            if (IS_RULE_TYPE(mod, gscParser::RuleDetour_info)) {

                if (mod->children.size() != 2) {
                    // function detour
                    std::string dnsp = mod->children[1]->getText();
                    std::string dscript = mod->children[3]->getText();
                    std::string dfunc = mod->children[6]->getText();

                    obj.AddHash(dnsp);
                    obj.AddHash(dscript);
                    obj.AddHash(dfunc);
                    exp.detour.nsp = obj.vmInfo->HashField(dnsp);
                    exp.detour.func = obj.vmInfo->HashField(dfunc);
                    exp.detour.script = obj.vmInfo->HashPath(dscript);
                }
                else {
                    // builtin detour
                    std::string target = mod->children[1]->getText();

                    obj.AddHash(target);
                    exp.detour.nsp = 0;
                    exp.detour.func = obj.vmInfo->HashField(target);
                    exp.detour.script = 0;
                }

                continue;
            }

            if (mod->getTreeType() != TREE_TERMINAL) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, mod, std::format("Bad modifier for {}", name));
                return nullptr;
            }

            TerminalNode* term = dynamic_cast<TerminalNode*>(mod);

            std::string txt = term->getText();

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
                    return nullptr;
                }
                exp.m_flags |= tool::gsc::T8GSCExportFlags::EVENT;
                auto* ev = func->children[i += 2];
                i++; // ']'
                if (ev->getTreeType() != TREE_TERMINAL) {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, ev, std::format("Bad event for {}", name));
                    return nullptr;
                }

                std::string evname = static_cast<TerminalNode*>(ev)->getText();

                obj.AddHash(evname);
                exp.m_data_name = obj.vmInfo->HashField(evname.data());
            }
        }

        // handle params

        auto* params = dynamic_cast<gscParser::Param_listContext*>(paramsRule);

        size_t index = 0;
        bool varargDetected{};

        for (auto* child : params->children) {
            if (index++ % 2) {
                continue; // ','
            }

            assert(IS_RULE_TYPE(child, gscParser::RuleParam_val));
            auto* param = dynamic_cast<gscParser::Param_valContext*>(child);

            if (varargDetected) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, child, "Can't register param after a vararg");
                return nullptr;
            }

            TerminalNode* idfNode{};
            byte idfFlags{};
            if (!IS_IDF(param->children[0])) {
                if (param->children.size() == 1) {
                    // '...'

                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_VA)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, param, "Modifier not available for this vm: vararg...");
                        return nullptr;
                    }
                    exp.m_flags |= tool::gsc::T8GSCExportFlags::VE;

                    std::string paramIdf = "vararg";


                    if (exp.m_params == 256) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("Can't register param '{}': too many params", paramIdf));
                        return nullptr;
                    }

                    exp.m_params++;

                    auto [err, vardef] = exp.RegisterVar(paramIdf, false, tool::gsc::opcode::VARIADIC);
                    if (err) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, param, err);
                        return nullptr;
                    }

                    continue;
                }

                // skip modifier
                idfNode = dynamic_cast<TerminalNode*>(param->children[1]);
                auto modifier = param->children[0]->getText();
                if (modifier == "*") {
                    // ptr (T9)
                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_PTR)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("Modifier not available for this vm: {}", modifier));
                        return nullptr;
                    }
                    idfFlags = tool::gsc::opcode::T9_VAR_REF;
                }
                else if (modifier == "&") {
                    // ref (T8)
                    if (!obj.gscHandler->HasFlag(tool::gsc::GOHF_SUPPORT_VAR_REF)) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("Modifier not available for this vm: {}", modifier));
                        return nullptr;
                    }
                    idfFlags = tool::gsc::opcode::ARRAY_REF;
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, param, std::format("Modifier not implemented: {}", modifier));
                    return nullptr;
                }
            }
            else {
                idfNode = dynamic_cast<TerminalNode*>(param->children[0]);
            }
            auto paramIdf = idfNode->getText();

            if (exp.m_params == 256) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, std::format("Can't register param '{}': too many params", paramIdf));
                return nullptr;
            }

            exp.m_params++;

            auto [err, vardef] = exp.RegisterVar(paramIdf, false, idfFlags);

            if (err) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, idfNode, err);
                return nullptr;
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
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, defaultValueExp, std::format("Can't create expression node for variable {}", paramIdf));
                    return nullptr;
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
        bool badRef{};

        if (!ParseExpressionNode(blockRule, parser, obj, exp, false)) {
            badRef = true;
        }

        exp.PopBreakNode();

        if (obj.devBlockDepth) {
            // add a dev block from start to begin
            AscmNode* djmp = new AscmNodeJump(endNode, OPCODE_DevblockBegin);
            obj.m_devBlocks.push_back(djmp);
            exp.AddNode(exp.m_nodes.begin(), func, djmp);
        }

        exp.AddNode(func, endNode);

        for (auto& [name, loc] : exp.m_jumpLocs) {
            if (loc.defined) {
                continue;
            }

            obj.info.PrintLineMessage(alogs::LVL_ERROR, loc.def ? loc.def : blockRule, std::format("The location {} was used, but isn't declared", name));

            badRef = true;

            if (loc.node) {
                delete loc.node; // free loc
                loc.node = nullptr;
            }
        }

        exp.AddNode(exp.m_nodes.begin(), func, exp.CreateParamNode());

        if (badRef) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, func, std::format("Can't compile function '{}'", hasName ? name : "<no name>"));
            return nullptr;
        }

        return &exp;
    }

    bool ParseInclude(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        std::string txt = nsp->children[1]->getText();

        // add the include/using into the includes
        obj.AddInclude(txt);

        return true;
    }

    bool ParseNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        std::string txt = nsp->children[1]->getText();

        // set the current namespace to the one specified

        obj.AddHash(txt);
        obj.currentNamespace = obj.vmInfo->HashField(txt.data());
        if (!obj.fileNameSpace) {
            obj.fileNameSpace = obj.vmInfo->HashFilePath(txt.data());
        }

        return true;
    }

    bool ParseFileNamespace(RuleContext* nsp, gscParser& parser, CompileObject& obj) {
        if (nsp->children.size() < 2 || nsp->children[1]->getTreeType() != TREE_TERMINAL) {
            return false; // bad
        }

        std::string txt = nsp->children[1]->getText();

        // set the current file namespace to the one specified

        obj.AddHash(txt);
        obj.fileNameSpace = obj.vmInfo->HashFilePath(txt.data());
        if (!obj.currentNamespace) {
            obj.currentNamespace = obj.vmInfo->HashField(txt.data());
        }

        return true;
    }

    bool ParseProg(gscParser::ProgContext* prog, gscParser& parser, CompileObject& obj) {
        if (prog->getTreeType() == TREE_ERROR) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad prog context");
            return false;
        }

        auto* eof = prog->EOF();

        // find the first namespace (used for multifile inputs)
        for (auto* es : prog->children) {
            if (es == eof) {
                break;
            }
            if (es->getTreeType() == TREE_ERROR) {
                obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, "Bad export rule type");
                return false;
            }
            if (es->getTreeType() == TREE_TERMINAL) {
                continue; // dev block part
            }

            RuleContext* rule = dynamic_cast<RuleContext*>(es);
            size_t idx = rule->getRuleIndex();

            if (idx == gscParser::RuleNamespace) {
                if (!ParseNamespace(rule, parser, obj)) {
                    return false;
                }
            }
            else if (idx == gscParser::RuleFilenamespace) {
                if (!ParseFileNamespace(rule, parser, obj)) {
                    return false;
                }
            }
        }

        obj.devBlockDepth = 0;
        for (auto& es : prog->children) {
            if (es == eof) {
                break; // done
            }

            if (es->getTreeType() == TREE_TERMINAL) {
                std::string txt{ es->getText() };

                if (txt == "/#") {
                    obj.devBlockDepth++;
                }
                else if (txt == "#/") {
                    if (!obj.devBlockDepth) {
                        obj.info.PrintLineMessage(alogs::LVL_ERROR, es, "Usage of #/ with no starting /#");
                        return false;
                    }
                    else {
                        obj.devBlockDepth--;
                    }
                }
                else {
                    obj.info.PrintLineMessage(alogs::LVL_ERROR, es, std::format("Bad export terminal {}", txt));
                    return false;
                }

                continue;
            }

            RuleContext* rule = dynamic_cast<RuleContext*>(es);

            switch (rule->getRuleIndex()) {
            case gscParser::RuleInclude:
                if (!ParseInclude(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleNamespace:
                if (!ParseNamespace(rule, parser, obj)) {
                    return false;
                }
                break; 
            case gscParser::RuleFilenamespace:
                if (!ParseFileNamespace(rule, parser, obj)) {
                    return false;
                }
                break;
            case gscParser::RuleFunction:
                if (!ParseFunction(rule, parser, obj)) {
                    return false;
                }
                break;
            default:
                obj.info.PrintLineMessage(alogs::LVL_ERROR, es, "Bad export rule");
                return false;
            }
        }

        if (obj.devBlockDepth > 0) {
            obj.info.PrintLineMessage(alogs::LVL_ERROR, prog, std::format("Missing {} #/", obj.devBlockDepth));
            return false;
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

        auto* readerBuilder = tool::gsc::GetGscReader(opt.m_vmInfo->vm);

        if (!readerBuilder) {
            LOG_ERROR("No GSC handler available for {}", opt.m_vmInfo->name);
            return tool::BASIC_ERROR;
        }

        std::shared_ptr<tool::gsc::GSCOBJHandler> handler {(*readerBuilder)(nullptr, 0)};

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
                // GscFile
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
                lineGsc = startLine + lineCount;
                info.gscData = info.gscData + dt.buffer + "\n";
            }

            preprocessor::PreProcessorOption popt = opt.processorOpt;
            popt.defines.insert("_SUPPORTS_GCSC");
            popt.defines.insert("_SUPPORTS_DETOURS");
            popt.defines.insert(utils::UpperCase(utils::va("_%s", opt.m_vmInfo->codeName)));
            popt.defines.insert(utils::MapString(utils::va("_%s", PlatformName(opt.m_platform)), [](char c) -> char { return isspace(c) ? '_' : std::toupper(c); }));

            if (tool::gsc::opcode::HasOpCode(opt.m_vmInfo->vm, opt.m_platform, OPCODE_T8C_GetLazyFunction)) {
                popt.defines.insert("_SUPPORTS_LAZYLINK");
            }

            if (client) {
                popt.defines.insert("_CSC");
            }
            else {
                popt.defines.insert("_GSC");
            }

            if (!popt.ApplyPreProcessor(info.gscData, 
                [&info](alogs::loglevel lvl, size_t line, const std::string& message) { info.PrintLineMessage(lvl, line, 0, message); })) {
                LOG_ERROR("Error when applying preprocessor on data");
                return tool::BASIC_ERROR;
            }

            if (opt.m_preproc) {
                utils::WriteFile(opt.m_preproc, info.gscData);
            }

            ANTLRInputStream is{ info.gscData };

            auto errList = std::make_unique<ACTSErrorListener>(info);

            gscLexer lexer{ &is };
            lexer.addErrorListener(&*errList);
            CommonTokenStream tokens{ &lexer };

            tokens.fill();
            gscParser parser{ &tokens };

            parser.removeErrorListeners();

            parser.addErrorListener(&*errList);

            gscParser::ProgContext* prog = parser.prog();
            CompileObject obj{ opt, client ? FILE_CSC : FILE_GSC, info, opt.m_vmInfo, opt.m_platform, handler };

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

            LOG_INFO("Done into {} ({}/{})", outFile, obj.vmInfo->codeName, PlatformName(obj.plt));
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

    ADD_TOOL("gscc", "gsc", " --help", "gsc compiler (Alpha)", nullptr, compiler);
}