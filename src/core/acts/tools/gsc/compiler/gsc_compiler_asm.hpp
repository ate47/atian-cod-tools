#pragma once
#include <tools/gsc/compiler/gsc_compiler_context.hpp>

namespace tool::gsc::compiler {
    class CompileObject;

    using namespace tool::gsc::opcode;

    enum AscmNodeType {
        ASCMNT_UNKNOWN = 0,
        ASCMNT_OPCODE = 1,
        ASCMNT_OPCODE_RAW = 1,
        ASCMNT_RAW = 2,
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


    class AscmNodeRaw : public AscmNode {
    public:
        std::vector<byte> data{};
        uint32_t align;

        AscmNodeRaw(size_t align = 1) : align((uint32_t)align) {
            nodetype = ASCMNT_RAW;
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            if (aligned) {
                start = (start + (align - 1)) & ~(align - 1);
            }
            return start + (uint32_t)data.size();
        }

        bool Write(AscmCompilerContext& ctx) override {
            ctx.Align(align);
            ctx.data.insert(ctx.data.end(), data.begin(), data.end());
            return true;
        }
    };

    template<typename Type>
    class AscmNodeRawData : public AscmNodeRaw {
    public:
        AscmNodeRawData(Type t) : AscmNodeRaw(sizeof(Type)) {
            utils::WriteValue<Type>(data, t);
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
            auto [ok, op] = GetOpCodeId(ctx.vmInfo->vmMagic, ctx.plt, opcode, ctx.cfg.useModToolOpCodes);
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

        virtual uint32_t GetDataFLoc(bool aligned) const {
            throw std::runtime_error("GetDataFLoc not implemented");
        }
    };

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

        uint32_t GetDataFLoc(bool aligned) const override {
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

        uint32_t GetDataFLoc(bool aligned) const override {
            return ShiftSize(floc, aligned) - hashSize;
        }
    };

    class AscmNodeStringData : public AscmNodeOpCode {
    public:
        std::string string;

        AscmNodeStringData(std::string string, OPCode opcode) : AscmNodeOpCode(opcode), string(string) {
        }

        uint32_t ShiftSize(uint32_t start, bool aligned) const override {
            return AscmNodeOpCode::ShiftSize(start, aligned) + (uint32_t)string.size() + 1;
        }

        bool Write(AscmCompilerContext& ctx) override {
            if (!AscmNodeOpCode::Write(ctx)) {
                return false;
            }

            for (size_t i = 0; i < string.length(); i++) {
                ctx.Write<char>(string[i]);
            }
            ctx.Write<char>(0);
            return true;
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
        FCF_BULL_PATH = 0x80,
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
        bool useIWCalls;
        uint32_t hashSize;

        AscmNodeFunctionCall(OPCode opcode, int flags, byte params, uint64_t clsName, uint64_t nameSpace, VmInfo* vm)
            : AscmNodeOpCode(opcode), flags(flags), params(params), clsName(clsName), nameSpace(nameSpace) {
            inlineCall = vm->HasFlag(VmFlags::VMF_ALIGN);
            useIWCalls = vm->HasFlag(VmFlags::VMF_IW_CALLS);
            hashSize = vm->HasFlag(VmFlags::VMF_HASH64) ? 8 : 4;
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
                    return utils::Aligned<uint32_t>(AscmNodeOpCode::ShiftSize(start, aligned) + (useParams ? 1 : 0)) + hashSize;
                }
                if (flags & FCF_POINTER) {
                    return AscmNodeOpCode::ShiftSize(start, aligned) + (useParams ? 1 : 0);
                }
                return utils::Aligned<uint64_t>(AscmNodeOpCode::ShiftSize(start, aligned) + (useParams ? 1 : 0)) + dataSize;
            }

            start = AscmNodeOpCode::ShiftSize(start, aligned);
            if ((inlineCall && useParams) || (useParams && useIWCalls)) {
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

            if ((inlineCall && useParams) || (useParams && useIWCalls)) {
                ctx.Write<byte>(params);
            }

            if (flags & FCF_POINTER_CLASS) {
                switch (hashSize) {
                case 4:
                    ctx.Align<uint32_t>();
                    ctx.Write<uint32_t>((uint32_t)clsName);
                    break;
                case 8:
                    ctx.Align<uint64_t>();
                    ctx.Write<uint64_t>(clsName);
                    break;
                default: throw std::runtime_error(utils::va("Invalid hash size cls ptr %d", hashSize));
                }
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

            if ((opcode == OPCODE_CallBuiltinFunction || opcode == OPCODE_CallBuiltinMethod) && useIWCalls) {
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
        bool obfuscate;
        uint32_t hashSize;
    public:
        AscmNodeCreateLocalVariables(const FunctionVar* lvars, size_t count, size_t params, const FunctionObject& fobj, bool obfuscate);

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
                    }
                }
                else if (hasRegisters) {
                    if (vars.size() - params) {
                        e = opCodeUndef.ShiftSize(e, aligned);
                        e++; // add var count

                        if (aligned) {
                            e = (e + (hashSize - 1)) & ~(hashSize - 1);
                        }

                        e += hashSize * (uint32_t)(vars.size() - params);
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
                            ctx.Write<uint64_t>(obfuscate ? (uint64_t)(i + 1) : ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        else {
                            ctx.Align<uint32_t>();
                            ctx.Write<uint32_t>(obfuscate ? (uint32_t)(i + 1) : (uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
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
                            ctx.Write<uint64_t>(obfuscate ? (uint64_t)(i + 1) : ctx.vmInfo->HashField(var.name.c_str()));
                        }
                        else {
                            ctx.Align<uint32_t>();
                            ctx.Write<uint32_t>(obfuscate ? (uint32_t)(i + 1) : (uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
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
                                ctx.Write<uint64_t>(obfuscate ? (uint64_t)(i + 1) : ctx.vmInfo->HashField(var.name.c_str()));
                            }
                            else {
                                ctx.Align<uint32_t>();
                                ctx.Write<uint32_t>(obfuscate ? (uint32_t)(i + 1) : (uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
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
                        ctx.Write<uint64_t>(obfuscate ? (uint64_t)(var.id + 1) : ctx.vmInfo->HashField(var.name.c_str()));
                    }
                    else {
                        ctx.Align<uint32_t>();
                        ctx.Write<uint32_t>(obfuscate ? (uint32_t)(var.id + 1) : (uint32_t)ctx.vmInfo->HashField(var.name.c_str()));
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

        AscmNodeGlobalVariable(GlobalVariableDef* def, OPCode op) :
            AscmNodeOpCode(op), def(def) {
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

        uint32_t GetDataFLoc(bool aligned) const override {
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

}