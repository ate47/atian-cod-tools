#include <includes.hpp>

std::map<std::string, tool::toolfunctiondata*>& tool::tools() {
	static std::map<std::string, tool::toolfunctiondata*> map{};
	return map;
}

tool::toolfunctiondata::toolfunctiondata(LPCCH name, LPCCH usage, LPCCH description, bool needGame, tool::toolfunction func)
	: m_name(name), m_usage(usage), m_description(description), m_needGame(needGame), m_func(func) {
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

const tool::toolfunctiondata& tool::findtool(LPCCH name) {
	static tool::toolfunctiondata invalid{ NULL, NULL, NULL, false, NULL };

	auto& tls = tools();

	auto tool = tls.find(name);

	if (tool == tls.end()) {
		return invalid;
	}

	return *tool->second;
}

void tool::usage(LPCCH message, LPCCH argv0) {
	if (message) {
		std::cerr
			<< "Error: " << message << "\n";
	}
	std::cerr
		<< argv0 << " [function] (params)\n"
		<< "Functions:\n";

	for (const auto& [key, value] : tools()) {
		std::cerr << "- " << key << value->m_usage << " : " << value->m_description << "\n";
	}
}
