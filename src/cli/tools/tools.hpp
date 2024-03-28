#pragma once
#include <memapi.hpp>

namespace tool {
	enum errorcode : int32_t {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef int(*toolfunction)(Process& proc, int argc, const char* argv[]);

	class toolfunctiondata {
	public:
		const char* m_name;
		const char* m_usage;
		const char* m_description;
		const wchar_t* m_game;
		tool::toolfunction m_func;
		toolfunctiondata(const char* name, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::toolfunctiondata*>& tools();

	const toolfunctiondata& findtool(const char* name);
	void usage(const char* message, const char* argv0);
}

#define ADD_TOOL(name, usage, desc, needGame, function) static tool::toolfunctiondata __toolfunctiondata_##function(name, usage, desc, needGame, function)
