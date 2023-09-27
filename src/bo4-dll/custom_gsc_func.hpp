#pragma once
#include <dll_includes.hpp>

namespace custom_gsc_func {
	typedef void (*BuiltinFunction)(scriptinstance::ScriptInstance inst);

	// (dank) custom error 1114190337 ( Juliett 694 Boiling Bison )
	constexpr auto custom_error_id = 0x42693201;

	enum BuiltinType : int {
		BUILTIN_DEFAULT = 0,
		BUILTIN_DEV = 1
	};

	struct __declspec(align(8)) Builtin {
		UINT32 name;
		int min_args;
		int max_args;
		BuiltinFunction actionFunc;
		BuiltinType type;
	};

	extern std::vector<Builtin> custom_functions[2];

}