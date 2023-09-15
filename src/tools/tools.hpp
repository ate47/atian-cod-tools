#pragma once

namespace tool {
	enum errorcode : INT32 {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef int(*toolfunction)(const Process& proc, int argc, const char* argv[]);

	class toolfunctiondata {
	public:
		LPCCH m_name;
		LPCCH m_usage;
		LPCCH m_description;
		bool m_needGame;
		tool::toolfunction m_func;
		toolfunctiondata(LPCCH name, LPCCH usage, LPCCH description, bool needGame, tool::toolfunction func);

		bool operator!() const;
		bool operatorbool() const;
	};

	std::map<std::string, tool::toolfunctiondata*>& tools();

	const toolfunctiondata& findtool(LPCCH name);
	void usage(LPCCH message, LPCCH argv0);
}

#define ADD_TOOL(name, usage, desc, needGame, function) static tool::toolfunctiondata __toolfunctiondata_##function(name, usage, desc, needGame, function)
