#include <includes.hpp>
#include <core/async.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm_nodes.hpp>

namespace tool::gsc::opcode {
	using namespace tool::gsc;

	size_t SizeNoEmptyNode(const std::vector<ASMContextStatement>& statements) {
		size_t acc = 0;
		for (const auto& stmt : statements) {
			if (stmt.node->m_type != TYPE_PRECODEPOS) {
				acc++;
			}
		}
		return acc;
	}

	ASMContextStatement* GetNoEmptyNode(std::vector<ASMContextStatement>& statements, size_t index) {
		for (auto& stmt : statements) {
			if (stmt.node->m_type != TYPE_PRECODEPOS) {
				if (index-- == 0) {
					return &stmt;
				}
			}
		}
		assert(0);
		return nullptr;
	}


	void PatchNumber(ASMContextNode*& node, double value) {
		if (!node || !node->IsConstNumber()) {
			LOG_WARNING("Usage of PatchNumber with node not a const number");
			return;
		}

		if (value - ((double)((int64_t)value)) > 0.00000000000001 && node->m_type != TYPE_FLOAT) {
			// we need to convert this node to a float
			delete node;
			node = new ASMContextNodeValue<FLOAT>((FLOAT)value, TYPE_FLOAT);
			return;
		}

		dynamic_cast<ASMContextNodeValueVir*>(node)->SetNumber(value);
	}

	void PatchNumber(ASMContextNode*& node, int64_t value) {
		if (!node || !node->IsConstNumber()) {
			LOG_WARNING("Usage of PatchNumber with node not a const number");
			return;
		}

		dynamic_cast<ASMContextNodeValueVir*>(node)->SetNumber(value);
	}

	void PatchNumber(ASMContextNode*& node, ASMContextNode* other) {
		if (!other || !other->IsConstNumber()) {
			LOG_WARNING("Usage of PatchNumber with other not a const number");
			return;
		}

		if (other->m_type == TYPE_FLOAT) {
			PatchNumber(node, dynamic_cast<ASMContextNodeValue<FLOAT>*>(other)->m_value);
		}
		else {
			PatchNumber(node, other->GetIntConst());
		}
	}

	float GetFloatNumber(ASMContextNode* node) {
		if (node->m_type == TYPE_FLOAT) {
			return dynamic_cast<ASMContextNodeValue<float>*>(node)->m_value;
		}
		else {
			return (float)node->GetIntConst();
		}
	}


	bool IsEqualNumber(ASMContextNode* a, ASMContextNode* b) {
		if (!a || !b || !a->IsConstNumber() || !b->IsConstNumber()) {
			return false;
		}
		ASMContextNodeValueVir* aa = dynamic_cast<ASMContextNodeValueVir*>(a);
		ASMContextNodeValueVir* bb = dynamic_cast<ASMContextNodeValueVir*>(b);
		ASMContextNodeValueVirDataType dt = aa->GetDatatype();

		if (dt != bb->GetDatatype()) {
			return false;
		}

		switch (dt) {
		case ASCNVD_FLOAT:
			return dynamic_cast<ASMContextNodeValue<FLOAT>*>(a)->m_value == dynamic_cast<ASMContextNodeValue<FLOAT>*>(b)->m_value;
		case ASCNVD_INT:
			return aa->GetIntConst() == bb->GetIntConst();
		case ASCNVD_STRING:
			return !strcmp(dynamic_cast<ASMContextNodeValue<const char*>*>(a)->m_value, dynamic_cast<ASMContextNodeValue<const char*>*>(b)->m_value);
		default:
			return false;
		}
	}
	bool IsStructSimilar(ASMContextNode* a, ASMContextNode* b) {
		if (a == b) {
			return true;
		}

		if (!a || !b || a->m_type != b->m_type) {
			return false;
		}

		switch (a->m_type) {
		case TYPE_SELF:
		case TYPE_GET_UNDEFINED:
			return true;
		case TYPE_GET_CODE_REF:
			return dynamic_cast<ASMContextNodeCodeRef*>(a)->m_location == dynamic_cast<ASMContextNodeCodeRef*>(b)->m_location;
		case TYPE_IDENTIFIER:
			return dynamic_cast<ASMContextNodeIdentifier*>(a)->m_value == dynamic_cast<ASMContextNodeIdentifier*>(b)->m_value;
		case TYPE_VECTOR: {
			ASMContextNodeVector* veca = dynamic_cast<ASMContextNodeVector*>(a);
			ASMContextNodeVector* vecb = dynamic_cast<ASMContextNodeVector*>(b);

			return IsStructSimilar(veca->m_x, vecb->m_x) && IsStructSimilar(veca->m_y, vecb->m_y) && IsStructSimilar(veca->m_z, vecb->m_z);
		}
		case TYPE_TERNARY: {
			ASMContextNodeTernary* tera = dynamic_cast<ASMContextNodeTernary*>(a);
			ASMContextNodeTernary* terb = dynamic_cast<ASMContextNodeTernary*>(b);
			return IsStructSimilar(tera->m_operand1, terb->m_operand1) && IsStructSimilar(tera->m_operand2, terb->m_operand2) && IsStructSimilar(tera->m_operand3, terb->m_operand3);
		}
		case TYPE_CONST_HASH:
			return dynamic_cast<ASMContextNodeHash*>(a)->m_value == dynamic_cast<ASMContextNodeHash*>(b)->m_value;
		case TYPE_STRING:
			return !strcmp(dynamic_cast<ASMContextNodeString*>(a)->m_value, dynamic_cast<ASMContextNodeString*>(b)->m_value);
		case TYPE_VALUE:
		case TYPE_FLOAT:
			return IsEqualNumber(a, b);
		case TYPE_OPERATION1:
		case TYPE_DELTA: {
			ASMContextNodeOp1* aa = dynamic_cast<ASMContextNodeOp1*>(a);
			ASMContextNodeOp1* bb = dynamic_cast<ASMContextNodeOp1*>(b);

			if (_strcmpi(aa->m_description, bb->m_description)) {
				return false;
			}
			return IsStructSimilar(aa->m_operand, bb->m_operand);
		}
		case TYPE_OPERATION2:
		case TYPE_OPERATION_MERGE:
		{
			ASMContextNodeOp2* aa = dynamic_cast<ASMContextNodeOp2*>(a);
			ASMContextNodeOp2* bb = dynamic_cast<ASMContextNodeOp2*>(b);

			if (_strcmpi(aa->m_description, bb->m_description)) {
				return false;
			}
			return IsStructSimilar(aa->m_operand1, bb->m_operand1)
				&& IsStructSimilar(aa->m_operand2, bb->m_operand2);
		}
		case TYPE_ARRAY_ACCESS: {
			ASMContextNodeArrayAccess* aa = dynamic_cast<ASMContextNodeArrayAccess*>(a);
			ASMContextNodeArrayAccess* bb = dynamic_cast<ASMContextNodeArrayAccess*>(b);

			return IsStructSimilar(aa->m_operandleft, bb->m_operandleft) && IsStructSimilar(aa->m_operandright, bb->m_operandright);
		}
		case TYPE_ACCESS:
		case TYPE_SET: {
			ASMContextNodeLeftRightOperator* aa = dynamic_cast<ASMContextNodeLeftRightOperator*>(a);
			ASMContextNodeLeftRightOperator* bb = dynamic_cast<ASMContextNodeLeftRightOperator*>(b);

			return IsStructSimilar(aa->m_left, bb->m_left) && IsStructSimilar(aa->m_right, bb->m_right);
		}
		case TYPE_SET_OP: {
			ASMContextNodeLeftRightOperator* aa = dynamic_cast<ASMContextNodeLeftRightOperator*>(a);
			ASMContextNodeLeftRightOperator* bb = dynamic_cast<ASMContextNodeLeftRightOperator*>(b);

			return aa->m_operatorName == bb->m_operatorName
				&& IsStructSimilar(aa->m_left, bb->m_left) && IsStructSimilar(aa->m_right, bb->m_right);
		}
		case TYPE_FUNC_REFNAME: {
			ASMContextNodeFuncRef* aa = dynamic_cast<ASMContextNodeFuncRef*>(a);
			ASMContextNodeFuncRef* bb = dynamic_cast<ASMContextNodeFuncRef*>(b);

			return aa->m_func == bb->m_func
				&& aa->m_script == bb->m_script
				&& aa->m_nsp == bb->m_nsp
				&& !_strcmpi(aa->m_op, bb->m_op);
		}
		case TYPE_FUNC_CALL: {
			ASMContextNodeCallFuncPtr* aa = dynamic_cast<ASMContextNodeCallFuncPtr*>(a);
			ASMContextNodeCallFuncPtr* bb = dynamic_cast<ASMContextNodeCallFuncPtr*>(b);
			if (aa->m_flags != bb->m_flags || aa->m_operands.size() != bb->m_operands.size()) {
				return false;
			}

			for (size_t i = 0; i < aa->m_operands.size(); i++) {
				if (!IsStructSimilar(aa->m_operands[i], bb->m_operands[i])) {
					return false;
				}
			}
			return true;
		}
		default:
			return false;
		}
	}


	bool IsJumpConditionForceReversed(const ASMContextNodeJumpOperator* op) {
		return op->m_type == TYPE_JUMP_ONFALSE || op->m_type == TYPE_JUMP_ONFALSEEXPR;
	}

	ASMContextNode* JumpCondition(ASMContextNodeJumpOperator* op, bool reversed) {
		switch (op->m_type) {
		case TYPE_JUMP_GREATERTHAN:
		case TYPE_JUMP_LOWERTHAN:
		{
			ASMContextNodeOp2* clo = static_cast<ASMContextNodeOp2*>(op->m_operand->Clone());
			if (reversed) {
				clo->m_description = op->m_type == TYPE_JUMP_GREATERTHAN ? "<" : ">";
			}
			return clo;
		}
		case TYPE_JUMP_ONDEFINED:
			if (reversed) {
				return new ASMContextNodeOp1("!", true, new ASMContextNodeFunctionOperator("isdefined", nullptr, op->m_operand->Clone(), TYPE_FUNC_IS_DEFINED, true), TYPE_UNDEFINED, true);
			}
			return new ASMContextNodeFunctionOperator("isdefined", nullptr, op->m_operand->Clone(), TYPE_FUNC_IS_DEFINED, true);
		case TYPE_JUMP_ONTRUE:
		case TYPE_JUMP_ONTRUEEXPR:
			if (reversed) {
				return new ASMContextNodeOp1("!", true, op->m_operand->Clone(), TYPE_UNDEFINED, true);
			}
			return op->m_operand->Clone();

		case TYPE_JUMP_ONFALSE:
		case TYPE_JUMP_ONFALSEEXPR:
			if (reversed) {
				return op->m_operand->Clone();
			}
			return new ASMContextNodeOp1("!", true, op->m_operand->Clone(), TYPE_UNDEFINED, true);
		default:
			return nullptr;
		}
	}



	ASMContextNode* ASMCNodeConvertToBool(ASMContextNode* node, ASMContext& ctx) {
		if (!node || !node->IsBoolConvertable(false, ctx)) {
			return node;
		}

		auto* bnode = node->ConvertToBool();

		if (!bnode) {
			LOG_WARNING("Bool convertable node can't be converted to bool: {}", *node);
			return node; // bad conversion??
		}

		if (bnode != node) {
			delete node;
		}
		return bnode;
	}

	bool IsBlockType(tool::gsc::opcode::ASMContextNodeType type) {
		using namespace tool::gsc::opcode;
		return type == TYPE_BLOCK || type == TYPE_DO_WHILE || type == TYPE_WHILE || type == TYPE_FOR
			|| type == TYPE_FOR_EACH || type == TYPE_IF || type == TYPE_SWITCH_POSTCOMPUTE || type == TYPE_SWITCH_PRECOMPUTE;
	}

	std::ostream& operator<<(std::ostream& os, const ASMContextNode& obj) {
		// fake decomp context
		DecompContext ctx{ 0, 0, {}, 0 };
		obj.Dump(os, ctx);
		return os;
	}

	ASMContextLocationOp::~ASMContextLocationOp() {
	}

	void ASMContextLocationOp::Run(ASMContext& context, T8GSCOBJContext& objctx) const {
		assert(0);
	}

	void ASMContextNode::ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx) {
		// do nothing, no sub blocks
	}
	void ASMContextNode::ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>&, SubNodeContext& ctx) {
		// do nothing, done on top
	}

	ASMContextNodeBlock::ASMContextNodeBlock(ASMContextNodeBlockType blockType, bool disabled, bool allowInline) : ASMContextNode(PRIORITY_INST, TYPE_BLOCK), m_blockType(blockType), m_disabled(disabled), m_allowInline(allowInline) {
		m_renderSemicolon = false;
	}

	ASMContextNodeBlock::~ASMContextNodeBlock() {
		for (auto& ref : m_statements) {
			if (ref.node) {
				delete ref.node;
			}
		}
	}

	ASMContextStatement* ASMContextNodeBlock::FetchFirstForLocation(int64_t rloc) {
		for (auto& loc : m_statements) {
			if (loc.location->rloc >= rloc) {
				return &loc;
			}
		}
		return nullptr;
	}

	ASMContextNode* ASMContextNodeBlock::Clone0() const {
		ASMContextNodeBlock* n = new ASMContextNodeBlock(m_blockType, m_disabled, m_allowInline);
		for (auto& node : m_statements) {
			n->m_statements.push_back({ node.node->Clone(), node.location });
		}
		return n;
	}

	void ASMContextNodeBlock::ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>& func, ASMContext& ctx) {
		// apply to this
		func(this, ctx);
	}
	void ASMContextNodeBlock::ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) {
		if (m_blockType == BLOCK_DEV) {
			ctx.devBlockDepth++;
		}
		for (auto& stmt : m_statements) {
			if (stmt.node) {
				ctx.stmtRloc = stmt.location;
				func(stmt.node, ctx);
				stmt.node->ApplySubNodes(func, ctx);
			}
		}
		if (m_blockType == BLOCK_DEV) {
			ctx.devBlockDepth--;
		}
	}

	void ASMContextNode::Dump(std::ostream& out, DecompContext& ctx) const {
		// nothing by default
	}

	ASMContextNode* ASMContextNode::Clone0() const {
		throw std::runtime_error("ASMContextNode::Clone0() not implemented for this node");
	}

	ASMContextNode::ASMContextNode(ASMContextNodePriority priority, ASMContextNodeType type) : m_priority(priority), m_type(type) {
	}

	ASMContextNode::~ASMContextNode() {
	}

	ASMContextNode* ASMContextNode::ConvertToBool() {
		if (IsIntConst() && GetIntConst()) {
			return new ASMContextNodeValue<const char*>("true", TYPE_VALUE);
		}
		return new ASMContextNodeValue<const char*>("false", TYPE_VALUE);
	}

	ASMContextNode* ASMContextNode::Clone() const {
		ASMContextNode* c{ Clone0() };
		c->m_rlocEstimated = m_rlocEstimated;
		c->m_renderSemicolon = m_renderSemicolon;
		c->m_renderRefIfAny = m_renderRefIfAny;
		return c;
	}

	bool ASMContextNode::IsBoolConvertable(bool strict, ASMContext& ctx) {
		if (!IsIntConst()) {
			return false;
		}
		int64_t v = GetIntConst();
		return v == 0 || v == 1;
	}

	int64_t ASMContextNode::GetIntConst() const {
		return 0;
	}

	bool ASMContextNode::IsIntConst() const {
		return false;
	}


	ASMContext::ASMContext(byte* fonctionStart, GSCOBJHandler& gscReader, T8GSCOBJContext& objctx, const GscInfoOption& opt, uint64_t nsp, GSCExportReader& exp, void* readerHandle, uint64_t vm, Platform platform)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_gscReader(gscReader), m_objctx(objctx), m_opt(opt), m_runDecompiler(opt.m_dcomp),
		m_lastOpCodeBase(-1), m_namespace(nsp), m_funcBlock(BLOCK_DEFAULT, false, false), m_exp(exp), m_readerHandle(readerHandle), m_vm(vm), m_platform(platform) {
		// set start as unhandled
		PushLocation();
		exp.SetHandle(m_readerHandle);
		funcRloc = exp.GetAddress();
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

	ASMContextLocation::ASMContextLocation() {}
	ASMContextLocation::~ASMContextLocation() {
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

	void ASMContextLocation::RemoveRef(int32_t origin) {
		refs.erase(origin);
	}

	ASMContextLocation& ASMContext::PushLocation(byte* location) {
		// push aligned location to avoid missing a location
		if (m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16)) {
			location = utils::Aligned<uint16_t>(location);
		}
		auto loc = FunctionRelativeLocation(location);
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

	bool ASMContext::FindNextLocation() {
		int64_t min = 0xFFFFFFFFFF;
		int32_t minloc = 0;
		for (const auto& [location, loc] : m_locs) {
			if (!loc.handled) {
				if (min > loc.rloc) {
					min = loc.rloc;
					minloc = location;
				}
			}
			else {
				if (loc.rloc > min) {
					LOG_WARNING("{}::{} : Unhandled rloc: 0x{:x} after handled 0x{:x}, the decompiled code won't be 100% correct",
						hashutils::ExtractTmp("namespace", m_exp.GetNamespace()),
						hashutils::ExtractTmp("function", m_exp.GetName()), min, loc.rloc);
					min = 0xFFFFFFFFFF;
					minloc = 0;
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

	int32_t ASMContext::FunctionRelativeLocation(byte* bytecodeLocation) {
		return (int32_t)(reinterpret_cast<uintptr_t>(bytecodeLocation) - reinterpret_cast<uintptr_t>(m_fonctionStart));
	}

	uint32_t ASMContext::ScriptAbsoluteLocation(byte* bytecodeLocation) {
		return funcRloc + (uint32_t)FunctionRelativeLocation(bytecodeLocation);
	}
	bool ASMContext::IsInsideScript(byte* bytecodeLocation) {
		return m_gscReader.Ptr() <= bytecodeLocation && m_gscReader.Ptr(m_gscReader.GetFileSize()) > bytecodeLocation;
	}
	void ASMContext::CheckInsideScript(byte* location) {
		if (!IsInsideScript(location)) {
			throw std::runtime_error(std::format("Outside script {}", (void*)m_bcl));
		}
	}
	bool ASMContext::SwitchEndian() const {
		return m_objctx.SwitchEndian();
	}

	std::ostream& ASMContext::WritePadding(std::ostream& out) {
		if (m_opt.m_func_floc) {
			out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << (m_bcl - m_gscReader.Ptr());
		}
		return out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << FunctionRelativeLocation() << ": "
			// no opcode write
			<< std::setfill(' ') << std::setw((sizeof(int16_t) << 1) + 25 + 2) << " ";
	}
	UINT ASMContext::FinalSize() const {
		INT max = 0;
		for (auto& [loc, ref] : m_locs) {
			if (max < ref.rloc) {
				max = ref.rloc;
			}
		}
		return (UINT)max + (this->m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16) ? 2 : 1); // +1 for the Return/End operator
	}

	int ASMContext::GetLocalVarIdByName(uint64_t name) const {
		for (size_t i = 0; i < m_localvars.size(); i++) {
			if (m_localvars[i].name == name) {
				return (int)i;
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
			m_objectId = new ASMContextNodeValue<const char*>("<error objectid>", TYPE_UNDEFINED);
		}
		return m_objectId->Clone();
	}

	ASMContextNode* ASMContext::GetFieldIdASMCNode() {
		if (!m_fieldId) {
			// create fake value for the decompiler
			m_fieldId = new ASMContextNodeValue<const char*>("<error fieldid>", TYPE_UNDEFINED);
		}
		return m_fieldId->Clone();
	}

	ASMContextNode* ASMContext::PopASMCNode() {
		if (!m_stack.size()) {
			// create fake value for the decompiler
			PushASMCNode(new ASMContextNodeValue<const char*>("<error pop>", TYPE_UNDEFINED));
		}
		ASMContextNode* val = m_stack[m_stack.size() - 1];
		m_stack.pop_back();
		return val;
	}

	ASMContextNode* ASMContext::PeekASMCNode() {
		if (!m_stack.size()) {
			// create fake value for the decompiler
			PushASMCNode(new ASMContextNodeValue<const char*>("<error pop>", TYPE_UNDEFINED));
		}
		return m_stack[m_stack.size() - 1];
	}

	void ASMContext::CompleteStatement(bool resetLoc) {
		if (m_stack.size() && m_lastOpCodeBase != -1) { // empty func tests
			//if (m_fieldId) {
			//	delete m_fieldId;
			//	m_fieldId = nullptr;
			//}
			//if (m_objectId) {
			//	delete m_objectId;
			//	m_objectId = nullptr;
			//}
			auto* node = PopASMCNode();
			auto* loc = &m_locs[m_lastOpCodeBase];

			m_funcBlock.m_statements.push_back({ node, loc });
			if (resetLoc) {
				m_lastOpCodeBase = -1;
			}
			//// clear stack
			//for (auto& n : m_stack) {
			//	delete n;
			//}
			//m_stack.clear();
		}
	}


	void ASMContext::DisableDecompiler(const std::string& reason) {
		m_disableDecompiler = true;
		m_disableDecompilerError = reason;
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
			if (param1->m_type != TYPE_JUMP_ONTRUE && param1->m_type != TYPE_JUMP_ONDEFINED) {
				break;
			}
			it++;

			if (it == m_funcBlock.m_statements.end() || it->node->m_type != TYPE_SET) {
				break;
			}

			if (param1->m_type == TYPE_JUMP_ONTRUE) {

				auto* jump = static_cast<ASMContextNodeJumpOperator*>(param1);
				auto* set = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
				auto end = jump->m_location;


				it++;
				ASMContextLocation* jumpEndLocation = nullptr;
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
				uint64_t name = static_cast<ASMContextNodeIdentifier*>(set->m_left)->m_value;

				if (static_cast<ASMContextNodeIdentifier*>(isDefinedFunc->m_operand)->m_value != name) {
					break; // not the same name value
				}

				uint64_t localVar = GetLocalVarIdByName(name);

				// the local variables are reversed and the first is an error check, so - 1 - params
				if (localVar == -1 || localVar < m_localvars.size() - 1 - m_exp.GetParamCount() || m_localvars[localVar].defaultValueNode) {
					break; // not a param var or already defined as default
				}

				// add default value node
				m_localvars[localVar].defaultValueNode = set->m_right->Clone();

				if (jumpEndLocation) {
					// remove one ref
					jumpEndLocation->RemoveRef(jump->m_opLoc);
				}

				headerSize += 2;
			}
			else if (param1->m_type == TYPE_JUMP_ONDEFINED) {

				auto* jump = static_cast<ASMContextNodeJumpOperator*>(param1);
				auto* set = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
				auto end = jump->m_location;


				it++;
				ASMContextLocation* jumpEndLocation = nullptr;
				if (it != m_funcBlock.m_statements.end()) {
					jumpEndLocation = it->location;
					if (jumpEndLocation->rloc < end) {
						break; // jump too far
					}
				}

				if (jump->m_operand->m_type != TYPE_IDENTIFIER || set->m_left->m_type != TYPE_IDENTIFIER || !set->m_right) {
					break; // not isdefined(param_name) or not param_name = ...
				}
				uint64_t name = static_cast<ASMContextNodeIdentifier*>(set->m_left)->m_value;

				if (static_cast<ASMContextNodeIdentifier*>(jump->m_operand)->m_value != name) {
					break; // not the same name value
				}

				uint64_t localVar = GetLocalVarIdByName(name);

				// the local variables are reversed and the first is an error check, so - 1 - params
				if (localVar == -1 || localVar < m_localvars.size() - 1 - m_exp.GetParamCount() || m_localvars[localVar].defaultValueNode) {
					break; // not a param var or already defined as default
				}

				// add default value node
				m_localvars[localVar].defaultValueNode = set->m_right->Clone();

				if (jumpEndLocation) {
					// remove one ref
					jumpEndLocation->RemoveRef(jump->m_opLoc);
				}

				headerSize += 2;
			}
			else {
				throw std::runtime_error("INVALID TYPE_JUMP DEFAULT PARAM");
			}
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



	std::ostream& DecompContext::WritePadding(std::ostream& out, bool forceNoRLoc) {
		utils::Padding(out, paddingPre);
		if (gdbData && baseloc && lineBuf) {
			// add these offset/line to the gdb output
			gdbData->lineInfos[lineBuf->line] = baseloc + rloc;
			if (opt.m_lineCount) {
				out << "/*" << std::dec << lineBuf->line << "/" << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << (baseloc + rloc) << "*/";
			}
		}
		if (opt.m_func_rloc || opt.m_func_floc) {
			out << "/*";
			if (forceNoRLoc) {
				if (opt.m_func_rloc) {
					for (size_t i = 0; i < sizeof(int32_t) << 1; i++) {
						out << "*";
					}
					if (opt.m_func_floc) {
						out << "*";
					}
				}
				if (opt.m_func_floc) {
					for (size_t i = 0; i < sizeof(int32_t) << 1; i++) {
						out << "*";
					}
				}
			}
			else {
				if (opt.m_func_floc) {
					out << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << baseloc;
					if (opt.m_func_floc) {
						out << ".";
					}
				}
				if (opt.m_func_rloc) {
					out << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << rloc;
				}
			}
			out << "*/";
		}
		utils::Padding(out, padding);
		return out;
	}

	void ASMContextNodeBlock::Dump(std::ostream& out, DecompContext& ctx) const {
		bool inlined{};
		if (!m_disabled) {
			switch (m_blockType) {
			case BLOCK_DEFAULT: {
				inlined = IsBlockInlineable(this, ctx.opt);
				if (inlined) {
					out << std::endl;
				}
				else {
					out << "{" << std::endl;
				}
				break;
			}
			case BLOCK_DEV:
				out << "/#" << std::endl;
				break;
			default:
				out << std::endl;
				break;
			}

			ctx.padding++;
		}

		// push rloc
		auto oldRLoc = ctx.rloc;

		// decompiler proto block loop
		bool hide = false;
		size_t i{};
		size_t visibleId{};
		enum {
			LT_NONE = 0,
			LT_BLOCK,
			LT_STMT
		} lastType{};
		while (i < m_statements.size()) {
			const auto& ref = m_statements[i];
			ctx.rloc = ref.location->rloc;
			if (ctx.opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NEWLINES_BETWEEN_BLOCKS)) {
				if (lastType && (((!hide && ref.node->m_type != TYPE_PRECODEPOS) || ctx.opt.m_show_internal_blocks))) {
					if (ref.node->m_type != TYPE_END || i != m_statements.size() - 1) {
						if (IsBlockType(ref.node->m_type)) {
							ctx.WritePadding(out) << std::endl;
							lastType = LT_BLOCK;
						}
						else if (lastType != LT_STMT) {
							ctx.WritePadding(out) << std::endl;
							lastType = LT_STMT;
						}
					}
				}
				else {
					if (ref.node->m_type == TYPE_PRECODEPOS) {
						if (ctx.opt.m_show_internal_blocks) {
							lastType = LT_STMT;
						}
					}
					else {
						lastType = IsBlockType(ref.node->m_type) ? LT_BLOCK : LT_STMT;
					}
				}
			}
			if (ref.location->refs.size() && ref.node->m_renderRefIfAny) {
				// write the label one layer bellow the current block
				ctx.padding--;
				ctx.WritePadding(out, true) << "LOC_" << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << ref.location->rloc;
				if (ctx.opt.m_show_ref_count) {
					out << std::dec << " (stack:";

					auto it = ref.location->refs.begin();
					out << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << *it;
					it++;
					while (it != ref.location->refs.end()) {
						out << ", " << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << *(it);
						it++;
					}

					out << ")";
				}
				out << ":" << std::endl;
				ctx.padding++;
			}
			if (ref.node->m_type != TYPE_END) {
				if ((!hide && ref.node->m_type != TYPE_PRECODEPOS) || ctx.opt.m_show_internal_blocks) {
					if (!m_disabled || visibleId) {
						ctx.WritePadding(out);
					}
					if (hide && ref.node->m_type != TYPE_PRECODEPOS) {
						out << "<END DETECTED> ";
					}
					if (ref.node->m_type == TYPE_COMMENT) {
						out << "// ";
					}
					ref.node->Dump(out, ctx);

					if (ref.node->m_renderSemicolon) {
						if (ref.node->m_type != TYPE_COMMENT) {
							out << ";";
						}
						out << std::endl;
					}
					if (ref.node->m_type == TYPE_JUMP || ref.node->m_type == TYPE_RETURN) {
						hide = true;
					}
				}
			}
			else {
				hide = true;
				if (i != m_statements.size() - 1) {
					ctx.WritePadding(out);
					// if we're not at the end, it means we are reading a return;
					out << "return;" << std::endl;
				}
				else if (ctx.opt.m_show_internal_blocks) {
					ctx.WritePadding(out) << "<end>;" << std::endl;
				}
				else if (m_statements.size() == 1) {
					// empty end block, write a small empty line
					ctx.WritePadding(out) << std::endl;
				}
			}
			out << std::flush;
			i++;
			if (ref.node->m_type != TYPE_PRECODEPOS) {
				visibleId++;
			}
			if (hide && !ctx.opt.m_show_internal_blocks) {
				// don't write hidden stuff
				break;
			}
			/*
			if (i < m_statements.size() && hide && !ctx.opt.m_show_internal_blocks) {
				// search the next referenced element in the block (if it exists)
				do {
					if (m_statements[i].location->refs.size()) {
						// this element is referenced, it might be from the front, a later add would be to check
						// if by looking back.
						hide = false;
						break;
					}
				} while (i < m_statements.size());
			}
			*/
		}
		// pop back rloc
		ctx.rloc = oldRLoc;

		if (!m_disabled) {
			ctx.padding--;

			switch (m_blockType) {
			case BLOCK_DEFAULT:
				if (!inlined) {
					ctx.WritePadding(out, true) << "}" << std::endl;
				}
				break;
			case BLOCK_DEV:
				ctx.WritePadding(out, true) << "#/" << std::endl;
				break;
			}
		}
		else {
			if (SizeNoEmptyNode(m_statements) == 0) {
				out << std::endl; // disabled and empty, we add an empty line
			}
		}
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
			int64_t end = jump->m_location;
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
				devBlock->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_enddev>", TYPE_PRECODEPOS), it->location });
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

	static void ReplaceSwitchBlockBreak(ASMContextNodeBlock& current, ASMContext& ctx, ASMContextLocation* endLocation, uint64_t endCase, int64_t end) {
		for (ASMContextStatement& stmt : current.m_statements) {
			if (stmt.location->rloc >= endCase) {
				break; // end of the case
			}

			if (stmt.node->m_type == TYPE_BLOCK) {
				ReplaceSwitchBlockBreak(*static_cast<ASMContextNodeBlock*>(stmt.node), ctx, endLocation, endCase, end);
			}
			else if (stmt.node->m_type == TYPE_JUMP || stmt.node->m_type == TYPE_JUMP_ENDSWITCH) {
				auto* jump = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
				// convert this to a break statement
				if (jump->m_location == end) {
					jump->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<switch_inblock>" : "break";
					jump->m_showJump = false;
					jump->m_special_op = SPECIAL_JUMP_BREAK;
					if (endLocation) {
						endLocation->RemoveRef(jump->m_opLoc);
					}
				}
			}
		}

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
			int64_t end = switchPreCompute->m_endLocation;
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
				LOG_ERROR("Error when parsing switch, a case is after the end");
				assert(0);
				continue;
			}


			if (it == m_statements.end()) {
				LOG_ERROR("Error when parsing switch, end before start of the cases");
				assert(0);
				return -1;
			}

			auto* endLoc = FetchFirstForLocation(end);

			ASMContextLocation* endLocation = nullptr;
			if (endLoc && endLoc->location->rloc == end) {
				endLocation = endLoc->location;
			}

			b.node = switchBlock;

			if (cases.size()) {
				// we have at least one case
				size_t cid{};

				do {
					ASMContextNodeBlockType blockType = (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_SWITCH_FORCE_BLOCKS)) ? BLOCK_DEFAULT : BLOCK_PADDING;
					ASMContextNodeBlock* block = new ASMContextNodeBlock(blockType);
					switchBlock->m_cases.push_back({ cases[cid].casenode ? cases[cid].casenode->Clone() : nullptr, block });
					// we pass this case to fetch the end
					cid++;

					int64_t endCase;
					ASMContextNode* endNode{};

					if (cid == cases.size()) {
						endCase = end;
					}
					else {
						endCase = cases[cid].jumpLocation;
					}

					while (it != m_statements.end()) {
						if (it->location->rloc >= endCase) {
							break; // end of the case
						}

						if (it->node->m_type == TYPE_BLOCK) {
							ReplaceSwitchBlockBreak(*static_cast<ASMContextNodeBlock*>(it->node), ctx, endLocation, endCase, end);
						}
						else if (it->node->m_type == TYPE_JUMP || it->node->m_type == TYPE_JUMP_ENDSWITCH) {
							auto* jump = static_cast<ASMContextNodeJumpOperator*>(it->node);
							// convert this to a break statement
							if (jump->m_location == end) {
								jump->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<switch>" : "break";
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
					if (it != m_statements.end() && cid != cases.size()) {
						// add next location at the end of the case, for ending blocks without break; between cases
						// not for the last one because it has an endswitch at the end
						block->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_switchnext>", TYPE_PRECODEPOS), it->location });
					}
				} while (cid < cases.size());
			}
			else if (it->node->m_type == TYPE_JUMP_ENDSWITCH) {
				auto* jump = static_cast<ASMContextNodeJumpOperator*>(it->node);
				// convert this to a break statement
				if (jump->m_location == end) {
					jump->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<switch_end>" : "break";
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

	void ApplySubStatement(ASMContextStatement& stmt, ASMContext& ctx, std::function<void(ASMContextStatement& stmt)> func, bool endOnReturn = false) {
		if (stmt.node->m_type != TYPE_BLOCK) {
			if (endOnReturn && stmt.node && (stmt.node->m_type == TYPE_RETURN || stmt.node->m_type == TYPE_END)) {
				func(stmt);
				return;
			}
			stmt.node->ApplySubBlocks([endOnReturn, &func](ASMContextNodeBlock* block, ASMContext& ctx) {
				for (auto& stmt : block->m_statements) {
					ApplySubStatement(stmt, ctx, func, endOnReturn);
				}
				}, ctx);
			func(stmt);
			return;
		}

		dynamic_cast<ASMContextNodeBlock*>(stmt.node)->ApplySubBlocks([&func, endOnReturn](ASMContextNodeBlock* block, ASMContext& ctx) {
			for (auto& stmt : block->m_statements) {
				ApplySubStatement(stmt, ctx, func, endOnReturn);
				if (endOnReturn && stmt.node && (stmt.node->m_type == TYPE_RETURN || stmt.node->m_type == TYPE_END)) {
					break;
				}
			}
			}, ctx);
	}


	namespace {
		constexpr uint64_t getfirstarraykeyIWHash = hash::HashJupScr("getfirstarraykey");
		constexpr uint64_t getnextarraykeyIWHash = hash::HashJupScr("getnextarraykey");
		constexpr uint64_t getfirstarraykeyCerHash = hash::HashT10Scr("getfirstarraykey");
		constexpr uint64_t getnextarraykeyCerHash = hash::HashT10Scr("getnextarraykey");
		constexpr uint64_t getfirstarraykeyCerBHash = hash::HashT10ScrSP("getfirstarraykey");
		constexpr uint64_t getnextarraykeyCerBHash = hash::HashT10ScrSP("getnextarraykey");
		constexpr uint64_t getarraykeysCerHash = hash::HashT10Scr("getarraykeys");
	}


	int ASMContextNodeBlock::ComputeForEachBlocksKeys(ASMContext& ctx) {
		/*
		T10 VM10
		arrayRef = array;
		if (isdefined(arrayRef)) {
			__ka2 = getarraykeys(arrayRef);
			for (__ki1 = 0; __ki1 < __ka2.size; __ki1++) {
				keyname = __ka2[__ki1];
				valuename = arrayRef[keyname];

				// loop code
			}
			// cleanup code
			arrayRef = undefined;
			__ka2 = undefined;
			keyname = undefined;
			valuename = undefined;
			__ki1 = undefined;
		}

		*/

		size_t index{};

		while (index < m_statements.size()) {
			ASMContextStatement& arrayRefLoc{ m_statements[index] };
			arrayRefLoc.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
				block->ComputeForEachBlocks(ctx);
				}, ctx);
			const size_t startIndex{ index };
			int moveDelta{ 1 };


			ASMContextNodeLeftRightOperator* setOp;
			// arrayRef = array
			if (arrayRefLoc.node->m_type != TYPE_SET
				|| (setOp = static_cast<ASMContextNodeLeftRightOperator*>(arrayRefLoc.node))->m_left->m_type != TYPE_IDENTIFIER) {
				index += moveDelta;
				continue;
			}

			uint64_t names[0x10]{};
			uint64_t* namesEnd{ names };

			uint64_t arrayRefName{ static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value };
			*(namesEnd++) = arrayRefName;

			index++;
			moveDelta--;

			ASMContextStatement& arrayDefinedTest{ m_statements[index] };
			ASMContextNodeJumpOperator* arrayDefinedTestJump;
			ASMContextNodeMultOp* arrayDefinedTestJumpIsDefined;

			// LOC_00000141:jumpiffalse(isdefined(arrayRef)) LOC_00000187 delta: 0x43 (type:b/spec:false);

			if (arrayDefinedTest.node->m_type != TYPE_JUMP_ONFALSE
				|| (arrayDefinedTestJump = static_cast<ASMContextNodeJumpOperator*>(arrayDefinedTest.node))->m_operand->m_type != TYPE_FUNC_IS_DEFINED_OP
				|| arrayDefinedTestJump->m_delta < 0
				|| (arrayDefinedTestJumpIsDefined = static_cast<ASMContextNodeMultOp*>(arrayDefinedTestJump->m_operand))->m_operands.size() != 1
				|| arrayDefinedTestJumpIsDefined->m_operands[0]->m_type != TYPE_IDENTIFIER
				|| static_cast<ASMContextNodeIdentifier*>(arrayDefinedTestJumpIsDefined->m_operands[0])->m_value != arrayRefName
				) {
				index += moveDelta;
				continue;
			}

			index++;
			moveDelta--;
			ASMContextStatement& getArrayKeys{ m_statements[index] };
			ASMContextNodeLeftRightOperator* setKeysOp;
			ASMContextNodeCallFuncPtr* funcCall;

			// __ka2 = getarraykeys(__a3);
			if (getArrayKeys.node->m_type != TYPE_SET
				|| (setKeysOp = static_cast<ASMContextNodeLeftRightOperator*>(getArrayKeys.node))->m_left->m_type != TYPE_IDENTIFIER
				|| setKeysOp->m_right->m_type != TYPE_FUNC_CALL
				|| (funcCall = static_cast<ASMContextNodeCallFuncPtr*>(setKeysOp->m_right))->m_flags // no flags = normal call
				|| funcCall->m_operands.size() != 2 // name + param
				|| funcCall->m_operands[0]->m_type != TYPE_FUNC_REFNAME
				|| funcCall->m_operands[1]->m_type != TYPE_IDENTIFIER
				|| static_cast<ASMContextNodeFuncRef*>(funcCall->m_operands[0])->m_func != getarraykeysCerHash
				|| static_cast<ASMContextNodeIdentifier*>(funcCall->m_operands[1])->m_value != arrayRefName
				) {
				index += moveDelta;
				continue;
			}
			uint64_t keysRefName{ static_cast<ASMContextNodeIdentifier*>(setKeysOp->m_left)->m_value };
			*(namesEnd++) = keysRefName;

			index++;
			moveDelta--;

			// __ki1 = 0;
			ASMContextStatement& incInit{ m_statements[index] };
			ASMContextNodeLeftRightOperator* setIncOp;
			if (incInit.node->m_type != TYPE_SET
				|| (setIncOp = static_cast<ASMContextNodeLeftRightOperator*>(incInit.node))->m_left->m_type != TYPE_IDENTIFIER
				|| !setIncOp->m_right->IsConstNumber()
				|| setIncOp->m_right->GetIntConst()
				) {
				index += moveDelta;
				continue;
			}
			uint64_t incRefName{ static_cast<ASMContextNodeIdentifier*>(setIncOp->m_left)->m_value };
			*(namesEnd++) = incRefName;

			index++;
			moveDelta--;

			// jumpiffalse(__ki1 < __ka2.size) LOC_00000178;
			ASMContextStatement& loopJumpStmt{ m_statements[index] };
			ASMContextNodeJumpOperator* loopJump;
			if (loopJumpStmt.node->m_type != TYPE_JUMP_ONFALSE
				|| !(loopJump = static_cast<ASMContextNodeJumpOperator*>(loopJumpStmt.node))->m_operand
				|| loopJump->m_operand->m_type != TYPE_OPERATION2
				|| strcmp(static_cast<ASMContextNodeOp2*>(loopJump->m_operand)->m_description, "<")
				// assume correct for the rest
				) {
				index += moveDelta;
				continue;
			}



			index++;
			moveDelta--;
			// __k0 = __ka2[__ki1];
			ASMContextStatement& getKeyStmt{ m_statements[index] };
			ASMContextNodeLeftRightOperator* getKey;
			ASMContextNodeArrayAccess* keyAccess;
			if (getKeyStmt.node->m_type != TYPE_SET
				|| (getKey = static_cast<ASMContextNodeLeftRightOperator*>(getKeyStmt.node))->m_right->m_type != TYPE_ARRAY_ACCESS
				|| getKey->m_left->m_type != TYPE_IDENTIFIER
				|| (keyAccess = static_cast<ASMContextNodeArrayAccess*>(getKey->m_right))->m_operandleft->m_type != TYPE_IDENTIFIER
				|| static_cast<ASMContextNodeIdentifier*>(keyAccess->m_operandleft)->m_value != keysRefName
				|| static_cast<ASMContextNodeIdentifier*>(keyAccess->m_operandright)->m_value != incRefName
				// assume correct for the rest
				) {
				index += moveDelta;
				continue;
			}
			uint64_t keyRefName{ static_cast<ASMContextNodeIdentifier*>(getKey->m_left)->m_value };
			*(namesEnd++) = keyRefName;

			index++;
			moveDelta--;
			// var_9503b421fb8ed25d = __a3[__k0];
			ASMContextStatement& getValueStmt{ m_statements[index] };
			ASMContextNodeLeftRightOperator* getValue;
			ASMContextNodeArrayAccess* valueAccess;
			if (getValueStmt.node->m_type != TYPE_SET
				|| (getValue = static_cast<ASMContextNodeLeftRightOperator*>(getValueStmt.node))->m_right->m_type != TYPE_ARRAY_ACCESS
				|| getValue->m_left->m_type != TYPE_IDENTIFIER
				|| (valueAccess = static_cast<ASMContextNodeArrayAccess*>(getValue->m_right))->m_operandleft->m_type != TYPE_IDENTIFIER
				|| static_cast<ASMContextNodeIdentifier*>(valueAccess->m_operandleft)->m_value != arrayRefName
				|| static_cast<ASMContextNodeIdentifier*>(valueAccess->m_operandright)->m_value != keyRefName
				// assume correct for the rest
				) {
				index += moveDelta;
				continue;
			}
			uint64_t valueRefName{ static_cast<ASMContextNodeIdentifier*>(getValue->m_left)->m_value };
			*(namesEnd++) = valueRefName;

			index++;
			moveDelta--;

			// jumpiffalse(isdefined(var_9503b421fb8ed25d)) LOC_00000172;
			ASMContextStatement& checkValueStmt{ m_statements[index] };
			ASMContextNodeJumpOperator* checkValueJump;
			if ((checkValueStmt.node->m_type != TYPE_JUMP_ONFALSE && checkValueStmt.node->m_type != TYPE_IF)) { // assume ok, if for empty block
				index += moveDelta;
				continue;
			}
			checkValueJump = static_cast<ASMContextNodeJumpOperator*>(checkValueStmt.node);

			index++;
			moveDelta--;

			// load loop data
			int64_t endLoc{ checkValueJump->m_location }; // jump location for the break
			size_t startNodeIndex{ index };

			while (index < m_statements.size() && m_statements[index].location->rloc < endLoc) {
				index++;
				moveDelta--;
			}

			if (index >= m_statements.size()) {
				LOG_ERROR("Bad foreach node endNodeIndex");
				index += moveDelta;
				continue;
			}
			size_t endNodeIndex{ index };


			// __ki1++;
			size_t incIndex{ index };
			ASMContextStatement& incPPStmt{ m_statements[index] };
			if (incPPStmt.node->m_type != TYPE_DELTA) { // assume ok
				index += moveDelta;
				continue;
			}
			index++;
			moveDelta--;

			// goto LOC_0000014f;
			ASMContextStatement& jumpBackStmt{ m_statements[index] };
			if (jumpBackStmt.node->m_type != TYPE_JUMP) { // assume ok
				index += moveDelta;
				continue;
			}
			index++;
			moveDelta--;
			ASMContextStatement& cleanupFirst{ m_statements[index] };

			// match cleanup data
			while (true) {
				ASMContextStatement& maybeSet{ m_statements[index] };
				ASMContextNodeLeftRightOperator* setOpRem;
				if (maybeSet.node->m_type != TYPE_SET
					|| (setOpRem = static_cast<ASMContextNodeLeftRightOperator*>(maybeSet.node))->m_left->m_type != TYPE_IDENTIFIER
					|| setOpRem->m_right->m_type != TYPE_GET_UNDEFINED
					) {
					break; // not a cleanup
				}

				uint64_t testName{ static_cast<ASMContextNodeIdentifier*>(setOpRem->m_left)->m_value };


				if (std::find(names, namesEnd, testName) == namesEnd) {
					break; // not one of the used val
				}

				// we can remove it
				index++;
				moveDelta--;
			}

			bool keyUsed{ ctx.m_localvars_ref[keyRefName] > 3 }; // creation + value access + cleanup

			// remove array jump test
			m_statements[index].location->RemoveRef(arrayDefinedTestJump->m_opLoc);
			m_statements[incIndex].location->RemoveRef(checkValueJump->m_opLoc);


			// assume set good

			int64_t incrementLoc{ incPPStmt.location->rloc };
			ASMContextLocation* endNodeLocation{ cleanupFirst.location };
			int64_t breakLoc{ cleanupFirst.location->rloc };

			// we found the foreach, we can now patch the jumps (TODO: find if we need to specify the jump location, i.e.: nested foreach)
			for (size_t i = startIndex; i < index; i++) {
				ASMContextStatement& stmt = m_statements[i];

				ApplySubStatement(stmt, ctx, [&ctx, &breakLoc, &endNodeLocation, &incrementLoc, &incPPStmt](ASMContextStatement& stmt) {
					if (!IsJumpType(stmt.node->m_type)) {
						return;
					}

					ASMContextNodeJumpOperator* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
					if (j->m_location == breakLoc) {
						j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<foreach>" : "break";
						j->m_special_op = SPECIAL_JUMP_BREAK;
						j->m_showJump = false;
						if (endNodeLocation) {
							endNodeLocation->RemoveRef(j->m_opLoc);
						}
					}
					else if (j->m_location == incrementLoc && j->m_special_op != SPECIAL_JUMP_BREAK) {
						j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<foreach>" : "continue";
						j->m_showJump = false;
						j->m_special_op = SPECIAL_JUMP_CONTINUE;
						incPPStmt.location->RemoveRef(j->m_opLoc);
					}
					});
			}

			ASMContextNodeBlock* block = new ASMContextNodeBlock();
			ASMContextNodeForEach* forEachNode = new ASMContextNodeForEach(setOp->m_right->Clone(), block, keyUsed ? keyRefName : 0, valueRefName);

			auto it = m_statements.begin() + startIndex;
			delete it->node;
			it->node = forEachNode;

			it++;
			for (size_t i = startIndex + 1; i < startNodeIndex; i++) {
				// remove component statement
				it = m_statements.erase(it);
			}
			for (size_t i = startNodeIndex; i < endNodeIndex; i++) {
				block->m_statements.push_back({ it->node, it->location });
				it = m_statements.erase(it);
			}
			block->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_foreachcontinue>", TYPE_PRECODEPOS), it->location });
			for (size_t i = endNodeIndex; i < index; i++) {
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
	int ASMContextNodeBlock::ComputeForEachBlocksIterator(ASMContext& ctx) {
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


		// MWIII

		same as T8, but using functions instead of operator and undefineds at the end:

		var_26568fed2b866c11 = level.teamdata[victim.team]["players"];
		var_c00a2b6249962b50 = var_26568fed2b866c11;
		var_2ef11e61df37c715 = getfirstarraykey(var_c00a2b6249962b50);
		while (isdefined(var_2ef11e61df37c715)) {
			player = var_c00a2b6249962b50[var_2ef11e61df37c715];
			if (player namespace_82dcd1d5ae30ff7::_hasperk("specialty_snapshot_immunity")) {
				interrogator namespace_e072c8407b2a861c::updatedamagefeedback("hittacresist");
				return;
			} else {
				player function_ab6a75b53de148ab(interrogator, level.var_762fc98bdfe1cb1d);
				triggerportableradarpingteam(player.origin, interrogator.team, level.var_575cb25b5d41b53, 500);
			}
			var_2ef11e61df37c715 = getnextarraykey(var_c00a2b6249962b50, var_2ef11e61df37c715);
		}
		var_2ef11e61df37c715 = undefined;
		var_c00a2b6249962b50 = undefined;


		var_26568fed2b866c11 = init array?
		var_c00a2b6249962b50 = array var
		var_2ef11e61df37c715 = key
		var_2ef11e61df37c715 = val
		getfirstarraykey/getnextarraykey = function calls
		getfirstarraykey = function_4ccfee6f73819653 (79d6530b0bb9b5d1/10000000233)
		getnextarraykey = function_6db433ae7304a362 (79d6530b0bb9b5d1/10000000233)

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
			ASMContextStatement& arrayRefLoc = m_statements[index];
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

			ASMContextNodeLeftRightOperator* setOp = static_cast<ASMContextNodeLeftRightOperator*>(arrayRefLoc.node);

			if (setOp->m_left->m_type != TYPE_IDENTIFIER) {
				index += moveDelta;
				continue;
			}


			// array ref name
			uint64_t arrayRefName = static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value;

			size_t arrayRefIndex = index;

			index++;
			moveDelta--;

			if (index >= m_statements.size()) {
				index += moveDelta;
				continue;
			}
			ASMContextStatement& firstArrayLoc = m_statements[index];
			if (firstArrayLoc.node->m_type != TYPE_SET) {
				index += moveDelta;
				continue;
			}
			ASMContextNodeLeftRightOperator* setFirstArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(firstArrayLoc.node);


			if (([setFirstArrayOp]() {
				if (setFirstArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
					return true; // not a idf = ??
				}
				if (setFirstArrayOp->m_right->m_type == TYPE_ARRAY_FIRSTKEY) {
					return false; // bo4
				}
				if (setFirstArrayOp->m_right->m_type != TYPE_FUNC_CALL) {
					return true; // not mw
				}
				ASMContextNodeCallFuncPtr* callFunc = dynamic_cast<ASMContextNodeCallFuncPtr*>(setFirstArrayOp->m_right);

				if (callFunc->m_flags || !callFunc->m_operands.size() || callFunc->m_operands[0]->m_type != TYPE_FUNC_REFNAME) {
					return true;
				}

				ASMContextNodeFuncRef* funcRef = dynamic_cast<ASMContextNodeFuncRef*>(callFunc->m_operands[0]);

				return funcRef->m_func != getfirstarraykeyIWHash && funcRef->m_func != getfirstarraykeyCerHash && funcRef->m_func != getfirstarraykeyCerBHash;
				})()) {
				index += moveDelta;
				continue; // not key = firstarray(...)
			}

			// For T9 it is the iterator name
			uint64_t keyValName = static_cast<ASMContextNodeIdentifier*>(setFirstArrayOp->m_left)->m_value;

			/*
			LOC_000000fc:
				LOC_00000100:jumpiffalse(isdefined(key)) LOC_000002f0;
				doorblocker = var_f15d3f7b[key];
				nextarray(var_f15d3f7b, var_7cbeb6de)
			*/

			index++;
			moveDelta--;

			size_t forincsize = ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW) ? 2 : ctx.m_vm <= VMI_T8_36 ? 3 : 4;

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

			ASMContextNodeJumpOperator* jumpOp = static_cast<ASMContextNodeJumpOperator*>(condJump.node);

			ASMContextStatement& itemSetStmt = m_statements[index];

			if (itemSetStmt.node->m_type != TYPE_SET) {
				index += moveDelta;
				continue;
			}
			index++;
			moveDelta--;

			// keyValName
			uint64_t itemValName;
			if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW)) {
				/*
					agent = var_57acddc40b2f741[var_54ed0dc40829774];;

					// ... code

					// unlike in treyarch, the nextarray is at the end and not at the start of the iteration
					var_54ed0dc40829774 = getnextarraykey(var_57acddc40b2f741, var_54ed0dc40829774)
					continue
				*/
				ASMContextNodeLeftRightOperator* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);

				if (setKeyArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
					index += moveDelta;
					continue; // agent = var_57acddc40b2f741[var_54ed0dc40829774];
				}

				itemValName = static_cast<ASMContextNodeIdentifier*>(setKeyArrayOp->m_left)->m_value;
			}
			else if (ctx.m_vm > VMI_T8_36) {
				/*
					var_d9f19f82 = iteratorkey(var_e4aec0cf);
					var_8487602c = iteratorval(var_e4aec0cf);
					var_60ff8ae8 = iteratornext(var_e4aec0cf);
				*/
				ASMContextStatement& iteratorvalStmt = m_statements[index];

				if (iteratorvalStmt.node->m_type != TYPE_SET) {
					index += moveDelta;
					continue;
				}
				index++;
				moveDelta--;

				ASMContextStatement& iteratornextStmt = m_statements[index];

				if (iteratornextStmt.node->m_type != TYPE_SET) {
					index += moveDelta;
					continue;
				}

				ASMContextNodeLeftRightOperator* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);
				ASMContextNodeLeftRightOperator* setValArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(iteratorvalStmt.node);
				ASMContextNodeLeftRightOperator* setNextArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(iteratornextStmt.node);

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
				ASMContextStatement& nextArrayStmt = m_statements[index];

				if (nextArrayStmt.node->m_type != TYPE_ARRAY_NEXTKEY
					&& !(nextArrayStmt.node->m_type == TYPE_SET
						&& static_cast<ASMContextNodeLeftRightOperator*>(nextArrayStmt.node)->m_right->m_type == TYPE_ARRAY_NEXTKEYPUSH)
					) {
					index += moveDelta;
					continue;
				}
				ASMContextNodeLeftRightOperator* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);

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
			int64_t endLoc = jumpOp->m_location; // jump location for the break
			int64_t condLoc = condJump.location->rloc; // jump back location for the end of the loop
			// index of the last node (at least + 2 for the next value set and the jump)
			size_t endNodeIndex = index;


			while (endNodeIndex < m_statements.size() && m_statements[endNodeIndex].location->rloc < endLoc) {
				endNodeIndex++;
			}

			if (endNodeIndex > m_statements.size()) {
				LOG_ERROR("Bad foreach node endNodeIndex");
				assert(0);
				index += moveDelta;
				continue;
			}

			auto* endNodeLocation = endNodeIndex == m_statements.size() ? nullptr : m_statements[endNodeIndex].location;
			auto incrementIndex = endNodeIndex - 2;

			ASMContextStatement& setNextKeyStmt = m_statements[incrementIndex];
			ASMContextStatement& jumpBackStmt = m_statements[incrementIndex + 1];

			// at the end ot the loop we can continue, break, return or return <...>
			if (jumpBackStmt.node->m_type != TYPE_END && jumpBackStmt.node->m_type != TYPE_RETURN && (jumpBackStmt.node->m_type != TYPE_JUMP
				|| (static_cast<ASMContextNodeJumpOperator*>(jumpBackStmt.node)->m_location != condLoc
					&& static_cast<ASMContextNodeJumpOperator*>(jumpBackStmt.node)->m_location != endLoc))) {
				// not a good increment (wtf?)
				index += moveDelta;
				continue;
			}
			int64_t incrementLoc;
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
				ASMContextStatement& stmt = m_statements[i];

				ApplySubStatement(stmt, ctx, [&ctx, &endLoc, &endNodeLocation, &incrementLoc, &setNextKeyStmt](ASMContextStatement& stmt) {
					if (!IsJumpType(stmt.node->m_type)) {
						return;
					}

					ASMContextNodeJumpOperator* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
					if (j->m_location == endLoc) {
						j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<foreach>" : "break";
						j->m_special_op = SPECIAL_JUMP_BREAK;
						j->m_showJump = false;
						if (endNodeLocation) {
							endNodeLocation->RemoveRef(j->m_opLoc);
						}
					}
					else if (j->m_location == incrementLoc && j->m_special_op != SPECIAL_JUMP_BREAK) {
						j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<foreach>" : "continue";
						j->m_showJump = false;
						j->m_special_op = SPECIAL_JUMP_CONTINUE;
						setNextKeyStmt.location->RemoveRef(j->m_opLoc);
					}
					});
			}

			if (endNodeLocation) {
				endNodeLocation->RemoveRef(jumpOp->m_opLoc);
			}

			// replace this whole mess by a cool foreach block keyValName
			ASMContextNodeBlock* block = new ASMContextNodeBlock();

			// remove the number of references for the key because maybe we don't use it
			int32_t& keyRef = ctx.m_localvars_ref[keyValName];
			if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW)) {
				keyRef = std::max(keyRef - 6, 0); // key is undefined at the end in mw23
			}
			else if (ctx.m_vm <= VMI_T8_36) {
				keyRef = std::max(keyRef - 5, 0);
			}
			else {
				keyRef = std::max(keyRef - 1, 0); // key isn't used as an iterator in T9
			}

			ASMContextNodeForEach* forEachNode = new ASMContextNodeForEach(setOp->m_right->Clone(), block, ctx.m_localvars_ref[keyValName] ? keyValName : 0, itemValName);

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
			block->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_foreachcontinue>", TYPE_PRECODEPOS), it->location });
			for (size_t i = incrementIndex; i < endNodeIndex; i++) {
				// remove component statement
				delete it->node;
				it = m_statements.erase(it);
			}
			if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW)) {
				// not present during the beta
				if (it != m_statements.end()) {
					// keyValName = undefined
					// arrayRefName = undefined

					if (it->node->m_type == TYPE_SET) {
						ASMContextNodeIdentifier* tmp;
						auto* clearKeyVal = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
						if (clearKeyVal->m_left->m_type == TYPE_IDENTIFIER
							&& ((tmp = static_cast<ASMContextNodeIdentifier*>(clearKeyVal->m_left))->m_value == keyValName || tmp->m_value == arrayRefName)
							&& clearKeyVal->m_right->m_type == TYPE_GET_UNDEFINED) {
							delete it->node;
							it = m_statements.erase(it);

							if (it != m_statements.end()) {

								if (it->node->m_type == TYPE_SET) {
									auto* clearKeyVal = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
									if (clearKeyVal->m_left->m_type == TYPE_IDENTIFIER
										&& ((tmp = static_cast<ASMContextNodeIdentifier*>(clearKeyVal->m_left))->m_value == keyValName || tmp->m_value == arrayRefName)
										&& clearKeyVal->m_right->m_type == TYPE_GET_UNDEFINED) {
										delete it->node;
										it = m_statements.erase(it);
									}
								}
							}
						}
					}
				}
			}

			forEachNode->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
				block->ComputeForEachBlocks(ctx);
				}, ctx);
			// go back to the first index
			index = startIndex;
		}

		return 0;

	}
	int ASMContextNodeBlock::ComputeForEachBlocks(ASMContext& ctx) {
		if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW_KEYS)) {
			return ComputeForEachBlocksKeys(ctx);
		}
		return ComputeForEachBlocksIterator(ctx);
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
				int64_t startLoc = jumpOp->m_location;

				size_t startIndex = 0;

				if (startLoc != jumpStmt.location->rloc) {
					while (startIndex < index && m_statements[startIndex].location->rloc < startLoc) {
						startIndex++;
					}

					if (startIndex == index || m_statements[startIndex].location->rloc != startLoc) {
						itindex++; // not in the same block
						continue;
					}
				}
				else {
					startIndex = index;
				}

				// start of the loop location
				auto* continueLocation = jumpStmt.location;
				// start of the loop location
				int32_t continueLoc = continueLocation->rloc;

				// end location for the break, can be an infinite loop, so we consider it
				ASMContextLocation* endLocation = nullptr;
				int64_t breakLoc = -0xFFFF; // random value
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
						//startIndex++;
					}
				}

				auto* block = new ASMContextNodeBlock();
				if (!(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
					cond = ASMCNodeConvertToBool(cond, ctx);
				}
				auto* node = new ASMContextNodeWhile(cond, block, jumpOp->Clone());
				for (size_t i = startIndex; i < index; i++) {
					auto* ref = it->node;

					ASMContextNode* replaceNode = nullptr;

					if (IsJumpType(ref->m_type)) {
						// replace jumps
						auto* j = static_cast<ASMContextNodeJumpOperator*>(ref);
						if (j->m_location == breakLoc) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<inf>" : "break";
							j->m_special_op = SPECIAL_JUMP_BREAK;
							j->m_showJump = false;
							assert(endLocation); // it would mean breakLoc was set
							endLocation->RemoveRef(j->m_opLoc);
						}
						else if (j->m_location == continueLoc && j->m_special_op != SPECIAL_JUMP_BREAK) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<inf_end_it>" : "continue";
							j->m_special_op = SPECIAL_JUMP_CONTINUE;
							j->m_showJump = false;
							continueLocation->RemoveRef(j->m_opLoc);
						}
						else if (j->m_location == jumpOp->m_location && j->m_special_op != SPECIAL_JUMP_BREAK) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<inf_start_it>" : "continue";
							j->m_special_op = SPECIAL_JUMP_CONTINUE;
							j->m_showJump = false;
							firstNodeLocation->RemoveRef(j->m_opLoc);
						}
					}
					else if (ref->m_type == TYPE_SWITCH_POSTCOMPUTE || ref->m_type == TYPE_BLOCK) {
						ref->ApplySubBlocks([&](ASMContextNodeBlock* block, ASMContext& context) {
							for (const auto& stmt : block->m_statements) {
								auto* refjump = stmt.node;
								if (IsJumpType(refjump->m_type)) {
									// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
									//and do not remove the ref because it's nested breaks/continues
									auto* j = static_cast<ASMContextNodeJumpOperator*>(refjump);
									if (j->m_location == breakLoc) {
										if (!j->m_special_op) {
											j->m_operatorName = context.m_opt.m_mark_jump_type ? "break<inf_block>" : "break";
											j->m_special_op = SPECIAL_JUMP_BREAK;
											j->m_showJump = false;
										}
										//if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										assert(endLocation); // it would mean breakLoc was set
										endLocation->RemoveRef(j->m_opLoc);
										//}
									}
									else if (j->m_location == continueLoc && j->m_special_op != SPECIAL_JUMP_BREAK) {
										if (!j->m_special_op) {
											j->m_operatorName = context.m_opt.m_mark_jump_type ? "continue<inf_block_end_it>" : "continue";
											j->m_special_op = SPECIAL_JUMP_CONTINUE;
											j->m_showJump = false;
										}
										//if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										continueLocation->RemoveRef(j->m_opLoc);
										//}
									}
									else if (j->m_location == jumpOp->m_location && j->m_special_op != SPECIAL_JUMP_BREAK) {
										if (!j->m_special_op) {
											j->m_operatorName = context.m_opt.m_mark_jump_type ? "continue<inf_block_start_it>" : "continue";
											j->m_special_op = SPECIAL_JUMP_CONTINUE;
											j->m_showJump = false;
										}
										//if (ref->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										firstNodeLocation->RemoveRef(j->m_opLoc);
										//}
									}
								}
							}
							}, ctx);
					}

					block->m_statements.push_back({ replaceNode ? replaceNode : ref->Clone(), it->location });
					delete it->node;
					it = m_statements.erase(it);
				}
				block->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_inf_end>", TYPE_PRECODEPOS), it->location });
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
				int32_t breakLoc = endLocation->rloc;
				// loop location for the continue
				int64_t continueLoc = jumpOp->m_location;

				auto* contLocation = m_statements[index].location;
				int64_t continueLoc2 = contLocation->rloc;

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

				if (!(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
					cond = ASMCNodeConvertToBool(cond, ctx);
				}
				auto* doWhile = new ASMContextNodeDoWhile(cond, newBlock, jumpOp->Clone());

				auto it = m_statements.begin() + startIndex;
				for (size_t i = startIndex; i < index; i++) {
					auto& stmt = m_statements[startIndex];

					ASMContextNode* replaceNode = nullptr;

					// compute special jump
					if (IsJumpType(stmt.node->m_type)) {
						auto* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
						if (j->m_location == breakLoc) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<do_while>" : "break";
							j->m_showJump = false;
							j->m_special_op = SPECIAL_JUMP_BREAK;
							endLocation->RemoveRef(j->m_opLoc);
						}
						else if (j->m_location == continueLoc && j->m_special_op != SPECIAL_JUMP_BREAK) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<do_while>" : "continue";
							j->m_showJump = false;
							j->m_special_op = SPECIAL_JUMP_CONTINUE;
							condLocation->RemoveRef(j->m_opLoc);
						}
						else if (j->m_location == continueLoc2 && j->m_special_op != SPECIAL_JUMP_BREAK) {
							j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<do_while2>" : "continue";
							j->m_showJump = false;
							j->m_special_op = SPECIAL_JUMP_CONTINUE;
							contLocation->RemoveRef(j->m_opLoc);
						}
					}
					else if (stmt.node->m_type == TYPE_SWITCH_POSTCOMPUTE || stmt.node->m_type == TYPE_BLOCK) {
						ApplySubStatement(stmt, ctx, [&stmt, breakLoc, &ctx, endLocation, continueLoc, condLocation, contLocation, continueLoc2](ASMContextStatement& refstmt) {
							auto* refjump = refstmt.node;
							if (IsJumpType(refjump->m_type)) {
								// replace jumps in switch (BECAUSE SOMEONE DID IT), show the jumps 
								//and do not remove the ref because it's nested breaks/continues
								auto* j = static_cast<ASMContextNodeJumpOperator*>(refjump);
								if (j->m_location == breakLoc) {
									if (!j->m_special_op) {
										j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "break<do_while_block2>" : "break";
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
									if (!j->m_special_op && j->m_special_op != SPECIAL_JUMP_BREAK) {
										j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<do_while_block3>" : "continue";
										j->m_special_op = SPECIAL_JUMP_CONTINUE;
										//j->m_showJump = true;
										j->m_showJump = false;
									}
									if (stmt.node->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										condLocation->RemoveRef(j->m_opLoc);
									}
								}
								else if (j->m_location == continueLoc2) {
									if (!j->m_special_op && j->m_special_op != SPECIAL_JUMP_BREAK) {
										j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<do_while_block4>" : "continue";
										j->m_special_op = SPECIAL_JUMP_CONTINUE;
										//j->m_showJump = true;
										j->m_showJump = false;
									}
									if (stmt.node->m_type == TYPE_BLOCK) {
										j->m_showJump = false;
										contLocation->RemoveRef(j->m_opLoc);
									}
								}
							}
							});
					}

					newBlock->m_statements.push_back({ replaceNode ? replaceNode : stmt.node->Clone(), stmt.location });
					// clear node
					delete stmt.node;
					it = m_statements.erase(it);
				}
				// clear the jump node
				itindex = startIndex;
				newBlock->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_whileend>", TYPE_PRECODEPOS), m_statements[itindex].location });
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

			if (!(jumpOp->m_special_op || jumpOp->m_delta >= 0) || jumpStmt.node->m_type == TYPE_JUMP) {
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

			if (endIndex > index + 1 && m_statements[endIndex - 1].node->m_type == TYPE_JUMP) {
				auto* jumpOpEndElse = static_cast<ASMContextNodeJumpOperator*>(m_statements[endIndex - 1].node);
				// after else goto
				if (jumpOpEndElse->m_delta > 0 && jumpOpEndElse->m_special_op == SPECIAL_JUMP_DEFAULT && !jumpOpEndElse->m_returnCandidate) {
					auto endElseLoc = jumpOpEndElse->m_location;
					// should be positive and not a special jump

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
						blockElse->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_elselast>", TYPE_PRECODEPOS), endElseFinalLoc });
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
				blockIf->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_ifend1>", TYPE_PRECODEPOS), it->location });
				delete it->node;
				it = m_statements.erase(it);
			}
			else {
				blockIf->m_statements.push_back({ new ASMContextNodeValue<const char*>("<emptypos_ifend2>", TYPE_PRECODEPOS), elsePartNodeLoc });
			}

			// swap the jump with the new if statement
			assert(IsJumpType(m_statements[index].node->m_type) && m_statements[index].node->m_type != TYPE_JUMP);

			ASMContextNode* ifCond = JumpCondition(static_cast<ASMContextNodeJumpOperator*>(m_statements[index].node), true);
			if (!(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
				ifCond = ASMCNodeConvertToBool(ifCond, ctx);
			}
			auto* ifElse = new ASMContextNodeIfElse(ifCond, blockIf, blockElse);
			delete m_statements[index].node;
			m_statements[index].node = ifElse;
			ifElse->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) {
				block->ComputeIfBlocks(ctx);
				}, ctx);
		}

		// LOC_0000015c:jumpiffalse(!self function_a39f313c() || self getcurrentweapon() == level.weaponnone) LOC_0000017c delta: 0x1c (type:9/spec:false)

		return 0;
	}

	bool IsForDeltaCandidate(ASMContextNode* node, uint64_t incrementName) {
		if (node->m_type == TYPE_DELTA) {
			// i++ i--
			auto* deltaPart = static_cast<ASMContextNodeOp1*>(node);

			return deltaPart->m_operand->m_type == TYPE_IDENTIFIER && static_cast<ASMContextNodeIdentifier*>(deltaPart->m_operand)->m_value == incrementName;
		}
		if (node->m_type == TYPE_SET) {
			// i = ????
			auto* setOp = static_cast<ASMContextNodeLeftRightOperator*>(node);

			return setOp->m_left->m_type == TYPE_IDENTIFIER && static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value == incrementName;
		}
		return false;
	}

	int ASMContextNodeBlock::ComputeForBlocks(ASMContext& ctx) {
		/*

			i = 0;
			while (i < var_8f5e3947.size) {
				var_4e2b3e3a[i] = var_8f5e3947[i].origin;
				i++;
				<emptypos_inf_end>;
			}

			i = ???
			while (i ???) {

				i = i ???
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

			uint64_t incrementName = static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value;

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

			if (!IsForDeltaCandidate(deltaStmt.node, incrementName)) {
				index++;
				continue; // not a delta for part
			}

			auto* forDeltaNode = new ASMContextNodeForDelta(initValNode->Clone(), /*Already a bool*/ whileBlock->m_condition, deltaStmt.node->Clone(), whileBlock->m_block, whileBlock->m_originJump->Clone());
			whileBlock->m_block = nullptr;
			whileBlock->m_condition = nullptr;
			delete whileStmt.node;
			whileStmt.node = forDeltaNode;


			delete initValStmt.node;
			initValStmt.node = new ASMContextNodeValue<const char*>("<for_init>", TYPE_PRECODEPOS);

			delete deltaStmt.node;
			deltaStmt.node = new ASMContextNodeValue<const char*>("<for_continue>", TYPE_PRECODEPOS);
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
					if (j->m_location == continueLoc->rloc && j->m_special_op != SPECIAL_JUMP_BREAK) {
						j->m_operatorName = ctx.m_opt.m_mark_jump_type ? "continue<for>" : "continue";
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

	int ASMContextNodeBlock::ComputeReturnJump(ASMContext& ctx) {
		auto remapJump = [&ctx](ASMContextStatement& stmt) {
			if (IsJumpType(stmt.node->m_type)) {
				auto* jmp = dynamic_cast<ASMContextNodeJumpOperator*>(stmt.node);

				if (jmp->m_returnCandidate && !jmp->m_special_op) {
					// we replace this node by a return or a if return

					auto* returnNode = new ASMContextNodeValue<const char*>("<END_JUMP>", TYPE_END);

					auto* cond = JumpCondition(jmp, true);

					if (cond) {
						ASMContextNodeBlock* blockIf = new ASMContextNodeBlock();
						blockIf->m_statements.emplace_back(returnNode, stmt.location);
						delete stmt.node;

						if (!(ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
							cond = ASMCNodeConvertToBool(cond, ctx);
						}
						stmt.node = new ASMContextNodeIfElse(cond, blockIf, nullptr);
					}
					else {
						delete stmt.node;
						stmt.node = returnNode;
					}

				}
			}

			};

		for (size_t i = 0; i < m_statements.size(); i++) {
			auto& stmt = m_statements[i];

			ApplySubStatement(stmt, ctx, remapJump);
		}

		return 0;
	}

	int ASMContextNodeBlock::ComputeBoolReturn(ASMContext& ctx) {
		if (ctx.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
			return 0;
		}
		bool isCandidate{ true };
		bool anyReturn{};

		// check all the return values, if all of them are bools, we can replace to bool
		for (size_t i = 0; i < m_statements.size(); i++) {
			auto& stmt = m_statements[i];

			ApplySubStatement(stmt, ctx, [&isCandidate, &anyReturn, &ctx](ASMContextStatement& stmt) {
				if (stmt.node->m_type == TYPE_END) {
					isCandidate = false;
					return; // empty return -> return undefined
				}

				if (stmt.node->m_type != TYPE_RETURN) {
					return;
				}
				anyReturn = true;

				if (!static_cast<ASMContextNodeOp1*>(stmt.node)->m_operand->IsBoolConvertable(true, ctx)) {
					isCandidate = false;
					return;
				}
				// ok
				}, true);

			if (!isCandidate) {
				return 0;
			}
		}

		if (!anyReturn) isCandidate = false;

		if (!isCandidate) {
			return 0;
		}

		ctx.m_boolFuncCandidate = true;
		{
			core::async::opt_lock_guard lg{ ctx.m_objctx.gdctx.asyncMtx };

			ctx.m_exp.SetHandle(ctx.m_readerHandle);
			NameLocated loc{};
			loc.name = ctx.m_exp.GetName();
			loc.name_space = ctx.m_exp.GetNamespace();

			ctx.m_objctx.gdctx.exportInfos[loc].boolFunc = true;
		}

		// convert the return values to booleans
		for (size_t i = 0; i < m_statements.size(); i++) {
			auto& stmt = m_statements[i];

			ApplySubStatement(stmt, ctx, [&ctx](ASMContextStatement& stmt) {
				if (stmt.node->m_type != TYPE_RETURN) {
					return;
				}

				auto* ret = reinterpret_cast<ASMContextNodeOp1*>(stmt.node);

				ret->m_operand = ASMCNodeConvertToBool(ret->m_operand, ctx);
				}, false);
		}

		return 0;
	}

	namespace {
		void ApplyPreSpecialPatternBlock(ASMContext& ctx, std::vector<ASMContextStatement>& stmts) {
			/* // Replace ondefined jump to default_to, but it's not really important and probably annoying to add in the compiler
			for (size_t i = 0; i < stmts.size(); i++) {
				auto& stmt = stmts[i];

				stmt.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) { ApplyPreSpecialPatternBlock(ctx, block->m_statements); }, ctx);

				if (!(ctx.m_opt.m_stepskip & tool::gsc::STEPSKIP_SPECIAL_PATTERN)) {
					if (i < 2 || stmts[i - 1].node->m_type != TYPE_SET || stmts[i - 2].node->m_type != TYPE_JUMP_ONDEFINED) {
						continue;
					}

					ASMContextNodeLeftRightOperator* set = dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[i - 1].node);
					ASMContextNodeJumpOperator* jump = dynamic_cast<ASMContextNodeJumpOperator*>(stmts[i - 2].node);

					if (!IsStructSimilar(set->m_left, jump->m_operand) || jump->m_location != stmt.location->rloc) {
						continue;
					}

					i -= 2;
					// replace ifdefined to default_to
					ASMContextNodeMultOp* defaultToNode = new ASMContextNodeMultOp("default_to", false);

					defaultToNode->AddParam(set->m_left);
					defaultToNode->AddParam(set->m_right);

					set->m_left	= nullptr;
					set->m_right = nullptr;
					delete set;

					// delete jump and refs
					stmts[i + 2].location->RemoveRef(jump->m_opLoc);
					delete stmts[i].node;
					stmts.erase(stmts.begin() + i + 1);
					stmts[i].node = defaultToNode;
					continue;
				}
			}
			*/
		}
		void ApplySpecialPatternBlock(ASMContext& ctx, std::vector<ASMContextStatement>& stmts) {
			for (size_t i = 0; i < stmts.size(); i++) {
				auto& stmt = stmts[i];

				stmt.node->ApplySubBlocks([](ASMContextNodeBlock* block, ASMContext& ctx) { ApplySpecialPatternBlock(ctx, block->m_statements); }, ctx);

				if (!(ctx.m_opt.m_stepskip & tool::gsc::STEPSKIP_SPECIAL_PATTERN)) {
					ASMContextNodeLeftRightOperator* mainSet;
					if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_LIKE) &&
						i && stmts[i - 1].node->m_type == TYPE_SET
						&& (mainSet = dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[i - 1].node))->m_left->m_type == TYPE_IDENTIFIER
						&& stmt.node->m_type == TYPE_SET) {
						// at this point
						// arr = ...
						// ? = ?
						uint64_t keyHash = dynamic_cast<ASMContextNodeIdentifier*>(mainSet->m_left)->m_value;
						/*
							m_localvars_ref
							i - 1 __axx = ...
							i patt1 = __axx[0]
							patt2 = __axx[1]
							...
							pattn = __axx[n]
							convert to [patt1, patt2, ... pattn]
						*/

						auto keyit = ctx.m_localvars_ref.find(keyHash);

						int32_t keyrefs;
						if (keyit != ctx.m_localvars_ref.end() && (keyrefs = keyit->second) > 1 && i + keyrefs - 1 <= stmts.size()) {
							size_t j{ i };
							size_t latest{ (size_t)keyrefs - 1 }; // we remove 1 for the first set ref
							size_t end{ i + keyrefs - 1 };
							for (; j < end; j++) {
								if (stmts[j].node->m_type != TYPE_SET) {
									break; // not: ... = ...
								}
								ASMContextNodeLeftRightOperator* set = dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[j].node);
								if (set->m_right->m_type != TYPE_ARRAY_ACCESS) {
									break; // not: var = ... [ ... ]
								}
								ASMContextNodeArrayAccess* arrayAccess = dynamic_cast<ASMContextNodeArrayAccess*>(set->m_right);
								if (arrayAccess->m_operandleft->m_type != TYPE_IDENTIFIER || !arrayAccess->m_operandright->IsIntConst()
									|| dynamic_cast<ASMContextNodeIdentifier*>(arrayAccess->m_operandleft)->m_value != keyHash
									) {
									break; // not: var = arr [ x ]
								}

								int64_t idx = arrayAccess->m_operandright->GetIntConst();

								if (idx < 0) break; // can't have negative index

								if ((size_t)(idx + 1) != latest) {
									break; // not: var = arr [ n - 1 ]
								}

								latest = (size_t)idx;
							}

							if (latest == 0) {
								// latest > 0 : bad index looking
								ASMContextNodeArrayBuild* bld = new ASMContextNodeArrayBuild();

								if (mainSet->m_left) delete mainSet->m_left;

								mainSet->m_left = bld;

								--i; // put the cursor at start

								for (size_t k = 1; k < keyrefs; k++) {
									ASMContextNodeLeftRightOperator* set = dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[i + (keyrefs - k)].node);
									assert(set != nullptr);

									// use the left identifier in our array
									bld->AddValue(new ASMContextNodeValue<size_t>(k - 1, TYPE_VALUE, false, true, true), set->m_left, false);
									set->m_left = nullptr;
									delete stmts[i + (keyrefs - k)].node;
									stmts.erase(stmts.begin() + (i + (keyrefs - k)));
								}

							}

						}
					}
					if (i
						/*
						  unk = undefined;
						  unk = ... waittill(...);
						*/
						&& stmt.node->m_type == TYPE_SET && dynamic_cast<ASMContextNodeLeftRightOperator*>(stmt.node)->m_right->m_type == TYPE_WAITTILL
						&& stmts[i - 1].node->m_type == TYPE_SET && dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[i - 1].node)->m_right->m_type == TYPE_GET_UNDEFINED
						&& IsStructSimilar(dynamic_cast<ASMContextNodeLeftRightOperator*>(stmt.node)->m_left, dynamic_cast<ASMContextNodeLeftRightOperator*>(stmts[i - 1].node)->m_left)
						) {
						i--;

						// remove the = undefined and apply the new location
						delete stmts[i].node;
						stmts[i + 1].location = stmts[i].location;
						stmts.erase(stmts.begin() + i);
						continue;
					}

				}

				if (!(ctx.m_opt.m_stepskip & tool::gsc::STEPSKIP_DEVBLOCK_INLINE)
					&& stmt.node->m_type == TYPE_BLOCK) {
					// inline dev calls
					auto* db = dynamic_cast<ASMContextNodeBlock*>(stmt.node);

					//ASMContextNode* (const std::vector<ASMContextStatement>&statements, size_t index) {
					if (db->m_blockType == BLOCK_DEV && SizeNoEmptyNode(db->m_statements) == 1 && GetNoEmptyNode(db->m_statements, 0)->node->m_type == TYPE_FUNC_CALL) {
						auto* dbStmt = GetNoEmptyNode(db->m_statements, 0);
						auto* fc = dynamic_cast<ASMContextNodeCallFuncPtr*>(dbStmt->node);

						if (fc && fc->m_ftype == FUNCTION_CALL) {
							auto* name = fc->m_operands[fc->m_flags & ASMContextNodeCallFuncFlag::SELF_CALL ? 1 : 0];
							if (name->m_type == TYPE_FUNC_REFNAME) {
								uint64_t refName = dynamic_cast<ASMContextNodeFuncRef*>(name)->m_func & hash::MASK60;
								if (ctx.m_objctx.m_vmInfo->devCallsNames.find(refName) != ctx.m_objctx.m_vmInfo->devCallsNames.end()) {
									// inline the dev call
									stmt.node = fc;
									dbStmt->node = nullptr;
									delete db;
								}
							}

						}
					}
				}

				// remove 
				uint64_t constr = ctx.m_objctx.m_vmInfo->HashField("__constructor");
				uint64_t desstr = ctx.m_objctx.m_vmInfo->HashField("__destructor");

				if (ctx.m_exp.GetName() == constr) {
					for (size_t i = 0; i < stmts.size();) {
						auto& stmt = stmts[i++];

						if (stmt.node->m_type == TYPE_FUNC_CALL) {
							auto* fc = dynamic_cast<ASMContextNodeCallFuncPtr*>(stmt.node);

							if (fc->m_ftype == FUNCTION_CALL && !(fc->m_flags & SELF_CALL) && fc->m_operands[0]->m_type == TYPE_FUNC_REFNAME
								&& (dynamic_cast<ASMContextNodeFuncRef*>(fc->m_operands[0])->m_func == constr)) {
								i--;
								stmts.erase(stmts.begin() + i);
								continue;
							}
						}
					}
				}
				else if (ctx.m_exp.GetName() == desstr) {
					for (size_t i = 0; i < stmts.size();) {
						auto& stmt = stmts[i++];

						if (stmt.node->m_type == TYPE_FUNC_CALL) {
							auto* fc = dynamic_cast<ASMContextNodeCallFuncPtr*>(stmt.node);

							if (fc->m_ftype == FUNCTION_CALL && !(fc->m_flags & SELF_CALL) && fc->m_operands[0]->m_type == TYPE_FUNC_REFNAME
								&& (dynamic_cast<ASMContextNodeFuncRef*>(fc->m_operands[0])->m_func == desstr)) {
								i--;
								stmts.erase(stmts.begin() + i);
								continue;
							}
						}
					}
				}
			}
			// x = x op y -> x op= y
			ctx.ForSubNodes([](ASMContextNode*& node, SubNodeContext& ctx) {
				if (node->m_type == TYPE_SET) {
					auto* set = dynamic_cast<ASMContextNodeLeftRightOperator*>(node);
					if (set->m_right && set->m_right->m_type == TYPE_OPERATION_MERGE) {
						auto* op2 = dynamic_cast<ASMContextNodeOp2*>(set->m_right);
						if (IsStructSimilar(set->m_left, op2->m_operand1)) {
							set->m_right = op2->m_operand2;
							op2->m_operand2 = nullptr;

							set->m_operatorName = std::format(" {}= ", op2->m_description);
							set->m_type = TYPE_SET_OP;
							delete op2;
						}
					}
				}
				});
		}

		uint64_t GetLeftIdentifier(ASMContextNode* node) {
			if (!node) {
				return 0; // bad node?
			}

			if (node->m_type == TYPE_SET || node->m_type == TYPE_ACCESS) {
				return GetLeftIdentifier(dynamic_cast<ASMContextNodeLeftRightOperator*>(node)->m_left);
			}

			if (node->m_type == TYPE_IDENTIFIER) {
				return dynamic_cast<ASMContextNodeIdentifier*>(node)->m_value;
			}

			return 0;
		}

	}

	int ASMContextNodeBlock::ComputeSpecialPattern(ASMContext& ctx) {
		// apply special structural changes
		ApplySpecialPatternBlock(ctx, m_statements);

		return 0;
	}
	int ASMContextNodeBlock::ComputePreSpecialPattern(ASMContext& ctx) {
		// apply pre special structural changes
		ApplyPreSpecialPatternBlock(ctx, m_statements);

		return 0;
	}

	int ASMContextNodeBlock::ComputeCustomCompilerPattern(ASMContext& ctx) {
		//hookfuncname(#test, ...) ->  compiler::test(...)
		uint64_t compilerHookFunctionName{ ctx.m_objctx.m_vmInfo->compilerHookFunctionName & hash::MASK60 };
		if (!compilerHookFunctionName) {
			return 0; // nothing to do
		}
		for (size_t i = 0; i < m_statements.size(); i++) {
			auto& stmt = m_statements[i];

			ApplySubStatement(stmt, ctx, [compilerHookFunctionName, &ctx](ASMContextStatement& stmt) {
				if (stmt.node->m_type != TYPE_FUNC_CALL) {
					return; // func calls
				}

				ASMContextNodeCallFuncPtr* fc{ dynamic_cast<ASMContextNodeCallFuncPtr*>(stmt.node) };
				if (fc->m_operands.size() < 2 || fc->m_operands[0]->m_type != TYPE_FUNC_REFNAME || (fc->m_operands[1]->m_type != TYPE_VALUE && fc->m_operands[1]->m_type != TYPE_CONST_HASH)) {
					return; // not at least a func call with a value
				}

				ASMContextNodeFuncRef* name{ dynamic_cast<ASMContextNodeFuncRef*>(fc->m_operands[0]) };
				if ((name->m_func & hash::MASK60) != compilerHookFunctionName) {
					return; // not the right function name
				}

				uint64_t funcName{ static_cast<uint64_t>(fc->m_operands[1]->GetIntConst()) };
				if (!funcName) {
					return; // invalid function name
				}

				// replace the namespace/name
				name->m_nsp = hash::HashT89Scr("compiler");
				name->m_func = funcName;

				// remove the first param
				delete fc->m_operands[1];
				fc->m_operands.erase(fc->m_operands.begin() + 1);
				});
		}

		return 0;
	}

	void ASMContext::ConvertToClassMethod(std::unordered_set<uint64_t>& selfmembers) {
		if (m_opt.m_stepskip & tool::gsc::STEPSKIP_CLASSMEMBER_INLINE) {
			return; // ignore
		}
		ForSubNodes([&selfmembers](ASMContextNode*& node, SubNodeContext& ctx) {
			if (node->m_type == TYPE_ACCESS) {
				auto* set = dynamic_cast<ASMContextNodeLeftRightOperator*>(node);

				if (set->m_left->m_type == TYPE_SELF) {
					uint64_t name = GetLeftIdentifier(set->m_right);

					if (name) {
						// remove the self call and add this member to the self members
						selfmembers.insert(name);
						node = set->m_right;
						set->m_right = nullptr;
						delete set;
					}
				}
			}

			if (node->m_type == TYPE_FUNC_CALL) {
				auto* fc = dynamic_cast<ASMContextNodeCallFuncPtr*>(node);
				if (fc->m_ftype == CLASS_CALL) {
					auto* ptr = fc->m_operands[0];
					auto* funcName = fc->m_operands[1];

					if (ptr->m_type == TYPE_SELF && funcName->m_type == TYPE_IDENTIFIER) {
						// (thread|childthread)? [[ self ]]->idf(...)
						// convert to :  (thread|childthread)? idf(...)

						delete ptr;
						fc->m_operands.erase(fc->m_operands.begin());
						fc->m_ftype = FUNCTION_CALL;
					}
				}
			}


			});
	}

}
