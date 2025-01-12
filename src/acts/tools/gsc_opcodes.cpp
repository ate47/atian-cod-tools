#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_formatter.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_opcode_nodes.hpp"
using namespace tool::gsc::opcode;

#pragma region opcode_node

namespace {
#undef assert
#ifdef CI_BUILD
# define assert(x) (void)(x)
#else
# define assert(x) if (!(x)) LOG_WARNING("assert error {}", #x)
#endif
	// maps to find the opcodes
	
	// VM->vminfo
	std::unordered_map<uint64_t, VmInfo> g_opcodeMap{};
	std::unordered_map<uint64_t, uint64_t> g_vmMap{};
	// opcode->opcode handler
	std::unordered_map<OPCode, const OPCodeInfo*> g_opcodeHandlerMap{};
}
namespace tool::gsc::opcode {
	VMId VMOf(const char* name) {
		RegisterOpCodes();
		auto it = g_vmMap.find(hash::Hash64(name));

		if (it != g_vmMap.end()) {
			return (VMId)it->second;
		}
		return VMI_UNKNOWN;
	}
	VMId OldVmOf(byte oldvm) {
		switch (oldvm) {
		case VMOld_T71B: return VMI_T71B;
		case VMOld_T7: return VMI_T7;
		case VMOld_T831: return VMI_T831;
		case VMOld_T8: return VMI_T8;
		case VMOld_T937: return VMI_T937;
		case VMOld_T9: return VMI_T9;
		case VMOld_MW23: return VMI_JUP_8A;
		case VMOld_MW23B: return VMI_JUP_8B;
		case VMOld_BO6_06: return VMI_T10_06;
		case VMOld_BO6_07: return VMI_T10_07;
		case VMOld_BO6_0B: return VMI_T10_0B;
		case VMOld_BO6_0C: return VMI_T10_0C;
		case VMOld_ACTS_F1: return VMI_ACTS_F1;
		default: return VMI_UNKNOWN;
		}
	}
	byte MapAsOldVM(uint64_t magicVal) {
		if ((magicVal & ~0xFF) == 0xa0d43534700) {
			return magicVal & 0xFF; // IW
		}
		else if ((magicVal & ~0xFF00000000000000) == 0xa0d43534780) {
			return (magicVal >> 56) & 0xFF; // Treyarch
		}
		return 0; // unk
	}

	Platform PlatformOf(const char* name) {
		if (!_strcmpi("pc", name)) {
			return PLATFORM_PC;
		}
		if (!_strcmpi("pc_alpha", name) || !_strcmpi("pca", name) || !_strcmpi("alpha", name) || !_strcmpi("a", name)) {
			return PLATFORM_PC_ALPHA;
		}
		if (!_strcmpi("xbox", name) || !_strcmpi("xb", name)) {
			return PLATFORM_XBOX;
		}
		if (!_strcmpi("ps", name) || !_strcmpi("ps4", name) || !_strcmpi("playstation", name)) {
			return PLATFORM_PLAYSTATION;
		}
		return PLATFORM_UNKNOWN;
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

	/*
	 * Convert a node to a boolean node, the input value might be free
	 * @param node node to convert
	 * @return boolean version of node or node
	 */
	ASMContextNode* ASMCNodeConvertToBool(ASMContextNode* node) {
		if (!node || !node->IsBoolConvertable(false)) {
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

	const char* PlatformName(Platform plt) {
		switch (plt) {
		case PLATFORM_PC: return "PC";
		case PLATFORM_XBOX: return "Xbox";
		case PLATFORM_PLAYSTATION: return "PlayStation";
		case PLATFORM_PC_ALPHA: return "Alpha";
		default: return "Unknown";
		}
	}


	const char* PlatformIdName(Platform plt) {
		switch (plt) {
		case PLATFORM_PC: return "pc";
		case PLATFORM_XBOX: return "xbox";
		case PLATFORM_PLAYSTATION: return "ps";
		case PLATFORM_PC_ALPHA: return "pc_alpha";
		default: return "unk";
		}
	}

	std::ostream& operator<<(std::ostream& os, const ASMContextNode& obj) {
		// fake decomp context
		DecompContext ctx{ 0, 0, {}, 0 };
		obj.Dump(os, ctx);
		return os;
	}

	bool IsJumpConditionForceReversed(const ASMContextNodeJumpOperator* op) {
		return op->m_type == TYPE_JUMP_ONFALSE || op->m_type == TYPE_JUMP_ONFALSEEXPR;
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
}

void VmInfo::AddDevCallName(uint64_t name) {
	devCallsNames.insert(name & hash::MASK62);
}

uint64_t VmInfo::HashField(const char* value) const {
	uint64_t t;
	if (!value || !*value) {
		return 0;
	}
	if (hash::TryHashPattern(value, t)) {
		return t;
	}
	if (HasFlag(VmFlags::VMF_HASH_CER)) {
		return hash::HashT10Scr(value);
	}
	if (HasFlag(VmFlags::VMF_HASH_CER_SP)) {
		return hash::HashT10ScrSP(value);
	}
	if (HasFlag(VmFlags::VMF_HASH_IW)) {
		return hash::HashJupScr(value);
	}
	if (HasFlag(VmFlags::VMF_HASH_T7)) {
		return hash::HashT7(value);
	}
	return hash::HashT89Scr(value);
}

uint64_t VmInfo::HashPath(const char* value) const {
	uint64_t t;
	if (!value || !*value) {
		return 0;
	}
	if (hash::TryHashPattern(value, t)) {
		return t;
	}
	if (HasFlag(VmFlags::VMF_HASH_PATH_IW)) {
		return hash::HashIWRes(value);
	}
	return hash::Hash64(value);
}

uint64_t VmInfo::HashFilePath(const char* value) const {
	return HashField(value);
}

OPCodeInfo::OPCodeInfo(OPCode id, const char* name) : m_id(id), m_name(name) {
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

ASMContextNodeBlock::ASMContextNodeBlock(nodeblocktype blockType, bool disabled, bool allowInline) : ASMContextNode(PRIORITY_INST, TYPE_BLOCK), m_blockType(blockType), m_disabled(disabled), m_allowInline(allowInline) {
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

ASMContextNode* ASMContextNodeBlock::Clone() const {
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

ASMContextNode* ASMContextNode::Clone() const {
	assert(0); // shouldn't be possible
	return nullptr;
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

bool ASMContextNode::IsBoolConvertable(bool strict) {
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

namespace {
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

	FLOAT GetFloatNumber(ASMContextNode* node) {
		if (node->m_type == TYPE_FLOAT) {
			return dynamic_cast<ASMContextNodeValue<FLOAT>*>(node)->m_value;
		}
		else {
			return (FLOAT)node->GetIntConst();
		}
	}


	bool IsEqualNumber(ASMContextNode* a, ASMContextNode* b) {
		if (!a || !b || !a->IsConstNumber() || !b->IsConstNumber()) {
			return false;
		}
		auto* aa = dynamic_cast<ASMContextNodeValueVir*>(a);
		auto* bb = dynamic_cast<ASMContextNodeValueVir*>(b);
		auto dt = aa->GetDatatype();

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
			auto* veca = dynamic_cast<ASMContextNodeVector*>(a);
			auto* vecb = dynamic_cast<ASMContextNodeVector*>(b);
			
			return IsStructSimilar(veca->m_x, vecb->m_x) && IsStructSimilar(veca->m_y, vecb->m_y) && IsStructSimilar(veca->m_z, vecb->m_z);
		}
		case TYPE_TERNARY: {
			auto* tera = dynamic_cast<ASMContextNodeTernary*>(a);
			auto* terb = dynamic_cast<ASMContextNodeTernary*>(b);
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
			auto* aa = dynamic_cast<ASMContextNodeOp1*>(a);
			auto* bb = dynamic_cast<ASMContextNodeOp1*>(b);

			if (_strcmpi(aa->m_description, bb->m_description)) {
				return false;
			}
			return IsStructSimilar(aa->m_operand, bb->m_operand);
		}
		case TYPE_OPERATION2:
		case TYPE_OPERATION_MERGE:
		{
			auto* aa = dynamic_cast<ASMContextNodeOp2*>(a);
			auto* bb = dynamic_cast<ASMContextNodeOp2*>(b);

			if (_strcmpi(aa->m_description, bb->m_description)) {
				return false;
			}
			return IsStructSimilar(aa->m_operand1, bb->m_operand1)
				&& IsStructSimilar(aa->m_operand2, bb->m_operand2);
		}
		case TYPE_ACCESS:
		case TYPE_SET: {
			auto* aa = dynamic_cast<ASMContextNodeLeftRightOperator*>(a);
			auto* bb = dynamic_cast<ASMContextNodeLeftRightOperator*>(b);

			return IsStructSimilar(aa->m_left, bb->m_left) && IsStructSimilar(aa->m_right, bb->m_right);
		}
		case TYPE_SET_OP: {
			auto* aa = dynamic_cast<ASMContextNodeLeftRightOperator*>(a);
			auto* bb = dynamic_cast<ASMContextNodeLeftRightOperator*>(b);

			return aa->m_operatorName == bb->m_operatorName 
				&& IsStructSimilar(aa->m_left, bb->m_left) && IsStructSimilar(aa->m_right, bb->m_right);
		}
		case TYPE_FUNC_REFNAME: {
			auto* aa = dynamic_cast<ASMContextNodeFuncRef*>(a);
			auto* bb = dynamic_cast<ASMContextNodeFuncRef*>(b);

			return aa->m_func == bb->m_func
				&& aa->m_script == bb->m_script
				&& aa->m_nsp == bb->m_nsp
				&& !_strcmpi(aa->m_op, bb->m_op);
		}
		case TYPE_FUNC_CALL: {
			auto* aa = dynamic_cast<ASMContextNodeCallFuncPtr*>(a);
			auto* bb = dynamic_cast<ASMContextNodeCallFuncPtr*>(b);
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
}

#pragma endregion
#pragma region opcode_info

class ASMContextLocationOpOp : public ASMContextLocationOp {
private:
	ASMContextNode* m_node;
	ASMContextNodePriority m_priority;
	const char* m_description;
public:
	ASMContextLocationOpOp(ASMContextNode* node, const char* description, ASMContextNodePriority priority) :
		ASMContextLocationOp(description), m_node(node), m_description(description), m_priority(priority) {
	}
	~ASMContextLocationOpOp() {
		if (m_node) {
			delete m_node;
		}
	}
	void Run(ASMContext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
		auto* right = context.PopASMCNode();
		context.PushASMCNode(new ASMContextNodeOp2(m_description, m_priority, m_node ? m_node->Clone() : nullptr, right, true));
	}
};
class ASMContextLocationOpCompleteTernary : public ASMContextLocationOp {
private:
	ASMContextNode* m_operand1;
	ASMContextNode* m_operand2;
	bool m_reverse;
public:
	ASMContextLocationOpCompleteTernary(ASMContextNode* operand1, ASMContextNode* operand2, bool reverse) :
		ASMContextLocationOp("?:"), m_operand1(operand1), m_operand2(operand2), m_reverse(reverse) {
		assert(operand1);
		assert(operand2);
	}
	~ASMContextLocationOpCompleteTernary() {
		delete m_operand1;
		delete m_operand2;
	}
	void Run(ASMContext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
		auto* operand3 = context.PopASMCNode();
		if (m_reverse) {
			context.PushASMCNode(new ASMContextNodeTernary(m_operand1->Clone(), operand3, m_operand2->Clone()));
		}
		else {
			context.PushASMCNode(new ASMContextNodeTernary(m_operand1->Clone(), m_operand2->Clone(), operand3));
		}
	}
};

int OPCodeInfo::Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const {
	out << "\n";
	return 0; // by default nop
}

int OPCodeInfo::Skip(uint16_t value, ASMSkipContext& ctx) const {
	return 0; // by default nop
}

class OPCodeInfoUnknown : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "Unknown operator: " << std::hex << value << "\n";

		byte* oldBcl = context.m_bcl;
		for (size_t j = 0; j < 0x4; j++) {
			context.WritePadding(out);
			for (size_t i = 0; i < 0x8; i++) {
				if (i) {
					out << " ";
				}
				out << std::setfill('0') << std::setw(2) << std::hex << (int)context.m_bcl[i];
			}

			out << " | " << *(float*)&context.m_bcl[0] << "/" << *(float*)&context.m_bcl[4] << "/" << std::hex << *(uint64_t*)&context.m_bcl[0];
			out << "\n";
			context.m_bcl += 8;
		}
		context.m_bcl = oldBcl;
		// check strings
		for (size_t j = 0; j < 0x32; j++) {
			uint32_t floc = context.ScriptAbsoluteLocation(context.m_bcl);
			const char* cand{ objctx.GetStringValueByLoc(floc) };
			if (cand) {
				context.WritePadding(out);
				out << "match string loc u8:0x" << (int)*(byte*)context.m_bcl;
				out << ", u16:0x" << *(uint16_t*)context.m_bcl;
				out << ", u32:0x" << *(uint32_t*)context.m_bcl;
				context.WritePadding(out << "\n");
				utils::PrintFormattedString(out << "\"", cand) << "\"";
				out << "\n";
			}

			context.m_bcl++;
		}


		context.m_bcl = oldBcl;

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			for (size_t i = 0; i < 0x3; i++) {
				auto* loc = utils::Aligned<uint32_t>(context.m_bcl);

				auto hash = *reinterpret_cast<uint32_t*>(loc + i * 4);
				auto* str = hashutils::ExtractPtr(hash);

				if (str) {
					context.WritePadding(out) << "#" << std::dec << i << ": " << str << " (var_" << std::hex << hash << ")\n";
				}
			}
		}
		else if (!(objctx.m_vmInfo->flags & tool::gsc::opcode::VmFlags::VMF_HASH64)) {
			for (size_t i = 0; i < 0x10; i++) {
				auto hash = *reinterpret_cast<uint32_t*>(context.m_bcl + i);
				auto* str = hash ? hashutils::ExtractPtr(hash) : nullptr;

				if (str) {
					context.WritePadding(out) << "#0x" << std::hex << i << ": " << str << " (var_" << std::hex << hash << ")\n";
				}
			}
		}

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			for (size_t i = 0; i < 0x3; i++) {
				auto* loc = utils::Aligned<uint64_t>(context.m_bcl);

				auto hash = *reinterpret_cast<uint64_t*>(loc + i * 8);
				auto* str = hashutils::ExtractPtr(hash);

				if (str) {
					context.WritePadding(out) << "#" << std::dec << i << ": #\"" << str << "\" (hash_" << std::hex << hash << ")\n";
				}
			}
		}
		else {
			for (size_t i = 0; i < 0x18; i++) {
				auto hash = *reinterpret_cast<uint64_t*>(context.m_bcl + i);
				auto* str = hash ? hashutils::ExtractPtr(hash) : nullptr;

				if (str) {
					context.WritePadding(out) << "#0x" << std::hex << i << ": " << str << " (hash_" << std::hex << hash << ")\n";
				}
			}
		}
		if (context.m_opt.m_dcomp) {
			ASMContextNodeMultOp* op = new ASMContextNodeMultOp(m_id == OPCODE_Undefined ? "InvalidOpCode" : m_name, false);
			op->AddParam(new ASMContextNodeValue<uint16_t>(value, TYPE_VALUE, true, false, true));

			// add params
			while (context.m_stack.size() && context.PeekASMCNode()->m_type != TYPE_PRECODEPOS) {
				op->AddParam(context.PopASMCNode());
			}
			op->m_priority = PRIORITY_INST;
			context.PushASMCNode(op);
			context.CompleteStatement(false);

			ASMContextNodeMultOp* op2 = new ASMContextNodeMultOp("Unknown operator ", false, TYPE_COMMENT);
			op2->AddParam(new ASMContextNodeValue<uint16_t>(value, TYPE_VALUE, true, false, true));
			op2->AddParam(new ASMContextNodeValue<const char*>(context.m_objctx.m_vmInfo->codeName, TYPE_VALUE));
			op2->AddParam(new ASMContextNodeValue<const char*>(PlatformName(context.m_platform), TYPE_VALUE));
			context.PushASMCNode(op2);
			context.CompleteStatement();
			//throw std::runtime_error(std::format("Unknown operator 0x{:x} ({}/{})", value, context.m_objctx.m_vmInfo->codeName, PlatformName(context.m_platform)));
		}

		
		return -2;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_error = std::format("Unknown operator: {:x}", value);
		return -1;
	}
};

class OPCodeInfoUnknownDev : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& loc = context.Aligned<uint16_t>();

		uint16_t val = context.Read<uint16_t>(loc);

		loc += 2;
		out << std::hex << val << "\n";
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint16_t>() += 2;
		return 0;
	}
};

class OPCodeInfoRegisterVariable : public OPCodeInfo {
public:
	OPCodeInfoRegisterVariable() : OPCodeInfo(OPCode::OPCODE_IW_RegisterVariable, "RegisterVariable") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name = context.Read<uint64_t>(context.m_bcl);
		context.m_bcl += 8;

		if (!context.m_localvars.size()) {
			// the local variables starts at 1
			context.m_localvars.insert(context.m_localvars.begin(), { hash::HashT89Scr("<error>"), 0 });
		}

		context.m_localvars.insert(context.m_localvars.begin(), { name, 0 });
		out << hashutils::ExtractTmp("var", name) << " (-" << std::dec << context.m_localvars.size() << ")" << std::endl;
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl += 8; // skip hash
		return 0;
	}
};

class OPCodeInfoRegisterMultipleVariables : public OPCodeInfo {
public:
	OPCodeInfoRegisterMultipleVariables() : OPCodeInfo(OPCode::OPCODE_IW_RegisterMultipleVariables, "RegisterMultipleVariables") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte count = context.Read<byte>(context.m_bcl++);

		out << "count: " << std::dec << (int)count << "\n";

		for (size_t i = 0; i < count; i++) {
			uint64_t name = context.Read<uint64_t>(context.m_bcl);
			context.m_bcl += 8;

			if (!context.m_localvars.size()) {
				// the local variables starts at 1
				context.m_localvars.insert(context.m_localvars.begin(), { hash::HashT89Scr("<error>"), 0 });
			}

			context.m_localvars.insert(context.m_localvars.begin(), { name, 0 });
			context.WritePadding(out) << hashutils::ExtractTmp("var", name) << " (-" << std::dec << context.m_localvars.size() << ")" << std::endl;
		}
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl += 8; // skip hash
		return 0;
	}
};

class OPCodeInfoSafeCreateLocalVariables : public OPCodeInfo {
public:
	OPCodeInfoSafeCreateLocalVariables() : OPCodeInfo(OPCODE_SafeCreateLocalVariables, "SafeCreateLocalVariables") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte count = context.Read<byte>(context.m_bcl++);

		context.m_localvars.reserve((size_t)count + 1);

		out << std::hex << "count: 0x" << (int)count << "\n";
		if (!context.m_localvars.size()) {
			// the local variables starts at 1
			context.m_localvars.insert(context.m_localvars.begin(), { hash::HashT89Scr("<error>"), 0 });
		}

		bool lastVa{};
		for (size_t i = 0; i < count; i++) {
			context.WritePadding(out);
			uint64_t varName;

			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_VAR_ID)) {
				const char* varNameStr{ utils::va("local_%d", context.Read<byte>(context.m_bcl++)) };
				varName = hash::Hash64(varNameStr);
				hashutils::AddPrecomputed(varName, varNameStr);
			}
			else if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_HASH64)) {
				if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
					context.Aligned<uint64_t>();
				}
				varName = context.Read<uint64_t>(context.m_bcl);
				context.m_bcl += 8;
			}
			else {
				if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
					context.Aligned<uint32_t>();
				}
				varName = context.Read<uint32_t>(context.m_bcl);
				context.m_bcl += 4;
			}
			auto& bytecode = context.m_bcl;

			byte flags;
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_NO_PARAM_FLAGS)) {
				flags = 0;
			}
			else {
				flags = *(bytecode++);
			}

			byte flagsGen{ flags };

			if (lastVa) {
				out << "(va count)";
				flagsGen |= T8GSCLocalVarFlag::IW_VARIADIC_COUNT;
				lastVa = false;
			}

			// the variables are in reversed order
			context.m_localvars.insert(context.m_localvars.begin(), { varName, flagsGen });

			out << hashutils::ExtractTmp("var", varName);

			if (flags & T8GSCLocalVarFlag::VARIADIC) {
				out << "...";
				if (context.m_gscReader.HasFlag(tool::gsc::GOHF_VAR_VA_COUNT)) {
					lastVa = true;
				}
			}
			else if (flags & T8GSCLocalVarFlag::ARRAY_REF) {
				out << "&";
			}
			else if (context.m_vm != tool::gsc::opcode::VMI_T8 && (flags & tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF)) {
				out << "*";
			}

			if (flags) {
				out << ", pflags: 0x" << std::hex << (int)flags;
			}


			byte mask = ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF);

			if (context.m_vm != tool::gsc::opcode::VMI_T8) {
				mask &= ~tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF;
			}

			if (flags & mask) {
				out << " unk flags: " << std::hex << (int)flags;
			}

			if (context.m_runDecompiler && context.m_opt.m_show_func_vars && i >= context.m_exp.GetParamCount()) {
				context.m_lastOpCodeBase = 0;
				context.PushASMCNode(new ASMContextNodeIdentifier(varName, "var"));
				context.CompleteStatement();
			}

			out << " (-" << std::dec << context.m_localvars.size() << ")" << std::endl;
		}
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		byte count = *ctx.m_bcl++;

		size_t varDelta = 0;

		if (!(ctx.m_vminfo->flags & VmFlags::VMF_HASH64)) {
			varDelta = 8;
		}
		else {
			varDelta = 4;
		}
		if (!(ctx.m_vminfo->flags & VmFlags::VMF_NO_PARAM_FLAGS)) {
			varDelta++;
		}


		for (size_t i = 0; i < count; i++) {
			if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
				if (ctx.m_vminfo->flags & VmFlags::VMF_HASH64) {
					ctx.Aligned<uint64_t>();
				}
				else {
					ctx.Aligned<uint32_t>();
				}
			}
			// skip name
			ctx.m_bcl += varDelta;
		}
		return 0;
	}
};
class OPCodeInfoCheckClearParams : public OPCodeInfo {
public:
	OPCodeInfoCheckClearParams() : OPCodeInfo(OPCODE_CheckClearParams, "CheckClearParams") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		// don't create statement, we can ignore it
		context.m_lastOpCodeBase = -1;
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetObjectType : public OPCodeInfo {
public:
	OPCodeInfoGetObjectType(OPCode id, const char* name, const char* hashType) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<uint32_t>();

		auto name = context.Read<uint32_t>(ref);

		ref += 4;

		out << hashutils::ExtractTmp("class", name) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<const char*>("<emptypos_getobjecttype>", TYPE_PRECODEPOS));
			context.PushASMCNode(new ASMContextNodeNew(name));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetNeg : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<Type>();
		}
		auto& bytecode = context.m_bcl;

		int64_t negv = -(int64_t)context.Read<Type>(bytecode);

		bytecode += sizeof(Type);

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<int64_t>(negv, TYPE_VALUE, false, true, true));
		}

		out << std::dec << negv << " (0x" << std::hex << -negv << ")" << std::endl;

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<Type>();
		}
		ctx.m_bcl += sizeof(Type);
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstant : public OPCodeInfo {
	Type m_value;
	bool m_canBeCastToBool;
	bool m_isIntConst;
	ASMContextNodeType m_nodeType;
public:
	OPCodeInfoGetConstant(OPCode id, const char* name, Type value, bool canBeCastToBool = false, bool isIntConst = false, ASMContextNodeType nodeType = TYPE_VALUE)
		: OPCodeInfo(id, name), m_value(value), m_canBeCastToBool(canBeCastToBool), m_isIntConst(isIntConst), m_nodeType(nodeType) {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<Type>(m_value, m_nodeType, false, m_canBeCastToBool, m_isIntConst));
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCastBool : public OPCodeInfo {
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (context.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS) {
				context.PushASMCNode(context.PopASMCNode());
			}
			else {
				context.PushASMCNode(ASMCNodeConvertToBool(context.PopASMCNode()));
			}
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstantRef : public OPCodeInfo {
	Type m_value;
	ASMContextNodeType m_type;
public:
	OPCodeInfoGetConstantRef(OPCode id, const char* name, Type value, ASMContextNodeType type = TYPE_VALUE)
		: OPCodeInfo(id, name), m_value(value), m_type(type) {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeValue<Type>(m_value, m_type));
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type>
class OPCodeInfoGetConstantSet : public OPCodeInfo {
	Type m_value;
	bool m_inst;
	ASMContextNodeType m_nodeType;
public:
	OPCodeInfoGetConstantSet(OPCode id, const char* name, Type value, bool inst = false, ASMContextNodeType nodeType = TYPE_VALUE)
		: OPCodeInfo(id, name), m_value(value), m_inst(inst), m_nodeType(nodeType) {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(left, new ASMContextNodeValue<Type>(m_value, m_nodeType)));
			if (m_inst) {
				context.CompleteStatement();
			}
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetObjectSize : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* left = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(left, new ASMContextNodeIdentifier(hash::HashT89Scr("size"), "var"), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoVectorScale : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {

			auto* vector = context.PopASMCNode();
			auto* factor = context.PopASMCNode();

			bool patched{};
			// can we simply patch the vector value?
			// in bo4 vectorscale((0, 0, 1), 24) is actually (0, 0, 24) to reduce the space
			if (vector->m_type == TYPE_VECTOR && factor->IsConstNumber()) {
				auto* vec = dynamic_cast<ASMContextNodeVector*>(vector);

				if (vec->m_x && vec->m_x->IsConstNumber()
					&& vec->m_y && vec->m_y->IsConstNumber()
					&& vec->m_z && vec->m_z->IsConstNumber()) {

					FLOAT factorVal = GetFloatNumber(factor);

					PatchNumber(vec->m_x, GetFloatNumber(vec->m_x) * factorVal);
					PatchNumber(vec->m_y, GetFloatNumber(vec->m_y) * factorVal);
					PatchNumber(vec->m_z, GetFloatNumber(vec->m_z) * factorVal);
					
					patched = true;
					context.PushASMCNode(vec);
					delete factor;
				}
			}

			if (!patched) {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("vectorscale", false);

				node->AddParam(vector);
				node->AddParam(factor);

				context.PushASMCNode(node);
			}
		}

		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

template<typename Type, typename WriteType = int64_t>
class OPCodeInfoGetNumber : public OPCodeInfo {
	ASMContextNodeType m_valtype;
public:
	OPCodeInfoGetNumber(OPCode id, const char* name, ASMContextNodeType type = TYPE_VALUE) : OPCodeInfo(id, name), m_valtype(type) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<Type>();
		}
		auto animTree{ objctx.m_animTreeLocations.find((uint32_t)(context.m_bcl - context.m_gscReader.Ptr())) };

		auto& bytecode = context.m_bcl;
		if (animTree != objctx.m_animTreeLocations.end()) {
			const char* str = objctx.GetStringValue(animTree->second);

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeString((str ? str : "<error>"), "#", true));
			}

			out << "anim tree #" << (str ? str : "<error>") << std::endl;
		}
		else {
			WriteType intValue = context.Read<Type>(bytecode);


			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeValue<WriteType>(intValue, m_valtype, false, true, true));
			}
			out << std::dec << intValue;
			
			if constexpr (std::is_integral<Type>()) {
				out << " (0x" << std::hex << intValue;

				const char* hashV{ hashutils::ExtractPtr(intValue) };

				if (hashV) {
					out << "/$" << hashV;
				}

				out << ")";
			}
			
			out << std::endl;
		}

		bytecode += sizeof(Type);

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<Type>();
		}
		ctx.m_bcl += sizeof(Type);
		return 0;
	}
};

class OPCodeInfoDevBlockCall : public OPCodeInfo {
public:
	OPCodeInfoDevBlockCall() : OPCodeInfo(OPCODE_IW_DevBlock, "DevBlockCall") {}
	
	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<int16_t>();
		}
		auto& bytecode = context.m_bcl;

		int16_t delta = context.Read<int16_t>(bytecode);

		bytecode += sizeof(delta);

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("DevBlockCall", false);
			node->AddParam(new ASMContextNodeValue<int16_t>(delta, TYPE_VALUE));
			context.PushASMCNode(node);
			context.CompleteStatement();
		}


		out << "delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<int16_t>();
		}
		ctx.m_bcl += sizeof(int16_t);
		return 0;
	}
};

class OPCodeInfoGetHash : public OPCodeInfo {
private:
	const char* m_type;
	bool m_hash64;
public:
	OPCodeInfoGetHash(OPCode id, const char* name, const char* type, bool hash64 = true) : m_type(type), m_hash64(hash64), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			if (m_hash64) {
				context.Aligned<uint64_t>();
			}
			else {
				context.Aligned<uint32_t>();
			}
		}
		auto& bytecode = context.m_bcl;

		uint64_t hash;
		
		if (m_hash64) {
			hash = context.Read<uint64_t>(bytecode);
			bytecode += 8;
		}
		else {
			hash = context.Read<uint32_t>(bytecode);
			bytecode += 4;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeHash(hash, false, m_type));
		}

		// dump dvars into stderr
		//if (OPCodeInfo::m_id == OPCODE_IW_GetDVarHash) {
		//	LOG_ERROR("dvar_{:x}", hash);
		//}

		out << m_type << "\"" << hashutils::ExtractTmp("hash", hash) << "\" (" << m_type << std::hex << hash << ")" << std::endl;

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<uint64_t>();
		}
		ctx.m_bcl += 8;
		return 0;
	}
};

class OPCodeInfoJump : public OPCodeInfo {
public:
	bool jump32;
public:
	OPCodeInfoJump(OPCode id, const char* name, bool jump32 = false) : jump32(jump32),  OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// get the jump opcode location
		int32_t m_jumpLocation = context.FunctionRelativeLocation(context.m_bcl - ((objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) ? 2 : 1));

		ASMContextNode* node = nullptr;
		ASMContextNodeType type = TYPE_JUMP;
		const char* name = nullptr;
		bool pushBack = false;
		if (context.m_runDecompiler) {
			switch (m_id) {
			case OPCODE_JumpOnGreaterThan:
			{
				// reversed
				ASMContextNode* op2 = context.PopASMCNode();
				ASMContextNode* op1 = context.PopASMCNode();
				node = new ASMContextNodeOp2(">", PRIORITY_COMPARE, op1, op2, true);
				type = TYPE_JUMP_GREATERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnLessThan:
			{
				// reversed
				ASMContextNode* op2 = context.PopASMCNode();
				ASMContextNode* op1 = context.PopASMCNode();
				node = new ASMContextNodeOp2("<", PRIORITY_COMPARE, op1, op2, true);
				type = TYPE_JUMP_LOWERTHAN;
				name = "jumpcmp";
			}
			break;
			case OPCODE_JumpOnDefined:
				node = context.PopASMCNode();
				type = TYPE_JUMP_ONDEFINED;
				name = "jumpifdefined";
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
			case OPCODE_IW_Jump32:
				name = "goto";
				break;
			default:
				name = "jumpukn";
				assert(0);
				break;
			}
		}

		assert(!context.m_runDecompiler || name != nullptr);

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			if (jump32) {
				context.Aligned<int32_t>();
			}
			else {
				context.Aligned<int16_t>();
			}
		}

		auto& bytecode = context.m_bcl;

		int32_t delta;
		
		if (jump32) {
			delta = context.Read<int32_t>(bytecode);

			bytecode += 4;
		}
		else {
			delta = context.Read<int16_t>(bytecode);

			bytecode += 2;
		}

		// push a location and mark it as referenced
		byte* newLoc = &context.m_bcl[delta];

		if (newLoc - context.m_gscReader.Ptr() > context.m_gscReader.GetFileSize() || newLoc < context.m_gscReader.Ptr()) {
			throw std::runtime_error(std::format("jump with invalid delta: {}0x{:x}", (delta < 0 ? "-" : ""), (delta < 0 ? -delta : delta)));
			return -1;
		}

		// push the node value during the jump if the operator is asking for it
		auto& locref = context.PushLocation(newLoc);

		uint16_t pointedOpCode{};
		if (context.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16)) {
			pointedOpCode = *reinterpret_cast<uint16_t*>(utils::Aligned<uint16_t>(newLoc));
		}
		else {
			pointedOpCode = *newLoc;
		}

		auto pointedOpCodeVal = context.LookupOpCode(pointedOpCode);

		bool returnCandidate = pointedOpCodeVal && pointedOpCodeVal->m_id == tool::gsc::opcode::OPCODE_End;

		if (delta != 0 || m_id == OPCODE_DevblockBegin) {
			if (context.m_runDecompiler) {
				bool inject = true;
				if ((m_id == OPCODE_Jump || m_id == OPCODE_IW_Jump32) && delta > 0) {
					// might be ternary
					//ASMContextNodeTernary
					if (context.m_stack.size() && context.m_funcBlock.m_statements.size()) {
						// at least one node in the stack and a previous statement
						auto& last = context.m_funcBlock.m_statements[context.m_funcBlock.m_statements.size() - 1];
						if (last.node->m_type != TYPE_JUMP && IsJumpType(last.node->m_type)) {
							auto* jumpNode = static_cast<ASMContextNodeJumpOperator*>(last.node);
							// is this operator pointing just after us?
							byte* curr;

							if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16)) {
								curr = utils::Aligned<int16_t>(context.m_bcl);
							}
							else {
								curr = context.m_bcl;
							}

							if (jumpNode->m_delta > 0 && jumpNode->m_location == context.FunctionRelativeLocation(curr)) {
								auto* top = context.PopASMCNode();
								if (top->m_type != TYPE_PRECODEPOS) {
									// bad top, no data
									//node = new ASMContextNodeValue<ASMContextNode*>(top, TYPE_JUMP_STACK_TOP);
									// we can remove the jump node, we won't need it
									//LOG_TRACE("set jump last loc {:x} top was {}", last.location->rloc, *top);
									context.m_lastOpCodeBase = last.location->rloc;
									context.m_funcBlock.m_statements.pop_back();
									// unlike in bo, the ternary operator in mw seems to use both JumpOnFalse and JumpOnTrue, so we have to look at the operator
									// I decided to avoid using ! so the result is prettier (imo)

									ASMContextNode* ifpart = jumpNode->m_operand;
									if (!(context.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
										ifpart = ASMCNodeConvertToBool(ifpart);
									}
									locref.m_lateop.emplace_back(new ASMContextLocationOpCompleteTernary(ifpart, top, !IsJumpConditionForceReversed(jumpNode)));
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
					auto* jumpNode = new ASMContextNodeJumpOperator(name, node, locref.rloc, type, m_jumpLocation, true, delta);
					jumpNode->m_returnCandidate = returnCandidate;
					context.PushASMCNode(jumpNode);
					context.CompleteStatement();
				}
			}

			if (!returnCandidate) {
				locref.refs.insert(m_jumpLocation);
			}
		}
		else {
			if (context.m_runDecompiler) {
				if (!node) {
					// add empty jump to redirect the jump
					context.PushASMCNode(new ASMContextNodeValue<const char*>("<emptypos_jump>", TYPE_PRECODEPOS));
					context.CompleteStatement();
				}
				else {
					// empty if
					// context.PushASMCNode(new ASMContextNodeJumpOperator(name, node, locref.rloc, type, m_jumpLocation, true, delta));
					if (!(context.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
						node = ASMCNodeConvertToBool(node);
					}
					context.PushASMCNode(new ASMContextNodeIfElse(node, new ASMContextNodeBlock(), nullptr));
					context.CompleteStatement();
				}
			}
		}

		if (m_id == OPCODE_DevblockBegin) {
			context.m_devBlocks.emplace_back(m_jumpLocation, delta);
			context.m_objctx.m_devblocks.insert(context.ScriptAbsoluteLocation(context.m_fonctionStart + m_jumpLocation));
		}

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
		//return m_id == OPCODE_Jump ? -2 : 0; // no code after jump
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		int32_t m_jumpLocation = ctx.FunctionRelativeLocation(ctx.m_bcl - 2);
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16)) {
			if (jump32) {
				ctx.Aligned<int32_t>();
			}
			else {
				ctx.Aligned<int16_t>();
			}
		}
		auto& bytecode = ctx.m_bcl;

		int32_t delta;
		if (jump32) {
			delta = *(int32_t*)bytecode;

			bytecode += 4;
		}
		else {
			delta = *(int16_t*)bytecode;

			bytecode += 2;
		}

		ctx.PushLocation(&ctx.m_bcl[delta]);

		return 0;
	}
};

class OPCodeInfoJumpExpr : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t m_jumpLocation = context.FunctionRelativeLocation(context.m_bcl - ((objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) ? 2 : 1));
		// get the jump opcode location

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<int16_t>();
		}
		auto& bytecode = context.m_bcl;

		int16_t delta = context.Read<int16_t>(bytecode);

		bytecode += 2;

		// push a location and mark it as referenced
		byte* newLoc = &context.m_bcl[delta];

		if (newLoc - context.m_gscReader.Ptr() > context.m_gscReader.GetFileSize() || newLoc < context.m_gscReader.Ptr()) {
			out << "INVALID JUMP, TOO FAR: delta:" << std::hex << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";
			return -1;
		}

		// push the node value during the jump if the operator is asking for it
		auto& locref = context.PushLocation(newLoc);

		if (context.m_runDecompiler) {
			ASMContextNodePriority priority = PRIORITY_VALUE;
			const char* desc = "<err>";
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
				case OPCODE_JumpOnDefinedExpr:
					priority = PRIORITY_COALESCE;
					desc = "??";
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

		out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		int32_t m_jumpLocation = ctx.FunctionRelativeLocation(ctx.m_bcl - 2);
		// get the jump opcode location

		auto& bytecode = ctx.Aligned<int16_t>();

		int16_t delta = *(int16_t*)bytecode;

		bytecode += 2;
		
		ctx.PushLocation(&ctx.m_bcl[delta]);
		return 0;
	}
};

class OPCodeInfoJumpPush : public OPCodeInfo {
public:
	OPCodeInfoJumpPush() : OPCodeInfo(OPCODE_JumpPush, "JumpPush") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& bytecode = context.Aligned<uintptr_t>();

		uintptr_t location = context.Read<uintptr_t>(bytecode);

		bytecode += 8;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeJumpOperator("jumppush", nullptr, location, TYPE_STATEMENT, jumpLocation));
			context.CompleteStatement();
		}

		out << "Jump 0x" << std::hex << location << "\n";

		return -2; // no code after that
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uintptr_t>() += 8;
		return -2;
	}
};

class OPCodeInfoVector : public OPCodeInfo {
public:
	OPCodeInfoVector() : OPCodeInfo(OPCODE_Vector, "Vector") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* x = context.PopASMCNode();
			auto* y = context.PopASMCNode();
			auto* z = context.PopASMCNode();
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}

		out << "\n";
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoGetVector : public OPCodeInfo {
public:
	OPCodeInfoGetVector() : OPCodeInfo(OPCODE_GetVector, "GetVector") {}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<float>();
		}
		auto& bytecode = context.m_bcl;

		float x = context.Read<float>(bytecode);
		bytecode += 4;

		float y = context.Read<float>(bytecode);
		bytecode += 4;

		float z = context.Read<float>(bytecode);
		bytecode += 4;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}
		out << "(" << x << ", " << y << ", " << z << ")\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<FLOAT>();
		}
		ctx.m_bcl += 12;
		return 0;
	}
};

class OPCodeInfoVectorConstant : public OPCodeInfo {
public:
	OPCodeInfoVectorConstant() : OPCodeInfo(OPCODE_VectorConstant, "VectorConstant") {}

	bool OfFlag(byte loc, float& out) const {
		switch (loc & 0x3) {
		case 0: 
			out = 0;
			return true;
		case 1: 
			out = -1;
			return true;
		case 2: 
			out = 1;
			return true;
		}
		out = 999;
		return false;
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte flag = context.Read<byte>(context.m_bcl++);

		float x, y, z;

		if (!OfFlag(flag >> 4, x)) out << "(invalid x)";
		if (!OfFlag(flag >> 2, y)) out << "(invalid y)";
		if (!OfFlag(flag, z)) out << "(invalid z)";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeVector(x, y, z));
		}

		out << "(" << x << ", " << y << ", " << z << ")\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoName : public OPCodeInfo {
private:
	const char* m_hashType;
public:
	OPCodeInfoName(OPCode id, const char* name, const char* hashType) : OPCodeInfo(id, name), m_hashType(hashType) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<uint32_t>();

		auto name = context.Read<uint32_t>(ref);

		ref += 4;

		out << hashutils::ExtractTmp(m_hashType, name) << std::endl;

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoSetVariableFieldRef : public OPCodeInfo {
public:
	OPCodeInfoSetVariableFieldRef(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<uint32_t>();

		auto name = context.Read<uint32_t>(ref);

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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoSetVariableFieldCached : public OPCodeInfo {
public:
	OPCodeInfoSetVariableFieldCached(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = context.Read<byte>(context.m_bcl++);

		uint64_t name;
		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << " ";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
		}

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			auto* node = new ASMContextNodeLeftRightOperator(
				new ASMContextNodeIdentifier(name), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET);
			context.SetFieldIdASMCNode(node->Clone());
			context.PushASMCNode(node);
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return -1;
	}
};

class OPCodeInfoSetWaittillVariableFieldCached : public OPCodeInfo {
	const char* m_valueName;
public:
	OPCodeInfoSetWaittillVariableFieldCached(OPCode id, const char* name, const char* valueName = nullptr) : OPCodeInfo(id, name), m_valueName(valueName) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name;
		if (!m_valueName) {
			int lvar = context.Read<byte>(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << " ";
			}
			else {
				name = context.m_localvars[lvar].name;
				context.m_localvars_ref[name]++;
			}

			out << hashutils::ExtractTmp("var", name);
		}
		else {
			name = hash::HashT89Scr("_");
		}
		out << std::endl;

		if (context.m_runDecompiler) {
			ASMContextNode* prev = context.PeekASMCNode();

			if (prev->m_type != TYPE_WAITTILL) {
				prev = context.PopASMCNode();
				ASMContextNode* node;
				if (prev->m_type == TYPE_WAITTILL_SET) {
					auto* lro = reinterpret_cast<ASMContextNodeLeftRightOperator*>(prev);
					lro->m_left = new ASMContextNodeLeftRightOperator(lro->m_left, new ASMContextNodeIdentifier(name), ", ", PRIORITY_SET, TYPE_WAITTILL_SET);
					node = lro;
				}
				else {
					node = new ASMContextNodeLeftRightOperator(
						new ASMContextNodeIdentifier(name), prev, " = ", PRIORITY_SET, TYPE_WAITTILL_SET);
				}

				context.SetFieldIdASMCNode(node->Clone());
				context.PushASMCNode(node);
			}
			else {
				ASMContextNodeMultOp* wtop = reinterpret_cast<ASMContextNodeMultOp*>(prev);
				ASMContextNode* idf = new ASMContextNodeIdentifier(name);
				wtop->AddParam(idf);
				context.SetFieldIdASMCNode(idf->Clone());
			}

		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return -1;
	}
};

class OPCodeInfoSetVariableField : public OPCodeInfo {
public:
	OPCodeInfoSetVariableField(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9SetVariableFieldFromEvalArrayRef : public OPCodeInfo {
public:
	OPCodeT9SetVariableFieldFromEvalArrayRef() : OPCodeInfo(OPCODE_T9_SetVariableFieldFromEvalArrayRef, "SetVariableFieldFromEvalArrayRef") {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9Iterator : public OPCodeInfo {
public:
	const char* m_op;
	ASMContextNodeType m_ittype;
	OPCodeT9Iterator(OPCode id, const char* name, const char* op, ASMContextNodeType ittype) : OPCodeInfo(id, name), m_op(op), m_ittype(ittype) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			ASMContextNode* iterator = context.PopASMCNode();

			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, false, m_ittype);

			node->AddParam(iterator);

			context.PushASMCNode(node);
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeT9GetVarRef : public OPCodeInfo {
public:
	OPCodeT9GetVarRef() : OPCodeInfo(OPCODE_T9_GetVarRef, "GetVarRef") {}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeRef("&", context.GetFieldIdASMCNode()->Clone()));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoEvalSelfFieldVariable : public OPCodeInfo {
public:
	OPCodeInfoEvalSelfFieldVariable(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& ref = context.Aligned<uint32_t>();

		auto name = context.Read<uint32_t>(ref);

		ref += 4;

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeValue<const char*>("self", TYPE_SELF));
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoClearFieldVariable : public OPCodeInfo {
private:
	bool m_stack;
	bool m_isref;
public:
	OPCodeInfoClearFieldVariable(OPCode id, const char* name, bool stack, bool isref) : OPCodeInfo(id, name), m_stack(stack), m_isref(isref) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name = 0;
		if (!m_stack) {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<uint32_t>();
			}
			auto& ref = context.m_bcl;

			if (m_isref) {
				int lvar = context.Read<byte>(ref++);

				if (lvar >= context.m_localvars.size()) {
					name = hash::HashT89Scr("<error>");
					out << "bad lvar stack: 0x" << std::hex << (int)lvar << " ";
				}
				else {
					name = context.m_localvars[lvar].name;
					context.m_localvars_ref[name]++;
				}
			}
			else {
				if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
					name = context.Read<uint64_t>(ref);
					ref += 8;
				}
				else {
					name = context.Read<uint32_t>(ref);
					ref += 4;
				}
			}


			out << hashutils::ExtractTmp("var", name) << std::endl;
		}
		else {
			out << "\n";
		}

		if (context.m_runDecompiler) {
			ASMContextNode* nameNode = m_stack ? context.PopASMCNode() : new ASMContextNodeIdentifier(name);
			ASMContextNode* fieldAccessNode = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), nameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(fieldAccessNode, new ASMContextNodeValue<const char*>("undefined", TYPE_GET_UNDEFINED), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<uint32_t>();
		}
		auto& ref = ctx.m_bcl;

		if (!m_stack) {
			if (ctx.m_vminfo->flags & VmFlags::VMF_HASH64) {
				ref += 8;
			}
			else {
				ref += 4;
			}
		}
		return 0;
	}
};

class OPCodeInfoSetGlobalObjectFieldVariable : public OPCodeInfo {
private:
	const char* gvarName;
public:
	OPCodeInfoSetGlobalObjectFieldVariable(OPCode id, const char* name, const char* gvarName = nullptr) : OPCodeInfo(id, name), gvarName(gvarName) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name;
		if (!gvarName) {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<uint16_t>();
			}
			auto& base = context.m_bcl;

			uint16_t objectid = context.Read<uint16_t>(base);
			base += 2;

			name = objctx.GetGlobalVarName(objectid);

			if (name) {
				out << hashutils::ExtractTmp("var", name) << std::flush;
			}
			else {
				out << "bad objectid stack: 0x" << std::hex << objectid;
			}
		}
		else {
			hashutils::Add(gvarName);
			name = hash::Hash64(gvarName);
			out << gvarName;
		}

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<uint32_t>();
		}
		auto& base2 = context.m_bcl;
		uint64_t fieldName;

		if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
			fieldName = context.Read<uint64_t>(base2);
			base2 += 8;
		}
		else {
			fieldName = context.Read<uint32_t>(base2);
			base2 += 4;
		}

		out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
			auto* ref = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(fieldName), ".", PRIORITY_ACCESS, TYPE_ACCESS);

			context.PushASMCNode(new ASMContextNodeLeftRightOperator(ref, context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint16_t>() += 2;
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};
class OPCodeInfoEvalGlobalObjectFieldVariable : public OPCodeInfo {
private:
	const char* gvarName;
	bool push;
public:
	OPCodeInfoEvalGlobalObjectFieldVariable(OPCode id, const char* name, const char* gvarName = nullptr, bool push = true) : OPCodeInfo(id, name), gvarName(gvarName), push(push) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name;
		if (!gvarName) {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<uint16_t>();
			}
			auto& base = context.m_bcl;

			uint16_t objectid = context.Read<uint16_t>(base);
			base += 2;

			name = objctx.GetGlobalVarName(objectid);

			if (name) {
				out << hashutils::ExtractTmp("var", name) << std::flush;
			}
			else {
				out << "bad objectid stack: 0x" << std::hex << objectid;
			}
		}
		else {
			hashutils::Add(gvarName);
			name = hash::Hash64(gvarName);
			out << gvarName;
		}

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<uint32_t>();
		}
		auto& base2 = context.m_bcl;
		uint64_t fieldName;

		if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
			fieldName = context.Read<uint64_t>(base2);
			base2 += 8;
		}
		else {
			fieldName = context.Read<uint32_t>(base2);
			base2 += 4;
		}
		
		out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
			auto* node = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(fieldName), ".", PRIORITY_ACCESS, TYPE_ACCESS);
			if (push) {
				context.PushASMCNode(node);
			}
			else {
				context.SetFieldIdASMCNode(node);
			}
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint16_t>() += 2;
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoCastAndEvalFieldVariable : public OPCodeInfo {
public:
	OPCodeInfoCastAndEvalFieldVariable(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<uint32_t>();
		}
		auto& base2 = context.m_bcl;
		uint64_t name;

		if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
			name = context.Read<uint64_t>(base2);
			base2 += 8;
		}
		else {
			name = context.Read<uint32_t>(base2);
			base2 += 4;
		}

		out << hashutils::ExtractTmp("var", name) << std::endl;

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), new ASMContextNodeIdentifier(name), ".", PRIORITY_ACCESS, TYPE_ACCESS));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoEvalFieldVariable : public OPCodeInfo {
private:
	bool m_stack;
	bool m_ref;
public:
	OPCodeInfoEvalFieldVariable(OPCode id, const char* name, bool stack, bool ref) : OPCodeInfo(id, name), m_stack(stack), m_ref(ref) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name = 0;

		if (!m_stack) {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<uint32_t>();
			}
			auto& ref = context.m_bcl;

			if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
				name = context.Read<uint64_t>(ref);
				ref += 8;
			}
			else {
				name = context.Read<uint32_t>(ref);
				ref += 4;
			}

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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (!m_stack) {
			ctx.Aligned<uint32_t>() += 4;
		}
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableCached : public OPCodeInfo {
	int count;
	int forceId;
public:
	OPCodeInfoEvalLocalVariableCached(OPCode id, const char* name, int count = 1, int forceId = -1) : OPCodeInfo(id, name), count(count), forceId(forceId) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		for (size_t i = 0; i < count; i++) {
			if (i) {
				context.WritePadding(out);
			}
			int lvar;
			
			if (forceId < 0) {
				lvar = (int)context.Read<byte>(context.m_bcl++);
			}
			else {
				lvar = forceId;
			}
			out << "(-" << std::dec << lvar << ") ";

			uint64_t name;
			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl += count;
		return 0;
	}
};

class OPCodeInfoSetLocalVariableCached : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoSetLocalVariableCached(OPCode id, const char* name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};
class OPCodeInfoClearLocalVariableCached : public OPCodeInfo {
public:
	OPCodeInfoClearLocalVariableCached(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(new ASMContextNodeIdentifier(name), new ASMContextNodeValue<const char*>("undefined", TYPE_GET_UNDEFINED), " = ", PRIORITY_SET, TYPE_SET));
			context.CompleteStatement();
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoFirstArrayKey : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoFirstArrayKey(OPCode id, const char* name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t name;
		if (!m_stack) {
			int lvar = (int)context.Read<byte>(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (!m_stack) {
			ctx.m_bcl++;
		}
		return 0;
	}
};

class OPCodeInfoSetNextArrayKeyCached : public OPCodeInfo {
	bool m_set;
public:
	OPCodeInfoSetNextArrayKeyCached(OPCode id, const char* name, bool set = true) : OPCodeInfo(id, name), m_set(set) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t name{};
		if (m_set) {
			int lvar = (int)context.Read<byte>(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar;
			}
			else {
				name = context.m_localvars[lvar].name;
				context.m_localvars_ref[name]++;
				out << hashutils::ExtractTmp("var", name);
			}
		}

		out << std::endl;

		if (context.m_runDecompiler) {
			ASMContextNode* arrayNode = context.PopASMCNode();
			ASMContextNode* thingNode = context.PopASMCNode();
			if (m_set) {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("nextarray", false, TYPE_ARRAY_NEXTKEY);

				node->AddParam(arrayNode);
				node->AddParam(thingNode);
				node->AddParam(new ASMContextNodeIdentifier(name));

				context.PushASMCNode(node);
				context.CompleteStatement();
			}
			else {
				auto* node = new ASMContextNodeCallFuncPtr(FUNCTION_CALL, 0, TYPE_ARRAY_NEXTKEYPUSH);

				node->AddParam(new ASMContextNodeFuncRef("", hash::HashT89Scr("nextarray"), 0));
				node->AddParam(arrayNode);
				node->AddParam(thingNode);

				context.PushASMCNode(node);
			}

		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalFieldObjectFromRef : public OPCodeInfo {
public:
	OPCodeInfoEvalFieldObjectFromRef(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t name;
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableRefCached : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalVariableRefCached(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t name;
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalLocalVariableDefined : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalVariableDefined(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t name;
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFunctionOperator("isdefined", nullptr, new ASMContextNodeIdentifier(name), TYPE_FUNC_IS_DEFINED, true));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalLocalArrayCached : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalArrayCached() : OPCodeInfo(OPCODE_IW_EvalLocalArrayCached, "EvalLocalArrayCached") {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		uint64_t name;
		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
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

		if (context.m_runDecompiler) {
			ASMContextNode* arrayNode = context.PopASMCNode();
			ASMContextNode* accessNode = new ASMContextNodeArrayAccess(arrayNode, context.PopASMCNode());
			context.PushASMCNode(accessNode);
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};
class OPCodeInfoEvalLocalObjectCached : public OPCodeInfo {
public:
	OPCodeInfoEvalLocalObjectCached() : OPCodeInfo(OPCODE_IW_EvalLocalVariableObjectCached, "EvalLocalVariableObjectCached") {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		uint64_t name;
		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name);
		}
		out << "\n";

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoEvalArray : public OPCodeInfo {
private:
	bool m_stack;
public:
	OPCodeInfoEvalArray(OPCode id, const char* name, bool stack) : OPCodeInfo(id, name), m_stack(stack) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCreateArray : public OPCodeInfo {
public:
	OPCodeInfoCreateArray(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeArrayBuild());
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoAddToArray : public OPCodeInfo {
public:
	OPCodeInfoAddToArray(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNode* key = context.PopASMCNode();
			ASMContextNode* value = context.PopASMCNode();
			ASMContextNode* arrayVal = context.PeekASMCNode();

			if (arrayVal->m_type == TYPE_ARRAY_BUILD) {
				// we are building an array, we can add the value
				static_cast<ASMContextNodeArrayBuild*>(arrayVal)->AddValue(key, value, (objctx.m_vmInfo->flags & VmFlags::VMF_INV_ADD_TO_OBJECT));
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCreateStruct : public OPCodeInfo {
public:
	OPCodeInfoCreateStruct(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeStructBuild());
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoAddToStruct : public OPCodeInfo {
private:
	bool m_popKey;
public:
	OPCodeInfoAddToStruct(OPCode id, const char* name, bool popKey) : m_popKey(popKey), OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		uint64_t key{};
		if (!m_popKey) {
			key = context.Read<uint64_t>(context.m_bcl);
			context.m_bcl += 8;
			out << hashutils::ExtractTmp("hash", key);
		}

		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNode* keyNode = m_popKey ? context.PopASMCNode() : new ASMContextNodeHash(key);
			ASMContextNode* value = context.PopASMCNode();
			ASMContextNode* structVal = context.PeekASMCNode();

			if (structVal->m_type == TYPE_STRUCT_BUILD) {
				// we are building an array, we can add the value
				static_cast<ASMContextNodeStructBuild*>(structVal)->AddValue(keyNode, value, (objctx.m_vmInfo->flags & VmFlags::VMF_INV_ADD_TO_OBJECT));
			}
			else {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("addtostruct", false);

				node->AddParam(context.PopASMCNode());
				node->AddParam(keyNode);
				node->AddParam(value);

				context.PushASMCNode(node);
			}
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCastFieldObject : public OPCodeInfo {
public:
	OPCodeInfoCastFieldObject(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.SetObjectIdASMCNode(context.PopASMCNode());
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoPreScriptCall : public OPCodeInfo {
public:
	OPCodeInfoPreScriptCall(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeValue<const char*>("<emptypos_prescriptcall>", TYPE_PRECODEPOS));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoEnd : public OPCodeInfo {
public:
	bool m_isReturn;
	OPCodeInfoEnd(OPCode id, const char* name, bool isReturn) : OPCodeInfo(id, name), m_isReturn(isReturn) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			if (m_isReturn) {
				auto* ref = new ASMContextNodeOp1("return ", true, context.PopASMCNode());
				// convert it to statement
				ref->m_priority = PRIORITY_INST;
				ref->m_type = TYPE_RETURN;
				context.PushASMCNode(ref);
			} else {
				// special node to print end ref
				auto* ref = new ASMContextNodeValue<const char*>("<END>", TYPE_END);
				context.PushASMCNode(ref);
			}
			context.CompleteStatement();
		}
		out << "\n";
		int64_t rloc = context.FunctionRelativeLocation();
		for (const auto& loc : context.m_locs) {
			if (loc.second.rloc > rloc) {
				// not the end, we can continue
				return 0;
			}
		}

		return -2;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		int64_t rloc = ctx.FunctionRelativeLocation();
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
	OPCodeInfoClearArray(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* fieldId = context.GetFieldIdASMCNode();
			auto* key = context.PopASMCNode();

			ASMContextNode* accessNode = new ASMContextNodeArrayAccess(fieldId, key);
			context.PushASMCNode(new ASMContextNodeLeftRightOperator(accessNode, new ASMContextNodeValue<const char*>("undefined", TYPE_GET_UNDEFINED), " = ", PRIORITY_SET, TYPE_SET));

			context.CompleteStatement();
		}
		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoStatement : public OPCodeInfo {
	const char* m_operatorName;
public:
	OPCodeInfoStatement(OPCode id, const char* name, const char* operatorName) : OPCodeInfo(id, name),
		m_operatorName(operatorName) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* ref = new ASMContextNodeValue<const char*>(m_operatorName, TYPE_STATEMENT);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};
class OPCodeInfoGscBinStrToken : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t token{ context.Read<int32_t>(context.m_bcl) };
		context.m_bcl += 4;

		out << "0x" << std::hex << token;
		if (context.m_runDecompiler) {
			auto* ref = new ASMContextNodeValue<int32_t>(token, TYPE_VALUE, true);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};
class OPCodeInfoSkip : public OPCodeInfo {
	const char* m_operatorName;
public:
	OPCodeInfoSkip(OPCode id, const char* name, const char* operatorName) : OPCodeInfo(id, name),
		m_operatorName(operatorName) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			auto* ref = new ASMContextNodeValue<const char*>(m_operatorName, TYPE_STATEMENT);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
		out << "\n";
		LOG_ERROR("Can't decompile skip operator {}", m_operatorName);
		return -2;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return -2;
	}
};

class OPCodeInfoDevOp : public OPCodeInfo {
	bool m_push;
public:
	OPCodeInfoDevOp(bool push) : OPCodeInfo(OPCODE_DevOp, "DevOp"), m_push(push) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("DevOp", false);

			node->AddParam(new ASMContextNodeValue<uint16_t>(value, TYPE_STATEMENT, true));
			// convert it to statement
			context.PushASMCNode(node);
			if (!m_push) {
				context.CompleteStatement();
			}
		}
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoInvalidOpCode : public OPCodeInfo {
public:
	OPCodeInfoInvalidOpCode() : OPCodeInfo(OPCODE_InvalidOpCode, "InvalidOpCode") {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("InvalidOpCode", false);

			node->AddParam(new ASMContextNodeValue<uint16_t>(value, TYPE_VALUE, true));
			// convert it to statement
			context.PushASMCNode(node);
			context.CompleteStatement();
		}
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};
class OPCodeInfoDevConsume : public OPCodeInfo {
	size_t len;
	bool push;
public:
	OPCodeInfoDevConsume(OPCode id, const char* name, size_t len, bool push) : len(len), push(push), OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "data(" << std::dec << len << "):";
		std::vector<ASMContextNode*> numbers{};
		for (size_t i = 0; i < len; i++) {
			int data = (int)context.Read<byte>(context.m_bcl++);
			if (context.m_runDecompiler) {
				numbers.push_back(new ASMContextNodeValue<int>(data, TYPE_VALUE, true));
			}
			out << " " << std::hex << std::setfill('0') << std::setw(2) << data;
		}
		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_name, false);

			for (ASMContextNode* v : numbers) {
				node->AddParam(v);
			}

			// convert it to statement
			context.PushASMCNode(node);
			if (!push) {
				context.CompleteStatement();
			}
				
		}
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoFuncHash : public OPCodeInfo {
	bool m_push;
	const char* m_desc;
public:
	OPCodeInfoFuncHash(OPCode id, const char* name, bool push, const char* desc) : OPCodeInfo(id, name), m_push(push), m_desc(desc) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& bytecode = context.Aligned<uint64_t>();

		uint64_t hash = context.Read<uint64_t>(bytecode);

		bytecode += 8;

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_desc, false);

			node->AddParam(new ASMContextNodeHash(hash, false, "#"));
			// convert it to statement
			node->m_priority = PRIORITY_INST;
			node->m_type = TYPE_STATEMENT;
			context.PushASMCNode(node);
			if (!m_push) {
				context.CompleteStatement();
			}
		}

		out << "#\"" << hashutils::ExtractTmp("hash", hash) << "\" (#" << std::hex << hash << ")" << std::endl;
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint64_t>() += 8; // name
		return 0;
	}
};

class OPCodeT9EvalFieldVariableFromObject : public OPCodeInfo {
	bool stack;
public:
	OPCodeT9EvalFieldVariableFromObject(OPCode id, const char* name, bool stack) : OPCodeInfo(id, name), stack(stack) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		auto& bytecode = context.Aligned<uint32_t>();

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}

		uint32_t field = context.Read<uint32_t>(bytecode);
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
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		ctx.Aligned<uint32_t>() += 4; // field
		return 0;
	}
};

class OPCodeT9EvalArrayCached : public OPCodeInfo {
private:
	bool m_push;
public:
	OPCodeT9EvalArrayCached(OPCode id, const char* name, bool push) : m_push(push), OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::endl;
		}

		if (context.m_runDecompiler) {
			auto* node = new ASMContextNodeArrayAccess(new ASMContextNodeIdentifier(name), context.PopASMCNode());
			if (m_push) {
				context.PushASMCNode(node);
			}
			else {
				context.SetFieldIdASMCNode(node);
			}
		}
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		return 0;
	}
};

class OPCodeT9SetFieldVariableFromObjectFromRef : public OPCodeInfo {
public:
	OPCodeT9SetFieldVariableFromObjectFromRef() : OPCodeInfo(OPCODE_T9_SetFieldVariableFromObjectFromRef, "SetFieldVariableFromObjectFromRef") {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		auto& bytecode = context.Aligned<uint32_t>();

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
			out << "bad lvar stack: 0x" << std::hex << (int)lvar;
		}
		else {
			name = context.m_localvars[lvar].name;
			context.m_localvars_ref[name]++;
			out << hashutils::ExtractTmp("var", name) << std::flush;
		}

		uint64_t field = context.Read<uint32_t>(bytecode);
		bytecode += 4;

		out << "." << hashutils::ExtractTmp("var", field) << std::endl;

		if (context.m_runDecompiler) {
			auto* left = new ASMContextNodeIdentifier(name);
			auto* right = new ASMContextNodeIdentifier(field);

			auto* objectNode = new ASMContextNodeLeftRightOperator(left, right, ".", PRIORITY_ACCESS, TYPE_ACCESS);
			auto* node = new ASMContextNodeLeftRightOperator(objectNode,
				context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET);
			context.SetFieldIdASMCNode(node->Clone());
			context.PushASMCNode(node);
			context.CompleteStatement();
		}
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // lvar
		ctx.Aligned<uint32_t>() += 4; // field
		return 0;
	}
};

class OPCodeInfoFunctionOperator : public OPCodeInfo {
	const char* m_operatorName;
	bool m_hasSelf;
	bool m_complete;
	bool m_canHaveArg2;
	bool m_isBool;
	bool m_noParenthesis;
	
public:
	OPCodeInfoFunctionOperator(OPCode id, const char* name, const char* operatorName, bool hasSelf = false, bool complete = true, bool canHaveArg2 = false, bool isBool = false, bool noParenthesis = false) : OPCodeInfo(id, name),
		m_operatorName(operatorName), m_hasSelf(hasSelf), m_complete(complete), m_canHaveArg2(canHaveArg2), m_isBool(isBool), m_noParenthesis(noParenthesis) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_operatorName, m_hasSelf, TYPE_STATEMENT, m_isBool, m_noParenthesis);
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
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoNotify : public OPCodeInfo {
public:
	OPCodeInfoNotify(OPCode id, const char* name) : OPCodeInfo(id, name){
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoIWNotify : public OPCodeInfo {
public:
	OPCodeInfoIWNotify(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp("notify", true);
			// self
			node->AddParam(context.PopASMCNode());

			int guess{};
			while (context.m_stack.size() && context.PeekASMCNode()->m_type != TYPE_PRECODEPOS) {
				node->AddParam(context.PopASMCNode());
				guess++;
			}
			out << "guessParam: " << guess;

			if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
				// clear PreScriptCall
				delete context.PopASMCNode();
			}

			context.PushASMCNode(node);
			context.CompleteStatement();
		}
		out << "\n";
		return 0;
	}
	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoFuncCall : public OPCodeInfo {
private:
	size_t m_delta;
	bool m_hasParam;
public:
	OPCodeInfoFuncCall(OPCode id, const char* name, size_t delta, bool hasParam) : m_delta(delta), m_hasParam(hasParam), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int params;
		if (!objctx.m_vmInfo->HasFlag(VmFlags::VMF_CALL_NO_PARAMS) || (objctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_CALLS) && m_hasParam)) {
			params = context.Read<byte>(context.m_bcl++);
		}
		else {
			switch (m_id) {
			case OPCODE_IW_CallBuiltinFunction0:
			case OPCODE_IW_CallBuiltinMethod0:
				params = 0;
				break;
			case OPCODE_IW_CallBuiltinFunction1:
			case OPCODE_IW_CallBuiltinMethod1:
				params = 1;
				break;
			case OPCODE_IW_CallBuiltinFunction2:
			case OPCODE_IW_CallBuiltinMethod2:
				params = 2;
				break;
			case OPCODE_IW_CallBuiltinFunction3:
			case OPCODE_IW_CallBuiltinMethod3:
				params = 3;
				break;
			case OPCODE_IW_CallBuiltinFunction4:
			case OPCODE_IW_CallBuiltinMethod4:
				params = 4;
				break;
			case OPCODE_IW_CallBuiltinFunction5:
			case OPCODE_IW_CallBuiltinMethod5:
				params = 5;
				break;
			default:
				params = -1;
				break;
			}
		}

		uint64_t tmpBuffData[2];
		uint64_t* data = tmpBuffData;
		size_t flags{};

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			auto& bytecode = context.Aligned<uint64_t>();

			tmpBuffData[0] = context.Read<uint32_t>(bytecode);
			tmpBuffData[1] = context.Read<uint32_t>(bytecode + 4);

			bytecode += 8;
		}
		else {
			auto idx = context.Read<uint16_t>(context.m_bcl);
			context.m_bcl += m_delta;

			if (idx >= objctx.m_linkedImports.size()) {
				tmpBuffData[0] = hash::HashT89Scr("<error>");
				tmpBuffData[1] = hash::HashT89Scr("<error>");
			}
			else {
				auto& imp = objctx.m_linkedImports[idx];

				tmpBuffData[0] = imp.name;
				tmpBuffData[1] = imp.name_space;
				flags = imp.flags;
			}
		}

		if (params >= 0) {
			out << std::dec << "params: " << params << " ";
		}

		uint64_t nsp = 0;
		if (data[1]
			&& data[1] != 0x222276a9 && data[1] != 0xc1243180 // "sys" or ""
			&& data[1] != context.m_namespace) { // same namespace call
			nsp = data[1];
			out << hashutils::ExtractTmpPath("namespace", nsp) << "::" << std::flush;
		}

		out << hashutils::ExtractTmp("function", data[0]);

		if (context.m_runDecompiler) {
			byte flags = 0;

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
			case OPCODE_IW_CallBuiltinMethod0:
			case OPCODE_IW_CallBuiltinMethod1:
			case OPCODE_IW_CallBuiltinMethod2:
			case OPCODE_IW_CallBuiltinMethod3:
			case OPCODE_IW_CallBuiltinMethod4:
			case OPCODE_IW_CallBuiltinMethod5:
				flags |= SELF_CALL;
				break;
			case OPCODE_ScriptFunctionCall:
			case OPCODE_CallBuiltinFunction:
			case OPCODE_IW_CallBuiltinFunction0:
			case OPCODE_IW_CallBuiltinFunction1:
			case OPCODE_IW_CallBuiltinFunction2:
			case OPCODE_IW_CallBuiltinFunction3:
			case OPCODE_IW_CallBuiltinFunction4:
			case OPCODE_IW_CallBuiltinFunction5:
				break; // ignored
			default:
				LOG_ERROR("bad func call {}", (int)m_id);
				return -1;
			}

			// classes-calls are by definition without self

			auto* ptr = new ASMContextNodeCallFuncPtr(FUNCTION_CALL, flags);

			// this

			if (flags & SELF_CALL) {
				ptr->AddParam(context.PopASMCNode());
			}

			ptr->AddParam(new ASMContextNodeFuncRef("", data[0], nsp));

			if (params >= 0) {
				for (size_t i = 0; i < params; i++) {
					ptr->AddParam(context.PopASMCNode());
				}

				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
			}
			else {
				int guess{};
				while (context.m_stack.size() && context.PeekASMCNode()->m_type != TYPE_PRECODEPOS) {
					ptr->AddParam(context.PopASMCNode());
					guess++;
				}

				out << ", guessParam: " << guess;
				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
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
		if (flags) {
			out << " iflags: 0x" << std::hex << flags << " (ift:0x" << (flags & 0xF) << ")";
		}
		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // params
		ctx.Aligned<uint64_t>() += 8; // import
		return 0;
	}
};

class OPCodeInfoFuncGet : public OPCodeInfo {
private:
	size_t m_delta;
public:
	OPCodeInfoFuncGet(OPCode id, const char* name, size_t delta) : m_delta(delta), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t tmpBuffData[2];
		uint64_t* data = tmpBuffData;
		size_t flags{};

		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			auto& bytecode = context.Aligned<uint64_t>();

			tmpBuffData[0] = context.Read<uint32_t>(bytecode);
			tmpBuffData[1] = context.Read<uint32_t>(bytecode + 4);

			bytecode += 8;
		}
		else {
			size_t idx = context.Read<uint16_t>(context.m_bcl);

			// (iw)
			// builtins: 16 bits
			// script functions: 32 bits
			
			context.m_bcl += m_delta;

			if (idx >= objctx.m_linkedImports.size()) {
				tmpBuffData[0] = hash::HashT89Scr("<error>");
				tmpBuffData[1] = hash::HashT89Scr("<error>");
			}
			else {
				auto& imp = objctx.m_linkedImports[idx];
				
				tmpBuffData[0] = imp.name;
				tmpBuffData[1] = imp.name_space;
				flags = imp.flags;
			}
		}

		out << "&";

		uint64_t nsp = 0;

		if (data[1] 
			&& data[1] != 0x222276a9 && data[1] != 0xc1243180 // "sys" or ""
			&& data[1] != context.m_namespace) { // current namespace
			out << hashutils::ExtractTmpPath("namespace", data[1]) << std::flush << "::";
			nsp = data[1];
		}
		out << hashutils::ExtractTmp("function", data[0]);

		if (flags) {
			out << " iflags: 0x" << std::hex << flags << " (ift:0x" << (flags & 0xF) << ")";
		}

		out << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFuncRef("&", data[0], nsp));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint64_t>() += 8; // import
		return 0;
	}
};

class OPCodeInfoFuncClassCall : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte params = context.Read<byte>(context.m_bcl++);
		auto& bytecode = context.Aligned<uint32_t>();

		uint32_t function = *(uint32_t*)bytecode;

		bytecode += 4;

		out << std::dec << "params: " << (int)params << ", function: " << std::hex << hashutils::ExtractTmp("function", function) << std::endl;

		if (context.m_runDecompiler) {
			auto* caller = context.PopASMCNode();

			byte flags = 0;

			if (m_id == OPCODE_ClassFunctionThreadCall) {
				flags |= THREAD_CALL;
			}
			else if (m_id == OPCODE_ClassFunctionThreadCallEndOn) {
				flags |= CHILDTHREAD_CALL;
			}

			if (!flags && caller->m_type == TYPE_NEW && (function == objctx.m_vmInfo->HashField("__constructor"))) {
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++; // params
		ctx.Aligned<uint32_t>() += 4; // name
		return 0;
	}
};

class OPCodeInfoFuncCallPtr : public OPCodeInfo {
private:
	bool m_hasParam;
public:
	OPCodeInfoFuncCallPtr(OPCode id, const char* name, bool hasParam) : m_hasParam(hasParam), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int params;
		if ((objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) || (m_hasParam && objctx.m_vmInfo->HasFlag(VMF_IW_CALLS))) {
			params = context.Read<byte>(context.m_bcl++);
			out << std::dec << "params:" << (int)params << ":" << std::hex << context.FunctionRelativeLocation();
		}
		else {
			params = -1;
		}


		if (context.m_runDecompiler) {
			byte flags = 0;

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
			case OPCODE_IW_BuiltinFunctionCallPointer:
				flags |= BUILTIN_CALL;
				break;
			case OPCODE_IW_BuiltinMethodCallPointer:
				flags |= SELF_CALL | BUILTIN_CALL;
				break;
			default:
				LOG_ERROR("bad func ptr call {}", (int)m_id);
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

			if (params >= 0) {
				for (size_t i = 0; i < params; i++) {
					ptr->AddParam(context.PopASMCNode());
				}

				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
			}
			else {
				// use <= to add the function pointer
				int guess{};
				while (context.m_stack.size() && context.PeekASMCNode()->m_type != TYPE_PRECODEPOS) {
					ptr->AddParam(context.PopASMCNode());
					guess++;
				}
				out << ", guessParam: " << guess;

				if (context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
					// clear PreScriptCall
					delete context.PopASMCNode();
				}
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
		out << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoGetString : public OPCodeInfo {
	bool m_istr;
public:
	OPCodeInfoGetString(OPCode id, const char* name, bool istr) : OPCodeInfo(id, name), m_istr(istr) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<int32_t>();
		}
		auto& base = context.m_bcl;

		uint32_t ref = context.Read<uint32_t>(base);

		uint32_t floc = context.ScriptAbsoluteLocation(base);

		const char* str = objctx.GetStringValueByLoc(floc);
		if (!str) {
			// build string
			str = objctx.GetStringError(floc, nullptr);
		}

		base += 4;

		if (str) {
			utils::PrintFormattedString(out << "\"", str) << "\"\n";
			if (context.m_runDecompiler) {
				if (m_istr) {
					context.PushASMCNode(new ASMContextNodeString(str, "%", true));
				}
				else {
					context.PushASMCNode(new ASMContextNodeString(str));
				}
			}
		}
		else {
			// probably only dev blocks
			out << "bad str stack: 0x" << std::hex << ref << "\n";
			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeString((context.m_opt.m_formatter->flags & tool::gsc::formatter::FormatterFlags::FFL_NOERROR_STR) ? "" : "<unknown string>", m_istr ? "%" : nullptr));
			}
		}


		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<int32_t>();
		}
		ctx.m_bcl += 4;
		return 0;
	}
};

class OPCodeInfoGetAnimation : public OPCodeInfo {
private:
	bool m_doubleAnim;
public:
	OPCodeInfoGetAnimation(OPCode id, const char* name, bool doubleAnim) : m_doubleAnim(doubleAnim), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<int64_t>();
		}
		auto& base = context.m_bcl;

		uint32_t ref1;
		const char* str1;
		const char* str2;
		uint32_t ref2;
		if (m_doubleAnim) {
			ref1 = context.Read<uint32_t>(base);
			str1 = objctx.GetStringValueOrError(ref1, context.ScriptAbsoluteLocation(base), nullptr);

			base += 4;

			ref2 = context.Read<uint32_t>(base);
			str2 = objctx.GetStringValueOrError(ref2, context.ScriptAbsoluteLocation(base), nullptr);

			base += 4;
		}
		else {
			str1 = "";
			ref1 = 0;
			ref2 = context.Read<byte>(base);
			str2 = objctx.GetStringValueOrError(ref2, context.ScriptAbsoluteLocation(base), nullptr);
			base++;
		}

		if (!str1) {
			out << "bad str stack: 0x" << std::hex << ref1 << " ";
			str1 = "<unknown>";
		}
		if (!str2) {
			out << "bad str stack: 0x" << std::hex << ref2 << " ";
			str2 = "<unknown>";
		}
		utils::PrintFormattedString(out, str1);
		out << "%";
		utils::PrintFormattedString(out, str2);
		out << "\n";

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeAnimation(str1, str2));
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<int32_t>();
		}
		ctx.m_bcl += 4;
		return 0;
	}
};

enum OPCodeInfoGetGlobalGetType {
	GGGT_PUSH = 0,
	GGGT_FIELD,
	GGGT_GLOBAL,
};

class OPCodeInfoGetGlobal : public OPCodeInfo {
private:
	const char* m_gvarName;
	OPCodeInfoGetGlobalGetType m_ref;
public:
	OPCodeInfoGetGlobal(OPCode id, const char* name, OPCodeInfoGetGlobalGetType ref, const char* gvarName) : m_ref(ref), m_gvarName(gvarName), OPCodeInfo(id, name) {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		uint64_t name;
		if (!m_gvarName) {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<uint16_t>();
			}
			auto& base = context.m_bcl;

			uint16_t objectid = context.Read<uint16_t>(base);
			base += 2;

			name = objctx.GetGlobalVarName(objectid);

			if (name) {
				out << hashutils::ExtractTmp("var", name) << std::flush;
			}
			else {
				out << "bad objectid stack: 0x" << std::hex << objectid;
			}
		}
		else {
			hashutils::Add(m_gvarName);
			name = hash::Hash64(m_gvarName);
			out << m_gvarName;
		}
		out << "\n";

		if (context.m_runDecompiler) {
			switch (m_ref) {
			case OPCodeInfoGetGlobalGetType::GGGT_GLOBAL:
				context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
				break;
			case OPCodeInfoGetGlobalGetType::GGGT_FIELD:
				context.SetFieldIdASMCNode(new ASMContextNodeIdentifier(name));
				break;
			case OPCodeInfoGetGlobalGetType::GGGT_PUSH:
				context.PushASMCNode(new ASMContextNodeIdentifier(name));
				break;
			default:
				assert(false && "invalid push");
				break;
			}
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		if (ctx.m_vminfo->HasFlag(VmFlags::VMF_ALIGN)) {
			ctx.Aligned<uint16_t>();
		}
		ctx.m_bcl += 2;
		return 0;
	}
};

class OPCodeT9EvalFieldVariableFromGlobalObject : public OPCodeInfo {
public:
	OPCodeT9EvalFieldVariableFromGlobalObject() : OPCodeInfo(OPCODE_T9_EvalFieldVariableFromGlobalObject, "EvalFieldVariableFromGlobalObject") {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& base = context.Aligned<uint16_t>();

		uint16_t ref = context.Read<uint16_t>(base);

		uint64_t name = objctx.GetGlobalVarName(ref);

		base += 2;

		auto& baseField = context.Aligned<uint32_t>();

		uint32_t field = context.Read<uint32_t>(baseField);

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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint16_t>() += 2;
		ctx.Aligned<uint32_t>() += 4;
		return 0;
	}
};

class OPCodeInfoGetLocalVar : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)context.Read<byte>(context.m_bcl++);

		if (lvar >= context.m_localvars.size()) {
			out << "bad lvar stack: 0x" << std::hex << (int)lvar << "\n";
		}
		else {
			uint64_t name = context.m_localvars[lvar].name;
			out << hashutils::ExtractTmp("var", name) << std::endl;
			context.m_localvars_ref[name]++;
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoSwitch : public OPCodeInfo {
public:
	OPCodeInfoSwitch() : OPCodeInfo(OPCODE_Switch, "Switch") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		// size:  8+16*cases,
		// align: 4 + 4 + (8 + 8)*cases,
		// logic uint32 deltaSwitchTable;uint32 cases;(uint64 value;uint64 delta)[cases]
		// move to switch table using deltaSwitchTable;pop value;search case using case; if found move using delta

		auto& baseTable = context.Aligned<int32_t>();
		int32_t table = context.Read<int32_t>(baseTable);
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = context.Aligned<int32_t>();

		int32_t cases = (*(int32_t*)baseCases) & 0x7FFFFFFF;

		baseCases += 4;

		out << "table: ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
			<< context.FunctionRelativeLocation(utils::Aligned<int64_t>(baseTable)) << " cases: " << cases << "\n";

		ASMContextNodeSwitchPreCompute* node = nullptr;
		if (context.m_runDecompiler) {
			node = new ASMContextNodeSwitchPreCompute(context.PopASMCNode());
		}

		for (size_t c = 1; c <= cases; c++) {
			uint64_t caseValue;
			byte* caseLoc;
			int64_t caseDelta;
			byte* endBase;
			if (context.m_vm >= VMI_T834) {
				auto& baseCaseValue = context.Aligned<int64_t>();

				context.WritePadding(out);

				caseValue = context.Read<uint64_t>(baseCaseValue);
				caseLoc = baseCaseValue;
				baseCaseValue += 8;
				auto& baseCaseDelta = context.Aligned<int64_t>();
				caseDelta = context.Read<int64_t>(baseCaseDelta);
				baseCaseDelta += 8;
				endBase = baseCaseDelta;
			}
			else {
				auto& baseCaseValue = context.Aligned<int32_t>();

				context.WritePadding(out);

				caseValue = context.Read<uint32_t>(baseCaseValue);
				caseLoc = baseCaseValue;
				baseCaseValue += 4;
				auto& baseCaseDelta = context.Aligned<int32_t>();
				caseDelta = context.Read<int32_t>(baseCaseDelta);
				baseCaseDelta += 4;
				endBase = baseCaseDelta;
			}

			auto caseRLoc = context.PushLocation(&endBase[caseDelta]).rloc;

			if (c == cases && (!caseValue || caseValue == 0xdefdefdefdefdef0)) {
				out << "default";
				if (node) {
					node->m_cases.push_back({ nullptr, caseRLoc });
				}
			}
			else {
				out << "case ";
				// bo3 string decomp
				uint32_t floc = (uint32_t)(caseLoc - context.m_gscReader.Ptr());
				const char* cv = objctx.GetStringValueByLoc(floc);
				if (cv) {
					utils::PrintFormattedString(out << "\"", cv) << "\"";
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeString(cv), caseRLoc });
					}
				}
				else if (caseValue >= 0x100000000LL || (context.m_vm == VMI_T831 && caseValue >= 0x10000LL)) {
					// assume it's an hash after int32_t max value, for vm31, only 32bits hashes are used, so the bar is lower
					out << "#\"" << hashutils::ExtractTmp("hash", caseValue) << "\"" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(caseValue, false, "#"), caseRLoc });
					}
				}
				else {
					out << std::dec << caseValue;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeValue<int64_t>(caseValue, TYPE_VALUE), caseRLoc });
					}
				}
				out << "(0x" << std::hex << floc << ")";
			}

			out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
				<< caseRLoc << "\n";

			if (c == cases) {
				if (node) {
					node->m_endLocation = context.FunctionRelativeLocation(utils::Aligned<uint16_t>(context.m_bcl));
				}

				if (!caseValue) {
					context.m_bcl += caseDelta;
				}
			}
			else {
				// align to the next opcode
				context.Aligned<uint16_t>();
			}
		}

		if (node) {
			if (!node->m_endLocation) {
				// wtf? no cases???
				node->m_endLocation = context.FunctionRelativeLocation(utils::Aligned<uint16_t>(context.m_bcl));
			}
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		context.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		auto& baseTable = ctx.Aligned<int32_t>();
		int32_t table = *(int32_t*)baseTable;
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = ctx.Aligned<int32_t>();

		int32_t cases = *(int32_t*)baseCases;

		baseCases += 4;

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = ctx.Aligned<int64_t>();
			int64_t caseValue = *(int64_t*)baseCaseValue;
			baseCaseValue += 8;
			auto& baseCaseDelta = ctx.Aligned<int64_t>();
			int64_t caseDelta = *(int64_t*)baseCaseDelta;
			baseCaseDelta += 8;

			auto caseRLoc = ctx.PushLocation(&baseCaseDelta[caseDelta]).rloc;

			if (c == cases) {
				if (!caseValue) {
					baseCaseDelta += caseDelta;
				}
			}
			else {
				// align to the next opcode
				ctx.Aligned<uint16_t>();
			}
		}

		ctx.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}
};

class OPCodeInfoGetPositionRef : public OPCodeInfo {
public:
	OPCodeInfoGetPositionRef() : OPCodeInfo(OPCODE_IW_GetPositionRef, "GetPositionRef") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t opcodeLocation = context.FunctionRelativeLocation(context.m_bcl - ((objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) ? 2 : 1));
		if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
			context.Aligned<int16_t>();
		}
		auto& bytecode = context.m_bcl;

		int16_t delta = *(int16_t*)bytecode;

		bytecode += 2;

		// push a location and mark it as referenced
		byte* refLocation = &context.m_bcl[delta];

		out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
			<< context.FunctionRelativeLocation(refLocation) << "\n";

		tool::gsc::opcode::asmcontextlocation& loc = context.PushLocation(refLocation);

		// add ref so this code can be handled
		loc.refs.insert(opcodeLocation);

		if (context.m_runDecompiler) {
			// push location
			context.PushASMCNode(new ASMContextNodeCodeRef(loc.rloc));
		}


		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		throw std::runtime_error("not impl");
	}
};

class OPCodeInfoIWSwitch : public OPCodeInfo {
public:
	OPCodeInfoIWSwitch() : OPCodeInfo(OPCODE_IW_Switch, "Switch") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		auto& baseTable = context.m_bcl;
		int32_t table = context.Read<int32_t>(baseTable);
		// we move to the table
		baseTable += table + 4;

		if (!context.IsInsideScript()) {
			throw std::runtime_error(std::format("Invalid switch table: {:x}", table));
			return -1;
		}

		uint16_t cases = context.Read<uint16_t>(baseTable) & 0x7FFFFFFF;

		baseTable += 2;

		byte* baseLocation = baseTable;

		out << "table: ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
			<< context.FunctionRelativeLocation(baseTable) << " cases: " << cases << "\n";

		ASMContextNodeSwitchPreCompute* node = nullptr;
		if (context.m_runDecompiler) {
			node = new ASMContextNodeSwitchPreCompute(context.PopASMCNode());
		}

		for (size_t c = 1; c <= cases; c++) {
			auto* basecase = baseLocation + 12 * (c - 1);
			context.m_bcl = basecase + 12;
			union CaseValue {
				int64_t integer;
				uint32_t str;
				uint64_t hash;
				uint32_t unkb; // i32
			};
			
			CaseValue val = context.Read<CaseValue>(basecase);
			int32_t caseDelta = ((context.Read<int32_t>(basecase + 8) << 8) >> 8) + 8; // remove type
			byte type = context.Read<byte>(basecase + 11);

			context.WritePadding(out);

			auto caseRLoc = context.PushLocation(&basecase[caseDelta]).rloc;

			if (c == cases && (!val.hash || val.hash == 0xdefdefdefdefdef0 || val.hash == 0xDDEFDEFFDEFFDEFF)) {
				out << "default";
				if (node) {
					node->m_cases.push_back({ nullptr, caseRLoc });
				}
			}
			else {
				out << "case ";
				switch (type) {
				case 1: // integer
					out << std::dec << val.integer;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeValue<int64_t>(val.integer, TYPE_VALUE), caseRLoc });
					}
					break;
				case 0xffff: {// string
					const char* str = objctx.GetStringValueOrError(val.str, context.ScriptAbsoluteLocation(basecase), nullptr);

					ASMContextNodeString* outputStr;
					if (str) {
						out << "\"";
						utils::PrintFormattedString(out, str);
						out << "\"";
						if (node) {
							outputStr = new ASMContextNodeString(str);
						}
					}
					else {
						// probably only dev blocks
						out << "bad str stack: 0x" << std::hex << val.hash;
						if (node) {
							outputStr = new ASMContextNodeString((context.m_opt.m_formatter->flags & tool::gsc::formatter::FormatterFlags::FFL_NOERROR_STR) ? "" : "<unknown string>");
						}
					}
					if (node) {
						node->m_cases.push_back({ outputStr, caseRLoc });
					}
				}
					break;
				case 2:
				case 4: // hash
					out << "#\"" << hashutils::ExtractTmp("hash", val.hash) << "\"" << "(0x" << std::hex << val.hash << "/casetype:" << (int)type << ")" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(val.hash, false, "#"), caseRLoc });
					}
					break;
				case 5: // unkb
					out << "t\"" << hashutils::ExtractTmp("hash", val.unkb) << "\"" << "(0x" << std::hex << val.unkb << ")" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(val.unkb, false, "t"), caseRLoc });
					}
					break;
				case 6: // unk9
					out << "%\"" << hashutils::ExtractTmp("hash", val.hash) << "\"" << "(0x" << std::hex << val.hash << ")" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(val.hash, false, "%"), caseRLoc });
					}
					break;
				case 7: // dvarhash
					out << "@\"" << hashutils::ExtractTmp("hash", val.hash) << "\"" << "(0x" << std::hex << val.hash << ")" << std::flush;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeHash(val.hash, false, "@"), caseRLoc });
					}
					break;
				default:
					out << std::dec << "unk " << type << ":" << val.hash;
					if (node) {
						node->m_cases.push_back({ new ASMContextNodeValue<int64_t>(val.hash, TYPE_VALUE), caseRLoc });
					}
					break;
				}
			}

			out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
				<< caseRLoc << "\n";

			if (c == cases) {
				if (node) {
					node->m_endLocation = context.FunctionRelativeLocation(context.m_bcl);
				}

				//if (!val.hash) {
				//	baseCaseDelta += caseDelta;
				//}
			}
		}

		if (node) {
			if (!node->m_endLocation) {
				// wtf? no cases???
				node->m_endLocation = context.FunctionRelativeLocation(context.m_bcl);
			}
			context.PushASMCNode(node);
			context.CompleteStatement();
		}

		context.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		auto& baseTable = ctx.Aligned<int32_t>();
		int32_t table = *(int32_t*)baseTable;
		// we move to the table
		baseTable += 4 + table;

		auto& baseCases = ctx.Aligned<int32_t>();

		int32_t cases = *(int32_t*)baseCases;

		baseCases += 4;

		for (size_t c = 1; c <= cases; c++) {
			auto& baseCaseValue = ctx.Aligned<int64_t>();
			int64_t caseValue = *(int64_t*)baseCaseValue;
			baseCaseValue += 8;
			auto& baseCaseDelta = ctx.Aligned<int64_t>();
			int64_t caseDelta = *(int64_t*)baseCaseDelta;
			baseCaseDelta += 8;

			auto caseRLoc = ctx.PushLocation(&baseCaseDelta[caseDelta]).rloc;

			if (c == cases) {
				if (!caseValue) {
					baseCaseDelta += caseDelta;
				}
			}
			else {
				// align to the next opcode
				ctx.Aligned<uint16_t>();
			}
		}

		ctx.PushLocation();
		return -2; // use pushed location to get asm from previous value
	}
};

class OPCodeInfoEndSwitch : public OPCodeInfo {
public:
	OPCodeInfoEndSwitch() : OPCodeInfo(OPCODE_EndSwitch, "EndSwitch") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& baseCount = context.Aligned<int32_t>();

		int32_t count = context.Read<int32_t>(baseCount);

		baseCount += 4;

		if (context.m_vm >= VMI_T834) {
			auto& ptrBase = context.Aligned<int64_t>();

			ptrBase += 16 * count;
		}
		else {
			auto& ptrBase = context.Aligned<int32_t>();

			ptrBase += 8 * count;
		}

		auto rloc = context.FunctionRelativeLocation();

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeJumpOperator("endswitch", nullptr, rloc, TYPE_JUMP_ENDSWITCH, jumpLocation));
			context.CompleteStatement();
		}

		out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << std::hex << rloc << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		auto& baseCount = ctx.Aligned<int32_t>();

		int32_t count = *(int32_t*)baseCount;

		baseCount += 4;

		auto& ptrBase = ctx.Aligned<int64_t>() += 16 * count;
		return 0;
	}
};
class OPCodeInfoIWEndSwitch : public OPCodeInfo {
public:
	OPCodeInfoIWEndSwitch() : OPCodeInfo(OPCODE_IW_EndSwitch, "EndSwitch") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
		auto& baseCount = context.m_bcl;

		uint16_t count = context.Read<uint16_t>(baseCount);

		baseCount += 2 + 12 * count;

		auto rloc = context.FunctionRelativeLocation();

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeJumpOperator("endswitch", nullptr, rloc, TYPE_JUMP_ENDSWITCH, jumpLocation));
			context.CompleteStatement();
		}

		out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << std::hex << rloc << "\n";

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return -1;
	}
};

class OPCodeInfopushopn : public OPCodeInfo {
public:
	INT m_n;
	const char* m_op;
	ASMContextNodePriority m_priority;
	ASMContextNodeType m_type;
	bool m_caller;
	bool m_forceFunc;
	bool m_convertBool;
	bool m_isBoolValueRet;
	OPCodeInfopushopn(OPCode id, const char* name, INT n, const char* op, ASMContextNodeType type, ASMContextNodePriority priority, bool caller = false, bool forceFunc = false, bool convertBool = false, bool isBoolValueRet = false) : OPCodeInfo(id, name),
		m_n(n), m_op(op), m_type(type), m_priority(priority), m_caller(caller), m_forceFunc(forceFunc), m_convertBool(convertBool), m_isBoolValueRet(isBoolValueRet) {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (context.m_runDecompiler) {
			bool cb = m_convertBool;
			auto popVal = [&context, cb]() {
				if (cb && !(context.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
					return ASMCNodeConvertToBool(context.PopASMCNode());
				}
				return context.PopASMCNode();
			};
			if (m_n == 1 && !m_forceFunc) {
				context.PushASMCNode(new ASMContextNodeOp1(m_op, m_caller, popVal(), m_type, m_isBoolValueRet));
			}
			else if (m_n == 2 && !m_caller && !m_forceFunc) {
				// reverse order
				ASMContextNode* op2 = popVal();
				ASMContextNode* op1 = popVal();

				context.PushASMCNode(new ASMContextNodeOp2(m_op, m_priority, op1, op2, m_isBoolValueRet, m_type));
			}
			else {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, m_caller, m_type, m_isBoolValueRet);

				for (size_t i = 0; i < m_n; i++) {
					node->AddParam(popVal());
				}

				context.PushASMCNode(node);
			}
		}
		out << "\n";
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfodec : public OPCodeInfo {
public:
	OPCodeInfodec(OPCode id, const char* name) : OPCodeInfo(id, name) {
	}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		return 0;
	}
};

class OPCodeInfoCount : public OPCodeInfo {
	const char* m_op;
	bool m_pushReturn;
	int m_addedToCount;
public:

	OPCodeInfoCount(OPCode id, const char* name, const char* op, bool pushReturn, int addedToCount = 0) : OPCodeInfo(id, name), m_addedToCount(addedToCount), m_op(op), m_pushReturn(pushReturn) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte count = context.Read<byte>(context.m_bcl++) + m_addedToCount;

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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoCountWaittill : public OPCodeInfo {
	const char* m_op;
	byte m_added;
public:

	OPCodeInfoCountWaittill(OPCode id, const char* name, const char* op, byte added = 0) : OPCodeInfo(id, name), m_op(op), m_added(added) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		byte count = context.Read<byte>(context.m_bcl++) + m_added;

		out << "count:" << (int)count << "\n";

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, true, TYPE_WAITTILL);

			// self
			node->AddParam(context.PopASMCNode());

			for (size_t i = 0; i < count; i++) {
				node->AddParam(context.PopASMCNode());
			}

			context.PushASMCNode(node);
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoSingle : public OPCodeInfo {
public:
	const char* m_op;
	bool m_pushReturn;
	unsigned int m_count;
	unsigned int m_delta;
	bool m_hasCaller;
	ASMContextNodeType m_type = TYPE_STATEMENT;
	OPCodeInfoSingle(OPCode id, const char* name, const char* op, bool pushReturn, unsigned int count = 1, unsigned int delta = 0, bool hasCaller = true, ASMContextNodeType type = TYPE_STATEMENT) 
		: OPCodeInfo(id, name), m_op(op), m_pushReturn(pushReturn), m_count(count), m_delta(delta), m_hasCaller(hasCaller), m_type(type) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		if (m_delta) {
			out << "data: ";

			for (size_t i = 0; i < m_delta; i++) {
				out << std::hex << std::setfill('0') << std::setw(2) << (int)*(context.m_bcl++) << " ";
			}
		}
		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, m_hasCaller, m_type);

			// self
			if (m_hasCaller) {
				node->AddParam(context.PopASMCNode());
			}

			// param
			for (size_t i = 0; i < m_count; i++) {
				node->AddParam(context.PopASMCNode());
			}

			context.PushASMCNode(node);

			if (!m_pushReturn) {
				context.CompleteStatement();
			}
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfoSingleFunc : public OPCodeInfo {
public:
	const char* m_op;
	bool m_pushReturn;
	bool m_isBoolVal;
	OPCodeInfoSingleFunc(OPCode id, const char* name, const char* op, bool pushReturn, bool isBoolVal = false) : OPCodeInfo(id, name), m_op(op), m_pushReturn(pushReturn), m_isBoolVal(isBoolVal) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		out << "\n";

		if (context.m_runDecompiler) {
			ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, false, TYPE_STATEMENT, m_isBoolVal);

			// param
			node->AddParam(context.PopASMCNode());

			context.PushASMCNode(node);

			if (!m_pushReturn) {
				context.CompleteStatement();
			}
		}

		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

class OPCodeInfonop : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;
};
class OPCodeInfoNopStmt : public OPCodeInfo {
public:
	using OPCodeInfo::OPCodeInfo;


	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		context.CompleteStatement();
		out << "\n";
		return 0;
	}
};

// T8-Compiler custom opcode
class OPCodeInfoT8CGetLazyFunction : public OPCodeInfo {
public:
	OPCodeInfoT8CGetLazyFunction() : OPCodeInfo(OPCODE_T8C_GetLazyFunction, "T8C_GetLazyFunction") {}

	int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int32_t lazylocation = context.FunctionRelativeLocation(context.m_bcl - ((objctx.m_vmInfo->HasFlag(VmFlags::VMF_OPCODE_U16)) ? 2 : 1));
		auto& base = context.Aligned<int32_t>();

		uint32_t nsp = context.Read<uint32_t>(base);
		uint32_t function = context.Read<uint32_t>(base + 4);
		uint64_t script = context.Read<uint64_t>(base + 8);

		base += 16;

		tool::gsc::NameLocated located{};

		located.name_space = nsp;
		located.name = function;
		located.script = script;

		context.m_objctx.m_lazyLinks[located].push_back(context.ScriptAbsoluteLocation(context.m_fonctionStart + lazylocation));

		out << "@" << hashutils::ExtractTmpPath("namespace", nsp)
			<< "<" << std::flush << hashutils::ExtractTmpScript(script)
			<< ">::" << std::flush << hashutils::ExtractTmp("function", function) << std::endl;

		if (context.m_runDecompiler) {
			context.PushASMCNode(new ASMContextNodeFuncRef("@", function, nsp, script));
		}
		return 0;
	}

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.Aligned<uint32_t>() += 16; // nsp32:func32:script64
		return 0;
	}
};

class OPCodeInfoDeltaVal : public OPCodeInfo {
public:
	const char* m_op;
	OPCodeInfoDeltaVal(OPCode id, const char* name, const char* op) : OPCodeInfo(id, name), m_op(op) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
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
	const char* m_op;
	OPCodeT9DeltaLocalVariableCached(OPCode id, const char* name, const char* op) : OPCodeInfo(id, name), m_op(op) {
	}

	int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
		int lvar = (int)*(context.m_bcl++);

		uint64_t name;

		if (lvar >= context.m_localvars.size()) {
			name = hash::HashT89Scr("<error>");
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

	int Skip(uint16_t value, ASMSkipContext& ctx) const override {
		ctx.m_bcl++;
		return 0;
	}
};

#pragma endregion
#pragma region opcode_registry

namespace {
	const OPCodeInfo* g_unknownOpcode = new OPCodeInfoUnknown(OPCODE_Undefined, "Undefined");
}

namespace tool::gsc::opcode {
	VmInfo* RegisterVM(uint64_t vm, const char* name, const char* codeName, const char* internalName, uint64_t flags) {
		auto it = g_opcodeMap.find(vm);
		if (it != g_opcodeMap.end() && it->second.vmMagic) {
			if (it->second.flags != flags) {
				LOG_WARNING("Registering twice the same vm with different flags, {:x} != {:x} for {}", it->second.flags, flags, name);
			}
			return &it->second; // assuming good name
		}
		VmInfo& nfo = g_opcodeMap[vm];
		nfo = { vm, name, codeName, internalName, flags, {} };
		g_vmMap[hash::Hash64(utils::va("%llx", vm))] = vm;
		byte revVm{ tool::gsc::opcode::MapAsOldVM(vm) };
		if (revVm) {
			g_vmMap[hash::Hash64(utils::va("%02x", revVm))] = vm;
		}
		g_vmMap[hash::Hash64(internalName)] = vm;
		return &nfo;
	}
	VmInfo* GetVm(uint64_t vm) {
		VmInfo& nfo = g_opcodeMap[vm];

		if (!nfo.vmMagic) {
			LOG_WARNING("Can't find vm {:x}", vm);
		}
		return &nfo;
	}

	void VmInfo::RegisterVmName(uint64_t hash) {
		g_vmMap[hash] = vmMagic;
	}

	void VmInfo::RegisterVMOperatorFunction(const char* name, const char* usage, OPCode opcode, int flags, int minArgs, int maxArgs) {
		uint64_t funcHash = HashField(name);

		if (opFuncs.find(funcHash) != opFuncs.end()) {
			LOG_ERROR("Operator function already defined for vm {}, func: {}", name, name);
			return;
		}

		auto& gv = opFuncs[funcHash];
		gv.name = name;
		gv.usage = usage;
		gv.opCode = opcode;
		gv.flags = flags;
		gv.minParam = minArgs;
		gv.maxParam = maxArgs;
	}

	void VmInfo::RegisterVMGlobalVariable(const char* name, OPCode getOpCode) {
		uint64_t hash = HashField(name);

		if (globalvars.find(hash) != globalvars.end()) {
			LOG_ERROR("Global variable already defined for vm {}, gvar: {}", this->name, name);
			return;
		}

		auto& gv = globalvars[hash];
		gv.name = name;
		gv.getOpCode = getOpCode;
	}

	void VmInfo::SetMaxOpCode(uint16_t maxOpCode) {
		this->maxOpCode = maxOpCode;
	}
	void VmInfo::SetOpaqueStringCount(size_t opaqueStringCount) {
		this->opaqueStringCount = opaqueStringCount;
	}
	void VmInfo::RegisterVMHashOPCode(char type, OPCode opCode, int size, std::function<uint64_t(const char*)> hashFunc) {
		if (!(size == 8 || size == 4)) {
			LOG_ERROR("Invalid size for hash vm {}: '{}' / {} bytes", name, type, size);
			return;
		}

		auto [h, ok] = hashesFunc.try_emplace(type, type, opCode, size, hashFunc);

		if (!ok) {
			LOG_ERROR("Registered existing hash into vm {}: '{}'", name, type);
			return;
		}
	}
	void VmInfo::RegisterOpCode(Platform platform, OPCode enumValue, uint16_t op) {
		opcodemap[op][platform] = enumValue;
		opcodemaplookup[enumValue][platform] = op;
		opcodemappltlookup[platform][enumValue].insert(op);
	}

	void VmInfo::RegisterSameCodePlatform(Platform main, Platform sub) {
		AddPlatform(sub);

		Platform to = main;

		while (true) {
			auto svmIt = sameVmMap.find(to);

			if (svmIt == sameVmMap.end()) {
				break;
			}

			to = svmIt->second;
		}

		if (to == sub) {
			LOG_ERROR("Trying to register cycling same code platform for vm {} {} -> {}", name, PlatformName(main), PlatformName(sub));
			return;
		}

		sameVmMap[sub] = to;
	}
	Platform VmInfo::RemapSamePlatform(Platform origin) const {
		auto svmIt = sameVmMap.find(origin);

		if (svmIt == sameVmMap.end()) {
			return origin;
		}

		return svmIt->second;
	}

	void VmInfo::RegisterDatatypeRenamed(const char* datatype, const char* trueName) {
		dataType[hash::Hash64(datatype)] = trueName;
	}

	void VmInfo::RegisterDatatype(const char* datatype) {
		dataType[hash::Hash64(datatype)] = datatype;
	}

	void VmInfo::RegisterDevCall(const char* devCall) {
		uint64_t hash = HashField(devCall);

		//LOG_TRACE("add dev call for {} {:x} -> {}", opnfo.internalName, hash, devCall);
		AddDevCallName(hash);
	}

	void RegisterOpCodes() {
		static std::once_flag f{};

		std::call_once(f, [] {
			// unknown
			RegisterOpCodeHandler(g_unknownOpcode);

			// nop operator
			// we don't care about the delete, it'll will stay until the end of the process,
			// todo: convert to object with destructor
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Abort, "Abort", "Abort()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Breakpoint, "Breakpoint", "Breakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_AutoBreakpoint, "AutoBreakpoint", "AutoBreakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ErrorBreakpoint, "ErrorBreakpoint", "ErrorBreakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_WatchBreakpoint, "WatchBreakpoint", "WatchBreakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_NotifyBreakpoint, "NotifyBreakpoint", "NotifyBreakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ThreadEndBreakpoint, "ThreadEndBreakpoint", "ThreadEndBreakpoint()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_PushVar, "PushVar", "PushVar()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_PushEntityVar, "PushEntityVar", "PushEntityVar()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown0, "Unknown0", "Unknown0()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown1, "Unknown1", "Unknown1()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown2, "Unknown2", "Unknown2()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown3, "Unknown3", "Unknown3()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown4, "Unknown4", "Unknown4()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown5, "Unknown5", "Unknown5()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown6, "Unknown6", "Unknown6()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown7, "Unknown7", "Unknown7()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknowna, "Unknowna", "Unknowna()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknownb, "Unknownb", "Unknownb()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Nop, "Nop", "Nop()"));
			RegisterOpCodeHandler(new OPCodeInfoInvalidOpCode());
			RegisterOpCodeHandler(new OPCodeInfoDevOp(false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume0, "Dev0", 0, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume1, "Dev1", 1, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume2, "Dev2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume4, "Dev4", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume8, "Dev8", 8, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume9, "Dev9", 9, false));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume0Push, "Dev0Push", 0, true));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume1Push, "Dev1Push", 1, true));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume2Push, "Dev2Push", 2, true));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume4Push, "Dev4Push", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume8Push, "Dev8Push", 8, true));
			RegisterOpCodeHandler(new OPCodeInfoDevConsume(OPCODE_DEV_Consume9Push, "Dev9Push", 9, true));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_Unknown38, "Unknown38", "operator_Unknown38()"));

			// skip opcodes for the gscbin decomp, they need a handler for the skip
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_0, "GscBinSkip0"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_1, "GscBinSkip1"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_2, "GscBinSkip2"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_3, "GscBinSkip3"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_4, "GscBinSkip4"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_5, "GscBinSkip5"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_N, "GscBinSkipN"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_3BC_4SD, "GscBinSkip3BC4SD"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_4BC_4SD, "GscBinSkip4BC4SD"));
			RegisterOpCodeHandler(new OPCodeInfoUnknown(OPCODE_GSCBIN_SKIP_4BC_1STR, "GscBinSkip4BC1STR"));

			RegisterOpCodeHandler(new OPCodeInfoGscBinStrToken(OPCODE_GSCBIN_SKIP_STR_TOKEN, "GscBinSkipSTRTOKEN"));

			// all op without params
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ProfileStart, "ProfileStart", "profilestart()"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_ProfileStop, "ProfileStop", "profilestop()"));
			RegisterOpCodeHandler(new OPCodeInfoFuncHash(OPCODE_PixBeginEvent, "PixBeginEvent", false, "pixbeginevent"));
			RegisterOpCodeHandler(new OPCodeInfoFuncHash(OPCODE_T7_ProfileStart, "ProfileStart", true, "profilestart"));
			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_PixEndEvent, "PixEndEvent", "pixendevent()"));
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
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_IW_Jump32, "Jump", true));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_Jump, "Jump"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnTrue, "JumpOnTrue"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnGreaterThan, "JumpOnGreaterThan"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnFalse, "JumpOnFalse"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnLessThan, "JumpOnLessThan"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_JumpOnDefined, "JumpOnDefined"));
			RegisterOpCodeHandler(new OPCodeInfoJumpExpr(OPCODE_JumpOnDefinedExpr, "JumpOnDefinedExpr"));
			RegisterOpCodeHandler(new OPCodeInfoJumpExpr(OPCODE_JumpOnFalseExpr, "JumpOnFalseExpr"));
			RegisterOpCodeHandler(new OPCodeInfoJumpExpr(OPCODE_JumpOnTrueExpr, "JumpOnTrueExpr"));
			
			// ukn jump
			RegisterOpCodeHandler(new OPCodeInfoJumpPush());


			RegisterOpCodeHandler(new OPCodeInfoEvalArray(OPCODE_EvalArrayRef, "EvalArrayRef", false));
			RegisterOpCodeHandler(new OPCodeInfoEvalArray(OPCODE_EvalArray, "EvalArray", true));
			RegisterOpCodeHandler(new OPCodeInfoCreateArray(OPCODE_CreateArray, "CreateArray"));
			RegisterOpCodeHandler(new OPCodeInfoAddToArray(OPCODE_AddToArray, "AddToArray"));
			RegisterOpCodeHandler(new OPCodeInfoCreateStruct(OPCODE_CreateStruct, "CreateStruct"));
			RegisterOpCodeHandler(new OPCodeInfoAddToStruct(OPCODE_AddToStruct, "AddToStruct", true));
			RegisterOpCodeHandler(new OPCodeInfoCastFieldObject(OPCODE_CastFieldObject, "CastFieldObject"));
			RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_Unknown35, "Unknown35"));
			RegisterOpCodeHandler(new OPCodeInfonop(OPCODE_Unknown9e, "Unknown9e"));

			// ref
			RegisterOpCodeHandler(new OPCodeInfoSetVariableField(OPCODE_SetVariableField, "SetVariableField"));
			RegisterOpCodeHandler(new OPCodeInfoSetVariableFieldRef(OPCODE_SetVariableFieldRef, "SetVariableFieldRef"));
			RegisterOpCodeHandler(new OPCodeInfoClearFieldVariable(OPCODE_ClearFieldVariable, "ClearFieldVariable", false, false));
			RegisterOpCodeHandler(new OPCodeInfoClearFieldVariable(OPCODE_ClearFieldVariableOnStack, "ClearFieldVariableOnStack", true, false));
			RegisterOpCodeHandler(new OPCodeInfoEvalSelfFieldVariable(OPCODE_EvalSelfFieldVariable, "EvalSelfFieldVariable"));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_EvalGlobalObjectFieldVariable, "EvalGlobalObjectFieldVariable"));
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
			RegisterOpCodeHandler(new OPCodeInfoSetNextArrayKeyCached(OPCODE_NextArrayKey, "NextArrayKey", false));
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
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTill, "WaitTill", "waittill")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatchTimeout, "WaitTillMatchTimeout", "waittillmatchtimeout"));
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatch, "WaitTillMatch", "waittillmatch")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaittillTimeout, "WaittillTimeout", "waittilltimeout")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatch2, "WaitTillMatch", "waittillmatch", 1)); // count = params + self - 1 (no hash?)
			// operation
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_And, "Bit_And", 2, "&", TYPE_OPERATION_MERGE, PRIORITY_BIT_AND));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_Or, "Bit_Or", 2, "|", TYPE_OPERATION_MERGE, PRIORITY_BIT_OR));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Bit_Xor, "Bit_Xor", 2, "^", TYPE_OPERATION_MERGE, PRIORITY_BIT_XOR));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_GreaterThan, "GreaterThan", 2, ">", TYPE_OPERATION2, PRIORITY_COMPARE, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_LessThan, "LessThan", 2, "<", TYPE_OPERATION2, PRIORITY_COMPARE, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_GreaterThanOrEqualTo, "GreaterThanOrEqualTo", 2, ">=", TYPE_OPERATION2, PRIORITY_COMPARE, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_LessThanOrEqualTo, "LessThanOrEqualTo", 2, "<=", TYPE_OPERATION2, PRIORITY_COMPARE, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_ShiftRight, "ShiftRight", 2, ">>", TYPE_OPERATION_MERGE, PRIORITY_BIT_SHIFTS));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_ShiftLeft, "ShiftLeft", 2, "<<", TYPE_OPERATION_MERGE, PRIORITY_BIT_SHIFTS));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Plus, "Plus", 2, "+", TYPE_OPERATION_MERGE, PRIORITY_PLUS));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Minus, "Minus", 2, "-", TYPE_OPERATION_MERGE, PRIORITY_PLUS));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Divide, "Divide", 2, "/", TYPE_OPERATION_MERGE, PRIORITY_MULT));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Modulus, "Modulus", 2, "%", TYPE_OPERATION_MERGE, PRIORITY_MULT));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Multiply, "Multiply", 2, "*", TYPE_OPERATION_MERGE, PRIORITY_MULT));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_Equal, "Equal", 2, "==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_SuperEqual, "SuperEqual", 2, "===", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_T10_GreaterThanOrSuperEqualTo, "GreaterThanOrSuperEqualTo", 2, ">==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_T10_LowerThanOrSuperEqualTo, "LowerThanOrSuperEqualTo", 2, "<==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_NotEqual, "NotEqual", 2, "!=", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_SuperNotEqual, "SuperNotEqual", 2, "!==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));

			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_BoolComplement, "BoolComplement", 1, "~", TYPE_OPERATION1, PRIORITY_UNARY, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_BoolNot, "BoolNot", 1, "!", TYPE_OPERATION1, PRIORITY_UNARY, true, false, true, true));
			RegisterOpCodeHandler(new OPCodeInfoCastBool(OPCODE_CastBool, "CastBool"));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_CastCanon, "CastCanon", 1, "", TYPE_OPERATION1, PRIORITY_UNARY, false, true)); // context dependent, the "" can be replaced to tag them

			// Remove return value from operator
			RegisterOpCodeHandler(new OPCodeInfodec(OPCODE_DecTop, "DecTop"));
			RegisterOpCodeHandler(new OPCodeInfodec(OPCODE_SafeDecTop, "SafeDecTop"));

			// gets
			RegisterOpCodeHandler(new OPCodeInfoGetConstant<const char*>(OPCODE_GetUndefined, "GetUndefined", "undefined", false, false, TYPE_GET_UNDEFINED));
			RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_GetTime, "GetTime", "gettime()"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_GetHash, "GetHash", "#"));
			RegisterOpCodeHandler(new OPCodeInfoGetConstant<int32_t>(OPCODE_GetZero, "GetZero", 0, true, true));
			RegisterOpCodeHandler(new OPCodeInfoGetNeg<uint32_t>(OPCODE_GetNegUnsignedInteger, "GetNegUnsignedInteger"));
			RegisterOpCodeHandler(new OPCodeInfoGetNeg<uint16_t>(OPCODE_GetNegUnsignedShort, "GetNegUnsignedShort"));
			RegisterOpCodeHandler(new OPCodeInfoGetNeg<uint8_t>(OPCODE_GetNegByte, "GetNegByte"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<byte>(OPCODE_GetByte, "GetByte"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<int32_t>(OPCODE_GetInteger, "GetInteger"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<int64_t>(OPCODE_GetLongInteger, "GetLongInteger"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<uint32_t>(OPCODE_GetUnsignedInteger, "GetUnsignedInteger"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<uint16_t>(OPCODE_GetUnsignedShort, "GetUnsignedShort"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<FLOAT, FLOAT>(OPCODE_GetFloat, "GetFloat", TYPE_FLOAT));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<uintptr_t, uintptr_t>(OPCODE_GetUIntPtr, "GetUIntPtr"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<int8_t>(OPCODE_GetSignedByte, "GetSignedByte"));
			RegisterOpCodeHandler(new OPCodeInfoGetNumber<int16_t>(OPCODE_GetShort, "GetShort"));
			RegisterOpCodeHandler(new OPCodeInfoVector());
			RegisterOpCodeHandler(new OPCodeInfoVectorConstant());
			RegisterOpCodeHandler(new OPCodeInfoGetVector());

			RegisterOpCodeHandler(new OPCodeInfoVectorScale(OPCODE_VectorScale, "VectorScale"));
			RegisterOpCodeHandler(new OPCodeInfoGetObjectSize(OPCODE_SizeOf, "SizeOf"));

			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd", "waittillframeend()"));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_Wait, "Wait", "wait", false, true, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_Wait2, "Wait2", "wait", false, true, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_WaitFrame, "WaitFrame", "waitframe"));
			RegisterOpCodeHandler(new OPCodeInfoNotify(OPCODE_Notify, "Notify"));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_IsDefined, "IsDefined", "isdefined", false, false, false, true));

			// PRECODEPOS/CODEPOS on stack
			RegisterOpCodeHandler(new OPCodeInfoNopStmt(OPCODE_ClearParams, "ClearParams"));
			RegisterOpCodeHandler(new OPCodeInfoPreScriptCall(OPCODE_PreScriptCall, "PreScriptCall"));

			RegisterOpCodeHandler(new OPCodeInfoGetConstant<const char*>(OPCODE_EmptyArray, "EmptyArray", "[]"));
			RegisterOpCodeHandler(new OPCodeInfoClearArray(OPCODE_ClearArray, "ClearArray"));
			RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_GetSelf, "GetSelf", "self", false, false, TYPE_SELF));
			RegisterOpCodeHandler(new OPCodeInfoGetConstantRef(OPCODE_GetSelfObject, "GetSelfObject", "self", TYPE_SELF));

			// class stuff
			RegisterOpCodeHandler(new OPCodeInfoGetObjectType(OPCODE_GetObjectType, "GetObjectType", "class"));
			RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionCall, "ClassFunctionCall"));
			RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionThreadCall, "ClassFunctionThreadCall"));
			RegisterOpCodeHandler(new OPCodeInfoFuncClassCall(OPCODE_ClassFunctionThreadCallEndOn, "ClassFunctionThreadCallEndOn"));

			// functions

			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptFunctionCall, "ScriptFunctionCall", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCallEndOn, "ScriptThreadCallEndOn", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCall, "ScriptThreadCall", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCallEndOn, "ScriptMethodThreadCallEndOn", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodCall, "ScriptMethodCall", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCall, "ScriptMethodThreadCall", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinFunction, "CallBuiltinFunction", 2, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction0, "CallBuiltinFunction0", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction1, "CallBuiltinFunction1", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction2, "CallBuiltinFunction2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction3, "CallBuiltinFunction3", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction4, "CallBuiltinFunction4", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction5, "CallBuiltinFunction5", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinMethod, "CallBuiltinMethod", 2, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod0, "CallBuiltinMethod0", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod1, "CallBuiltinMethod1", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod2, "CallBuiltinMethod2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod3, "CallBuiltinMethod3", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod4, "CallBuiltinMethod4", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod5, "CallBuiltinMethod5", 2, false));

			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalCall, "LocalCall", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalCall2, "LocalCall2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalThreadCall, "LocalThreadCall", 2, true));

			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointerEndOn, "ScriptThreadCallPointerEndOn", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointer, "ScriptThreadCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointer, "ScriptMethodThreadCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointerEndOn, "ScriptMethodThreadCallPointerEndOn", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptFunctionCallPointer, "ScriptFunctionCallPointer", false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodCallPointer, "ScriptMethodCallPointer", false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_IW_BuiltinFunctionCallPointer, "BuiltinFunctionCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_IW_BuiltinMethodCallPointer, "BuiltinMethodCallPointer", true));

			// linked ref
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetResolveFunction, "GetResolveFunction", 4));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetFunction, "GetFunction", 2));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetLocal, "GetLocal", 2));
			RegisterOpCodeHandler(new OPCodeInfoGetString(OPCODE_GetString, "GetString", false));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobal, "GetGlobal", GGGT_PUSH, nullptr));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobalObject, "GetGlobalObject", GGGT_GLOBAL, nullptr));

			// T9
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectFromRef, "EvalFieldVariableFromObjectFromRef", true));
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectCached, "EvalFieldVariableFromObjectCached", false));
			RegisterOpCodeHandler(new OPCodeT9SetFieldVariableFromObjectFromRef());
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromGlobalObject());
			RegisterOpCodeHandler(new OPCodeT9SetVariableFieldFromEvalArrayRef());
			RegisterOpCodeHandler(new OPCodeT9EvalArrayCached(OPCODE_T9_EvalArrayCached, "EvalArrayCached", true));
			RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_T9_EndOnCallbackParam, "EndOnCallbackParam", "endoncallbackparam", false)); // count = params + self
			RegisterOpCodeHandler(new OPCodeT9GetVarRef());
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorKey, "IteratorKey", "iteratorkey", TYPE_ARRAY_NEXTKEY_IT));
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorVal, "IteratorVal", "iteratorval", TYPE_ARRAY_NEXTVAL_IT));
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorNext, "IteratorNext", "iteratornext", TYPE_ARRAY_NEXT_IT));

			RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_IncLocalVariableCached, "IncLocalVariableCached", "++"));
			RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_DecLocalVariableCached, "DecLocalVariableCached", "--"));

			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_T9_EvalLocalVariableCachedDouble, "EvalLocalVariableCached2n", 2));

			// IW
			RegisterOpCodeHandler(new OPCodeInfoRegisterVariable());
			RegisterOpCodeHandler(new OPCodeInfoRegisterMultipleVariables());
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached0, "EvalLocalVariableCached0", 1, 0));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached1, "EvalLocalVariableCached1", 1, 1));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached2, "EvalLocalVariableCached2", 1, 2));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached3, "EvalLocalVariableCached3", 1, 3));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached4, "EvalLocalVariableCached4", 1, 4));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached5, "EvalLocalVariableCached5", 1, 5));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalArrayCached());
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalObjectCached());
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariable, "EvalLevelFieldVariable", "level", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariableRef, "EvalLevelFieldVariableRef", "level", false));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetLevelFieldVariable, "SetLevelFieldVar", "level"));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVar, "EvalAnimFieldVar", "anim", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVarRef, "EvalAnimFieldVarRef", "anim", false));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetAnimFieldVar, "SetAnimFieldVar", "anim"));
			// let's say it's a "global"
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVar, "EvalSelfFieldVar", "self", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVarRef, "EvalSelfFieldVarRef", "self", false));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetSelfFieldVar, "SetSelfFieldVar", "self"));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetBuiltinFunction, "GetBuiltinFunction", 2));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetBuiltinMethod, "GetBuiltinMethod", 2));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_SingleEndon, "Endon", "endon", false));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_SingleWaitTill, "WaitTill", "waittill", true, 1, 0, true, TYPE_WAITTILL));
			RegisterOpCodeHandler(new OPCodeInfoSingleFunc(OPCODE_IW_IsTrue, "IsTrue", "istrue", true, true));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetLevel, "GetLevel", GGGT_PUSH, "level"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetLevelGRef, "GetLevelGRef", GGGT_GLOBAL, "level"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetGame, "GetGame", GGGT_PUSH, "game"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetGameRef, "GetGameRef", GGGT_FIELD, "game"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetAnim, "GetAnim", GGGT_PUSH, "anim"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetAnimGRef, "GetAnimGRef", GGGT_GLOBAL, "anim"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetWorld, "GetWorld", GGGT_PUSH, "world"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetWorldObject, "GetWorldObject", GGGT_GLOBAL, "world"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetClasses, "OPCODE_GetClasses", GGGT_PUSH, "classes"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetClassesObject, "GetClassesObject", GGGT_GLOBAL, "classes"));
			RegisterOpCodeHandler(new OPCodeT9EvalArrayCached(OPCODE_IW_EvalArrayCachedField, "EvalArrayCachedField", false));
			RegisterOpCodeHandler(new OPCodeInfoClearLocalVariableCached(OPCODE_IW_ClearFieldVariableRef, "ClearFieldVariableRef"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetDVarHash, "GetDVarHash", "@"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetResourceHash, "GetResourceHash", "%"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetResourceHash2, "GetResourceHash2", "r"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetTagHash, "GetTagHash", "t", false));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_T10_GetScrHash, "GetScrHash", "&"));
			
			RegisterOpCodeHandler(new OPCodeInfoIWSwitch());
			RegisterOpCodeHandler(new OPCodeInfoIWEndSwitch());
			RegisterOpCodeHandler(new OPCodeInfoGetString(OPCODE_IW_GetIString, "GetIString", true));
			RegisterOpCodeHandler(new OPCodeInfoGetAnimation(OPCODE_IW_GetAnimation, "GetAnimation", true));
			RegisterOpCodeHandler(new OPCodeInfoGetAnimation(OPCODE_IW_GetAnimationTree, "GetAnimationTree", false));
			RegisterOpCodeHandler(new OPCodeInfoAddToStruct(OPCODE_IW_AddToStruct, "AddToStruct", false));
			RegisterOpCodeHandler(new OPCodeInfoSetWaittillVariableFieldCached(OPCODE_IW_SetWaittillVariableFieldCached, "SetWaittillVariableFieldCached"));
			RegisterOpCodeHandler(new OPCodeInfoSetWaittillVariableFieldCached(OPCODE_IgnoreWaittillVariableFieldCached, "SafeDecTop", "_"));

			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_IW_WaitFrame, "WaitSingleFrame", "waitframe()"));
			RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_IW_GetThread, "GetThread", "getthread()"));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_WaitTillMatch, "WaitTillMatch", "waittillmatch", true, 2, 2, true, TYPE_WAITTILL));
			// scripts\asm\asm::asm_getanim()'s assertmsg lol
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_T10_FlatArgs, "FlatArgs", "flat_args", true, 2, 0, false));
			RegisterOpCodeHandler(new OPCodeInfoGetPositionRef());
			
			RegisterOpCodeHandler(new OPCodeInfoIWNotify(OPCODE_IW_Notify, "Notify"));

			// T7
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_GetHash32, "GetHash", "#", false));


			// T8compiler custom opcode
			RegisterOpCodeHandler(new OPCodeInfoT8CGetLazyFunction());

			RegisterOpCodesMap();
			});
	}

	void RegisterOpCodeHandler(const OPCodeInfo* info) {
		g_opcodeHandlerMap[info->m_id] = info;
	}

	const std::unordered_map<uint64_t, VmInfo>& GetVMMaps() {
		RegisterOpCodes();
		return g_opcodeMap;
	}


	bool IsValidVmMagic(uint64_t magic, VmInfo*& info, bool registerOpCodes) {
		// same function, before we used the revision, now we use the magic
		return IsValidVm(magic, info, registerOpCodes);
	}

	bool IsValidVm(uint64_t vm, VmInfo*& info, bool registerOpCodes) {
		// build map
		if (registerOpCodes) RegisterOpCodes();
		auto ref = g_opcodeMap.find(vm);
		if (ref == g_opcodeMap.end()) {
			return false;
		}
		info = &ref->second;

		return true;
	}

	const OPCodeInfo* LookupOpCode(uint64_t vm, Platform platform, uint16_t opcode) {
		// build map
		RegisterOpCodes();

		VmInfo* info;

		if (!(IsValidVm(vm, info))) {
			return g_unknownOpcode;
		}

		auto ref = info->opcodemap.find(opcode);

		if (ref == info->opcodemap.end()) {
			return g_unknownOpcode;
		}

		auto ref2 = ref->second.find(info->RemapSamePlatform(platform));

		if (ref2 == ref->second.end()) {
			return g_unknownOpcode;
		}

		const auto refHandler = g_opcodeHandlerMap.find(ref2->second);

		if (refHandler == g_opcodeHandlerMap.end()) {
			return g_unknownOpcode;
		}

		return refHandler->second;
	}
	std::pair<bool, uint16_t> GetOpCodeId(uint64_t vm, Platform platform, OPCode opcode) {
		RegisterOpCodes();

		VmInfo* info;

		if (!(IsValidVm(vm, info))) {
			return std::make_pair(false, 0);
		}


		auto ref = info->opcodemaplookup.find(opcode);

		if (ref == info->opcodemaplookup.end()) {
			return std::make_pair(false, 0);
		}

		auto ref2 = ref->second.find(info->RemapSamePlatform(platform));


		if (ref2 == ref->second.end()) {
			return std::make_pair(false, 0);
		}

		return std::make_pair(true, ref2->second);
	}

	bool HasOpCode(uint64_t vm, Platform plt, OPCode opcode) {
		auto [ok, id] = GetOpCodeId(vm, plt, opcode);
		return ok;
	}

#pragma endregion
#pragma region asmctx 
	ASMContext::ASMContext(byte* fonctionStart, GSCOBJHandler& gscReader, T8GSCOBJContext& objctx, const GscInfoOption& opt, uint64_t nsp, GSCExportReader& exp, void* readerHandle, uint64_t vm, Platform platform)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_gscReader(gscReader), m_objctx(objctx), m_opt(opt), m_runDecompiler(opt.m_dcomp),
		m_lastOpCodeBase(-1), m_namespace(nsp), m_funcBlock(BLOCK_DEFAULT, false, false), m_exp(exp), m_readerHandle(readerHandle), m_vm(vm), m_platform(platform) {
		// set start as unhandled
		PushLocation();
		exp.SetHandle(m_readerHandle);
		funcRloc = exp.GetAddress();
	}


	ASMSkipContext::ASMSkipContext(byte* fonctionStart, Platform platform, VmInfo* vminfo)
		: m_fonctionStart(fonctionStart), m_bcl(fonctionStart), m_platform(platform), m_vminfo(vminfo) {
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

	void asmcontextlocation::RemoveRef(int32_t origin) {
		refs.erase(origin);
	}

	asmcontextlocation& ASMContext::PushLocation(byte* location) {
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

	asmcontextlocation& ASMSkipContext::PushLocation(byte* location) {
		// push aligned location to avoid missing a location
		if (m_vminfo->HasFlag(VmFlags::VMF_ALIGN | VmFlags::VMF_OPCODE_U16)) {
			location = utils::Aligned<uint16_t>(location);
		}
		auto loc = FunctionRelativeLocation(location);
		auto& ref = m_locs[loc];

		// we write the location for the return value
		ref.rloc = loc;
		return ref;
	}

	bool ASMSkipContext::FindNextLocation() {
		int64_t min = 0xFFFFFFFFFF;
		int32_t minloc = 0;
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

#pragma endregion
#pragma region asm_stack 

int ASMContext::GetLocalVarIdByName(uint64_t name) const {
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

		m_funcBlock.m_statements.push_back({ node, loc});
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

#pragma endregion
#pragma region asm_block

std::ostream& DecompContext::WritePadding(std::ostream& out, bool forceNoRLoc) {
	utils::Padding(out, paddingPre);
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

bool IsBlockType(tool::gsc::opcode::ASMContextNodeType type) {
	using namespace tool::gsc::opcode;
	return type == TYPE_BLOCK || type == TYPE_DO_WHILE || type == TYPE_WHILE || type == TYPE_FOR 
		|| type == TYPE_FOR_EACH || type == TYPE_IF || type == TYPE_SWITCH_POSTCOMPUTE || type == TYPE_SWITCH_PRECOMPUTE;
}

void ASMContextNodeBlock::Dump(std::ostream& out, DecompContext& ctx) const {
	bool inlined{};
	if (!m_disabled) {
		switch (m_blockType) {
		case BLOCK_DEFAULT: {
			inlined = IsBlockInlineable(this, ctx.opt);
			if (inlined) {
				out << "\n";
			}
			else {
				out << "{\n";
			}
			break;
		}
		case BLOCK_DEV:
			out << "/#\n";
			break;
		default:
			out << "\n";
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
		if (ctx.opt.m_formatter->flags & tool::gsc::formatter::FFL_NEWLINES_BETWEEN_BLOCKS) {
			if (lastType && (((!hide && ref.node->m_type != TYPE_PRECODEPOS) || ctx.opt.m_show_internal_blocks))) {
				if (ref.node->m_type != TYPE_END || i != m_statements.size() - 1) {
					if (IsBlockType(ref.node->m_type)) {
						ctx.WritePadding(out) << "\n";
						lastType = LT_BLOCK;
					}
					else if (lastType != LT_STMT) {
						ctx.WritePadding(out) << "\n";
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
			out << ":\n";
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
					out << "\n";
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
				out << "return;\n";
			}
			else if (ctx.opt.m_show_internal_blocks) {
				ctx.WritePadding(out) << "<end>;\n";
			}
			else if (m_statements.size() == 1) {
				// empty end block, write a small empty line
				ctx.WritePadding(out) << "\n";
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
				ctx.WritePadding(out, true) << "}\n";
			}
			break;
		case BLOCK_DEV:
			ctx.WritePadding(out, true) << "#/\n";
			break;
		}
	}
	else {
		if (SizeNoEmptyNode(m_statements) == 0) {
			out << "\n"; // disabled and empty, we add an empty line
		}
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
			asmcontextlocation* jumpEndLocation = nullptr;
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

static void ReplaceSwitchBlockBreak(ASMContextNodeBlock& current, ASMContext& ctx, asmcontextlocation* endLocation, uint64_t endCase, int64_t end) {
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

		asmcontextlocation* endLocation = nullptr;
		if (endLoc && endLoc->location->rloc == end) {
			endLocation = endLoc->location;
		}

		b.node = switchBlock;

		if (cases.size()) {
			// we have at least one case
			size_t cid{};

			do {
				nodeblocktype blockType = (ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_SWITCH_FORCE_BLOCKS) ? BLOCK_DEFAULT : BLOCK_PADDING;
				ASMContextNodeBlock* block = new ASMContextNodeBlock(blockType);
				switchBlock->m_cases.push_back({ cases[cid].casenode ? cases[cid].casenode->Clone() : nullptr, block});
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
		} else if (it->node->m_type == TYPE_JUMP_ENDSWITCH) {
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
			return;
		}
		stmt.node->ApplySubBlocks([endOnReturn, &func](ASMContextNodeBlock* block, ASMContext& ctx) {
			for (auto& stmt : block->m_statements) {
				if (endOnReturn && stmt.node && (stmt.node->m_type == TYPE_RETURN || stmt.node->m_type == TYPE_END)) {
					break;
				}
				ApplySubStatement(stmt, ctx, func, endOnReturn);
			}
		}, ctx);
		func(stmt);
		return;
	}

	dynamic_cast<ASMContextNodeBlock*>(stmt.node)->ApplySubBlocks([&func, endOnReturn](ASMContextNodeBlock* block, ASMContext& ctx) {
		for (auto& stmt : block->m_statements) {
			if (endOnReturn && stmt.node && (stmt.node->m_type == TYPE_RETURN || stmt.node->m_type == TYPE_END)) {
				break;
			}
			ApplySubStatement(stmt, ctx, func, endOnReturn);
		}
	}, ctx);
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
	constexpr uint64_t getfirstarraykeyIWHash = hash::HashJupScr("getfirstarraykey");
	constexpr uint64_t getnextarraykeyIWHash = hash::HashJupScr("getnextarraykey");
	constexpr uint64_t getfirstarraykeyCerHash = hash::HashT10Scr("getfirstarraykey");
	constexpr uint64_t getnextarraykeyCerHash = hash::HashT10Scr("getnextarraykey");
	constexpr uint64_t getfirstarraykeyCerBHash = hash::HashT10ScrSP("getfirstarraykey");
	constexpr uint64_t getnextarraykeyCerBHash = hash::HashT10ScrSP("getnextarraykey");

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
		uint64_t arrayRefName = static_cast<ASMContextNodeIdentifier*>(setOp->m_left)->m_value;

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
			auto* callFunc = dynamic_cast<ASMContextNodeCallFuncPtr*>(setFirstArrayOp->m_right);

			if (callFunc->m_flags || !callFunc->m_operands.size() || callFunc->m_operands[0]->m_type != TYPE_FUNC_REFNAME) {
				return true;
			}

			auto* funcRef = dynamic_cast<ASMContextNodeFuncRef*>(callFunc->m_operands[0]);

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

		size_t forincsize = ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW) ? 2 : ctx.m_vm <= VMI_T8 ? 3 : 4;

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
		uint64_t itemValName;
		if (ctx.m_objctx.m_vmInfo->HasFlag(VmFlags::VMF_FOREACH_IW)) {
			/*
				agent = var_57acddc40b2f741[var_54ed0dc40829774];;

				// ... code

				// unlike in treyarch, the nextarray is at the end and not at the start of the iteration
				var_54ed0dc40829774 = getnextarraykey(var_57acddc40b2f741, var_54ed0dc40829774)
				continue
			*/
			auto* setKeyArrayOp = static_cast<ASMContextNodeLeftRightOperator*>(itemSetStmt.node);

			if (setKeyArrayOp->m_left->m_type != TYPE_IDENTIFIER) {
				index += moveDelta;
				continue; // agent = var_57acddc40b2f741[var_54ed0dc40829774];
			}

			itemValName = static_cast<ASMContextNodeIdentifier*>(setKeyArrayOp->m_left)->m_value;
		}
		else if (ctx.m_vm > VMI_T8) {
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
			
			if (nextArrayStmt.node->m_type != TYPE_ARRAY_NEXTKEY 
				&& !(nextArrayStmt.node->m_type == TYPE_SET 
					&& static_cast<ASMContextNodeLeftRightOperator*>(nextArrayStmt.node)->m_right->m_type == TYPE_ARRAY_NEXTKEYPUSH)
				) {
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
		int64_t endLoc = jumpOp->m_location; // jump location for the break
		int64_t condLoc = condJump.location->rloc; // jump back location for the end of the loop
		// index of the last node (at least + 2 for the next value set and the jump)
		auto endNodeIndex = index;


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

				auto* j = static_cast<ASMContextNodeJumpOperator*>(stmt.node);
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
		else if (ctx.m_vm <= VMI_T8) {
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
					auto* clearKeyVal = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
					if (clearKeyVal->m_left->m_type == TYPE_IDENTIFIER && static_cast<ASMContextNodeIdentifier*>(clearKeyVal->m_left)->m_value == keyValName
						&& clearKeyVal->m_right->m_type == TYPE_GET_UNDEFINED) {
						delete it->node;
						it = m_statements.erase(it);

						if (it != m_statements.end()) {

							if (it->node->m_type == TYPE_SET) {
								auto* clearKeyVal = static_cast<ASMContextNodeLeftRightOperator*>(it->node);
								if (clearKeyVal->m_left->m_type == TYPE_IDENTIFIER && static_cast<ASMContextNodeIdentifier*>(clearKeyVal->m_left)->m_value == arrayRefName
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
			asmcontextlocation* endLocation = nullptr;
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
			if (!(ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
				cond = ASMCNodeConvertToBool(cond);
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

			if (!(ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
				cond = ASMCNodeConvertToBool(cond);
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

				newBlock->m_statements.push_back({ replaceNode ? replaceNode : stmt.node->Clone(), stmt.location});
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
		if (!(ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
			ifCond = ASMCNodeConvertToBool(ifCond);
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

					if (!(ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
						cond = ASMCNodeConvertToBool(cond);
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
	if (ctx.m_opt.m_formatter->flags & tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS) {
		return 0;
	}
	bool isCandidate{ true };

	// check all the return values, if all of them are bools, we can replace to bool
	for (size_t i = 0; i < m_statements.size(); i++) {
		auto& stmt = m_statements[i];

		ApplySubStatement(stmt, ctx, [&isCandidate](ASMContextStatement& stmt) {
			if (stmt.node->m_type == TYPE_END) {
				isCandidate = false;
				return; // empty return -> return undefined
			}

			if (stmt.node->m_type != TYPE_RETURN) {
				return;
			}

			auto* ret = reinterpret_cast<ASMContextNodeOp1*>(stmt.node);

			auto* retRes = ret->m_operand;

			if (!retRes->IsBoolConvertable(true)) {
				isCandidate = false;
				return;
			}
			// ok
		}, false);

		if (!isCandidate) {
			return 0;
		}
	}
	if (!isCandidate) {
		return 0;
	}

	// convert the return values to booleans
	for (size_t i = 0; i < m_statements.size(); i++) {
		auto& stmt = m_statements[i];

		ApplySubStatement(stmt, ctx, [](ASMContextStatement& stmt) {
			if (stmt.node->m_type != TYPE_RETURN) {
				return;
			}

			auto* ret = reinterpret_cast<ASMContextNodeOp1*>(stmt.node);

			ret->m_operand = ASMCNodeConvertToBool(ret->m_operand);
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
							uint64_t refName = dynamic_cast<ASMContextNodeFuncRef*>(name)->m_func & 0x7FFFFFFFFFFFFFFF;
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
void ASMContext::DisableDecompiler(const std::string& reason) {
	m_disableDecompiler = true;
	m_disableDecompilerError = reason;
}

#pragma endregion
