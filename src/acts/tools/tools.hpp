#pragma once
#include <utils/memapi.hpp>
#include <utils/utils.hpp>

namespace tool {
	enum errorcode : int32_t {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef std::function<int(Process& proc, int argc, const char* argv[])> toolfunction;
	typedef std::function<int(int argc, const char* argv[])> toolfunctionnf;

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
		toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func);
		toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, tool::toolfunctionnf func);

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

	/*
	 * Test if the tool args contains enough arguments (argv[0] = acts argv[1] = tool name)
	 * @param argc argc
	 * @param count required count
	 * @return true if enough, false otherwise
	 */
	constexpr bool NotEnoughParam(int argc, size_t count) {
		return argc < count + 2;
	}
}

#define ADD_TOOL(id, ...) static tool::toolfunctiondata __toolfunctiondata_##id(#id, __VA_ARGS__)
