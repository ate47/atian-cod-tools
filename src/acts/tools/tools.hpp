#pragma once
#include <utils/memapi.hpp>
#include <utils/utils.hpp>
#include "tools_data.hpp"

namespace tool {
	typedef std::function<int(Process& proc, int argc, const char* argv[])> toolfunction;

	class toolfunctiondata {
	public:
		const char* m_name;
		const char* m_usage;
		const char* m_description;
		const char* m_category;
		const wchar_t* m_game;
		bool visible{ true };
		std::string m_nameLower;
		std::string m_usageLower;
		std::string m_descriptionLower;
		std::string m_categoryLower;
		std::wstring m_gameLower;
		tool::toolfunction m_func;
		std::string filename;
		size_t line;
		toolfunctiondata(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func);
		toolfunctiondata(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, tool::toolfunctionnf func);

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
	bool DisplayInvisibleTools();

	const toolfunctiondata& findtool(const char* name);
	bool search(const char** query, int paramCount, std::function<void(const toolfunctiondata* tool)> each);
	void usage(const char* message, const char* argv0, core::logs::loglevel lvl = core::logs::loglevel::LVL_ERROR);
}

#define ADD_TOOL(id, ...) static tool::toolfunctiondata __toolfunctiondata_##id(#id, LOG_GET_LOG_REF_STR, __LINE__, __VA_ARGS__)
