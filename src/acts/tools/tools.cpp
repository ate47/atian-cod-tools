#include <includes.hpp>
#include <clicolor.hpp>
#include <tools/tools_ui.hpp>
#include <tools/gsc_opcodes.hpp>

std::map<std::string, tool::toolfunctiondata*>& tool::tools() {
	static std::map<std::string, tool::toolfunctiondata*> map{};
	return map;
}
std::map<std::string, tool::toolcategory>& tool::toolsCategories() {
	static std::map<std::string, tool::toolcategory> map{};
	if (map.empty()) {
		map["acts"] = { "acts", "ACTS tools", true, {} };
		map["lib"] = { "lib", "ACTS lib tools and tests", true, {} };
		map["gsc"] = { "gsc", "GSC utilities", true, {} };
		map["hash"] = { "hash", "Hash utilities", true, {} };
		map["bo4"] = { "bo4", "Black Ops 4 tools", true, {} };
		map["cw"] = { "cw", "Black Ops Cold War tools", true, {} };
		map["mwiii"] = { "mwiii", "Modern Warfare III tools", true, {} };
		map["bo3"] = { "bo3", "Black Ops 3 tools", true, {} };
		map["ps4"] = { "ps4", "PS4 tools", true, {} };
		map["common"] = { "common", "Common tools", true, {} };
		map["dev"] = { "dev", "Dev tools", true, {} };
		map["compatibility"] = { "compatibility", "Compatibility tools", true, {} };
		map["fastfile"] = { "fastfile", "Fastfile tools", true, {} };
		map["lua"] = { "lua", "LUA tools", true, {} };
	}

	return map;
}

tool::toolfunctiondata::toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, const wchar_t* game, tool::toolfunction func)
	: m_name(name), m_category(category), m_usage(usage), m_description(description), m_game(game), m_func(func),
		m_nameLower(name ? name : ""), m_usageLower(usage ? usage : ""), m_descriptionLower(description ? description : ""), m_gameLower(game ? game : L""), 
	m_categoryLower(category ? category : "") {

	std::transform(m_nameLower.begin(), m_nameLower.end(), m_nameLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_usageLower.begin(), m_usageLower.end(), m_usageLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_descriptionLower.begin(), m_descriptionLower.end(), m_descriptionLower.begin(), [](char c) { return std::tolower(c); });
	std::transform(m_gameLower.begin(), m_gameLower.end(), m_gameLower.begin(), [](wchar_t c) { return std::tolower(c); });
	std::transform(m_categoryLower.begin(), m_categoryLower.end(), m_categoryLower.begin(), [](wchar_t c) { return std::tolower(c); });
	
	if (name) {
		tools()[name] = this;
		if (!category) {
			category = "unknown";
		}
		tool::toolcategory& cat = toolsCategories()[category];
		if (!cat.m_name) {
			LOG_WARNING("The category '{}' isn't registered, but used by the tool '{}'", category, name);
			cat.m_name = category;
		}
		cat.m_tools[name] = this;
	}
}

bool tool::toolfunctiondata::operator!() const {
	return !m_name;
}

bool tool::toolfunctiondata::operatorbool() const {
	return !m_name;
}

const tool::toolfunctiondata& tool::findtool(const char* name) {
	static tool::toolfunctiondata invalid{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

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
		if (!query[i] || !query[i][0]) continue;
		args.emplace_back(query[i], utils::StrToWStr(query[i]));
		auto& [s, w] = args[args.size() - 1];
		std::transform(s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });
		std::transform(w.begin(), w.end(), w.begin(), [](wchar_t c) { return std::tolower(c); });
	}

	bool findAny{};
	for (const auto& [name, tool] : tool::tools()) {
		if (!tool) {
			continue;
		}
		bool find{ true };
		for (const auto& [s, w] : args) {
			if (
				tool->m_nameLower.rfind(s) != std::string::npos
				|| tool->m_usageLower.rfind(s) != std::string::npos
				|| tool->m_descriptionLower.rfind(s) != std::string::npos
				|| tool->m_categoryLower.rfind(s) != std::string::npos
				|| (!tool->m_gameLower.empty() && tool->m_gameLower.rfind(w) != std::wstring::npos)
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
		if (argc < 3) {
			// show categories

			LOG_INFO("{} {} [category]", argv[0], argv[1]);
			LOG_INFO("Categories");

			LOG_INFO("- all : all the categories");
			for (const auto& [key, value] : tool::toolsCategories()) {
				if (!value.visible || value.m_tools.empty()) {
					continue;
				}
				LOG_INFO("- {} : {}", key, value.description);
			}

			return tool::OK;
		}

		const char* cat{ argv[2] };

		auto& categories{ tool::toolsCategories() };

		if (!_strcmpi(cat, "all")) {
			// show all cat/tools
			LOG_INFO("{} [function] (params)", argv[0]);
			LOG_INFO("Functions", argv[0]);

			for (const auto& [catName, cat] : categories) {
				if (!cat.visible || cat.m_tools.empty()) {
					continue;
				}
				LOG_INFO("- {} ({})", cat.description, catName);
				for (const auto& [key, value] : cat.m_tools) {
					LOG_INFO("  - {}{} : {}", key, value->m_usage, value->m_description);
				}
				LOG_INFO("");
			}
			return tool::OK;
		}

		auto it = categories.find(cat);

		if (it == categories.end() || it->second.m_tools.empty()) {
			LOG_ERROR("The category '{}' doesn't exist", cat);
			return tool::BASIC_ERROR;
		}

		// show category

		LOG_INFO("- {} ({})", it->second.description, cat);
		for (const auto& [key, value] : it->second.m_tools) {
			LOG_INFO("  - {}{} : {}", key, value->m_usage, value->m_description);
		}
		return tool::OK;
	}

	int help(Process& proc, int argc, const char* argv[]) {
		return list(proc, argc, argv);
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
#ifndef CI_BUILD
	int color(Process& proc, int argc, const char* argv[]) {
		for (int b = 0; b < 6; b++) {
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					std::cout << " " << clicolor::ColorBackground(i, j, b) << i << j << b << clicolor::Reset();
				}
				std::cout << " ";
				for (int j = 0; j < 6; j++) {
					std::cout << " " << clicolor::Color(i, j, b) << i << j << b << clicolor::Reset();
				}
				std::cout << "\n";
			}
		}

		std::cout << "\n";

		LOG_TRACE("Trace log");
		LOG_DEBUG("Debug log");
		LOG_INFO("Info log");
		LOG_WARNING("Warning log");
		LOG_ERROR("Error log");

		return tool::OK;
	}
	ADD_TOOL("color", "dev", "", "color tool", nullptr, color);
#endif
	int info(Process& proc, int argc, const char* argv[]) {
		LOG_INFO("Loading acts data...");
		hashutils::ReadDefaultFile();
		tool::gsc::opcode::RegisterOpCodes();
		LOG_INFO("----- acts");
		LOG_INFO("version .. {} (0x{:x})", actsinfo::VERSION, actsinfo::VERSION_ID);
		LOG_INFO("tools .... {} ({} categories)", tool::tools().size(), tool::toolsCategories().size());
		LOG_INFO("tools ui . {}", tool::ui::tools().size());
		LOG_INFO("hash(es) . {}", hashutils::GetMap().size());
		LOG_INFO("path ..... {}", utils::GetProgDir().string());
		std::filesystem::path cwd{ std::filesystem::absolute(".") };
		LOG_INFO("cwd ...... {}", cwd.string());
		LOG_INFO("----- games");

		auto& gameMap = tool::gsc::opcode::GetVMMaps();

		if (gameMap.empty()) {
			LOG_WARNING("No game available");
		}

		for (auto& [vm, info] : gameMap) {
			const char* out = utils::va("- %s (%s/%x) ->", info.name, info.codeName, (int)info.vm);

			for (size_t i = 0; i < tool::gsc::opcode::PLATFORM_COUNT; i++) {
				tool::gsc::opcode::Platform plt = (tool::gsc::opcode::Platform)i;

				if (!info.HasPlatform(plt)) continue;

				size_t count{};

				auto it = info.opcodemappltlookup.find(plt);

				if (it != info.opcodemappltlookup.end()) {
					for (const auto& [op, reg] : it->second) {
						count += reg.size();
					}
				}
				out = utils::va("%s %s:0x%llx", out, 
					utils::MapString(utils::va("%s", tool::gsc::opcode::PlatformName(plt)), [](char c) { return c == ' ' ? '_' : c; }), 
					count);
			}

			LOG_INFO("{}", out);
		}

		return tool::OK;
	}
}
ADD_TOOL("list", "acts", "", "list all the tools", nullptr, list);
ADD_TOOL("help", "acts", "", "list all the tools", nullptr, help);
ADD_TOOL("info", "acts", "", "acts info", nullptr, info);
ADD_TOOL("search", "acts", " (args)*", "search a tool", nullptr, search);
