#include <dll_includes.hpp>
#include <utils.hpp>
#include <scriptinstance.hpp>
#include <core/system.hpp>
#include "cw.hpp"


namespace {

	byte* FindExport(scriptinstance::ScriptInstance inst, uint64_t target_script, uint32_t name_space, uint32_t name) {
		//F6F5FD0 gObjFileInfoCount
		//F6EC9D0 gObjFileInfo

		uint32_t* count = reinterpret_cast<uint32_t*>(process::Relativise(0xF6F5FD0));
		cw::ObjFileInfoStruct* gObjFileInfo = reinterpret_cast<cw::ObjFileInfoStruct*>(process::Relativise(0xF6EC9D0));

		cw::ObjFileInfo* end = gObjFileInfo[inst] + count[inst];

		auto its = std::find_if(gObjFileInfo[inst], end,
			[target_script](const cw::ObjFileInfo& info) { return info.activeVersion->name == target_script; });

		if (its == end) {
			return nullptr;
		}

		cw::T9GSCOBJ* obj = its->activeVersion;

		auto exportEnd = reinterpret_cast<cw::T8GSCExport*>(obj->magic + obj->exports_tables) + obj->exports_count;
		auto ite = std::find_if(reinterpret_cast<cw::T8GSCExport*>(obj->magic + obj->exports_tables), exportEnd,
			[name_space, name](const cw::T8GSCExport& exp) { return exp.name == name && exp.name_space == name_space; });

		if (ite == exportEnd) {
			return nullptr;
		}

		return obj->magic + ite->address;
	}

	void VM_OP_LazyLink_Handler(scriptinstance::ScriptInstance inst, void* varInfo, cw::FunctionStack* fs_0, void* ctx, bool* terminate) {
		struct LazyLinkData {
			uint32_t nameSpace;
			uint32_t name;
			uint64_t script;
		};
		LazyLinkData* data = (LazyLinkData*)utils::Aligned<uint32_t>(fs_0->pos);

		fs_0->pos = (byte*)(data + 1);

		byte* exp = FindExport(inst, data->script, data->nameSpace, data->name);

		fs_0->top++;

		if (exp) {
			fs_0->top->type = cw::TYPE_SCRIPT_FUNCTION;
			fs_0->top->u.codePosValue = exp;
		}
		else {
			fs_0->top->type = cw::TYPE_UNDEFINED;
			fs_0->top->u.intValue = 0;
		}
	}

	void PostInit(uint64_t id) {
		// add lazylink operator

		// gVmOpJumpTable[]
		reinterpret_cast<void**>(process::Relativise(0xDE87740))[0x13] = VM_OP_LazyLink_Handler;

	}

}
REGISTER_SYSTEM(gsc, nullptr, PostInit);