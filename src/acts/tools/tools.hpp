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
		const char* m_category;
		const wchar_t* m_game;
		std::string m_nameLower;
		std::string m_usageLower;
		std::string m_descriptionLower;
		std::string m_categoryLower;
		std::wstring m_gameLower;
		tool::toolfunction m_func;
		toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func);

		bool operator!() const;
		bool operatorbool() const;
	};

	struct toolcategory {
		const char* m_name{};
		const char* description{ "" };
		bool visible{};
		std::map<std::string, tool::toolfunctiondata*> m_tools{};
	};

	std::map<std::string, tool::toolfunctiondata*>& tools();
	std::map<std::string, toolcategory>& toolsCategories();

	const char** ReadParams(std::string& str, int& argc);
	int HandleCommand(int argc, const char* argv[]);

	const toolfunctiondata& findtool(const char* name);
	bool search(const char** query, int paramCount, std::function<void(const toolfunctiondata* tool)> each);
	void usage(const char* message, const char* argv0, alogs::loglevel lvl = alogs::loglevel::LVL_ERROR);
}

#define ADD_TOOL(name, category, usage, desc, needGame, function) static tool::toolfunctiondata __toolfunctiondata_##function(name, category, usage, desc, needGame, function)
