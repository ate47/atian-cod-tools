#include <dll_includes.hpp>

using namespace custom_gsc_func;
using namespace bo4;
using scriptinstance::ScriptInstance;

void LogPrint(ScriptInstance inst) {
	using namespace t8internal;
	auto type = ScrVm_GetType(inst, 0);

	switch (type) {
	case TYPE_UNDEFINED:
		LOG_INFO("undefined");
		break;
	case TYPE_STRING:
		LOG_INFO("{}", ScrVm_GetString(inst, 0));
		break;
	case TYPE_HASH: {
		Hash hash;
		LOG_INFO("#{}", hash_lookup::ExtractTmp(inst, ScrVm_GetHash(&hash, inst, 0)->hash));
	}
		break;
	case TYPE_INTEGER:
		LOG_INFO("{}", ScrVm_GetInt(inst, 0));
		break;
	case TYPE_FLOAT:
		LOG_INFO("{}", ScrVm_GetFloat(inst, 0));
		break;
	case TYPE_VECTOR:
	{
		Vec3 vec{};
		ScrVm_GetVector(inst, 0, &vec);

		LOG_INFO("({}, {}, {})", vec.x, vec.y, vec.z);
		break;
	}
	default:
		LOG_INFO("{}", t8internal::ScrVarTypeName(type));
		break;
	}
	ScrVm_AddUndefined(inst);
}

void HashLookup(ScriptInstance inst) {
	auto type = ScrVm_GetType(inst, 0);

	if (type == t8internal::TYPE_STRING) {
		ScrVm_AddConstString(inst, ScrVm_GetConstString(inst, 0));
	}
	else if (type == t8internal::TYPE_HASH) {
		Hash hash;

		auto hashedValue = ScrVm_GetHash(&hash, inst, 0)->hash;

		auto* res = hash_lookup::Extract(hashedValue);

		if (res) {
			ScrVm_AddString(inst, res);
		}
		else {
			ScrVm_AddHash(inst, &hash);
		}
	}
	else if (type == t8internal::TYPE_UNDEFINED) {
		ScrVm_AddUndefined(inst);
	}
	else {
		// bad type
		ScrVm_Error(inst, "Usage of ACTSLookup with bad type %s", false, t8internal::ScrVarTypeName(type));
	}
}

std::vector<Builtin> custom_gsc_func::custom_functions[2] = {
	{ // Server functions
		{
			.name = hash::Hash32("ACTSLogPrint"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = LogPrint,
			.type = BUILTIN_DEFAULT
		},
		{
			.name = hash::Hash32("ACTSLookup"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = HashLookup,
			.type = BUILTIN_DEFAULT
		}
	}, 
	{ // Client functions
		{
			.name = hash::Hash32("ACTSLogPrint"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = LogPrint,
			.type = BUILTIN_DEFAULT
		},
		{
			.name = hash::Hash32("ACTSLookup"),
			.min_args = 1,
			.max_args = 1,
			.actionFunc = HashLookup,
			.type = BUILTIN_DEFAULT
		}
	}
};
