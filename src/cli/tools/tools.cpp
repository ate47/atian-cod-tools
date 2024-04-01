#include <includes.hpp>

std::map<std::string, tool::toolfunctiondata*>& tool::tools() {
	static std::map<std::string, tool::toolfunctiondata*> map{};
	return map;
}

tool::toolfunctiondata::toolfunctiondata(const char* name, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func)
	: m_name(name), m_usage(usage), m_description(description), m_game(game), m_func(func),
		m_nameLower(name ? name : ""), m_usageLower(usage ? usage : ""), m_descriptionLower(description ? description : ""), m_gameLower(game ? game : L"") {

	std::transform(m_nameLower.begin(), m_nameLower.end(), m_nameLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_usageLower.begin(), m_usageLower.end(), m_usageLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_descriptionLower.begin(), m_descriptionLower.end(), m_descriptionLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_gameLower.begin(), m_gameLower.end(), m_gameLower.begin(), [](wchar_t c) { return std::tolower(c); });
	
	if (name) {
		tools()[name] = this;
	}
}

bool tool::toolfunctiondata::operator!() const {
	return m_name == NULL;
}

bool tool::toolfunctiondata::operatorbool() const {
	return m_name != NULL;
}

const tool::toolfunctiondata& tool::findtool(const char* name) {
	static tool::toolfunctiondata invalid{ NULL, NULL, NULL, NULL, NULL };

	auto& tls = tools();

	auto tool = tls.find(name);

	if (tool == tls.end()) {
		return invalid;
	}

	return *tool->second;
}

void tool::usage(const char* message, const char* argv0, alogs::loglevel lvl) {
	if (message) {
		LOG_LVL(lvl, "Error: {}", message);
	}
	LOG_LVL(lvl, "{} [function] (params)", argv0);
	LOG_LVL(lvl, "Functions", argv0);

	for (const auto& [key, value] : tools()) {
		LOG_LVL(lvl, "- {}{} : {}", key, value->m_usage, value->m_description);
	}
}

bool tool::search(const char** query, int paramCount, std::function<void(const toolfunctiondata* tool)> each) {
	std::vector<std::pair<std::string, std::wstring>> args{};

	for (size_t i = 0; i < paramCount; i++) {
		args.emplace_back(query[i], utils::StrToWStr(query[i]));
		auto& [s, w] = args[args.size() - 1];
		std::transform(s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });
		std::transform(w.begin(), w.end(), w.begin(), [](wchar_t c) { return std::tolower(c); });
	}

	bool findAny{};
	for (const auto& [name, tool] : tool::tools()) {

		bool find{ true };
		for (const auto& [s, w] : args) {
			if (
				tool->m_nameLower.rfind(s) != std::string::npos
				|| tool->m_usageLower.rfind(s) != std::string::npos
				|| tool->m_descriptionLower.rfind(s) != std::string::npos
				|| tool->m_gameLower.rfind(w) != std::wstring::npos
				) {
				continue;
			}
			find = false;
			break;
		}

		if (!find) {
			continue;
		}

		each(tool);
		findAny = true;
	}

	return findAny;
}

namespace {
	int list(Process& proc, int argc, const char* argv[]) {
		tool::usage(nullptr, argv[0], alogs::LVL_INFO);
		return tool::OK;
	}


	int search(Process& proc, int argc, const char* argv[]) {
		if (!tool::search(argv + 2, argc - 2, [](const tool::toolfunctiondata* tool) {
				LOG_INFO("- {}{} : {}", tool->m_name, tool->m_usage, tool->m_description);
			})) {
			LOG_ERROR("Can't find any tool with this query");
			return tool::BASIC_ERROR;
		}
		return tool::OK;
	}

}
ADD_TOOL("list", "", "list all the tools", nullptr, list);
ADD_TOOL("search", " (args)*", "search a tool", nullptr, search);
