#pragma once
#include <memapi.hpp>

namespace tool {
	enum errorcode : INT32 {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef int(*toolfunction)(Process& proc, int argc, const char* argv[]);

	class toolfunctiondata {
	public:
		LPCCH m_name;
		LPCCH m_usage;
		LPCCH m_description;
		LPCWCH m_game;
		tool::toolfunction m_func;
		toolfunctiondata(LPCCH name, LPCCH usage, LPCCH description, LPCWCH game, tool::toolfunction func);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::toolfunctiondata*>& tools();

	const toolfunctiondata& findtool(LPCCH name);
	void usage(LPCCH message, LPCCH argv0);
}

#define ADD_TOOL(name, usage, desc, needGame, function) static tool::toolfunctiondata __toolfunctiondata_##function(name, usage, desc, needGame, function)
