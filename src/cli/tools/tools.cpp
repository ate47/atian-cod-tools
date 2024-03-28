#include <includes.hpp>

std::map<std::string, tool::toolfunctiondata*>& tool::tools() {
	static std::map<std::string, tool::toolfunctiondata*> map{};
	return map;
}

tool::toolfunctiondata::toolfunctiondata(const char* name, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func)
	: m_name(name), m_usage(usage), m_description(description), m_game(game), m_func(func) {
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

void tool::usage(const char* message, const char* argv0) {
	if (message) {
		LOG_ERROR("Error: {}", message);
	}
	LOG_ERROR("{} [function] (params)", argv0);
	LOG_ERROR("Functions", argv0);

	for (const auto& [key, value] : tools()) {
		LOG_ERROR("- {}{} : {}", key, value->m_usage, value->m_description);
	}
}

namespace {
	int list(Process& proc, int argc, const char* argv[]) {
		tool::usage(nullptr, argv[0]);
		return tool::OK;
	}
}
ADD_TOOL("list", "", "list all the tools", nullptr, list);
