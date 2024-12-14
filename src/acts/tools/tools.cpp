#include <includes.hpp>
#include <cli/clicolor.hpp>
#include <actscli.hpp>
#include <core/config.hpp>
#include <tools/tools_ui.hpp>
#include <tools/gsc_opcodes.hpp>
#include <acts.hpp>

namespace tool {
	bool DisplayInvisibleTools() {
#ifdef CI_BUILD
		// use the same as the ui
		return core::config::GetBool("nui.bg.showDevTools", false);
#else
		return true;
#endif
	}
	std::map<std::string, tool::toolfunctiondata*>& tools() {
		static std::map<std::string, tool::toolfunctiondata*> map{};
		return map;
	}
	std::map<std::string, tool::toolcategory>& toolsCategories() {
		static std::map<std::string, tool::toolcategory> map{};
		if (map.empty()) {
			map["acts"] = { "acts", "ACTS tools", true, {} };
			map["lib"] = { "lib", "ACTS lib tools and tests", true, {} };
			map["gsc"] = { "gsc", "GSC utilities", true, {} };
			map["dev_gsc"] = { "dev_gsc", "Dev GSC utilities", false, {} };
			map["hash"] = { "hash", "Hash utilities", true, {} };
			map["bo4"] = { "bo4", "Black Ops 4 tools", true, {} };
			map["cw"] = { "cw", "Black Ops Cold War tools", true, {} };
			map["mw19"] = { "mw19", "Modern Warfare 2019 tools", true, {} };
			map["mwiii"] = { "mwiii", "Modern Warfare III tools", true, {} };
			map["bo3"] = { "bo3", "Black Ops 3 tools", true, {} };
			map["bo6"] = { "bo6", "Black Ops 6 tools", true, {} };
			map["ps4"] = { "ps4", "PS4 tools", true, {} };
			map["common"] = { "common", "Common tools", true, {} };
			map["dev"] = { "dev", "Dev tools", false, {} };
			map["compatibility"] = { "compatibility", "Compatibility tools", true, {} };
			map["fastfile"] = { "fastfile", "Fastfile tools", true, {} };
			map["lua"] = { "lua", "LUA tools", true, {} };
		}

		return map;
	}

	toolfunctiondata::toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, tool::toolfunctionnf func) 
		: toolfunctiondata(name, category, usage, description, nullptr, [func](Process& proc, int argc, const char* argv[]) -> int { return func(argc, argv); }) {
	}
	
	toolfunctiondata::toolfunctiondata(const char* name, const char* category, const char* usage, const char* description, const wchar_t* game, toolfunction func)
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
			toolcategory& cat = toolsCategories()[category];
			if (!cat.m_name) {
				LOG_WARNING("The category '{}' isn't registered, but used by the tool '{}'", category, name);
				cat.m_name = category;
			}
			this->visible = cat.visible;
			cat.m_tools[name] = this;
		}
	}

	bool toolfunctiondata::operator!() const {
		return !m_name;
	}

	bool toolfunctiondata::operatorbool() const {
		return !m_name;
	}

	const toolfunctiondata& findtool(const char* name) {
		static toolfunctiondata invalid{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		auto& tls = tools();

		auto tool = tls.find(name);

		if (tool == tls.end()) {
			return invalid;
		}

		return *tool->second;
	}

	void usage(const char* message, const char* argv0, core::logs::loglevel lvl) {
		if (message) {
			LOG_LVLF(lvl, "Error: {}", message);
		}
		LOG_LVLF(lvl, "{} [function] (params)", argv0);
		LOG_LVLF(lvl, "Functions", argv0);

		for (const auto& [key, value] : tools()) {
			LOG_LVLF(lvl, "- {}{} : {}", key, value->m_usage, value->m_description);
		}
	}

	bool search(const char** query, int paramCount, std::function<void(const toolfunctiondata* tool)> each) {
		std::vector<std::pair<std::string, std::wstring>> args{};

		for (size_t i = 0; i < paramCount; i++) {
			if (!query[i] || !query[i][0]) continue;
			args.emplace_back(query[i], utils::StrToWStr(query[i]));
			auto& [s, w] = args[args.size() - 1];
			std::transform(s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });
			std::transform(w.begin(), w.end(), w.begin(), [](wchar_t c) { return std::tolower(c); });
		}

		bool showInvisible{ tool::DisplayInvisibleTools() };

		bool findAny{};
		for (const auto& [name, tool] : tools()) {
			if (!tool || (!tool->visible && !showInvisible)) {
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
		class ParamsContainer {
			std::vector<std::string> values{};
			std::vector<const char*> parsed{};
		public:
			ParamsContainer() {}

			int SyncValues() {
				parsed.clear();
				parsed.reserve(values.size() + 1);

				for (const std::string& str : values) {
					parsed.push_back(str.data());
					LOG_TRACE("param {}", str);
				}

				parsed.push_back(nullptr);

				return (int)values.size();
			}

			const char** ReadParams(std::string& str, int& argc) {
				size_t idx{};

				// base value
				values.clear();
				values.push_back(">");
				std::vector<char> formatted{};

				do {
					// trim spaces
					while (idx < str.length() && isspace(str[idx])) {
						idx++;
					}

					if (idx == str.length()) {
						break;
					}

					char c = str[idx];

					if (c == '"' || c == '\'') {
						// quote
						size_t start = ++idx;

						bool isFormatted{};
						while (idx < str.length() && str[idx] != c) {
							if (str[idx] == '\\') {
								idx++; // ignore next char
								if (idx == str.length()) {
									LOG_ERROR("Missing end quote {} for param {}", c, str.substr(start - 1));
									return nullptr;
								}
							}
							idx++;
							isFormatted = true;
						}

						if (idx == str.length()) {
							LOG_ERROR("Missing end quote {} for param {}", c, str.substr(start - 1));
							return nullptr;
						}

						size_t end = idx;

						if (isFormatted) {
							std::string_view sw{ str.begin() + start, str.begin() + end };
							formatted.clear();


							for (size_t i = 0; i < sw.length(); i++) {
								if (sw[i] != '\\') {
									formatted.push_back(sw[i]);
									continue;
								}

								switch (sw[++i]) {
								case 'n': formatted.push_back('\n'); break;
								case 't': formatted.push_back('\t'); break;
								case 'b': formatted.push_back('\b'); break;
								case 'r': formatted.push_back('\r'); break;
								case 'a': formatted.push_back('\a'); break;
								default: formatted.push_back(sw[i]); break;
								}
							}
							formatted.push_back(0);
							values.push_back(formatted.data());
						}
						else {
							values.push_back(str.substr(start, end - start));
						}

						idx++;
					}
					else {
						// unquoted, to next space

						size_t start = idx++;

						while (idx < str.length() && !isspace(str[idx])) {
							idx++;
						}

						values.push_back(str.substr(start, idx - start));
					}


				} while (idx < str.length());


				argc = SyncValues();
				return parsed.data();
			}
		};

		static ParamsContainer container{};
	}

	const char** ReadParams(std::string& str, int& argc) {
		return container.ReadParams(str, argc);
	}

	int HandleCommand(int argc, const char* argv[]) {
		const auto& tool = findtool(argv[1]);

		if (!tool) {
			LOG_ERROR("Error: Bad tool name. {} list for the tools list", *argv);
			bool find{};
			const char* query[]{ argv[1] };
			tool::search(query, 1, [&find](const tool::toolfunctiondata* tool) {
				if (!find) {
					LOG_INFO("Similar tool name(s):");
					find = true;
				}
				LOG_INFO("- {}", tool->m_name);
				});

			return tool::BASIC_ERROR;
		}

		Process proc(tool.m_game);
		actscli::ActsOptions& opt = actscli::options();

		if (tool.m_game) {

			if (!proc) {
				LOG_ERROR("Can't find game process: {}", utils::WStrToStr(tool.m_game));
				return -1;
			}
			LOG_INFO("Find process {} {}", utils::WStrToStr(tool.m_game), proc);

			if (!proc.Open()) {
				LOG_ERROR("Can't open game process: 0x{:x}", GetLastError());
				return -1;
			}
		}

		if (opt.dumpHashmap) {
			hashutils::SaveExtracted(true, opt.dumpHashmapUnknown);
		}

		const clock_t beginTime = clock();

		actslib::profiler::Profiler& profiler = actscli::GetProfiler();

		int output;
		{
			actslib::profiler::ProfiledSection ps{ profiler, tool.m_name ? tool.m_name : "no-tool-name" };
			if (opt.forceError) {
				output = tool.m_func(proc, argc, argv);
			}
			else {
#ifndef DEBUG
				try {
#endif
					output = tool.m_func(proc, argc, argv);
#ifndef DEBUG
				}
				catch (std::exception& e) {
					LOG_ERROR("Unhandled exception: {}", e.what());
					output = tool::BASIC_ERROR;
				}
#endif
			}
		}

		LOG_TRACE("Tool took {}s to run with output {}{}", (double)(clock() - beginTime) / CLOCKS_PER_SEC, output,
			(output == tool::OK ? " (OK)" : output == tool::BAD_USAGE ? " (BAD_USAGE)" : output == tool::BASIC_ERROR ? " (BASIC_ERROR)" : "")
		);

		hashutils::WriteExtracted(opt.dumpHashmap);

		if (output == tool::BAD_USAGE) {
			LOG_ERROR("Error: Bad tool usage: {} {} {}", *argv, argv[1], tool.m_usage);
		}

		return output;
	}
}

namespace {
	int list(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			// show categories

			LOG_INFO("{} {} [category]", argv[0], argv[1]);
			LOG_INFO("Categories");

			LOG_INFO("- all : all the categories");
			bool showInvisible{ tool::DisplayInvisibleTools() };
			for (const auto& [key, value] : tool::toolsCategories()) {
				if ((!value.visible && !showInvisible) || value.m_tools.empty()) {
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

			bool showInvisible{ tool::DisplayInvisibleTools() };
			for (const auto& [catName, cat] : categories) {
				if ((!cat.visible && !showInvisible) || cat.m_tools.empty()) {
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
					std::cout << " " << cli::clicolor::ColorBackground(i, j, b) << i << j << b << cli::clicolor::Reset();
				}
				std::cout << " ";
				for (int j = 0; j < 6; j++) {
					std::cout << " " << cli::clicolor::Color(i, j, b) << i << j << b << cli::clicolor::Reset();
				}
				std::cout << "\n";
			}
		}

		std::cout << "\n";

		LOG_INFO("test color {}OK /{}BAD", cli::clicolor::Color(0, 5, 0), cli::clicolor::Color(5, 0, 0));

		LOG_TRACE("Trace log");
		LOG_DEBUG("Debug log");
		LOG_INFO("Info log");
		LOG_WARNING("Warning log");
		LOG_ERROR("Error log");

		return tool::OK;
	}
	ADD_TOOL(color, "dev", "", "color tool", nullptr, color);
#endif
	int info(Process& proc, int argc, const char* argv[]) {
		LOG_INFO("Loading acts data...");
		hashutils::ReadDefaultFile();
		tool::gsc::opcode::RegisterOpCodes();
		LOG_INFO("----- acts");
		LOG_INFO("version .. {} (0x{:x})", core::actsinfo::VERSION, core::actsinfo::VERSION_ID);
		LOG_INFO("tools .... {} ({} categories)", tool::tools().size(), tool::toolsCategories().size());
		LOG_INFO("tools ui . {}", tool::ui::tools().size());
		LOG_INFO("hash(es) . {}", hashutils::GetMap().size());
		LOG_INFO("path ..... {}", utils::GetProgDir().string());
		std::filesystem::path cwd{ std::filesystem::absolute(".") };
		LOG_INFO("cwd ...... {}", cwd.string());

		auto& gameMap = tool::gsc::opcode::GetVMMaps();

		if (gameMap.empty()) {
			LOG_WARNING("No game available");
		}
		std::set<uint64_t> vms{};
		// order vms
		for (auto& [vm, info] : gameMap) {
			vms.insert(vm);
		}
		LOG_INFO("----- games ({})", vms.size());


		for (uint64_t vm : vms) {
			auto& info = gameMap.find(vm)->second;

			const char* out{ "" };

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

			const char* type{ info.HasFlag(tool::gsc::opcode::VMF_GSCBIN) ? "gscbin" : "gscc"};

			LOG_INFO("{:016x} - {} ({}/{}) ->{}", vm, info.name, type, info.codeName, out);
		}

		return tool::OK;
	}
}
namespace {
	std::vector<std::unique_ptr<tool::toolfunctiondata>>& GetExToolAlloc() {
		static std::vector<std::unique_ptr<tool::toolfunctiondata>> exToolAlloc{};
		return exToolAlloc;
	}
}

void RegisterActsTool(const char* name, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[])) {
	GetExToolAlloc().push_back(std::make_unique<tool::toolfunctiondata>(name, category, usage, description, func));
}

ADD_TOOL(list, "acts", "", "list all the tools", nullptr, list);
ADD_TOOL(help, "acts", "", "list all the tools", nullptr, help);
ADD_TOOL(info, "acts", "", "acts info", nullptr, info);
ADD_TOOL(search, "acts", " (args)*", "search a tool", nullptr, search);
