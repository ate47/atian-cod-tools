#include <includes.hpp>
#include "gsc_opcodes_load.hpp"
using namespace tool::gsc::opcode;

// maps to find the opcodes

// VM->vminfo
static std::unordered_map<BYTE, vminfo> g_opcodeMap{};
// opcode->opcode handler
static std::unordered_map<opcode, const opcodeinfo*> g_opcodeHandlerMap{};

#pragma region opcode_node

size_t SizeNoEmptyNode(const std::vector<asmcontextstatement>& statements) {
	size_t acc = 0;
	for (const auto& stmt : statements) {
		if (stmt.node->m_type != TYPE_PRECODEPOS) {
			acc++;
		}
	}
	return acc;
}

asmcontextnode* GetNoEmptyNode(const std::vector<asmcontextstatement>& statements, size_t index) {
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

opcodeinfo::opcodeinfo(opcode id, LPCCH name) : m_id(id), m_name(name) {
}

asmcontextlocationop::~asmcontextlocationop() {
}

void asmcontextlocationop::Run(asmcontext& context, T8GSCOBJContext& objctx) const {
	assert(0);
}

asmcontextnode::asmcontextnode(asmcontextnode_priority priority, asmcontextnode_type type) : m_priority(priority), m_type(type) {
}

asmcontextnode::~asmcontextnode() {
}

void asmcontextnode::ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>&, asmcontext& ctx) {
	// do nothing, no sub blocks
}

asmcontextnodeblock::asmcontextnodeblock(nodeblocktype blockType) : asmcontextnode(PRIORITY_INST, TYPE_BLOCK), m_blockType(blockType) {
	m_renderSemicolon = false;
}

asmcontextnodeblock::~asmcontextnodeblock() {
	for (auto& ref : m_statements) {
		delete ref.node;
	}
}

asmcontextstatement* asmcontextnodeblock::FetchFirstForLocation(INT64 rloc) {
	for (auto& loc : m_statements) {
		if (loc.location->rloc >= rloc) {
			return &loc;
		}
	}
	return nullptr;
}

asmcontextnode* asmcontextnodeblock::Clone() const {
	asmcontextnodeblock* n = new asmcontextnodeblock(m_blockType);
	for (auto& node : m_statements) {
		n->m_statements.push_back({ node.node->Clone(), node.location });
	}
	return n;
}

void asmcontextnodeblock::ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) {
	// apply to this
	func(this, ctx);
}

void asmcontextnode::Dump(std::ostream& out, decompcontext& ctx) const {
	// nothing by default
}

asmcontextnode* asmcontextnode::Clone() const {
	assert(0); // shouldn't be possible
	return nullptr;
}

template<typename Type>
class asmcontextnode_Value : public asmcontextnode {
public:
	Type m_value;
	asmcontextnode_Value(Type value, asmcontextnode_type type = TYPE_UNDEFINED) : asmcontextnode(PRIORITY_VALUE, type), m_value(value) {
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << std::dec << m_value;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Value<Type>(m_value, m_type);
	}
};

class asmcontextnode_Identifier : public asmcontextnode {
public:
	UINT32 m_value;
	LPCCH m_type;
	asmcontextnode_Identifier(UINT32 value, LPCCH type = "var") : asmcontextnode(PRIORITY_VALUE, TYPE_IDENTIFIER), m_value(value), m_type(type) {
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << hashutils::ExtractTmp(m_type, m_value) << std::flush;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Identifier(m_value, m_type);
	}
};

class asmcontextnode_Hash : public asmcontextnode {
public:
	UINT64 m_value;
	bool m_canonid;
	asmcontextnode_Hash(UINT64 value, bool canonid = false) : asmcontextnode(PRIORITY_VALUE, TYPE_CONST_HASH), m_value(value), m_canonid(canonid) {
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		if (m_canonid) {
			out << "#" << hashutils::ExtractTmp("var", m_value) << std::flush;
		}
		else {
			out << "#\"" << hashutils::ExtractTmp("hash", m_value) << '"' << std::flush;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Hash(m_value, m_canonid);
	}
};

class asmcontextnode_Vector : public asmcontextnode {
public:
	asmcontextnode* m_x;
	asmcontextnode* m_y;
	asmcontextnode* m_z;
	asmcontextnode_Vector(asmcontextnode* x, asmcontextnode* y, asmcontextnode* z) : asmcontextnode(PRIORITY_VALUE), m_x(x), m_y(y), m_z(z) {
	}
	asmcontextnode_Vector(FLOAT x, FLOAT y, FLOAT z) : asmcontextnode_Vector(
		new asmcontextnode_Value<FLOAT>(x),
		new asmcontextnode_Value<FLOAT>(y),
		new asmcontextnode_Value<FLOAT>(z)
		) {
	}
	~asmcontextnode_Vector() {
		delete m_x;
		delete m_y;
		delete m_z;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "(";
		m_x->Dump(out, ctx);
		out << ", ";
		m_y->Dump(out, ctx);
		out << ", ";
		m_z->Dump(out, ctx);
		out << ")";
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Vector(m_x->Clone(), m_y->Clone(), m_z->Clone());
	}
};

class asmcontextnode_String : public asmcontextnode {
public:
	LPCCH m_value;
	asmcontextnode_String(LPCCH value) : asmcontextnode(PRIORITY_VALUE), m_value(value) {
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "\"" << m_value << "\"" << std::flush;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_String(m_value);
	}
};

class asmcontextnode_FuncRef : public asmcontextnode {
public:
	LPCCH m_op;
	UINT32 m_func;
	UINT32 m_nsp;
	UINT64 m_script;
	asmcontextnode_FuncRef(LPCCH op, UINT32 func, UINT32 nsp = 0, UINT64 script = 0) : asmcontextnode(PRIORITY_VALUE),
		m_op(op), m_func(func), m_nsp(nsp), m_script(script) {
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << m_op;
		if (m_nsp) {
			out << hashutils::ExtractTmp("namespace", m_nsp) << std::flush;
			if (m_script) {
				out << "<" << hashutils::ExtractTmp("script", m_script) << ">" << std::flush;
			}
			out << "::";
		}
		out << hashutils::ExtractTmp("function", m_func) << std::flush;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_FuncRef(m_op, m_func, m_nsp, m_script);
	}
};

class asmcontextnode_ArrayAccess : public asmcontextnode {
public:
	asmcontextnode* m_operandleft;
	asmcontextnode* m_operandright;
	asmcontextnode_ArrayAccess(asmcontextnode* operandleft, asmcontextnode* operandright) :
		asmcontextnode(PRIORITY_ACCESS), m_operandleft(operandleft), m_operandright(operandright) {
	}
	~asmcontextnode_ArrayAccess() {
		delete m_operandleft;
		delete m_operandright;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

	asmcontextnode* Clone() const override {
		return new asmcontextnode_ArrayAccess(m_operandleft->Clone(), m_operandright->Clone());
	}
};

class asmcontextnode_ArrayBuildNode : public asmcontextnode {
public:
	asmcontextnode* m_key;
	asmcontextnode* m_value;
	asmcontextnode_ArrayBuildNode(asmcontextnode* key, asmcontextnode* value) :
		asmcontextnode(PRIORITY_SET), m_key(key), m_value(value) {
	}
	~asmcontextnode_ArrayBuildNode() {
		delete m_key;
		delete m_value;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		m_key->Dump(out, ctx);

		out << ":";

		m_value->Dump(out, ctx);
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_ArrayBuildNode(m_key->Clone(), m_value->Clone());
	}
};

class asmcontextnode_ArrayBuild : public asmcontextnode {
public:
	std::vector<asmcontextnode_ArrayBuildNode*> m_operands{};
	asmcontextnode_ArrayBuild() :
		asmcontextnode(PRIORITY_ACCESS, TYPE_ARRAY_BUILD) {
	}
	~asmcontextnode_ArrayBuild() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

	void AddValue(asmcontextnode* key, asmcontextnode* value) {
		m_operands.push_back(new asmcontextnode_ArrayBuildNode(key, value));
	}

	asmcontextnode* Clone() const override {
		auto* n = new asmcontextnode_ArrayBuild();

		for (const auto& ref : m_operands) {
			n->AddValue(ref->m_key->Clone(), ref->m_value->Clone());
		}

		return n;
	}
};

class asmcontextnode_StructBuild : public asmcontextnode {
public:
	std::vector<asmcontextnode_ArrayBuildNode*> m_operands{};
	asmcontextnode_StructBuild() :
		asmcontextnode(PRIORITY_ACCESS, TYPE_STRUCT_BUILD) {
	}
	~asmcontextnode_StructBuild() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

	void AddValue(asmcontextnode* key, asmcontextnode* value) {
		if (key->m_type == TYPE_CONST_HASH) {
			// this hash is a canon id, not a fnva1
			static_cast<asmcontextnode_Hash*>(key)->m_canonid = true;
		}
		m_operands.push_back(new asmcontextnode_ArrayBuildNode(key, value));
	}

	asmcontextnode* Clone() const override {
		auto* n = new asmcontextnode_StructBuild();

		for (const auto& ref : m_operands) {
			n->AddValue(ref->m_key->Clone(), ref->m_value->Clone());
		}

		return n;
	}
};

enum asmcontextnode_CallFuncPtrType {
	CLASS_CALL,
	PARAMS_CALL,
	POINTER_CALL,
	FUNCTION_CALL
};

enum asmcontextnode_CallFuncFlag : BYTE {
	THREAD_CALL = 1,
	THREADENDON_CALL = 2,
	SELF_CALL = 4
};

class asmcontextnode_CallFuncPtr : public asmcontextnode {
private:
	asmcontextnode_CallFuncPtrType m_type;
	BYTE m_flags;
public:
	std::vector<asmcontextnode*> m_operands{};
	asmcontextnode_CallFuncPtr(asmcontextnode_CallFuncPtrType type, BYTE flags) : asmcontextnode(PRIORITY_ACCESS, TYPE_FUNC_CALL), m_type(type), m_flags(flags) {
	}
	~asmcontextnode_CallFuncPtr() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	void AddParam(asmcontextnode* node) {
		m_operands.push_back(node);
	}

	asmcontextnode* Clone() const override {
		auto* ref = new asmcontextnode_CallFuncPtr(m_type, m_flags);
		ref->m_operands.reserve(m_operands.size());
		for (const auto& op : m_operands) {
			ref->m_operands.push_back(op->Clone());
		}

		return ref;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		size_t start = 0;
		if (m_flags & SELF_CALL) {
			m_operands[start]->Dump(out, ctx);
			out << " ";
			start++;
		}
		if (m_flags & THREADENDON_CALL) {
			out << "threadendon ";
		}
		else if (m_flags & THREAD_CALL) {
			out << "thread ";
		}

		switch (m_type) {
		case asmcontextnode_CallFuncPtrType::CLASS_CALL:
			assert(m_operands.size() >= start + 2);
			out << "[[ ";
			m_operands[start]->Dump(out, ctx);
			out << " ]]->";
			m_operands[start + 1]->Dump(out, ctx);
			start += 2;
			break;
		case asmcontextnode_CallFuncPtrType::POINTER_CALL:
			assert(m_operands.size() >= start + 1);
			out << "[[ ";
			m_operands[start]->Dump(out, ctx);
			out << " ]]";
			start++;
			break;
		case asmcontextnode_CallFuncPtrType::FUNCTION_CALL:
			assert(m_operands.size() >= start + 1);
			m_operands[start]->Dump(out, ctx);
			start++;
			break;
		case asmcontextnode_CallFuncPtrType::PARAMS_CALL:
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

class asmcontextnode_Op2 : public asmcontextnode {
public:
	LPCCH m_description;
	asmcontextnode* m_operand1;
	asmcontextnode* m_operand2;
	asmcontextnode_Op2(LPCCH description, asmcontextnode_priority priority, asmcontextnode* operand1, asmcontextnode* operand2) :
		asmcontextnode(priority), m_description(description), m_operand1(operand1), m_operand2(operand2) {
	}
	~asmcontextnode_Op2() {
		delete m_operand1;
		delete m_operand2;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Op2(m_description, m_priority, m_operand1->Clone(), m_operand2->Clone());
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

class asmcontextnode_Op1 : public asmcontextnode {
public:
	LPCCH m_description;
	bool m_prefix;
	asmcontextnode* m_operand;
	asmcontextnode_Op1(LPCCH description, bool prefix, asmcontextnode* operand, asmcontextnode_type type = TYPE_UNDEFINED) :
		asmcontextnode(PRIORITY_UNARY, type), m_prefix(prefix), m_description(description), m_operand(operand) {
		assert(operand);
	}
	~asmcontextnode_Op1() {
		delete m_operand;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Op1(m_description, m_prefix, m_operand->Clone(), m_type);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

class asmcontextnode_MultOp : public asmcontextnode {
public:
	LPCCH m_description;
	bool m_caller;
	std::vector<asmcontextnode*> m_operands{};
	asmcontextnode_MultOp(LPCCH description, bool caller, asmcontextnode_type type = TYPE_STATEMENT) :
		asmcontextnode(PRIORITY_VALUE, type), m_description(description), m_caller(caller) {
	}
	~asmcontextnode_MultOp() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	asmcontextnode* Clone() const override {
		auto* ref = new asmcontextnode_MultOp(m_description, m_caller, m_type);
		ref->m_operands.reserve(m_operands.size());
		for (const auto& op : m_operands) {
			ref->m_operands.push_back(op->Clone());
		}

		return ref;
	}

	void AddParam(asmcontextnode* node) {
		m_operands.push_back(node);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

class asmcontextnode_FunctionOperator : public asmcontextnode {
public:
	LPCCH m_operatorName;
	asmcontextnode* m_self;
	asmcontextnode* m_operand;
	asmcontextnode_FunctionOperator(LPCCH operatorName, asmcontextnode* self, asmcontextnode* operand, asmcontextnode_type type = TYPE_STATEMENT) :
		asmcontextnode(PRIORITY_VALUE, type), m_operatorName(operatorName), m_self(self), m_operand(operand) {
		assert(operand);
	}
	~asmcontextnode_FunctionOperator() {
		if (m_self) {
			delete m_self;
		}
		delete m_operand;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_FunctionOperator(m_operatorName, m_self ? m_self->Clone() : nullptr, m_operand->Clone(), m_type);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

enum asmcontextnode_JumpOperatorSpecialJump {
	SPECIAL_JUMP_DEFAULT = 0,
	SPECIAL_JUMP_BREAK,
	SPECIAL_JUMP_CONTINUE
};

class asmcontextnode_JumpOperator : public asmcontextnode {
public:
	LPCCH m_operatorName;
	asmcontextnode* m_operand;
	INT64 m_location;
	INT32 m_opLoc;
	bool m_showJump;
	asmcontextnode_JumpOperatorSpecialJump m_special_op;
	INT64 m_delta;
	asmcontextnode_JumpOperator(LPCCH operatorName, asmcontextnode* operand, INT64 location, asmcontextnode_type type, INT32 opLoc, bool showJump = true, INT64 delta = 0, asmcontextnode_JumpOperatorSpecialJump special_op = SPECIAL_JUMP_DEFAULT) :
		asmcontextnode(PRIORITY_INST, type), m_operatorName(operatorName), m_operand(operand), m_location(location), m_opLoc(opLoc), m_showJump(showJump), m_delta(delta), m_special_op(special_op){
		assert(IsJumpType(type));
	}
	~asmcontextnode_JumpOperator() {
		if (m_operand) {
			delete m_operand;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_JumpOperator(m_operatorName, m_operand ? m_operand->Clone() : nullptr, m_location, m_type, m_opLoc, m_showJump, m_delta, m_special_op);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

class asmcontextnode_LeftRightOperator : public asmcontextnode {
public:
	LPCCH m_operatorName;
	asmcontextnode* m_left;
	asmcontextnode* m_right;
	asmcontextnode_LeftRightOperator(asmcontextnode* left, asmcontextnode* right, LPCCH operatorName = " = ", asmcontextnode_priority priority = PRIORITY_SET, asmcontextnode_type type = TYPE_SET) :
		asmcontextnode(priority, type), m_operatorName(operatorName), m_left(left), m_right(right) {
	}
	~asmcontextnode_LeftRightOperator() {
		delete m_left;
		delete m_right;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_LeftRightOperator(m_left->Clone(), m_right->Clone(), m_operatorName, m_priority, m_type);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		m_left->Dump(out, ctx);

		out << m_operatorName;

		m_right->Dump(out, ctx);
	}
};

class asmcontextnode_New : public asmcontextnode {
public:
	UINT32 m_classname;
	asmcontextnode* m_constructorCall;
	bool m_constructorCallDec;
	asmcontextnode_New(UINT32 clsName, asmcontextnode* constructorCall = nullptr, bool constructorCallDec = false) :
		asmcontextnode(PRIORITY_VALUE, TYPE_NEW), m_classname(clsName), m_constructorCall(constructorCall), m_constructorCallDec(constructorCallDec) {
	}
	~asmcontextnode_New() {
		if (m_constructorCall) {
			delete m_constructorCall;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_New(m_classname, m_constructorCall ? m_constructorCall->Clone() : nullptr, m_constructorCallDec);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "new " << hashutils::ExtractTmp("class", m_classname) << std::flush;
		if (m_constructorCall) {
			m_constructorCall->Dump(out, ctx);
		}
	}
};

struct switchcase {
	asmcontextnode* casenode = nullptr;
	INT32 jumpLocation = 0;
};

class asmcontextnode_SwitchPreCompute : public asmcontextnode {
public:
	asmcontextnode* m_node;
	std::vector<switchcase> m_cases{};
	INT32 m_endLocation;
	asmcontextnode_SwitchPreCompute(asmcontextnode* node, INT32 endLocation = 0) :
		asmcontextnode(PRIORITY_INST, TYPE_SWITCH_PRECOMPUTE), m_node(node), m_endLocation(endLocation) {
		m_renderSemicolon = false;
	}
	~asmcontextnode_SwitchPreCompute() {
		delete m_node;
		for (auto& cs : m_cases) {
			if (cs.casenode) {
				delete cs.casenode;
			}
		}
	}

	asmcontextnode* Clone() const override {
		auto* sw = new asmcontextnode_SwitchPreCompute(m_node->Clone(), m_endLocation);
		sw->m_cases.reserve(m_cases.size());
		for (const auto& cs : m_cases) {
			sw->m_cases.push_back({ cs.casenode ? cs.casenode->Clone() : cs.casenode, cs.jumpLocation });
		}
		return sw;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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

struct switchcasepost{
	asmcontextnode* value;
	asmcontextnodeblock* block;
};

class asmcontextnode_SwitchPostCompute : public asmcontextnode {
public:
	asmcontextnode* m_node;
	std::vector<switchcasepost> m_cases{};
	asmcontextnode_SwitchPostCompute(asmcontextnode* node) :
		asmcontextnode(PRIORITY_INST, TYPE_SWITCH_POSTCOMPUTE), m_node(node) {
		m_renderSemicolon = false;
	}
	~asmcontextnode_SwitchPostCompute() {
		delete m_node;
		for (auto& cs : m_cases) {
			if (cs.value) {
				delete cs.value;
			}
			delete cs.block;
		}
	}

	asmcontextnode* Clone() const override {
		auto* sw = new asmcontextnode_SwitchPostCompute(m_node->Clone());
		sw->m_cases.reserve(m_cases.size());
		for (const auto& cs : m_cases) {
			sw->m_cases.push_back({ cs.value ? cs.value->Clone() : cs.value, static_cast<asmcontextnodeblock*>(cs.block->Clone()) });
		}
		return sw;
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		for (const auto& cs : m_cases) {
			cs.block->ApplySubBlocks(func, ctx);
		}
	}
};

class asmcontextnode_ForEach : public asmcontextnode {
public:
	UINT32 m_key;
	UINT32 m_item;
	asmcontextnode* m_arrayNode;
	asmcontextnodeblock* m_block;
	asmcontextnode_ForEach(asmcontextnode* arrayNode, asmcontextnodeblock* block, UINT32 key, UINT32 item) :
		asmcontextnode(PRIORITY_INST, TYPE_FOR_EACH), m_arrayNode(arrayNode), m_block(block), m_key(key), m_item(item) {
		m_renderSemicolon = false;
	}
	~asmcontextnode_ForEach() {
		delete m_arrayNode;
		delete m_block;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_ForEach(m_arrayNode->Clone(), static_cast<asmcontextnodeblock*>(m_block->Clone()), m_key, m_item);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "foreach (";
		if (m_key) {
			out << hashutils::ExtractTmp("var", m_key) << std::flush << ", ";
		}
		out << hashutils::ExtractTmp("var", m_item) << std::flush << " in ";
		m_arrayNode->Dump(out, ctx);
		out << ") ";
		m_block->Dump(out, ctx);
	}
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class asmcontextnode_ForDelta : public asmcontextnode {
public:
	asmcontextnode* m_init;
	asmcontextnode* m_cond;
	asmcontextnode* m_delta;
	asmcontextnodeblock* m_block;
	asmcontextnode_ForDelta(asmcontextnode* init, asmcontextnode* cond, asmcontextnode* delta, asmcontextnodeblock* block) :
		asmcontextnode(PRIORITY_INST, TYPE_FOR), m_init(init), m_cond(cond), m_delta(delta), m_block(block) {
		assert(init);
		// condition can be null for(;;) converted into for(i=0;;i++)
		assert(delta);
		assert(block);
		m_renderSemicolon = false;
	}
	~asmcontextnode_ForDelta() {
		delete m_init;
		delete m_cond;
		delete m_delta;
		delete m_block;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_ForDelta(m_init->Clone(), m_cond ? m_cond->Clone() : nullptr, m_delta->Clone(), static_cast<asmcontextnodeblock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class asmcontextnode_DoWhile : public asmcontextnode {
public:
	asmcontextnode* m_condition;
	asmcontextnodeblock* m_block;
	asmcontextnode_DoWhile(asmcontextnode* condition, asmcontextnodeblock* block) :
		asmcontextnode(PRIORITY_INST, TYPE_DO_WHILE), m_condition(condition), m_block(block) {
		block->m_blockType = BLOCK_PADDING; // use custom block, no need to write the { }
	}
	~asmcontextnode_DoWhile() {
		delete m_condition;
		delete m_block;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_DoWhile(m_condition->Clone(), static_cast<asmcontextnodeblock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "do {";
		m_block->Dump(out, ctx);
		ctx.WritePadding(out, true) << "} while(";
		m_condition->Dump(out, ctx);
		out << ")";
	}
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class asmcontextnode_While : public asmcontextnode {
public:
	asmcontextnode* m_condition;
	asmcontextnodeblock* m_block;
	asmcontextnode_While(asmcontextnode* condition, asmcontextnodeblock* block) :
		asmcontextnode(PRIORITY_INST, TYPE_WHILE), m_condition(condition), m_block(block) {
		m_renderSemicolon = false;
	}
	~asmcontextnode_While() {
		if (m_condition) {
			delete m_condition;
		}
		if (m_block) {
			delete m_block;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_While(m_condition ? m_condition->Clone() : nullptr, static_cast<asmcontextnodeblock*>(m_block->Clone()));
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
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
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}
};

class asmcontextnode_IfElse : public asmcontextnode {
public:
	asmcontextnode* m_condition;
	asmcontextnodeblock* m_ifblock;
	asmcontextnodeblock* m_elseblock;
	asmcontextnode_IfElse(asmcontextnode* condition, asmcontextnodeblock* ifblock, asmcontextnodeblock* elseblock) :
		asmcontextnode(PRIORITY_INST, TYPE_IF), m_condition(condition), m_ifblock(ifblock), m_elseblock(elseblock){
		assert(condition);
		assert(ifblock);
		m_renderSemicolon = false;
		// set padding
		ifblock->m_blockType = BLOCK_PADDING;
		if (elseblock) {
			elseblock->m_blockType = BLOCK_PADDING;
		}
	}
	~asmcontextnode_IfElse() {
		delete m_condition;
		delete m_ifblock;
		if (m_elseblock) {
			delete m_elseblock;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_IfElse(m_condition->Clone(), 
			static_cast<asmcontextnodeblock*>(m_ifblock->Clone()), 
			m_elseblock ? static_cast<asmcontextnodeblock*>(m_elseblock->Clone()) : nullptr);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << "if (";
		m_condition->Dump(out, ctx);
		out << ") {";
		m_ifblock->Dump(out, ctx);
		asmcontextnodeblock* elseBlock = m_elseblock;

		// loop over all the nested if blocks to create a pretty itie*e output
		while (elseBlock && SizeNoEmptyNode(elseBlock->m_statements) == 1) {
			auto* ref = GetNoEmptyNode(elseBlock->m_statements, 0);
			if (ref->m_type != TYPE_IF) {
				break;
			}
			auto* ifb = static_cast<asmcontextnode_IfElse*>(ref);
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
	void ApplySubBlocks(const std::function<void(asmcontextnodeblock* block, asmcontext& ctx)>& func, asmcontext& ctx) override {
		m_ifblock->ApplySubBlocks(func, ctx);
		if (m_elseblock) {
			m_elseblock->ApplySubBlocks(func, ctx);
		}
	}
};
#pragma endregion
#pragma region opcode_info

class asmcontextlocationop_Op : public asmcontextlocationop {
private:
	asmcontextnode* m_node;
	asmcontextnode_priority m_priority;
	LPCCH m_description;
public:
	asmcontextlocationop_Op(asmcontextnode* node, LPCCH description, asmcontextnode_priority priority) : 
		asmcontextlocationop(), m_node(node), m_description(description), m_priority(priority) {
	}
	~asmcontextlocationop_Op() {
		if (m_node) {
			delete m_node;
		}
	}
	void Run(asmcontext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
		auto* right = context.PopASMCNode();
		context.PushASMCNode(new asmcontextnode_Op2(m_description, m_priority, m_node ? m_node->Clone() : nullptr, right));
	}
};

int opcodeinfo::Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const {
	out << "\n";
	return 0; // by default nop
}

class opcodeinfo_unknown : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "Unknown operator: " << std::hex << value << "\n";
		return -1;
	}
};

class opcodeinfo_UnknownDev : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& loc = context.Aligned<UINT16>();

		UINT16 val = *(UINT16*)loc;

		loc += 2;
		out << std::hex << val << "\n";
		return 0;
	}
};

class opcodeinfo_SafeCreateLocalVariables : public opcodeinfo {
public:
	opcodeinfo_SafeCreateLocalVariables() : opcodeinfo(OPCODE_SafeCreateLocalVariables, "SafeCreateLocalVariables") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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

			if (flags & ~(T8GSCLocalVarFlag::VARIADIC | T8GSCLocalVarFlag::ARRAY_REF)) {
				out << " unk flags: " << std::hex << (int)flags;
			}

			if (context.m_runDecompiler && context.m_opt.m_show_func_vars && i >= context.m_exp.param_count) {
				context.m_lastOpCodeBase = 0;
				context.PushASMCNode(new asmcontextnode_Identifier(varName, "var"));
				context.CompleteStatement();
			}

			out << std::endl;
		}
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}
};
class opcodeinfo_CheckClearParams : public opcodeinfo {
public:
	opcodeinfo_CheckClearParams() : opcodeinfo(OPCODE_CheckClearParams, "CheckClearParams") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}
};

class opcodeinfo_GetObjectType : public opcodeinfo {
public:
	opcodeinfo_GetObjectType(opcode id, LPCCH name, LPCCH hashType) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("class", name) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Value<LPCCH>("<emptypos_getobjecttype>", TYPE_PRECODEPOS));
			context.PushASMCNode(new asmcontextnode_New(name));
		}

		return 0;
	}
};

template<typename Type>
class opcodeinfo_GetNeg : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<Type>();

		INT64 negv = -*(Type*)bytecode;

		bytecode += sizeof(Type);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Value<INT64>(negv));
		}

		out << std::dec << negv << std::endl;

		return 0;
	}
};

template<typename Type>
class opcodeinfo_GetConstant : public opcodeinfo {
	Type m_value;
public:
	opcodeinfo_GetConstant(opcode id, LPCCH name, Type value) : opcodeinfo(id, name), m_value(value) {
	}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Value<Type>(m_value));
		}

		out << "\n";

		return 0;
	}
};

template<typename Type>
class opcodeinfo_GetConstantRef : public opcodeinfo {
	Type m_value;
public:
	opcodeinfo_GetConstantRef(opcode id, LPCCH name, Type value) : opcodeinfo(id, name), m_value(value) {
	}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new asmcontextnode_Value<Type>(m_value));
		}

		out << "\n";

		return 0;
	}
};

template<typename Type>
class opcodeinfo_GetConstantSet : public opcodeinfo {
	Type m_value;
	bool m_inst;
public:
	opcodeinfo_GetConstantSet(opcode id, LPCCH name, Type value, bool inst = false) : opcodeinfo(id, name), m_value(value), m_inst(inst) {
	}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(left, new asmcontextnode_Value<Type>(m_value)));
			if (m_inst) {
				context.CompleteStatement();
			}
		}

		out << "\n";

		return 0;
	}
};

class opcodeinfo_GetObjectSize : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(left, new asmcontextnode_Value<LPCCH>("size"), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		out << "\n";

		return 0;
	}
};

class opcodeinfo_VectorScale : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("vectorscale", false);

			node->AddParam(context.PopASMCNode());
			node->AddParam(context.PopASMCNode());

			context.PushASMCNode(node);
		}

		out << "\n";

		return 0;
	}
};

template<typename Type, typename WriteType = INT64>
class opcodeinfo_GetNumber : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<Type>();

		WriteType intValue = *(Type*)bytecode;

		bytecode += sizeof(Type);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Value<WriteType>(intValue));
		}

		out << std::dec << intValue << std::endl;

		return 0;
	}
};

class opcodeinfo_GetHash : public opcodeinfo {
public:
	opcodeinfo_GetHash() : opcodeinfo(OPCODE_GetHash, "GetHash") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<UINT64>();

		UINT64 hash = *(UINT64*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Hash(hash));
		}

		out << "#\"" << hashutils::ExtractTmp("hash", hash) << "\" (#" << std::hex << hash << ")" << std::endl;

		return 0;
	}
};

class opcodeinfo_Jump : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// get the jump opcode location
		INT32 m_jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);

		asmcontextnode* node = nullptr;
		asmcontextnode_type type = TYPE_JUMP;
		LPCCH name = nullptr;
		bool pushBack = false;
		if (context.m_runDecompiler) {
			switch (m_id) {
			case OPCODE_JumpOnGreaterThan:
			{
				// reversed
				asmcontextnode* op2 = context.PopASMCNode();
				asmcontextnode* op1 = context.PopASMCNode();
				node = new asmcontextnode_Op2(">", PRIORITY_COMPARE, op1, op2);
				type = TYPE_JUMP_GREATERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnLessThan:
			{
				// reversed
				asmcontextnode* op2 = context.PopASMCNode();
				asmcontextnode* op1 = context.PopASMCNode();
				node = new asmcontextnode_Op2("<", PRIORITY_COMPARE, op1, op2);
				type = TYPE_JUMP_LOWERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnFalseExpr:
				pushBack = true;
				name = "jumpiffalseexpr";
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONFALSEEXPR;
				break;
			case OPCODE_JumpOnFalse:
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONFALSE;
				name = "jumpiffalse";
				break;
			case OPCODE_JumpOnTrueExpr:
				pushBack = true;
				name = "jumpiftrueexpr";
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONTRUEEXPR;
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
				break;
			}
		}

		assert(name != nullptr);

		auto& bytecode = context.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;

		// push a location and mark it as referenced
		BYTE* newLoc = &context.m_bcl[delta];

		// push the node value during the jump if the operator is asking for it
		auto& locref = context.PushLocation(newLoc);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_JumpOperator(name, node, locref.rloc, type, m_jumpLocation, true, delta));
			context.CompleteStatement();
		}

		locref.refs.insert(m_jumpLocation);

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
		//return m_id == OPCODE_Jump ? -2 : 0; // no code after jump
	}
};

class opcodeinfo_JumpExpr : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			asmcontextnode_priority priority = PRIORITY_VALUE;
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
			asmcontextnode* node = context.PopASMCNode();
			locref.m_lateop.push_back(new asmcontextlocationop_Op(node, desc, priority));
		}

		locref.refs.insert(m_jumpLocation);

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
	}
};

class opcodeinfo_JumpPush : public opcodeinfo {
public:
	opcodeinfo_JumpPush() : opcodeinfo(OPCODE_JumpPush, "JumpPush") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		INT32 jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& bytecode = context.Aligned<uintptr_t>();

		uintptr_t location = *(uintptr_t*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_JumpOperator("jumppush", nullptr, location, TYPE_STATEMENT, jumpLocation));
			context.CompleteStatement();
		}

		out << "Jump 0x" << std::hex << location << "\n";

		return -2; // no code after that
	}
};

class opcodeinfo_Vector : public opcodeinfo {
public:
	opcodeinfo_Vector() : opcodeinfo(OPCODE_Vector, "Vector") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* x = context.PopASMCNode();
			auto* y = context.PopASMCNode();
			auto* z = context.PopASMCNode();
			context.PushASMCNode(new asmcontextnode_Vector(x, y, z));
		}

		out << "\n";
		return 0;
	}
};

class opcodeinfo_GetVector : public opcodeinfo {
public:
	opcodeinfo_GetVector() : opcodeinfo(OPCODE_GetVector, "GetVector") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			context.PushASMCNode(new asmcontextnode_Vector(x, y, z));
		}
		out << "(" << x << ", " << y << ", " << z << ")\n";

		return 0;
	}
};

class opcodeinfo_VectorConstant : public opcodeinfo {
public:
	opcodeinfo_VectorConstant() : opcodeinfo(OPCODE_VectorConstant, "VectorConstant") {}

	float OfFlag(BYTE loc) const {
		switch (loc & 0x3) {
		case 0: return 0;
		case 1: return -1;
		case 2: return 1;
		default: return 999;
		}
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE flag = *(context.m_bcl++);

		FLOAT x = OfFlag(flag >> 4);
		FLOAT y = OfFlag(flag >> 2);
		FLOAT z = OfFlag(flag);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Vector(x, y, z));
		}

		out << "(" << x << ", " << y<< ", " << z << ")\n";

		return 0;
	}
};

class opcodeinfo_Name : public opcodeinfo {
private:
	LPCCH m_hashType;
public:
	opcodeinfo_Name(opcode id, LPCCH name, LPCCH hashType) : opcodeinfo(id, name), m_hashType(hashType) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp(m_hashType, name) << std::endl;

		return 0;
	}
};

class opcodeinfo_SetVariableFieldRef : public opcodeinfo {
public:
	opcodeinfo_SetVariableFieldRef(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			auto* node = new asmcontextnode_LeftRightOperator(
				new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), new asmcontextnode_Identifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS),
				context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET);
			context.SetFieldIdASMCNode(node->Clone());
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		return 0;
	}
};

class opcodeinfo_SetVariableField : public opcodeinfo {
public:
	opcodeinfo_SetVariableField(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}
};

class opcodeinfo_EvalSelfFieldVariable : public opcodeinfo {
public:
	opcodeinfo_EvalSelfFieldVariable(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		auto name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new asmcontextnode_Identifier(hashutils::Hash32("self")));
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), new asmcontextnode_Identifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}
};

class opcodeinfo_ClearFieldVariable : public opcodeinfo {
private:
	bool m_stack;
public:
	opcodeinfo_ClearFieldVariable(opcode id, LPCCH name, bool stack) : opcodeinfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			asmcontextnode* nameNode = m_stack ? context.PopASMCNode() : new asmcontextnode_Identifier(name);
			asmcontextnode* fieldAccessNode = new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), nameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(fieldAccessNode, new asmcontextnode_Value<LPCCH>("undefined"), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}
};

class opcodeinfo_EvalGlobalObjectFieldVariable : public opcodeinfo {
public:
	opcodeinfo_EvalGlobalObjectFieldVariable() : opcodeinfo(OPCODE_EvalGlobalObjectFieldVariable, "EvalGlobalObjectFieldVariable") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			out << "bad objectid ref: 0x" << std::hex << objectid;
		}

		out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new asmcontextnode_Identifier(name));
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), new asmcontextnode_Identifier(fieldName), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}
};

class opcodeinfo_CastAndEvalFieldVariable : public opcodeinfo {
public:
	opcodeinfo_CastAndEvalFieldVariable(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<UINT32>();

		UINT32 name = *(UINT32*)ref;

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
			context.PushASMCNode(new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), new asmcontextnode_Identifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}
};

class opcodeinfo_EvalFieldVariable : public opcodeinfo {
private:
	bool m_stack;
	bool m_ref;
public:
	opcodeinfo_EvalFieldVariable(opcode id, LPCCH name, bool stack, bool ref) : opcodeinfo(id, name), m_stack(stack), m_ref(ref) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			asmcontextnode* node = new asmcontextnode_LeftRightOperator(context.GetObjectIdASMCNode(), m_stack ? context.PopASMCNode() : new asmcontextnode_Identifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS);

			if (m_ref) {
				context.SetFieldIdASMCNode(node);
			}
			else {
				context.PushASMCNode(node);
			}
		}

		return 0;
	}
};

class opcodeinfo_EvalLocalVariableCached : public opcodeinfo {
public:
	opcodeinfo_EvalLocalVariableCached(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Identifier(name));
		}

		return 0;
	}
};

class opcodeinfo_SetLocalVariableCached : public opcodeinfo {
private:
	bool m_stack;
public:
	opcodeinfo_SetLocalVariableCached(opcode id, LPCCH name, bool stack) : opcodeinfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		UINT32 name;

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			if (!m_stack) {
				context.SetFieldIdASMCNode(new asmcontextnode_Identifier(name));
				context.PushASMCNode(new asmcontextnode_LeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			}
			else {
				context.PushASMCNode(new asmcontextnode_LeftRightOperator(new asmcontextnode_Identifier(name), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			}
			context.CompleteStatement();
		}

		return 0;
	}
};

class opcodeinfo_FirstArrayKey : public opcodeinfo {
private:
	bool m_stack;
public:
	opcodeinfo_FirstArrayKey(opcode id, LPCCH name, bool stack) : opcodeinfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		if (!m_stack) {
			int lvar = (int)*(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				name = hashutils::Hash32("<error>");
				out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
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
			asmcontextnode* arrayNode;
			if (m_stack) {
				arrayNode = context.PopASMCNode();
			}
			else {
				arrayNode = new asmcontextnode_Identifier(name);
			}

			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("firstarray", false, TYPE_ARRAY_FIRSTKEY);

			node->AddParam(arrayNode);

			context.PushASMCNode(node);
		}

		return 0;
	}
};

class opcodeinfo_SetNextArrayKeyCached : public opcodeinfo {
public:
	opcodeinfo_SetNextArrayKeyCached(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			asmcontextnode* arrayNode = context.PopASMCNode();

			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("nextarray", false, TYPE_ARRAY_NEXTKEY);

			node->AddParam(arrayNode);
			node->AddParam(new asmcontextnode_Identifier(name));

			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		return 0;
	}
};

class opcodeinfo_EvalFieldObjectFromRef : public opcodeinfo {
public:
	opcodeinfo_EvalFieldObjectFromRef(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new asmcontextnode_Identifier(name));
		}

		return 0;
	}
};

class opcodeinfo_EvalLocalVariableRefCached : public opcodeinfo {
public:
	opcodeinfo_EvalLocalVariableRefCached(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.SetFieldIdASMCNode(new asmcontextnode_Identifier(name));
		}

		return 0;
	}
};

class opcodeinfo_EvalLocalVariableDefined : public opcodeinfo {
public:
	opcodeinfo_EvalLocalVariableDefined(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		UINT32 name;
		int lvar = (int)*(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hashutils::Hash32("<error>");
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_FunctionOperator("isdefined", nullptr, new asmcontextnode_Identifier(name), TYPE_FUNC_IS_DEFINED));
		}

		return 0;
	}
};

class opcodeinfo_EvalArray : public opcodeinfo {
private:
	bool m_stack;
public:
	opcodeinfo_EvalArray(opcode id, LPCCH name, bool stack) : opcodeinfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			if (!m_stack) {
				context.SetObjectIdASMCNode(context.GetFieldIdASMCNode());
			}
			asmcontextnode* arrayNode = m_stack ? context.PopASMCNode() : context.GetObjectIdASMCNode();
			asmcontextnode* accessNode = new asmcontextnode_ArrayAccess(arrayNode, context.PopASMCNode());
			if (m_stack) {
				context.PushASMCNode(accessNode);
			}
			else {
				context.SetFieldIdASMCNode(accessNode);
			}
		}

		return 0;
	}
};

class opcodeinfo_CreateArray : public opcodeinfo {
public:
	opcodeinfo_CreateArray(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_ArrayBuild());
		}

		return 0;
	}
};

class opcodeinfo_AddToArray : public opcodeinfo {
public:
	opcodeinfo_AddToArray(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			asmcontextnode* key = context.PopASMCNode();
			asmcontextnode* value = context.PopASMCNode();
			asmcontextnode* arrayVal = context.PeekASMCNode();

			if (arrayVal->m_type == TYPE_ARRAY_BUILD) {
				// we are building an array, we can add the value
				static_cast<asmcontextnode_ArrayBuild*>(arrayVal)->AddValue(key, value);
			}
			else {
				asmcontextnode_MultOp* node = new asmcontextnode_MultOp("$addtoarray", false);


				node->AddParam(context.PopASMCNode());
				node->AddParam(key);
				node->AddParam(value);

				context.PushASMCNode(node);
			}
		}

		return 0;
	}
};

class opcodeinfo_CreateStruct : public opcodeinfo {
public:
	opcodeinfo_CreateStruct(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_StructBuild());
		}

		return 0;
	}
};

class opcodeinfo_AddToStruct : public opcodeinfo {
public:
	opcodeinfo_AddToStruct(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			asmcontextnode* key = context.PopASMCNode();
			asmcontextnode* value = context.PopASMCNode();
			asmcontextnode* structVal = context.PeekASMCNode();

			if (structVal->m_type == TYPE_STRUCT_BUILD) {
				// we are building an array, we can add the value
				static_cast<asmcontextnode_StructBuild*>(structVal)->AddValue(key, value);
			}
			else {
				asmcontextnode_MultOp* node = new asmcontextnode_MultOp("addtostruct", false);

				node->AddParam(context.PopASMCNode());
				node->AddParam(key);
				node->AddParam(value);

				context.PushASMCNode(node);
			}
		}

		return 0;
	}
};

class opcodeinfo_CastFieldObject : public opcodeinfo {
public:
	opcodeinfo_CastFieldObject(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
		}

		return 0;
	}
};

class opcodeinfo_PreScriptCall : public opcodeinfo {
public:
	opcodeinfo_PreScriptCall(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_Value<LPCCH>("<emptypos_prescriptcall>", TYPE_PRECODEPOS));
		}

		return 0;
	}
};

class opcodeinfo_End : public opcodeinfo {
public:
	bool m_isReturn;
	opcodeinfo_End(opcode id, LPCCH name, bool isReturn) : opcodeinfo(id, name), m_isReturn(isReturn) {
	}
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (m_isReturn) {
				auto* ref = new asmcontextnode_Op1("return ", true, context.PopASMCNode());
				// convert it to statement
				ref->m_priority = PRIORITY_INST;
				ref->m_type = TYPE_RETURN;
				context.PushASMCNode(ref);
			} else {
				// special node to print end ref
				auto* ref = new asmcontextnode_Value<LPCCH>("<END>");
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
};

class opcodeinfo_Statement : public opcodeinfo {
	LPCCH m_operatorName;
public:
	opcodeinfo_Statement(opcode id, LPCCH name, LPCCH operatorName) : opcodeinfo(id, name),
		m_operatorName(operatorName) {
	}
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* ref = new asmcontextnode_Value<LPCCH>(m_operatorName);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			ref->m_type = TYPE_STATEMENT;
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
};

class opcodeinfo_FunctionOperator : public opcodeinfo {
	LPCCH m_operatorName;
	bool m_hasSelf;
	bool m_complete;
	bool m_canHaveArg2;
public:
	opcodeinfo_FunctionOperator(opcode id, LPCCH name, LPCCH operatorName, bool hasSelf = false, bool complete = true, bool canHaveArg2 = false) : opcodeinfo(id, name),
		m_operatorName(operatorName), m_hasSelf(hasSelf), m_complete(complete), m_canHaveArg2(canHaveArg2) {
	}
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			asmcontextnode_MultOp* node = new asmcontextnode_MultOp(m_operatorName, m_hasSelf);
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
};


class opcodeinfo_Notify : public opcodeinfo {
public:
	opcodeinfo_Notify(opcode id, LPCCH name) : opcodeinfo(id, name){
	}
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("notify", true);
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
};

class opcodeinfo_FuncCall : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
				flags |= THREADENDON_CALL;
				break;
			case OPCODE_ScriptMethodThreadCallEndOn:
				flags |= THREADENDON_CALL | SELF_CALL;
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

			auto* ptr = new asmcontextnode_CallFuncPtr(FUNCTION_CALL, flags);

			// this

			if (flags & SELF_CALL) {
				ptr->AddParam(context.PopASMCNode());
			}

			ptr->AddParam(new asmcontextnode_FuncRef("", data[0], nsp));

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
};

class opcodeinfo_FuncGet : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
			context.PushASMCNode(new asmcontextnode_FuncRef("&", data[0], nsp));
		}

		return 0;
	}
};

class opcodeinfo_FuncClassCall : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
				flags |= THREADENDON_CALL;
			}

			if (!flags && caller->m_type == TYPE_NEW && function == constructorName) {
				// calling new constructor
				auto* newNode = static_cast<asmcontextnode_New*>(caller);

				auto* ptr = new asmcontextnode_CallFuncPtr(PARAMS_CALL, 0);

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

				auto* ptr = new asmcontextnode_CallFuncPtr(CLASS_CALL, flags);

				// this
				ptr->AddParam(caller);
				ptr->AddParam(new asmcontextnode_Identifier(function, "function"));
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
};

class opcodeinfo_FuncCallPtr : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
				flags |= THREADENDON_CALL;
				break;
			case OPCODE_ScriptMethodThreadCallPointerEndOn:
				flags |= THREADENDON_CALL | SELF_CALL;
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

			auto* ptr = new asmcontextnode_CallFuncPtr(POINTER_CALL, flags);

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
};

class opcodeinfo_GetString : public opcodeinfo {
public:
	opcodeinfo_GetString() : opcodeinfo(OPCODE_GetString, "GetString") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<INT32>();

		UINT32 ref = *(UINT32*)base;

		LPCCH str = objctx.GetStringValue(ref);

		base += 4;

		if (str) {
			out << "\"" << str << "\"\n";
			if (context.m_runDecompiler) {
				context.PushASMCNode(new asmcontextnode_String(str));
			}
		}
		else {
			// probably only dev blocks
			out << "bad str ref: 0x" << std::hex << ref << "\n";
			if (context.m_runDecompiler) {
				context.PushASMCNode(new asmcontextnode_String("<unknown string>"));
			}
		}


		return 0;
	}
};

class opcodeinfo_GetGlobal : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<UINT16>();

		UINT16 ref = *(UINT16*)base;

		UINT32 name = objctx.GetGlobalVarName(ref);

		base += 2;

		if (name) {
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}
		else {
			out << "bad gvar ref: 0x" << std::hex << ref << "\n";
		}

		if (context.m_runDecompiler) {
			if (m_id == OPCODE_GetGlobal) {
				context.PushASMCNode(new asmcontextnode_Identifier(name));
			}
			else if (m_id == OPCODE_GetGlobalObject) {
				context.SetObjectIdASMCNode(new asmcontextnode_Identifier(name));
			}
		}

		return 0;
	}
};

class opcodeinfo_GetLocalVar : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int) *(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			out << "bad lvar ref: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			UINT32 name = context.m_localvars[lvar].name;
			out << hashutils::ExtractTmp("var", name) << std::endl;
			context.m_localvars_ref[name]++;
		}

		return 0;
	}
};

class opcodeinfo_Switch : public opcodeinfo {
public:
	opcodeinfo_Switch() : opcodeinfo(OPCODE_Switch, "Switch") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// size:  8+16*cases,
		// align: 4 + 4 + (8 + 8)*cases,
		// logic uint32 deltaSwitchTable;uint32 cases;(uint64 value;uint64 delta)[cases]
		// move to switch table using deltaSwitchTable;pop value;search case using case; if found move using delta

		auto& baseTable = context.Aligned<INT32>();
		INT32 table = *(INT32*)baseTable;
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = context.Aligned<INT32>();

		INT32 cases = *(INT32*)baseCases;

		baseCases += 4;

		out << "table: ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) 
			<< context.FunctionRelativeLocation(utils::Aligned<INT64>(baseTable)) << " cases: " << cases << "\n";

		asmcontextnode_SwitchPreCompute* node = nullptr;
		if (context.m_runDecompiler) {
			node = new asmcontextnode_SwitchPreCompute(context.PopASMCNode());
		}

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = context.Aligned<INT64>();

			context.WritePadding(out);

			INT64 caseValue = *(INT64*)baseCaseValue;
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
						node->m_cases.push_back({ new asmcontextnode_Hash(caseValue), caseRLoc });
					}
				}
				else {
					out << std::dec << caseValue;
					if (node) {
						node->m_cases.push_back({new asmcontextnode_Value<INT64>(caseValue), caseRLoc });
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
};

class opcodeinfo_EndSwitch : public opcodeinfo {
public:
	opcodeinfo_EndSwitch() : opcodeinfo(OPCODE_EndSwitch, "EndSwitch") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		INT32 jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& baseCount = context.Aligned<INT32>();

		INT32 count = *(INT32*)baseCount;

		baseCount += 4;

		auto& ptrBase = context.Aligned<INT64>();

		ptrBase += 16 * count;

		auto rloc = context.FunctionRelativeLocation();

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_JumpOperator("endswitch", nullptr, rloc, TYPE_JUMP_ENDSWITCH, jumpLocation));
			context.CompleteStatement();
		}

		out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << std::hex << rloc << "\n";

		return 0;
	}
};

class opcodeinfo_pushopn : public opcodeinfo {
public:
	INT m_n;
	LPCCH m_op;
	asmcontextnode_priority m_priority;
	bool m_caller;
	bool m_forceFunc;
	opcodeinfo_pushopn(opcode id, LPCCH name, INT n, LPCCH op, asmcontextnode_priority priority, bool caller = false, bool forceFunc = false) : opcodeinfo(id, name),
		m_n(n), m_op(op), m_priority(priority), m_caller(caller), m_forceFunc(forceFunc) {
	}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (m_n == 1 && !m_forceFunc) {
				context.PushASMCNode(new asmcontextnode_Op1(m_op, m_caller, context.PopASMCNode()));
			}
			else if (m_n == 2 && !m_caller && !m_forceFunc) {
				// reverse order
				asmcontextnode* op2 = context.PopASMCNode();
				asmcontextnode* op1 = context.PopASMCNode();

				context.PushASMCNode(new asmcontextnode_Op2(m_op, m_priority, op1, op2));
			}
			else {
				asmcontextnode_MultOp* node = new asmcontextnode_MultOp(m_op, m_caller);

				for (size_t i = 0; i < m_n; i++) {
					node->AddParam(context.PopASMCNode());
				}

				context.PushASMCNode(node);
			}
		}
		out << "\n";
		return 0;
	}
};

class opcodeinfo_dec : public opcodeinfo {
public:
	opcodeinfo_dec(opcode id, LPCCH name) : opcodeinfo(id, name) {
	}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {

			if (context.m_stack.size()) {
				auto* peek = context.PeekASMCNode();
				if (peek->m_type == TYPE_NEW) {
					auto* newNode = static_cast<asmcontextnode_New*>(peek);
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
};

class opcodeinfo_Count : public opcodeinfo {
public:
	LPCCH m_op;
	bool m_pushReturn;
	opcodeinfo_Count(opcode id, LPCCH name, LPCCH op, bool pushReturn) : opcodeinfo(id, name), m_op(op), m_pushReturn(pushReturn) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		BYTE count = *(context.m_bcl++);

		out << "count:" << (int)count << "\n";

		if (context.m_runDecompiler) {
			asmcontextnode_MultOp* node = new asmcontextnode_MultOp(m_op, true);

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
};

class opcodeinfo_nop : public opcodeinfo {
public:
	using opcodeinfo::opcodeinfo;
};

// T8-Compiler custom opcode
class opcodeinfo_T8C_GetLazyFunction : public opcodeinfo {
public:
	opcodeinfo_T8C_GetLazyFunction() : opcodeinfo(OPCODE_T8C_GetLazyFunction, "T8C_GetLazyFunction") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<INT32>();

		UINT32 nsp = *(UINT32*)base;
		UINT32 function = *(UINT32*)(base + 4);
		UINT64 script = *(UINT64*)(base + 8);

		base += 16;

		out << "@" << hashutils::ExtractTmp("namespace", nsp)
			<< "<" << std::flush << hashutils::ExtractTmp("script", script)
			<< ">::" << std::flush << hashutils::ExtractTmp("function", function) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_FuncRef("@", function, nsp, script));
		}
		return 0;
	}
};


class opcodeinfo_DeltaVal : public opcodeinfo {
public:
	LPCCH m_op;
	opcodeinfo_DeltaVal(opcode id, LPCCH name, LPCCH op) : opcodeinfo(id, name), m_op(op) {
	}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << m_op << "\n";

		if (context.m_runDecompiler) {
			if (context.m_fieldId) {
				context.PushASMCNode(new asmcontextnode_Op1(m_op, false, context.GetFieldIdASMCNode(), TYPE_DELTA));
			}
			else {
				context.PushASMCNode(new asmcontextnode_Op1(m_op, false, context.PopASMCNode(), TYPE_DELTA));
			}
			context.CompleteStatement();
		}

		return 0;
	}
};
#pragma endregion
#pragma region opcode_registry

static const opcodeinfo* g_unknownOpcode = new opcodeinfo_unknown(OPCODE_Undefined, "Undefined");

void tool::gsc::opcode::RegisterVM(BYTE vm, LPCCH name) {
	g_opcodeMap[vm] = { vm, name, {} };
}
void tool::gsc::opcode::RegisterOpCode(BYTE vm, opcode enumValue, UINT16 op) {
	auto ref = g_opcodeMap.find(vm);
	if (ref == g_opcodeMap.end()) {
		assert(0);
		std::cerr << "Registering unknown OPCODE vm 0x" << std::hex << (int)vm << "\n";
		return;
	}

	ref->second.opcodemap[op] = enumValue;
}

void tool::gsc::opcode::RegisterOpCodes() {
	static std::once_flag f{};

	std::call_once(f, [] {
		// unknown
		RegisterOpCodeHandler(g_unknownOpcode);

		// nop operator
		// we don't care about the delete, it'll will stay until the end of the process,
		// todo: convert to object with destructor
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown0, "Unknown0", "operator_Unknown0()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown1, "Unknown1", "operator_Unknown1()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown2, "Unknown2", "operator_Unknown2()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown3, "Unknown3", "operator_Unknown3()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown4, "Unknown4", "operator_Unknown4()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown5, "Unknown5", "operator_Unknown5()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown6, "Unknown6", "operator_Unknown6()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown7, "Unknown7", "operator_Unknown7()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknowna, "Unknowna", "operator_Unknowna()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknownb, "Unknownb", "operator_Unknownb()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Nop, "Nop", "Nop()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown38, "Unknown38", "operator_Unknown38()"));

		// all op without params
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_ProfileStart, "ProfileStart", "profilestart()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_ProfileStop, "ProfileStop", "profilestop()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown10e, "Unknown10e", "operator_Unknown10e()"));
		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_Unknown126, "Unknown126", "operator_Unknown126()"));

		// end
		RegisterOpCodeHandler(new opcodeinfo_End(OPCODE_End, "End", false));
		RegisterOpCodeHandler(new opcodeinfo_End(OPCODE_Return, "Return", true));

		// switch
		RegisterOpCodeHandler(new opcodeinfo_Switch());
		RegisterOpCodeHandler(new opcodeinfo_EndSwitch());
		// dev/jump
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_DevblockBegin, "DevblockBegin"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_Jump, "Jump"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnTrue, "JumpOnTrue"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnGreaterThan, "JumpOnGreaterThan"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnFalse, "JumpOnFalse"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnLessThan, "JumpOnLessThan"));
		RegisterOpCodeHandler(new opcodeinfo_JumpExpr(OPCODE_JumpOnFalseExpr, "JumpOnFalseExpr"));
		RegisterOpCodeHandler(new opcodeinfo_JumpExpr(OPCODE_JumpOnTrueExpr, "JumpOnTrueExpr"));

		// ukn jump
		RegisterOpCodeHandler(new opcodeinfo_JumpPush());


		RegisterOpCodeHandler(new opcodeinfo_EvalArray(OPCODE_EvalArrayRef, "EvalArrayRef", false));
		RegisterOpCodeHandler(new opcodeinfo_EvalArray(OPCODE_EvalArray, "EvalArray", true));
		RegisterOpCodeHandler(new opcodeinfo_CreateArray(OPCODE_CreateArray, "CreateArray"));
		RegisterOpCodeHandler(new opcodeinfo_AddToArray(OPCODE_AddToArray, "AddToArray"));
		RegisterOpCodeHandler(new opcodeinfo_CreateStruct(OPCODE_CreateStruct, "CreateStruct"));
		RegisterOpCodeHandler(new opcodeinfo_AddToStruct(OPCODE_AddToStruct, "AddToStruct"));
		RegisterOpCodeHandler(new opcodeinfo_CastFieldObject(OPCODE_CastFieldObject, "CastFieldObject"));
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_Unknown35, "Unknown35"));
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_Unknown9e, "Unknown9e"));

		// ref
		RegisterOpCodeHandler(new opcodeinfo_SetVariableField(OPCODE_SetVariableField, "SetVariableField"));
		RegisterOpCodeHandler(new opcodeinfo_SetVariableFieldRef(OPCODE_SetVariableFieldRef, "SetVariableFieldRef"));
		RegisterOpCodeHandler(new opcodeinfo_ClearFieldVariable(OPCODE_ClearFieldVariable, "ClearFieldVariable", false));
		RegisterOpCodeHandler(new opcodeinfo_ClearFieldVariable(OPCODE_ClearFieldVariableOnStack, "ClearFieldVariableOnStack", true));
		RegisterOpCodeHandler(new opcodeinfo_EvalSelfFieldVariable(OPCODE_EvalSelfFieldVariable, "EvalSelfFieldVariable"));
		RegisterOpCodeHandler(new opcodeinfo_EvalGlobalObjectFieldVariable());
		RegisterOpCodeHandler(new opcodeinfo_CastAndEvalFieldVariable(OPCODE_CastAndEvalFieldVariable, "CastAndEvalFieldVariable"));
		RegisterOpCodeHandler(new opcodeinfo_EvalFieldVariable(OPCODE_EvalFieldVariable, "EvalFieldVariable", false, false));
		RegisterOpCodeHandler(new opcodeinfo_EvalFieldVariable(OPCODE_EvalFieldVariableRef, "EvalFieldVariableRef", false, true));
		RegisterOpCodeHandler(new opcodeinfo_EvalFieldVariable(OPCODE_EvalFieldVariableOnStack, "EvalFieldVariableOnStack", true, false));
		RegisterOpCodeHandler(new opcodeinfo_EvalFieldVariable(OPCODE_EvalFieldVariableOnStackRef, "EvalFieldVariableOnStackRef", true, true));

		// localvar related
		RegisterOpCodeHandler(new opcodeinfo_CheckClearParams());
		RegisterOpCodeHandler(new opcodeinfo_SafeCreateLocalVariables());
		RegisterOpCodeHandler(new opcodeinfo_EvalLocalVariableCached(OPCODE_EvalLocalVariableCached, "EvalLocalVariableCached"));
		RegisterOpCodeHandler(new opcodeinfo_EvalLocalVariableCached(OPCODE_EvalLocalVariableCachedSafe, "EvalLocalVariableCachedSafe"));
		RegisterOpCodeHandler(new opcodeinfo_SetLocalVariableCached(OPCODE_SetLocalVariableCachedOnStack, "SetLocalVariableCachedOnStack", true));
		RegisterOpCodeHandler(new opcodeinfo_SetLocalVariableCached(OPCODE_SetLocalVariableCached, "SetLocalVariableCached", false));
		RegisterOpCodeHandler(new opcodeinfo_FirstArrayKey(OPCODE_FirstArrayKey, "FirstArrayKey", true));
		RegisterOpCodeHandler(new opcodeinfo_FirstArrayKey(OPCODE_FirstArrayKeyCached, "FirstArrayKeyCached", false));
		RegisterOpCodeHandler(new opcodeinfo_SetNextArrayKeyCached(OPCODE_SetNextArrayKeyCached, "SetNextArrayKeyCached"));
		RegisterOpCodeHandler(new opcodeinfo_EvalLocalVariableRefCached(OPCODE_EvalLocalVariableRefCached, "EvalLocalVariableRefCached"));
		RegisterOpCodeHandler(new opcodeinfo_EvalLocalVariableDefined(OPCODE_EvalLocalVariableDefined, "EvalLocalVariableDefined"));
		RegisterOpCodeHandler(new opcodeinfo_EvalFieldObjectFromRef(OPCODE_EvalFieldObjectFromRef, "EvalFieldObjectFromRef"));
		  
		// idc
		RegisterOpCodeHandler(new opcodeinfo_Name(OPCODE_EvalLocalVariableCachedDebug, "EvalLocalVariableCachedDebug", "var"));
		RegisterOpCodeHandler(new opcodeinfo_Name(OPCODE_EvalLocalVariableRefCachedDebug, "EvalLocalVariableRefCachedDebug", "var"));
		RegisterOpCodeHandler(new opcodeinfo_GetLocalVar(OPCODE_Unknownc7, "Unknownc7"));

		// ref op
		RegisterOpCodeHandler(new opcodeinfo_DeltaVal(OPCODE_Dec, "Dec", "--"));
		RegisterOpCodeHandler(new opcodeinfo_DeltaVal(OPCODE_Inc, "Inc", "++"));

		// control
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_EndOnCallback, "EndOnCallback", "endoncallback", false)); // count = params + self
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_EndOn, "EndOn", "endon", false)); // count = params + self

		// ret control
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_WaitTill, "WaitTill", "waittill", true)); // count = params + self
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_WaitTillMatchTimeout, "WaitTillMatchTimeout", "waittillmatchtimeout", true));
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_WaitTillMatch, "WaitTillMatch", "waittillmatch", true)); // count = params + self
		RegisterOpCodeHandler(new opcodeinfo_Count(OPCODE_WaittillTimeout, "WaittillTimeout", "waittilltimeout", true)); // count = params + self

		// operation
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Bit_And, "Bit_And", 2, "&", PRIORITY_BIT_AND));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Bit_Or, "Bit_Or", 2, "|", PRIORITY_BIT_OR));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Bit_Xor, "Bit_Xor", 2, "^", PRIORITY_BIT_XOR));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_GreaterThan, "GreaterThan", 2, ">", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_LessThan, "LessThan", 2, "<", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_GreaterThanOrEqualTo, "GreaterThanOrEqualTo", 2, ">=", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_LessThanOrEqualTo, "LessThanOrEqualTo", 2, "<=", PRIORITY_COMPARE));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_ShiftRight, "ShiftRight", 2, ">>", PRIORITY_BIT_SHIFTS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_ShiftLeft, "ShiftLeft", 2, "<<", PRIORITY_BIT_SHIFTS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Plus, "Plus", 2, "+", PRIORITY_PLUS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Minus, "Minus", 2, "-", PRIORITY_PLUS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Divide, "Divide", 2, "/", PRIORITY_MULT));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Modulus, "Modulus", 2, "%", PRIORITY_MULT));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Multiply, "Multiply", 2, "*", PRIORITY_MULT));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_Equal, "Equal", 2, "==", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_SuperEqual, "SuperEqual", 2, "===", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_NotEqual, "NotEqual", 2, "!=", PRIORITY_EQUALS));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_SuperNotEqual, "SuperNotEqual", 2, "!==", PRIORITY_EQUALS));

		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_BoolComplement, "BoolComplement", 1, "~", PRIORITY_UNARY, true));
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_BoolNot, "BoolNot", 1, "!", PRIORITY_UNARY, true));
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_CastBool, "CastBool")); // we ignore this one
		RegisterOpCodeHandler(new opcodeinfo_pushopn(OPCODE_CastCanon, "CastCanon", 1, "", PRIORITY_UNARY, false, true)); // context dependent, the "" can be replaced to tag them

		// Remove return value from operator
		RegisterOpCodeHandler(new opcodeinfo_dec(OPCODE_DecTop, "DecTop"));
		RegisterOpCodeHandler(new opcodeinfo_dec(OPCODE_SafeDecTop, "SafeDecTop"));

		// gets
		RegisterOpCodeHandler(new opcodeinfo_GetConstant<LPCCH>(OPCODE_GetUndefined, "GetUndefined", "undefined"));
		RegisterOpCodeHandler(new opcodeinfo_GetConstant(OPCODE_GetSelf, "GetSelf", "self"));
		RegisterOpCodeHandler(new opcodeinfo_GetConstant(OPCODE_GetTime, "GetTime", "gettime()"));
		RegisterOpCodeHandler(new opcodeinfo_GetHash());
		RegisterOpCodeHandler(new opcodeinfo_GetConstant<INT32>(OPCODE_GetZero, "GetZero", 0));
		RegisterOpCodeHandler(new opcodeinfo_GetNeg<UINT16>(OPCODE_GetNegUnsignedShort, "GetNegUnsignedShort"));
		RegisterOpCodeHandler(new opcodeinfo_GetNeg<UINT8>(OPCODE_GetNegByte, "GetNegByte"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<BYTE>(OPCODE_GetByte, "GetByte"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<INT32>(OPCODE_GetInteger, "GetInteger"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<INT64>(OPCODE_GetLongInteger, "GetLongInteger"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<UINT32>(OPCODE_GetUnsignedInteger, "GetUnsignedInteger"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<UINT16>(OPCODE_GetUnsignedShort, "GetUnsignedShort"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<FLOAT, FLOAT>(OPCODE_GetFloat, "GetFloat"));
		RegisterOpCodeHandler(new opcodeinfo_GetNumber<uintptr_t, uintptr_t>(OPCODE_GetUIntPtr, "GetUIntPtr"));
		RegisterOpCodeHandler(new opcodeinfo_Vector());
		RegisterOpCodeHandler(new opcodeinfo_VectorConstant());
		RegisterOpCodeHandler(new opcodeinfo_GetVector());

		RegisterOpCodeHandler(new opcodeinfo_VectorScale(OPCODE_VectorScale, "VectorScale"));
		RegisterOpCodeHandler(new opcodeinfo_GetObjectSize(OPCODE_SizeOf, "SizeOf"));

		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd", "waittillframeend()"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_Wait, "Wait", "wait"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_WaitFrame, "WaitFrame", "waitframe"));
		RegisterOpCodeHandler(new opcodeinfo_Notify(OPCODE_Notify, "Notify"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_IsDefined, "IsDefined", "isdefined", false, false));

		// PRECODEPOS/CODEPOS on stack
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_ClearParams, "ClearParams"));
		RegisterOpCodeHandler(new opcodeinfo_PreScriptCall(OPCODE_PreScriptCall, "PreScriptCall"));

		RegisterOpCodeHandler(new opcodeinfo_GetConstant<LPCCH>(OPCODE_EmptyArray, "EmptyArray", "[]"));
		RegisterOpCodeHandler(new opcodeinfo_GetConstantSet<LPCCH>(OPCODE_ClearArray, "ClearArray", "[]", true));
		RegisterOpCodeHandler(new opcodeinfo_GetConstantRef(OPCODE_GetSelfObject, "GetSelfObject", "self"));

		// class stuff
		RegisterOpCodeHandler(new opcodeinfo_GetObjectType(OPCODE_GetObjectType, "GetObjectType", "class"));
		RegisterOpCodeHandler(new opcodeinfo_FuncClassCall(OPCODE_ClassFunctionCall, "ClassFunctionCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncClassCall(OPCODE_ClassFunctionThreadCall, "ClassFunctionThreadCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncClassCall(OPCODE_ClassFunctionThreadCallEndOn, "ClassFunctionThreadCallEndOn"));

		// functions
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptFunctionCall, "ScriptFunctionCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptThreadCallEndOn, "ScriptThreadCallEndOn"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptThreadCall, "ScriptThreadCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptMethodThreadCallEndOn, "ScriptMethodThreadCallEndOn"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptMethodCall, "ScriptMethodCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_ScriptMethodThreadCall, "ScriptMethodThreadCall"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_CallBuiltinFunction, "CallBuiltinFunction"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCall(OPCODE_CallBuiltinMethod, "CallBuiltinMethod"));

		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptThreadCallPointerEndOn, "ScriptThreadCallPointerEndOn"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptThreadCallPointer, "ScriptThreadCallPointer"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptMethodThreadCallPointer, "ScriptMethodThreadCallPointer"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptMethodThreadCallPointerEndOn, "ScriptMethodThreadCallPointerEndOn"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptFunctionCallPointer, "ScriptFunctionCallPointer"));
		RegisterOpCodeHandler(new opcodeinfo_FuncCallPtr(OPCODE_ScriptMethodCallPointer, "ScriptMethodCallPointer"));

		// linked ref
		RegisterOpCodeHandler(new opcodeinfo_FuncGet(OPCODE_GetResolveFunction, "GetResolveFunction"));
		RegisterOpCodeHandler(new opcodeinfo_FuncGet(OPCODE_GetFunction, "GetFunction"));
		RegisterOpCodeHandler(new opcodeinfo_GetString());
		RegisterOpCodeHandler(new opcodeinfo_GetGlobal(OPCODE_GetGlobal, "GetGlobal"));
		RegisterOpCodeHandler(new opcodeinfo_GetGlobal(OPCODE_GetGlobalObject, "GetGlobalObject"));

		// T8compiler custom opcode
		RegisterOpCodeHandler(new opcodeinfo_T8C_GetLazyFunction());

		RegisterOpCodesMap();
	});
}

void tool::gsc::opcode::RegisterOpCodeHandler(const opcodeinfo* info) {
	g_opcodeHandlerMap[info->m_id] = info;
}

bool tool::gsc::opcode::IsValidVm(BYTE vm, vminfo*& info) {
	// build map
	RegisterOpCodes();
	auto ref = g_opcodeMap.find(vm);
	if (ref == g_opcodeMap.end()) {
		return false;
	}
	info = &ref->second;

	return true;
}

const opcodeinfo* tool::gsc::opcode::LookupOpCode(BYTE vm, UINT16 opcode) {
	// build map
	RegisterOpCodes();

	vminfo* info;

	if (!(IsValidVm(vm, info))) {
		return g_unknownOpcode;
	}

	auto ref = info->opcodemap.find(opcode & 0xFFF);

	if (ref == info->opcodemap.end()) {
		return g_unknownOpcode;
	}

	const auto refHandler = g_opcodeHandlerMap.find(ref->second);

	if (refHandler == g_opcodeHandlerMap.end()) {
		return g_unknownOpcode;
	}

	return refHandler->second;
}

#pragma endregion
#pragma region asmctx 
asmcontext::asmcontext(BYTE* fonctionStart, const GscInfoOption& opt, UINT32 nsp, const T8GSCExport& exp, BYTE vm)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_opt(opt), m_runDecompiler(opt.m_dcomp), 
			m_lastOpCodeBase(-1), m_namespace(nsp), m_funcBlock(BLOCK_DEFAULT), m_exp(exp), m_vm(vm) {
	// set start as unhandled
	PushLocation();
}

asmcontext::~asmcontext() {
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

asmcontextlocation& asmcontext::PushLocation(BYTE* location) {
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
bool asmcontext::FindNextLocation() {
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

INT32 asmcontext::FunctionRelativeLocation(BYTE* bytecodeLocation) {
	return (INT32) (reinterpret_cast<uintptr_t>(bytecodeLocation) - reinterpret_cast<uintptr_t>(m_fonctionStart));
}

std::ostream& asmcontext::WritePadding(std::ostream& out) {
	return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << FunctionRelativeLocation() << ": "
		// no opcode write
		<< std::setfill(' ') << std::setw((sizeof(INT16) << 1) + 25 + 2) << " ";
}
UINT asmcontext::FinalSize() const {
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

int asmcontext::GetLocalVarIdByName(UINT32 name) const {
	for (size_t i = 0; i < m_localvars.size(); i++) {
		if (m_localvars[i].name == name) {
			return (int) i;
		}
	}

	return -1;
}
void asmcontext::PushASMCNode(asmcontextnode* node) {
	assert(node);
	m_stack.push_back(node);
}

void asmcontext::SetObjectIdASMCNode(asmcontextnode* node) {
	if (m_objectId) {
		delete m_objectId;
	}
	m_objectId = node;
}
void asmcontext::SetFieldIdASMCNode(asmcontextnode* node) {
	if (m_fieldId) {
		delete m_fieldId;
	}
	m_fieldId = node;
}

asmcontextnode* asmcontext::GetObjectIdASMCNode() {
	if (!m_objectId) {
		// create fake value for the decompiler
		m_objectId = new asmcontextnode_Value<LPCCH>("<error objectid>");
	}
	return m_objectId->Clone();
}

asmcontextnode* asmcontext::GetFieldIdASMCNode() {
	if (!m_fieldId) {
		// create fake value for the decompiler
		m_fieldId = new asmcontextnode_Value<LPCCH>("<error fieldid>");
	}
	return m_fieldId->Clone();
}

asmcontextnode* asmcontext::PopASMCNode() {
	if (!m_stack.size()) {
		// create fake value for the decompiler
		PushASMCNode(new asmcontextnode_Value<LPCCH>("<error pop>"));
	}
	asmcontextnode* val = m_stack[m_stack.size() - 1];
	m_stack.pop_back();
	return val;
}

asmcontextnode* asmcontext::PeekASMCNode() {
	if (!m_stack.size()) {
		// create fake value for the decompiler
		PushASMCNode(new asmcontextnode_Value<LPCCH>("<error pop>"));
	}
	return m_stack[m_stack.size() - 1];
}

void asmcontext::CompleteStatement() {
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
	}
}

#pragma endregion
#pragma region asm_block

std::ostream& decompcontext::WritePadding(std::ostream& out, bool forceNoRLoc) {
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

void asmcontextnodeblock::Dump(std::ostream& out, decompcontext& ctx) const {
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
				out << std::dec << " (ref:";

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

void asmcontext::ComputeDefaultParamValue() {
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

		auto* jump = static_cast<asmcontextnode_JumpOperator*>(param1);
		auto* set = static_cast<asmcontextnode_LeftRightOperator*>(it->node);
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

		auto* isDefinedFunc = static_cast<asmcontextnode_FunctionOperator*>(jump->m_operand);

		if (isDefinedFunc->m_operand->m_type != TYPE_IDENTIFIER || set->m_left->m_type != TYPE_IDENTIFIER || !set->m_right) {
			break; // not isdefined(param_name) or not param_name = ...
		}
		UINT32 name = static_cast<asmcontextnode_Identifier*>(set->m_left)->m_value;

		if (static_cast<asmcontextnode_Identifier*>(isDefinedFunc->m_operand)->m_value != name) {
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


int asmcontextnodeblock::ComputeDevBlocks(asmcontext& ctx) {
	auto it = m_statements.begin();
	while (it != m_statements.end()) {
		auto& b = *it;
		it->node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeDevBlocks(ctx);
		}, ctx);
		if (b.node->m_type != TYPE_JUMP_DEVBLOCK) {
			it++;
			continue;
		}

		// we are reading a devblock jump
		auto* jump = static_cast<asmcontextnode_JumpOperator*>(b.node);
		INT64 end = jump->m_location;
		asmcontextnodeblock* devBlock = new asmcontextnodeblock(BLOCK_DEV);
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
			devBlock->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_enddev>", TYPE_PRECODEPOS), it->location });
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
int asmcontextnodeblock::ComputeSwitchBlocks(asmcontext& ctx) {

	auto it = this->m_statements.begin();
	while (it != m_statements.end()) {
		auto& b = *it;
		it->node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeSwitchBlocks(ctx);
		}, ctx);
		if (b.node->m_type != TYPE_SWITCH_PRECOMPUTE) {
			// not a switch to compute, we pass to the next pointer
			it++;
			continue;
		}

		// we are reading a devblock jump
		auto* switchPreCompute = static_cast<asmcontextnode_SwitchPreCompute*>(b.node);
		INT64 end = switchPreCompute->m_endLocation;
		asmcontextnode_SwitchPostCompute* switchBlock = new asmcontextnode_SwitchPostCompute(switchPreCompute->m_node->Clone());
		// replace with the computed switch
		auto& cases = switchPreCompute->m_cases;

		// sort the cases by their locations, we know a default won't be reordered 
		// because it was parsed during the precompute node creation.
		std::sort(cases.begin(), cases.end(), [](const switchcase& e1, const switchcase& e2) {
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
				asmcontextnodeblock* block = new asmcontextnodeblock(BLOCK_PADDING);
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
						auto* jump = static_cast<asmcontextnode_JumpOperator*>(it->node);
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
			auto* jump = static_cast<asmcontextnode_JumpOperator*>(it->node);
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


		switchBlock->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeSwitchBlocks(ctx);
		}, ctx);

		// delete the precompute operator
		delete switchPreCompute;
	}

	return 0;
}

asmcontextnode* JumpCondition(asmcontextnode_JumpOperator* op, bool reversed) {
	switch (op->m_type) {
	case TYPE_JUMP_GREATERTHAN:
	case TYPE_JUMP_LOWERTHAN:
	{
		auto* clo = static_cast<asmcontextnode_Op2*>(op->m_operand->Clone());
		if (reversed) {
			clo->m_description = op->m_type == TYPE_JUMP_GREATERTHAN ? "<" : ">";
		}
		return clo;
	}
	case TYPE_JUMP_ONTRUE:
	case TYPE_JUMP_ONTRUEEXPR:
		if (reversed) {
			return new asmcontextnode_Op1("!", true, op->m_operand->Clone());
		}
		return op->m_operand->Clone();

	case TYPE_JUMP_ONFALSE:
	case TYPE_JUMP_ONFALSEEXPR:
		if (reversed) {
			return op->m_operand->Clone();
		}
		return new asmcontextnode_Op1("!", true, op->m_operand->Clone());
	default:
		return nullptr;
	}
}

int asmcontextnodeblock::ComputeForEachBlocks(asmcontext& ctx) {
	/*
	// INIT STEP
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
	*/

	size_t index = 0;

	while (index < m_statements.size()) {
		auto& arrayRefLoc = m_statements[index];
		arrayRefLoc.node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
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

		auto* setOp = static_cast<asmcontextnode_LeftRightOperator*>(arrayRefLoc.node);

		if (setOp->m_left->m_type != TYPE_IDENTIFIER) {
			index += moveDelta;
			continue;
		}


		// array ref name
		UINT32 arrayRefName = static_cast<asmcontextnode_Identifier*>(setOp->m_left)->m_value;

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
		auto* setFirstArrayOp = static_cast<asmcontextnode_LeftRightOperator*>(firstArrayLoc.node);

		if (setFirstArrayOp->m_left->m_type != TYPE_IDENTIFIER || setFirstArrayOp->m_right->m_type != TYPE_ARRAY_FIRSTKEY) {
			index += moveDelta;
			continue; // not key = firstarray(...)
		}

		UINT32 keyValName = static_cast<asmcontextnode_Identifier*>(setFirstArrayOp->m_left)->m_value;

		/*
		LOC_000000fc:
			LOC_00000100:jumpiffalse(isdefined(key)) LOC_000002f0;
			doorblocker = var_f15d3f7b[key];
			nextarray(var_f15d3f7b, var_7cbeb6de)
		*/

		index++;
		moveDelta--;


		if (index + 3 >= m_statements.size()) {
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

		auto* jumpOp = static_cast<asmcontextnode_JumpOperator*>(condJump.node);

		auto& itemSetStmt = m_statements[index];

		if (itemSetStmt.node->m_type != TYPE_SET) {
			index += moveDelta;
			continue;
		}

		index++;
		moveDelta--;
		auto& nextArrayStmt = m_statements[index];

		if (nextArrayStmt.node->m_type != TYPE_ARRAY_NEXTKEY) {
			index += moveDelta;
			continue;
		}
		auto* setKeyArrayOp = static_cast<asmcontextnode_LeftRightOperator*>(itemSetStmt.node);

		if (setKeyArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
			index += moveDelta;
			continue; // not key = firstarray(...)
		}

		UINT32 itemValName = static_cast<asmcontextnode_Identifier*>(setKeyArrayOp->m_left)->m_value;

		index++;
		moveDelta--;

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
			|| (static_cast<asmcontextnode_JumpOperator*>(jumpBackStmt.node)->m_location != condLoc
				&& static_cast<asmcontextnode_JumpOperator*>(jumpBackStmt.node)->m_location != endLoc))) {
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

			auto* j = static_cast<asmcontextnode_JumpOperator*>(stmt.node);
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
		auto* block = new asmcontextnodeblock();

		// remove the number of references for the key because maybe we don't use it
		auto& keyRef = ctx.m_localvars_ref[keyValName];
		keyRef = max(keyRef - 5, 0);

		auto* forEachNode = new asmcontextnode_ForEach(setOp->m_right->Clone(), block, ctx.m_localvars_ref[keyValName] ? keyValName : 0, itemValName);

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
		block->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_foreachcontinue>", TYPE_PRECODEPOS), it->location });
		for (size_t i = incrementIndex; i < endNodeIndex; i++) {
			// remove component statement
			delete it->node;
			it = m_statements.erase(it);
		}

		forEachNode->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeForEachBlocks(ctx);
		}, ctx);
		// go back to the first index
		index = startIndex;
	}

	return 0;
}

int asmcontextnodeblock::ComputeWhileBlocks(asmcontext& ctx) {
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
		jumpStmtVal.node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeWhileBlocks(ctx);
		}, ctx);
		if (!IsJumpType(jumpStmtVal.node->m_type)) {
			itindex++;
			continue; // not a jump
		}
		auto* jumpOp = static_cast<asmcontextnode_JumpOperator*>(jumpStmtVal.node);
		if (jumpOp->m_delta >= 0) {
			itindex++;
			continue; // not a back jump
		}

		// we search the last ref to the start, index element can be a continue
		auto index = itindex;
		for (size_t i = itindex + 1; i < m_statements.size(); i++) {
			if (IsJumpType(m_statements[i].node->m_type)) {
				auto* jumpOp2 = static_cast<asmcontextnode_JumpOperator*>(m_statements[i].node);
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
			asmcontextnode* cond = nullptr;

			auto it = m_statements.begin() + startIndex;

			if (IsJumpType(firstNode.node->m_type)) {
				// clone and erase the condition
				// while/for
				auto* jumpCondNode = static_cast<asmcontextnode_JumpOperator*>(firstNode.node);
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

			auto* block = new asmcontextnodeblock();
			auto* node = new asmcontextnode_While(cond, block);
			for (size_t i = startIndex; i < index; i++) {
				auto* ref = it->node;

				asmcontextnode* replaceNode = nullptr;

				if (IsJumpType(ref->m_type)) {
					// replace jumps
					auto* j = static_cast<asmcontextnode_JumpOperator*>(ref);
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
					ref->ApplySubBlocks([=](asmcontextnodeblock* block, asmcontext& context) {
						for (const auto& stmt : block->m_statements) {
							auto* refjump = stmt.node;
							if (IsJumpType(refjump->m_type)) {
								// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
								//and do not remove the ref because it's nested breaks/continues
								auto* j = static_cast<asmcontextnode_JumpOperator*>(refjump);
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
			block->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_inf_end>", TYPE_PRECODEPOS), it->location });
			// it = jump
			firstNodeLocation->RemoveRef(jumpOp->m_opLoc);
			it->location = firstNodeLocation;
			delete it->node;
			it->node = node;
			node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
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

			auto* newBlock = new asmcontextnodeblock();
			auto* doWhile = new asmcontextnode_DoWhile(cond, newBlock);

			auto it = m_statements.begin() + startIndex;
			for (size_t i = startIndex; i < index; i++) {
				auto& stmt = m_statements[startIndex];

				asmcontextnode* replaceNode = nullptr;

				// compute special jump
				if (IsJumpType(stmt.node->m_type)) {
					auto* j = static_cast<asmcontextnode_JumpOperator*>(stmt.node);
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
					stmt.node->ApplySubBlocks([=](asmcontextnodeblock* block, asmcontext& context) {
						for (const auto& stmtjump : block->m_statements) {
							auto* refjump = stmtjump.node;
							if (IsJumpType(refjump->m_type)) {
								// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
								//and do not remove the ref because it's nested breaks/continues
								auto* j = static_cast<asmcontextnode_JumpOperator*>(refjump);
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
			newBlock->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_whileend>", TYPE_PRECODEPOS), m_statements[itindex].location });
			delete m_statements[itindex].node;
			m_statements[itindex].node = doWhile;
			m_statements[itindex].location = doWhileLoc;

			doWhile->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
				block->ComputeWhileBlocks(ctx);
			}, ctx);
		}
	}
	return 0;
}

int asmcontextnodeblock::ComputeIfBlocks(asmcontext& ctx) {
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
		jumpStmt.node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeIfBlocks(ctx);
		}, ctx);

		if (!IsJumpType(jumpStmt.node->m_type)) {
			index++;
			continue; // not a jump for an if
		}
		auto* jumpOp = static_cast<asmcontextnode_JumpOperator*>(jumpStmt.node);
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

		asmcontextnodeblock* blockIf = new asmcontextnodeblock();
		asmcontextnodeblock* blockElse = nullptr;
		bool ignoreLast = false;

		// remove if ref
		elsePartNodeLoc->RemoveRef(jumpOp->m_opLoc);

		// check +2 to avoid converting a if (...) { continue; } into an empty if (...) {} else {...}
		if (endIndex > index + 2 && m_statements[endIndex - 1].node->m_type == TYPE_JUMP) {
			auto* jumpOpEndElse = static_cast<asmcontextnode_JumpOperator*>(m_statements[endIndex - 1].node);
			// after else goto
			if (jumpOpEndElse->m_delta > 0) {
				auto endElseLoc = jumpOpEndElse->m_location;
				// should be positive

				auto endElseIndex = endIndex;
				while (endElseIndex < m_statements.size() && m_statements[endElseIndex].location->rloc < endElseLoc) {
					endElseIndex++;
				}

				if (endElseIndex < m_statements.size() && m_statements[endElseIndex].location->rloc == endElseLoc) {
					blockElse = new asmcontextnodeblock();

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
					blockElse->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_elselast>", TYPE_PRECODEPOS), endElseFinalLoc });
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
			blockIf->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_ifend1>", TYPE_PRECODEPOS), it->location });
			delete it->node;
			it = m_statements.erase(it);
		}
		else {
			blockIf->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<emptypos_ifend2>", TYPE_PRECODEPOS), elsePartNodeLoc });
		}

		// swap the jump with the new if statement
		assert(IsJumpType(m_statements[index].node->m_type) && m_statements[index].node->m_type != TYPE_JUMP);
		auto* ifElse = new asmcontextnode_IfElse(JumpCondition(static_cast<asmcontextnode_JumpOperator*>(m_statements[index].node), true), blockIf, blockElse);
		delete m_statements[index].node;
		m_statements[index].node = ifElse;
		ifElse->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
			block->ComputeIfBlocks(ctx);
		}, ctx);
	}

	// LOC_0000015c:jumpiffalse(!self function_a39f313c() || self getcurrentweapon() == level.weaponnone) LOC_0000017c delta: 0x1c (type:9/spec:false)

	return 0;
}

int asmcontextnodeblock::ComputeForBlocks(asmcontext& ctx) {
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
		whileStmt.node->ApplySubBlocks([](asmcontextnodeblock* block, asmcontext& ctx) {
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

		auto* setOp = static_cast<asmcontextnode_LeftRightOperator*>(initValNode);

		if (setOp->m_left->m_type != TYPE_IDENTIFIER) {
			index++;
			continue; // not an I = ...
		}

		UINT32 incrementName = static_cast<asmcontextnode_Identifier*>(setOp->m_left)->m_value;

		auto* whileBlock = static_cast<asmcontextnode_While*>(whileStmt.node);

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
		auto* deltaPart = static_cast<asmcontextnode_Op1*>(deltaStmt.node);

		if (deltaPart->m_operand->m_type != TYPE_IDENTIFIER || static_cast<asmcontextnode_Identifier*>(deltaPart->m_operand)->m_value != incrementName) {
			index++;
			continue; // not an identifier delta
		}

		auto* forDeltaNode = new asmcontextnode_ForDelta(initValNode->Clone(), whileBlock->m_condition, deltaPart->Clone(), whileBlock->m_block);
		whileBlock->m_block = nullptr;
		whileBlock->m_condition = nullptr;
		delete whileStmt.node;
		whileStmt.node = forDeltaNode;


		delete initValStmt.node;
		initValStmt.node = new asmcontextnode_Value<LPCCH>("<for_init>", TYPE_PRECODEPOS);

		delete deltaStmt.node;
		deltaStmt.node = new asmcontextnode_Value<LPCCH>("<for_continue>", TYPE_PRECODEPOS);
		auto* continueLoc = deltaStmt.location;

		std::function<void(asmcontextnodeblock* block, asmcontext& ctx)> func;

		// continue
		func = [&func, continueLoc](asmcontextnodeblock* block, asmcontext& ctx) {
			for (auto& bs : block->m_statements) {
				if (!IsJumpType(bs.node->m_type)) {
					bs.node->ApplySubBlocks(func, ctx);
					continue;
				}

				auto* j = static_cast<asmcontextnode_JumpOperator*>(bs.node);
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
