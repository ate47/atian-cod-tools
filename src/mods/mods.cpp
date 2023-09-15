#include <includes.hpp>


std::map<std::string, mods::modfunctiondata*>& mods::mods() {
	static std::map<std::string, mods::modfunctiondata*> map{};
	return map;
}

const mods::modfunctiondata& mods::findmod(LPCCH name) {
	static mods::modfunctiondata invalid{ NULL, NULL, NULL };

	auto& mod = mods();

	auto tool = mod.find(name);

	if (tool == mod.end()) {
		return invalid;
	}

	return *tool->second;
}

mods::modfunctiondata::modfunctiondata(LPCCH name, LPCCH description, modfunction func)
	: m_name(name), m_description(description), m_func(func) {
	if (name) {
		mods()[name] = this;
	}
}

bool mods::modfunctiondata::operator!() const {
	return m_name == NULL;
}

bool mods::modfunctiondata::operatorbool() const {
	return m_name != NULL;
}


int mod(const Process& unused, int argc, const char* argv[]) {
	if (argc == 2) {
		// no arg = list
		auto& mds = mods::mods();

		std::cout << "Available mods:\n";

		for (const auto& [key, md] : mds) {
			std::cout << "- " << md->m_name << " : " << md->m_description << "\n";
		}

		return tool::OK;
	}
	auto& md = mods::findmod(argv[2]);

	if (!md) {
		std::cerr << "Bad mod id '" << argv[2] << "'\n";
		return tool::BASIC_ERROR;
	}

	return md.m_func(argc, argv);
}

ADD_TOOL("mod", " [modname] [options]", "mods", false, mod);
