#pragma once
#include "gsc.hpp"

namespace tool::gsc::opcode {
	size_t SizeNoEmptyNode(const std::vector<ASMContextStatement>& statements);
	ASMContextStatement* GetNoEmptyNode(std::vector<ASMContextStatement>& statements, size_t index);
	ASMContextNode* ASMCNodeConvertToBool(ASMContextNode* node, ASMContext& ctx);

	inline bool IsBlockInlineable(const ASMContextNodeBlock* blk, const tool::gsc::GscInfoOption& opt) {
		if (!blk) return false;
		if ((opt.m_formatter->flags & tool::gsc::formatter::FFL_BLOCK_INLINE) && blk->m_allowInline) {
			size_t count{};
			for (size_t i = 0; i < blk->m_statements.size(); i++) {
				if (blk->m_statements[i].node->m_type != TYPE_PRECODEPOS && count++) {
					break;
				}
			}
			return count == 1;
		}
		return false;
	}

	enum ASMContextNodeValueVirDataType {
		ASCNVD_UNDEFINED = 0,
		ASCNVD_INT,
		ASCNVD_FLOAT,
		ASCNVD_STRING,
	};

	class ASMContextNodeValueVir : public ASMContextNode {
	public:
		using ASMContextNode::ASMContextNode;
		virtual void SetNumber(double val) = 0;
		virtual void SetNumber(int64_t val) = 0;
		virtual ASMContextNodeValueVirDataType GetDatatype() const = 0;
	};

	template<typename Type>
	class ASMContextNodeValue : public ASMContextNodeValueVir {
	public:
		Type m_value;
		bool m_hex;
		bool m_canBeCastToBool;
		bool m_isIntConst;
		ASMContextNodeValue(Type value, ASMContextNodeType type, bool hex = false, bool canBeCastToBool = false, bool isIntConst = false)
			: ASMContextNodeValueVir(PRIORITY_VALUE, type), m_value(value), m_hex(hex), m_canBeCastToBool(canBeCastToBool), m_isIntConst(isIntConst) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_hex) {
				out << std::hex << "0x";
			}
			else {
				out << std::dec;
			}
			out << m_value;
		}


		ASMContextNode* ConvertToBool() override {
			if (!m_canBeCastToBool) {
				return nullptr;
			}

			if (m_value) {
				return new ASMContextNodeValue<const char*>("true", TYPE_VALUE);
			}
			return new ASMContextNodeValue<const char*>("false", TYPE_VALUE);
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			if (!strict) {
				return m_canBeCastToBool;
			}

			return m_canBeCastToBool && (!m_value || m_value == (Type)(1));
		}
		bool IsIntConst() const override {
			return m_isIntConst;
		}

		int64_t GetIntConst() const override {
			return (int64_t)m_value;
		}

		void SetNumber(double val) override {
			if (IsConstNumber()) {
				if constexpr (std::is_arithmetic<Type>::value) {
					m_value = (Type)val;
				}
			}
		}

		void SetNumber(int64_t val) override {
			if (IsConstNumber()) {
				m_value = (Type)val;
			}
		}

		ASMContextNodeValueVirDataType GetDatatype() const override {
			if constexpr (std::is_integral<Type>::value) {
				return ASCNVD_INT;
			}

			if constexpr (std::is_floating_point<Type>::value) {
				return ASCNVD_FLOAT;
			}
			if constexpr (std::is_same<Type, const char*>::value) {
				return ASCNVD_STRING;
			}
			return ASCNVD_UNDEFINED;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeValue<Type>(m_value, m_type, m_hex, m_canBeCastToBool, m_isIntConst);
		}
	};

	class ASMContextNodeIdentifier : public ASMContextNode {
	public:
		uint64_t m_value;
		const char* m_type;
		ASMContextNodeIdentifier(uint64_t value, const char* type = "var") : ASMContextNode(PRIORITY_VALUE, TYPE_IDENTIFIER), m_value(value), m_type(type) {
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
		uint64_t m_value;
		bool m_canonid;
		const char* m_type;
		ASMContextNodeHash(uint64_t value, bool canonid = false, const char* type = "#") : m_type(type), ASMContextNode(PRIORITY_VALUE, TYPE_CONST_HASH), m_value(value), m_canonid(canonid) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_canonid) {
				out << m_type << "" << hashutils::ExtractTmp("var", m_value) << std::flush;
			}
			else {
				out << m_type << "\"" << hashutils::ExtractTmp("hash", m_value) << '"' << std::flush;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeHash(m_value, m_canonid, m_type);
		}
	};

	class ASMContextNodeVectorN : public ASMContextNode {
	public:
		typedef ASMContextNode* NodePtr;
		NodePtr* nodes;
		size_t count;
		ASMContextNodeVectorN(ASMContextNode** nodes, size_t count) : ASMContextNode(PRIORITY_VALUE, TYPE_VECTORN), count(count), nodes(nodes) {
		}
		ASMContextNodeVectorN(float* vector, size_t count) : ASMContextNode(PRIORITY_VALUE, TYPE_VECTORN), count(count) {
			nodes = new NodePtr[count];
			for (size_t i = 0; i < count; i++) {
				nodes[i] = new ASMContextNodeValue<float>(vector[i], TYPE_FLOAT);
			}
		}
		~ASMContextNodeVectorN() {
			for (size_t i = 0; i < count; i++) {
				delete nodes[i];
			}
			delete[] nodes;
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			for (size_t i = 0; i < count; i++) {
				if (i)
					out << ", ";
				nodes[i]->Dump(out, ctx);;
			}

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
		}

		ASMContextNode* Clone() const override {
			ASMContextNode** copy = new NodePtr[count];
			for (size_t i = 0; i < count; i++) {
				copy[i] = nodes[i]->Clone();
			}
			return new ASMContextNodeVectorN(copy, count);
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			for (size_t i = 0; i < count; i++) {
				if (!nodes[i]) continue;

				func(nodes[i], ctx);
				nodes[i]->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeVector : public ASMContextNode {
	public:
		ASMContextNode* m_x;
		ASMContextNode* m_y;
		ASMContextNode* m_z;
		ASMContextNodeVector(ASMContextNode* x, ASMContextNode* y, ASMContextNode* z) : ASMContextNode(PRIORITY_VALUE, TYPE_VECTOR), m_x(x), m_y(y), m_z(z) {
		}
		ASMContextNodeVector(float x, float y, float z) : ASMContextNodeVector(
			new ASMContextNodeValue<float>(x, TYPE_FLOAT),
			new ASMContextNodeValue<float>(y, TYPE_FLOAT),
			new ASMContextNodeValue<float>(z, TYPE_FLOAT)
			) {
		}
		~ASMContextNodeVector() {
			delete m_x;
			delete m_y;
			delete m_z;
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_x->Dump(out, ctx);
			out << ", ";
			m_y->Dump(out, ctx);
			out << ", ";
			m_z->Dump(out, ctx);

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeVector(m_x->Clone(), m_y->Clone(), m_z->Clone());
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_x) {
				func(m_x, ctx);
				m_x->ApplySubNodes(func, ctx);
			}
			if (m_y) {
				func(m_y, ctx);
				m_y->ApplySubNodes(func, ctx);
			}
			if (m_z) {
				func(m_z, ctx);
				m_z->ApplySubNodes(func, ctx);
			}
		}
	};

	inline bool ASMContextNodeStringHasSpace(const char* str) {
		if (!*str) {
			return false;
		}
		do {
			if (isspace(*(str++))) {
				return true;
			}
		} while (*str);
		return false;
	}

	class ASMContextNodeString : public ASMContextNode {
	public:
		const char* m_value;
		const char* m_prefix;
		bool m_allowNoQuote;
		ASMContextNodeString(const char* value, const char* prefix = nullptr, bool allowNoQuote = false)
			: ASMContextNode(PRIORITY_VALUE), m_value(value), m_prefix(prefix), m_allowNoQuote(allowNoQuote) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			bool quotes{ !m_allowNoQuote || ASMContextNodeStringHasSpace(m_value) };
			if (m_prefix) {
				out << m_prefix;
			}

			if (quotes) out << "\"";
			utils::PrintFormattedString(out, m_value);
			if (quotes) out << "\"";
			out << std::flush;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeString(m_value, m_prefix, m_allowNoQuote);
		}
	};

	class ASMContextNodeAnimation : public ASMContextNode {
	public:
		const char* m_str1;
		const char* m_str2;
		ASMContextNodeAnimation(const char* str1, const char* str2) : ASMContextNode(PRIORITY_VALUE), m_str1(str1), m_str2(str2) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {

			utils::PrintFormattedString(out, m_str1);
			out << "#";
			utils::PrintFormattedString(out, m_str2);
			out << std::flush;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeAnimation(m_str1, m_str2);
		}
	};

	class ASMContextNodeFuncRef : public ASMContextNode {
	public:
		const char* m_op;
		uint64_t m_func;
		uint64_t m_nsp;
		uint64_t m_script;
		ASMContextNodeFuncRef(const char* op, uint64_t func, uint64_t nsp = 0, uint64_t script = 0) : ASMContextNode(PRIORITY_VALUE, TYPE_FUNC_REFNAME),
			m_op(op), m_func(func), m_nsp(nsp), m_script(script) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << m_op;
			if (m_nsp) {
				out << hashutils::ExtractTmpPath("namespace", m_nsp) << std::flush;
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
		const char* m_op;
		ASMContextNode* m_var;
		ASMContextNodeRef(const char* op, ASMContextNode* var) : ASMContextNode(PRIORITY_VALUE, TYPE_GET_REF),
			m_op(op), m_var(var) {
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << m_op;
			m_var->Dump(out, ctx);
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeRef(m_op, m_var->Clone());
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_var) {
				func(m_var, ctx);
				m_var->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeArrayAccess : public ASMContextNode {
	public:
		ASMContextNode* m_operandleft;
		ASMContextNode* m_operandright;
		ASMContextNodeArrayAccess(ASMContextNode* operandleft, ASMContextNode* operandright) :
			ASMContextNode(PRIORITY_ACCESS, TYPE_ARRAY_ACCESS), m_operandleft(operandleft), m_operandright(operandright) {
		}
		~ASMContextNodeArrayAccess() {
			delete m_operandleft;
			delete m_operandright;
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_operandleft->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operandleft->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operandleft->Dump(out, ctx);
			}
			out << "[";

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_operandright->Dump(out, ctx);

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << "]";
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeArrayAccess(m_operandleft->Clone(), m_operandright->Clone());
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_operandleft) {
				func(m_operandleft, ctx);
				m_operandleft->ApplySubNodes(func, ctx);
			}
			if (m_operandright) {
				func(m_operandright, ctx);
				m_operandright->ApplySubNodes(func, ctx);
			}
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

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_key) {
				func(m_key, ctx);
				m_key->ApplySubNodes(func, ctx);
			}
			if (m_value) {
				func(m_value, ctx);
				m_value->ApplySubNodes(func, ctx);
			}
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
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}

				for (size_t i = 0; i < m_operands.size(); i++) {
					if (i) out << ", ";

					auto& item = m_operands[i];
					if (item->m_key->IsIntConst() && item->m_key->GetIntConst() == i) {
						// we can omit this key because it matches the index
						item->m_value->Dump(out, ctx);
					}
					else {
						item->Dump(out, ctx);
					}
				}
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
			}

			out << "]";
		}

		void AddValue(ASMContextNode* key, ASMContextNode* value, bool start) {
			if (start) {
				m_operands.insert(m_operands.begin(), new ASMContextNodeArrayBuildNode(key, value));
			}
			else {
				m_operands.push_back(new ASMContextNodeArrayBuildNode(key, value));
			}
		}

		ASMContextNode* Clone() const override {
			auto* n = new ASMContextNodeArrayBuild();

			for (const auto& ref : m_operands) {
				n->AddValue(ref->m_key->Clone(), ref->m_value->Clone(), false);
			}

			return n;
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			for (ASMContextNodeArrayBuildNode*& ref : m_operands) {
				if (ref) {
					func(reinterpret_cast<ASMContextNode*&>(ref), ctx);
					ref->ApplySubNodes(func, ctx);
				}
			}
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
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}

				for (size_t i = 0; i < m_operands.size(); i++) {
					if (i) out << ", ";
					m_operands[i]->Dump(out, ctx);
				}

				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
			}

			out << "}";
		}

		void AddValue(ASMContextNode* key, ASMContextNode* value, bool start) {
			if (key->m_type == TYPE_CONST_HASH) {
				// this hash is a canon id, not a fnva1
				static_cast<ASMContextNodeHash*>(key)->m_canonid = true;
			}
			if (start) {
				m_operands.insert(m_operands.begin(), new ASMContextNodeArrayBuildNode(key, value));
			}
			else {
				m_operands.push_back(new ASMContextNodeArrayBuildNode(key, value));
			}
		}

		ASMContextNode* Clone() const override {
			auto* n = new ASMContextNodeStructBuild();

			for (const auto& ref : m_operands) {
				n->AddValue(ref->m_key->Clone(), ref->m_value->Clone(), false);
			}

			return n;
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			for (ASMContextNodeArrayBuildNode*& ref : m_operands) {
				if (ref) {
					func(reinterpret_cast<ASMContextNode*&>(ref), ctx);
					ref->ApplySubNodes(func, ctx);
				}
			}
		}
	};

	enum ASMContextNodeCallFuncPtrType {
		CLASS_CALL,
		PARAMS_CALL,
		POINTER_CALL,
		FUNCTION_CALL
	};

	enum ASMContextNodeCallFuncFlag : byte {
		THREAD_CALL = 1,
		CHILDTHREAD_CALL = 2,
		SELF_CALL = 4,
		BUILTIN_CALL = 8
	};

	class ASMContextNodeCallFuncPtr : public ASMContextNode {
	public:
		ASMContextNodeCallFuncPtrType m_ftype;
		byte m_flags;
		std::vector<ASMContextNode*> m_operands{};
		ASMContextNodeCallFuncPtr(ASMContextNodeCallFuncPtrType type, byte flags, ASMContextNodeType nodeType = TYPE_FUNC_CALL) : ASMContextNode(PRIORITY_ACCESS, nodeType), m_ftype(type), m_flags(flags) {
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
			auto* ref = new ASMContextNodeCallFuncPtr(m_ftype, m_flags, m_type);
			ref->m_operands.reserve(m_operands.size());
			for (const auto& op : m_operands) {
				ref->m_operands.push_back(op->Clone());
			}

			return ref;
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			size_t idx;
			if (m_flags & SELF_CALL) {
				idx = 1;
			}
			else {
				idx = 0;
			}
			if (idx >= m_operands.size()) return false; // wtf
			if (m_operands[idx]->m_type != TYPE_FUNC_REFNAME) return false; // not a named call

			ASMContextNodeFuncRef* idf{ static_cast<ASMContextNodeFuncRef*>(m_operands[idx]) };
			//idf->m_func
			NameLocated loc{};
			loc.name = idf->m_func;
			loc.name_space = idf->m_nsp ? idf->m_nsp : ctx.m_namespace;

			if (ctx.m_objctx.gdctx.asyncMtx) {
				// mutex, we check first the sync data
				auto it{ ctx.m_objctx.contextes.find(loc) };

				if (it != ctx.m_objctx.contextes.end()) {
					// if the other function is a bool candidate, we can return this one as a bool candidate too
					return it->second.m_boolFuncCandidate;
				}
			}
			{
				core::async::opt_lock_guard lg{ ctx.m_objctx.gdctx.asyncMtx };

				auto it{ ctx.m_objctx.gdctx.exportInfos.find(loc) };

				if (it != ctx.m_objctx.gdctx.exportInfos.end()) {
					return it->second.boolFunc; // it is a bool somewhere else
				}
			}
			return false; // not seen
		}

		ASMContextNode* ConvertToBool() {
			return this;
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
			else if (m_flags & BUILTIN_CALL) {
				out << "builtin ";
			}

			switch (m_ftype) {
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
				out << "<error CallFuncPtrType:" << m_ftype << ">";
				break;
			}

			out << "(";
			if (start < m_operands.size()) {
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}

				for (size_t i = start; i < m_operands.size(); i++) {
					if (i != start) {
						out << ", ";
					}
					m_operands[i]->Dump(out, ctx);
				}

				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
			}
			out << ")";
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			for (auto& ref : m_operands) {
				if (ref) {
					func(ref, ctx);
					ref->ApplySubNodes(func, ctx);
				}
			}
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
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand1->Dump(out, ctx);

				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand1->Dump(out, ctx);
			}

			out << " ? ";

			if (m_operand2->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand2->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand2->Dump(out, ctx);
			}

			out << " : ";

			if (m_operand3->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand3->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand3->Dump(out, ctx);
			}

		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_operand1) {
				func(m_operand1, ctx);
				m_operand1->ApplySubNodes(func, ctx);
			}
			if (m_operand2) {
				func(m_operand2, ctx);
				m_operand2->ApplySubNodes(func, ctx);
			}
			if (m_operand3) {
				func(m_operand3, ctx);
				m_operand3->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeOp2 : public ASMContextNode {
	public:
		const char* m_description;
		ASMContextNode* m_operand1;
		ASMContextNode* m_operand2;
		bool m_isBoolValue;
		ASMContextNodeOp2(const char* description, ASMContextNodePriority priority, ASMContextNode* operand1, ASMContextNode* operand2, bool isBoolValue, ASMContextNodeType type = TYPE_UNDEFINED) :
			ASMContextNode(priority, type), m_description(description), m_operand1(operand1), m_operand2(operand2), m_isBoolValue(isBoolValue) {
		}
		~ASMContextNodeOp2() {
			delete m_operand1;
			delete m_operand2;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeOp2(m_description, m_priority, m_operand1->Clone(), m_operand2->Clone(), m_isBoolValue, m_type);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_operand1->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand1->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand1->Dump(out, ctx);
			}

			out << " " << m_description << " ";

			if (m_operand2->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand2->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand2->Dump(out, ctx);
			}
		}

		ASMContextNode* ConvertToBool() override {
			return m_isBoolValue ? this : nullptr;
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			return m_isBoolValue;
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_operand1) {
				func(m_operand1, ctx);
				m_operand1->ApplySubNodes(func, ctx);
			}
			if (m_operand2) {
				func(m_operand2, ctx);
				m_operand2->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeOp1 : public ASMContextNode {
	public:
		const char* m_description;
		bool m_prefix;
		ASMContextNode* m_operand;
		bool m_isBoolValue;
		ASMContextNodeOp1(const char* description, bool prefix, ASMContextNode* operand, ASMContextNodeType type = TYPE_UNDEFINED, bool isBoolValue = false) :
			ASMContextNode(PRIORITY_UNARY, type), m_prefix(prefix), m_description(description), m_operand(operand), m_isBoolValue(isBoolValue) {
			assert(operand);
		}
		~ASMContextNodeOp1() {
			delete m_operand;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeOp1(m_description, m_prefix, m_operand->Clone(), m_type, m_isBoolValue);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_prefix) {
				out << m_description;
			}
			if (m_operand->m_priority < m_priority) {
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_operand->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
			}
			else {
				m_operand->Dump(out, ctx);
			}

			if (!m_prefix) {
				out << m_description;
			}
		}

		ASMContextNode* ConvertToBool() override {
			return m_isBoolValue ? this : nullptr;
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			return m_isBoolValue;
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_operand) {
				func(m_operand, ctx);
				m_operand->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeMultOp : public ASMContextNode {
	public:
		const char* m_description;
		bool m_caller;
		bool m_isBoolVal;
		bool m_noParenthesis;
		std::vector<ASMContextNode*> m_operands{};
		ASMContextNodeMultOp(const char* description, bool caller, ASMContextNodeType type = TYPE_STATEMENT, bool isBoolVal = false, bool noParenthesis = false) :
			ASMContextNode(PRIORITY_VALUE, type), m_description(description), m_caller(caller), m_isBoolVal(isBoolVal), m_noParenthesis(noParenthesis) {
		}
		~ASMContextNodeMultOp() {
			for (auto& ref : m_operands) {
				delete ref;
			}
		}

		ASMContextNode* Clone() const override {
			auto* ref = new ASMContextNodeMultOp(m_description, m_caller, m_type, m_isBoolVal, m_noParenthesis);
			ref->m_operands.reserve(m_operands.size());
			for (const auto& op : m_operands) {
				ref->m_operands.push_back(op->Clone());
			}

			return ref;
		}

		ASMContextNode* ConvertToBool() override {
			return m_isBoolVal ? this : nullptr;
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			return m_isBoolVal;
		}

		void AddParam(ASMContextNode* node) {
			m_operands.push_back(node);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			size_t start = 0;
			if (m_caller) {
				if (!m_operands.size()) {
					LOG_ERROR("empty MultOp {}", m_description);
					return; // wtf?
				}
				m_operands[0]->Dump(out, ctx);
				out << " ";
				start = 1;
			}
			out << m_description;
			if (!m_noParenthesis) {
				out << "(";

				if (start < m_operands.size() && (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
					out << " ";
				}
			}
			else if (start < m_operands.size()) {
				out << " "; // at least one param
			}
			
			for (size_t i = start; i < m_operands.size(); i++) {
				const auto& operand = m_operands[i];
				if (i != start) {
					out << ", ";
				}
				operand->Dump(out, ctx);
			}

			if (!m_noParenthesis) {
				if (start < m_operands.size() && (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS)) {
					out << " ";
				}
				out << ")";
			}
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			for (auto& ref : m_operands) {
				if (ref) {
					func(ref, ctx);
					ref->ApplySubNodes(func, ctx);
				}
			}
		}
	};

	class ASMContextNodeFunctionOperator : public ASMContextNode {
	public:
		const char* m_operatorName;
		ASMContextNode* m_self;
		ASMContextNode* m_operand;
		bool m_isBoolVal;
		ASMContextNodeFunctionOperator(const char* operatorName, ASMContextNode* self, ASMContextNode* operand, ASMContextNodeType type = TYPE_STATEMENT, bool isBoolVal = false) :
			ASMContextNode(PRIORITY_VALUE, type), m_operatorName(operatorName), m_self(self), m_operand(operand), m_isBoolVal(isBoolVal) {
			assert(operand);
		}
		~ASMContextNodeFunctionOperator() {
			if (m_self) {
				delete m_self;
			}
			delete m_operand;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeFunctionOperator(m_operatorName, m_self ? m_self->Clone() : nullptr, m_operand->Clone(), m_type, m_isBoolVal);
		}

		ASMContextNode* ConvertToBool() override {
			return m_isBoolVal ? this : nullptr;
		}

		bool IsBoolConvertable(bool strict, ASMContext& ctx) override {
			return m_isBoolVal;
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_self) {
				m_self->Dump(out, ctx);
				out << " ";
			}
			out << m_operatorName;
			out << "(";

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_operand->Dump(out, ctx);

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_self) {
				func(m_self, ctx);
				m_self->ApplySubNodes(func, ctx);
			}
			if (m_operand) {
				func(m_operand, ctx);
				m_operand->ApplySubNodes(func, ctx);
			}
		}
	};

	enum ASMContextNodeJumpOperatorSpecialJump {
		SPECIAL_JUMP_DEFAULT = 0,
		SPECIAL_JUMP_BREAK,
		SPECIAL_JUMP_CONTINUE
	};

	class ASMContextNodeJumpOperator : public ASMContextNode {
	public:
		const char* m_operatorName;
		ASMContextNode* m_operand;
		int64_t m_location;
		int32_t m_opLoc;
		bool m_showJump;
		bool m_returnCandidate;
		ASMContextNodeJumpOperatorSpecialJump m_special_op;
		int64_t m_delta;

		bool IsFakeIf() const {
			return m_special_op && m_operand;
		}

		ASMContextNodeJumpOperator(const char* operatorName, ASMContextNode* operand, int64_t location, ASMContextNodeType type, int32_t opLoc, bool showJump = true, int64_t delta = 0, ASMContextNodeJumpOperatorSpecialJump special_op = SPECIAL_JUMP_DEFAULT, bool returnCandidate = false) :
			ASMContextNode(PRIORITY_INST, type), m_operatorName(operatorName), m_operand(operand), m_location(location), m_opLoc(opLoc), m_showJump(showJump), m_delta(delta), m_special_op(special_op), m_returnCandidate(returnCandidate) {
			assert(IsJumpType(type));
			m_renderSemicolon = !IsFakeIf();
		}
		~ASMContextNodeJumpOperator() {
			if (m_operand) {
				delete m_operand;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeJumpOperator(m_operatorName, m_operand ? m_operand->Clone() : nullptr, m_location, m_type, m_opLoc, m_showJump, m_delta, m_special_op, m_returnCandidate);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			// we don't show the jump if asked, continue/break?
			if (ctx.opt.m_show_jump_delta) {
				out << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << m_opLoc << ":";
				if (!m_showJump) {
					out << "(hide)";
				}
			}

			// fake an if if the jump isn't a cond jump
			bool fakeIf{ IsFakeIf() };

			if (m_returnCandidate && ctx.opt.m_show_jump_delta) {
				out << "<return>";
			}

			if (!fakeIf) {
				out << m_operatorName;
			}

			if (m_operand) {
				if (m_operand->m_type != TYPE_JUMP_STACK_TOP) {
					if (fakeIf) {
						out << "if";

						if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
							out << " ";
						}
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
				else if (ctx.opt.m_show_jump_delta) {
					m_operand->Dump(out, ctx);
					out << " stored:(";
					static_cast<ASMContextNodeValue<ASMContextNode*>*>(m_operand)->m_value->Dump(out, ctx);
					out << ")";
				}

			}
			if (m_showJump || ctx.opt.m_show_jump_delta) {
				out << " LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << m_location;
				if (!m_showJump) {
					out << "(hide)";
				}
			}
			if (ctx.opt.m_show_jump_delta) {
				out << " delta: " << std::hex << (m_delta < 0 ? "-0x" : "0x") << (m_delta < 0 ? -m_delta : m_delta) << " (type:" << m_type << "/spec:" << (m_special_op ? "true" : "false") << ")";
			}
			if (fakeIf) {
				out << ";\n";
				ctx.padding--;
				ctx.WritePadding(out);
				out << "}";
			}
			if (!m_renderSemicolon) {
				out << "\n";
			}
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_operand) {
				func(m_operand, ctx);
				m_operand->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeCodeRef : public ASMContextNode {
	public:
		int64_t m_location;

		ASMContextNodeCodeRef(int64_t location) :
			ASMContextNode(PRIORITY_VALUE, TYPE_GET_CODE_REF), m_location(location) {
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeCodeRef(m_location);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << m_location;
		}
	};

	bool IsJumpConditionForceReversed(const ASMContextNodeJumpOperator* op);
	ASMContextNode* JumpCondition(ASMContextNodeJumpOperator* op, bool reversed);

	class ASMContextNodeLeftRightOperator : public ASMContextNode {
	public:
		std::string m_operatorName;
		ASMContextNode* m_left;
		ASMContextNode* m_right;
		ASMContextNodeLeftRightOperator(ASMContextNode* left, ASMContextNode* right, const std::string& operatorName = " = ", ASMContextNodePriority priority = PRIORITY_SET, ASMContextNodeType type = TYPE_SET) :
			ASMContextNode(priority, type), m_operatorName(operatorName), m_left(left), m_right(right) {
		}
		~ASMContextNodeLeftRightOperator() {
			if (m_left) delete m_left;
			if (m_right) delete m_right;
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeLeftRightOperator(m_left->Clone(), m_right->Clone(), m_operatorName, m_priority, m_type);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			m_left->Dump(out, ctx);

			out << m_operatorName;

			m_right->Dump(out, ctx);
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_left) {
				func(m_left, ctx);
				m_left->ApplySubNodes(func, ctx);
			}
			if (m_right) {
				func(m_right, ctx);
				m_right->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeNew : public ASMContextNode {
	public:
		uint32_t m_classname;
		ASMContextNode* m_constructorCall;
		bool m_constructorCallDec;
		ASMContextNodeNew(uint32_t clsName, ASMContextNode* constructorCall = nullptr, bool constructorCallDec = false) :
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

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_constructorCall) {
				func(m_constructorCall, ctx);
				m_constructorCall->ApplySubNodes(func, ctx);
			}
		}
	};

	struct ASMContextSwitchPreComputeCase {
		ASMContextNode* casenode = nullptr;
		int32_t jumpLocation = 0;
	};

	class ASMContextNodeSwitchPreCompute : public ASMContextNode {
	public:
		ASMContextNode* m_node;
		std::vector<ASMContextSwitchPreComputeCase> m_cases{};
		int32_t m_endLocation;
		ASMContextNodeSwitchPreCompute(ASMContextNode* node, int32_t endLocation = 0) :
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

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_node->Dump(out, ctx);
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}
			out << "{\n";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_PAD_CASES) {
				ctx.padding++;
			}

			for (const auto& cs : m_cases) {
				if (cs.casenode) {
					ctx.WritePadding(out, true) << "case ";
					cs.casenode->Dump(out, ctx);
				}
				else {
					ctx.WritePadding(out, true) << "default";
				}
				out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << cs.jumpLocation << ";\n";
			}

			ctx.WritePadding(out, true) << "end: ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << m_endLocation << ";\n";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_PAD_CASES) {
				ctx.padding--;
			}
			//
			ctx.WritePadding(out, true) << "}\n";
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_node) {
				func(m_node, ctx);
				m_node->ApplySubNodes(func, ctx);
			}
			for (auto& cse : m_cases) {
				if (cse.casenode) {
					func(cse.casenode, ctx);
					cse.casenode->ApplySubNodes(func, ctx);
				}
			}
		}
	};

	struct ASMContextSwitchPostComputeCase {
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
			out << "switch";
			if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
				out << " ";
			}
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_node->Dump(out, ctx);
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}
			out << "{\n";

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_PAD_CASES) {
				ctx.padding++;
			}

			for (const auto& cs : m_cases) {
				if (cs.value) {
					ctx.WritePadding(out, true) << "case ";
					cs.value->Dump(out, ctx);
					out << ":";
				}
				else {
					ctx.WritePadding(out, true) << "default:";
				}
				bool emptyBlock{ true };
				for (const auto& stmt : cs.block->m_statements) {
					if (stmt.node->m_type != TYPE_PRECODEPOS) {
						emptyBlock = false;
						break;
					}
				}
				if (emptyBlock) {
					out << "\n";
				}
				else {
					if ((ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) && (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_FORCE_BLOCKS)) {
						ctx.WritePadding(out << "\n", true);
					}
					cs.block->Dump(out, ctx);
					if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_FORCE_BLOCKS_PADDING) {
						ctx.WritePadding(out, true) << "\n";
					}
				}
			}
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_PAD_CASES) {
				ctx.padding--;
			}

			ctx.WritePadding(out, true) << "}\n";
		}

		void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
			for (const auto& cs : m_cases) {
				cs.block->ApplySubBlocks(func, ctx);
			}
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_node) {
				func(m_node, ctx);
				m_node->ApplySubNodes(func, ctx);
			}
			for (auto& cse : m_cases) {
				if (cse.value) {
					func(cse.value, ctx);
					cse.value->ApplySubNodes(func, ctx);
				}
				if (cse.block) {
					func(reinterpret_cast<ASMContextNode*&>(cse.block), ctx);
					cse.block->ApplySubNodes(func, ctx);
				}
			}
		}
	};

	class ASMContextNodeForEach : public ASMContextNode {
	public:
		uint64_t m_key;
		uint64_t m_item;
		ASMContextNode* m_arrayNode;
		ASMContextNodeBlock* m_block;
		ASMContextNodeForEach(ASMContextNode* arrayNode, ASMContextNodeBlock* block, uint64_t key, uint64_t item) :
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
			out << "foreach";
			if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
				out << " ";
			}
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			if (m_key) {
				out << hashutils::ExtractTmp("var", m_key) << std::flush << ", ";
			}
			out << hashutils::ExtractTmp("var", m_item) << std::flush << " in ";
			m_arrayNode->Dump(out, ctx);
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}
			m_block->Dump(out, ctx);
		}

		void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
			m_block->ApplySubBlocks(func, ctx);
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_arrayNode) {
				func(m_arrayNode, ctx);
				m_arrayNode->ApplySubNodes(func, ctx);
			}
			if (m_block) {
				func(reinterpret_cast<ASMContextNode*&>(m_block), ctx);
				m_block->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeForDelta : public ASMContextNode {
	public:
		ASMContextNode* m_init;
		ASMContextNode* m_cond;
		ASMContextNode* m_delta;
		ASMContextNodeBlock* m_block;
		ASMContextNode* m_originJump;
		ASMContextNodeForDelta(ASMContextNode* init, ASMContextNode* cond, ASMContextNode* delta, ASMContextNodeBlock* block,
			ASMContextNode* originJump) :
			ASMContextNode(PRIORITY_INST, TYPE_FOR), m_init(init), m_cond(cond), m_delta(delta), m_block(block), m_originJump(originJump) {
			assert(init);
			// condition can be null for(;;) converted into for(i=0;;i++)
			assert(delta);
			assert(block);
			m_renderSemicolon = false;
			m_originJump->m_renderSemicolon = true;
		}
		~ASMContextNodeForDelta() {
			delete m_init;
			delete m_cond;
			delete m_delta;
			delete m_block;
			if (m_originJump) {
				delete m_originJump;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeForDelta(m_init->Clone(), m_cond ? m_cond->Clone() : nullptr, m_delta->Clone(), static_cast<ASMContextNodeBlock*>(m_block->Clone()), m_originJump ? m_originJump->Clone() : nullptr);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "for";
			if (ctx.opt.m_show_jump_delta) {
				out << "<";
				if (m_originJump) {
					m_originJump->Dump(out, ctx);
				}
				else {
					out << "NO_ORIGIN";
				}
				out << ">";
			} 
			else if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
				out << " ";
			}
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_init->Dump(out, ctx);
			out << "; ";
			if (m_cond) {
				m_cond->Dump(out, ctx);
			}
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << "; ";
			m_delta->Dump(out, ctx);
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}
			m_block->Dump(out, ctx);
		}

		void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
			m_block->ApplySubBlocks(func, ctx);
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_init) {
				func(m_init, ctx);
				m_init->ApplySubNodes(func, ctx);
			}
			if (m_cond) {
				func(m_cond, ctx);
				m_cond->ApplySubNodes(func, ctx);
			}
			if (m_delta) {
				func(m_delta, ctx);
				m_delta->ApplySubNodes(func, ctx);
			}
			if (m_originJump) {
				func(m_originJump, ctx);
				m_originJump->ApplySubNodes(func, ctx);
			}
			if (m_block) {
				func(reinterpret_cast<ASMContextNode*&>(m_block), ctx);
				m_block->ApplySubNodes(func, ctx);
			}
		}

	};

	class ASMContextNodeDoWhile : public ASMContextNode {
	public:
		ASMContextNode* m_condition;
		ASMContextNodeBlock* m_block;
		ASMContextNode* m_originJump;
		ASMContextNodeDoWhile(ASMContextNode* condition, ASMContextNodeBlock* block, ASMContextNode* originJump) :
			ASMContextNode(PRIORITY_INST, TYPE_DO_WHILE), m_condition(condition), m_block(block), m_originJump(originJump) {
			block->m_blockType = BLOCK_PADDING; // use custom block, no need to write the { }
			m_originJump->m_renderSemicolon = true;
		}
		~ASMContextNodeDoWhile() {
			delete m_condition;
			delete m_block;
			if (m_originJump) {
				delete m_originJump;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeDoWhile(m_condition->Clone(), static_cast<ASMContextNodeBlock*>(m_block->Clone()), m_originJump ? m_originJump->Clone() : nullptr);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "do";
			if (ctx.opt.m_show_jump_delta) {
				out << "<";
				if (m_originJump) {
					ctx.padding++;
					m_originJump->Dump(out, ctx);
					ctx.padding--;
				}
				else {
					out << "NO_ORIGIN";
				}
				out << ">";
			}

			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}
			out << "{";
			m_block->Dump(out, ctx);
			ctx.WritePadding(out, true) << "}";


			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
				ctx.WritePadding(out << "\n", true);
			}
			else {
				out << " ";
			}

			out << "while";
			if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
				out << " ";
			}
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			m_condition->Dump(out, ctx);
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";
		}

		void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
			m_block->ApplySubBlocks(func, ctx);
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_condition) {
				func(m_condition, ctx);
				m_condition->ApplySubNodes(func, ctx);
			}
			if (m_originJump) {
				func(m_originJump, ctx);
				m_originJump->ApplySubNodes(func, ctx);
			}
			if (m_block) {
				func(reinterpret_cast<ASMContextNode*&>(m_block), ctx);
				m_block->ApplySubNodes(func, ctx);
			}
		}
	};

	class ASMContextNodeWhile : public ASMContextNode {
	public:
		ASMContextNode* m_condition;
		ASMContextNodeBlock* m_block;
		ASMContextNode* m_originJump;
		ASMContextNodeWhile(ASMContextNode* condition, ASMContextNodeBlock* block, ASMContextNode* originJump) :
			ASMContextNode(PRIORITY_INST, TYPE_WHILE), m_condition(condition), m_block(block), m_originJump(originJump) {
			m_renderSemicolon = false;
			m_originJump->m_renderSemicolon = true;
		}
		~ASMContextNodeWhile() {
			if (m_condition) {
				delete m_condition;
			}
			if (m_block) {
				delete m_block;
			}
			if (m_originJump) {
				delete m_originJump;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeWhile(m_condition ? m_condition->Clone() : nullptr, static_cast<ASMContextNodeBlock*>(m_block->Clone()), m_originJump ? m_originJump->Clone() : nullptr);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			if (m_condition) {
				out << "while";
				if (ctx.opt.m_show_jump_delta) {
					out << "<";
					if (m_originJump) {
						ctx.padding++;
						m_originJump->Dump(out, ctx);
						ctx.padding--;
					}
					else {
						out << "NO_ORIGIN";
					}
					out << ">";
				}
				else if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
					out << " ";
				}
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				m_condition->Dump(out, ctx);
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
					ctx.WritePadding(out << "\n", true);
				}
				else {
					out << " ";
				}
			}
			else {
				// while without condition, equivalent as "for (;;)"
				out << "for";
				if (ctx.opt.m_show_jump_delta) {
					out << "<";
					if (m_originJump) {
						ctx.padding++;
						m_originJump->Dump(out, ctx);
						ctx.padding--;
					}
					else {
						out << "NO_ORIGIN";
					}
					out << ">";
				}
	 else if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
	  out << " ";
	}
	if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
		out << "( ;; )";
	}
	else {
		out << "(;;)";
	}

	if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
		ctx.WritePadding(out << "\n", true);
	}
	else {
		out << " ";
	}
	}
	m_block->Dump(out, ctx);
	}

	void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
		m_block->ApplySubBlocks(func, ctx);
	}

	void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
		if (m_condition) {
			func(m_condition, ctx);
			m_condition->ApplySubNodes(func, ctx);
		}
		if (m_originJump) {
			func(m_originJump, ctx);
			m_originJump->ApplySubNodes(func, ctx);
		}
		if (m_block) {
			func(reinterpret_cast<ASMContextNode*&>(m_block), ctx);
			m_block->ApplySubNodes(func, ctx);
		}
	}
	};

	class ASMContextNodeIfElse : public ASMContextNode {
	public:
		ASMContextNode* m_condition;
		ASMContextNodeBlock* m_ifblock;
		ASMContextNodeBlock* m_elseblock;
		ASMContextNodeIfElse(ASMContextNode* condition, ASMContextNodeBlock* ifblock, ASMContextNodeBlock* elseblock) :
			ASMContextNode(PRIORITY_INST, TYPE_IF), m_condition(condition), m_ifblock(ifblock), m_elseblock(elseblock) {
			assert(condition && "empty condition");
			assert(ifblock && "empty if block");
			m_renderSemicolon = false;
			// set padding
			if (ifblock) {
				ifblock->m_blockType = BLOCK_PADDING;
			}
			if (elseblock) {
				elseblock->m_blockType = BLOCK_PADDING;
			}
		}
		~ASMContextNodeIfElse() {
			if (m_condition) {
				delete m_condition;
			}
			if (m_ifblock) {
				delete m_ifblock;
			}
			if (m_elseblock) {
				delete m_elseblock;
			}
		}

		ASMContextNode* Clone() const override {
			return new ASMContextNodeIfElse(m_condition ? m_condition->Clone() : nullptr,
				m_ifblock ? static_cast<ASMContextNodeBlock*>(m_ifblock->Clone()) : nullptr,
				m_elseblock ? static_cast<ASMContextNodeBlock*>(m_elseblock->Clone()) : nullptr);
		}

		void Dump(std::ostream& out, DecompContext& ctx) const override {
			out << "if";
			if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
				out << " ";
			}
			out << "(";
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			if (m_condition) {
				m_condition->Dump(out, ctx);
			}
			else {
				out << "<error_no_condition>";
			}
			if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
				out << " ";
			}
			out << ")";

			bool lastInlineable = IsBlockInlineable(m_ifblock, ctx.opt);
			if (!lastInlineable) {
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
					ctx.WritePadding(out << "\n", true);
				}
				else {
					out << " ";
				}
				out << "{";
			}
			if (m_ifblock) {
				m_ifblock->Dump(out, ctx);
			}
			else {
				out << "<error_no_block>";
			}
			ASMContextNodeBlock* elseBlock = m_elseblock;

			// loop over all the nested if blocks to create a pretty itie*e output
			while (elseBlock && SizeNoEmptyNode(elseBlock->m_statements) == 1) {
				auto* ref = GetNoEmptyNode(elseBlock->m_statements, 0)->node;
				if (ref->m_type != TYPE_IF) {
					break;
				}
				auto* ifb = static_cast<ASMContextNodeIfElse*>(ref);
				if (!lastInlineable) {
					ctx.WritePadding(out, true) << "}";
					if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
						ctx.WritePadding(out << "\n", true);
					}
					else {
						out << " ";
					}
				}
				else {
					ctx.WritePadding(out, true);
				}

				out << "else if";
				if (!(ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_SPACE_AFTER_CONTROL)) {
					out << " ";
				}
				out << "(";
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				if (ifb->m_condition) {
					ifb->m_condition->Dump(out, ctx);
				}
				else {
					out << "<error_no_condition>";
				}
				if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_SPACE_BEFOREAFTER_PARAMS) {
					out << " ";
				}
				out << ")";
				lastInlineable = IsBlockInlineable(ifb->m_ifblock, ctx.opt);
				if (!lastInlineable) {
					if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
						ctx.WritePadding(out << "\n", true);
					}
					else {
						out << " ";
					}
					out << "{";
				}
				if (ifb->m_ifblock) {
					ifb->m_ifblock->Dump(out, ctx);
				}
				else {
					out << "<error_no_block>";
				}
				elseBlock = ifb->m_elseblock;
			}
			if (elseBlock) {
				if (!lastInlineable) {
					ctx.WritePadding(out, true) << "}";

					if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
						ctx.WritePadding(out << "\n", true);
					}
					else {
						out << " ";
					}
				}
				else {
					ctx.WritePadding(out, true);
				}

				out << "else";
				lastInlineable = IsBlockInlineable(elseBlock, ctx.opt);
				if (!lastInlineable) {
					if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINE_AFTER_BLOCK_START) {
						ctx.WritePadding(out << "\n", true);
					}
					else {
						out << " ";
					}
					out << "{";
				}
				elseBlock->Dump(out, ctx);
			}
			if (!lastInlineable) {
				ctx.WritePadding(out, true) << "}\n";
			}
		}

		void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) override {
			if (m_ifblock) {
				m_ifblock->ApplySubBlocks(func, ctx);
			}
			if (m_elseblock) {
				m_elseblock->ApplySubBlocks(func, ctx);
			}
		}

		void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override {
			if (m_condition) {
				func(m_condition, ctx);
				m_condition->ApplySubNodes(func, ctx);
			}

			if (m_ifblock) {
				func(reinterpret_cast<ASMContextNode*&>(m_ifblock), ctx);
				m_ifblock->ApplySubNodes(func, ctx);
			}
			if (m_elseblock) {
				func(reinterpret_cast<ASMContextNode*&>(m_elseblock), ctx);
				m_elseblock->ApplySubNodes(func, ctx);
			}
		}
	};

}