#include <includes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_opcodes.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_asm_nodes.hpp>

namespace tool::gsc::opcode {
	namespace {
		// opcode->opcode handler
		std::unordered_map<OPCode, const OPCodeInfo*> g_opcodeHandlerMap{};
	}

	OPCodeInfo::OPCodeInfo(OPCode id, const char* name) : m_id(id), m_name(name) {
	}

	int OPCodeInfo::Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const {
		out << std::endl;
		return 0; // by default nop
	}



	class ASMContextLocationOpCatch : public ASMContextLocationOp {
	private:
		int tid;
	public:
		ASMContextLocationOpCatch(int tid) : ASMContextLocationOp("Catch"), tid(tid) {
		}
		void Run(ASMContext& context, tool::gsc::T8GSCOBJContext& objctx)  const override {
			ASMContextNodeValue<const char*>* ref = new ASMContextNodeValue<const char*>("Catch", TYPE_CATCH_PRECOMPUTE);
			// convert it to statement
			ref->m_priority = PRIORITY_INST;
			// TODO: use tid
			context.PushASMCNode(ref);
			context.CompleteStatement();
		}
	};
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


	class OPCodeInfoUnknown : public OPCodeInfo {
	public:
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << "Unknown operator: " << std::hex << value << std::endl;

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
				out << std::endl;
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
					context.WritePadding(out << std::endl);
					out << '"' << utils::FormattedString(cand) << '"';
					out << std::endl;
				}

				context.m_bcl++;
			}


			context.m_bcl = oldBcl;

			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				for (size_t i = 0; i < 0x3; i++) {
					auto* loc = utils::Aligned<uint32_t>(context.m_bcl);

					uint32_t hash = *reinterpret_cast<uint32_t*>(loc + i * 4);
					auto* str = hashutils::ExtractPtr(hash);

					if (str) {
						context.WritePadding(out) << "#" << std::dec << i << ": " << str << " (var_" << std::hex << hash << ")" << std::endl;
					}
				}
			}
			else if (!(objctx.m_vmInfo->flags & tool::gsc::opcode::VmFlags::VMF_HASH64)) {
				for (size_t i = 0; i < 0x10; i++) {
					uint32_t hash = *reinterpret_cast<uint32_t*>(context.m_bcl + i);
					const char* str = hash ? hashutils::ExtractPtr(hash) : nullptr;

					if (str) {
						context.WritePadding(out) << "#0x" << std::hex << i << ": " << str << " (var_" << std::hex << hash << ")" << std::endl;
					}
				}
			}

			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				for (size_t i = 0; i < 0x3; i++) {
					byte* loc = utils::Aligned<uint64_t>(context.m_bcl);

					uint64_t hash = *reinterpret_cast<uint64_t*>(loc + i * 8);
					const char* str = hashutils::ExtractPtr(hash);

					if (str) {
						context.WritePadding(out) << "#" << std::dec << i << ": #\"" << str << "\" (hash_" << std::hex << hash << ")" << std::endl;
					}
				}
			}
			else {
				for (size_t i = 0; i < 0x18; i++) {
					uint64_t hash = *reinterpret_cast<uint64_t*>(context.m_bcl + i);
					const char* str = hash ? hashutils::ExtractPtr(hash) : nullptr;

					if (str) {
						context.WritePadding(out) << "#0x" << std::hex << i << ": " << str << " (hash_" << std::hex << hash << ")" << std::endl;
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
	};


	class OPCodeInfoUnknownDev : public OPCodeInfo {
	public:
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			auto& loc = context.Aligned<uint16_t>();

			uint16_t val = context.Read<uint16_t>(loc);

			loc += 2;
			out << std::hex << val << std::endl;
			return 0;
		}
	};

	class OPCodeInfoRegisterVariable : public OPCodeInfo {
		bool m_hasName;
	public:
		OPCodeInfoRegisterVariable(OPCode id, const char* name, bool hasName) : OPCodeInfo(id, name), m_hasName(hasName) {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			uint64_t name;

			if (m_hasName) {
				name = context.Read<uint64_t>(context.m_bcl);
				context.m_bcl += 8;
			}
			else {
				byte id{ context.Read<byte>(context.m_bcl) };
				const char* ids{ utils::va("var%d", id) };
				name = hash::Hash64(ids);
				hashutils::AddPrecomputed(name, ids);
				context.m_bcl += 1;
			}

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
	};

	class OPCodeInfoRegisterMultipleVariables : public OPCodeInfo {
	public:
		OPCodeInfoRegisterMultipleVariables() : OPCodeInfo(OPCode::OPCODE_IW_RegisterMultipleVariables, "IW_RegisterMultipleVariables") {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			byte count = context.Read<byte>(context.m_bcl++);

			out << "count: " << std::dec << (int)count << std::endl;

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
	};

	class OPCodeInfoSetNewLocalVariable : public OPCodeInfo {
	public:
		OPCodeInfoSetNewLocalVariable() : OPCodeInfo(OPCode::OPCODE_IW_SetNewLocalVariableFieldCached0, "IW_SetNewLocalVariableFieldCached0") {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			byte id{ context.Read<byte>(context.m_bcl) };
			const char* ids{ utils::va("var%d", id) };
			uint64_t name = hash::Hash64(ids);
			hashutils::AddPrecomputed(name, ids);
			context.m_bcl += 1;

			if (!context.m_localvars.size()) {
				// the local variables starts at 1
				context.m_localvars.insert(context.m_localvars.begin(), { hash::HashT89Scr("<error>"), 0 });
			}

			context.m_localvars.insert(context.m_localvars.begin(), { name, 0 });
			out << hashutils::ExtractTmp("var", name) << " (-" << std::dec << context.m_localvars.size() << ")" << std::endl;

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeLeftRightOperator(new ASMContextNodeIdentifier(name), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
				context.CompleteStatement();
			}

			// don't create statement, we can ignore it
			context.m_lastOpCodeBase = -1;
			return 0;
		}
	};

	class OPCodeInfoRemoveLocalVariables : public OPCodeInfo {
	public:
		OPCodeInfoRemoveLocalVariables() : OPCodeInfo(OPCode::OPCODE_IW_RemoveVariables, "IW_RemoveVariables") {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			byte count{ context.Read<byte>(context.m_bcl) };
			context.m_bcl += 1;

			out << "count: " << (int)count;

			for (size_t i = 0; i < count; i++) {
				if (context.m_localvars.size() <= 1) {
					out << " <invalid>";
					continue;
				}
				out << " " << hashutils::ExtractTmp("var", context.m_localvars[0].name);
				context.m_localvars.erase(context.m_localvars.begin());
			}
			out << std::endl;

			context.PushASMCNode(new ASMContextNodeValue<const char*>("<emptypos_removelocalvars>", TYPE_PRECODEPOS));
			context.CompleteStatement();
			return 0;
		}
	};

	class OPCodeInfoSafeCreateLocalVariables : public OPCodeInfo {
	public:
		OPCodeInfoSafeCreateLocalVariables() : OPCodeInfo(OPCODE_SafeCreateLocalVariables, "SafeCreateLocalVariables") {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			byte count = context.Read<byte>(context.m_bcl++);

			context.m_localvars.reserve((size_t)count + 1);

			out << std::hex << "count: 0x" << (int)count << std::endl;
			if (!context.m_localvars.size()) {
				// the local variables starts at 1
				context.m_localvars.insert(context.m_localvars.begin(), { hash::HashT89Scr("<error>"), 0 });
			}

			bool lastVa{};
			for (size_t i = 0; i < count; i++) {
				context.WritePadding(out);
				uint64_t varName;

				if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_VAR_ID)) {
					const char* varNameStr{ utils::va("var%d", context.Read<byte>(context.m_bcl++)) };
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
				else if (context.m_vm != tool::gsc::opcode::VMI_T8_36 && (flags & tool::gsc::opcode::T8GSCLocalVarFlag::T9_VAR_REF)) {
					out << "*";
				}

				if (flags) {
					out << ", pflags: 0x" << std::hex << (int)flags;
				}


				byte mask = ~(tool::gsc::opcode::T8GSCLocalVarFlag::VARIADIC | tool::gsc::opcode::T8GSCLocalVarFlag::ARRAY_REF);

				if (context.m_vm != tool::gsc::opcode::VMI_T8_36) {
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
	};
	class OPCodeInfoCheckClearParams : public OPCodeInfo {
	public:
		OPCodeInfoCheckClearParams() : OPCodeInfo(OPCODE_CheckClearParams, "CheckClearParams") {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;
			// don't create statement, we can ignore it
			context.m_lastOpCodeBase = -1;
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

			out << std::dec << negv << " (-0x" << std::hex << -negv << ")" << std::endl;

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

			out << std::endl;

			return 0;
		}
	};

	class OPCodeInfoCastBool : public OPCodeInfo {
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (context.m_runDecompiler) {
				if (context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS)) {
					context.PushASMCNode(context.PopASMCNode());
				}
				else {
					context.PushASMCNode(ASMCNodeConvertToBool(context.PopASMCNode(), context));
				}
			}

			out << std::endl;

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

			out << std::endl;

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

			out << std::endl;

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

			out << std::endl;

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

			out << std::endl;

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
					bool useReal{ (context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FormatterFlags::FFL_ANIM_REAL)) != 0 };
					// check if we can only display the anim name
					if (useReal && context.useAnimTree && strcmp(context.useAnimTree, str)) {
						context.m_exp.SetHandle(context.m_readerHandle);
						LOG_WARNING(
							"using more than one #animtree in {}::{}, can't use anim renderer, current: {} != {}",
							hashutils::ExtractTmp("namespace", context.m_exp.GetNamespace()), hashutils::ExtractTmp("function", context.m_exp.GetName()),
							context.useAnimTree ? context.useAnimTree : "<none>", str
						);
						useReal = false;
					}
					context.useAnimTree = str;

					if (useReal) {
						context.PushASMCNode(new ASMContextNodeAnimationRender(nullptr));
					}
					else {
						context.PushASMCNode(new ASMContextNodeAnimation(nullptr, (str ? str : "<error>")));
					}
				}

				out << "anim tree $" << (str ? str : "<error>") << std::endl;
			}
			else {
				WriteType intValue = context.Read<Type>(bytecode);


				if (context.m_runDecompiler) {
					bool isHex{};
					if constexpr (std::is_integral<Type>()) {
						//isHex = utils::IsProbablyHex((int64_t)intValue); // to test
					}

					context.PushASMCNode(new ASMContextNodeValue<WriteType>(intValue, m_valtype, isHex, true, true));
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
	};

	class OPCodeInfoDevBlockCall : public OPCodeInfo {
	public:
		OPCodeInfoDevBlockCall() : OPCodeInfo(OPCODE_IW_DevBlock, "IW_DevBlock") {}

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


			out << "delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << std::endl;

			return 0;
		}
	};

	class OPCodeInfoGetHash : public OPCodeInfo {
	private:
		const char* m_type;
		bool m_hash64;
		bool m_isInlined;
		bool m_canInline;
		bool m_localized;
	public:
		OPCodeInfoGetHash(OPCode id, const char* name, const char* type, bool hash64 = true, bool isInlined = false, bool canInline = false, bool localized = false)
			: m_type(type), m_hash64(hash64), m_isInlined(isInlined), m_canInline(canInline), m_localized(localized), OPCodeInfo(id, name) {
		}

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

				context.PushASMCNode(new ASMContextNodeHash(hash, m_isInlined || (m_canInline && objctx.m_vmInfo->IsScrHash(hash)), m_type, m_localized));
			}

			out << m_type << "\"";
			if (m_localized && objctx.gdctx.opt.LookupLocalizedFunc) {
				out << utils::FormattedString{ objctx.gdctx.opt.LookupLocalizedFunc(hash) };
			}
			else {
				out << hashutils::ExtractTmp("hash", hash);
			}
			out << "\" (" << m_type << std::hex << hash << ")" << std::endl;


			return 0;
		}
	};

	class OPCodeInfoJump : public OPCodeInfo {
	public:
		bool jump32;
		bool jumpNeg;
	public:
		OPCodeInfoJump(OPCode id, const char* name, bool jump32 = false, bool jumpNeg = false) : jump32(jump32), jumpNeg(jumpNeg), OPCodeInfo(id, name) {}

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
				case OPCODE_IW_JumpBack:
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

			if (jumpNeg) {
				if (jump32) {
					delta = -context.Read<int32_t>(bytecode);

					bytecode += 4;
				}
				else {
					delta = -(int32_t)context.Read<uint16_t>(bytecode);
					bytecode += 2;
				}
			}
			else {
				if (jump32) {
					delta = context.Read<int32_t>(bytecode);

					bytecode += 4;
				}
				else {
					delta = context.Read<int16_t>(bytecode);

					bytecode += 2;
				}
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
										if (!(context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
											ifpart = ASMCNodeConvertToBool(ifpart, context);
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
						if (!(context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
							node = ASMCNodeConvertToBool(node, context);
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

			out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")" << std::endl;

			return 0;
			//return m_id == OPCODE_Jump ? -2 : 0; // no code after jump
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
				out << "INVALID JUMP, TOO FAR: delta:" << std::hex << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")" << std::endl;
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
						out << "bad op code jumpexpr: " << std::hex << m_id << std::endl;
						return -1;
					}
				}
				ASMContextNode* node = context.PopASMCNode();
				locref.m_lateop.push_back(new ASMContextLocationOpOp(node, desc, priority));
			}

			locref.refs.insert(m_jumpLocation);

			out << "Jump ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc << " (delta:" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")" << std::endl;

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

			out << "Jump 0x" << std::hex << location << std::endl;

			return -2; // no code after that
		}
	};

	class OPCodeInfoVector : public OPCodeInfo {
		size_t count;
	public:
		OPCodeInfoVector(OPCode opcode, const char* desc, size_t count) : OPCodeInfo(opcode, desc), count(count) {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (context.m_runDecompiler) {
				if (count == 3) {
					auto* x = context.PopASMCNode();
					auto* y = context.PopASMCNode();
					auto* z = context.PopASMCNode();
					context.PushASMCNode(new ASMContextNodeVector(x, y, z));
				}
				else {
					ASMContextNode** nodes{ new ASMContextNodeVectorN::NodePtr[count] };
					for (size_t i = 0; i < count; i++) {
						nodes[i] = context.PopASMCNode();
					}
					context.PushASMCNode(new ASMContextNodeVectorN(nodes, count));
				}
			}

			out << std::endl;
			return 0;
		}
	};

	class OPCodeInfoGetVector : public OPCodeInfo {
		size_t count;
	public:
		OPCodeInfoGetVector(OPCode id, const char* name, size_t count) : OPCodeInfo(id, name), count(count) {}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<float>();
			}
			float vec[0x10];
			assert(count < 0x10);

			auto& bytecode = context.m_bcl;

			out << "(";
			for (size_t i = 0; i < count; i++) {
				if (i) out << ", ";
				vec[i] = context.Read<float>(bytecode);
				bytecode += 4;
				out << vec[i];
			}
			out << ")" << std::endl;

			if (context.m_runDecompiler) {
				if (count == 3) {
					context.PushASMCNode(new ASMContextNodeVector(vec[0], vec[1], vec[2]));
				}
				else {
					context.PushASMCNode(new ASMContextNodeVectorN(vec, count));
				}
			}

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

			out << "(" << x << ", " << y << ", " << z << ")" << std::endl;

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
	};

	class OPCodeInfoSetVariableField : public OPCodeInfo {
	public:
		OPCodeInfoSetVariableField(OPCode id, const char* name) : OPCodeInfo(id, name) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;

			if (context.m_runDecompiler) {
				// iw8 check
				if (context.PeekASMCNode()->m_type != ASMContextNodeType::TYPE_DELTA) {
					context.PushASMCNode(new ASMContextNodeLeftRightOperator(context.GetFieldIdASMCNode(), context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
				}
				context.CompleteStatement();
			}

			return 0;
		}
	};

	class OPCodeT9SetVariableFieldFromEvalArrayRef : public OPCodeInfo {
	public:
		OPCodeT9SetVariableFieldFromEvalArrayRef() : OPCodeInfo(OPCODE_T9_SetVariableFieldFromEvalArrayRef, "SetVariableFieldFromEvalArrayRef") {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;

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
	};

	class OPCodeT9Iterator : public OPCodeInfo {
	public:
		const char* m_op;
		ASMContextNodeType m_ittype;
		OPCodeT9Iterator(OPCode id, const char* name, const char* op, ASMContextNodeType ittype) : OPCodeInfo(id, name), m_op(op), m_ittype(ittype) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;
			if (context.m_runDecompiler) {
				ASMContextNode* iterator = context.PopASMCNode();

				ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_op, false, m_ittype);

				node->AddParam(iterator);

				context.PushASMCNode(node);
			}

			return 0;
		}
	};

	class OPCodeT9GetVarRef : public OPCodeInfo {
	public:
		OPCodeT9GetVarRef() : OPCodeInfo(OPCODE_T9_GetVarRef, "GetVarRef") {}
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;
			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeRef("&", context.GetFieldIdASMCNode()->Clone()));
			}

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
	};

	enum ReadObjectType {
		ROT_ID,
		ROT_TOKEN
	};

	class OPCodeInfoClearFieldVariable : public OPCodeInfo {
	private:
		bool m_stack;
		bool m_isref;
		bool m_useref;
		ReadObjectType readtype;
	public:
		OPCodeInfoClearFieldVariable(OPCode id, const char* name, bool stack, bool isref, ReadObjectType readtype = ROT_ID, bool useref = false)
			: OPCodeInfo(id, name), m_stack(stack), m_isref(isref), readtype(readtype), m_useref(useref) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			ASMContextNode* fieldNameNode;
			if (!m_stack) {
				if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
					context.Aligned<uint32_t>();
				}
				auto& ref = context.m_bcl;

				if (m_isref) {
					int lvar = context.Read<byte>(ref++);

					uint64_t name;
					if (lvar >= context.m_localvars.size()) {
						name = hash::HashT89Scr("<error>");
						out << "bad lvar stack: 0x" << std::hex << (int)lvar << " ";
					}
					else {
						name = context.m_localvars[lvar].name;
						context.m_localvars_ref[name]++;
					}

					fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;

					out << hashutils::ExtractTmp("var", name) << std::endl;
				}
				else {
					if (readtype == ROT_ID) {
						uint64_t fieldName;
						if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
							fieldName = context.Read<uint64_t>(ref);
							ref += 8;
						}
						else {
							fieldName = context.Read<uint32_t>(ref);
							ref += 4;
						}
						fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(fieldName) : nullptr;
						out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;
					}
					else if (readtype == ROT_TOKEN) {
						const char* name{ objctx.GetTokenValue(context.Read<uint32_t>(ref)) };
						ref += 4;
						fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;
						out << "." << name << std::endl;
					}
					else {
						throw std::runtime_error(std::format("OPCodeInfoSetGlobalObjectFieldVariable: Can't read {}", (int)readtype));
					}
				}
			}
			else {
				out << std::endl;
			}

			if (context.m_runDecompiler) {
				ASMContextNode* nameNode = m_stack ? context.PopASMCNode() : fieldNameNode;
				ASMContextNode* fieldAccessNode = new ASMContextNodeLeftRightOperator(m_useref ? context.GetFieldIdASMCNode() : context.GetObjectIdASMCNode(), nameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);
				context.PushASMCNode(new ASMContextNodeLeftRightOperator(fieldAccessNode, new ASMContextNodeValue<const char*>("undefined", TYPE_GET_UNDEFINED), " = ", PRIORITY_SET, TYPE_SET));
				context.CompleteStatement();
			}

			return 0;
		}
	};
	class OPCodeInfoSetGlobalObjectFieldVariable : public OPCodeInfo {
	private:
		const char* gvarName;
		ReadObjectType readtype;
	public:
		OPCodeInfoSetGlobalObjectFieldVariable(OPCode id, const char* name, const char* gvarName = nullptr, ReadObjectType readtype = ROT_ID) : OPCodeInfo(id, name), gvarName(gvarName), readtype(readtype) {}

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
			ASMContextNode* fieldNameNode;

			if (readtype == ROT_ID) {
				uint64_t fieldName;
				if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
					fieldName = context.Read<uint64_t>(base2);
					base2 += 8;
				}
				else {
					fieldName = context.Read<uint32_t>(base2);
					base2 += 4;
				}
				fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(fieldName) : nullptr;
				out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;
			}
			else if (readtype == ROT_TOKEN) {
				const char* name{ objctx.GetTokenValue(context.Read<uint32_t>(base2)) };
				base2 += 4;
				fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;
				out << "." << name << std::endl;
			}
			else {
				throw std::runtime_error(std::format("OPCodeInfoSetGlobalObjectFieldVariable: Can't read {}", (int)readtype));
			}


			if (context.m_runDecompiler) {
				context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
				auto* ref = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), fieldNameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);

				context.PushASMCNode(new ASMContextNodeLeftRightOperator(ref, context.PopASMCNode(), " = ", PRIORITY_SET, TYPE_SET));
				context.CompleteStatement();
			}

			return 0;
		}
	};
	class OPCodeInfoEvalGlobalObjectFieldVariable : public OPCodeInfo {
	private:
		ReadObjectType readtype;
		const char* gvarName;
		bool push;
	public:
		OPCodeInfoEvalGlobalObjectFieldVariable(OPCode id, const char* name, const char* gvarName = nullptr, bool push = true, ReadObjectType readtype = ROT_ID)
			: OPCodeInfo(id, name), gvarName(gvarName), push(push), readtype(readtype) {
		}

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
			ASMContextNode* fieldNameNode;
			if (readtype == ROT_ID) {
				uint64_t fieldName;
				if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
					fieldName = context.Read<uint64_t>(base2);
					base2 += 8;
				}
				else {
					fieldName = context.Read<uint32_t>(base2);
					base2 += 4;
				}
				fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(fieldName) : nullptr;
				out << "." << hashutils::ExtractTmp("var", fieldName) << std::endl;
			}
			else if (readtype == ROT_TOKEN) {
				const char* name{ objctx.GetTokenValue(context.Read<uint32_t>(base2)) };
				base2 += 4;
				fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;
				out << "." << name << std::endl;
			}
			else {
				throw std::runtime_error(std::format("OPCodeInfoSetGlobalObjectFieldVariable: Can't read {}", (int)readtype));
			}

			if (context.m_runDecompiler) {
				context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
				auto* node = new ASMContextNodeLeftRightOperator(context.GetObjectIdASMCNode(), fieldNameNode, ".", PRIORITY_ACCESS, TYPE_ACCESS);
				if (push) {
					context.PushASMCNode(node);
				}
				else {
					context.SetFieldIdASMCNode(node);
				}
			}

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
	};

	class OPCodeInfoEvalFieldVariable : public OPCodeInfo {
	private:
		bool m_stack;
		bool m_ref;
		ReadObjectType readtype;
	public:
		OPCodeInfoEvalFieldVariable(OPCode id, const char* name, bool stack, bool ref, ReadObjectType readtype = ROT_ID)
			: OPCodeInfo(id, name), m_stack(stack), m_ref(ref), readtype(readtype) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			ASMContextNode* fieldNameNode{};
			if (!m_stack) {
				if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
					context.Aligned<uint32_t>();
				}

				auto& ref = context.m_bcl;

				if (readtype == ROT_ID) {
					uint64_t name;
					if (objctx.m_vmInfo->flags & VmFlags::VMF_HASH64) {
						name = context.Read<uint64_t>(ref);
						ref += 8;
					}
					else {
						name = context.Read<uint32_t>(ref);
						ref += 4;
					}
					fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;

					out << hashutils::ExtractTmp("var", name) << std::endl;
				}
				else if (readtype == ROT_TOKEN) {
					const char* name{ objctx.GetTokenValue(context.Read<uint32_t>(ref)) };
					ref += 4;
					fieldNameNode = context.m_runDecompiler ? new ASMContextNodeIdentifier(name) : nullptr;
					out << "." << name << std::endl;
				}
				else {
					throw std::runtime_error(std::format("OPCodeInfoSetGlobalObjectFieldVariable: Can't read {}", (int)readtype));
				}
			}
			else {
				out << std::endl;
			}

			if (context.m_runDecompiler) {
				ASMContextNode* node = new ASMContextNodeLeftRightOperator(
					context.GetObjectIdASMCNode(),
					m_stack ? context.PopASMCNode() : fieldNameNode,
					".", PRIORITY_ACCESS, TYPE_ACCESS
				);

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
					out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
	};

	class OPCodeInfoSetLocalVariableCached : public OPCodeInfo {
	private:
		int localId;
		bool m_stack;
	public:
		OPCodeInfoSetLocalVariableCached(OPCode id, const char* name, bool stack, int localId = -1) : OPCodeInfo(id, name), m_stack(stack), localId(localId) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			int lvar;

			if (localId >= 0) {
				lvar = localId;
			}
			else {
				lvar = (int)context.Read<byte>(context.m_bcl++);
			}

			uint64_t name;

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
	};
	class OPCodeInfoClearLocalVariableCached : public OPCodeInfo {
		int localId;
	public:
		OPCodeInfoClearLocalVariableCached(OPCode id, const char* name, int localId = -1) : OPCodeInfo(id, name), localId(localId) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			int lvar;
			if (localId >= 0) {
				lvar = localId;
			}
			else {
				lvar = (int)context.Read<byte>(context.m_bcl++);
			}


			uint64_t name;

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
					out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
				}
				else {
					name = context.m_localvars[lvar].name;
					context.m_localvars_ref[name]++;
					out << hashutils::ExtractTmp("var", name) << std::endl;
				}
			}
			else {
				out << std::endl;
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
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
	};

	class OPCodeInfoEvalLocalVariableRefCached : public OPCodeInfo {
		int m_localid;
	public:
		OPCodeInfoEvalLocalVariableRefCached(OPCode id, const char* name, int localid = -1) : OPCodeInfo(id, name), m_localid(localid) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			uint64_t name;
			int lvar;

			if (m_localid < 0) {
				lvar = (int)context.Read<byte>(context.m_bcl++);
			}
			else {
				lvar = m_localid;
			}

			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
	};

	class OPCodeInfoEvalLocalArrayCached : public OPCodeInfo {
		bool ref;
	public:
		OPCodeInfoEvalLocalArrayCached(OPCode id, const char* name, bool ref) : OPCodeInfo(id, name), ref(ref) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			int lvar = (int)context.Read<byte>(context.m_bcl++);

			uint64_t name;
			if (lvar >= context.m_localvars.size()) {
				name = hash::HashT89Scr("<error>");
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
			}
			else {
				name = context.m_localvars[lvar].name;
				context.m_localvars_ref[name]++;
				out << hashutils::ExtractTmp("var", name) << std::endl;
			}

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeIdentifier(name));

				ASMContextNode* arrayNode = context.PopASMCNode();
				ASMContextNode* accessNode = new ASMContextNodeArrayAccess(arrayNode, context.PopASMCNode());
				if (ref) {
					context.SetFieldIdASMCNode(new ASMContextNodeIdentifier(name));
				}
				else {
					context.PushASMCNode(accessNode);
				}
			}

			return 0;
		}
	};
	class OPCodeInfoEvalLocalObjectCached : public OPCodeInfo {
	public:
		OPCodeInfoEvalLocalObjectCached() : OPCodeInfo(OPCODE_IW_EvalLocalVariableObjectCached, "IW_EvalLocalVariableObjectCached") {
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
			out << "(0x" << std::hex << lvar << ")" << std::endl;

			if (context.m_runDecompiler) {
				context.SetObjectIdASMCNode(new ASMContextNodeIdentifier(name));
			}

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

			out << std::endl;

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
	};

	class OPCodeInfoCreateArray : public OPCodeInfo {
	public:
		OPCodeInfoCreateArray(OPCode id, const char* name) : OPCodeInfo(id, name) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			out << std::endl;

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeArrayBuild());
			}

			return 0;
		}
	};

	class OPCodeInfoAddToArray : public OPCodeInfo {
		bool m_hasKey;
	public:
		OPCodeInfoAddToArray(OPCode id, const char* name, bool hasKey) : OPCodeInfo(id, name), m_hasKey(hasKey) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			out << std::endl;

			if (context.m_runDecompiler) {
				ASMContextNode* key = m_hasKey ? context.PopASMCNode() : nullptr;
				ASMContextNode* value = context.PopASMCNode();
				ASMContextNode* arrayVal = context.PeekASMCNode();

				if (arrayVal->m_type == TYPE_ARRAY_BUILD) {
					// we are building an array, we can add the value
					static_cast<ASMContextNodeArrayBuild*>(arrayVal)->AddValue(key, value, objctx.m_vmInfo->HasFlag(VmFlags::VMF_INV_ADD_TO_OBJECT));
				}
				else {
					ASMContextNodeMultOp* node = new ASMContextNodeMultOp("$addtoarray", false);


					node->AddParam(context.PopASMCNode());
					if (key) node->AddParam(key);
					node->AddParam(value);

					context.PushASMCNode(node);
				}
			}

			return 0;
		}
	};

	class OPCodeInfoCreateStruct : public OPCodeInfo {
	public:
		OPCodeInfoCreateStruct(OPCode id, const char* name) : OPCodeInfo(id, name) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			out << std::endl;

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeStructBuild());
			}

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

			out << std::endl;

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
	};

	class OPCodeInfoCastFieldObject : public OPCodeInfo {
	public:
		OPCodeInfoCastFieldObject(OPCode id, const char* name) : OPCodeInfo(id, name) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			out << std::endl;

			if (context.m_runDecompiler) {
				context.SetObjectIdASMCNode(context.PopASMCNode());
			}

			return 0;
		}
	};

	class OPCodeInfoPreScriptCall : public OPCodeInfo {
	public:
		OPCodeInfoPreScriptCall(OPCode id, const char* name) : OPCodeInfo(id, name) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {

			out << std::endl;

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeValue<const char*>("<emptypos_prescriptcall>", TYPE_PRECODEPOS));
			}

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
				}
				else {
					// special node to print end ref
					auto* ref = new ASMContextNodeValue<const char*>("<END>", TYPE_END);
					context.PushASMCNode(ref);
				}
				context.CompleteStatement();
			}
			out << std::endl;
			int64_t rloc = context.FunctionRelativeLocation();
			for (const auto& loc : context.m_locs) {
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
			out << std::endl;

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
			out << std::endl;
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
			out << std::endl;
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
			out << std::endl;
			LOG_ERROR("Can't decompile skip operator {}", m_operatorName);
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
			out << std::endl;
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
	};

	class OPCodeInfoInvalidOpCode : public OPCodeInfo {
	public:
		OPCodeInfoInvalidOpCode() : OPCodeInfo(OPCODE_InvalidOpCode, "InvalidOpCode") {
		}
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;
			if (context.m_runDecompiler) {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp("InvalidOpCode", false);

				node->AddParam(new ASMContextNodeValue<uint16_t>(value, TYPE_VALUE, true));
				// convert it to statement
				context.PushASMCNode(node);
				context.CompleteStatement();
			}
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
			out << std::endl;

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
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
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
	};

	class OPCodeT9SetFieldVariableFromObjectFromRef : public OPCodeInfo {
	public:
		OPCodeT9SetFieldVariableFromObjectFromRef() : OPCodeInfo(OPCODE_T9_SetFieldVariableFromObjectFromRef, "T9_SetFieldVariableFromObjectFromRef") {
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
	};

	class OPCodeInfoFunctionOperator : public OPCodeInfo {
		const char* m_operatorName;
		bool m_hasSelf;
		bool m_complete;
		bool m_canHaveArg2;
		bool m_isBool;
		bool m_noParenthesis;
		ASMContextNodeType m_type;

	public:
		OPCodeInfoFunctionOperator(OPCode id, const char* name, const char* operatorName, bool hasSelf = false, bool complete = true, bool canHaveArg2 = false, bool isBool = false, bool noParenthesis = false, ASMContextNodeType type = TYPE_STATEMENT) : OPCodeInfo(id, name),
			m_operatorName(operatorName), m_hasSelf(hasSelf), m_complete(complete), m_canHaveArg2(canHaveArg2), m_isBool(isBool), m_noParenthesis(noParenthesis), m_type(type) {
		}
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (context.m_runDecompiler) {
				ASMContextNodeMultOp* node = new ASMContextNodeMultOp(m_operatorName, m_hasSelf, m_type, m_isBool, m_noParenthesis);
				if (m_hasSelf) {
					node->AddParam(context.PopASMCNode());
				}
				node->AddParam(context.PopASMCNode());

				context.PushASMCNode(node);
				if (m_complete) {
					context.CompleteStatement();
				}
			}
			out << std::endl;
			return 0;
		}
	};

	class OPCodeInfoNotify : public OPCodeInfo {
	public:
		OPCodeInfoNotify(OPCode id, const char* name) : OPCodeInfo(id, name) {
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
			out << std::endl;
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
			out << std::endl;
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
			bool readParam{ !objctx.m_vmInfo->HasFlag(VmFlags::VMF_CALL_NO_PARAMS) || (objctx.m_vmInfo->HasFlag(VmFlags::VMF_IW_CALLS) && m_hasParam) };
			if (readParam) {
				params = context.Read<byte>(context.m_bcl++);
			}
			else {
				switch (m_id) {
				case OPCODE_IW_CallBuiltinFunction0:
				case OPCODE_IW_CallBuiltinMethod0:
					// because it makes so much sense to name them with a bloody 2 at the end
				case OPCODE_IW_ScriptFunctionCallFar2:
				case OPCODE_IW_LocalCall2:
				case OPCODE_IW_LocalMethodCall2:
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
				if (m_delta == 4 && objctx.m_vmInfo->HasFlag(VmFlags::VMF_FUNC_REF_24)) {
					context.m_bcl += 3;
				}
				else {
					context.m_bcl += m_delta;
				}

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
				case OPCODE_IW_LocalThreadCall:
				case OPCODE_IW_LocalMethodThreadCall:
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
				case OPCODE_IW_ScriptFunctionCallFar2:
				case OPCODE_CallBuiltinFunction:
				case OPCODE_IW_CallBuiltinFunction0:
				case OPCODE_IW_CallBuiltinFunction1:
				case OPCODE_IW_CallBuiltinFunction2:
				case OPCODE_IW_CallBuiltinFunction3:
				case OPCODE_IW_CallBuiltinFunction4:
				case OPCODE_IW_CallBuiltinFunction5:
				case OPCODE_IW_LocalCall:
				case OPCODE_IW_LocalCall2:
				case OPCODE_IW_LocalMethodCall:
				case OPCODE_IW_LocalMethodCall2:
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

					if (readParam && context.m_stack.size() && context.PeekASMCNode()->m_type == TYPE_PRECODEPOS) {
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
			out << std::endl;

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
				// local calls: 24 bits
				// script functions: 32 bits

				if (m_delta == 4 && objctx.m_vmInfo->HasFlag(VmFlags::VMF_FUNC_REF_24)) {
					context.m_bcl += 3;
				}
				else {
					context.m_bcl += m_delta;
				}


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
			out << std::endl;

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
				out << '"' << utils::FormattedString(str) << '"' << std::endl;
				if (context.m_runDecompiler) {
					if (m_istr) {
						context.PushASMCNode(new ASMContextNodeString(str, "&"));
					}
					else {
						context.PushASMCNode(new ASMContextNodeString(str));
					}
				}
			}
			else {
				// probably only dev blocks
				out << "bad str stack: 0x" << std::hex << ref << std::endl;
				if (context.m_runDecompiler) {
					context.PushASMCNode(new ASMContextNodeString((context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FormatterFlags::FFL_NOERROR_STR)) ? "" : "<unknown string>", m_istr ? "&" : nullptr));
				}
			}


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

				out << "%" << utils::FormattedString(str1) << "::" << utils::FormattedString(str2);

				bool useReal{ (context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FormatterFlags::FFL_ANIM_REAL)) != 0 };
				// check if we can only display the anim name
				if (useReal && context.useAnimTree && strcmp(context.useAnimTree, str1)) {
					context.m_exp.SetHandle(context.m_readerHandle);
					LOG_WARNING(
						"using more than one animtree in {}::{}, can't use anim renderer, current: {} != {}",
						hashutils::ExtractTmp("namespace", context.m_exp.GetNamespace()), hashutils::ExtractTmp("function", context.m_exp.GetName()),
						context.useAnimTree ? context.useAnimTree : "<none>", str1
					);
					useReal = false;
				}
				context.useAnimTree = str1;

				if (context.m_runDecompiler) {
					if (useReal) {
						context.PushASMCNode(new ASMContextNodeAnimationRender(str2));
					}
					else {
						context.PushASMCNode(new ASMContextNodeAnimation(str1, str2));

					}
				}
			}
			else {
				str1 = "";
				ref1 = 0;
				ref2 = context.Read<byte>(base);
				str2 = objctx.GetStringValueOrError(ref2, context.ScriptAbsoluteLocation(base), nullptr);
				base++;
				out << "$" << utils::FormattedString(str2);

				bool useReal{ (context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FormatterFlags::FFL_ANIM_REAL)) != 0 };
				// check if we can only display the anim name
				if (useReal && context.useAnimTree && strcmp(context.useAnimTree, str2)) {
					context.m_exp.SetHandle(context.m_readerHandle);
					LOG_WARNING(
						"using more than one #animtree in {}::{}, can't use anim renderer, current: {} != {}",
						hashutils::ExtractTmp("namespace", context.m_exp.GetNamespace()), hashutils::ExtractTmp("function", context.m_exp.GetName()),
						context.useAnimTree ? context.useAnimTree : "<none>", str2
					);
					useReal = false;
				}
				context.useAnimTree = str2;

				if (context.m_runDecompiler) {
					if (useReal) {
						context.PushASMCNode(new ASMContextNodeAnimationRender(nullptr));
					}
					else {
						context.PushASMCNode(new ASMContextNodeAnimation(str1, str2));

					}
				}

			}

			if (!str1) {
				out << "bad str stack: 0x" << std::hex << ref1 << " ";
				str1 = "<unknown>";
			}
			if (!str2) {
				out << "bad str stack: 0x" << std::hex << ref2 << " ";
				str2 = "<unknown>";
			}
			out << std::endl;

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
			out << std::endl;

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
	};

	class OPCodeT9EvalFieldVariableFromGlobalObject : public OPCodeInfo {
	public:
		OPCodeT9EvalFieldVariableFromGlobalObject() : OPCodeInfo(OPCODE_T9_EvalFieldVariableFromGlobalObject, "T9_EvalFieldVariableFromGlobalObject") {
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
	};

	class OPCodeInfoGetLocalVar : public OPCodeInfo {
	public:
		using OPCodeInfo::OPCodeInfo;

		int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			int lvar = (int)context.Read<byte>(context.m_bcl++);

			if (lvar >= context.m_localvars.size()) {
				out << "bad lvar stack: 0x" << std::hex << (int)lvar << std::endl;
			}
			else {
				uint64_t name = context.m_localvars[lvar].name;
				out << hashutils::ExtractTmp("var", name) << std::endl;
				context.m_localvars_ref[name]++;
			}

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
				<< context.FunctionRelativeLocation(utils::Aligned<int64_t>(baseTable)) << " cases: " << cases << std::endl;

			ASMContextNodeSwitchPreCompute* node = nullptr;
			if (context.m_runDecompiler) {
				node = new ASMContextNodeSwitchPreCompute(context.PopASMCNode());
			}

			for (size_t c = 1; c <= cases; c++) {
				uint64_t caseValue;
				byte* caseLoc;
				int64_t caseDelta;
				byte* endBase;
				if (context.m_vm >= VMI_T8_34) {
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
						out << '"' << utils::FormattedString(cv) << '"';
						if (node) {
							node->m_cases.push_back({ new ASMContextNodeString(cv), caseRLoc });
						}
					}
					else if (caseValue >= 0x100000000LL || (context.m_vm == VMI_T8_31 && caseValue >= 0x10000LL)) {
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
					<< caseRLoc << std::endl;

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
	};

	class OPCodeInfoGetPositionRef : public OPCodeInfo {
	public:
		OPCodeInfoGetPositionRef() : OPCodeInfo(OPCODE_IW_GetPositionRef, "IW_GetPositionRef") {}

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
				<< context.FunctionRelativeLocation(refLocation) << std::endl;

			tool::gsc::opcode::ASMContextLocation& loc = context.PushLocation(refLocation);

			// add ref so this code can be handled
			loc.refs.insert(opcodeLocation);

			if (context.m_runDecompiler) {
				// push location
				context.PushASMCNode(new ASMContextNodeCodeRef(loc.rloc));
			}


			return 0;
		}
	};

	namespace {
		void GetSwitchInfo(VmInfo* info, size_t* caseSize, size_t* caseValSize, bool* hasType) {
			if (info->HasFlag(VmFlags::VMF_SWITCH_32)) {
				if (caseSize) *caseSize = 7;
				if (caseValSize) *caseValSize = 4;
				if (hasType) *hasType = false;
			}
			else {
				if (caseSize) *caseSize = 12;
				if (caseValSize) *caseValSize = 8;
				if (hasType) *hasType = true;
			}
		}

	}

	class OPCodeInfoIWSwitch : public OPCodeInfo {
	public:
		OPCodeInfoIWSwitch() : OPCodeInfo(OPCODE_IW_Switch, "IW_Switch") {}

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
				<< context.FunctionRelativeLocation(baseTable) << " cases: " << cases << std::endl;

			ASMContextNodeSwitchPreCompute* node = nullptr;
			if (context.m_runDecompiler) {
				node = new ASMContextNodeSwitchPreCompute(context.PopASMCNode());
			}
			size_t caseSize;
			size_t caseValSize;
			bool hasType;
			GetSwitchInfo(objctx.m_vmInfo, &caseSize, &caseValSize, &hasType);

			for (size_t c = 1; c <= cases; c++) {

				auto* basecase = baseLocation + caseSize * (c - 1);
				context.m_bcl = basecase + caseSize;
				union CaseValue {
					int64_t integer;
					uint32_t str;
					uint64_t hash;
					uint32_t unkb; // i32
				};

				CaseValue val;

				context.Read<byte>((byte*)&val, caseValSize, basecase);
				int32_t caseDelta = (int32_t)(((context.Read<int32_t>(basecase + caseValSize) << 8) >> 8) + caseValSize); // i24
				byte type;

				if (hasType) {
					type = context.Read<byte>(basecase + caseSize - 1);
				}

				context.WritePadding(out);

				auto caseRLoc = context.PushLocation(&basecase[caseDelta]).rloc;

				if (c == cases && (!val.hash || val.hash == 0xdefdefdefdefdef0 || val.hash == 0xDDEFDEFFDEFFDEFF || val.str == 0xdefdef)) {
					out << "default";
					if (node) {
						node->m_cases.push_back({ nullptr, caseRLoc });
					}
				}
				else if (hasType) {
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
							out << '"' << utils::FormattedString(str) << '"';
							if (node) {
								outputStr = new ASMContextNodeString(str);
							}
						}
						else {
							// probably only dev blocks
							out << "bad str stack: 0x" << std::hex << val.hash;
							if (node) {
								outputStr = new ASMContextNodeString((context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FormatterFlags::FFL_NOERROR_STR)) ? "" : "<unknown string>");
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
				else {
					// no type
					uint32_t floc = (uint32_t)(basecase - context.m_gscReader.Ptr());
					const char* casestr{ objctx.GetStringValueByLoc(floc) };

					if (casestr) {
						out << "case \"" << utils::FormattedString(casestr) << '"';
						if (node) {
							node->m_cases.push_back({ new ASMContextNodeString(casestr), caseRLoc });
						}
					}
					else {
						out << "case 0x" << std::hex << val.str;

						if (node) {
							node->m_cases.push_back({ new ASMContextNodeValue<int64_t>(val.str, TYPE_VALUE), caseRLoc });
						}
					}
				}

				out << ": ." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1)
					<< caseRLoc << std::endl;

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
			context.WritePadding(out) << "end" << std::endl;
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

			if (context.m_vm >= VMI_T8_34) {
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

			out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << std::hex << rloc << std::endl;

			return 0;
		}
	};
	class OPCodeInfoIWEndSwitch : public OPCodeInfo {
	public:
		OPCodeInfoIWEndSwitch() : OPCodeInfo(OPCODE_IW_EndSwitch, "IW_EndSwitch") {}

		int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			int32_t jumpLocation = context.FunctionRelativeLocation(context.m_bcl - 2);
			auto& baseCount = context.m_bcl;

			uint16_t count = context.Read<uint16_t>(baseCount);

			size_t caseSize;
			GetSwitchInfo(objctx.m_vmInfo, &caseSize, nullptr, nullptr);

			baseCount += 2 + caseSize * count;

			auto rloc = context.FunctionRelativeLocation();

			if (context.m_runDecompiler) {
				context.PushASMCNode(new ASMContextNodeJumpOperator("endswitch", nullptr, rloc, TYPE_JUMP_ENDSWITCH, jumpLocation));
				context.CompleteStatement();
			}

			out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << std::hex << rloc << std::endl;

			return 0;
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
					if (cb && !(context.m_opt.m_formatter->HasFlag(tool::gsc::formatter::FFL_NO_BOOL_ANALYSIS))) {
						return ASMCNodeConvertToBool(context.PopASMCNode(), context);
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
			out << std::endl;
			return 0;
		}
	};

	static thread_local int tryCatchId{};

	class OPCodeInfoTry : public OPCodeInfo {
	public:
		OPCodeInfoTry() : OPCodeInfo(OPCODE_T10_Try, "T10_Try") {
		}

		int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<int16_t>();
			}
			auto& base = context.m_bcl;

			int16_t delta = context.Read<int16_t>(base);

			byte* newLoc = &context.m_bcl[delta];
			auto& locref = context.PushLocation(newLoc);

			out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc
				<< "(" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")" << std::endl;

			if (context.m_runDecompiler) {
				int tid{ tryCatchId++ };
				auto* ref = new ASMContextNodeValue<const char*>("Try", TYPE_TRY_PRECOMPUTE);
				// TODO: use tid
				ref->m_priority = PRIORITY_INST;
				context.PushASMCNode(ref);
				context.CompleteStatement();
				locref.m_lateop.push_back(new ASMContextLocationOpCatch(tid));
			}
			return 0;
		}
	};
	class OPCodeInfoTryEnd : public OPCodeInfo {
	public:
		OPCodeInfoTryEnd() : OPCodeInfo(OPCODE_T10_TryEnd, "T10_TryEnd") {
		}

		int Dump(std::ostream& out, uint16_t v, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			if (objctx.m_vmInfo->HasFlag(VmFlags::VMF_ALIGN)) {
				context.Aligned<int16_t>();
			}
			auto& base = context.m_bcl;

			int16_t delta = context.Read<int16_t>(base);

			byte* newLoc = &context.m_bcl[delta];
			auto& locref = context.PushLocation(newLoc);

			out << "." << std::hex << std::setfill('0') << std::setw(sizeof(int32_t) << 1) << locref.rloc
				<< "(" << (delta < 0 ? "-" : "") << "0x" << (delta < 0 ? -delta : delta) << ")" << std::endl;

			// Todo: do we need it?
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
			out << std::endl;
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

			out << "count:" << (int)count << std::endl;

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
	};

	class OPCodeInfoCountWaittill : public OPCodeInfo {
		const char* m_op;
		byte m_added;
	public:

		OPCodeInfoCountWaittill(OPCode id, const char* name, const char* op, byte added = 0) : OPCodeInfo(id, name), m_op(op), m_added(added) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			byte count = context.Read<byte>(context.m_bcl++) + m_added;

			out << "count:" << (int)count << std::endl;

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
			out << std::endl;

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
	};

	class OPCodeInfoSingleFunc : public OPCodeInfo {
	public:
		const char* m_op;
		bool m_pushReturn;
		bool m_isBoolVal;
		OPCodeInfoSingleFunc(OPCode id, const char* name, const char* op, bool pushReturn, bool isBoolVal = false) : OPCodeInfo(id, name), m_op(op), m_pushReturn(pushReturn), m_isBoolVal(isBoolVal) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << std::endl;

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
			out << std::endl;
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
	};

	class OPCodeInfoDeltaVal : public OPCodeInfo {
		bool m_ref;
	public:
		const char* m_op;
		OPCodeInfoDeltaVal(OPCode id, const char* name, const char* op, bool ref) : OPCodeInfo(id, name), m_op(op), m_ref(ref) {
		}

		int Dump(std::ostream& out, uint16_t value, ASMContext& context, tool::gsc::T8GSCOBJContext& objctx) const override {
			out << m_op << std::endl;

			if (context.m_runDecompiler) {
				if (context.m_fieldId) {
					context.PushASMCNode(new ASMContextNodeOp1(m_op, false, context.GetFieldIdASMCNode(), TYPE_DELTA));
				}
				else {
					context.PushASMCNode(new ASMContextNodeOp1(m_op, false, context.PopASMCNode(), TYPE_DELTA));
				}
				if (!m_ref) {
					context.CompleteStatement();
				}
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

			out << m_op << std::endl;

			if (context.m_runDecompiler) {
				auto* op = new ASMContextNodeIdentifier(name);
				context.PushASMCNode(new ASMContextNodeOp1(m_op, false, op, TYPE_DELTA));
				context.CompleteStatement();
			}
			return 0;
		}
	};

	namespace {
		const OPCodeInfoUnknown g_unknownOpcode{ OPCODE_Undefined, "Undefined" };
	}

	void RegisterOpCodeHandler(const OPCodeInfo* info) {
		g_opcodeHandlerMap[info->m_id] = info;
	}

	const OPCodeInfo* LookupOpCode(uint64_t vm, Platform platform, uint16_t opcode) {
		// build map
		RegisterOpCodes();

		VmInfo* info;

		if (!(IsValidVm(vm, info))) {
			return &g_unknownOpcode;
		}

		auto ref = info->opcodemap.find(opcode);

		if (ref == info->opcodemap.end()) {
			return &g_unknownOpcode;
		}

		auto ref2 = ref->second.find(info->RemapSamePlatform(platform));

		if (ref2 == ref->second.end()) {
			return &g_unknownOpcode;
		}

		const auto refHandler = g_opcodeHandlerMap.find(ref2->second);

		if (refHandler == g_opcodeHandlerMap.end()) {
			return &g_unknownOpcode;
		}

		return refHandler->second;
	}

	void RegisterOpCodes() {
		static std::once_flag f{};

		std::call_once(f, [] {
			// unknown
			RegisterOpCodeHandler(&g_unknownOpcode);

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
			RegisterOpCodeHandler(new OPCodeInfoFuncHash(OPCODE_T7_ProfileStart, "T7_ProfileStart", true, "profilestart"));
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
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_IW_Jump32, "IW_Jump32", true));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_Jump, "Jump"));
			RegisterOpCodeHandler(new OPCodeInfoJump(OPCODE_IW_JumpBack, "IW_JumpBack", false, true));
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
			RegisterOpCodeHandler(new OPCodeInfoAddToArray(OPCODE_AddToArray, "AddToArray", true));
			RegisterOpCodeHandler(new OPCodeInfoAddToArray(OPCODE_IW_AppendToArray, "IW_AppendToArray", false));
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
			RegisterOpCodeHandler(new OPCodeInfoClearFieldVariable(OPCODE_IW_ClearFieldVariableToken, "IW_ClearFieldVariableToken", false, false, ROT_TOKEN));

			RegisterOpCodeHandler(new OPCodeInfoEvalSelfFieldVariable(OPCODE_EvalSelfFieldVariable, "EvalSelfFieldVariable"));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_EvalGlobalObjectFieldVariable, "EvalGlobalObjectFieldVariable"));
			RegisterOpCodeHandler(new OPCodeInfoCastAndEvalFieldVariable(OPCODE_CastAndEvalFieldVariable, "CastAndEvalFieldVariable"));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariable, "EvalFieldVariable", false, false));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableRef, "EvalFieldVariableRef", false, true));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableOnStack, "EvalFieldVariableOnStack", true, false));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_EvalFieldVariableOnStackRef, "EvalFieldVariableOnStackRef", true, true));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_IW_EvalFieldVariableToken, "IW_EvalFieldVariableToken", false, false, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldVariable(OPCODE_IW_EvalFieldVariableTokenRef, "IW_EvalFieldVariableTokenRef", false, true, ROT_TOKEN));

			// localvar related
			RegisterOpCodeHandler(new OPCodeInfoCheckClearParams());
			RegisterOpCodeHandler(new OPCodeInfoSafeCreateLocalVariables());
			RegisterOpCodeHandler(new OPCodeInfoRemoveLocalVariables());
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_EvalLocalVariableCached, "EvalLocalVariableCached"));

			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_EvalLocalVariableCachedSafe, "EvalLocalVariableCachedSafe"));
			RegisterOpCodeHandler(new OPCodeInfoSetLocalVariableCached(OPCODE_SetLocalVariableCachedOnStack, "SetLocalVariableCachedOnStack", true));
			RegisterOpCodeHandler(new OPCodeInfoSetLocalVariableCached(OPCODE_SetLocalVariableCached, "SetLocalVariableCached", false));
			RegisterOpCodeHandler(new OPCodeInfoSetLocalVariableCached(OPCODE_IW_SetLocalVariableCached0, "IW_SetLocalVariableCached", false, 0));
			RegisterOpCodeHandler(new OPCodeInfoFirstArrayKey(OPCODE_FirstArrayKey, "FirstArrayKey", true));
			RegisterOpCodeHandler(new OPCodeInfoFirstArrayKey(OPCODE_FirstArrayKeyCached, "FirstArrayKeyCached", false));
			RegisterOpCodeHandler(new OPCodeInfoSetNextArrayKeyCached(OPCODE_NextArrayKey, "NextArrayKey", false));
			RegisterOpCodeHandler(new OPCodeInfoSetNextArrayKeyCached(OPCODE_SetNextArrayKeyCached, "SetNextArrayKeyCached"));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableRefCached(OPCODE_EvalLocalVariableRefCached, "EvalLocalVariableRefCached"));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableRefCached(OPCODE_IW_EvalLocalVariableRefCached0, "IW_EvalLocalVariableRefCached0", 0));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableDefined(OPCODE_EvalLocalVariableDefined, "EvalLocalVariableDefined"));
			RegisterOpCodeHandler(new OPCodeInfoEvalFieldObjectFromRef(OPCODE_EvalFieldObjectFromRef, "EvalFieldObjectFromRef"));

			// idc
			RegisterOpCodeHandler(new OPCodeInfoName(OPCODE_EvalLocalVariableCachedDebug, "EvalLocalVariableCachedDebug", "var"));
			RegisterOpCodeHandler(new OPCodeInfoName(OPCODE_EvalLocalVariableRefCachedDebug, "EvalLocalVariableRefCachedDebug", "var"));
			RegisterOpCodeHandler(new OPCodeInfoGetLocalVar(OPCODE_Unknownc7, "Unknownc7"));

			// ref op
			RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_Dec, "Dec", "--", false));
			RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_Inc, "Inc", "++", false));
			RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_IW_DecRef, "IW_DecRef", "--", true));
			RegisterOpCodeHandler(new OPCodeInfoDeltaVal(OPCODE_IW_IncRef, "IW_IncRef", "++", true));

			// control
			RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_EndOnCallback, "EndOnCallback", "endoncallback", false)); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_EndOn, "EndOn", "endon", false)); // count = params + self
			// ret control
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTill, "WaitTill", "waittill")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatchTimeout, "WaitTillMatchTimeout", "waittillmatchtimeout"));
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatch, "WaitTillMatch", "waittillmatch")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaittillTimeout, "WaittillTimeout", "waittilltimeout")); // count = params + self
			RegisterOpCodeHandler(new OPCodeInfoCountWaittill(OPCODE_WaitTillMatch2, "WaitTillMatch2", "waittillmatch", 1)); // count = params + self - 1 (no hash?)
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
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_T10_GreaterThanOrSuperEqualTo, "T10_GreaterThanOrSuperEqualTo", 2, ">==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfopushopn(OPCODE_T10_LowerThanOrSuperEqualTo, "T10_LowerThanOrSuperEqualTo", 2, "<==", TYPE_OPERATION2, PRIORITY_EQUALS, false, false, false, true));
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
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_GetHash, "GetHash", "#", true, false, false)); // set last to true for test?
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
			RegisterOpCodeHandler(new OPCodeInfoVector(OPCODE_Vector, "Vector", 3));
			RegisterOpCodeHandler(new OPCodeInfoVector(OPCODE_T10_Vector2, "T10_Vector2", 2));
			RegisterOpCodeHandler(new OPCodeInfoVector(OPCODE_T10_Vector4, "T10_Vector4", 4));
			RegisterOpCodeHandler(new OPCodeInfoVectorConstant());
			RegisterOpCodeHandler(new OPCodeInfoGetVector(OPCODE_T10_GetVector2, "T10_GetVector2", 2));
			RegisterOpCodeHandler(new OPCodeInfoGetVector(OPCODE_GetVector, "GetVector", 3));
			RegisterOpCodeHandler(new OPCodeInfoGetVector(OPCODE_T10_GetVector4, "T10_GetVector4", 4));

			RegisterOpCodeHandler(new OPCodeInfoVectorScale(OPCODE_VectorScale, "VectorScale"));
			RegisterOpCodeHandler(new OPCodeInfoGetObjectSize(OPCODE_SizeOf, "SizeOf"));

			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_WaitTillFrameEnd, "WaitTillFrameEnd", "waittillframeend()"));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_Wait, "Wait", "wait", false, true, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_Wait2, "Wait2", "wait", false, true, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_WaitFrame, "WaitFrame", "waitframe"));
			RegisterOpCodeHandler(new OPCodeInfoNotify(OPCODE_Notify, "Notify"));
			RegisterOpCodeHandler(new OPCodeInfoFunctionOperator(OPCODE_IsDefined, "IsDefined", "isdefined", false, false, false, true, false, TYPE_FUNC_IS_DEFINED_OP));

			// PRECODEPOS/CODEPOS on stack
			RegisterOpCodeHandler(new OPCodeInfoNopStmt(OPCODE_ClearParams, "ClearParams"));
			RegisterOpCodeHandler(new OPCodeInfoPreScriptCall(OPCODE_PreScriptCall, "PreScriptCall"));
			RegisterOpCodeHandler(new OPCodeInfoPreScriptCall(OPCODE_IW_VoidCodePos, "IW_VoidCodePos"));

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
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_ScriptFunctionCallFar2, "IW_ScriptFunctionCallFar2", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCallEndOn, "ScriptThreadCallEndOn", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptThreadCall, "ScriptThreadCall", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCallEndOn, "ScriptMethodThreadCallEndOn", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodCall, "ScriptMethodCall", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_ScriptMethodThreadCall, "ScriptMethodThreadCall", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinFunction, "CallBuiltinFunction", 2, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction0, "IW_CallBuiltinFunction0", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction1, "IW_CallBuiltinFunction1", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction2, "IW_CallBuiltinFunction2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction3, "IW_CallBuiltinFunction3", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction4, "IW_CallBuiltinFunction4", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinFunction5, "IW_CallBuiltinFunction5", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_CallBuiltinMethod, "CallBuiltinMethod", 2, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod0, "IW_CallBuiltinMethod0", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod1, "IW_CallBuiltinMethod1", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod2, "IW_CallBuiltinMethod2", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod3, "IW_CallBuiltinMethod3", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod4, "IW_CallBuiltinMethod4", 2, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_CallBuiltinMethod5, "IW_CallBuiltinMethod5", 2, false));

			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalCall, "IW_LocalCall", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalCall2, "IW_LocalCall2", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalThreadCall, "IW_LocalThreadCall", 4, true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalMethodCall, "IW_LocalMethodCall", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalMethodCall2, "IW_LocalMethodCall2", 4, false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCall(OPCODE_IW_LocalMethodThreadCall, "IW_LocalMethodThreadCall", 4, true));

			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointerEndOn, "ScriptThreadCallPointerEndOn", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptThreadCallPointer, "ScriptThreadCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointer, "ScriptMethodThreadCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodThreadCallPointerEndOn, "ScriptMethodThreadCallPointerEndOn", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptFunctionCallPointer, "ScriptFunctionCallPointer", false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_ScriptMethodCallPointer, "ScriptMethodCallPointer", false));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_IW_BuiltinFunctionCallPointer, "IW_BuiltinFunctionCallPointer", true));
			RegisterOpCodeHandler(new OPCodeInfoFuncCallPtr(OPCODE_IW_BuiltinMethodCallPointer, "IW_BuiltinMethodCallPointer", true));

			// linked ref
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetResolveFunction, "GetResolveFunction", 4));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_GetFunction, "GetFunction", 2));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetLocal, "IW_GetLocal", 4));
			RegisterOpCodeHandler(new OPCodeInfoGetString(OPCODE_GetString, "GetString", false));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobal, "GetGlobal", GGGT_PUSH, nullptr));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetGlobalObject, "GetGlobalObject", GGGT_GLOBAL, nullptr));

			// T9
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectFromRef, "T9_EvalFieldVariableFromObjectFromRef", true));
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromObject(OPCODE_T9_EvalFieldVariableFromObjectCached, "T9_EvalFieldVariableFromObjectCached", false));
			RegisterOpCodeHandler(new OPCodeT9SetFieldVariableFromObjectFromRef());
			RegisterOpCodeHandler(new OPCodeT9EvalFieldVariableFromGlobalObject());
			RegisterOpCodeHandler(new OPCodeT9SetVariableFieldFromEvalArrayRef());
			RegisterOpCodeHandler(new OPCodeT9EvalArrayCached(OPCODE_T9_EvalArrayCached, "T9_EvalArrayCached", true));
			RegisterOpCodeHandler(new OPCodeInfoCount(OPCODE_T9_EndOnCallbackParam, "T9_EndOnCallbackParam", "endoncallbackparam", false)); // count = params + self
			RegisterOpCodeHandler(new OPCodeT9GetVarRef());
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorKey, "T9_IteratorKey", "iteratorkey", TYPE_ARRAY_NEXTKEY_IT));
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorVal, "T9_IteratorVal", "iteratorval", TYPE_ARRAY_NEXTVAL_IT));
			RegisterOpCodeHandler(new OPCodeT9Iterator(OPCODE_T9_IteratorNext, "T9_IteratorNext", "iteratornext", TYPE_ARRAY_NEXT_IT));

			RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_IncLocalVariableCached, "IncLocalVariableCached", "++"));
			RegisterOpCodeHandler(new OPCodeT9DeltaLocalVariableCached(OPCODE_T9_DecLocalVariableCached, "DecLocalVariableCached", "--"));

			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_T9_EvalLocalVariableCachedDouble, "EvalLocalVariableCached2n", 2));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_T10_EvalLocalVariableCachedTriple, "EvalLocalVariableCached3n", 3));

			// IW
			RegisterOpCodeHandler(new OPCodeInfoRegisterVariable(OPCode::OPCODE_IW_RegisterVariable, "IW_RegisterVariable", true));
			RegisterOpCodeHandler(new OPCodeInfoRegisterVariable(OPCode::OPCODE_IW_CreateLocalVar, "IW_CreateLocalVar", false));
			RegisterOpCodeHandler(new OPCodeInfoRegisterMultipleVariables());
			RegisterOpCodeHandler(new OPCodeInfoSetNewLocalVariable());
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached0, "IW_EvalLocalVariableCached0", 1, 0));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached1, "IW_EvalLocalVariableCached1", 1, 1));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached2, "IW_EvalLocalVariableCached2", 1, 2));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached3, "IW_EvalLocalVariableCached3", 1, 3));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached4, "IW_EvalLocalVariableCached4", 1, 4));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalVariableCached(OPCODE_IW_EvalLocalVariableCached5, "IW_EvalLocalVariableCached5", 1, 5));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalArrayCached(OPCODE_IW_EvalLocalArrayRefCached, "IW_EvalLocalArrayRefCached", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalArrayCached(OPCODE_IW_EvalLocalArrayCached, "IW_EvalLocalArrayCached", false));
			RegisterOpCodeHandler(new OPCodeInfoEvalLocalObjectCached());
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariable, "IW_EvalLevelFieldVariable", "level", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariableRef, "IW_EvalLevelFieldVariableRef", "level", false));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariableToken, "IW_EvalLevelFieldVariableToken", "level", true, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalLevelFieldVariableTokenRef, "IW_EvalLevelFieldVariableTokenRef", "level", false, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetLevelFieldVariable, "IW_SetLevelFieldVariable", "level"));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetLevelFieldVariableToken, "IW_SetLevelFieldVariableToken", "level", ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVar, "IW_EvalAnimFieldVar", "anim", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVarRef, "IW_EvalAnimFieldVarRef", "anim", false));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVariableToken, "IW_EvalAnimFieldVariableToken", "anim", true, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalAnimFieldVariableTokenRef, "IW_EvalAnimFieldVariableTokenRef", "anim", false, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetAnimFieldVariableToken, "IW_SetAnimFieldVariableToken", "anim", ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetAnimFieldVar, "IW_SetAnimFieldVar", "anim"));
			// let's say it's a "global"
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVar, "IW_EvalSelfFieldVar", "self", true));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVarRef, "IW_EvalSelfFieldVarRef", "self", false));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVariableToken, "IW_EvalSelfFieldVariableToken", "self", true, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoEvalGlobalObjectFieldVariable(OPCODE_IW_EvalSelfFieldVariableTokenRef, "IW_EvalSelfFieldVariableTokenRef", "self", false, ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetSelfFieldVariableToken, "IW_SetSelfFieldVariableToken", "self", ROT_TOKEN));
			RegisterOpCodeHandler(new OPCodeInfoSetGlobalObjectFieldVariable(OPCODE_IW_SetSelfFieldVar, "IW_SetSelfFieldVar", "self"));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetBuiltinFunction, "IW_GetBuiltinFunction", 2));
			RegisterOpCodeHandler(new OPCodeInfoFuncGet(OPCODE_IW_GetBuiltinMethod, "IW_GetBuiltinMethod", 2));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_SingleEndon, "IW_SingleEndon", "endon", false));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_SingleWaitTill, "IW_SingleWaitTill", "waittill", true, 1, 0, true, TYPE_WAITTILL));
			RegisterOpCodeHandler(new OPCodeInfoSingleFunc(OPCODE_IW_IsTrue, "IW_IsTrue", "istrue", true, true));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetLevel, "IW_GetLevel", GGGT_PUSH, "level"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetLevelGRef, "IW_GetLevelGRef", GGGT_GLOBAL, "level"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetGame, "IW_GetGame", GGGT_PUSH, "game"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetGameRef, "IW_GetGameRef", GGGT_FIELD, "game"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_IW_GetAnim, "IW_GetAnim", GGGT_PUSH, "anim"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetAnimGRef, "GetAnimGRef", GGGT_GLOBAL, "anim"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetWorld, "GetWorld", GGGT_PUSH, "world"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetWorldObject, "GetWorldObject", GGGT_GLOBAL, "world"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetClasses, "GetClasses", GGGT_PUSH, "classes"));
			RegisterOpCodeHandler(new OPCodeInfoGetGlobal(OPCODE_GetClassesObject, "GetClassesObject", GGGT_GLOBAL, "classes"));
			RegisterOpCodeHandler(new OPCodeT9EvalArrayCached(OPCODE_IW_EvalArrayCachedField, "IW_EvalArrayCachedField", false));
			RegisterOpCodeHandler(new OPCodeInfoClearLocalVariableCached(OPCODE_IW_ClearFieldVariableRef, "IW_ClearFieldVariableRef"));
			RegisterOpCodeHandler(new OPCodeInfoClearLocalVariableCached(OPCODE_IW_ClearLocalVariableCached0, "IW_ClearLocalVariableCached0", 0));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetDVarHash, "IW_GetDVarHash", "@"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetResourceHash, "IW_GetResourceHash", "%"));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetLocalizedHash, "IW_GetLocalizedHash", "&", true, false, false, true));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_IW_GetTagHash, "IW_GetTagHash", "t", false));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_T10_GetScrHash, "T10_GetScrHash", "#", true, true));
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_SAT_GetOmnVarHash, "SAT_GetOmnVarHash", "o", true));

			RegisterOpCodeHandler(new OPCodeInfoIWSwitch());
			RegisterOpCodeHandler(new OPCodeInfoIWEndSwitch());
			RegisterOpCodeHandler(new OPCodeInfoGetString(OPCODE_IW_GetIString, "IW_GetIString", true));
			RegisterOpCodeHandler(new OPCodeInfoGetAnimation(OPCODE_IW_GetAnimation, "IW_GetAnimation", true));
			RegisterOpCodeHandler(new OPCodeInfoGetAnimation(OPCODE_IW_GetAnimationTree, "IW_GetAnimationTree", false));
			RegisterOpCodeHandler(new OPCodeInfoAddToStruct(OPCODE_IW_AddToStruct, "IW_AddToStruct", false));
			RegisterOpCodeHandler(new OPCodeInfoSetWaittillVariableFieldCached(OPCODE_IW_SetWaittillVariableFieldCached, "IW_SetWaittillVariableFieldCached"));
			RegisterOpCodeHandler(new OPCodeInfoSetWaittillVariableFieldCached(OPCODE_IgnoreWaittillVariableFieldCached, "IgnoreWaittillVariableFieldCached", "_"));

			RegisterOpCodeHandler(new OPCodeInfoStatement(OPCODE_IW_WaitFrame, "IW_WaitFrame", "waitframe()"));
			RegisterOpCodeHandler(new OPCodeInfoGetConstant(OPCODE_IW_GetThread, "IW_GetThread", "getthread()"));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_IW_WaitTillMatch, "IW_WaitTillMatch", "waittillmatch", true, 2, 2, true, TYPE_WAITTILL));
			// scripts\asm\asm::asm_getanim()'s assertmsg lol
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_T10_FlatArgs, "T10_FlatArgs", "flat_args", true, 2, 0, false));
			RegisterOpCodeHandler(new OPCodeInfoSingle(OPCODE_T10_ArrayContainsKey, "T10_ArrayContainsKey", "array_contains_key", true, 2, 0, false));
			RegisterOpCodeHandler(new OPCodeInfoTry());
			RegisterOpCodeHandler(new OPCodeInfoGetPositionRef());

			RegisterOpCodeHandler(new OPCodeInfoIWNotify(OPCODE_IW_Notify, "IW_Notify"));

			// T7
			RegisterOpCodeHandler(new OPCodeInfoGetHash(OPCODE_GetHash32, "GetHash32", "#", false));


			// T8compiler custom opcode
			RegisterOpCodeHandler(new OPCodeInfoT8CGetLazyFunction());

			RegisterOpCodesMap();
			});
	}
}