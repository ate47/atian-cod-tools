#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/system.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <systems/errors.hpp>

namespace {

	void ActsLog(bo4::scriptInstance_t inst) {
		constexpr const char* LOG_PREFIX = "[ActsLog] ";
		const char* prefix{ inst ? "[ActsLog][CSC]" : "[ActsLog][GSC]" };
		bo4::ScrVarType_t type{ bo4::ScrVm_GetType(inst, 0) };
		switch (type) {
		case bo4::TYPE_UNDEFINED:
			LOG_INFO("{} undefined", prefix);
			break;
		case bo4::TYPE_STRING:
			LOG_INFO("{} \"{}\"", prefix, bo4::ScrVm_GetString(inst, 0));
			break;
		case bo4::TYPE_VECTOR: {
			bo4::vec3_t vec{};
			bo4::ScrVm_GetVector(inst, 0, &vec);
			LOG_INFO("{} ({}, {}, {})", prefix, vec[0], vec[1], vec[2]);
			break;
		}
		case bo4::TYPE_HASH: {
			XHash out;
			LOG_INFO("{} #\"{}\"", prefix, core::hashes::ExtractTmp("hash", bo4::ScrVm_GetHash(&out, inst, 0)->name));
			break;
		}
		case bo4::TYPE_FLOAT:
			LOG_INFO("{} {}", prefix, bo4::ScrVm_GetFloat(inst, 0));
			break;
		case bo4::TYPE_INTEGER:
			LOG_INFO("{} {}", prefix, bo4::ScrVm_GetInt(inst, 0));
			break;
		default:
			LOG_INFO("{} \"unk:{}\"", prefix, bo4::scrVarTypeNames[type]);
			break;
		}
	}

	void ActsHashLookup(bo4::scriptInstance_t inst) {
		bo4::ScrVarType_t type{ bo4::ScrVm_GetType(inst, 0) };

		if (type == bo4::TYPE_HASH) {
			XHash hash{ *bo4::ScrVm_GetHash(&hash, inst, 0) };
			const char* str{ core::hashes::ExtractPtr(hash) };

			if (str) {
				bo4::ScrVm_AddString(inst, str);
			}
			else {
				bo4::ScrVm_AddHash(inst, &hash);
			}
		}
		else if (type == bo4::TYPE_STRING) {
			// redirect str
			bo4::ScrVm_AddConstString(inst, bo4::ScrVm_GetConstString(inst, 0));
		}
		else if (type != bo4::TYPE_UNDEFINED) {
			systems::errors::ScrVm_Error(inst, "Invalid Hash lookup type '%s'", false, bo4::scrVarTypeNames[type]);
		}
	}

	struct BuiltinFunctionDef {
		uint32_t canonId;
		uint32_t min_args;
		uint32_t max_args;
		void* actionFunc;
		uint32_t type;
	};

	BuiltinFunctionDef customFuncsGsc[] {
		{ // ActsLog(message)
			.canonId = hash::HashT89Scr("ActsLog"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = ActsLog,
			.type = 0,
		},
		{ // ActsHashLookup(hash)
			.canonId = hash::HashT89Scr("ActsHashLookup"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = ActsHashLookup,
			.type = 0,
		},
	};
	BuiltinFunctionDef customFuncsCsc[] {
		{ // ActsLog(message)
			.canonId = hash::HashT89Scr("ActsLog"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = ActsLog,
			.type = 0,
		},
		{ // ActsHashLookup(hash)
			.canonId = hash::HashT89Scr("ActsHashLookup"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = ActsHashLookup,
			.type = 0,
		},
	};

	hook::library::Detour Scr_GetFunction_Detour;
	hook::library::Detour Scr_GetFunctionReverseLookup_Detour;
	hook::library::Detour CScr_GetFunction_Detour;
	hook::library::Detour CScr_GetFunctionReverseLookup_Detour;

	bool Scr_GetFunctionReverseLookup_Stub(void* func, uint32_t* hash, bool* isFunction) {
		if (Scr_GetFunctionReverseLookup_Detour.Call<bool>(func, hash, isFunction)) {
			return true;
		}

		auto f = std::find_if(std::begin(customFuncsGsc), std::end(customFuncsGsc), [func](const BuiltinFunctionDef& def) { return def.actionFunc == func; });

		if (f != std::end(customFuncsGsc)) {
			*hash = f->canonId;
			*isFunction = true;
			return true;
		}

		return false;
	}
	bool CScr_GetFunctionReverseLookup_Stub(void* func, uint32_t* hash, bool* isFunction) {
		if (CScr_GetFunctionReverseLookup_Detour.Call<bool>(func, hash, isFunction)) {
			return true;
		}

		auto f = std::find_if(std::begin(customFuncsCsc), std::end(customFuncsCsc), [func](const BuiltinFunctionDef& def) { return def.actionFunc == func; });

		if (f != std::end(customFuncsCsc)) {
			*hash = f->canonId;
			*isFunction = true;
			return true;
		}

		return false;
	}

	void* Scr_GetFunction_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
		void* func = Scr_GetFunction_Detour.Call<void*>(name, type, min_args, max_args);

		if (func) {
			return func;
		}

		auto f = std::find_if(std::begin(customFuncsGsc), std::end(customFuncsGsc), [name](const BuiltinFunctionDef& func) { return func.canonId == name; });

		if (f != std::end(customFuncsGsc)) {
			*type = f->type;
			*min_args = f->min_args;
			*max_args = f->max_args;

			return f->actionFunc;
		}

		return nullptr;
	}
	void* CScr_GetFunction_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
		void* func = CScr_GetFunction_Detour.Call<void*>(name, type, min_args, max_args);

		if (func) {
			return func;
		}

		auto f = std::find_if(std::begin(customFuncsCsc), std::end(customFuncsCsc), [name](const BuiltinFunctionDef& func) { return func.canonId == name; });

		if (f != std::end(customFuncsCsc)) {
			*type = f->type;
			*min_args = f->min_args;
			*max_args = f->max_args;

			return f->actionFunc;
		}

		return nullptr;
	}

	void PostInit(uint64_t uid) {
		Scr_GetFunctionReverseLookup_Detour.Create(0x33AF8A0_a, Scr_GetFunctionReverseLookup_Stub);
		CScr_GetFunctionReverseLookup_Detour.Create(0x1F132A0_a, CScr_GetFunctionReverseLookup_Stub);
		Scr_GetFunction_Detour.Create(0x33AF840_a, Scr_GetFunction_Stub);
		CScr_GetFunction_Detour.Create(0x1F13140_a, CScr_GetFunction_Stub);
	}


	REGISTER_SYSTEM(gsc_funcs, nullptr, PostInit);
}