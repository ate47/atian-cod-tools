#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <systems/gsc_op.hpp>
#include <systems/gsc_link.hpp>

namespace systems::gsc::op {
	namespace {
		void VM_OP_LazyLink(bo4::scriptInstance_t inst, bo4::function_stack_t* stack, bo4::ScrVmContext_t* ctx, bool* terminate) {
			byte* base{ utils::Aligned<uint32_t>(stack->pos) };
			stack->pos = base + 0x10;

			uint32_t nsp{ *(uint32_t*)(base) };
			uint32_t func{ *(uint32_t*)(base + 4) };
			uint64_t script{ *(uint64_t*)(base + 8) };

			byte* funcPtr{};

			// is this function detoured?
			byte* detour{ systems::gsc::link::Scr_FindDetour(inst, base, script, nsp, func) };
			if (detour) {
				funcPtr = detour;
			}
			else {
				size_t count{ bo4::gObjFileInfoCount[inst] };
				for (size_t i = 0; i < count; i++) {
					bo4::objFileInfo_t* info{ (*bo4::gObjFileInfo)[inst] + i };

					if (info->activeVersion->name != script) {
						continue; // not this script
					}


					bo4::GSC_EXPORT_ITEM* exports{ (bo4::GSC_EXPORT_ITEM*)(info->activeVersion->magic + info->activeVersion->export_table_offset) };
					for (size_t j = 0; j < info->activeVersion->exports_count; j++) {
						bo4::GSC_EXPORT_ITEM* exp{ exports + j };

						if (exp->name != func || exp->name_space != nsp) {
							continue; // not this export
						}

						funcPtr = info->activeVersion->magic + exp->address;
						break;
					}

					break;
				}

			}

			bo4::ScrVarValue_t* top{ ++stack->top };

			if (funcPtr) {
				top->type = bo4::TYPE_SCRIPT_FUNCTION;
				top->u.codePosValue = funcPtr;
			}
			else {
				top->type = bo4::TYPE_UNDEFINED;
			}
		}


		void PostInit(uint64_t uid) {
			bo4::gVmOpJumpTable[OP_LazyLink] = VM_OP_LazyLink;
			bo4::gVmOpJumpTableModTool[OPMT_LazyLink] = VM_OP_LazyLink;
		}

		REGISTER_SYSTEM(gsc_op, nullptr, PostInit);
	}
}