#include <dll_includes.hpp>

using namespace custom_gsc_func;
using scriptinstance::ScriptInstance;

void actshello(ScriptInstance inst) {
	LOG_INFO("Hello from vm {}", scriptinstance::Name(inst));
}

std::vector<Builtin> custom_gsc_func::custom_functions[2] = {
	{ // Server functions
		{
			.name = hash::Hash32("actshello"),
			.min_args = 0,
			.max_args = 0,
			.actionFunc = actshello,
			.type = BUILTIN_DEFAULT
		}
	}, 
	{ // Client functions
		{
			.name = hash::Hash32("actshello"),
			.min_args = 0,
			.max_args = 0,
			.actionFunc = actshello,
			.type = BUILTIN_DEFAULT
		}
	}
};

