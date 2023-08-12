#include <includes.hpp>
using namespace tool::gsc::opcode;

static std::unordered_map<UINT16, opcode> g_opcodeMap{};
static std::unordered_map<opcode, const opcodeinfo*> g_opcodeHandlerMap{};


opcodeinfo::opcodeinfo(opcode id, LPCCH name) : m_id(id), m_name(name) {
}


tool::gsc::opcode::asmcontextnode::asmcontextnode(asmcontextnode_priority priority, asmcontextnode_type type) : m_priority(priority), m_type(type) {
}

tool::gsc::opcode::asmcontextnode::~asmcontextnode() {
}


asmcontextnodeblock::asmcontextnodeblock(bool devBlock) : asmcontextnode(PRIORITY_INST, TYPE_BLOCK), m_devBlock(devBlock) {
}
asmcontextnodeblock::~asmcontextnodeblock() {
	for (auto& ref : m_statements) {
		delete ref.node;
	}
}
asmcontextnode* asmcontextnodeblock::Clone() const {
	asmcontextnodeblock* n = new asmcontextnodeblock(m_devBlock);
	for (auto& node : m_statements) {
		n->m_statements.push_back({ node.node->Clone(), node.location });
	}
	return n;
}

void tool::gsc::opcode::asmcontextnode::Dump(std::ostream& out, decompcontext& ctx) const {
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
			dynamic_cast<asmcontextnode_Hash*>(key)->m_canonid = true;
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
	asmcontextnode_Op1(LPCCH description, bool prefix, asmcontextnode* operand) :
		asmcontextnode(PRIORITY_UNARY), m_prefix(prefix), m_description(description), m_operand(operand) {
		assert(operand);
	}
	~asmcontextnode_Op1() {
		delete m_operand;
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_Op1(m_description, m_prefix, m_operand->Clone());
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
	asmcontextnode_MultOp(LPCCH description, bool caller) :
		asmcontextnode(PRIORITY_INST, TYPE_STATEMENT), m_description(description), m_caller(caller) {
	}
	~asmcontextnode_MultOp() {
		for (auto& ref : m_operands) {
			delete ref;
		}
	}

	asmcontextnode* Clone() const override {
		auto* ref = new asmcontextnode_MultOp(m_description, m_caller);
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
		asmcontextnode(PRIORITY_INST, type), m_operatorName(operatorName), m_self(self), m_operand(operand) {
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

class asmcontextnode_JumpOperator : public asmcontextnode {
public:
	LPCCH m_operatorName;
	asmcontextnode* m_operand;
	INT64 m_location;
	asmcontextnode_JumpOperator(LPCCH operatorName, asmcontextnode* operand, INT64 location, asmcontextnode_type type) :
		asmcontextnode(PRIORITY_INST, type), m_operatorName(operatorName), m_operand(operand), m_location(location) {
	}
	~asmcontextnode_JumpOperator() {
		if (m_operand) {
			delete m_operand;
		}
	}

	asmcontextnode* Clone() const override {
		return new asmcontextnode_JumpOperator(m_operatorName, m_operand ? m_operand->Clone() : nullptr, m_location, m_type);
	}

	void Dump(std::ostream& out, decompcontext& ctx) const override {
		out << m_operatorName;
		if (m_operand) {
			out << "(";
			m_operand->Dump(out, ctx);
			out << ")";
		}
		out << " LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << m_location;
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
class asmcontextnode_New: public asmcontextnode {
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

		context.m_localvars.reserve((size_t) count + 1);

		out << std::hex << "count: 0x" << (int)count << "\n";
		if (!context.m_localvars.size()) {
			// the local variables starts at 1
			context.m_localvars.insert(context.m_localvars.begin(), {hashutils::Hash32("<error>"), 0});
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

			out << std::endl;
		}

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
			context.PushASMCNode(new asmcontextnode_Value<LPCCH>("<precodepos>", TYPE_PRECODEPOS));
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
		asmcontextnode* node = nullptr;
		asmcontextnode_type type = TYPE_JUMP;
		bool pushBack = false;
		if (context.m_runDecompiler) {
			switch (m_id) {
			case OPCODE_JumpOnGreaterThan:
			case OPCODE_JumpOnLessThan:
			{
				// reversed
				asmcontextnode* op2 = context.PopASMCNode();
				asmcontextnode* op1 = context.PopASMCNode();
				node = new asmcontextnode_Op2(m_id == OPCODE_JumpOnGreaterThan ? ">" : "<", PRIORITY_COMPARE, op1, op2);
				type = TYPE_JUMP_ONTRUE;
			}
				break;
				// the only diff between JumpOnXXXX and JumpOnXXXXExpr is if we cast the result or not to bool,
				// also we keep on the stack the value during the jump
			case OPCODE_JumpOnFalseExpr:
				pushBack = true;
			case OPCODE_JumpOnFalse:
				// keep it like this for render, we'll see later
				node = new asmcontextnode_Op1("!", true, context.PopASMCNode());
				type = TYPE_JUMP_ONFALSE;
				break;
			case OPCODE_JumpOnTrueExpr:
				pushBack = true;
			case OPCODE_JumpOnTrue:
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONTRUE;
				break;
			case OPCODE_DevblockBegin:
				type = TYPE_JUMP_DEVBLOCK;
				break;
			}
		}

		auto& bytecode = context.Aligned<INT16>();

		INT16 delta = *(INT16*)bytecode;

		bytecode += 2;

		// push a location and mark it as referenced
		BYTE* newLoc = &context.m_bcl[delta];

		// push the node value during the jump if the operator is asking for it
		if (pushBack) {
			context.PushASMCNode(node);
		}
		auto& locref = context.PushLocation(newLoc);

		if (pushBack) {
			context.PopASMCNode();
		}

		if (context.m_runDecompiler) {
			if (node) {
				context.PushASMCNode(new asmcontextnode_JumpOperator("jumpif", node, locref.rloc, type));
			}
			else {
				context.PushASMCNode(new asmcontextnode_JumpOperator(m_id == OPCODE_Jump ? "jump" : m_id == OPCODE_DevblockBegin ? "jumpdev" : "jumpukn", nullptr, locref.rloc, type));
			}
			context.CompleteStatement();
		}
		
		locref.ref++;

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << locref.rloc << " (delta:0x" << delta << ")\n";

		return m_id == OPCODE_Jump ? -2 : 0; // no code after jump
	}
};

class opcodeinfo_JumpPush : public opcodeinfo {
public:
	opcodeinfo_JumpPush() : opcodeinfo(OPCODE_JumpPush, "JumpPush") {}

	int Dump(std::ostream& out, UINT16 value, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<uintptr_t>();

		uintptr_t location = *(uintptr_t*)bytecode;

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new asmcontextnode_JumpOperator("jumppush", nullptr, location, TYPE_STATEMENT));
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

			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("firstarray", false);

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
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			asmcontextnode* arrayNode = context.PopASMCNode();

			asmcontextnode_MultOp* node = new asmcontextnode_MultOp("nextarray", false);

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
				dynamic_cast<asmcontextnode_ArrayBuild*>(arrayVal)->AddValue(key, value);
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
				dynamic_cast<asmcontextnode_StructBuild*>(structVal)->AddValue(key, value);
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
			context.PushASMCNode(new asmcontextnode_Value<LPCCH>("<precodepos>", TYPE_PRECODEPOS));
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
				ref->m_type = TYPE_STATEMENT;
				context.PushASMCNode(ref);
			}
			context.CompleteStatement();
			if (!m_isReturn) {
				// special node to print end ref
				auto* ref = new asmcontextnode_Value<LPCCH>("<END>");
				ref->m_type = TYPE_END;
				context.PushASMCNode(ref);
				context.CompleteStatement();
			}
		}
		out << "\n";
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
				auto* newNode = dynamic_cast<asmcontextnode_New*>(caller);

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

			if (flags & SELF_CALL) {
				ptr->AddParam(context.PopASMCNode());
			}
			// use <= to add the function pointer
			for (size_t i = 0; i <= params; i++) {
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
			out << hashutils::ExtractTmp("var", context.m_localvars[lvar].name) << std::endl;
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

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = context.Aligned<INT64>();

			context.WritePadding(out);

			INT64 caseValue = *(INT64*)baseCaseValue;
			baseCaseValue += 8;
			auto& baseCaseDelta = context.Aligned<INT64>();
			INT64 caseDelta = *(INT64*)baseCaseDelta;
			baseCaseDelta += 8;

			if (c == cases && !caseValue) {
				out << "default";
			}
			else {
				out << "case ";
				if (caseValue >= 0x100000000LL) {
					// assume it's an hash after INT32 max value
					out << "#\"" << hashutils::ExtractTmp("hash", caseValue) << "\"" << std::flush;
				}
				else {
					out << std::dec << caseValue;
				}
			}

			out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1)
				<< context.PushLocation(&baseCaseDelta[caseDelta]).rloc << "\n";

			if (c == cases && !caseValue) {
				baseCaseDelta += caseDelta;
			}
			else {
				// align to the next opcode
				context.Aligned<UINT16>();
				// Write next field
				//context.WritePadding(out) << "Next ." << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << context.FunctionRelativeLocation() << "\n";
			}
		}

		context.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}
};

class opcodeinfo_EndSwitch : public opcodeinfo {
public:
	opcodeinfo_EndSwitch() : opcodeinfo(OPCODE_EndSwitch, "EndSwitch") {}

	int Dump(std::ostream& out, UINT16 v, asmcontext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& baseCount = context.Aligned<INT32>();

		INT32 count = *(INT32*)baseCount;

		baseCount += 4;

		auto& ptrBase = context.Aligned<INT64>();

		ptrBase += 16 * count;

		out << "End to 0x" << std::hex << context.FunctionRelativeLocation() << "\n";

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
					auto* newNode = dynamic_cast<asmcontextnode_New*>(peek);
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
				context.PushASMCNode(new asmcontextnode_Op1(m_op, false, context.GetFieldIdASMCNode()));
			}
			else {
				context.PushASMCNode(new asmcontextnode_Op1(m_op, false, context.PopASMCNode()));
			}
			context.CompleteStatement();
		}

		return 0;
	}
};

static const opcodeinfo* g_unknownOpcode = new opcodeinfo_unknown(OPCODE_Undefined, "Undefined");

void tool::gsc::opcode::RegisterOpCode(opcode enumValue, UINT16 op) {
	g_opcodeMap[op] = enumValue;
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
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnTrueExpr, "JumpOnTrueExpr"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnGreaterThan, "JumpOnGreaterThan"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnFalse, "JumpOnFalse"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnFalseExpr, "JumpOnFalseExpr"));
		RegisterOpCodeHandler(new opcodeinfo_Jump(OPCODE_JumpOnLessThan, "JumpOnLessThan"));

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

		RegisterOpCodeHandler(new opcodeinfo_Statement(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd", "waitframe 1"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_Wait, "Wait", "wait"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_WaitFrame, "WaitFrame", "waitframe"));
		RegisterOpCodeHandler(new opcodeinfo_Notify(OPCODE_Notify, "Notify"));
		RegisterOpCodeHandler(new opcodeinfo_FunctionOperator(OPCODE_IsDefined, "IsDefined", "isdefined", false, false));

		// PRECODEPOS/CODEPOS on stack
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_ClearParams, "ClearParams"));
		RegisterOpCodeHandler(new opcodeinfo_nop(OPCODE_CheckClearParams, "CheckClearParams"));
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

		// Register mapping
		// todo: create file containing it

		RegisterOpCode(OPCODE_Unknown0, 0x0);
		RegisterOpCode(OPCODE_Unknown1, 0x1);
		RegisterOpCode(OPCODE_Unknown2, 0x2);
		RegisterOpCode(OPCODE_Unknown3, 0x3);
		RegisterOpCode(OPCODE_Unknown4, 0x4);
		RegisterOpCode(OPCODE_Unknown5, 0x5);
		RegisterOpCode(OPCODE_Unknown6, 0x6);
		RegisterOpCode(OPCODE_Unknown7, 0x7);
		RegisterOpCode(OPCODE_Unknowna, 0xa);
		RegisterOpCode(OPCODE_Unknownb, 0xb);
		RegisterOpCode(OPCODE_Nop,
			0x12, 0x13, 0x14, 0x16, 0x18, 0x1a, 0x1e, 0x21, 0x23, 0x25, 0x27, 0x28, 0x2b,
			0x2c, 0x2d, 0x2f, 0x30, 0x3d, 0x3e, 0x3f, 0x41, 0x43, 0x48, 0x4f, 0x50, 0x52,
			0x53, 0x5b, 0x5e, 0x66, 0x67, 0x69, 0x6d, 0x73, 0x74, 0x75, 0x79, 0x7f, 0x80,
			0x83, 0x84, 0x86, 0x87, 0x88, 0x90, 0x97, 0x99, 0x9c, 0xa3, 0xa5, 0xa8, 0xaa,
			0xab, 0xac, 0xaf, 0xb2, 0xb3, 0xb6, 0xbd, 0xbf, 0xc3, 0xc6, 0xcb, 0xcd, 0xd1,
			0xd2, 0xd5, 0xd6, 0xd8, 0xdb, 0xde, 0xdf, 0xe0, 0xe2, 0xe5, 0xe8, 0xec, 0xee);
		RegisterOpCode(OPCODE_Nop,
			0xef, 0xf1, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xff, 0x103, 0x108, 0x109, 0x10b, 0x111,
			0x115, 0x11a, 0x121, 0x124, 0x127, 0x128, 0x12b, 0x12e, 0x12f, 0x131, 0x132, 0x135, 0x139,
			0x13f, 0x140, 0x143, 0x148, 0x14a, 0x14d, 0x14f, 0x153, 0x156, 0x158, 0x159, 0x15b, 0x15c,
			0x15d, 0x15e, 0x160, 0x161, 0x163, 0x166, 0x16b, 0x16c, 0x173, 0x174, 0x176, 0x17a, 0x17b,
			0x185, 0x188, 0x18b, 0x18c, 0x18d, 0x18f, 0x191, 0x195, 0x19b, 0x19c, 0x19e, 0x19f, 0x1a0,
			0x1a2, 0x1a4, 0x1a7, 0x1ad, 0x1ae, 0x1af, 0x1b3, 0x1b5, 0x1b7, 0x1b8, 0x1b9, 0x1bc, 0x1c3);
		RegisterOpCode(OPCODE_Nop,
			0x1c7, 0x1ca, 0x1cb, 0x1cc, 0x1d1, 0x1d5, 0x1dd, 0x1e0, 0x1e1, 0x1e5, 0x1e7, 0x1ea, 0x1ef,
			0x1fa, 0x1fc, 0x1fd, 0x203, 0x20b, 0x20c, 0x20e, 0x214, 0x215, 0x216, 0x218, 0x221, 0x225,
			0x226, 0x227, 0x229, 0x22c, 0x22e, 0x233, 0x234, 0x239, 0x23b, 0x23d, 0x240, 0x244, 0x245,
			0x247, 0x24b, 0x24d, 0x254, 0x256, 0x258, 0x259, 0x25b, 0x25e, 0x25f, 0x263, 0x266, 0x267,
			0x268, 0x26a, 0x26d, 0x271, 0x273, 0x277, 0x27a, 0x27b, 0x27f, 0x281, 0x282, 0x286, 0x289,
			0x28a, 0x292, 0x297, 0x298, 0x299, 0x29b, 0x29e, 0x2a0, 0x2a1, 0x2a4, 0x2ac, 0x2ae, 0x2b0);
		RegisterOpCode(OPCODE_Nop,
			0x2b2, 0x2b5, 0x2b8, 0x2ba, 0x2bb, 0x2c4, 0x2c6, 0x2c7, 0x2c8, 0x2c9, 0x2cb, 0x2cc, 0x2cd,
			0x2ce, 0x2d0, 0x2d1, 0x2d4, 0x2d5, 0x2d7, 0x2d8, 0x2da, 0x2e3, 0x2e4, 0x2e8, 0x2e9, 0x2ea,
			0x2ec, 0x2f0, 0x2f5, 0x2f9, 0x2fa, 0x2fd, 0x301, 0x302, 0x305, 0x306, 0x308, 0x309, 0x30d,
			0x310, 0x311, 0x313, 0x317, 0x318, 0x319, 0x31b, 0x31d, 0x31f, 0x322, 0x328, 0x329, 0x32a,
			0x32c, 0x32d, 0x32e, 0x336, 0x339, 0x33c, 0x33f, 0x343, 0x346, 0x348, 0x34f, 0x351, 0x352,
			0x353, 0x355, 0x362, 0x363, 0x367, 0x368, 0x369, 0x36a, 0x370, 0x373, 0x378, 0x37a, 0x37e);
		RegisterOpCode(OPCODE_Nop,
			0x37f, 0x382, 0x384, 0x387, 0x38b, 0x38d, 0x38e, 0x38f, 0x393, 0x398, 0x39a, 0x3a1, 0x3a3,
			0x3a4, 0x3a5, 0x3a6, 0x3a8, 0x3a9, 0x3aa, 0x3ac, 0x3ad, 0x3af, 0x3b4, 0x3bc, 0x3bf, 0x3c2,
			0x3c5, 0x3c7, 0x3ca, 0x3d0, 0x3d2, 0x3d5, 0x3d6, 0x3d7, 0x3d9, 0x3df, 0x3e1, 0x3e2, 0x3ea,
			0x3ec, 0x3ef, 0x3f0, 0x3f3, 0x3f4, 0x3fe, 0x404, 0x407, 0x408, 0x411, 0x412, 0x413, 0x416,
			0x419, 0x41a, 0x41c, 0x41d, 0x420, 0x421, 0x422, 0x423, 0x427, 0x429, 0x42b, 0x42c, 0x431,
			0x437, 0x438, 0x439, 0x43a, 0x43c, 0x43f, 0x441, 0x443, 0x444, 0x445, 0x44a, 0x44d, 0x44f);
		RegisterOpCode(OPCODE_Nop,
			0x453, 0x456, 0x457, 0x458, 0x459, 0x45a, 0x460, 0x466, 0x46a, 0x46b, 0x46c, 0x46e, 0x474,
			0x477, 0x47a, 0x47e, 0x47f, 0x480, 0x485, 0x488, 0x48c, 0x48e, 0x490, 0x493, 0x494, 0x495,
			0x497, 0x498, 0x49b, 0x49f, 0x4ab, 0x4ac, 0x4ad, 0x4ae, 0x4b1, 0x4bc, 0x4bf, 0x4c3, 0x4c6,
			0x4cb, 0x4cd, 0x4d4, 0x4d6, 0x4d9, 0x4dc, 0x4de, 0x4e2, 0x4e3, 0x4e5, 0x4e8, 0x4e9, 0x4ee,
			0x4f4, 0x4f6, 0x4fa, 0x4fe, 0x4ff, 0x500, 0x501, 0x504, 0x506, 0x507, 0x508, 0x50f, 0x511,
			0x515, 0x517, 0x518, 0x519, 0x51d, 0x51e, 0x521, 0x523, 0x52a, 0x52c, 0x52e, 0x532, 0x535);
		RegisterOpCode(OPCODE_Nop,
			0x538, 0x539, 0x53a, 0x540, 0x545, 0x546, 0x54a, 0x54b, 0x550, 0x553, 0x554, 0x555, 0x559,
			0x55a, 0x55e, 0x55f, 0x562, 0x563, 0x569, 0x56b, 0x56c, 0x56f, 0x570, 0x575, 0x576, 0x577,
			0x579, 0x57f, 0x580, 0x581, 0x587, 0x588, 0x58f, 0x592, 0x59f, 0x5a1, 0x5a2, 0x5a4, 0x5a7,
			0x5ab, 0x5af, 0x5b0, 0x5b1, 0x5b5, 0x5b8, 0x5b9, 0x5bc, 0x5be, 0x5c1, 0x5c3, 0x5c5, 0x5c6,
			0x5c7, 0x5cd, 0x5ce, 0x5cf, 0x5d3, 0x5d5, 0x5dc, 0x5df, 0x5e6, 0x5e7, 0x5e8, 0x5ee, 0x5ef,
			0x5f0, 0x5f2, 0x5f3, 0x5f5, 0x5f6, 0x5f7, 0x5fa, 0x5fe, 0x5ff, 0x600, 0x603, 0x609, 0x60e);
		RegisterOpCode(OPCODE_Nop,
			0x611, 0x612, 0x613, 0x614, 0x615, 0x619, 0x61b, 0x61d, 0x61f, 0x620, 0x626, 0x62b, 0x62c,
			0x632, 0x633, 0x634, 0x636, 0x639, 0x63b, 0x63c, 0x63e, 0x641, 0x648, 0x64a, 0x64c, 0x64e,
			0x651, 0x658, 0x659, 0x65b, 0x65c, 0x660, 0x661, 0x664, 0x665, 0x666, 0x669, 0x66c, 0x66d,
			0x66f, 0x671, 0x674, 0x676, 0x677, 0x678, 0x67a, 0x682, 0x685, 0x68e, 0x68f, 0x691, 0x695,
			0x697, 0x698, 0x699, 0x69c, 0x69d, 0x6a4, 0x6a6, 0x6a7, 0x6aa, 0x6af, 0x6b0, 0x6b3, 0x6bc,
			0x6bd, 0x6bf, 0x6c2, 0x6c3, 0x6c7, 0x6c9, 0x6ca, 0x6cb, 0x6cc, 0x6ce, 0x6d0, 0x6d6, 0x6d8);
		RegisterOpCode(OPCODE_Nop,
			0x6d9, 0x6db, 0x6dc, 0x6dd, 0x6de, 0x6e0, 0x6e1, 0x6e2, 0x6e3, 0x6e5, 0x6e7, 0x6f0, 0x6f3,
			0x6f6, 0x6f8, 0x6f9, 0x6fb, 0x6fc, 0x701, 0x704, 0x706, 0x70d, 0x710, 0x711, 0x713, 0x71c,
			0x71f, 0x724, 0x726, 0x728, 0x731, 0x733, 0x737, 0x738, 0x73b, 0x73c, 0x73d, 0x73e, 0x740,
			0x742, 0x743, 0x748, 0x750, 0x751, 0x754, 0x758, 0x75b, 0x75e, 0x764, 0x76e, 0x76f, 0x772,
			0x77b, 0x77e, 0x77f, 0x783, 0x784, 0x785, 0x795, 0x79a, 0x79b, 0x79e, 0x7a0, 0x7a4, 0x7a6,
			0x7a8, 0x7ac, 0x7ae, 0x7af, 0x7b0, 0x7b1, 0x7b2, 0x7b4, 0x7b6, 0x7b8, 0x7ba, 0x7bc, 0x7bd);
		RegisterOpCode(OPCODE_Nop,
			0x7c0, 0x7c2, 0x7c5, 0x7c7, 0x7cb, 0x7cc, 0x7cd, 0x7ce, 0x7cf, 0x7d2, 0x7d4, 0x7d5, 0x7d7,
			0x7d9, 0x7de, 0x7df, 0x7e3, 0x7e5, 0x7e7, 0x7e8, 0x7ec, 0x7ed, 0x7f3, 0x7f4, 0x7fc, 0x802,
			0x806, 0x808, 0x812, 0x813, 0x817, 0x818, 0x819, 0x81c, 0x81e, 0x820, 0x821, 0x822, 0x823,
			0x827, 0x833, 0x835, 0x836, 0x838, 0x83c, 0x83e, 0x83f, 0x840, 0x845, 0x847, 0x848, 0x849,
			0x84b, 0x84d, 0x84e, 0x84f, 0x853, 0x854, 0x855, 0x857, 0x858, 0x85b, 0x85d, 0x85e, 0x860,
			0x863, 0x864, 0x865, 0x867, 0x870, 0x876, 0x87a, 0x87b, 0x87d, 0x87f, 0x889, 0x88c, 0x88d);
		RegisterOpCode(OPCODE_Nop,
			0x88f, 0x890, 0x892, 0x894, 0x895, 0x896, 0x89d, 0x8a5, 0x8a7, 0x8aa, 0x8ae, 0x8ba, 0x8bc,
			0x8c0, 0x8c2, 0x8c3, 0x8c4, 0x8c9, 0x8cb, 0x8d4, 0x8d7, 0x8d8, 0x8d9, 0x8dc, 0x8de, 0x8e3,
			0x8e4, 0x8e7, 0x8e8, 0x8f0, 0x8f1, 0x8f2, 0x8f3, 0x8f4, 0x8f5, 0x8f7, 0x8fb, 0x8fc, 0x8fd,
			0x8fe, 0x901, 0x903, 0x907, 0x909, 0x90b, 0x90e, 0x911, 0x912, 0x915, 0x916, 0x91a, 0x91e,
			0x922, 0x925, 0x927, 0x928, 0x92a, 0x92c, 0x933, 0x938, 0x939, 0x93c, 0x93e, 0x941, 0x943,
			0x944, 0x949, 0x94a, 0x94b, 0x94d, 0x94f, 0x950, 0x951, 0x952, 0x959, 0x95a, 0x95b, 0x961);
		RegisterOpCode(OPCODE_Nop,
			0x962, 0x967, 0x968, 0x969, 0x96e, 0x96f, 0x974, 0x978, 0x982, 0x98a, 0x98c, 0x98d, 0x98f,
			0x992, 0x993, 0x995, 0x999, 0x99a, 0x9a6, 0x9a9, 0x9ae, 0x9b7, 0x9be, 0x9c0, 0x9c1, 0x9c3,
			0x9c6, 0x9c7, 0x9d1, 0x9d4, 0x9d6, 0x9d8, 0x9da, 0x9e0, 0x9e2, 0x9e7, 0x9e8, 0x9ea, 0x9ef,
			0x9f0, 0x9f2, 0x9f4, 0x9f5, 0x9fa, 0x9fc, 0x9fe, 0xa01, 0xa03, 0xa04, 0xa05, 0xa06, 0xa07,
			0xa0f, 0xa10, 0xa13, 0xa14, 0xa17, 0xa18, 0xa1b, 0xa1c, 0xa1d, 0xa1e, 0xa20, 0xa22, 0xa27,
			0xa2a, 0xa2e, 0xa2f, 0xa30, 0xa39, 0xa40, 0xa42, 0xa43, 0xa46, 0xa49, 0xa4a, 0xa52, 0xa57);
		RegisterOpCode(OPCODE_Nop,
			0xa5e, 0xa5f, 0xa61, 0xa65, 0xa68, 0xa6a, 0xa70, 0xa72, 0xa76, 0xa78, 0xa7a, 0xa80, 0xa82,
			0xa84, 0xa85, 0xa87, 0xa89, 0xa8c, 0xa8d, 0xa90, 0xa91, 0xa93, 0xa94, 0xa98, 0xa9b, 0xa9c,
			0xa9e, 0xa9f, 0xaa0, 0xaa6, 0xaab, 0xab0, 0xab1, 0xab2, 0xab4, 0xab7, 0xaba, 0xabd, 0xabe,
			0xabf, 0xac0, 0xac3, 0xac4, 0xac5, 0xac9, 0xaca, 0xacd, 0xacf, 0xad1, 0xad2, 0xad3, 0xad7,
			0xad8, 0xada, 0xade, 0xadf, 0xae1, 0xae7, 0xae8, 0xaf1, 0xaf2, 0xafc, 0xafe, 0xb00, 0xb01,
			0xb02, 0xb08, 0xb09, 0xb0a, 0xb0d, 0xb15, 0xb1e, 0xb26, 0xb2b, 0xb2c, 0xb2e, 0xb32, 0xb33);
		RegisterOpCode(OPCODE_Nop,
			0xb39, 0xb3a, 0xb3c, 0xb3f, 0xb40, 0xb41, 0xb43, 0xb44, 0xb47, 0xb4b, 0xb4c, 0xb55, 0xb56,
			0xb57, 0xb61, 0xb68, 0xb6a, 0xb6c, 0xb6e, 0xb70, 0xb71, 0xb75, 0xb77, 0xb7b, 0xb7c, 0xb7d,
			0xb87, 0xb8b, 0xb8c, 0xb8d, 0xb96, 0xb99, 0xb9a, 0xb9e, 0xba0, 0xba3, 0xba5, 0xba9, 0xbab,
			0xbae, 0xbb1, 0xbbb, 0xbc1, 0xbc5, 0xbc6, 0xbc8, 0xbcb, 0xbd5, 0xbda, 0xbdb, 0xbdc, 0xbde,
			0xbe0, 0xbe1, 0xbe5, 0xbe7, 0xbeb, 0xbed, 0xbf1, 0xbf3, 0xbf4, 0xbfb, 0xbfe, 0xbff, 0xc00,
			0xc03, 0xc05, 0xc0a, 0xc0c, 0xc0f, 0xc10, 0xc11, 0xc13, 0xc16, 0xc17, 0xc1c, 0xc1f, 0xc20);
		RegisterOpCode(OPCODE_Nop,
			0xc21, 0xc29, 0xc2b, 0xc2d, 0xc2f, 0xc35, 0xc38, 0xc3a, 0xc3d, 0xc3f, 0xc41, 0xc42, 0xc45,
			0xc49, 0xc50, 0xc52, 0xc54, 0xc55, 0xc56, 0xc57, 0xc58, 0xc59, 0xc5a, 0xc61, 0xc63, 0xc67,
			0xc71, 0xc72, 0xc74, 0xc76, 0xc78, 0xc7a, 0xc7f, 0xc81, 0xc84, 0xc86, 0xc8a, 0xc8c, 0xc92,
			0xc97, 0xc9c, 0xca4, 0xca5, 0xca7, 0xca8, 0xca9, 0xcae, 0xcaf, 0xcb0, 0xcb4, 0xcb9, 0xcbf,
			0xcc0, 0xcc1, 0xcce, 0xcd3, 0xcd4, 0xcd5, 0xcdc, 0xcdd, 0xcde, 0xcdf, 0xce2, 0xce3, 0xce5,
			0xce9, 0xcea, 0xceb, 0xced, 0xcee, 0xcf0, 0xcf5, 0xcf6, 0xcf7, 0xcf8, 0xcfe, 0xd03, 0xd05);
		RegisterOpCode(OPCODE_Nop,
			0xd07, 0xd09, 0xd0f, 0xd10, 0xd11, 0xd12, 0xd19, 0xd1b, 0xd26, 0xd28, 0xd2a, 0xd2c, 0xd2f,
			0xd36, 0xd39, 0xd3a, 0xd3b, 0xd3e, 0xd3f, 0xd43, 0xd44, 0xd45, 0xd46, 0xd4b, 0xd4d, 0xd4e,
			0xd51, 0xd53, 0xd57, 0xd59, 0xd5c, 0xd5d, 0xd62, 0xd63, 0xd66, 0xd67, 0xd69, 0xd6c, 0xd6e,
			0xd73, 0xd74, 0xd7b, 0xd83, 0xd84, 0xd85, 0xd86, 0xd8a, 0xd8e, 0xd92, 0xd95, 0xd99, 0xd9b,
			0xd9c, 0xd9e, 0xd9f, 0xda1, 0xda2, 0xda5, 0xda9, 0xdac, 0xdad, 0xdb0, 0xdb2, 0xdb7, 0xdb9,
			0xdbc, 0xdbd, 0xdbf, 0xdc2, 0xdc4, 0xdc6, 0xdc9, 0xdcd, 0xdce, 0xdd0, 0xdd4, 0xdd5, 0xdd9);
		RegisterOpCode(OPCODE_Nop,
			0xde0, 0xde1, 0xde2, 0xde3, 0xde5, 0xde7, 0xde9, 0xdf0, 0xdf1, 0xdf3, 0xdf8, 0xdf9, 0xdfb,
			0xe01, 0xe02, 0xe05, 0xe07, 0xe09, 0xe0e, 0xe0f, 0xe12, 0xe13, 0xe14, 0xe19, 0xe1b, 0xe1d,
			0xe21, 0xe25, 0xe26, 0xe28, 0xe2a, 0xe2b, 0xe2d, 0xe2e, 0xe30, 0xe36, 0xe38, 0xe3e, 0xe3f,
			0xe40, 0xe43, 0xe44, 0xe48, 0xe49, 0xe4a, 0xe4b, 0xe4f, 0xe50, 0xe52, 0xe59, 0xe5f, 0xe63,
			0xe64, 0xe68, 0xe6b, 0xe6c, 0xe70, 0xe71, 0xe7a, 0xe7e, 0xe82, 0xe84, 0xe85, 0xe86, 0xe8a,
			0xe8b, 0xe8e, 0xe92, 0xe95, 0xe98, 0xe9f, 0xea0, 0xea1, 0xea2, 0xea4, 0xea7, 0xeaf, 0xeb1);
		RegisterOpCode(OPCODE_Nop,
			0xeb6, 0xeb7, 0xeb9, 0xebb, 0xebc, 0xec2, 0xec3, 0xec4, 0xec5, 0xec6, 0xec9, 0xecb, 0xecd,
			0xece, 0xed3, 0xed5, 0xed7, 0xeda, 0xede, 0xee0, 0xee1, 0xee4, 0xee7, 0xeeb, 0xeee, 0xef0,
			0xef7, 0xefa, 0xefc, 0xf01, 0xf02, 0xf03, 0xf04, 0xf07, 0xf0a, 0xf0b, 0xf0c, 0xf0e, 0xf0f,
			0xf10, 0xf15, 0xf17, 0xf19, 0xf1a, 0xf1c, 0xf1d, 0xf1e, 0xf1f, 0xf20, 0xf24, 0xf27, 0xf28,
			0xf29, 0xf2b, 0xf2c, 0xf2d, 0xf30, 0xf32, 0xf33, 0xf34, 0xf35, 0xf36, 0xf3c, 0xf43, 0xf46,
			0xf48, 0xf4f, 0xf55, 0xf5e, 0xf69, 0xf6a, 0xf6c, 0xf6e, 0xf74, 0xf77, 0xf78, 0xf82, 0xf83);
		RegisterOpCode(OPCODE_Nop,
			0xf84, 0xf85, 0xf87, 0xf8b, 0xf97, 0xf9a, 0xf9c, 0xf9e, 0xfa1, 0xfa2, 0xfa6, 0xfa9, 0xfad,
			0xfaf, 0xfb2, 0xfb5, 0xfb6, 0xfbb, 0xfbc, 0xfbf, 0xfc2, 0xfce, 0xfcf, 0xfd4, 0xfd6, 0xfd7,
			0xfd8, 0xfda, 0xfdb, 0xfe3, 0xfe5, 0xfe8, 0xfeb, 0xff8, 0xffc, 0xffd
		);
		RegisterOpCode(OPCODE_EvalLocalVariableCachedDebug, 0x8);
		RegisterOpCode(OPCODE_EvalLocalVariableRefCachedDebug, 0x9);
		RegisterOpCode(OPCODE_ClearParams, 0xc);
		RegisterOpCode(OPCODE_CheckClearParams, 0xd);
		RegisterOpCode(OPCODE_PreScriptCall, 0xe);
		RegisterOpCode(OPCODE_CallBuiltinFunction, 0xf);
		RegisterOpCode(OPCODE_CallBuiltinMethod, 0x10);
		RegisterOpCode(OPCODE_End, 0x11);
		RegisterOpCode(OPCODE_EmptyArray, 0x15, 0xbc, 0x1ab, 0x1c1, 0x449, 0x478, 0x479, 0x4b2, 0x582, 0x8c8, 0xb28, 0xb46, 0xbb9, 0xccf, 0xd0d, 0xfea);
		RegisterOpCode(OPCODE_WaitTill, 0x17, 0xc8, 0x167, 0x1ac, 0x568, 0x6d1, 0x80b, 0x9bc, 0xa6e, 0xc15, 0xf31);
		RegisterOpCode(OPCODE_ClearFieldVariableOnStack, 0x19, 0x1e9, 0x212, 0x284, 0x290, 0x2f3, 0x2ff, 0x32f, 0x405, 0x42e, 0x49a, 0x543, 0x599, 0x5cc, 
			0x5e0, 0x74e, 0x74f, 0x7ca, 0x81a, 0x95c, 0x979, 0x97e, 0xbad, 0xbe6, 0xcd1, 0xd17, 0xdf5, 0xe2c, 0xf3a, 0xfd0);
		RegisterOpCode(OPCODE_ClearArray, 0x1b, 0xbe, 0x1e8, 0x208, 0x3bb, 0x463, 0x516, 0x6c0, 0x762, 0x7b9, 0x7dd, 0x871, 0x8ac, 0x9a1, 0x9b4, 0x9f3, 0xa6f, 0xd49, 0xe23, 0xea3);
		RegisterOpCode(OPCODE_ScriptMethodThreadCallPointer, 0x1c, 0x145, 0x1d3, 0x2b3, 0x335, 0x4c7, 0x524, 0x5d6, 0x631, 0x656, 0x6ea, 0x715, 0x7d6, 0x825, 0x931, 0x9a7, 0xa5a, 
			0xb21, 0xbd2, 0xbf9, 0xcff, 0xdd2, 0xe20, 0xe51, 0xf7a, 0xf7d);
		RegisterOpCode(OPCODE_WaitTillMatchTimeout, 0x1d, 0x81, 0x1f5, 0x283, 0x2a3, 0x3b8, 0x452, 0x58a, 0x60c, 0x643, 0x65e, 0x694, 0x811, 0x878, 0x8a4, 0x8ef, 0x8f8, 0xaa4, 
			0xc70, 0xccc, 0xcda, 0xd2d, 0xdb1, 0xeaa);
		RegisterOpCode(OPCODE_GetHash, 0x1f, 0x85, 0x107, 0x28f, 0x3b1, 0x4b0, 0x4ba, 0x4da, 0x5b3, 0x63f, 0x6d2, 0x7e0, 0x8a6, 0x8eb, 0x904, 0x96d, 0xa28, 0xa77, 0xaff, 0xbe9, 
			0xc23, 0xc39, 0xc9d, 0xd31, 0xe1f, 0xe69, 0xed2, 0xff7);
		RegisterOpCode(OPCODE_WaitFrame, 0x20, 0xc5, 0x16d, 0x20f, 0x264, 0x280, 0x33a, 0x3b9, 0x4be, 0x4e4, 0x5a3, 0x716, 0x765, 0x898, 0x914, 0x96c, 0xa3e, 0xa92, 0xb13, 0xb90, 
			0xcac, 0xd14, 0xd8c, 0xfab, 0xfb1);
		RegisterOpCode(OPCODE_ScriptMethodThreadCallPointerEndOn, 0x22, 0xa2, 0xa6, 0x1cf, 0x1da, 0x2c0, 0x2e2, 0x30c, 0x35e, 0x37b, 0x37d, 0x396, 0x3c9, 0x3f6, 0x4c4, 0x5ec,
			0x76a, 0x89f, 0x8d3, 0x906, 0x948, 0xbcf, 0xc6a, 0xc6f, 0xcc9, 0xe9b, 0xfbe);
		RegisterOpCode(OPCODE_WaitTillMatch, 0x24, 0x47, 0x58, 0x70, 0xdd, 0x21f, 0x251, 0x296, 0x2d9, 0x391, 0x4b7, 0x503, 0x57d, 0x6a0, 0x7a2, 0x97d, 0x991, 0xa48, 0xb58,
			0xb9d, 0xbf5, 0xcec, 0xd72, 0xd7c, 0xe47, 0xe93, 0xef5);
		RegisterOpCode(OPCODE_ScriptThreadCallEndOn, 0x26, 0xfe, 0x23f, 0x2a2, 0x3c8, 0x4d8, 0x4eb, 0x526, 0x629, 0x688, 0x70b, 0x788, 0x868, 0xa1f, 0xa64, 0xaeb, 0xbb8, 0xbd3,
			0xc4d, 0xd1f, 0xd35, 0xdb3, 0xe42, 0xeb3, 0xeb8, 0xf7f, 0xfe2);
		RegisterOpCode(OPCODE_Switch, 0x29, 0x2a, 0xa0, 0x2f6, 0x4b9, 0x7f5, 0x83b, 0x918, 0x9ad, 0x9cc, 0x9f7, 0xac6, 0xb38, 0xb92, 0xbf7, 0xc02, 0xe87, 0xee5, 0xf58, 0xf73);
		RegisterOpCode(OPCODE_ScriptFunctionCallPointer, 0x2e, 0x23c, 0x243, 0x2ed, 0x326, 0x345, 0x3d1, 0x424, 0x440, 0x447, 0x454, 0x473, 0x541, 0x5e2, 0x5fc, 0x622, 0x652,
			0x773, 0x78c, 0x794, 0x856, 0x8a8, 0x8af, 0xa0a, 0xadd, 0xe1e, 0xecf, 0xfa0, 0xffe);
		RegisterOpCode(OPCODE_DevblockBegin, 0x31, 0x147, 0x157, 0x1a9, 0x1b6, 0x262, 0x279, 0x2c2, 0x3e5, 0x44b, 0x672, 0x6f4, 0x85f, 0x8d2, 0x92b, 0x987, 0x9ca, 0xa29, 0xa71,
			0xadc, 0xb3d, 0xbb2, 0xd1d, 0xdee, 0xef1, 0xf00, 0xf41);
		RegisterOpCode(OPCODE_NotEqual, 0x32, 0x28b, 0x291, 0x2cf, 0x2f1, 0x379, 0x3cb, 0x48b, 0x49d, 0x51b, 0x53b, 0x6f2, 0x8db, 0x913, 0x9ed, 0xd77, 0xd79, 0xde6, 0xe9a, 0xf9d);
		RegisterOpCode(OPCODE_ScriptMethodCallPointer, 0x33, 0x3a, 0xb8, 0x238, 0x2be, 0x342, 0x638, 0x9b1, 0x9b6, 0xa0b, 0xa95, 0xc77, 0xf67);
		RegisterOpCode(OPCODE_JumpOnTrue, 0x34, 0x64, 0xb0, 0x374, 0x385, 0x3ff, 0x43d, 0x44c, 0x4f5, 0x623, 0x769, 0x905, 0x91c, 0x958, 0x9d9, 0xc4e, 0xcfa, 0xd8f, 0xe57);
		RegisterOpCode(OPCODE_Unknown35, 0x35, 0x4a, 0x175, 0x1c8, 0x235, 0x2f8, 0x3e3, 0x4a0, 0x4f7, 0x552, 0x604, 0x7f0, 0x8c7, 0x91b, 0x930, 0xa26, 0xc04, 0xca1, 0xddc, 0xed9);
		RegisterOpCode(OPCODE_ScriptMethodThreadCallEndOn, 0x36, 0x172, 0x38a, 0x621, 0x775, 0xa2d, 0xa5c, 0xdbe, 0xf4c, 0xfcb);
		RegisterOpCode(OPCODE_SafeCreateLocalVariables, 0x37, 0x89, 0x150, 0x1a8, 0x261, 0x26f, 0x334, 0x34c, 0x3cd, 0x489, 0x533, 0x640, 0x65d, 0x719, 0x807, 0x882, 0x955, 0xae5,
			0xb2f, 0xbbf, 0xbd4, 0xc6b, 0xc89, 0xdde, 0xe83, 0xebd, 0xf44, 0xf61, 0xfb7);
		RegisterOpCode(OPCODE_Unknown38, 0x38, 0x6f, 0x72, 0xca, 0xda, 0x178, 0x269, 0x316, 0x409, 0x578, 0x642, 0x797, 0x79d, 0x7e6, 0x98e, 0x997, 0x99b, 0xb3e, 0xbb0, 0xbdd,
			0xd94, 0xdb5, 0xddd, 0xe4e, 0xe58, 0xf4a);
		RegisterOpCode(OPCODE_GetNegUnsignedShort, 0x39, 0x14e, 0x1b2, 0x1f9, 0x202, 0x35f, 0x3db, 0x3f1, 0x406, 0x51a, 0x714, 0x76b, 0x799, 0x7fa, 0x815, 0x85c, 0x897, 0x8ce,
			0xa37, 0xad0, 0xaf3, 0xb1d, 0xb3b, 0xd64, 0xdff, 0xf7b, 0xfb8);
		RegisterOpCode(OPCODE_ClassFunctionCall, 0x3b, 0x5d, 0x14b, 0x27e, 0x29a, 0x58e, 0x61a, 0x70a, 0x78d, 0x78f, 0x850, 0x8a0, 0x94c, 0xa8a, 0xb14, 0xba1, 0xbcc, 0xbf8, 0xcd9, 0xdca, 0xf23);
		RegisterOpCode(OPCODE_Return, 0x3c, 0xb1, 0x26c, 0x288, 0x36b, 0x45e, 0x482, 0x547, 0x57e, 0x68c, 0x73f, 0x800, 0x885, 0x9ab, 0x9fb, 0x9fd, 0xa16, 0xb0b, 0xbee, 0xc07,
			0xd34, 0xe4d, 0xf08, 0xf1b, 0xf21, 0xf62, 0xfaa, 0xfb4);
		RegisterOpCode(OPCODE_EvalFieldVariableRef, 0x40, 0x7d, 0x1f0, 0x29c, 0x2b7, 0x487, 0x4ec, 0x4ed, 0x593, 0x5c0, 0x69e, 0x700, 0x72b, 0x786, 0x7ab, 0x80d, 0x8df, 0x9d5,
			0xa50, 0xaf5, 0xb48, 0xb94, 0xbfc, 0xc06, 0xc1d, 0xe79, 0xe88, 0xef2, 0xf9f);
		RegisterOpCode(OPCODE_SafeDecTop, 0x42, 0x60, 0x98, 0xb9, 0x3c6, 0x67b, 0x72f, 0x759, 0x805, 0x888, 0x88e, 0x902, 0x920, 0x946, 0x96a, 0xa0e, 0xb1a, 0xb1f, 0xeec, 0xfc5);
		RegisterOpCode(OPCODE_Bit_Or, 0x44, 0x197, 0x23a, 0x2de, 0x350, 0x3a0, 0x43b, 0x56a, 0x595, 0x68a, 0x6e9, 0x761, 0x8d1, 0x940, 0xa6b, 0xdeb, 0xed0, 0xf4e);
		RegisterOpCode(OPCODE_ScriptThreadCall, 0x45, 0x9d, 0xa4, 0x1aa, 0x223, 0x356, 0x392, 0x442, 0x8cd, 0x966, 0xa63, 0xabb, 0xb34, 0xb98, 0xd48, 0xd5a, 0xe10, 0xef4, 0xf53, 0xfdc);
		RegisterOpCode(OPCODE_Bit_Xor, 0x46, 0x213, 0x257, 0x33e, 0x36e, 0x4af, 0x8dd, 0x937, 0x956, 0x9c9, 0xaf7, 0xb76, 0xb79, 0xc5c, 0xc5e, 0xc75, 0xc83, 0xd41, 0xdc1);
		RegisterOpCode(OPCODE_GetSelfObject, 0x49, 0x59, 0x5a, 0x6b, 0x125, 0x129, 0x196, 0x3f5, 0x472, 0x4e7, 0x54d, 0x571, 0x62e, 0x6e8, 0x756, 0x7bf, 0x8ad, 0x91d, 0xa2c,
			0xa38, 0xd47, 0xea5, 0xf11, 0xf81, 0xf96, 0xfa4, 0xfdd);
		RegisterOpCode(OPCODE_GetNegByte, 0x4b, 0x199, 0x21e, 0x29d, 0x3b6, 0x42d, 0x549, 0x583, 0x6a3, 0x8ec, 0x9e5, 0xa24, 0xa3f, 0xbd9, 0xc19, 0xc79, 0xca2, 0xcb7, 0xd78,
			0xd81, 0xdfc, 0xe62, 0xf22, 0xf5a, 0xfd2);
		RegisterOpCode(OPCODE_ScriptThreadCallPointerEndOn, 0x4c, 0x57, 0x162, 0x35a, 0x35d, 0x371, 0x375, 0x414, 0x468, 0x4b3, 0x4dd, 0x5f1, 0x693, 0x6c6, 0x720, 0x77c, 0x7e2,
			0x824, 0x82e, 0xc30, 0xd5e, 0xe03, 0xe66, 0xe94, 0xfc4);
		RegisterOpCode(OPCODE_BoolComplement, 0x4d, 0x13a, 0x3b7, 0x42a, 0x514, 0x573, 0x5d0, 0x5f4, 0x668, 0x71b, 0x89e, 0x8b4, 0x9d7, 0xa88, 0xac7, 0xcf2, 0xe33, 0xe5d, 0xead);
		RegisterOpCode(OPCODE_IsDefined, 0x4e, 0x77, 0x38c, 0x3c3, 0x3ee, 0x481, 0x55c, 0x565, 0x5d2, 0x5dd, 0x5e4, 0x736, 0x74d, 0x79c, 0x7f1, 0x841, 0x94e, 0xaec, 0xb20, 0xb36,
			0xb51, 0xb93, 0xc08, 0xc14, 0xc26, 0xe74, 0xf5d, 0xf79);
		RegisterOpCode(OPCODE_AddToArray, 0x51, 0x119, 0x146, 0x180, 0x4e1, 0x510, 0x60a, 0x64f, 0x6d5, 0x777, 0x986, 0xa60, 0xaf4, 0xc7c, 0xe60, 0xfae);
		RegisterOpCode(OPCODE_Wait, 0x54, 0x10c, 0x17d, 0x324, 0x3e6, 0x67d, 0x6ec, 0x809, 0x923, 0xc5b, 0xc64, 0xded, 0xe5a, 0xee9, 0xf49, 0xf5f, 0xfc0);
		RegisterOpCode(OPCODE_SuperEqual, 0x55, 0xcc, 0x1bd, 0x30b, 0x31c, 0x361, 0x4a4, 0x522, 0x7b7, 0x7e1, 0x8c5, 0x93a, 0x9b0, 0x9c5, 0xa3d, 0xa45, 0xa69, 0xb72, 0xbdf, 0xd27,
			0xd6a, 0xd8b, 0xdae, 0xdd1, 0xdd8, 0xe16, 0xed4, 0xf3b, 0xf4d, 0xff6);
		RegisterOpCode(OPCODE_GetUndefined, 0x56, 0x1a3, 0x1cd, 0x358, 0x739, 0x84a, 0x9f6, 0xb6b, 0xdef, 0xe3c, 0xf92, 0xfa5, 0xff4);
		RegisterOpCode(OPCODE_ScriptFunctionCall, 0x5c, 0x68, 0xa7, 0x17e, 0x24c, 0x327, 0x37c, 0x75c, 0x814, 0x834, 0x880, 0x97f, 0xac8, 0xbb6, 0xc0e, 0xc9b, 0xcb5,
			0xcf1, 0xde4, 0xeca, 0xfd3);
		RegisterOpCode(OPCODE_EvalFieldVariableOnStackRef, 0x5f, 0x65, 0x93, 0x1d2, 0x219, 0x28c, 0x40e, 0x448, 0x450, 0x4d5, 0x6ac, 0x877, 0x970, 0x97a, 0x9b5, 0xb31, 0xbc7,
			0xc4f, 0xdb4, 0xe15, 0xe8c, 0xe96, 0xf64);
		RegisterOpCode(OPCODE_JumpOnTrueExpr, 0x61, 0x10a, 0x170, 0x17c, 0x2a9, 0x2fc, 0x303, 0x3bd, 0x618, 0x69f, 0x7bb, 0x8fa, 0x9f1, 0xa6d, 0xaf6, 0xb12, 0xb27, 0xbe8,
			0xbfa, 0xca6, 0xcc7, 0xe1c, 0xe78, 0xef8, 0xf06, 0xf98);
		RegisterOpCode(OPCODE_CreateArray, 0x62, 0x1fb, 0x255, 0x2fb, 0x39c, 0x461, 0x505, 0x58d, 0x6ba, 0x6ff, 0x75d, 0x832, 0x8e0, 0x9c8, 0x9cb, 0x9e3, 0xab6, 0xb5c, 0xb63,
			0xb83, 0xbce, 0xc09, 0xd1c, 0xd58, 0xe37, 0xe5e, 0xf70, 0xf7c);
		RegisterOpCode(OPCODE_Inc, 0x63, 0x8f, 0x1fe, 0x304, 0x35c, 0x36d, 0x3ba, 0x4cf, 0x4f1, 0x5aa, 0x7eb, 0x8b3, 0xa7e, 0xb19, 0xdc8, 0xf51, 0xfb0);
		RegisterOpCode(OPCODE_ShiftLeft, 0x6a, 0xb4, 0x179, 0x186, 0x249, 0x3b0, 0x3d4, 0x465, 0x483, 0x49c, 0x4b6, 0x584, 0x654, 0x6e4, 0x6f1, 0x75f, 0x85a, 0x9a8, 0xaea,
			0xaed, 0xb03, 0xb30, 0xba6, 0xc48, 0xc7e, 0xcc8, 0xcd7, 0xd06, 0xe34, 0xfc6);
		RegisterOpCode(OPCODE_JumpOnGreaterThan, 0x6c, 0x102, 0x164, 0x389, 0x435, 0x5b7, 0x650, 0x6c8, 0x796, 0x7b5, 0x953, 0xc9e, 0xce7, 0xe0b, 0xe6e, 0xe7c);
		RegisterOpCode(OPCODE_Plus, 0x6e, 0xae, 0x104, 0x106, 0x144, 0x2a6, 0x323, 0x51f, 0x596, 0x59a, 0x59e, 0x8e6, 0x975, 0xa56, 0xb4d, 0xb54, 0xbac, 0xd7e, 0xef3, 0xfca);
		RegisterOpCode(OPCODE_CastAndEvalFieldVariable, 0x71, 0x32b, 0x340, 0x512, 0x7a3, 0x80a, 0x8e1, 0xb4a, 0xb7e, 0xbaa, 0xbd7, 0xcd2, 0xe41, 0xedd, 0xf80);
		RegisterOpCode(OPCODE_ShiftRight, 0x76, 0x24a, 0x366, 0x3d3, 0x6b9, 0x707, 0x7fe, 0x86b, 0x8ed, 0xa02, 0xa35, 0xab5, 0xda4, 0xf71, 0xfba);
		RegisterOpCode(OPCODE_CreateStruct, 0x78, 0x1eb, 0x201, 0x2bc, 0x3f9, 0x455, 0x52b, 0x65f, 0x679, 0x72c, 0x873, 0x8b5, 0xa66, 0xbc2, 0xd75, 0xdc7, 0xe00, 0xeb0);
		RegisterOpCode(OPCODE_CastCanon, 0x7a, 0xd7, 0x15f, 0x265, 0x2b4, 0x2b6, 0x2f4, 0x39b, 0x5db, 0x722, 0x730, 0x7da, 0x8d0, 0x9a3, 0x9d3, 0xbe2, 0xc27);
		RegisterOpCode(OPCODE_GreaterThanOrEqualTo, 0x7b, 0x211, 0x29f, 0x315, 0x4c8, 0x5e9, 0x5eb, 0x69a, 0x87e, 0xa83, 0xad4, 0xb07, 0xb10, 0xb8e, 0xb91, 0xcbd, 0xd70, 
			0xf14, 0xf45, 0xfed);
		RegisterOpCode(OPCODE_SetVariableFieldRef, 0x7c, 0xf7, 0x133, 0x154, 0x1ff, 0x388, 0x492, 0x57a, 0x8a2, 0x8d5, 0x954, 0xa33, 0xc47, 0xc96, 0xd55, 0xd6b, 0xe6f, 0xf40);
		RegisterOpCode(OPCODE_GetUIntPtr, 0x7e, 0xf9, 0x2c3, 0x35b, 0x3e8, 0x410, 0x47b, 0x525, 0x5a0, 0x5cb, 0x70c, 0xa47, 0xb60, 0xd15, 0xd22, 0xedf, 0xf76);
		RegisterOpCode(OPCODE_GetLongInteger, 0x82, 0x13d, 0x1df, 0x377, 0x39f, 0x602, 0x662, 0x69b, 0x801, 0x804, 0x9af, 0x9eb, 0xa4f, 0xafa, 0xb42, 0xb67, 0xb73, 0xb81, 
			0xc0b, 0xc4b, 0xca3, 0xd8d, 0xdb8, 0xe17, 0xe65);
		RegisterOpCode(OPCODE_EvalArray, 0x8a, 0x12a, 0x1de, 0x278, 0x2df, 0x36c, 0x496, 0x4f8, 0x50e, 0x5ed, 0x65a, 0x6a2, 0x6eb, 0x9e4, 0x9ec, 0xa11, 0xae4, 0xaf9, 0xafd, 
			0xc24, 0xc2c, 0xdda, 0xdf2, 0xe11, 0xf09, 0xf6d);
		RegisterOpCode(OPCODE_WaitTillFrameEnd, 0x8b, 0x11e, 0x285, 0x2ab, 0x2c1, 0x390, 0x3e0, 0x64d, 0x79f, 0x89a, 0x8a9, 0x8ab, 0x976, 0x9ff, 0xb65, 0xb6d, 0xb80, 0xbf0,
			0xc2a, 0xc37, 0xc80, 0xddf, 0xe08);
		RegisterOpCode(OPCODE_EndOnCallback, 0x8c, 0xd4, 0x56e, 0x605, 0x908, 0x9c4, 0x9ce, 0xb9f, 0xbd1, 0xefd);
		RegisterOpCode(OPCODE_FirstArrayKey, 0x8d, 0x22d, 0x30e, 0x57c, 0x78a, 0x899, 0x964, 0xa23, 0xb8a, 0xd01, 0xdf6, 0xfc7);
		RegisterOpCode(OPCODE_SetVariableField, 0x8e, 0xf3, 0x21b, 0x28e, 0x2ee, 0x830, 0x831, 0x9bf, 0xa96, 0xbcd, 0xbd6, 0xc18, 0xd08, 0xd9a, 0xda6, 0xf8f, 0xfd9, 0xfec);
		RegisterOpCode(OPCODE_EndOn, 0x91, 0xb5, 0xe6, 0x10d, 0x237, 0x246, 0x272, 0x341, 0x40d, 0x45b, 0x45d, 0x4d2, 0x667, 0x6be, 0x6e6, 0x6ee, 0x734, 0x80c, 0x92f, 0x960, 
			0xa7d, 0xacb, 0xb89, 0xc1a, 0xd1a, 0xd23, 0xd9d, 0xe0d, 0xfa7);
		RegisterOpCode(OPCODE_SuperNotEqual, 0x92, 0x13e, 0x1ce, 0x2a5, 0x359, 0x426, 0x61c, 0x725, 0xa99, 0xb18, 0xb5a, 0xd90);
		RegisterOpCode(OPCODE_GetFloat, 0x94, 0x100, 0x183, 0x1e2, 0x20a, 0x381, 0x3d8, 0x3e4, 0x598, 0x5ad, 0x630, 0x70f, 0x74a, 0x7a5, 0x7ea, 0x883, 0x8f9, 0xaa7, 0xb50,
			0xb7f, 0xce1, 0xd3d, 0xe39, 0xf52, 0xfc9);
		RegisterOpCode(OPCODE_ProfileStart, 0x95, 0xba, 0x1d0, 0x228, 0x242, 0x2bd, 0x556, 0x5bf, 0x62d, 0x6cf, 0x7ee, 0x82d, 0xa3a, 0xa5d, 0xaa5, 0xaa9, 0xace, 0xad6, 0xba7,
			0xbc0, 0xd89, 0xdbb, 0xdc0, 0xe04, 0xe80, 0xec8, 0xf5b, 0xfe0);
		RegisterOpCode(OPCODE_GetString, 0x96, 0x1b0, 0x217, 0x231, 0x2af, 0x2d6, 0x380, 0x467, 0x601, 0x608, 0x6c1, 0x8b0, 0x9de, 0xc44, 0xd4c, 0xff1);
		RegisterOpCode(OPCODE_BoolNot, 0x9a, 0x13c, 0x19a, 0x1d8, 0x220, 0x566, 0x606, 0x6b2, 0x7d1, 0xaac, 0xc6c, 0xda0, 0xe73, 0xeab, 0xf39, 0xf3e, 0xf5c, 0xfd1, 0xff0, 0xffa);
		RegisterOpCode(OPCODE_CastBool, 0x9b, 0xed, 0x11b, 0x152, 0x1c4, 0x364, 0x464, 0x49e, 0x537, 0x585, 0x681, 0x78b, 0x881, 0x90c, 0x9e1, 0xa32, 0xa4b, 0xa59, 0xa5b, 0xc3b, 0xcb8, 0xe3a, 0xeff);
		RegisterOpCode(OPCODE_Unknown9e, 0x9e, 0xe7, 0x1a5, 0x1b1, 0x1f8, 0x274, 0x287, 0x28d, 0x31a, 0x372, 0x3fa, 0x40b, 0x41e, 0x4f3, 0x6ad, 0x6b1, 0x6d7, 0x839, 0x988, 0xa74,
			0xb66, 0xb82, 0xbc4, 0xc34, 0xc7d, 0xcbc, 0xd0e, 0xf18, 0xf8a, 0xfc1);
		RegisterOpCode(OPCODE_Equal, 0x9f, 0x11c, 0x21d, 0x24e, 0x2e5, 0x531, 0x5bb, 0x627, 0x717, 0x7db, 0x93f, 0x95e, 0xaa8, 0xb0c, 0xc8f, 0xddb, 0xea8, 0xf2e);
		RegisterOpCode(OPCODE_GetUnsignedInteger, 0xa1, 0x15a, 0x1bf, 0x330, 0x44e, 0x591, 0x5d9, 0x637, 0x768, 0x76d, 0x7fd, 0x80e, 0x879, 0x90a, 0x972, 0xd1e, 0xd6d, 0xd6f, 0xdf7, 0xe91, 0xf2a);
		RegisterOpCode(OPCODE_WaittillTimeout, 0xa9, 0xb7, 0x149, 0x189, 0x198, 0x293, 0x360, 0x3f8, 0x57b, 0x5f8, 0x703, 0x723, 0x7d8, 0x81b, 0x859, 0x8b8, 0x989, 0x9f8, 0xa79, 
			0xb59, 0xbaf, 0xbbd, 0xbe3, 0xd71, 0xdb6, 0xdf4, 0xeb2, 0xf8d);
		RegisterOpCode(OPCODE_EvalLocalVariableCachedSafe, 0xad, 0x10f, 0x12d, 0x141, 0x696, 0x77d, 0xa3c, 0xb2a, 0xbfd, 0xdcb, 0xfbd);
		RegisterOpCode(OPCODE_GreaterThan, 0xbb, 0x110, 0x2dd, 0x3cf, 0x3f7, 0x4d1, 0x534, 0x5b2, 0x8bd, 0xa19, 0xb5b, 0xcb3, 0xdab, 0xf25, 0xfb3);
		RegisterOpCode(OPCODE_Jump, 0xc0, 0x151, 0x16a, 0x502, 0x5c8, 0x5f9, 0x73a, 0x75a, 0x86d, 0x8ca, 0x900, 0x924, 0x942, 0x9d0, 0xaf8, 0xcaa, 0xcdb, 0xdcc, 0xdfe, 0xfe9);
		RegisterOpCode(OPCODE_Divide, 0xc1, 0x2ca, 0x3c0, 0x469, 0x486, 0x826, 0x9e6, 0xc90, 0xe61, 0xea9, 0xfe4);
		RegisterOpCode(OPCODE_EndSwitch, 0xc2, 0x200, 0x23e, 0x2ef, 0x314, 0x386, 0x394, 0x40f, 0x41f, 0x43e, 0x462, 0x46d, 0x4b5, 0x574, 0x6d3, 0x792, 0x7a9, 0x7f8, 0x7fb, 
			0x81f, 0x86e, 0xad9, 0xb53, 0xc3e, 0xcc2, 0xe45, 0xf4b, 0xfef);
		RegisterOpCode(OPCODE_SetLocalVariableCachedOnStack, 0xc4, 0xcf, 0xd0, 0x224, 0x26e, 0x2a8, 0x337, 0x349, 0x365, 0x4a2, 0x4f2, 0x54e, 0x647, 0x72e, 0x760, 0x91f, 0x998, 0x99e, 0xa6c,
			0xa8f, 0xb6f, 0xc82, 0xec1, 0xf59, 0xfa8);
		RegisterOpCode(OPCODE_Unknownc7, 0xc7, 0x3a7, 0x50c, 0x567, 0x624, 0x67c, 0x708, 0x776, 0x778, 0x780, 0x842, 0x88a, 0x8bb, 0xa2b, 0xa7c, 0xae2, 0xae9, 0xba4, 0xbb5,
			0xc43, 0xdaf, 0xdcf, 0xe46, 0xe54, 0xe6d, 0xeae, 0xeb5, 0xf99);
		RegisterOpCode(OPCODE_JumpOnFalse, 0xc9, 0x1c2, 0x204, 0x2d3, 0x3cc, 0x3dc, 0x430, 0x4e0, 0x6b6, 0x6da, 0x757, 0x7dc, 0xa08, 0xb25, 0xb4e, 0xc1b, 0xcc4, 0xdfd, 0xe22, 
			0xe77, 0xe8d, 0xf0d, 0xf12, 0xfee);
		RegisterOpCode(OPCODE_JumpOnFalseExpr, 0xce, 0x275, 0x347, 0x594, 0x59d, 0x63a, 0x95d, 0x996, 0x9db, 0xafb, 0xb23, 0xb84, 0xcd0, 0xd20, 0xd5b, 0xe35, 0xe67, 0xe90, 
			0xea6, 0xf7e);
		RegisterOpCode(OPCODE_EvalSelfFieldVariable, 0xd3, 0x134, 0x1f7, 0x46f, 0x59c, 0x657, 0x66b, 0x774, 0x88b, 0x8a3, 0x93b, 0xaaf, 0xbb7, 0xc2e, 0xcca, 0xce8, 0xd91);
		RegisterOpCode(OPCODE_Minus, 0xd9, 0x13b, 0x22a, 0x395, 0x520, 0x536, 0x6ae, 0x6fe, 0x771, 0x86a, 0x963, 0xa97, 0xb1c, 0xb24, 0xbc3, 0xbc9, 0xe5c, 0xe7f, 0xf68, 0xfdf);
		RegisterOpCode(OPCODE_ProfileStop, 0xdc, 0x20d, 0x59b, 0x875, 0x99f, 0xa51, 0xc6d, 0xce0, 0xf50, 0xf60, 0xfcd);
		RegisterOpCode(OPCODE_GetInteger, 0xe1, 0x1ec, 0x321, 0x3f2, 0x471, 0x4c9, 0x4df, 0x58b, 0x635, 0x749, 0x803, 0x837, 0x935, 0x95f, 0x9d2, 0xa0d, 0xa12, 0xa9a,
			0xe97, 0xe9d, 0xeb4, 0xf65);
		RegisterOpCode(OPCODE_ClassFunctionThreadCall, 0xe3, 0x114, 0x1a1, 0x1d6, 0x3b5, 0x451, 0x50a, 0x53e, 0x5fd, 0x6b4, 0x709, 0x793, 0xa55, 0xa81, 0xaad, 0xb69, 
			0xbd0, 0xc46, 0xcbe, 0xd25, 0xd61, 0xd7f, 0xde8, 0xebf, 0xee8, 0xeed, 0xf6f);
		RegisterOpCode(OPCODE_VectorScale, 0xe4, 0x1f3, 0x2e7, 0x344, 0x3a2, 0x4a1, 0x4bd, 0x513, 0x542, 0x55d, 0x747, 0x790, 0x869, 0x926, 0x9bb, 0xa15, 0xa7b, 0xb52, 0xd37, 0xe56, 0xefe);
		RegisterOpCode(OPCODE_Modulus, 0xe9, 0x1a6, 0x1c9, 0x22b, 0x294, 0x300, 0x30f, 0x3be, 0x40c, 0x5e5, 0x5ea, 0x62f, 0x649, 0x66e, 0x712, 0x7ad, 0x7ef, 0x945, 0x973,
			0x9bd, 0xb5d, 0xbca, 0xc51, 0xc87, 0xd0c, 0xd82, 0xebe, 0xf37, 0xf75, 0xf86);
		RegisterOpCode(OPCODE_VectorConstant, 0xea, 0x105, 0x177, 0x415, 0x4c0, 0x8e9, 0x921, 0xa75, 0xae3, 0xb2d, 0xe9c, 0xef9);
		RegisterOpCode(OPCODE_SizeOf, 0xeb, 0x184, 0x1d9, 0x241, 0x27c, 0x2e1, 0x34b, 0x357, 0x55b, 0x727, 0x7a7, 0x7c8, 0x816, 0x89c, 0xa0c, 0xa67, 0xaee, 0xb22, 0xc62, 0xe0c);
		RegisterOpCode(OPCODE_Notify, 0xf0, 0x207, 0x2a7, 0x2eb, 0x3c4, 0x403, 0x625, 0x67e, 0x82a, 0x9dd, 0xc66, 0xcb2, 0xdc3, 0xdc5, 0xf2f, 0xf3d);
		RegisterOpCode(OPCODE_Vector, 0xf2, 0x112, 0x34e, 0x3fd, 0x6ed, 0x781, 0x7e9, 0x81d, 0x8a1, 0xa44, 0xb88, 0xd4a, 0xe7d, 0xf38, 0xf72);
		RegisterOpCode(OPCODE_Dec, 0xf5, 0x165, 0x1f6, 0x295, 0x31e, 0x3de, 0x41b, 0x4a7, 0x4d3, 0x645, 0x752, 0x779, 0x77a, 0x82c, 0x9df, 0xb04, 0xb5e, 0xb7a, 0xb85, 0xb95,
			0xbec, 0xcba, 0xccd, 0xdaa, 0xe55, 0xe99, 0xec0, 0xf93);
		RegisterOpCode(OPCODE_JumpOnLessThan, 0xfb, 0x130, 0x190, 0x48d, 0x572, 0x646, 0x663, 0x744, 0x861, 0x86f, 0x886, 0xb0f, 0xbbe, 0xbf2, 0xc25, 0xcf3, 0xd18, 0xedc, 0xfd5);
		RegisterOpCode(OPCODE_Bit_And, 0xfd, 0x4fd, 0x684, 0x6b7, 0x6bb, 0x6d4, 0xc12, 0xc9a, 0xd54, 0xe24, 0xfe7);
		RegisterOpCode(OPCODE_GetObjectType, 0x101, 0x194, 0x276, 0x2fe, 0x34a, 0x3b2, 0x40a, 0x47c, 0x58c, 0x644, 0x655, 0x721, 0x7f6, 0x828, 0x843, 0x934, 0x9b3,
			0x9b9, 0xaa2, 0xcd6, 0xcfd, 0xd33, 0xd7a, 0xda3, 0xe6a, 0xf89, 0xfff);
		RegisterOpCode(OPCODE_Multiply, 0x113, 0x206, 0x2ad, 0x3ce, 0x433, 0x484, 0x4d0, 0x529, 0x617, 0x673, 0x6c5, 0x76c, 0x7d0, 0x89b, 0x9f9, 0xa1a, 0xa36, 0xb37,
			0xba2, 0xc8e, 0xcc6, 0xcfb, 0xd00, 0xd88, 0xe27, 0xe4c, 0xf95, 0xffb);
		RegisterOpCode(OPCODE_EvalFieldVariableOnStack, 0x116, 0x118, 0x16f, 0x2dc, 0x3b3, 0x3fb, 0x4b4, 0x6df, 0x770, 0x7c3, 0x917, 0xa41, 0xca0, 0xee6);
		RegisterOpCode(OPCODE_GetTime, 0x117, 0x137, 0x19d, 0x253, 0x376, 0x45f, 0x475, 0x4a5, 0x4a6, 0x4c2, 0x4ef, 0x5a9, 0x753, 0x7c9, 0x9ba, 0xc53, 0xcd8, 0xd02, 0xda8);
		RegisterOpCode(OPCODE_SetLocalVariableCached, 0x11d, 0x1c0, 0x1e4, 0x1ee, 0x3e7, 0x4f0, 0x60f, 0x683, 0x687, 0x910, 0x936, 0x9a0, 0xd16, 0xd38, 0xd80, 0xdba);
		RegisterOpCode(OPCODE_AddToStruct, 0x11f, 0x1db, 0x5ac, 0x71a, 0xad5, 0xc5d, 0xc9f, 0xcbb, 0xd2b, 0xd30);
		RegisterOpCode(OPCODE_ClassFunctionThreadCallEndOn, 0x120, 0x17f, 0x210, 0x401, 0x4b8, 0x530, 0x5ba, 0x66a, 0x690, 0x6b8, 0x82f, 0x8b9, 0x8be, 0x8d6, 0x8ee,
			0x984, 0x9b2, 0xa25, 0xcab, 0xd21, 0xecc, 0xeea);
		RegisterOpCode(OPCODE_LessThanOrEqualTo, 0x122, 0x182, 0x2b1, 0x2e6, 0x333, 0x3ed, 0x446, 0x560, 0x5b4, 0x5c4, 0x5d7, 0x68d, 0x6fa, 0x985, 0x9a4, 0xc1e, 0xc5f,
			0xc88, 0xd76, 0xe31, 0xe8f);
		RegisterOpCode(OPCODE_FirstArrayKeyCached, 0x123, 0x1d4, 0x1dc, 0x7f9, 0x866, 0xab8, 0xb5f, 0xbea, 0xc40, 0xd29, 0xdfa, 0xe7b, 0xed6);
		RegisterOpCode(OPCODE_Unknown10e, 0x10e, 0x1d7, 0x3e9, 0x418, 0x557, 0x798, 0x7aa, 0x874, 0x977, 0xb29, 0xe0a, 0xe18);
		RegisterOpCode(OPCODE_Unknown126, 0x126, 0x142, 0x169, 0x21c, 0x2b9, 0x417, 0x4ca, 0x52f, 0x60b, 0x6ef, 0x7be, 0x862, 0x893, 0x8da, 0x9dc, 0xa4e, 0xa7f,
			0xaa3, 0xd68, 0xdd6, 0xe81, 0xeac, 0xf94);
		RegisterOpCode(OPCODE_GetGlobal, 0x12c, 0x4f9, 0x5ae, 0x6cd, 0x755, 0x7e4, 0x872, 0x980, 0x994, 0xaaa, 0xabc, 0xaef, 0xaf0, 0xb74, 0xbbc, 0xbf6, 0xc33, 
			0xdec, 0xe72, 0xf13, 0xfb9);
		RegisterOpCode(OPCODE_GetZero, 0x136, 0x2c5, 0x56d, 0x5e1, 0x8e5, 0x9aa, 0xa8b, 0xb16, 0xc28, 0xd87, 0xfa3);
		RegisterOpCode(OPCODE_EvalLocalVariableRefCached, 0x138, 0x236, 0x33d, 0x4a3, 0x53c, 0x589, 0x5a5, 0x5a8, 0x686, 0x6f5, 0x6fd, 0x957, 0xa21, 0xa3b, 0xa86, 0xadb,
			0xb0e, 0xb49, 0xc65, 0xcf4, 0xd50, 0xe32, 0xe5b, 0xec7, 0xfde);
		RegisterOpCode(OPCODE_ClearFieldVariable, 0x14c, 0x27d, 0x4fb, 0x50d, 0x586, 0x5d4, 0x67f, 0x766, 0x852, 0x8bf, 0x97b, 0xb8f, 0xb97, 0xce6, 0xd3c, 0xd7d,
			0xd96, 0xe06, 0xe2f, 0xf6b, 0xff3);
		RegisterOpCode(OPCODE_EvalGlobalObjectFieldVariable, 0x155, 0x1ba, 0x25c, 0x25d, 0x2f2, 0x436, 0x47d, 0x48f, 0x4c5, 0x50b, 0x5fb, 0x616, 0x729, 0x7f7,
			0x891, 0x8b1, 0x90d, 0x98b, 0x9a5, 0xb45, 0xbb3, 0xd0a, 0xd2e, 0xd60, 0xf47);
		RegisterOpCode(OPCODE_EvalFieldObjectFromRef, 0x168, 0x171, 0x1b4, 0x2bf, 0x4d7, 0x509, 0x675, 0x6ab, 0x810, 0x87c, 0x8c6, 0x9e9, 0xa58, 0xb9b, 0xba8,
			0xc22, 0xe3b);
		RegisterOpCode(OPCODE_GetSelf, 0x16e, 0x1be, 0x205, 0x209, 0x252, 0x25a, 0x260, 0x5de, 0x653, 0x72a, 0x763, 0x767, 0x83a, 0x844, 0x846, 0xa54, 0xb4f,
			0xcf9, 0xda7, 0xe76, 0xf56, 0xf88);
		RegisterOpCode(OPCODE_GetResolveFunction, 0x181, 0x222, 0x230, 0x232, 0x2aa, 0x2e0, 0x34d, 0x402, 0x434, 0x4db, 0x4e6, 0x558, 0x670, 0x6f7, 0x745, 0x74c,
			0x7d3, 0x86c, 0x8c1, 0x9ee, 0xa62, 0xa8e, 0xc01, 0xc4a, 0xcb6, 0xcc3, 0xf42, 0xfcc);
		RegisterOpCode(OPCODE_GetGlobalObject, 0x187, 0x1bb, 0x250, 0x2db, 0x30a, 0x338, 0x39d, 0x470, 0x4bb, 0x4ea, 0x51c, 0x54c, 0x5da, 0x60d, 0x6a5, 0x6a9, 
			0x8f6, 0x8ff, 0x92e, 0x947, 0xb1b, 0xc4c, 0xd24, 0xe29, 0xeef, 0xefb, 0xf26);
		RegisterOpCode(OPCODE_GetByte, 0x18a, 0x1ed, 0x476, 0x544, 0x5a6, 0x680, 0x692, 0x9c2, 0xa73, 0xc95, 0xd93, 0xed8, 0xf9b);
		RegisterOpCode(OPCODE_CastFieldObject, 0x18e, 0x193, 0x1f1, 0x39e, 0x3dd, 0x42f, 0x45c, 0x4aa, 0x5b6, 0x5e3, 0x607, 0x791, 0x7ff, 0x82b, 0x8b7, 0x97c, 
			0xb62, 0xc69, 0xc7b, 0xc98, 0xc99, 0xccb, 0xd40, 0xd42);
		RegisterOpCode(OPCODE_EvalLocalVariableDefined, 0x192, 0x1e3, 0x24f, 0x270, 0x2f7, 0x48a, 0x63d, 0x789, 0x7c1, 0x932, 0xab3, 0xb17, 0xbd8, 0xc68, 0xcef, 
			0xd0b, 0xd56, 0xd98);
		RegisterOpCode(OPCODE_ScriptThreadCallPointer, 0x1c5, 0x1f2, 0x21a, 0x325, 0x33b, 0x397, 0x3eb, 0x499, 0x52d, 0x71d, 0x74b, 0x7a1, 0x84c, 0x9ac, 0x9b8,
			0xa4d, 0xac1, 0xb05, 0xc6e, 0xd04, 0xd5f, 0xdd7, 0xe89, 0xeba, 0xfc3, 0xfe1);
		RegisterOpCode(OPCODE_LessThan, 0x1c6, 0x399, 0x54f, 0x5bd, 0x62a, 0x787, 0x92d, 0x93d, 0x99d, 0xab9, 0xbba, 0xc36, 0xe9e, 0xfac);
		RegisterOpCode(OPCODE_ScriptMethodCall, 0x1e6, 0x36f, 0x425, 0x6c4, 0x705, 0x7f2, 0x8b2, 0x8b6, 0x983, 0xa4c, 0xb78, 0xcad, 0xd52);
		RegisterOpCode(OPCODE_DecTop, 0x1f4, 0x53f, 0x561, 0x597, 0x732, 0x851, 0x8cf, 0x965, 0x99c, 0xdd3, 0xee2, 0xef6, 0xf3f, 0xf91);
		RegisterOpCode(OPCODE_GetVector, 0x22f, 0x3fc, 0x628, 0x702, 0x981, 0x9cd, 0xa00, 0xb11, 0xf57, 0xf90);
		RegisterOpCode(OPCODE_ScriptMethodThreadCall, 0x248, 0x53d, 0x590, 0x78e, 0x83d, 0xa34, 0xbe4, 0xc93, 0xcc5, 0xd97, 0xe75);
		RegisterOpCode(OPCODE_SetNextArrayKeyCached, 0x26b, 0x320, 0x331, 0x383, 0x491, 0x4c1, 0x4fc, 0x564, 0x610, 0x71e, 0x72d, 0x80f, 0x8ea, 0x929, 0x971,
			0x990, 0x9cf, 0xaa1, 0xae0, 0xbb4, 0xbef, 0xc8b, 0xcfc, 0xe3d, 0xe53, 0xee3, 0xf54, 0xf66, 0xfc8, 0xff5);
		RegisterOpCode(OPCODE_JumpPush, 0x2d2, 0x3ab, 0x61e, 0x70e, 0x90f, 0xc31, 0xc32, 0xc94, 0xf8c, 0xff9);
		RegisterOpCode(OPCODE_GetUnsignedShort, 0x307, 0x312, 0x354, 0x428, 0x4cc, 0x5c2, 0x64b, 0x735, 0x7c6, 0x96b, 0xa09, 0xa53, 0xacc, 0xb86, 0xc0d, 0xc73, 0xd13, 0xd65, 0xf63, 0xff2);
		RegisterOpCode(OPCODE_EvalLocalVariableCached, 0x332, 0x3ae, 0x528, 0x5ca, 0x5d1, 0x689, 0x6b5, 0x741, 0x829, 0x8cc, 0x8e2, 0x919, 0xa31, 0xac2, 0xc3c, 0xc60, 0xc8d, 0xd32, 0xf8e);
		RegisterOpCode(OPCODE_EvalArrayRef, 0x3c1, 0x3da, 0x4a8, 0x4ce, 0x527, 0x5c9, 0x6a8, 0x782, 0x884, 0x887, 0x9a2, 0xa9d, 0xaae, 0xae6, 0xb06, 0xb35, 0xc85, 0xce4, 0xdea, 0xf05, 0xfe6);
		RegisterOpCode(OPCODE_EvalFieldVariable, 0x400, 0x4a9, 0x548, 0x551, 0x68b, 0x746, 0x7b3, 0x7c4, 0xd4f, 0xe1a, 0xed1);
		RegisterOpCode(OPCODE_GetFunction, 0x432, 0x5d8, 0x6a1, 0x718, 0xb64, 0xb9c, 0xc91, 0xcb1, 0xedb, 0xf16);

		// T8-Compiler opcodes
		RegisterOpCode(OPCODE_T8C_GetLazyFunction, 0x16);
	});
}

void tool::gsc::opcode::RegisterOpCodeHandler(const opcodeinfo* info) {
	g_opcodeHandlerMap[info->m_id] = info;
}

const opcodeinfo* tool::gsc::opcode::LookupOpCode(UINT16 opcode) {
	// build map
	RegisterOpCodes();
	auto ref = g_opcodeMap.find(opcode & 0xFFF);

	if (ref == g_opcodeMap.end()) {
		return g_unknownOpcode;
	}

	const auto refHandler = g_opcodeHandlerMap.find(ref->second);

	if (refHandler == g_opcodeHandlerMap.end()) {
		return g_unknownOpcode;
	}

	return refHandler->second;
}

asmcontext::asmcontext(BYTE* fonctionStart, const GscInfoOption& opt, UINT32 nsp, const T8GSCExport& exp)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_opt(opt), m_runDecompiler(opt.m_dcomp), 
			m_lastOpCodeBase(-1), m_namespace(nsp), m_funcBlock(false), m_exp(exp) {
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

asmcontextlocation::asmcontextlocation() : handled(false), ref(0), rloc(0) {}
asmcontextlocation::~asmcontextlocation() {
	if (fieldId) {
		delete fieldId;
	}
	if (objectId) {
		delete objectId;
	}
	for (auto& ref : m_stack) {
		delete ref;
	}
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

void asmcontext::SearchDefaultParamValue() {
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

		auto* jump = dynamic_cast<asmcontextnode_JumpOperator*>(param1);
		auto* set = dynamic_cast<asmcontextnode_LeftRightOperator*>(it->node);
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

		auto* isDefinedFunc = dynamic_cast<asmcontextnode_FunctionOperator*>(jump->m_operand);

		if (isDefinedFunc->m_operand->m_type != TYPE_IDENTIFIER || set->m_left->m_type != TYPE_IDENTIFIER || !set->m_right) {
			break; // not isdefined(param_name) or not param_name = ...
		}
		UINT32 name = dynamic_cast<asmcontextnode_Identifier*>(set->m_left)->m_value;

		if (dynamic_cast<asmcontextnode_Identifier*>(isDefinedFunc->m_operand)->m_value != name) {
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
			jumpEndLocation->ref--;
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

std::ostream& decompcontext::WritePadding(std::ostream& out) {
	for (size_t i = 0; i < padding; i++) {
		out << "    ";
	}
	return out;
}

void asmcontextnodeblock::Dump(std::ostream& out, decompcontext& ctx) const {
	if (m_devBlock) {
		out << "/#\n";
	}
	else {
		out << "{\n";
	}

	ctx.padding++;

	// decompiler proto block loop

	for (size_t i = 0; i < m_statements.size(); i++) {
		const auto& ref = m_statements[i];
		if (ref.location->ref) {
			// write the label one layer bellow the current block
			ctx.padding--;
			ctx.WritePadding(out) << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(INT32) << 1) << ref.location->rloc << ":\n";
			ctx.padding++;
		}
		if (ref.node->m_type != TYPE_END) {
			if (ref.node->m_type != TYPE_PRECODEPOS) {
				ctx.WritePadding(out);
				ref.node->Dump(out, ctx);

				if (ref.node->m_type != TYPE_BLOCK) {
					out << ";\n";
				}
			}
			else if (ctx.asmctx.m_opt.m_show_internal_blocks) {
				ctx.WritePadding(out) << "<precodepos>;\n";
			}
		}
		else if (i != m_statements.size() - 1) {
			ctx.WritePadding(out);
			// if we're not at the end, it means we are reading a return;
			out << "return;\n";
		}
		else if (ctx.asmctx.m_opt.m_show_internal_blocks) {
			ctx.WritePadding(out) << "<end>;\n";
		}
		out << std::flush;
	}

	ctx.padding--;

	if (m_devBlock) {
		ctx.WritePadding(out) << "#/\n";
	}
	else {
		ctx.WritePadding(out) << "}\n";
	}
}

int asmcontextnodeblock::ComputeDevBlocks() {
	auto it = m_statements.begin();
	while (it != m_statements.end()) {
		auto& b = *it;
		if (b.node->m_type != TYPE_JUMP_DEVBLOCK) {
			it++;
			continue;
		}

		// we are reading a devblock jump
		auto* jump = dynamic_cast<asmcontextnode_JumpOperator*>(b.node);
		INT64 end = jump->m_location;
		asmcontextnodeblock* devBlock = new asmcontextnodeblock(true);
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
			devBlock->m_statements.push_back({ new asmcontextnode_Value<LPCCH>("<precodepos>", TYPE_PRECODEPOS), it->location });
			// remove the dev block jump reference
			it->location->ref--;
		}

		// compute nested dev blocks
		devBlock->ComputeDevBlocks();

		// delete the jump operator
		delete jump;
	}

	return 0;
}