#include <includes.hpp>
#include <tools/gsc/compiler/gsc_compiler_function_object.hpp>
#include <tools/gsc/compiler/gsc_compiler_parser.hpp>
#include <tools/gsc/compiler/gsc_compiler_script_object.hpp>

namespace tool::gsc::compiler {

    AscmNode* FunctionObject::CreateParamNode() const {
        return new AscmNodeCreateLocalVariables(m_vars, m_allocatedVar, (size_t)m_params, *this, obj.config.obfuscate);
    }

    void FunctionObject::AddNode(ParseTree* tree, AscmNode* node) {
        Token* token = obj.info.GetToken(tree);
        if (token) {
            node->line = token->getLine();
        }
        m_nodes.push_back(node);
    }

    AscmNode* FunctionObject::CreateFieldHashRaw(uint64_t v) const {
        return m_vmInfo->HasFlag(VmFlags::VMF_HASH64) ? (AscmNode*)new AscmNodeRawData<uint64_t>(v) : new AscmNodeRawData<uint32_t>((uint32_t)v);
    }

    AscmNode* FunctionObject::CreateFieldHash(uint64_t v, OPCode op) const {
        return new AscmNodeHash(v, op, m_vmInfo->HasFlag(VmFlags::VMF_HASH64) ? 8 : 4);
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

    AscmNode* FunctionObject::CreateFieldHashRaw(const char* v) const {
        obj.AddHash(v);
        return CreateFieldHashRaw(m_vmInfo->HashField(v));
    }

    AscmNode* FunctionObject::CreateFieldHashRaw(const std::string& v) const {
        obj.AddHash(v);
        return CreateFieldHashRaw(m_vmInfo->HashField(v));
    }


    AscmNode* FunctionObject::PeekBreakNode() const {
        if (m_jumpBreak.size()) {
            return m_jumpBreak.top();
        }
        return nullptr;
    }

    AscmNode* FunctionObject::PeekContinueNode() const {
        if (m_jumpContinue.size()) {
            return m_jumpContinue.top();
        }
        return nullptr;
    }

    void FunctionObject::PushBreakNode(AscmNode* node) {
        m_jumpBreak.push(node);
    }

    void FunctionObject::PopBreakNode() {
        m_jumpBreak.pop();
    }

    void FunctionObject::PushContinueNode(AscmNode* node) {
        m_jumpContinue.push(node);
    }

    void FunctionObject::PopContinueNode() {
        m_jumpContinue.pop();
    }

    FunctionVar* FunctionObject::VarEnd() {
        return &m_vars[m_allocatedVar];
    }

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

    FunctionVar* FunctionObject::FindVar(const std::string& name) {
        return std::find_if(std::begin(m_vars), VarEnd(), [&name](const FunctionVar& var) -> bool { return name == var.name; });
    }

    std::pair<const char*, FunctionVar*> FunctionObject::RegisterVarRnd() {
        return RegisterVar(std::format("$$v{:x}", rndVarStart++), false);
    }

    std::pair<const char*, FunctionVar*> FunctionObject::GetSpecialTmpVar() {
        if (!specialTempVar) {
            auto [err, var] = RegisterVar("$$tmp", false);

            if (err) {
                return std::make_pair<>(err, var);
            }

            specialTempVar = var;
        }

        return std::make_pair<>(nullptr, specialTempVar);
    }

    int32_t FunctionObject::ComputeRelativeLocations(int32_t floc) {
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

    void FunctionObject::OptimizeNodes() {
        // todo: find something to do?
    }

}