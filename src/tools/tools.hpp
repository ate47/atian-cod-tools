#pragma once

namespace tool {
	enum errorcode : INT32 {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef int(*toolfunction)(const Process& proc, int argc, const char* argv[]);
}