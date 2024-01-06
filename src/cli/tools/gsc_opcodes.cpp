#include <includes.hpp>
#include "gsc_opcodes_load.hpp"
using namespace tool::gsc::opcode;

// maps to find the opcodes

// VM->vminfo
static std::unordered_map<BYTE, VmInfo> g_opcodeMap{};
// opcode->opcode handler
static std::unordered_map<OPCode, const OPCodeInfo*> g_opcodeHandlerMap{};

#pragma region opcode_node


Platform tool::gsc::opcode::PlatformOf(LPCCH name) {
	if (!_strcmpi("pc", name)) {
		return PLATFORM_PC;
	}
	if (!_strcmpi("xbox", name) || !_strcmpi("xb", name)) {
		return PLATFORM_XBOX;
	}
	if (!_strcmpi("ps", name) || !_strcmpi("ps4", name) || !_strcmpi("playstation", name)) {
		return PLATFORM_PLAYSTATION;
	}
	return PLATFORM_UNKNOWN;
}

LPCCH tool::gsc::opcode::PlatformName(Platform plt) {
	switch (plt) {
	case PLATFORM_PC: return "PC";
	case PLATFORM_XBOX: return "Xbox";
	case PLATFORM_PLAYSTATION: return "PlayStation";
	default: return "Unknown";
	}
}

size_t SizeNoEmptyNode(const std::vector<ASMContextStatement>& statements) {
	size_t acc = 0;
	for (const auto& stmt : statements) {
		if (stmt.node->m_type != TYPE_PRECODEPOS) {
			acc++;
		}
	}
	return acc;
}

ASMContextNode* GetNoEmptyNode(const std::vector<ASMContextStatement>& statements, size_t index) {
	for (const auto& stmt : statements) {
		if (stmt.node->m_type != TYPE_PRECODEPOS) {
			if (index-- == 0) {
				return stmt.node;
			}
		}
	}
	assert(0);
	return nullptr;
}

OPCodeInfo::OPCodeInfo(OPCode id, LPCCH name) : m_id(id), m_name(name) {
}

ASMContextLocationOp::~ASMContextLocationOp() {
}

void ASMContextLocationOp::Run(ASMContext& context, T8GSCOBJContext& objctx) const {
	assert(0);
}

ASMContextNode::ASMContextNode(ASMContextNodePriority priority, ASMContextNodeType type) : m_priority(priority), m_type(type) {
}

ASMContextNode::~ASMContextNode() {
}

void ASMContextNode::ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx) {
	// do nothing, no sub blocks
}

ASMContextNodeBlock::ASMContextNodeBlock(nodeblocktype blockType) : ASMContextNode(PRIORITY_INST, TYPE_BLOCK), m_blockType(blockType) {
	m_renderSemicolon = false;
}

ASMContextNodeBlock::~ASMContextNodeBlock() {
	for (auto& ref : m_statements) {
		delete ref.node;
	}
}

ASMContextStatement* ASMContextNodeBlock::FetchFirstForLocation(INT64 rloc) {
	for (auto& loc : m_statements) {
		if (loc.location->rloc >= rloc) {
			return &loc;
		}
	}
	return nullptr;
}

ASMContextNode* ASMContextNodeBlock::Clone() const {
	ASMContextNodeBlock* n = new ASMContextNodeBlock(m_blockType);
	for (auto& node : m_statements) {
		n->m_statements.push_back({ node.node->Clone(), node.location });
	}
	return n;
}

void ASMContextNodeBlock::ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) {
	// apply to this
	func(this, ctx);
}

void ASMContextNode::Dump(std::ostream& out, DecompContext& ctx) const {
	// nothing by default
}

ASMContextNode* ASMContextNode::Clone() const {
	assert(0); // shouldn't be possible
	return nullptr;
}

template<typename Type>
class ASMContextNodeValue : public ASMContextNode {
public:
	Type m_value;
	ASMContextNodeValue(Type value, ASMContextNodeType type = TYPE_UNDEFINED) : ASMContextNode(PRIORITY_VALUE, type), m_value(value) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << std::dec << m_value;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeValue<Type>(m_value, m_type);
	}
};

class ASMContextNodeIdentifier : public ASMContextNode {
public:
	UINT32 m_value;
	LPCCH m_type;
	ASMContextNodeIdentifier(UINT32 value, LPCCH type = "var") : ASMContextNode(PRIORITY_VALUE, TYPE_IDENTIFIER), m_value(value), m_type(type) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << hashutils::ExtractTmp(m_type, m_value) << std::flush;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeIdentifier(m_value, m_type);
	}
};

class ASMContextNodeHash : public ASMContextNode {
public:
	UINT64 m_value;
	bool m_canonid;
	ASMContextNodeHash(UINT64 value, bool canonid = false) : ASMContextNode(PRIORITY_VALUE, TYPE_CONST_HASH), m_value(value), m_canonid(canonid) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_canonid) {
			out << "#" << hashutils::ExtractTmp("var", m_value) << std::flush;
		}
		else {
			out << "#\"" << hashutils::ExtractTmp("hash", m_value) << '"' << std::flush;
		}
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeHash(m_value, m_canonid);
	}
};

class ASMContextNodeVector : public ASMContextNode {
public:
	ASMContextNode* m_x;
	ASMContextNode* m_y;
	ASMContextNode* m_z;
	ASMContextNodeVector(ASMContextNode* x, ASMContextNode* y, ASMContextNode* z) : ASMContextNode(PRIORITY_VALUE), m_x(x), m_y(y), m_z(z) {
	}
	ASMContextNodeVector(FLOAT x, FLOAT y, FLOAT z) : ASMContextNodeVector(
		new ASMContextNodeValue<FLOAT>(x),
		new ASMContextNodeValue<FLOAT>(y),
		new ASMContextNodeValue<FLOAT>(z)
		) {
	}
	~ASMContextNodeVector() {
		delete m_x;
		delete m_y;
		delete m_z;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "(";
		m_x->Dump(out, ctx);
		out << ", ";
		m_y->Dump(out, ctx);
		out << ", ";
		m_z->Dump(out, ctx);
		out << ")";
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeVector(m_x->Clone(), m_y->Clone(), m_z->Clone());
	}
};

class ASMContextNodeString : public ASMContextNode {
public:
	LPCCH m_value;
	ASMContextNodeString(LPCCH value) : ASMContextNode(PRIORITY_VALUE), m_value(value) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "\"" << m_value << "\"" << std::flush;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeString(m_value);
	}
};

class ASMContextNodeFuncRef : public ASMContextNode {
public:
	LPCCH m_op;
	UINT32 m_func;
	UINT32 m_nsp;
	UINT64 m_script;
	ASMContextNodeFuncRef(LPCCH op, UINT32 func, UINT32 nsp = 0, UINT64 script = 0) : ASMContextNode(PRIORITY_VALUE),
		m_op(op), m_func(func), m_nsp(nsp), m_script(script) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << m_op;
		if (m_nsp) {
			out << hashutils::ExtractTmp("namespace", m_nsp) << std::flush;
			if (m_script) {
				out << "<" << hashutils::ExtractTmpScript(m_script) << ">" << std::flush;
			}
			out << "::";
		}
		out << hashutils::ExtractTmp("function", m_func) << std::flush;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeFuncRef(m_op, m_func, m_nsp, m_script);
	}
};

class ASMContextNodeRef : public ASMContextNode {
public:
	LPCCH m_op;
	ASMContextNode* m_var;
	ASMContextNodeRef(LPCCH op, ASMContextNode* var) : ASMContextNode(PRIORITY_VALUE),
		m_op(op), m_var(var) {
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << m_op;
		m_var->Dump(out, ctx);
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeRef(m_op, m_var->Clone());
	}
};

class ASMContextNodeArrayAccess : public ASMContextNode {
public:
	ASMContextNode* m_operandleft;
	ASMContextNode* m_operandright;
	ASMContextNodeArrayAccess(ASMContextNode* operandleft, ASMContextNode* operandright) :
		ASMContextNode(PRIORITY_ACCESS), m_operandleft(operandleft), m_operandright(operandright) {
	}
	~ASMContextNodeArrayAccess() {
		delete m_operandleft;
		delete m_operandright;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_operandleft->m_priority < m_priority) {
			out << "(";
			m_operandleft->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operandleft->Dump(out, ctx);
		}
		out << "[";

		m_operandright->Dump(out, ctx);

		out << "]";
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeArrayAccess(m_operandleft->Clone(), m_operandright->Clone());
	}
};

class ASMContextNodeArrayBuildNode : public ASMContextNode {
public:
	ASMContextNode* m_key;
	ASMContextNode* m_value;
	ASMContextNodeArrayBuildNode(ASMContextNode* key, ASMContextNode* value) :
		ASMContextNode(PRIORITY_SET), m_key(key), m_value(value) {
	}
	~ASMContextNodeArrayBuildNode() {
		delete m_key;
		delete m_value;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		m_key->Dump(out, ctx);

		out << ":";

		m_value->Dump(out, ctx);
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeArrayBuildNode(m_key->Clone(), m_value->Clone());
	}
};

class ASMContextNodeArrayBuild : public ASMContextNode {
public:
	std::vector<ASMContextNodeArrayBuildNode*> m_operands{};
	ASMContextNodeArrayBuild() :
		ASMContextNode(PRIORITY_ACCESS, TYPE_ARRAY_BUILD) {
	}
	~ASMContextNodeArrayBuild() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "[";

		if (m_operands.size()) {
			m_operands[0]->Dump(out, ctx);
		}
		for (size_t i = 1; i < m_operands.size(); i++) {
			out << ", ";
			m_operands[i]->Dump(out, ctx);
		}

		out << "]";
	}

	void AddValue(ASMContextNode* key, ASMContextNode* value) {
		m_operands.push_back(new ASMContextNodeArrayBuildNode(key, value));
	}

	ASMContextNode* Clone() const override {
		auto* n = new ASMContextNodeArrayBuild();

		for (const auto& ref : m_operands) {
			n->AddValue(ref->m_key->Clone(), ref->m_value->Clone());
		}

		return n;
	}
};

class ASMContextNodeStructBuild : public ASMContextNode {
public:
	std::vector<ASMContextNodeArrayBuildNode*> m_operands{};
	ASMContextNodeStructBuild() :
		ASMContextNode(PRIORITY_ACCESS, TYPE_STRUCT_BUILD) {
	}
	~ASMContextNodeStructBuild() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "{";

		if (m_operands.size()) {
			m_operands[0]->Dump(out, ctx);
		}
		for (size_t i = 1; i < m_operands.size(); i++) {
			out << ", ";
			m_operands[i]->Dump(out, ctx);
		}

		out << "}";
	}

	void AddValue(ASMContextNode* key, ASMContextNode* value) {
		if (key->m_type == TYPE_CONST_HASH) {
			// this hash is a canon id, not a fnva1
			static_cast<ASMContextNodeHash*>(key)->m_canonid = true;
		}
		m_operands.push_back(new ASMContextNodeArrayBuildNode(key, value));
	}

	ASMContextNode* Clone() const override {
		auto* n = new ASMContextNodeStructBuild();

		for (const auto& ref : m_operands) {
			n->AddValue(ref->m_key->Clone(), ref->m_value->Clone());
		}

		return n;
	}
};

enum ASMContextNodeCallFuncPtrType {
	CLASS_CALL,
	PARAMS_CALL,
	POINTER_CALL,
	FUNCTION_CALL
};

enum ASMContextNodeCallFuncFlag : BYTE {
	THREAD_CALL = 1,
	CHILDTHREAD_CALL = 2,
	SELF_CALL = 4
};

class ASMContextNodeCallFuncPtr : public ASMContextNode {
private:
	ASMContextNodeCallFuncPtrType m_type;
	BYTE m_flags;
public:
	std::vector<ASMContextNode*> m_operands{};
	ASMContextNodeCallFuncPtr(ASMContextNodeCallFuncPtrType type, BYTE flags) : ASMContextNode(PRIORITY_ACCESS, TYPE_FUNC_CALL), m_type(type), m_flags(flags) {
	}
	~ASMContextNodeCallFuncPtr() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void AddParam(ASMContextNode* node) {
		m_operands.push_back(node);
	}

	ASMContextNode* Clone() const override {
		auto* ref = new ASMContextNodeCallFuncPtr(m_type, m_flags);
		ref->m_operands.reserve(m_operands.size());
		for (const auto& op : m_operands) {
			ref->m_operands.push_back(op->Clone());
		}

		return ref;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		size_t start = 0;
		if (m_flags & SELF_CALL) {
			m_operands[start]->Dump(out, ctx);
			out << " ";
			start++;
		}
		if (m_flags & CHILDTHREAD_CALL) {
			out << "childthread ";
		}
		else if (m_flags & THREAD_CALL) {
			out << "thread ";
		}

		switch (m_type) {
		case ASMContextNodeCallFuncPtrType::CLASS_CALL:
			assert(m_operands.size() >= start + 2);
			out << "[[ ";
			m_operands[start]->Dump(out, ctx);
			out << " ]]->";
			m_operands[start + 1]->Dump(out, ctx);
			start += 2;
			break;
		case ASMContextNodeCallFuncPtrType::POINTER_CALL:
			assert(m_operands.size() >= start + 1);
			out << "[[ ";
			m_operands[start]->Dump(out, ctx);
			out << " ]]";
			start++;
			break;
		case ASMContextNodeCallFuncPtrType::FUNCTION_CALL:
			assert(m_operands.size() >= start + 1);
			m_operands[start]->Dump(out, ctx);
			start++;
			break;
		case ASMContextNodeCallFuncPtrType::PARAMS_CALL:
			// a params call is only printing the params
			break;
		default:
			out << "<error CallFuncPtrType:" << m_type << ">";
			break;
		}

		out << "(";

		for (size_t i = start; i < m_operands.size(); i++) {
			if (i != start) {
				out << ", ";
			}
			m_operands[i]->Dump(out, ctx);
		}
		out << ")";
	}
};

class ASMContextNodeTernary : public ASMContextNode {
public:
	ASMContextNode* m_operand1;
	ASMContextNode* m_operand2;
	ASMContextNode* m_operand3;
	ASMContextNodeTernary(ASMContextNode* operand1, ASMContextNode* operand2, ASMContextNode* operand3) :
		ASMContextNode(PRIORITY_TERNARY, TYPE_TERNARY), m_operand1(operand1), m_operand2(operand2), m_operand3(operand3) {
	}
	~ASMContextNodeTernary() {
		delete m_operand1;
		delete m_operand2;
		delete m_operand3;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeTernary(m_operand1->Clone(), m_operand2->Clone(), m_operand3->Clone());
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_operand1->m_priority < m_priority) {
			out << "(";
			m_operand1->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand1->Dump(out, ctx);
		}

		out << " ? ";

		if (m_operand2->m_priority < m_priority) {
			out << "(";
			m_operand2->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand2->Dump(out, ctx);
		}

		out << " : ";

		if (m_operand3->m_priority < m_priority) {
			out << "(";
			m_operand3->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand3->Dump(out, ctx);
		}

	}
};

class ASMContextNodeOp2 : public ASMContextNode {
public:
	LPCCH m_description;
	ASMContextNode* m_operand1;
	ASMContextNode* m_operand2;
	ASMContextNodeOp2(LPCCH description, ASMContextNodePriority priority, ASMContextNode* operand1, ASMContextNode* operand2) :
		ASMContextNode(priority), m_description(description), m_operand1(operand1), m_operand2(operand2) {
	}
	~ASMContextNodeOp2() {
		delete m_operand1;
		delete m_operand2;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeOp2(m_description, m_priority, m_operand1->Clone(), m_operand2->Clone());
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_operand1->m_priority < m_priority) {
			out << "(";
			m_operand1->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand1->Dump(out, ctx);
		}

		out << " " << m_description << " ";

		if (m_operand2->m_priority < m_priority) {
			out << "(";
			m_operand2->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand2->Dump(out, ctx);
		}
	}
};

class ASMContextNodeOp1 : public ASMContextNode {
public:
	LPCCH m_description;
	bool m_prefix;
	ASMContextNode* m_operand;
	ASMContextNodeOp1(LPCCH description, bool prefix, ASMContextNode* operand, ASMContextNodeType type = TYPE_UNDEFINED) :
		ASMContextNode(PRIORITY_UNARY, type), m_prefix(prefix), m_description(description), m_operand(operand) {
		assert(operand);
	}
	~ASMContextNodeOp1() {
		delete m_operand;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeOp1(m_description, m_prefix, m_operand->Clone(), m_type);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_prefix) {
			out << m_description;
		}
		if (m_operand->m_priority < m_priority) {
			out << "(";
			m_operand->Dump(out, ctx);
			out << ")";
		}
		else {
			m_operand->Dump(out, ctx);
		}

		if (!m_prefix) {
			out << m_description;
		}
	}
};

class ASMContextNodeMultOp : public ASMContextNode {
public:
	LPCCH m_description;
	bool m_caller;
	std::vector<ASMContextNode*> m_operands{};
	ASMContextNodeMultOp(LPCCH description, bool caller, ASMContextNodeType type = TYPE_STATEMENT) :
		ASMContextNode(PRIORITY_VALUE, type), m_description(description), m_caller(caller) {
	}
	~ASMContextNodeMultOp() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	ASMContextNode* Clone() const override {
		auto* ref = new ASMContextNodeMultOp(m_description, m_caller, m_type);
		ref->m_operands.reserve(m_operands.size());
		for (const auto& op : m_operands) {
			ref->m_operands.push_back(op->Clone());
		}

		return ref;
	}

	void AddParam(ASMContextNode* node) {
		m_operands.push_back(node);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		size_t start = 0;
		if (m_caller) {
			if (!m_operands.size()) {
				std::cerr << "empty MultOp " << m_description << "\n";
				return; // wtf?
			}
			m_operands[0]->Dump(out, ctx);
			out << " ";
			start = 1;
		}
		out << m_description << "(";

		for (size_t i = start; i < m_operands.size(); i++) {
			const auto& operand = m_operands[i];
			if (i != start) {
				out << ", ";
			}
			operand->Dump(out, ctx);
		}
		out << ")";
	}
};

class ASMContextNodeFunctionOperator : public ASMContextNode {
public:
	LPCCH m_operatorName;
	ASMContextNode* m_self;
	ASMContextNode* m_operand;
	ASMContextNodeFunctionOperator(LPCCH operatorName, ASMContextNode* self, ASMContextNode* operand, ASMContextNodeType type = TYPE_STATEMENT) :
		ASMContextNode(PRIORITY_VALUE, type), m_operatorName(operatorName), m_self(self), m_operand(operand) {
		assert(operand);
	}
	~ASMContextNodeFunctionOperator() {
		if (m_self) {
			delete m_self;
		}
		delete m_operand;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeFunctionOperator(m_operatorName, m_self ? m_self->Clone() : nullptr, m_operand->Clone(), m_type);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_self) {
			m_self->Dump(out, ctx);
			out << " ";
		}
		out << m_operatorName;
		out << "(";
		m_operand->Dump(out, ctx);
		out << ")";
	}
};

enum ASMContextNodeJumpOperatorSpecialJump {
	SPECIAL_JUMP_DEFAULT = 0,
	SPECIAL_JUMP_BREAK,
	SPECIAL_JUMP_CONTINUE
};

class ASMContextNodeJumpOperator : public ASMContextNode {
public:
	LPCCH m_operatorName;
	ASMContextNode* m_operand;
	INT64 m_location;
	INT32 m_opLoc;
	bool m_showJump;
	ASMContextNodeJumpOperatorSpecialJump m_special_op;
	INT64 m_delta;
	ASMContextNodeJumpOperator(LPCCH operatorName, ASMContextNode* operand, INT64 location, ASMContextNodeType type, INT32 opLoc, bool showJump = true, INT64 delta = 0, ASMContextNodeJumpOperatorSpecialJump special_op = SPECIAL_JUMP_DEFAULT) :
		ASMContextNode(PRIORITY_INST, type), m_operatorName(operatorName), m_operand(operand), m_location(location), m_opLoc(opLoc), m_showJump(showJump), m_delta(delta), m_special_op(special_op){
		assert(IsJumpType(type));
	}
	~ASMContextNodeJumpOperator() {
		if (m_operand) {
			delete m_operand;
		}
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeJumpOperator(m_operatorName, m_operand ? m_operand->Clone() : nullptr, m_location, m_type, m_opLoc, m_showJump, m_delta, m_special_op);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		// we don't show the jump if asked, continue/break?
		if (ctx.asmctx.m_opt.m_show_jump_delta) {
			out << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << m_opLoc << ":";
			if (!m_showJump) {
				out << "(hide)";
			}
		}

		// fake an if if the jump isn't a cond jump
		bool fakeIf = m_special_op && m_operand;

		if (!fakeIf) {
			out << m_operatorName;
		}

		if (m_operand) {
			if (m_operand->m_type != TYPE_JUMP_STACK_TOP) {
				if (fakeIf) {
					out << "if ";
				}
				out << "(";
				m_operand->Dump(out, ctx);
				out << ")";
				if (fakeIf) {
					out << " {\n";
					ctx.padding++;
					ctx.WritePadding(out);
					out << m_operatorName;
				}
			}
			else if (ctx.asmctx.m_opt.m_show_jump_delta) {
				m_operand->Dump(out, ctx);
				out << " stored:(";
				static_cast<ASMContextNodeValue<ASMContextNode*>*>(m_operand)->m_value->Dump(out, ctx);
				out << ")";
			}

		}
		if (m_showJump || ctx.asmctx.m_opt.m_show_jump_delta) {
			out << " LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << m_location;
			if (!m_showJump) {
				out << "(hide)";
			}
		}
		if (ctx.asmctx.m_opt.m_show_jump_delta) {
			out << " delta: " << std::hex << (m_delta < 0 ? "-0x" : "0x") << (m_delta < 0 ? -m_delta : m_delta) << " (type:" << m_type << "/spec:" << (m_special_op ? "true" : "false") << ")";
		}
		if (fakeIf) {
			out << ";\n";
			ctx.padding--;
			ctx.WritePadding(out);
			out << "}";
		}
	}
};

class ASMContextNodeLeftRightOperator : public ASMContextNode {
public:
	LPCCH m_operatorName;
	ASMContextNode* m_left;
	ASMContextNode* m_right;
	ASMContextNodeLeftRightOperator(ASMContextNode* left, ASMContextNode* right, LPCCH operatorName = " = ", ASMContextNodePriority priority = PRIORITY_SET, ASMContextNodeType type = TYPE_SET) :
		ASMContextNode(priority, type), m_operatorName(operatorName), m_left(left), m_right(right) {
	}
	~ASMContextNodeLeftRightOperator() {
		delete m_left;
		delete m_right;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeLeftRightOperator(m_left->Clone(), m_right->Clone(), m_operatorName, m_priority, m_type);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		m_left->Dump(out, ctx);

		out << m_operatorName;

		m_right->Dump(out, ctx);
	}
};

class ASMContextNodeNew : public ASMContextNode {
public:
	UINT32 m_classname;
	ASMContextNode* m_constructorCall;
	bool m_constructorCallDec;
	ASMContextNodeNew(UINT32 clsName, ASMContextNode* constructorCall = nullptr, bool constructorCallDec = false) :
		ASMContextNode(PRIORITY_VALUE, TYPE_NEW), m_classname(clsName), m_constructorCall(constructorCall), m_constructorCallDec(constructorCallDec) {
	}
	~ASMContextNodeNew() {
		if (m_constructorCall) {
			delete m_constructorCall;
		}
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeNew(m_classname, m_constructorCall ? m_constructorCall->Clone() : nullptr, m_constructorCallDec);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "new " << hashutils::ExtractTmp("class", m_classname) << std::flush;
		if (m_constructorCall) {
			m_constructorCall->Dump(out, ctx);
		}
	}
};

struct ASMContextSwitchPreComputeCase {
	ASMContextNode* casenode = nullptr;
	INT32 jumpLocation = 0;
};

class ASMContextNodeSwitchPreCompute : public ASMContextNode {
public:
	ASMContextNode* m_node;
	std::vector<ASMContextSwitchPreComputeCase> m_cases{};
	INT32 m_endLocation;
	ASMContextNodeSwitchPreCompute(ASMContextNode* node, INT32 endLocation = 0) :
		ASMContextNode(PRIORITY_INST, TYPE_SWITCH_PRECOMPUTE), m_node(node), m_endLocation(endLocation) {
		m_renderSemicolon = false;
	}
	~ASMContextNodeSwitchPreCompute() {
		delete m_node;
		for (auto& cs : m_cases) {
			if (cs.casenode) {
				delete cs.casenode;
			}
		}
	}

	ASMContextNode* Clone() const override {
		auto* sw = new ASMContextNodeSwitchPreCompute(m_node->Clone(), m_endLocation);
		sw->m_cases.reserve(m_cases.size());
		for (const auto& cs : m_cases) {
			sw->m_cases.push_back({ cs.casenode ? cs.casenode->Clone() : cs.casenode, cs.jumpLocation });
		}
		return sw;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "switchpre(";
		m_node->Dump(out, ctx);
		out << ") {\n";
		ctx.padding++;

		for (const auto& cs : m_cases) {
			if (cs.casenode) {
				ctx.WritePadding(out, true) << "case ";
				cs.casenode->Dump(out, ctx);
			}
			else {
				ctx.WritePadding(out, true) << "default";
			}
			out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << cs.jumpLocation << ";\n";
		}

		ctx.WritePadding(out, true) << "end: ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << m_endLocation << ";\n";
		ctx.padding--;
		//
		ctx.WritePadding(out, true) << "}\n";
	}
};

struct ASMContextSwitchPostComputeCase{
	ASMContextNode* value;
	ASMContextNodeBlock* block;
};

class ASMContextNodeSwitchPostCompute : public ASMContextNode {
public:
	ASMContextNode* m_node;
	std::vector<ASMContextSwitchPostComputeCase> m_cases{};
	ASMContextNodeSwitchPostCompute(ASMContextNode* node) :
		ASMContextNode(PRIORITY_INST, TYPE_SWITCH_POSTCOMPUTE), m_node(node) {
		m_renderSemicolon = false;
	}
	~ASMContextNodeSwitchPostCompute() {
		delete m_node;
		for (auto& cs : m_cases) {
			if (cs.value) {
				delete cs.value;
			}
			delete cs.block;
		}
	}

	ASMContextNode* Clone() const override {
		auto* sw = new ASMContextNodeSwitchPostCompute(m_node->Clone());
		sw->m_cases.reserve(m_cases.size());
		for (const auto& cs : m_cases) {
			sw->m_cases.push_back({ cs.value ? cs.value->Clone() : cs.value, static_cast<ASMContextNodeBlock*>(cs.block->Clone()) });
		}
		return sw;
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "switch (";
		m_node->Dump(out, ctx);
		out << ") {\n";

		for (const auto& cs : m_cases) {
			if (cs.value) {
				ctx.WritePadding(out, true) << "case ";
				cs.value->Dump(out, ctx);
				out << ":";
			}
			else {
				ctx.WritePadding(out, true) << "default:";
			}
			cs.block->Dump(out, ctx);
		}

		ctx.WritePadding(out, true) << "}\n";
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		for (const auto& cs : m_cases) {
			cs.block->ApplySubBlocks(func, ctx);
		}
	}
};

class ASMContextNodeForEach : public ASMContextNode {
public:
	UINT32 m_key;
	UINT32 m_item;
	ASMContextNode* m_arrayNode;
	ASMContextNodeBlock* m_block;
	ASMContextNodeForEach(ASMContextNode* arrayNode, ASMContextNodeBlock* block, UINT32 key, UINT32 item) :
		ASMContextNode(PRIORITY_INST, TYPE_FOR_EACH), m_arrayNode(arrayNode), m_block(block), m_key(key), m_item(item) {
		m_renderSemicolon = false;
	}
	~ASMContextNodeForEach() {
		delete m_arrayNode;
		delete m_block;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeForEach(m_arrayNode->Clone(), static_cast<ASMContextNodeBlock*>(m_block->Clone()), m_key, m_item);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "foreach (";
		if (m_key) {
			out << hashutils::ExtractTmp("var", m_key) << std::flush << ", ";
		}
		out << hashutils::ExtractTmp("var", m_item) << std::flush << " in ";
		m_arrayNode->Dump(out, ctx);
		out << ") ";
		m_block->Dump(out, ctx);
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class ASMContextNodeForDelta : public ASMContextNode {
public:
	ASMContextNode* m_init;
	ASMContextNode* m_cond;
	ASMContextNode* m_delta;
	ASMContextNodeBlock* m_block;
	ASMContextNodeForDelta(ASMContextNode* init, ASMContextNode* cond, ASMContextNode* delta, ASMContextNodeBlock* block) :
		ASMContextNode(PRIORITY_INST, TYPE_FOR), m_init(init), m_cond(cond), m_delta(delta), m_block(block) {
		assert(init);
		// condition can be null for(;;) converted into for(i=0;;i++)
		assert(delta);
		assert(block);
		m_renderSemicolon = false;
	}
	~ASMContextNodeForDelta() {
		delete m_init;
		delete m_cond;
		delete m_delta;
		delete m_block;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeForDelta(m_init->Clone(), m_cond ? m_cond->Clone() : nullptr, m_delta->Clone(), static_cast<ASMContextNodeBlock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "for (";
		m_init->Dump(out, ctx);
		out << "; ";
		if (m_cond) {
			m_cond->Dump(out, ctx);
		}
		out << "; ";
		m_delta->Dump(out, ctx);
		out << ") ";
		m_block->Dump(out, ctx);
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class ASMContextNodeDoWhile : public ASMContextNode {
public:
	ASMContextNode* m_condition;
	ASMContextNodeBlock* m_block;
	ASMContextNodeDoWhile(ASMContextNode* condition, ASMContextNodeBlock* block) :
		ASMContextNode(PRIORITY_INST, TYPE_DO_WHILE), m_condition(condition), m_block(block) {
		block->m_blockType = BLOCK_PADDING; // use custom block, no need to write the { }
	}
	~ASMContextNodeDoWhile() {
		delete m_condition;
		delete m_block;
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeDoWhile(m_condition->Clone(), static_cast<ASMContextNodeBlock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "do {";
		m_block->Dump(out, ctx);
		ctx.WritePadding(out, true) << "} while(";
		m_condition->Dump(out, ctx);
		out << ")";
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class ASMContextNodeWhile : public ASMContextNode {
public:
	ASMContextNode* m_condition;
	ASMContextNodeBlock* m_block;
	ASMContextNodeWhile(ASMContextNode* condition, ASMContextNodeBlock* block) :
		ASMContextNode(PRIORITY_INST, TYPE_WHILE), m_condition(condition), m_block(block) {
		m_renderSemicolon = false;
	}
	~ASMContextNodeWhile() {
		if (m_condition) {
			delete m_condition;
		}
		if (m_block) {
			delete m_block;
		}
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeWhile(m_condition ? m_condition->Clone() : nullptr, static_cast<ASMContextNodeBlock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		if (m_condition) {
			out << "while (";
			m_condition->Dump(out, ctx);
			out << ") ";
		}
		else {
			// while without condition, equivalent as "for (;;)"
			out << "for (;;) ";
		}
		m_block->Dump(out, ctx);
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class ASMContextNodeIfElse : public ASMContextNode {
public:
	ASMContextNode* m_condition;
	ASMContextNodeBlock* m_ifblock;
	ASMContextNodeBlock* m_elseblock;
	ASMContextNodeIfElse(ASMContextNode* condition, ASMContextNodeBlock* ifblock, ASMContextNodeBlock* elseblock) :
		ASMContextNode(PRIORITY_INST, TYPE_IF), m_condition(condition), m_ifblock(ifblock), m_elseblock(elseblock){
		assert(condition);
		assert(ifblock);
		m_renderSemicolon = false;
		// set padding
		ifblock->m_blockType = BLOCK_PADDING;
		if (elseblock) {
			elseblock->m_blockType = BLOCK_PADDING;
		}
	}
	~ASMContextNodeIfElse() {
		delete m_condition;
		delete m_ifblock;
		if (m_elseblock) {
			delete m_elseblock;
		}
	}

	ASMContextNode* Clone() const override {
		return new ASMContextNodeIfElse(m_condition->Clone(), 
			static_cast<ASMContextNodeBlock*>(m_ifblock->Clone()), 
			m_elseblock ? static_cast<ASMContextNodeBlock*>(m_elseblock->Clone()) : nullptr);
	}

	void Dump(std::ostream& out, DecompContext& ctx) const override {
		out << "if (";
		m_condition->Dump(out, ctx);
		out << ") {";
		m_ifblock->Dump(out, ctx);
		ASMContextNodeBlock* elseBlock = m_elseblock;

		// loop over all the nested if blocks to create a pretty itie*e output
		while (elseBlock && SizeNoEmptyNode(elseBlock->m_statements) == 1) {
			auto* ref = GetNoEmptyNode(elseBlock->m_statements, 0);
			if (ref->m_type != TYPE_IF) {
				break;
			}
			auto* ifb = static_cast<ASMContextNodeIfElse*>(ref);
			ctx.WritePadding(out, true) << "} else if (";
			ifb->m_condition->Dump(out, ctx);
			out << ") {";
			ifb->m_ifblock->Dump(out, ctx);
			elseBlock = ifb->m_elseblock;
		}
		if (elseBlock) {
			ctx.WritePadding(out, true) << "} else {";
			elseBlock->Dump(out, ctx);
		}
		ctx.WritePadding(out, true) << "}\n";
	}
	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_ifblock->ApplySubBlocks(func, ctx);
		if (m_elseblock) {
			m_elseblock->ApplySubBlocks(func, ctx);
		}
	}
};
#pragma endregion
#pragma region opcode_info

class ASMContextLocationOpOp : public ASMContextLocationOp {
private:
	ASMContextNode* m_node;
	ASMContextNodePriority m_priority;
	LPCCH m_description;
public:
	ASMContextLocationOpOp(ASMContextNode* node, LPCCH description, ASMContextNodePriority priority) :
		ASMContextLocationOp(), m_node(node), m_description(description), m_priority(priority) {
	}
	~ASMContextLocationOpOp() {
		if (m_node) {
			delete m_node;
		}
	}
	void Run(ASMContext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
		auto* right = context.PopASMCNode();
		context.PushASMCNode(new ASMContextNodeOp2(m_description, m_priority, m_node ? m_node->Clone() : nullptr, right));
	}
};
class ASMContextLocationOpCompleteTernary : public ASMContextLocationOp {
private:
	ASMContextNode* m_operand1;
	ASMContextNode* m_operand2;
public:
	ASMContextLocationOpCompleteTernary(ASMContextNode* operand1, ASMContextNode* operand2) :
		ASMContextLocationOp(), m_operand1(operand1), m_operand2(operand2) {
		assert(operand1);
		assert(operand2);
	}
	~ASMContextLocationOpCompleteTernary() {
		delete m_operand1;
		delete m_operand2;
	}
	void Run(ASMContext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
		auto* operand3 = context.PopASMCNode();
		context.PushASMCNode(new ASMContextNodeTernary(m_operand1->Clone(), m_operand2->Clone(), operand3));
	}
};

int OPCodeInfo::Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const {
	out << "\n";
	return 0; // by default nop
}

int tool::gsc::opcode::OPCodeInfo::Skip(UINT16 value, ASMSkipContext& ctx) const {
	return 0; // by default nop
}

class OPCodeInfounknown : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "Unknown operator: " << std::hex << value << "\n";

		for (size_t j = 0; j < 0x4; j++) {
			context.WritePadding(out);
			for (size_t i = 0; i < 0x8; i++) {
				if (i) {
					out << " ";
				}
				out << std::setfill('0') << std::setw(2) << std::hex << (int)context.m_bcl[i + j * 0x8];
			}
			out << "\n";
		}

		for (size_t i = 0; i < 0x3; i++) {
			auto* loc = utils::Aligned<UINT32>(context.m_bcl);

			auto hash = *reinterpret_cast<UINT32*>(loc + i * 4);
			auto* str = hashutils::ExtractPtr(hash);

			if (str) {
				context.WritePadding(out) << "#" << std::dec << i << ": " << str << " (var_" << std::hex << hash << ")\n";
			}
		}

		for (size_t i = 0; i < 0x3; i++) {
			auto* loc = utils::Aligned<UINT64>(context.m_bcl);

			auto hash = *reinterpret_cast<UINT64*>(loc + i * 8);
			auto* str = hashutils::ExtractPtr(hash);

			if (str) {
				context.WritePadding(out) << "#" << std::dec << i << ": #\"" << str << "\" (hash_" << std::hex << hash << ")\n";
			}
		}
		
		return -1;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_error = std::format("Unknown operator: {:x}", value);
		return -1;
	}
};

class OPCodeInfoUnknownDev : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& loc = context.Aligned<UINT16>();

		UINT16 val = *(UINT16*)loc;

		loc += 2;
		out << std::hex << val << "\n";
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT16>() += 2;
		return 0;
	}
};

class OPCodeInfoSafeCreateLocalVariables : public OPCodeInfo {
public:
	OPCodeInfoSafeCreateLocalVariables() : OPCodeInfo(OPCODE_SafeCreateLocalVariables, "SafeCreateLocalVariables") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE count = *context.m_bcl++;

		context.m_localvars.reserve((size_t)count + 1);

		out << std::hex << "count: 0x" << (int)count << "\n";
		if (!context.m_localvars.size()) {
			// the local variables starts at 1
			context.m_localvars.insert(context.m_localvars.begin(), { hashutils::Hash32("<error>"), 0 });
		}

		for (size_t i = 0; i < count; i++) {
			auto& bytecode = context.Aligned<UINT32>();
			context.WritePadding(out);
			UINT32 varName = *(UINT32*)bytecode;
			bytecode += 4;
			BYTE flags = *(bytecode++);

			// the variables are in reversed order
			context.m_localvars.insert(context.m_localvars.begin(), { varName, flags });

			out << hashutils::ExtractTmp("var", varName);

			if (flags & T8GSCLocalVarFlag::VARIADIC) {
				out << "...";
			}
			else if (flags & T8GSCLocalVarFlag::ARRAY_REF) {
				out << "&";
			}
			else if (context.m_vm == tool::gsc::opcode::VM_T9 && (flags & tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF)) {
				out << "*";
			}


			BYTE mask = ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF);

			if (context.m_vm == tool::gsc::opcode::VM_T9) {
				mask &= ~tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF;
			}

			if (flags & mask) {
				out << " unk flags: " << std::hex << (int)flags;
			}

			if (context.m_runDecompiler && context.m_opt.m_show_func_vars && i >= context.m_exp.param_count) {
				context.m_lastOpCodeBase = 0;
				context.PushASMCNode(new ASMContextNodeIdentifier(varName, "var"));
				context.CompleteStatement();
			}

			out << std::endl;
		}
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		BYTE count = *ctx.m_bcl++;
		for (size_t i = 0; i < count; i++) {
			// skip name
			ctx.Aligned<UINT32>() += 4;
			// skip flag
			ctx.m_bcl++;
		}
		return 0;
	}
};
class OPCodeInfoCheckClearParams : public OPCodeInfo {
public:
	OPCodeInfoCheckClearParams() : OPCodeInfo(OPCODE_CheckClearParams, "CheckClearParams") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetObjectType : public OPCodeInfo {
public:
	OPCodeInfoGetObjectType(OPCode id, LPCCH name, LPCCH hashType) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("class", name) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<LPCCH>("<emptypos_getobjecttype>", TYPE_PRECODEPOS));
			context.PushASMCNode(new ASMContextNodeNew(name));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetNeg : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<Type>();

		INT64 negv = -(INT64)*(Type*)bytecode;

		bytecode += sizeof(Type);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<INT64>(negv));
		}

		out << std::dec << negv << std::endl;

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<Type>() += sizeof(Type);
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstant : public OPCodeInfo {
	Type m_value;
public:
	OPCodeInfoGetConstant(OPCode id, LPCCH name, Type value) : OPCodeInfo(id, name), m_value(value) {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<Type>(m_value));
		}

		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstantRef : public OPCodeInfo {
	Type m_value;
public:
	OPCodeInfoGetConstantRef(OPCode id, LPCCH name, Type value) : OPCodeInfo(id, name), m_value(value) {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeValue<Type>(m_value));
		}

		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstantSet : public OPCodeInfo {
	Type m_value;
	bool m_inst;
public:
	OPCodeInfoGetConstantSet(OPCode id, LPCCH name, Type value, bool inst = false) : OPCodeInfo(id, name), m_value(value), m_inst(inst) {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(left, new ASMContextNodeValue<Type>(m_value)));
			if (m_inst) {
				context.CompleteStatement();
			}
		}

		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetObjectSize : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(left, new ASMContextNodeValue<LPCCH>("size"), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoVectorScale : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("vectorscale", false);

			node->AddParam(context.PopASMCNode());
			node->AddParam(context.PopASMCNode());

			context.PushASMCNode(node);
		}

		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type, typename WriteType = INT64>
class OPCodeInfoGetNumber : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<Type>();

		WriteType intValue = *(Type*)bytecode;

		bytecode += sizeof(Type);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<WriteType>(intValue));
		}

		out << std::dec << intValue << std::endl;

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<Type>() += sizeof(Type);
		return 0;
	}
};

class OPCodeInfoGetHash : public OPCodeInfo {
public:
	OPCodeInfoGetHash() : OPCodeInfo(OPCODE_GetHash, "GetHash") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<UINT64>();

		UINT64 hash = *(UINT64*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeHash(hash));
		}

		out << "#\"" << hashutils::ExtractTmp("hash", hash) << "\" (#" << std::hex << hash << ")" << std::endl;

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT64>() += 8;
		return 0;
	}
};

class OPCodeInfoJump : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// get the jump opcode location
		INT32 m_jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);

		ASMContextNode* node = nullptr;
		ASMContextNodeType type = TYPE_JUMP;
		LPCCH name = nullptr;
		bool pushBack = false;
		if (context.m_runDecompiler) {
			switch (m_id) {
			case OPCODE_JumpOnGreaterThan:
			{
				// reversed
				ASMContextNode* op2 = context.PopASMCNode();
				ASMContextNode* op1 = context.PopASMCNode();
				node = new ASMContextNodeOp2(">", PRIORITY_COMPARE, op1, op2);
				type = TYPE_JUMP_GREATERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnLessThan:
			{
				// reversed
				ASMContextNode* op2 = context.PopASMCNode();
				ASMContextNode* op1 = context.PopASMCNode();
				node = new ASMContextNodeOp2("<", PRIORITY_COMPARE, op1, op2);
				type = TYPE_JUMP_LOWERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnFalse:
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONFALSE;
				name = "jumpiffalse";
				break;
			case OPCODE_JumpOnTrue:
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONTRUE;
				name = "jumpiftrue";
				break;
			case OPCODE_DevblockBegin:
				type = TYPE_JUMP_DEVBLOCK;
				name = "jumpdev";
				break;
			case OPCODE_Jump:
				name = "goto";
				break;
			default:
				name = "jumpukn";
				assert(0);
				break;
			}
		}

		assert(!context.m_runDecompiler || name != nullptr);

		auto& bytecode = context.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;

		// push a location and mark it as referenced
		BYTE* newLoc = &context.m_bcl[delta];

		// push the node value during the jump if the operator is asking for it
		auto& locref = context.PushLocation(newLoc);

		if (context.m_runDecompiler) {
			bool inject = true;
			if (m_id == OPCODE_Jump && delta > 0) {
				// might be ternary
				//ASMContextNodeTernary
				if (context.m_stack.size() && context.m_funcBlock.m_statements.size()) {
					// at least one node in the stack and a previous statement
					auto& last = context.m_funcBlock.m_statements[context.m_funcBlock.m_statements.size() - 1];
					if (last.node->m_type != TYPE_JUMP && IsJumpType(last.node->m_type)) {
						auto* jumpNode = static_cast<ASMContextNodeJumpOperator*>(last.node);
						// is this operator pointing just after us?
						if (jumpNode->m_delta > 0 && jumpNode->m_location == context.FunctionRelativeLocation(utils::Aligned<INT16>(context.m_bcl))) {
							auto* top = context.PopASMCNode();
							if (top->m_type != TYPE_PRECODEPOS) {
								// bad top, no data
								//node = new ASMContextNodeValue<ASMContextNode*>(top, TYPE_JUMP_STACK_TOP);
								// we can remove the jump node, we won't need it
								context.m_lastOpCodeBase = last.location->rloc;
								context.m_funcBlock.m_statements.pop_back();
								locref.m_lateop.emplace_back(new ASMContextLocationOpCompleteTernary(jumpNode->m_operand, top));
								jumpNode->m_operand = nullptr;
								delete jumpNode;
								inject = false;
							}
							else {
								delete top;
							}
						}
					}
				}
			}
			if (inject) {
				context.PushASMCNode(new ASMContextNodeJumpOperator(name, node, locref.rloc, type, m_jumpLocation, true, delta));
				context.CompleteStatement();
			}
		}

		locref.refs.insert(m_jumpLocation);

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
		//return m_id == OPCODE_Jump ? -2 : 0; // no code after jump
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		INT32 m_jumpLocation = ctx.FunctionRelativeLocation(ctx.m_bcl - 2);
		auto& bytecode = ctx.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;

		ctx.PushLocation(&ctx.m_bcl[delta]);

		return 0;
	}
};

class OPCodeInfoJumpExpr : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		INT32 m_jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		// get the jump opcode location

		auto& bytecode = context.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;

		// push a location and mark it as referenced
		BYTE* newLoc = &context.m_bcl[delta];

		// push the node value during the jump if the operator is asking for it
		auto& locref = context.PushLocation(newLoc);

		if (context.m_runDecompiler) {
			ASMContextNodePriority priority = PRIORITY_VALUE;
			LPCCH desc = "<err>";
			if (context.m_runDecompiler) {
				switch (m_id) {
				case OPCODE_JumpOnFalseExpr:
					priority = PRIORITY_BOOL_AND;
					desc = "&&";
					break;
				case OPCODE_JumpOnTrueExpr:
					priority = PRIORITY_BOOL_OR;
					desc = "||";
					break;
				default:
					out << "bad op code jumpexpr: " << std::hex << m_id << "\n";
					return -1;
				}
			}
			ASMContextNode* node = context.PopASMCNode();
			locref.m_lateop.push_back(new ASMContextLocationOpOp(node, desc, priority));
		}

		locref.refs.insert(m_jumpLocation);

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		INT32 m_jumpLocation = ctx.FunctionRelativeLocation(ctx.m_bcl - 2);
		// get the jump opcode location

		auto& bytecode = ctx.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;
		
		ctx.PushLocation(&ctx.m_bcl[delta]);
		return 0;
	}
};

class OPCodeInfoJumpPush : public OPCodeInfo {
public:
	OPCodeInfoJumpPush() : OPCodeInfo(OPCODE_JumpPush, "JumpPush") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		INT32 jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& bytecode = context.Aligned<uintptr_t>();

		uintptr_t location = *(uintptr_t*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeJumpOperator("jumppush", nullptr, location, TYPE_STATEMENT, jumpLocation));
			context.CompleteStatement();
		}

		out << "Jump 0x" << std::hex << location << "\n";

		return -2; // no code after that
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uintptr_t>() += 8;
		return -2;
	}
};

class OPCodeInfoVector : public OPCodeInfo {
public:
	OPCodeInfoVector() : OPCodeInfo(OPCODE_Vector, "Vector") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* x = context.PopASMCNode();
			auto* y = context.PopASMCNode();
			auto* z = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}

		out << "\n";
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetVector : public OPCodeInfo {
public:
	OPCodeInfoGetVector() : OPCodeInfo(OPCODE_GetVector, "GetVector") {}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<FLOAT>();

		float x = *(FLOAT*)bytecode;
		bytecode += 4;

		bytecode = context.Aligned<FLOAT>();
		float y = *(FLOAT*)bytecode;
		bytecode += 4;

		bytecode = context.Aligned<FLOAT>();
		float z = *(FLOAT*)bytecode;
		bytecode += 4;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}
		out << "(" << x << ", " << y << ", " << z << ")\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<FLOAT>() += 4;
		ctx.Aligned<FLOAT>() += 4;
		ctx.Aligned<FLOAT>() += 4;
		return 0;
	}
};

class OPCodeInfoVectorConstant : public OPCodeInfo {
public:
	OPCodeInfoVectorConstant() : OPCodeInfo(OPCODE_VectorConstant, "VectorConstant") {}

	float OfFlag(BYTE loc) const {
		switch (loc & 0x3) {
		case 0: return 0;
		case 1: return -1;
		case 2: return 1;
		default: return 999;
		}
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE flag = *(context.m_bcl++);

		FLOAT x = OfFlag(flag >> 4);
		FLOAT y = OfFlag(flag >> 2);
		FLOAT z = OfFlag(flag);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}

		out << "(" << x << ", " << y<< ", " << z << ")\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoName : public OPCodeInfo {
private:
	LPCCH m_hashType;
public:
	OPCodeInfoName(OPCode id, LPCCH name, LPCCH hashType) : OPCodeInfo(id, name), m_hashType(hashType) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp(m_hashType, name) << std::endl;

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoSetVariableFieldRef : public OPCodeInfo {
public:
	OPCodeInfoSetVariableFieldRef(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			auto* node = new ASMContextNodeLeftRightOperator(
				new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS),
				context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET);
			context.SetFieldIdASMCNode(node->Clone());
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoSetVariableField : public OPCodeInfo {
public:
	OPCodeInfoSetVariableField(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9SetVariableFieldFromEvalArrayRef : public OPCodeInfo {
public:
	OPCodeT9SetVariableFieldFromEvalArrayRef() : OPCodeInfo(OPCODE_T9_SetVariableFieldFromEvalArrayRef, "SetVariableFieldFromEvalArrayRef") {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.GetFieldIdASMCNode());
			ASMContextNode* arrayNode = context.GetObjectIdASMCNode();
			ASMContextNode* accessNode = new ASMContextNodeArrayAccess(arrayNode, context.PopASMCNode());
			context.SetFieldIdASMCNode(accessNode);

			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9Iterator : public OPCodeInfo {
public:
	LPCCH m_op;
	ASMContextNodeType m_ittype;
	OPCodeT9Iterator(OPCode id, LPCCH name, LPCCH op, ASMContextNodeType ittype) : OPCodeInfo(id, name), m_op(op), m_ittype(ittype) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			ASMContextNode* iterator = context.PopASMCNode();

			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, false, m_ittype);

			node->AddParam(iterator);

			context.PushASMCNode(node);
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9GetVarRef : public OPCodeInfo {
public:
	OPCodeT9GetVarRef() : OPCodeInfo(OPCODE_T9_GetVarRef, "GetVarRef") {}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeRef("&", context.GetFieldIdASMCNode()->Clone()));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoEvalSelfFieldVariable : public OPCodeInfo {
public:
	OPCodeInfoEvalSelfFieldVariable(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(hashutils::Hash32("self")));
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoClearFieldVariable : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoClearFieldVariable(OPCode id, LPCCH name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		UINT32 name = 0;
		if (!m_stack) {
			auto& ref = context.Aligned<UINT32>();

			name = *(UINT32*)ref;

			ref += 4;

			out << hashutils::ExtractTmp("var", name) << std::endl;
		}
		else {
			out << "\n";
		}

		if (context.m_runDecompiler) {
			ASMContextNode* nameNode = m_stack ? context.PopASMCNode() : new ASMContextNodeIdentifier(name);
			ASMContextNode* fieldAccessNode = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), nameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(fieldAccessNode, new ASMContextNodeValue<LPCCH>("undefined"), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		if (!m_stack) {
			ctx.Aligned<UINT32>() += 4;
		}
		return 0;
	}
};

class OPCodeInfoEvalGlobalObjectFieldVariable : public OPCodeInfo {
public:
	OPCodeInfoEvalGlobalObjectFieldVariable() : OPCodeInfo(OPCODE_EvalGlobalObjectFieldVariable, "EvalGlobalObjectFieldVariable") {}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<UINT16>();

		UINT16 objectid = *(UINT16*)base;

		base += 2;
		auto& base2 = context.Aligned<UINT32>();
		UINT32 fieldName = *(UINT32*)(base2);
		base2 += 4;

		UINT32 name = objctx.GetGlobalVarName(objectid);

		if (name) {
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}
		else {
			out << "bad objectid stack: 0x" << std::hex << objectid;
		}

		out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(fieldName), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT16>() += 2;
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoCastAndEvalFieldVariable : public OPCodeInfo {
public:
	OPCodeInfoCastAndEvalFieldVariable(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		UINT32 name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoEvalFieldVariable : public OPCodeInfo {
private:
	bool m_stack;
	bool m_ref;
public:
	OPCodeInfoEvalFieldVariable(OPCode id, LPCCH name, bool stack, bool ref) : OPCodeInfo(id, name), m_stack(stack), m_ref(ref) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		UINT32 name = 0;

		if (!m_stack) {
			auto& ref = context.Aligned<UINT32>();

			name = *(UINT32*)ref;

			ref += 4;

			out << hashutils::ExtractTmp("var", name) << std::endl;
		}
		else {
			out << "\n";
		}

		if (context.m_runDecompiler) {
			ASMContextNode* node = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), m_stack ? context.PopASMCNode() : new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS);

			if (m_ref) {
				context.SetFieldIdASMCNode(node);
			}
			else {
				context.PushASMCNode(node);
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		if (!m_stack) {
			ctx.Aligned<UINT32>() += 4;
		}
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableCached : public OPCodeInfo {
	int count;
public:
	OPCodeInfoEvalLocalVariableCached(OPCode id, LPCCH name, int count = 1) : OPCodeInfo(id, name), count(count) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		for (size_t i = 0; i < count; i++) {
			if (i) {
				context.WritePadding(out);
			}
			int lvar = (int)*(context.m_bcl++);

			UINT32 name;
			if (lvar >= context.m_localvars.size()) {
				name = hashutils::Hash32("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
			}
			else {
				name = context.m_localvars[lvar].name;
				context.m_localvars_ref[name]++;
				out << hashutils::ExtractTmp("var", name) << std::endl;
			}

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeIdentifier(name));
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl += count;
		return 0;
	}
};

class OPCodeInfoSetLocalVariableCached : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoSetLocalVariableCached(OPCode id, LPCCH name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			if (!m_stack) {
				context.SetFieldIdASMCNode(new ASMContextNodeIdentifier(name));
				context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			}
			else {
				context.PushASMCNode(new ASMContextNodeLeftRightOperator(new ASMContextNodeIdentifier(name), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			}
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoFirstArrayKey : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoFirstArrayKey(OPCode id, LPCCH name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		if (!m_stack) {
			int lvar = (int)*(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				name = hashutils::Hash32("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
			}
			else {
				name = context.m_localvars[lvar].name;
				context.m_localvars_ref[name]++;
				out << hashutils::ExtractTmp("var", name) << std::endl;
			}
		}
		else {
			out << "\n";
		}

		if (context.m_runDecompiler) {
			ASMContextNode* arrayNode;
			if (m_stack) {
				arrayNode = context.PopASMCNode();
			}
			else {
				arrayNode = new ASMContextNodeIdentifier(name);
			}

			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("firstarray", false, TYPE_ARRAY_FIRSTKEY);

			node->AddParam(arrayNode);

			context.PushASMCNode(node);
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		if (!m_stack) {
			ctx.m_bcl++;
		}
		return 0;
	}
};

class OPCodeInfoSetNextArrayKeyCached : public OPCodeInfo {
public:
	OPCodeInfoSetNextArrayKeyCached(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			ASMContextNode* arrayNode = context.PopASMCNode();
			ASMContextNode* thingNode = context.PopASMCNode();

			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("nextarray", false, TYPE_ARRAY_NEXTKEY);

			node->AddParam(arrayNode);
			node->AddParam(thingNode);
			node->AddParam(new ASMContextNodeIdentifier(name));

			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalFieldObjectFromRef : public OPCodeInfo {
public:
	OPCodeInfoEvalFieldObjectFromRef(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableRefCached : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalVariableRefCached(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.SetFieldIdASMCNode(new ASMContextNodeIdentifier(name));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableDefined : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalVariableDefined(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFunctionOperator("isdefined", nullptr, new ASMContextNodeIdentifier(name), TYPE_FUNC_IS_DEFINED));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalArray : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoEvalArray(OPCode id, LPCCH name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			if (!m_stack) {
				context.SetObjectIdASMCNode(context.GetFieldIdASMCNode());
			}
			ASMContextNode* arrayNode = m_stack ? context.PopASMCNode() : context.GetObjectIdASMCNode();
			ASMContextNode* accessNode = new ASMContextNodeArrayAccess(arrayNode, context.PopASMCNode());
			if (m_stack) {
				context.PushASMCNode(accessNode);
			}
			else {
				context.SetFieldIdASMCNode(accessNode);
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCreateArray : public OPCodeInfo {
public:
	OPCodeInfoCreateArray(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeArrayBuild());
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoAddToArray : public OPCodeInfo {
public:
	OPCodeInfoAddToArray(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNode* key = context.PopASMCNode();
			ASMContextNode* value = context.PopASMCNode();
			ASMContextNode* arrayVal = context.PeekASMCNode();

			if (arrayVal->m_type == TYPE_ARRAY_BUILD) {
				// we are building an array, we can add the value
				static_cast<ASMContextNodeArrayBuild*>(arrayVal)->AddValue(key, value);
			}
			else {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("$addtoarray", false);


				node->AddParam(context.PopASMCNode());
				node->AddParam(key);
				node->AddParam(value);

				context.PushASMCNode(node);
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCreateStruct : public OPCodeInfo {
public:
	OPCodeInfoCreateStruct(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeStructBuild());
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoAddToStruct : public OPCodeInfo {
public:
	OPCodeInfoAddToStruct(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNode* key = context.PopASMCNode();
			ASMContextNode* value = context.PopASMCNode();
			ASMContextNode* structVal = context.PeekASMCNode();

			if (structVal->m_type == TYPE_STRUCT_BUILD) {
				// we are building an array, we can add the value
				static_cast<ASMContextNodeStructBuild*>(structVal)->AddValue(key, value);
			}
			else {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("addtostruct", false);

				node->AddParam(context.PopASMCNode());
				node->AddParam(key);
				node->AddParam(value);

				context.PushASMCNode(node);
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCastFieldObject : public OPCodeInfo {
public:
	OPCodeInfoCastFieldObject(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoPreScriptCall : public OPCodeInfo {
public:
	OPCodeInfoPreScriptCall(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<LPCCH>("<emptypos_prescriptcall>", TYPE_PRECODEPOS));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoEnd : public OPCodeInfo {
public:
	bool m_isReturn;
	OPCodeInfoEnd(OPCode id, LPCCH name, bool isReturn) : OPCodeInfo(id, name), m_isReturn(isReturn) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (m_isReturn) {
				auto* ref = new ASMContextNodeOp1("return ", true, context.PopASMCNode());
				// convert it to statement
				ref->m_priority = PRIORITY_INST;
				ref->m_type = TYPE_RETURN;
				context.PushASMCNode(ref);
			} else {
				// special node to print end ref
				auto* ref = new ASMContextNodeValue<LPCCH>("<END>");
				ref->m_type = TYPE_END;
				context.PushASMCNode(ref);
			}
			context.CompleteStatement();
		}
		out << "\n";
		INT64 rloc = context.FunctionRelativeLocation();
		for (const auto& loc : context.m_locs) {
			if (loc.second.rloc > rloc) {
				// not the end, we can continue
				return 0;
			}
		}

		return -2;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		INT64 rloc = ctx.FunctionRelativeLocation();
		for (const auto& loc : ctx.m_locs) {
			if (loc.second.rloc > rloc) {
				// not the end, we can continue
				return 0;
			}
		}
		return -2;
	}
};

class OPCodeInfoClearArray : public OPCodeInfo {
public:
	OPCodeInfoClearArray(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* fieldId = context.GetFieldIdASMCNode();
			auto* key = context.PopASMCNode();

			ASMContextNode* accessNode = new ASMContextNodeArrayAccess(fieldId, key);
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(accessNode, new ASMContextNodeValue<LPCCH>("undefined"), " = ", PRIORITY_SET, TYPE_SET));

			context.CompleteStatement();
		}
		out << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoStatement : public OPCodeInfo {
	LPCCH m_operatorName;
public:
	OPCodeInfoStatement(OPCode id, LPCCH name, LPCCH operatorName) : OPCodeInfo(id, name),
		m_operatorName(operatorName) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* ref = new ASMContextNodeValue<LPCCH>(m_operatorName);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			ref->m_type = TYPE_STATEMENT;
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoProfileNamed : public OPCodeInfo {
public:
	OPCodeInfoProfileNamed(OPCode id) : OPCodeInfo(id, "ProfileNamedStart") {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<UINT64>();

		UINT64 hash = *(UINT64*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("profileNamedStart", false);

			node->AddParam(new ASMContextNodeHash(hash));
			// convert it to statement
			node->m_priority = PRIORITY_INST;
			node->m_type = TYPE_STATEMENT;
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		out << "#\"" << hashutils::ExtractTmp("hash", hash) << "\" (#" << std::hex << hash << ")" << std::endl;
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT64>() += 8; // name
		return 0;
	}
};

class OPCodeT9EvalFieldVariableFromObject : public OPCodeInfo {
	bool stack;
public:
	OPCodeT9EvalFieldVariableFromObject(OPCode id, LPCCH name, bool stack) : OPCodeInfo(id, name), stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		auto& bytecode = context.Aligned<UINT32>();

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}

		UINT32 field = *(UINT32*)bytecode;
		bytecode += 4;

		out << "." << hashutils::ExtractTmp("var", field) << std::endl;

		if (context.m_runDecompiler) {
			auto* left = new ASMContextNodeIdentifier(name);
			auto* right = new ASMContextNodeIdentifier(field);

			auto* res = new ASMContextNodeLeftRightOperator(left, right, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			if (stack) {
				context.PushASMCNode(res);
			}
			else {
				context.SetFieldIdASMCNode(res);
			}
		}
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		ctx.Aligned<UINT32>() += 4; // field
		return 0;
	}
};

class OPCodeT9EvalArrayCached : public OPCodeInfo {
public:
	OPCodeT9EvalArrayCached(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeArrayAccess(new ASMContextNodeIdentifier(name), context.PopASMCNode()));
		}
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		ctx.Aligned<UINT32>() += 4; // field
		return 0;
	}
};

class OPCodeT9SetFieldVariableFromObjectFromRef : public OPCodeInfo {
public:
	OPCodeT9SetFieldVariableFromObjectFromRef() : OPCodeInfo(OPCODE_T9_SetFieldVariableFromObjectFromRef, "SetFieldVariableFromObjectFromRef") {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		auto& bytecode = context.Aligned<UINT32>();

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}

		UINT32 field = *(UINT32*)bytecode;
		bytecode += 4;

		out << "." << hashutils::ExtractTmp("var", field) << std::endl;

		if (context.m_runDecompiler) {
			auto* left = new ASMContextNodeIdentifier(name);
			auto* right = new ASMContextNodeIdentifier(field);

			auto* objectNode = new ASMContextNodeLeftRightOperator(left, right, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			auto* node = new ASMContextNodeLeftRightOperator(
				new ASMContextNodeLeftRightOperator(objectNode, new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS),
				context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET);
			context.SetFieldIdASMCNode(node->Clone());
			context.PushASMCNode(node);
			context.CompleteStatement();
		}
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		ctx.Aligned<UINT32>() += 4; // field
		return 0;
	}
};

class OPCodeInfoFunctionOperator : public OPCodeInfo {
	LPCCH m_operatorName;
	bool m_hasSelf;
	bool m_complete;
	bool m_canHaveArg2;
public:
	OPCodeInfoFunctionOperator(OPCode id, LPCCH name, LPCCH operatorName, bool hasSelf = false, bool complete = true, bool canHaveArg2 = false) : OPCodeInfo(id, name),
		m_operatorName(operatorName), m_hasSelf(hasSelf), m_complete(complete), m_canHaveArg2(canHaveArg2) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_operatorName, m_hasSelf);
			if (m_hasSelf) {
				node->AddParam(context.PopASMCNode());
			}
			node->AddParam(context.PopASMCNode());

			context.PushASMCNode(node);
			if (m_complete) {
				context.CompleteStatement();
			}
		}
		out << "\n";
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoNotify : public OPCodeInfo {
public:
	OPCodeInfoNotify(OPCode id, LPCCH name) : OPCodeInfo(id, name){
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("notify", true);
			// self
			node->AddParam(context.PopASMCNode());
			// param
			node->AddParam(context.PopASMCNode());

			//node->AddParam();

			if (context.m_stack.size()) {
				if (context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
				else {
					node->AddParam(context.PopASMCNode());
					if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
						// clear PreScriptCall
						delete context.PopASMCNode();
					}
				}
			}

			context.PushASMCNode(node);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoFuncCall : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE params = *(context.m_bcl++);
		auto& bytecode = context.Aligned<UINT64>();

		UINT32* data = (UINT32*)bytecode;

		bytecode += 8;

		out << std::dec << "params: " << (int)params << " ";

		UINT32 nsp = 0;
		if (data[1]
			&& data[1] != 0x222276a9 && data[1] != 0xc1243180 // "sys" or ""
			&& data[1] != context.m_namespace) { // same namespace call
			nsp = data[1];
			out << hashutils::ExtractTmp("namespace", nsp) << "::" << std::flush;
		}

		out << hashutils::ExtractTmp("function", data[0]) << std::endl;

		if (context.m_runDecompiler) {
			BYTE flags = 0;

			switch (m_id) {
			case OPCODE_ScriptThreadCall:
				flags |= THREAD_CALL;
				break;
			case OPCODE_ScriptMethodThreadCall:
				flags |= THREAD_CALL | SELF_CALL;
				break;
			case OPCODE_ScriptThreadCallEndOn:
				flags |= CHILDTHREAD_CALL;
				break;
			case OPCODE_ScriptMethodThreadCallEndOn:
				flags |= CHILDTHREAD_CALL | SELF_CALL;
				break;
			case OPCODE_ScriptMethodCall:
				flags |= SELF_CALL;
				break;
			case OPCODE_CallBuiltinMethod:
				flags |= SELF_CALL;
				break;
			case OPCODE_ScriptFunctionCall:
			case OPCODE_CallBuiltinFunction:
				break; // ignored
			default:
				std::cerr << "bad func call: " << m_id << "\n";
				return -1;
			}

			// classes-calls are by definition without self

			auto* ptr = new ASMContextNodeCallFuncPtr(FUNCTION_CALL, flags);

			// this

			if (flags & SELF_CALL) {
				ptr->AddParam(context.PopASMCNode());
			}

			ptr->AddParam(new ASMContextNodeFuncRef("", data[0], nsp));

			for (size_t i = 0; i < params; i++) {
				ptr->AddParam(context.PopASMCNode());
			}

			if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
				// clear PreScriptCall
				delete context.PopASMCNode();
			}

			context.PushASMCNode(ptr);

			if (context.m_fieldId) {
				delete context.m_fieldId;
				context.m_fieldId = nullptr;
			}
			if (context.m_objectId) {
				delete context.m_objectId;
				context.m_objectId = nullptr;
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // params
		ctx.Aligned<UINT64>() += 8; // import
		return 0;
	}
};

class OPCodeInfoFuncGet : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<UINT64>();

		UINT32* data = (UINT32*)bytecode;

		bytecode += 8;

		out << "&";

		UINT32 nsp = 0;

		if (data[1] 
			&& data[1] != 0x222276a9 && data[1] != 0xc1243180 // "sys" or ""
			&& data[1] != context.m_namespace) { // current namespace
			out << hashutils::ExtractTmp("namespace", data[1]) << std::flush << "::";
			nsp = data[1];
		}
		out << hashutils::ExtractTmp("function", data[0]) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFuncRef("&", data[0], nsp));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT64>() += 8; // import
		return 0;
	}
};

class OPCodeInfoFuncClassCall : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE params = *(context.m_bcl++);
		auto& bytecode = context.Aligned<UINT32>();

		UINT32 function = *(UINT32*)bytecode;

		bytecode += 4;

		out << std::dec << "params: " << (int)params << ", function: " << std::hex << hashutils::ExtractTmp("function", function) << std::endl;

		if (context.m_runDecompiler) {
			static UINT32 constructorName = hashutils::Hash32("__constructor");

			auto* caller = context.PopASMCNode();

			BYTE flags = 0;

			if (m_id == OPCODE_ClassFunctionThreadCall) {
				flags |= THREAD_CALL;
			}
			else if (m_id == OPCODE_ClassFunctionThreadCallEndOn) {
				flags |= CHILDTHREAD_CALL;
			}

			if (!flags && caller->m_type == TYPE_NEW && function == constructorName) {
				// calling new constructor
				auto* newNode = static_cast<ASMContextNodeNew*>(caller);

				auto* ptr = new ASMContextNodeCallFuncPtr(PARAMS_CALL, 0);

				for (size_t i = 0; i < params; i++) {
					ptr->AddParam(context.PopASMCNode());
				}

				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
				newNode->m_constructorCall = ptr;
				context.PushASMCNode(newNode);
			}
			else {
				// classes-calls are by definition without self

				auto* ptr = new ASMContextNodeCallFuncPtr(CLASS_CALL, flags);

				// this
				ptr->AddParam(caller);
				ptr->AddParam(new ASMContextNodeIdentifier(function, "function"));
				for (size_t i = 0; i < params; i++) {
					ptr->AddParam(context.PopASMCNode());
				}

				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}

				context.PushASMCNode(ptr);
			}

			if (context.m_fieldId) {
				delete context.m_fieldId;
				context.m_fieldId = nullptr;
			}
			if (context.m_objectId) {
				delete context.m_objectId;
				context.m_objectId = nullptr;
			}
		}
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // params
		ctx.Aligned<UINT32>() += 4; // name
		return 0;
	}
};

class OPCodeInfoFuncCallPtr : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE params = *(context.m_bcl++);

		out << std::dec << "params:" << (int)params << std::endl;


		if (context.m_runDecompiler) {
			BYTE flags = 0;

			switch (m_id) {
			case OPCODE_ScriptThreadCallPointer:
				flags |= THREAD_CALL;
				break;
			case OPCODE_ScriptMethodThreadCallPointer:
				flags |= THREAD_CALL | SELF_CALL;
				break;
			case OPCODE_ScriptThreadCallPointerEndOn:
				flags |= CHILDTHREAD_CALL;
				break;
			case OPCODE_ScriptMethodThreadCallPointerEndOn:
				flags |= CHILDTHREAD_CALL | SELF_CALL;
				break;
			case OPCODE_ScriptMethodCallPointer:
				flags |= SELF_CALL;
				break;
			case OPCODE_ScriptFunctionCallPointer:
				break; // ignored
			default:
				std::cerr << "bad func ptr call: " << m_id << "\n";
				return -1;
			}

			// classes-calls are by definition without self

			auto* ptr = new ASMContextNodeCallFuncPtr(POINTER_CALL, flags);

			// this

			auto* func = context.PopASMCNode();

			if (flags & SELF_CALL) {
				ptr->AddParam(context.PopASMCNode());
			}
			ptr->AddParam(func);
			// use <= to add the function pointer
			for (size_t i = 1; i <= params; i++) {
				ptr->AddParam(context.PopASMCNode());
			}

			if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
				// clear PreScriptCall
				delete context.PopASMCNode();
			}

			context.PushASMCNode(ptr);

			if (context.m_fieldId) {
				delete context.m_fieldId;
				context.m_fieldId = nullptr;
			}
			if (context.m_objectId) {
				delete context.m_objectId;
				context.m_objectId = nullptr;
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoGetString : public OPCodeInfo {
public:
	OPCodeInfoGetString() : OPCodeInfo(OPCODE_GetString, "GetString") {}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<INT32>();

		UINT32 ref = *(UINT32*)base;

		LPCCH str = objctx.GetStringValue(ref);

		base += 4;

		if (str) {
			out << "\"" << str << "\"\n";
			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeString(str));
			}
		}
		else {
			// probably only dev blocks
			out << "bad str stack: 0x" << std::hex << ref << "\n";
			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeString("<unknown string>"));
			}
		}


		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<INT32>() += 4;
		return 0;
	}
};

class OPCodeInfoGetGlobal : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<UINT16>();

		UINT16 ref = *(UINT16*)base;

		UINT32 name = objctx.GetGlobalVarName(ref);

		base += 2;

		if (name) {
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}
		else {
			out << "bad gvar stack: 0x" << std::hex << ref << "\n";
		}

		if (context.m_runDecompiler) {
			if (m_id == OPCODE_GetGlobal) {
				context.PushASMCNode(new ASMContextNodeIdentifier(name));
			}
			else if (m_id == OPCODE_GetGlobalObject) {
				context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT16>() += 2;
		return 0;
	}
};

class OPCodeT9EvalFieldVariableFromGlobalObject : public OPCodeInfo {
public:
	OPCodeT9EvalFieldVariableFromGlobalObject() : OPCodeInfo(OPCODE_T9_EvalFieldVariableFromGlobalObject, "EvalFieldVariableFromGlobalObject") {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<UINT16>();

		UINT16 ref = *(UINT16*)base;

		UINT32 name = objctx.GetGlobalVarName(ref);

		base += 2;

		auto& baseField = context.Aligned<UINT32>();

		UINT32 field = *(UINT32*)baseField;

		base += 4;

		if (name) {
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}
		else {
			out << "bad gvar stack: 0x" << std::hex << ref;
		}

		out << "." << hashutils::ExtractTmp("var", field) << std::endl;

		if (context.m_runDecompiler) {
			auto* left = new ASMContextNodeIdentifier(name);
			auto* right = new ASMContextNodeIdentifier(field);

			context.SetObjectIdASMCNode(left->Clone());
			context.SetFieldIdASMCNode(new ASMContextNodeLeftRightOperator(left, right, ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT16>() += 2;
		ctx.Aligned<UINT32>() += 4;
		return 0;
	}
};

class OPCodeInfoGetLocalVar : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int) *(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			UINT32 name = context.m_localvars[lvar].name;
			out << hashutils::ExtractTmp("var", name) << std::endl;
			context.m_localvars_ref[name]++;
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoSwitch : public OPCodeInfo {
public:
	OPCodeInfoSwitch() : OPCodeInfo(OPCODE_Switch, "Switch") {}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// size:  8+16*cases,
		// align: 4 + 4 + (8 + 8)*cases,
		// logic uint32 deltaSwitchTable;uint32 cases;(uint64 value;uint64 delta)[cases]
		// move to switch table using deltaSwitchTable;pop value;search case using case; if found move using delta

		auto& baseTable = context.Aligned<INT32>();
		INT32 table = *(INT32*)baseTable;
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = context.Aligned<INT32>();

		INT32 cases = (*(INT32*)baseCases) & 0x7FFFFFFF;

		baseCases += 4;

		out << "table: ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) 
			<< context.FunctionRelativeLocation(utils::Aligned<INT64>(baseTable)) << " cases: " << cases << "\n";

		ASMContextNodeSwitchPreCompute* node = nullptr;
		if (context.m_runDecompiler) {
			node = new ASMContextNodeSwitchPreCompute(context.PopASMCNode());
		}

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = context.Aligned<INT64>();

			context.WritePadding(out);

			UINT64 caseValue = *(UINT64*)baseCaseValue;
			baseCaseValue += 8;
			auto& baseCaseDelta = context.Aligned<INT64>();
			INT64 caseDelta = *(INT64*)baseCaseDelta;
			baseCaseDelta += 8;

			auto caseRLoc = context.PushLocation(&baseCaseDelta[caseDelta]).rloc;

			if (c == cases && !caseValue) {
				out << "default";
				if (node) {
					node->m_cases.push_back({ nullptr, caseRLoc });
				}
			}
			else {
				out << "case ";
				if (caseValue >= 0x100000000LL) {
					// assume it's an hash after INT32 max value
					out << "#\"" << hashutils::ExtractTmp("hash", caseValue) << "\"" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(caseValue), caseRLoc });
					}
				}
				else {
					out << std::dec << caseValue;
					if (node) {
						node->m_cases.push_back({new ASMContextNodeValue<INT64>(caseValue), caseRLoc });
					}
				}
			}

			out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1)
				<< caseRLoc << "\n";

			if (c == cases) {
				if (node) {
					node->m_endLocation = context.FunctionRelativeLocation(utils::Aligned<UINT16>(context.m_bcl));
				}

				if (!caseValue) {
					baseCaseDelta += caseDelta;
				}
			}
			else {
				// align to the next opcode
				context.Aligned<UINT16>();
			}
		}

		if (node) {
			if (!node->m_endLocation) {
				// wtf? no cases???
				node->m_endLocation = context.FunctionRelativeLocation(utils::Aligned<UINT16>(context.m_bcl));
			}
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		context.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		auto& baseTable = ctx.Aligned<INT32>();
		INT32 table = *(INT32*)baseTable;
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = ctx.Aligned<INT32>();

		INT32 cases = *(INT32*)baseCases;

		baseCases += 4;

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = ctx.Aligned<INT64>();
			INT64 caseValue = *(INT64*)baseCaseValue;
			baseCaseValue += 8;
			auto& baseCaseDelta = ctx.Aligned<INT64>();
			INT64 caseDelta = *(INT64*)baseCaseDelta;
			baseCaseDelta += 8;

			auto caseRLoc = ctx.PushLocation(&baseCaseDelta[caseDelta]).rloc;

			if (c == cases) {
				if (!caseValue) {
					baseCaseDelta += caseDelta;
				}
			}
			else {
				// align to the next opcode
				ctx.Aligned<UINT16>();
			}
		}

		ctx.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}
};

class OPCodeInfoEndSwitch : public OPCodeInfo {
public:
	OPCodeInfoEndSwitch() : OPCodeInfo(OPCODE_EndSwitch, "EndSwitch") {}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		INT32 jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& baseCount = context.Aligned<INT32>();

		INT32 count = *(INT32*)baseCount;

		baseCount += 4;

		auto& ptrBase = context.Aligned<INT64>();

		ptrBase += 16 * count;

		auto rloc = context.FunctionRelativeLocation();

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeJumpOperator("endswitch", nullptr, rloc, TYPE_JUMP_ENDSWITCH, jumpLocation));
			context.CompleteStatement();
		}

		out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << std::hex << rloc << "\n";

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		auto& baseCount = ctx.Aligned<INT32>();

		INT32 count = *(INT32*)baseCount;

		baseCount += 4;

		auto& ptrBase = ctx.Aligned<INT64>() += 16 * count;
		return 0;
	}
};

class OPCodeInfopushopn : public OPCodeInfo {
public:
	INT m_n;
	LPCCH m_op;
	ASMContextNodePriority m_priority;
	bool m_caller;
	bool m_forceFunc;
	OPCodeInfopushopn(OPCode id, LPCCH name, INT n, LPCCH op, ASMContextNodePriority priority, bool caller = false, bool forceFunc = false) : OPCodeInfo(id, name),
		m_n(n), m_op(op), m_priority(priority), m_caller(caller), m_forceFunc(forceFunc) {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (m_n == 1 && !m_forceFunc) {
				context.PushASMCNode(new ASMContextNodeOp1(m_op, m_caller, context.PopASMCNode()));
			}
			else if (m_n == 2 && !m_caller && !m_forceFunc) {
				// reverse order
				ASMContextNode* op2 = context.PopASMCNode();
				ASMContextNode* op1 = context.PopASMCNode();

				context.PushASMCNode(new ASMContextNodeOp2(m_op, m_priority, op1, op2));
			}
			else {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, m_caller);

				for (size_t i = 0; i < m_n; i++) {
					node->AddParam(context.PopASMCNode());
				}

				context.PushASMCNode(node);
			}
		}
		out << "\n";
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfodec : public OPCodeInfo {
public:
	OPCodeInfodec(OPCode id, LPCCH name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {

			if (context.m_stack.size()) {
				auto* peek = context.PeekASMCNode();
				if (peek->m_type == TYPE_NEW) {
					auto* newNode = static_cast<ASMContextNodeNew*>(peek);
					if (!newNode->m_constructorCallDec) {
						newNode->m_constructorCallDec = true;
						// calling new constructor, we ignore the return result
						goto end;
					}
				}
			}
			context.CompleteStatement();
		}
	end:
		out << "\n";
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCount : public OPCodeInfo {
public:
	LPCCH m_op;
	bool m_pushReturn;
	OPCodeInfoCount(OPCode id, LPCCH name, LPCCH op, bool pushReturn) : OPCodeInfo(id, name), m_op(op), m_pushReturn(pushReturn) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE count = *(context.m_bcl++);

		out << "count:" << (int)count << "\n";

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, true);

			// self
			node->AddParam(context.PopASMCNode());

			for (size_t i = 0; i < count; i++) {
				node->AddParam(context.PopASMCNode());
			}

			context.PushASMCNode(node);

			if (!m_pushReturn) {
				context.CompleteStatement();
			}
		}

		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfonop : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;
};

// T8-Compiler custom opcode
class OPCodeInfoT8CGetLazyFunction : public OPCodeInfo {
public:
	OPCodeInfoT8CGetLazyFunction() : OPCodeInfo(OPCODE_T8C_GetLazyFunction, "T8C_GetLazyFunction") {}

	int Dump(std::ostream& out, UINT16 v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<INT32>();

		UINT32 nsp = *(UINT32*)base;
		UINT32 function = *(UINT32*)(base + 4);
		UINT64 script = *(UINT64*)(base + 8);

		base += 16;

		out << "@" << hashutils::ExtractTmp("namespace", nsp)
			<< "<" << std::flush << hashutils::ExtractTmpScript(script)
			<< ">::" << std::flush << hashutils::ExtractTmp("function", function) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFuncRef("@", function, nsp, script));
		}
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.Aligned<UINT32>() += 16; // nsp32:func32:script64
		return 0;
	}
};

class OPCodeInfoDeltaVal : public OPCodeInfo {
public:
	LPCCH m_op;
	OPCodeInfoDeltaVal(OPCode id, LPCCH name, LPCCH op) : OPCodeInfo(id, name), m_op(op) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << m_op << "\n";

		if (context.m_runDecompiler) {
			if (context.m_fieldId) {
				context.PushASMCNode(new ASMContextNodeOp1(m_op, false, context.GetFieldIdASMCNode(), TYPE_DELTA));
			}
			else {
				context.PushASMCNode(new ASMContextNodeOp1(m_op, false, context.PopASMCNode(), TYPE_DELTA));
			}
			context.CompleteStatement();
		}

		return 0;
	}
};

class OPCodeT9DeltaLocalVariableCached : public OPCodeInfo {
public:
	LPCCH m_op;
	OPCodeT9DeltaLocalVariableCached(OPCode id, LPCCH name, LPCCH op) : OPCodeInfo(id, name), m_op(op) {
	}

	int Dump(std::ostream& out, UINT16 value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}

		out << m_op << "\n";

		if (context.m_runDecompiler) {
			auto* op = new ASMContextNodeIdentifier(name);
			context.PushASMCNode(new ASMContextNodeOp1(m_op, false, op, TYPE_DELTA));
			context.CompleteStatement();
		}
		return 0;
	}

	int Skip(UINT16 value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

#pragma endregion
#pragma region opcode_registry

static const OPCodeInfo* g_unknownOpcode = new OPCodeInfounknown(OPCODE_Undefined, "Undefined");

void tool::gsc::opcode::RegisterVM(BYTE vm, LPCCH name) {
	g_opcodeMap[vm] = { vm, name, {} };
}
void tool::gsc::opcode::RegisterOpCode(BYTE vm, Platform platform, OPCode enumValue, UINT16 op) {
	auto ref = g_opcodeMap.find(vm);
	if (ref == g_opcodeMap.end()) {
		assert(0);
		std::cerr << "Registering unknown OPCODE vm 0x" << std::hex << (int)vm << "\n";
		return;
	}

	auto& opnfo = ref->second;

	opnfo.opcodemap[op][platform] = enumValue;
	opnfo.opcodemaplookup[enumValue][platform] = op;
}

void tool::gsc::opcode::RegisterOpCodes() {
	static std::once_flag f{};

	std::call_once(f, [] {
		// unknown
		RegisterOpCodeHandler(g_unknownOpcode);

		// nop operator
		// we don't care about the delete, it'll will stay until the end of the process,
		// todo: convert to object with destructor
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown0, "Unknown0", "operator_Unknown0()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown1, "Unknown1", "operator_Unknown1()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown2, "Unknown2", "operator_Unknown2()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown3, "Unknown3", "operator_Unknown3()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown4, "Unknown4", "operator_Unknown4()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown5, "Unknown5", "operator_Unknown5()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown6, "Unknown6", "operator_Unknown6()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown7, "Unknown7", "operator_Unknown7()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknowna, "Unknowna", "operator_Unknowna()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknownb, "Unknownb", "operator_Unknownb()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Nop, "Nop", "Nop()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown38, "Unknown38", "operator_Unknown38()"));

		// all op without params
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ProfileStart, "ProfileStart", "profilestart()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ProfileStop, "ProfileStop", "profilestop()"));
		RegisterOpCodeHandler(new OPCodeInfoProfileNamed(OPCODE_ProfileNamedStart));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ProfileNamedEnd, "ProfileNamedEnd", "profileNamedStop()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown10e, "Unknown10e", "operator_Unknown10e()"));
		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown126, "Unknown126", "operator_Unknown126()"));

		// end
		RegisterOpCodeHandler(new OPCodeInfoEnd(OPCODE_End, "End", false));
		RegisterOpCodeHandler(new OPCodeInfoEnd(OPCODE_Return, "Return", true));

		// switch
		RegisterOpCodeHandler(new OPCodeInfoSwitch());
		RegisterOpCodeHandler(new OPCodeInfoEndSwitch());
		// dev/jump
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_DevblockBegin, "DevblockBegin"));
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_Jump, "Jump"));
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnTrue, "JumpOnTrue"));
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnGreaterThan, "JumpOnGreaterThan"));
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnFalse, "JumpOnFalse"));
		RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnLessThan, "JumpOnLessThan"));
		RegisterOpCodeHandler(new OPCodeInfoJumpExpr(OPCODE_JumpOnFalseExpr, "JumpOnFalseExpr"));
		RegisterOpCodeHandler(new OPCodeInfoJumpExpr(OPCODE_JumpOnTrueExpr, "JumpOnTrueExpr"));

		// ukn jump
		RegisterOpCodeHandler(new OPCodeInfoJumpPush());


		RegisterOpCodeHandler(new OPCodeInfoEvalArray(OPCODE_EvalArrayRef, "EvalArrayRef", false));
		RegisterOpCodeHandler(new OPCodeInfoEvalArray(OPCODE_EvalArray, "EvalArray", true));
		RegisterOpCodeHandler(new OPCodeInfoCreateArray(OPCODE_CreateArray, "CreateArray"));
		RegisterOpCodeHandler(new OPCodeInfoAddToArray(OPCODE_AddToArray, "AddToArray"));
		RegisterOpCodeHandler(new OPCodeInfoCreateStruct(OPCODE_CreateStruct, "CreateStruct"));
		RegisterOpCodeHandler(new OPCodeInfoAddToStruct(OPCODE_AddToStruct, "AddToStruct"));
		RegisterOpCodeHandler(new OPCodeInfoCastFieldObject(OPCODE_CastFieldObject, "CastFieldObject"));
		RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_Unknown35, "Unknown35"));
		RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_Unknown9e, "Unknown9e"));

		// ref
		RegisterOpCodeHandler(new OPCodeInfoSetVariableField(OPCODE_SetVariableField, "SetVariableField"));
		RegisterOpCodeHandler(new OPCodeInfoSetVariableFieldRef(OPCODE_SetVariableFieldRef, "SetVariableFieldRef"));
		RegisterOpCodeHandler(new OPCodeInfoClearFieldVariable(OPCODE_ClearFieldVariable, "ClearFieldVariable", false));
		RegisterOpCodeHandler(new OPCodeInfoClearFieldVariable(OPCODE_ClearFieldVariableOnStack, "ClearFieldVariableOnStack", true));
		RegisterOpCodeHandler(new OPCodeInfoEvalSelfFieldVariable(OPCODE_EvalSelfFieldVariable, "EvalSelfFieldVariable"));
		RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable());
		RegisterOpCodeHandler(new OPCodeInfoCastAndEvalFieldVariable(OPCODE_CastAndEvalFieldVariable, "CastAndEvalFieldVariable"));
		RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariable, "EvalFieldVariable", false, false));
		RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableRef, "EvalFieldVariableRef", false, true));
		RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableOnStack, "EvalFieldVariableOnStack", true, false));
		RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableOnStackRef, "EvalFieldVariableOnStackRef", true, true));

		// localvar related
		RegisterOpCodeHandler(new OPCodeInfoCheckClearParams());
		RegisterOpCodeHandler(new OPCodeInfoSafeCreateLocalVariables());
		RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_EvalLocalVariableCached, "EvalLocalVariableCached"));
		
		RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_EvalLocalVariableCachedSafe, "EvalLocalVariableCachedSafe"));
		RegisterOpCodeHandler(new OPCodeInfoSetLocalVariableCached(OPCODE_SetLocalVariableCachedOnStack, "SetLocalVariableCachedOnStack", true));
		RegisterOpCodeHandler(new OPCodeInfoSetLocalVariableCached(OPCODE_SetLocalVariableCached, "SetLocalVariableCached", false));
		RegisterOpCodeHandler(new OPCodeInfoFirstArrayKey(OPCODE_FirstArrayKey, "FirstArrayKey", true));
		RegisterOpCodeHandler(new OPCodeInfoFirstArrayKey(OPCODE_FirstArrayKeyCached, "FirstArrayKeyCached", false));
		RegisterOpCodeHandler(new OPCodeInfoSetNextArrayKeyCached(OPCODE_SetNextArrayKeyCached, "SetNextArrayKeyCached"));
		RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableRefCached(OPCODE_EvalLocalVariableRefCached, "EvalLocalVariableRefCached"));
		RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableDefined(OPCODE_EvalLocalVariableDefined, "EvalLocalVariableDefined"));
		RegisterOpCodeHandler(new OPCodeInfoEvalFieldObjectFromRef(OPCODE_EvalFieldObjectFromRef, "EvalFieldObjectFromRef"));
		  
		// idc
		RegisterOpCodeHandler(new OPCodeInfoName(OPCODE_EvalLocalVariableCachedDebug, "EvalLocalVariableCachedDebug", "var"));
		RegisterOpCodeHandler(new OPCodeInfoName(OPCODE_EvalLocalVariableRefCachedDebug, "EvalLocalVariableRefCachedDebug", "var"));
		RegisterOpCodeHandler(new OPCodeInfoGetLocalVar(OPCODE_Unknownc7, "Unknownc7"));

		// ref op
		RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_Dec, "Dec", "--"));
		RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_Inc, "Inc", "++"));

		// control
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_EndOnCallback, "EndOnCallback", "endoncallback", false)); // count = params + self
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_EndOn, "EndOn", "endon", false)); // count = params + self
		// ret control
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_WaitTill, "WaitTill", "waittill", true)); // count = params + self
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_WaitTillMatchTimeout, "WaitTillMatchTimeout", "waittillmatchtimeout", true));
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_WaitTillMatch, "WaitTillMatch", "waittillmatch", true)); // count = params + self
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_WaittillTimeout, "WaittillTimeout", "waittilltimeout", true)); // count = params + self

		// operation
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_And, "Bit_And", 2, "&", PRIORITY_BIT_AND));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_Or, "Bit_Or", 2, "|", PRIORITY_BIT_OR));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_Xor, "Bit_Xor", 2, "^", PRIORITY_BIT_XOR));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_GreaterThan, "GreaterThan", 2, ">", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_LessThan, "LessThan", 2, "<", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_GreaterThanOrEqualTo, "GreaterThanOrEqualTo", 2, ">=", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_LessThanOrEqualTo, "LessThanOrEqualTo", 2, "<=", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_ShiftRight, "ShiftRight", 2, ">>", PRIORITY_BIT_SHIFTS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_ShiftLeft, "ShiftLeft", 2, "<<", PRIORITY_BIT_SHIFTS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Plus, "Plus", 2, "+", PRIORITY_PLUS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Minus, "Minus", 2, "-", PRIORITY_PLUS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Divide, "Divide", 2, "/", PRIORITY_MULT));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Modulus, "Modulus", 2, "%", PRIORITY_MULT));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Multiply, "Multiply", 2, "*", PRIORITY_MULT));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Equal, "Equal", 2, "==", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_SuperEqual, "SuperEqual", 2, "===", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_NotEqual, "NotEqual", 2, "!=", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_SuperNotEqual, "SuperNotEqual", 2, "!==", PRIORITY_EQUALS));

		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_BoolComplement, "BoolComplement", 1, "~", PRIORITY_UNARY, true));
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_BoolNot, "BoolNot", 1, "!", PRIORITY_UNARY, true));
		RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_CastBool, "CastBool")); // we ignore this one
		RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_CastCanon, "CastCanon", 1, "", PRIORITY_UNARY, false, true)); // context dependent, the "" can be replaced to tag them

		// Remove return value from operator
		RegisterOpCodeHandler(new OPCodeInfodec(OPCODE_DecTop, "DecTop"));
		RegisterOpCodeHandler(new OPCodeInfodec(OPCODE_SafeDecTop, "SafeDecTop"));

		// gets
		RegisterOpCodeHandler(new OPCodeInfoGetConstant<LPCCH>(OPCODE_GetUndefined, "GetUndefined", "undefined"));
		RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_GetSelf, "GetSelf", "self"));
		RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_GetTime, "GetTime", "gettime()"));
		RegisterOpCodeHandler(new OPCodeInfoGetHash());
		RegisterOpCodeHandler(new OPCodeInfoGetConstant<INT32>(OPCODE_GetZero, "GetZero", 0));
		RegisterOpCodeHandler(new OPCodeInfoGetNeg<UINT32>(OPCODE_GetNegUnsignedInteger, "GetNegUnsignedInteger"));
		RegisterOpCodeHandler(new OPCodeInfoGetNeg<UINT16>(OPCODE_GetNegUnsignedShort, "GetNegUnsignedShort"));
		RegisterOpCodeHandler(new OPCodeInfoGetNeg<UINT8>(OPCODE_GetNegByte, "GetNegByte"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<BYTE>(OPCODE_GetByte, "GetByte"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<INT32>(OPCODE_GetInteger, "GetInteger"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<INT64>(OPCODE_GetLongInteger, "GetLongInteger"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<UINT32>(OPCODE_GetUnsignedInteger, "GetUnsignedInteger"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<UINT16>(OPCODE_GetUnsignedShort, "GetUnsignedShort"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<FLOAT, FLOAT>(OPCODE_GetFloat, "GetFloat"));
		RegisterOpCodeHandler(new OPCodeInfoGetNumber<uintptr_t, uintptr_t>(OPCODE_GetUIntPtr, "GetUIntPtr"));
		RegisterOpCodeHandler(new OPCodeInfoVector());
		RegisterOpCodeHandler(new OPCodeInfoVectorConstant());
		RegisterOpCodeHandler(new OPCodeInfoGetVector());

		RegisterOpCodeHandler(new OPCodeInfoVectorScale(OPCODE_VectorScale, "VectorScale"));
		RegisterOpCodeHandler(new OPCodeInfoGetObjectSize(OPCODE_SizeOf, "SizeOf"));

		RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd", "waittillframeend()"));
		RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_Wait, "Wait", "wait"));
		RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_WaitFrame, "WaitFrame", "waitframe"));
		RegisterOpCodeHandler(new OPCodeInfoNotify(OPCODE_Notify, "Notify"));
		RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_IsDefined, "IsDefined", "isdefined", false, false));

		// PRECODEPOS/CODEPOS on stack
		RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_ClearParams, "ClearParams"));
		RegisterOpCodeHandler(new OPCodeInfoPreScriptCall(OPCODE_PreScriptCall, "PreScriptCall"));

		RegisterOpCodeHandler(new OPCodeInfoGetConstant<LPCCH>(OPCODE_EmptyArray, "EmptyArray", "[]"));
		RegisterOpCodeHandler(new OPCodeInfoClearArray(OPCODE_ClearArray, "ClearArray"));
		RegisterOpCodeHandler(new OPCodeInfoGetConstantRef(OPCODE_GetSelfObject, "GetSelfObject", "self"));

		// class stuff
		RegisterOpCodeHandler(new OPCodeInfoGetObjectType(OPCODE_GetObjectType, "GetObjectType", "class"));
		RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionCall, "ClassFunctionCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionThreadCall, "ClassFunctionThreadCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionThreadCallEndOn, "ClassFunctionThreadCallEndOn"));

		// functions
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptFunctionCall, "ScriptFunctionCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCallEndOn, "ScriptThreadCallEndOn"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCall, "ScriptThreadCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCallEndOn, "ScriptMethodThreadCallEndOn"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodCall, "ScriptMethodCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCall, "ScriptMethodThreadCall"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinFunction, "CallBuiltinFunction"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinMethod, "CallBuiltinMethod"));

		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointerEndOn, "ScriptThreadCallPointerEndOn"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointer, "ScriptThreadCallPointer"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointer, "ScriptMethodThreadCallPointer"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointerEndOn, "ScriptMethodThreadCallPointerEndOn"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptFunctionCallPointer, "ScriptFunctionCallPointer"));
		RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodCallPointer, "ScriptMethodCallPointer"));

		// linked ref
		RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetResolveFunction, "GetResolveFunction"));
		RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetFunction, "GetFunction"));
		RegisterOpCodeHandler(new OPCodeInfoGetString());
		RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobal, "GetGlobal"));
		RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobalObject, "GetGlobalObject"));

		// T9
		RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectFromRef, "EvalFieldVariableFromObjectFromRef", true));
		RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectCached, "EvalFieldVariableFromObjectCached", false));
		RegisterOpCodeHandler(new OPCodeT9SetFieldVariableFromObjectFromRef());
		RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromGlobalObject());
		RegisterOpCodeHandler(new OPCodeT9SetVariableFieldFromEvalArrayRef()); 
		RegisterOpCodeHandler(new OPCodeT9EvalArrayCached(OPCODE_T9_EvalArrayCached, "EvalArrayCached"));
		RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_T9_EndOnCallbackParam, "EndOnCallbackParam", "endoncallbackparam", false)); // count = params + self
		RegisterOpCodeHandler(new OPCodeT9GetVarRef());
		RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorKey, "IteratorKey", "iteratorkey", TYPE_ARRAY_NEXTKEY_IT));
		RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorVal, "IteratorVal", "iteratorval", TYPE_ARRAY_NEXTVAL_IT));
		RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorNext, "IteratorNext", "iteratornext", TYPE_ARRAY_NEXT_IT));
		
		RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_IncLocalVariableCached, "IncLocalVariableCached", "++"));
		RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_DecLocalVariableCached, "DecLocalVariableCached", "--"));
		
		RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_T9_EvalLocalVariableCachedDouble, "EvalLocalVariableCached2n", 2));
		
		// T8compiler custom opcode
		RegisterOpCodeHandler(new OPCodeInfoT8CGetLazyFunction());

		RegisterOpCodesMap();
	});
}

void tool::gsc::opcode::RegisterOpCodeHandler(const OPCodeInfo* info) {
	g_opcodeHandlerMap[info->m_id] = info;
}

bool tool::gsc::opcode::IsValidVm(BYTE vm, VmInfo*& info) {
	// build map
	RegisterOpCodes();
	auto ref = g_opcodeMap.find(vm);
	if (ref == g_opcodeMap.end()) {
		return false;
	}
	info = &ref->second;

	return true;
}

const OPCodeInfo* tool::gsc::opcode::LookupOpCode(BYTE vm, Platform platform, UINT16 opcode) {
	// build map
	RegisterOpCodes();

	VmInfo* info;

	if (!(IsValidVm(vm, info))) {
		return g_unknownOpcode;
	}

	auto ref = info->opcodemap.find(opcode & 0xFFF);

	if (ref == info->opcodemap.end()) {
		return g_unknownOpcode;
	}

	auto ref2 = ref->second.find(platform);

	if (ref2 == ref->second.end()) {
		return g_unknownOpcode;
	}

	const auto refHandler = g_opcodeHandlerMap.find(ref2->second);

	if (refHandler == g_opcodeHandlerMap.end()) {
		return g_unknownOpcode;
	}

	return refHandler->second;
}
std::pair<bool, UINT16> tool::gsc::opcode::GetOpCodeId(BYTE vm, Platform platform, OPCode opcode) {
	RegisterOpCodes();

	VmInfo* info;

	if (!(IsValidVm(vm, info))) {
		return std::make_pair(false, 0);
	}


	auto ref = info->opcodemaplookup.find(opcode);

	if (ref == info->opcodemaplookup.end()) {
		return std::make_pair(false, 0);
	}

	auto ref2 = ref->second.find(platform);


	if (ref2 == ref->second.end()) {
		return std::make_pair(false, 0);
	}

	return std::make_pair(true, ref2->second);
}

#pragma endregion
#pragma region asmctx 
ASMContext::ASMContext(BYTE* fonctionStart, const GscInfoOption& opt, UINT32 nsp, const T8GSCExport& exp, BYTE vm, Platform platform)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_opt(opt), m_runDecompiler(opt.m_dcomp), 
			m_lastOpCodeBase(-1), m_namespace(nsp), m_funcBlock(BLOCK_DEFAULT), m_exp(exp), m_vm(vm), m_platform(platform) {
	// set start as unhandled
	PushLocation();
}


tool::gsc::opcode::ASMSkipContext::ASMSkipContext(BYTE* fonctionStart, BYTE vm, Platform platform) 
	: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_vm(vm), m_platform(platform) {
	PushLocation();
}

ASMContext::~ASMContext() {
	if (m_fieldId) {
		delete m_fieldId;
	}
	if (m_objectId) {
		delete m_objectId;
	}
	for (auto& ref : m_stack) {
		delete ref;
	}
}

asmcontextlocation::asmcontextlocation() {}
asmcontextlocation::~asmcontextlocation() {
	if (fieldId) {
		delete fieldId;
	}
	if (objectId) {
		delete objectId;
	}
	for (auto& r : m_stack) {
		delete r;
	}
	for (auto& r : m_lateop) {
		delete r;
	}
}

void asmcontextlocation::RemoveRef(INT32 origin) {
	refs.erase(origin);
}

asmcontextlocation& ASMContext::PushLocation(BYTE* location) {
	// push aligned location to avoid missing a location
	auto loc = FunctionRelativeLocation(utils::Aligned<UINT16>(location));
	auto& ref = m_locs[loc];

	// clone the stack/objectid/fieldid for future handle
	if (m_runDecompiler) {
		if (!ref.objectId && m_objectId) {
			ref.objectId = m_objectId->Clone();
		}
		if (!ref.fieldId && m_fieldId) {
			ref.fieldId = m_fieldId->Clone();
		}
		if (!ref.m_stack.size() && !ref.handled) {
			ref.m_stack.reserve(m_stack.size());

			for (const auto& node : m_stack) {
				ref.m_stack.push_back(node->Clone());
			}
		}
	}

	// we write the location for the return value
	ref.rloc = loc;
	return ref;
}

asmcontextlocation& ASMSkipContext::PushLocation(BYTE* location) {
	// push aligned location to avoid missing a location
	auto loc = FunctionRelativeLocation(utils::Aligned<UINT16>(location));
	auto& ref = m_locs[loc];

	// we write the location for the return value
	ref.rloc = loc;
	return ref;
}

bool tool::gsc::opcode::ASMSkipContext::FindNextLocation() {
	INT64 min = 0xFFFFFFFFFF;
	INT32 minloc = 0;
	for (const auto& [location, loc] : m_locs) {
		if (!loc.handled) {
			if (min > loc.rloc) {
				min = loc.rloc;
				minloc = location;
			}
		}
	}
	if (min != 0xFFFFFFFFFF) {
		m_bcl = &m_fonctionStart[minloc];

		return true;
	}

	return false;
}

bool ASMContext::FindNextLocation() {
	INT64 min = 0xFFFFFFFFFF;
	INT32 minloc = 0;
	for (const auto& [location, loc] : m_locs) {
		if (!loc.handled) {
			if (min > loc.rloc) {
				min = loc.rloc;
				minloc = location;
			}
		}
	}
	if (min != 0xFFFFFFFFFF) {
		m_bcl = &m_fonctionStart[minloc];

		// clear old and write archived stack
		for (const auto& node : m_stack) {
			delete node;
		}
		m_stack.clear();
		const auto& loc = m_locs[minloc];
		m_stack.reserve(loc.m_stack.size());
		for (const auto& node : loc.m_stack) {
			m_stack.push_back(node->Clone());
		}
		// clear old and write archived object id
		SetObjectIdASMCNode(loc.objectId ? loc.objectId->Clone() : nullptr);
		SetFieldIdASMCNode(loc.fieldId ? loc.fieldId->Clone() : nullptr);

		return true;
	}

	return false;
}

INT32 ASMContext::FunctionRelativeLocation(BYTE* bytecodeLocation) {
	return (INT32) (reinterpret_cast<uintptr_t>(bytecodeLocation) - reinterpret_cast<uintptr_t>(m_fonctionStart));
}

std::ostream& ASMContext::WritePadding(std::ostream& out) {
	return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << FunctionRelativeLocation() << ": "
		// no opcode write
		<< std::setfill(' ') << std::setw((sizeof(INT16) << 1) + 25 + 2) << " ";
}
UINT ASMContext::FinalSize() const {
	INT max = 0;
	for (auto& [loc, ref] : m_locs) {
		if (max < ref.rloc) {
			max = ref.rloc;
		}
	}
	return (UINT)max + 2; // +1 for the Return/End operator
}

#pragma endregion
#pragma region asm_stack 

int ASMContext::GetLocalVarIdByName(UINT32 name) const {
	for (size_t i = 0; i < m_localvars.size(); i++) {
		if (m_localvars[i].name == name) {
			return (int) i;
		}
	}

	return -1;
}
void ASMContext::PushASMCNode(ASMContextNode* node) {
	assert(node);
	if (node->m_rlocEstimated == -1) {
		node->m_rlocEstimated = m_lastOpCodeBase;
	}
	m_stack.push_back(node);
}

void ASMContext::SetObjectIdASMCNode(ASMContextNode* node) {
	if (m_objectId) {
		delete m_objectId;
	}
	m_objectId = node;
}
void ASMContext::SetFieldIdASMCNode(ASMContextNode* node) {
	if (m_fieldId) {
		delete m_fieldId;
	}
	m_fieldId = node;
}

ASMContextNode* ASMContext::GetObjectIdASMCNode() {
	if (!m_objectId) {
		// create fake value for the decompiler
		m_objectId = new ASMContextNodeValue<LPCCH>("<error objectid>");
	}
	return m_objectId->Clone();
}

ASMContextNode* ASMContext::GetFieldIdASMCNode() {
	if (!m_fieldId) {
		// create fake value for the decompiler
		m_fieldId = new ASMContextNodeValue<LPCCH>("<error fieldid>");
	}
	return m_fieldId->Clone();
}

ASMContextNode* ASMContext::PopASMCNode() {
	if (!m_stack.size()) {
		// create fake value for the decompiler
		PushASMCNode(new ASMContextNodeValue<LPCCH>("<error pop>"));
	}
	ASMContextNode* val = m_stack[m_stack.size() - 1];
	m_stack.pop_back();
	return val;
}

ASMContextNode* ASMContext::PeekASMCNode() {
	if (!m_stack.size()) {
		// create fake value for the decompiler
		PushASMCNode(new ASMContextNodeValue<LPCCH>("<error pop>"));
	}
	return m_stack[m_stack.size() - 1];
}

void ASMContext::CompleteStatement() {
	if (m_stack.size() && m_lastOpCodeBase != -1) { // empty func tests
		if (m_fieldId) {
			delete m_fieldId;
			m_fieldId = nullptr;
		}
		if (m_objectId) {
			delete m_objectId;
			m_objectId = nullptr;
		}
		m_funcBlock.m_statements.push_back({ PopASMCNode(), &m_locs[m_lastOpCodeBase]});
		m_lastOpCodeBase = -1;
		//// clear stack
		//for (auto& n : m_stack) {
		//	delete n;
		//}
		//m_stack.clear();
	}
}

#pragma endregion
#pragma region asm_block

std::ostream& DecompContext::WritePadding(std::ostream& out, bool forceNoRLoc) {
	if (asmctx.m_opt.m_func_rloc) {
		out << "/*";
		if (forceNoRLoc) {
			for (size_t i = 0; i < sizeof(INT32) << 1; i++) {
				out << "*";
			}
		}
		else {
			out << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << rloc;
		}
		out << "*/";
	}
	utils::Padding(out, padding);
	return out;
}

void ASMContextNodeBlock::Dump(std::ostream& out, DecompContext& ctx) const {
	switch (m_blockType) {
	case BLOCK_DEFAULT:
		out << "{\n";
		break;
	case BLOCK_DEV:
		out << "/#\n";
		break;
	default:
		out << "\n";
		break;
	}

	ctx.padding++;
	
	// push rloc
	auto oldRLoc = ctx.rloc;

	// decompiler proto block loop
	bool hide = false;
	for (size_t i = 0; i < m_statements.size(); i++) {
		const auto& ref = m_statements[i];
		ctx.rloc = ref.location->rloc;
		if (ref.location->refs.size() && ref.node->m_renderRefIfAny) {
			// write the label one layer bellow the current block
			ctx.padding--;
			ctx.WritePadding(out, true) << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << ref.location->rloc;
			if (ctx.asmctx.m_opt.m_show_ref_count) {
				out << std::dec << " (stack:";

				auto it = ref.location->refs.begin();
				out << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << *it;
				it++;
				while (it != ref.location->refs.end()) {
					out << ", " << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << *(it);
					it++;
				}

				out << ")";
			}
			out << ":\n";
			ctx.padding++;
		}
		if (ref.node->m_type != TYPE_END) {
			if (ref.node->m_type != TYPE_PRECODEPOS || ctx.asmctx.m_opt.m_show_internal_blocks) {
				ctx.WritePadding(out);
				if (hide && ref.node->m_type != TYPE_PRECODEPOS) {
					out << "<END DETECTED> ";
				}
				ref.node->Dump(out, ctx);

				if (ref.node->m_renderSemicolon) {
					out << ";\n";
				}
				if (ref.node->m_type == TYPE_JUMP) {
					hide = true;
				}
			}
		}
		else {
			hide = true;
			if (i != m_statements.size() - 1) {
				ctx.WritePadding(out);
				// if we're not at the end, it means we are reading a return;
				out << "return;\n";
			}
			else if (ctx.asmctx.m_opt.m_show_internal_blocks) {
				ctx.WritePadding(out) << "<end>;\n";
			}
			else if (m_statements.size() == 1) {
				// empty end block, write a small empty line
				ctx.WritePadding(out) << "\n";
			}
		}
		out << std::flush;
		if (hide && !ctx.asmctx.m_opt.m_show_internal_blocks) {
			// don't write hidden stuff
			break;
		}
	}
	// pop back rloc
	ctx.rloc = oldRLoc;

	ctx.padding--;

	switch (m_blockType) {
	case BLOCK_DEFAULT:
		ctx.WritePadding(out, true) << "}\n";
		break;
	case BLOCK_DEV:
		ctx.WritePadding(out, true) << "#/\n";
		break;
	}
}

void ASMContext::ComputeDefaultParamValue() {
	if (m_funcBlock.m_statements.size() < 2) {
		return; // we need at least 2 elements
	}
	auto it = m_funcBlock.m_statements.begin();

	int headerSize = 0;
	while (it != m_funcBlock.m_statements.end()) {

		// jumpif(isdefined(param_name)) after;
		// param_name = RIGHT_VALUE;
		// after:


		auto& param1 = it->node;
		if (param1->m_type != TYPE_JUMP_ONTRUE) {
			break;
		}
		it++;
		if (it == m_funcBlock.m_statements.end() || it->node->m_type != TYPE_SET) {
			break;
		}

		auto* jump = static_cast<ASMContextNodeJumpOperator*>(param1);
		auto* set = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
		auto end = jump->m_location;


		it++;
		asmcontextlocation* jumpEndLocation = nullptr;
		if (it != m_funcBlock.m_statements.end()) {
			jumpEndLocation = it->location;
			if (jumpEndLocation->rloc < end) {
				break; // jump too far
			}
		}

		if (!jump->m_operand || jump->m_operand->m_type != TYPE_FUNC_IS_DEFINED) {
			break; // not jumpif(isdefined(??))
		}

		auto* isDefinedFunc = static_cast<ASMContextNodeFunctionOperator*>(jump->m_operand);

		if (isDefinedFunc->m_operand->m_type != TYPE_IDENTIFIER || set->m_left->m_type != TYPE_IDENTIFIER || !set->m_right) {
			break; // not isdefined(param_name) or not param_name = ...
		}
		UINT32 name = static_cast<ASMContextNodeIdentifier*>(set->m_left)->m_value;

		if (static_cast<ASMContextNodeIdentifier*>(isDefinedFunc->m_operand)->m_value != name) {
			break; // not the same name value
		}

		int localVar = GetLocalVarIdByName(name);

		// the local variables are reversed and the first is an error check, so - 1 - params
		if (localVar == -1 || localVar < m_localvars.size() - 1 - m_exp.param_count || m_localvars[localVar].defaultValueNode) {
			break; // not a param var or already defined as default
		}

		//std::cout << hashutils::ExtractTmp("function", m_exp.name) << std::flush << "-" << hashutils::ExtractTmp("var", name) << ":" << localVar << "/" << m_localvars.size() << "/" << (int)m_exp.param_count << std::endl;

		// add default value node
		m_localvars[localVar].defaultValueNode = set->m_right->Clone();

		if (jumpEndLocation) {
			// remove one ref
			jumpEndLocation->RemoveRef(jump->m_opLoc);
		}

		headerSize += 2;
	}

	if (!headerSize) {
		return; // nothing to delete
	}

	auto ith = m_funcBlock.m_statements.begin();

	// clear nodes
	for (size_t i = 0; i < headerSize; i++) {
		delete ith->node;
		ith++;
	}

	m_funcBlock.m_statements.erase(m_funcBlock.m_statements.begin(), ith);
}

int ASMContextNodeBlock::ComputeDevBlocks(ASMContext& ctx) {
	auto it = m_statements.begin();
	while (it != m_statements.end()) {
		auto& b = *it;
		it->node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeDevBlocks(ctx);
		}, ctx);
		if (b.node->m_type != TYPE_JUMP_DEVBLOCK) {
			it++;
			continue;
		}

		// we are reading a devblock jump
		auto* jump = static_cast<ASMContextNodeJumpOperator*>(b.node);
		INT64 end = jump->m_location;
		ASMContextNodeBlock* devBlock = new ASMContextNodeBlock(BLOCK_DEV);
		// we replace the block by our node
		b.node = devBlock;

		// we move to the next node
		it++;
		// put all the statements into the dev block
		while (it != m_statements.end() && it->location->rloc < end) {
			devBlock->m_statements.push_back(*it);
			it = m_statements.erase(it);
		}

		if (it != m_statements.end() && it->location->rloc == end) {
			// add end devblock
			devBlock->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_enddev>", TYPE_PRECODEPOS), it->location });
			// remove the dev block jump reference
			it->location->RemoveRef(jump->m_opLoc);
		}

		// compute nested dev blocks
		devBlock->ComputeDevBlocks(ctx);

		// delete the jump operator
		delete jump;
	}

	return 0;
}

int ASMContextNodeBlock::ComputeSwitchBlocks(ASMContext& ctx) {

	auto it = this->m_statements.begin();
	while (it != m_statements.end()) {
		auto& b = *it;
		it->node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeSwitchBlocks(ctx);
		}, ctx);
		if (b.node->m_type != TYPE_SWITCH_PRECOMPUTE) {
			// not a switch to compute, we pass to the next pointer
			it++;
			continue;
		}

		// we are reading a devblock jump
		auto* switchPreCompute = static_cast<ASMContextNodeSwitchPreCompute*>(b.node);
		INT64 end = switchPreCompute->m_endLocation;
		ASMContextNodeSwitchPostCompute* switchBlock = new ASMContextNodeSwitchPostCompute(switchPreCompute->m_node->Clone());
		// replace with the computed switch
		auto& cases = switchPreCompute->m_cases;

		// sort the cases by their locations, we know a default won't be reordered 
		// because it was parsed during the precompute node creation.
		std::sort(cases.begin(), cases.end(), [](const ASMContextSwitchPreComputeCase& e1, const ASMContextSwitchPreComputeCase& e2) {
			return e1.jumpLocation < e2.jumpLocation;
		});
		// first node
		it++;

		// check that the index is logic
		if (cases.size() && cases[cases.size() - 1].jumpLocation > end) {
			std::cerr << "Error when parsing switch, a case is after the end\n";
			assert(0);
			continue;
		}


		if (it == m_statements.end()) {
			std::cerr << "Error when parsing switch, end before start of the cases\n";
			assert(0);
			return -1;
		}

		auto* endLoc = FetchFirstForLocation(end);

		asmcontextlocation* endLocation = nullptr;
		if (endLoc && endLoc->location->rloc == end) {
			endLocation = endLoc->location;
		}

		b.node = switchBlock;

		if (cases.size()) {
			// we have at least one case
			auto cit = cases.begin();

			do {
				ASMContextNodeBlock* block = new ASMContextNodeBlock(BLOCK_PADDING);
				switchBlock->m_cases.push_back({ cit->casenode ? cit->casenode->Clone() : nullptr, block });
				// we pass this case to fetch the end
				cit++;

				INT64 endCase;

				if (cit == cases.end()) {
					endCase = end;
				}
				else {
					endCase = cit->jumpLocation;
				}

				while (it != m_statements.end()) {
					if (it->location->rloc >= endCase) {
						break; // end of the case
					}

					if (it->node->m_type == TYPE_JUMP || it->node->m_type == TYPE_JUMP_ENDSWITCH) {
						auto* jump = static_cast<ASMContextNodeJumpOperator*>(it->node);
						// convert this to a break statement
						if (jump->m_location == end) {
							jump->m_operatorName = "break";
							jump->m_showJump = false;
							jump->m_special_op = SPECIAL_JUMP_BREAK;
							if (endLocation) {
								endLocation->RemoveRef(jump->m_opLoc);
							}
						}
					}

					// we add all the location to this case
					block->m_statements.push_back({ it->node->Clone(), it->location });
					it = m_statements.erase(it);
				}
			} while (cit != cases.end());
		} else if (it->node->m_type == TYPE_JUMP_ENDSWITCH) {
			auto* jump = static_cast<ASMContextNodeJumpOperator*>(it->node);
			// convert this to a break statement
			if (jump->m_location == end) {
				jump->m_operatorName = "break";
				jump->m_special_op = SPECIAL_JUMP_BREAK;
				jump->m_showJump = false;
				if (endLocation) {
					endLocation->RemoveRef(jump->m_opLoc);
				}
			}
			it++;
		}


		switchBlock->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeSwitchBlocks(ctx);
		}, ctx);

		// delete the precompute operator
		delete switchPreCompute;
	}

	return 0;
}

ASMContextNode* JumpCondition(ASMContextNodeJumpOperator* op, bool reversed) {
	switch (op->m_type) {
	case TYPE_JUMP_GREATERTHAN:
	case TYPE_JUMP_LOWERTHAN:
	{
		auto* clo = static_cast<ASMContextNodeOp2*>(op->m_operand->Clone());
		if (reversed) {
			clo->m_description = op->m_type == TYPE_JUMP_GREATERTHAN ? "<" : ">";
		}
		return clo;
	}
	case TYPE_JUMP_ONTRUE:
	case TYPE_JUMP_ONTRUEEXPR:
		if (reversed) {
			return new ASMContextNodeOp1("!", true, op->m_operand->Clone());
		}
		return op->m_operand->Clone();

	case TYPE_JUMP_ONFALSE:
	case TYPE_JUMP_ONFALSEEXPR:
		if (reversed) {
			return op->m_operand->Clone();
		}
		return new ASMContextNodeOp1("!", true, op->m_operand->Clone());
	default:
		return nullptr;
	}
}

int ASMContextNodeBlock::ComputeForEachBlocks(ASMContext& ctx) {
	/*
	// INIT STEP
	// T8
		var_f15d3f7b = doorblockers;
		key = firstarray(var_f15d3f7b);
	// LOOP STEP
	LOC_000000fc:
		LOC_00000100:jumpiffalse(isdefined(key)) LOC_000002f0;
		doorblocker = var_f15d3f7b[key];
		nextarray(var_f15d3f7b, var_7cbeb6de)

		// loop code
		
		jump LOC_000002f0 = break;
		jump LOC_000002e4 = continue;

	// N+1 STEP
	LOC_000002e4:
		key = var_7cbeb6de;
		LOC_000002ec:jump LOC_000000fc;
	// END STEP
	LOC_000002f0:

	var_f15d3f7b is an internal name for the array pointer
	var_7cbeb6de is an internal name to compute the next array element
	key is the key name, it might not be used, idea: counting the ref?

	// T9
	
	var_a970ac8 = var_f04dd3ef;
	var_b4eeb030 = firstarray(var_a970ac8);
	while (isdefined(var_b4eeb030)) {
	    var_23ea8daa = iteratorkey(var_b4eeb030);
	    e_clip = iteratorval(var_b4eeb030);
	    var_bff7a077 = iteratornext(var_b4eeb030);

		// loop code

		// N+1 STEP
	    var_b4eeb030 = var_bff7a077;
	}
	var_a970ac8 is a ref to the array
	var_b4eeb030 is an iterator

	var_23ea8daa is the key, it might not be used, idea: counting the ref?
	e_clip is the value
	*/

	size_t index = 0;

	while (index < m_statements.size()) {
		auto& arrayRefLoc = m_statements[index];
		arrayRefLoc.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeForEachBlocks(ctx);
		}, ctx);
		const auto startIndex = index;
		int moveDelta = 1;

		/*
			var_f15d3f7b = doorblockers;
			key = firstarray(var_f15d3f7b);
		*/


		// matching "var_f15d3f7b = doorblockers", var_f15d3f7b = array ref;
		if (arrayRefLoc.node->m_type != TYPE_SET) {
			index += moveDelta;
			continue;
		}

		auto* setOp = static_cast<ASMContextNodeLeftRightOperator*>(arrayRefLoc.node);

		if (setOp->m_left->m_type != TYPE_IDENTIFIER) {
			index += moveDelta;
			continue;
		}


		// array ref name
		UINT32 arrayRefName = static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value;

		auto arrayRefIndex = index;

		index++;
		moveDelta--;

		if (index >= m_statements.size()) {
			index += moveDelta;
			continue;
		}
		auto& firstArrayLoc = m_statements[index];
		if (firstArrayLoc.node->m_type != TYPE_SET) {
			index += moveDelta;
			continue;
		}
		auto* setFirstArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(firstArrayLoc.node);

		if (setFirstArrayOp->m_left->m_type != TYPE_IDENTIFIER || setFirstArrayOp->m_right->m_type != TYPE_ARRAY_FIRSTKEY) {
			index += moveDelta;
			continue; // not key = firstarray(...)
		}

		// For T9 it is the iterator name
		UINT32 keyValName = static_cast<ASMContextNodeIdentifier*>(setFirstArrayOp->m_left)->m_value;

		/*
		LOC_000000fc:
			LOC_00000100:jumpiffalse(isdefined(key)) LOC_000002f0;
			doorblocker = var_f15d3f7b[key];
			nextarray(var_f15d3f7b, var_7cbeb6de)
		*/

		index++;
		moveDelta--;

		size_t forincsize = ctx.m_vm == VM_T9 ? 4 : 3;

		if (index + forincsize >= m_statements.size()) {
			index += moveDelta;
			continue;
		}
		auto& condJump = m_statements[index];

		if (condJump.node->m_type != TYPE_JUMP_ONFALSE) {
			index += moveDelta;
			continue;
		}

		index++;
		moveDelta--;

		auto* jumpOp = static_cast<ASMContextNodeJumpOperator*>(condJump.node);

		auto& itemSetStmt = m_statements[index];

		if (itemSetStmt.node->m_type != TYPE_SET) {
			index += moveDelta;
			continue;
		}
		index++;
		moveDelta--;

		// keyValName
		UINT32 itemValName;
		if (ctx.m_vm == VM_T9) {
			/*
				var_d9f19f82 = iteratorkey(var_e4aec0cf);
				var_8487602c = iteratorval(var_e4aec0cf);
				var_60ff8ae8 = iteratornext(var_e4aec0cf);
			*/
			auto& iteratorvalStmt = m_statements[index];

			if (iteratorvalStmt.node->m_type != TYPE_SET) {
				index += moveDelta;
				continue;
			}
			index++;
			moveDelta--;

			auto& iteratornextStmt = m_statements[index];

			if (iteratornextStmt.node->m_type != TYPE_SET) {
				index += moveDelta;
				continue;
			}

			auto* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);
			auto* setValArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(iteratorvalStmt.node);
			auto* setNextArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(iteratornextStmt.node);

			if (setKeyArrayOp->m_left->m_type != TYPE_IDENTIFIER 
				|| setValArrayOp->m_left->m_type != TYPE_IDENTIFIER 
				|| setNextArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
				index += moveDelta;
				continue; // not our 3 sets
			}

			keyValName = static_cast<ASMContextNodeIdentifier*>(setKeyArrayOp->m_left)->m_value;
			itemValName = static_cast<ASMContextNodeIdentifier*>(setValArrayOp->m_left)->m_value;

			index++;
			moveDelta--;
		}
		else {
			/*
				doorblocker = var_f15d3f7b[key];
				nextarray(var_f15d3f7b, var_7cbeb6de)
			*/
			auto& nextArrayStmt = m_statements[index];

			if (nextArrayStmt.node->m_type != TYPE_ARRAY_NEXTKEY) {
				index += moveDelta;
				continue;
			}
			auto* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);

			if (setKeyArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
				index += moveDelta;
				continue; // not itemValName = firstarray(...)
			}

			itemValName = static_cast<ASMContextNodeIdentifier*>(setKeyArrayOp->m_left)->m_value;

			index++;
			moveDelta--;
		}


		/*
		LOC_000002e4:
			key = var_7cbeb6de;
			LOC_000002ec:jump LOC_000000fc;
		*/

		// internal location in the foreach
		INT64 endLoc = jumpOp->m_location; // jump location for the break
		INT64 condLoc = condJump.location->rloc; // jump back location for the end of the loop
		// index of the last node (at least + 2 for the next value set and the jump)
		auto endNodeIndex = index;


		while (endNodeIndex < m_statements.size() && m_statements[endNodeIndex].location->rloc < endLoc) {
			endNodeIndex++;
		}

		if (endNodeIndex > m_statements.size()) {
			std::cerr << "Bad foreach node endNodeIndex\n";
			assert(0);
			index += moveDelta;
			continue;
		}

		auto* endNodeLocation = endNodeIndex == m_statements.size() ? nullptr : m_statements[endNodeIndex].location;
		auto incrementIndex = endNodeIndex - 2;

		auto& setNextKeyStmt = m_statements[incrementIndex];
		auto& jumpBackStmt = m_statements[incrementIndex + 1];

		// at the end ot the loop we can continue, break, return or return <...>
		if (jumpBackStmt.node->m_type != TYPE_END && jumpBackStmt.node->m_type != TYPE_RETURN && (jumpBackStmt.node->m_type != TYPE_JUMP
			|| (static_cast<ASMContextNodeJumpOperator*>(jumpBackStmt.node)->m_location != condLoc
				&& static_cast<ASMContextNodeJumpOperator*>(jumpBackStmt.node)->m_location != endLoc))) {
			// not a good increment (wtf?)
			index += moveDelta;
			continue;
		}
		INT64 incrementLoc;
		if (setNextKeyStmt.node->m_type != TYPE_SET) {
			incrementIndex += 2,
			incrementLoc = jumpBackStmt.location->rloc; // jump location for the continue
		}
		else {
			incrementLoc = setNextKeyStmt.location->rloc; // jump location for the continue
		}
		
		// assume set good


		// we found the foreach, we can now patch the jumps (TODO: find if we need to specify the jump location, i.e.: nested foreach)
		for (size_t i = index; i < incrementIndex; i++) {
			auto& stmt = m_statements[i];

			if (!IsJumpType(stmt.node->m_type)) {
				continue;
			}

			auto* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
			if (j->m_location == endLoc) {
				j->m_operatorName = "break";
				j->m_special_op = SPECIAL_JUMP_BREAK;
				j->m_showJump = false;
				if (endNodeLocation) {
					endNodeLocation->RemoveRef(j->m_opLoc);
				}
			}
			else if (j->m_location == incrementLoc) {
				j->m_operatorName = "continue";
				j->m_showJump = false;
				j->m_special_op = SPECIAL_JUMP_CONTINUE;
				setNextKeyStmt.location->RemoveRef(j->m_opLoc);
			}
		}

		if (endNodeLocation) {
			endNodeLocation->RemoveRef(jumpOp->m_opLoc);
		}

		// replace this whole mess by a cool foreach block keyValName
		auto* block = new ASMContextNodeBlock();

		// remove the number of references for the key because maybe we don't use it
		auto& keyRef = ctx.m_localvars_ref[keyValName];
		if (ctx.m_vm == VM_T9) {
			keyRef = max(keyRef - 1, 0); // key isn't used as an iterator in T9
		}
		else {
			keyRef = max(keyRef - 5, 0);
		}

		auto* forEachNode = new ASMContextNodeForEach(setOp->m_right->Clone(), block, ctx.m_localvars_ref[keyValName] ? keyValName : 0, itemValName);

		auto it = m_statements.begin() + startIndex;
		delete it->node;
		it->node = forEachNode;

		it++;
		for (size_t i = startIndex + 1; i < index; i++) {
			// remove component statement
			it = m_statements.erase(it);
		}
		for (size_t i = index; i < incrementIndex; i++) {
			block->m_statements.push_back({ it->node, it->location });
			it = m_statements.erase(it);
		}
		block->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_foreachcontinue>", TYPE_PRECODEPOS), it->location });
		for (size_t i = incrementIndex; i < endNodeIndex; i++) {
			// remove component statement
			delete it->node;
			it = m_statements.erase(it);
		}

		forEachNode->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeForEachBlocks(ctx);
		}, ctx);
		// go back to the first index
		index = startIndex;
	}

	return 0;
}

int ASMContextNodeBlock::ComputeWhileBlocks(ASMContext& ctx) {
	/*
	while() {}

	LOC_000001d6:
		LOC_00000208:jumpiffalse(self isgrappling() && self getcurrentoffhand() == weapon) LOC_00000256 delta: 0x4a;
		self waittill(#"hash_347a612b61067eb3");
		self bot_action::function_8a2b82ad(actionparams);
		self bot_action::aim_at_target(actionparams);
		LOC_00000252:goto LOC_000001d6 delta: -0x80;
	LOC_00000256:

	a continue is pointing to the negative goto, only one negative goto.

	a break is pointing to after the loop.

	do while() {}

	LOC_00000022:
		waitframe(1);
		LOC_0000004a:jumpiftrue(self getzbarrierpiecestate(5) == "closing") LOC_00000022 delta : -0x2c;

	for (;;) is a negative goto that isn't pointing to a jumpif

	*/
	size_t itindex = 0;
	while (itindex < m_statements.size()) {
		auto& jumpStmtVal = m_statements[itindex];
		//std::cout << "parsing " << hashutils::ExtractTmp("function", ctx.m_exp.name) << ": " << std::hex << jumpStmtVal.location->rloc << "\n";
		jumpStmtVal.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeWhileBlocks(ctx);
		}, ctx);
		if (!IsJumpType(jumpStmtVal.node->m_type)) {
			itindex++;
			continue; // not a jump
		}
		auto* jumpOp = static_cast<ASMContextNodeJumpOperator*>(jumpStmtVal.node);
		if (jumpOp->m_delta >= 0) {
			itindex++;
			continue; // not a back jump
		}

		// we search the last ref to the start, index element can be a continue
		auto index = itindex;
		for (size_t i = itindex + 1; i < m_statements.size(); i++) {
			if (IsJumpType(m_statements[i].node->m_type)) {
				auto* jumpOp2 = static_cast<ASMContextNodeJumpOperator*>(m_statements[i].node);
				if (jumpOp2->m_location == jumpOp->m_location) {
					jumpOp = jumpOp2;
					index = i;
				}
			}
		}
		auto& jumpStmt = m_statements[index];

		if (jumpOp->m_type == TYPE_JUMP) {
			// location for the continue
			INT64 startLoc = jumpOp->m_location;

			size_t startIndex = 0;
			while (startIndex < index && m_statements[startIndex].location->rloc < startLoc) {
				startIndex++;
			}

			if (startIndex == index || m_statements[startIndex].location->rloc != startLoc) {
				itindex++; // not in the same block
				continue;
			}

			// start of the loop location
			auto* continueLocation = jumpStmt.location;
			// start of the loop location
			INT32 continueLoc = continueLocation->rloc;

			// end location for the break, can be an infinite loop, so we consider it
			asmcontextlocation* endLocation = nullptr;
			INT64 breakLoc = -0xFFFF; // random value
			if (index + 1 < m_statements.size()) {
				endLocation = m_statements[index + 1].location;
				breakLoc = endLocation->rloc;
			}

			const auto& firstNode = m_statements[startIndex];
			auto* firstNodeLocation = firstNode.location;
			ASMContextNode* cond = nullptr;

			auto it = m_statements.begin() + startIndex;

			if (IsJumpType(firstNode.node->m_type)) {
				// clone and erase the condition
				// while/for
				auto* jumpCondNode = static_cast<ASMContextNodeJumpOperator*>(firstNode.node);
				if (jumpCondNode->m_location == breakLoc) {
					cond = JumpCondition(jumpCondNode, true);

					if (endLocation) {
						assert(jumpCondNode->m_location == endLocation->rloc);
						endLocation->RemoveRef(jumpCondNode->m_opLoc);
					}

					delete firstNode.node;
					startIndex++;

					it = m_statements.erase(it);
				}
				else {
					startIndex++;
				}
			}

			auto* block = new ASMContextNodeBlock();
			auto* node = new ASMContextNodeWhile(cond, block);
			for (size_t i = startIndex; i < index; i++) {
				auto* ref = it->node;

				ASMContextNode* replaceNode = nullptr;

				if (IsJumpType(ref->m_type)) {
					// replace jumps
					auto* j = static_cast<ASMContextNodeJumpOperator*>(ref);
					if (j->m_location == breakLoc) {
						j->m_operatorName = "break";
						j->m_special_op = SPECIAL_JUMP_BREAK;
						j->m_showJump = false;
						assert(endLocation); // it would mean breakLoc was set
						endLocation->RemoveRef(j->m_opLoc);
					}
					else if (j->m_location == continueLoc) {
						j->m_operatorName = "continue";
						j->m_special_op = SPECIAL_JUMP_CONTINUE;
						j->m_showJump = false;
						continueLocation->RemoveRef(j->m_opLoc);
					}
					else if (j->m_location == jumpOp->m_location) {
						j->m_operatorName = "continue";
						j->m_special_op = SPECIAL_JUMP_CONTINUE;
						j->m_showJump = false;
						firstNodeLocation->RemoveRef(j->m_opLoc);
					}
				}
				else if (ref->m_type == TYPE_SWITCH_POSTCOMPUTE || ref->m_type == TYPE_BLOCK) {
					ref->ApplySubBlocks([=](ASMContextNodeBlock* block, ASMContext& context) {
						for (const auto& stmt : block->m_statements) {
							auto* refjump = stmt.node;
							if (IsJumpType(refjump->m_type)) {
								// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
								//and do not remove the ref because it's nested breaks/continues
								auto* j = static_cast<ASMContextNodeJumpOperator*>(refjump);
								if (j->m_location == breakLoc) {
									if (!j->m_special_op) {
										j->m_operatorName = "break";
										j->m_special_op = SPECIAL_JUMP_BREAK;
										j->m_showJump = false;
									}
									if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										assert(endLocation); // it would mean breakLoc was set
										endLocation->RemoveRef(j->m_opLoc);
									}
								}
								else if (j->m_location == continueLoc) {
									if (!j->m_special_op) {
										j->m_operatorName = "continue";
										j->m_special_op = SPECIAL_JUMP_CONTINUE;
										j->m_showJump = false;
									}
									if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										continueLocation->RemoveRef(j->m_opLoc);
									}
								}
								else if (j->m_location == jumpOp->m_location) {
									if (!j->m_special_op) {
										j->m_operatorName = "continue";
										j->m_special_op = SPECIAL_JUMP_CONTINUE;
										j->m_showJump = false;
									}
									if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										firstNodeLocation->RemoveRef(j->m_opLoc);
									}
								}
							}
						}
					}, ctx);
				}

				block->m_statements.push_back({ replaceNode ? replaceNode : ref->Clone(), it->location });
				delete it->node;
				it = m_statements.erase(it);
			}
			block->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_inf_end>", TYPE_PRECODEPOS), it->location });
			// it = jump
			firstNodeLocation->RemoveRef(jumpOp->m_opLoc);
			it->location = firstNodeLocation;
			delete it->node;
			it->node = node;
			node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
				block->ComputeWhileBlocks(ctx);
			}, ctx);
			itindex = startIndex;
		}
		else {
			// do {} while(...);
			auto* cond = JumpCondition(jumpOp, false);
			auto* endLocation = m_statements[index + 1].location;
			auto* condLocation = jumpStmt.location;

			// end location for the break
			INT32 breakLoc = endLocation->rloc;
			// loop location for the continue
			INT64 continueLoc = jumpOp->m_location;

			size_t startIndex = 0;
			while (startIndex < index && m_statements[startIndex].location->rloc < continueLoc) {
				startIndex++;
			}

			if (startIndex == index || m_statements[startIndex].location->rloc != continueLoc) {
				itindex++;
				assert(false); // wtf? bad ref
				continue;
			}

			auto* doWhileLoc = m_statements[startIndex].location;

			// remove loop ref
			doWhileLoc->RemoveRef(jumpOp->m_opLoc);

			auto* newBlock = new ASMContextNodeBlock();
			auto* doWhile = new ASMContextNodeDoWhile(cond, newBlock);

			auto it = m_statements.begin() + startIndex;
			for (size_t i = startIndex; i < index; i++) {
				auto& stmt = m_statements[startIndex];

				ASMContextNode* replaceNode = nullptr;

				// compute special jump
				if (IsJumpType(stmt.node->m_type)) {
					auto* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
					if (j->m_location == breakLoc) {
						j->m_operatorName = "break";
						j->m_showJump = false;
						j->m_special_op = SPECIAL_JUMP_BREAK;
						endLocation->RemoveRef(j->m_opLoc);
					} else if (j->m_location == continueLoc) {
						j->m_operatorName = "continue";
						j->m_showJump = false;
						j->m_special_op = SPECIAL_JUMP_CONTINUE;
						condLocation->RemoveRef(j->m_opLoc);
					}
				}
				else if (stmt.node->m_type == TYPE_SWITCH_POSTCOMPUTE || stmt.node->m_type == TYPE_BLOCK) {
					stmt.node->ApplySubBlocks([=](ASMContextNodeBlock* block, ASMContext& context) {
						for (const auto& stmtjump : block->m_statements) {
							auto* refjump = stmtjump.node;
							if (IsJumpType(refjump->m_type)) {
								// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
								//and do not remove the ref because it's nested breaks/continues
								auto* j = static_cast<ASMContextNodeJumpOperator*>(refjump);
								if (j->m_location == breakLoc) {
									if (!j->m_special_op) {
										j->m_operatorName = "break";
										j->m_special_op = SPECIAL_JUMP_BREAK;
										//j->m_showJump = true;
										j->m_showJump = false;
									}
									if (stmt.node->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										endLocation->RemoveRef(j->m_opLoc);
									}
								}
								else if (j->m_location == continueLoc) {
									if (!j->m_special_op) {
										j->m_operatorName = "continue";
										j->m_special_op = SPECIAL_JUMP_CONTINUE;
										//j->m_showJump = true;
										j->m_showJump = false;
									}
									if (stmt.node->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										condLocation->RemoveRef(j->m_opLoc);
									}
								}
							}
						}
						}, ctx);
				}

				newBlock->m_statements.push_back({ replaceNode ? replaceNode : stmt.node->Clone(), stmt.location});
				// clear node
				delete stmt.node;
				it = m_statements.erase(it);
			}
			// clear the jump node
			itindex = startIndex;
			newBlock->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_whileend>", TYPE_PRECODEPOS), m_statements[itindex].location });
			delete m_statements[itindex].node;
			m_statements[itindex].node = doWhile;
			m_statements[itindex].location = doWhileLoc;

			doWhile->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
				block->ComputeWhileBlocks(ctx);
			}, ctx);
		}
	}
	return 0;
}

int ASMContextNodeBlock::ComputeIfBlocks(ASMContext& ctx) {
	/*
	ITEIE:
	    LOC_000001a2:jumpiffalse(state == "loot_case_open") LOC_000001c0;
        var_f56984dc = "crateidle";
        var_61de15c5 = "open";
        LOC_000001bc:goto LOC_00000214;
    LOC_000001c0:
        LOC_000001da:jumpiffalse(level.var_cf24a85f.var_f56984dc === "idle") LOC_000001e8;
        var_f56984dc = "crate";
    LOC_000001e8:
        level.var_cf24a85f.var_63efd7bf = "loot_case";
        var_ac97b37c = #"hash_31c4c4c8b87dc6a4";
    LOC_00000214:
	
	*/
	size_t index = 0;
	while (index < m_statements.size()) {
		auto& jumpStmt = m_statements[index];
		jumpStmt.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeIfBlocks(ctx);
		}, ctx);

		if (!IsJumpType(jumpStmt.node->m_type)) {
			index++;
			continue; // not a jump for an if
		}
		auto* jumpOp = static_cast<ASMContextNodeJumpOperator*>(jumpStmt.node);
		// can't have a negative delta
		assert(jumpOp->m_special_op || jumpOp->m_delta >= 0);
		if (jumpStmt.node->m_type == TYPE_JUMP) {
			index++;
			continue;
		}

		auto elsePart = jumpOp->m_location;

		auto endIndex = index + 1;
		while (endIndex < m_statements.size() && m_statements[endIndex].location->rloc < elsePart) {
			endIndex++;
		}

		if (endIndex == m_statements.size()) {
			index++; // break; continue
			continue;
		}

		auto* elsePartNodeLoc = m_statements[endIndex].location;

		if (elsePartNodeLoc->rloc != elsePart) {
			index++; // bad jump? (ternaries aren't made when I'm writing that)
			continue;
		}

		ASMContextNodeBlock* blockIf = new ASMContextNodeBlock();
		ASMContextNodeBlock* blockElse = nullptr;
		bool ignoreLast = false;

		// remove if ref
		elsePartNodeLoc->RemoveRef(jumpOp->m_opLoc);

		// check +2 to avoid converting a if (...) { continue; } into an empty if (...) {} else {...}
		if (endIndex > index + 2 && m_statements[endIndex - 1].node->m_type == TYPE_JUMP) {
			auto* jumpOpEndElse = static_cast<ASMContextNodeJumpOperator*>(m_statements[endIndex - 1].node);
			// after else goto
			if (jumpOpEndElse->m_delta > 0) {
				auto endElseLoc = jumpOpEndElse->m_location;
				// should be positive

				auto endElseIndex = endIndex;
				while (endElseIndex < m_statements.size() && m_statements[endElseIndex].location->rloc < endElseLoc) {
					endElseIndex++;
				}

				if (endElseIndex < m_statements.size() && m_statements[endElseIndex].location->rloc == endElseLoc) {
					blockElse = new ASMContextNodeBlock();

					// remove ref else
					auto* endElseFinalLoc = m_statements[endElseIndex].location;
					endElseFinalLoc->RemoveRef(jumpOpEndElse->m_opLoc);
					
					ignoreLast = true; // ignore last element of the ifblock

					// else block
					auto it = m_statements.begin() + endIndex;
					for (size_t i = endIndex; i < endElseIndex; i++) {
						// fill the else node
						blockElse->m_statements.push_back({ it->node->Clone(), it->location });

						delete it->node;
						it = m_statements.erase(it);
					}
					blockElse->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_elselast>", TYPE_PRECODEPOS), endElseFinalLoc });
				}
			}
		}

		auto it = m_statements.begin() + (index + 1);
		if (ignoreLast) {
			endIndex--;
		}
		for (size_t i = index + 1; i < endIndex; i++) {
			// fill the if node
			blockIf->m_statements.push_back({ it->node->Clone(), it->location });

			delete it->node;
			it = m_statements.erase(it);
		}
		if (ignoreLast) {
			blockIf->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_ifend1>", TYPE_PRECODEPOS), it->location });
			delete it->node;
			it = m_statements.erase(it);
		}
		else {
			blockIf->m_statements.push_back({ new ASMContextNodeValue<LPCCH>("<emptypos_ifend2>", TYPE_PRECODEPOS), elsePartNodeLoc });
		}

		// swap the jump with the new if statement
		assert(IsJumpType(m_statements[index].node->m_type) && m_statements[index].node->m_type != TYPE_JUMP);
		auto* ifElse = new ASMContextNodeIfElse(JumpCondition(static_cast<ASMContextNodeJumpOperator*>(m_statements[index].node), true), blockIf, blockElse);
		delete m_statements[index].node;
		m_statements[index].node = ifElse;
		ifElse->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeIfBlocks(ctx);
		}, ctx);
	}

	// LOC_0000015c:jumpiffalse(!self function_a39f313c() || self getcurrentweapon() == level.weaponnone) LOC_0000017c delta: 0x1c (type:9/spec:false)

	return 0;
}

int ASMContextNodeBlock::ComputeForBlocks(ASMContext& ctx) {
	/*

	    i = 0;
	    while (i < var_8f5e3947.size) {
	        var_4e2b3e3a[i] = var_8f5e3947[i].origin;
	        i++;
	        <emptypos_inf_end>;
		}
	*/

	size_t index = 0;
	while (index < m_statements.size()) {
		auto& whileStmt = m_statements[index];
		whileStmt.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
			block->ComputeForBlocks(ctx);
		}, ctx);

		if (index == 0 || whileStmt.node->m_type != TYPE_WHILE) {
			index++;
			continue; // at least one statement before and a while
		}

		auto& initValStmt = m_statements[index - 1];
		auto* initValNode = initValStmt.node;

		if (initValNode->m_type != TYPE_SET) {
			index++;
			continue; // not a delta for part
		}

		auto* setOp = static_cast<ASMContextNodeLeftRightOperator*>(initValNode);

		if (setOp->m_left->m_type != TYPE_IDENTIFIER) {
			index++;
			continue; // not an I = ...
		}

		UINT32 incrementName = static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value;

		auto* whileBlock = static_cast<ASMContextNodeWhile*>(whileStmt.node);

		size_t blockSize = SizeNoEmptyNode(whileBlock->m_block->m_statements);

		if (!blockSize) {
			index++;
			continue; // at least one statement in the while
		}

		size_t lastIndex = whileBlock->m_block->m_statements.size() - 1;

		while (whileBlock->m_block->m_statements[lastIndex].node->m_type == TYPE_PRECODEPOS) {
			lastIndex--;
		}

		auto& deltaStmt = whileBlock->m_block->m_statements[lastIndex];

		if (deltaStmt.node->m_type != TYPE_DELTA) {
			index++;
			continue; // not a delta for part
		}
		auto* deltaPart = static_cast<ASMContextNodeOp1*>(deltaStmt.node);

		if (deltaPart->m_operand->m_type != TYPE_IDENTIFIER || static_cast<ASMContextNodeIdentifier*>(deltaPart->m_operand)->m_value != incrementName) {
			index++;
			continue; // not an identifier delta
		}

		auto* forDeltaNode = new ASMContextNodeForDelta(initValNode->Clone(), whileBlock->m_condition, deltaPart->Clone(), whileBlock->m_block);
		whileBlock->m_block = nullptr;
		whileBlock->m_condition = nullptr;
		delete whileStmt.node;
		whileStmt.node = forDeltaNode;


		delete initValStmt.node;
		initValStmt.node = new ASMContextNodeValue<LPCCH>("<for_init>", TYPE_PRECODEPOS);

		delete deltaStmt.node;
		deltaStmt.node = new ASMContextNodeValue<LPCCH>("<for_continue>", TYPE_PRECODEPOS);
		auto* continueLoc = deltaStmt.location;

		std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)> func;

		// continue
		func = [&func, continueLoc](ASMContextNodeBlock* block, ASMContext& ctx) {
			for (auto& bs : block->m_statements) {
				if (!IsJumpType(bs.node->m_type)) {
					bs.node->ApplySubBlocks(func, ctx);
					continue;
				}

				auto* j = static_cast<ASMContextNodeJumpOperator*>(bs.node);
				if (j->m_location == continueLoc->rloc) {
					j->m_operatorName = "continue";
					j->m_showJump = false;
					j->m_special_op = SPECIAL_JUMP_CONTINUE;
					continueLoc->RemoveRef(j->m_opLoc);
				}
			}
		};

		forDeltaNode->ApplySubBlocks(func, ctx);
	}

	return 0;
}

#pragma endregion
