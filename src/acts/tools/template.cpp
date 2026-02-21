#include <includes.hpp>
#include <core/config.hpp>
#include <mstch/mstch.hpp>
#include <regex>

namespace {
	std::filesystem::path GetBaseDir() {
		std::string dir{ core::config::GetString("data.dir", "") };
		if (dir.empty()) {
			return utils::GetProgDir() / "data" / "templates";
		}
		return std::filesystem::path{ dir } / "templates";
	}

	void PrintTemplateInfo(const std::filesystem::path& base, const char* name) {
		std::filesystem::path path{ base / std::format("{}.json", name) };
		core::config::Config tmpl{ path };
		if (!tmpl.SyncConfig(false)) {
			LOG_ERROR("Can't load {}", path.string());
			return;
		}

		rapidjson::Value& params{ tmpl.GetVal("params", 0, tmpl.main) };

		LOG_INFO(" {}: {}", name, tmpl.GetString("description", "<Missing description>"));

		if (!params.IsNull()) {
			if (!params.IsObject()) {
				throw std::runtime_error(std::format("Invalid params type in {}: not an object", path.string()));
			}
			LOG_INFO("   params:");
			for (auto& [k, v] : params.GetObj()) {
				const char* param{ k.GetString() };
				bool required{ tmpl.GetBool(std::format("params.{}.required", param), true) };
				const char* desc{ tmpl.GetCString(std::format("params.{}.desc", param), "") };
				const char* example{ tmpl.GetCString(std::format("params.{}.example", param), "") };
				const char* defaultVal{ tmpl.GetCString(std::format("params.{}.default", param), "") };


				LOG_INFO("   --{} (value) : {}{}{}{}{}{}",
					param, desc,
					*example ? " example: " : "", example,
					*defaultVal ? " default value: " : "", defaultVal,
					required ? " (required)" : ""
				);
			}
		}

	}

	void PrintTemplateInfoAll(const std::filesystem::path& base) {
		LOG_INFO("Existing templates");
		for (const std::filesystem::directory_entry& sub : std::filesystem::directory_iterator{ base }) {
			if (!sub.is_regular_file()) {
				continue;
			}
			std::filesystem::path path{ sub };
			if (path.extension() != ".json") {
				continue;
			}
			path = path.filename();
			path.replace_extension();
			std::string templateName{ path.string() };
			PrintTemplateInfo(base, templateName.data());
		}
	}


	int gen_template(int argc, const char* argv[]) {
		std::filesystem::path base{ GetBaseDir() };
		if (tool::NotEnoughParam(argc, 1)) {
			PrintTemplateInfoAll(base);
			return tool::BAD_USAGE;
		}
		if (!_strcmpi("help", argv[2]) || !_strcmpi("?", argv[2])) {
			if (tool::NotEnoughParam(argc, 2)) {
				PrintTemplateInfoAll(base);
				return tool::OK;
			}
			else {
				PrintTemplateInfo(base, argv[3]);
				return tool::OK;
			}
		}
		std::filesystem::path templateFile{ base / std::format("{}.json", argv[2]) };
		core::config::Config tmpl{ templateFile };
		if (!tmpl.SyncConfig(false)) {
			LOG_ERROR("Can't load {}", templateFile.string());
			return tool::BASIC_ERROR;
		}
		std::filesystem::path outBase{ std::filesystem::current_path() };

		// parse params

		mstch::map templateData{};

		const char* outArg{};

		bool err{};
		for (size_t i = 3; i < argc; i++) {
			const char* arg{ argv[i] };
			if (*arg != '-') {
				if (!outArg) {
					outArg = arg;
				}
				continue;
			}

			if (arg[1] != '-') {
				LOG_ERROR("Invalid parameter: {}, use type '{} help {}' for help", arg, argv[1], argv[2]);
				return tool::BASIC_ERROR;
			}
			const char* param{ &arg[2] };

			std::string val = argv[++i];
			templateData[param] = val;
			templateData[std::format("_{}_{}_", param, argv[i])] = true;

			const char* validation{ tmpl.GetCString(std::format("params.{}.validation", param)) };
			if (validation) {
				std::regex pattern{ validation };
				std::sregex_iterator rbegin{ val.begin(), val.end(), pattern };

				if (rbegin == std::sregex_iterator() || rbegin->length() != val.size()) {
					LOG_ERROR("Invalid parameter value: {}, pattern: {}", val, validation);
					err = true;
				}
			}
		}

		if (outArg) {
			outBase = outArg;
		}

		std::string actsDir{ utils::GetProgDir().string() };
		std::string workspaceDir{ outBase.string() };
		templateData["template"] = argv[2];
		templateData["acts_dir"] = utils::FormatString(actsDir.data());
		templateData["workspace"] = utils::FormatString(workspaceDir.data());

		rapidjson::Value& params{ tmpl.GetVal("params", 0, tmpl.main) };
		if (!params.IsNull()) {
			if (!params.IsObject()) {
				throw std::runtime_error(std::format("Invalid params type in {}: not an object", tmpl.configFile.string()));
			}
			for (auto& [k, v] : params.GetObj()) {
				const char* param{ k.GetString() };

				if (templateData.find(param) != templateData.end()) {
					continue; // loaded
				}

				if (tmpl.GetBool(std::format("params.{}.required", param), true)) {
					LOG_ERROR("Missing parameter --{} (value), use type '{} help {}' for help", param, argv[1], argv[2]);
					err = true;
					continue;
				}

				const char* defaultVal{ tmpl.GetCString(std::format("params.{}.default", param)) };
				if (defaultVal) {
					templateData[param] = defaultVal;
				}
				
			}
		}
		if (err) {
			return tool::BASIC_ERROR;
		}

		mstch::node context{ mstch::map_wrapper{ templateData } };

		rapidjson::Value& files{ tmpl.GetVal("files", 0, tmpl.main) };

		if (files.IsNull() || !files.IsObject()) {
			throw std::runtime_error(std::format("Invalid files in {}: not an object", tmpl.configFile.string()));
		}

		for (auto& [t, d] : files.GetObj()) {
			std::filesystem::path in{ base / mstch::render(t.GetString(), context) };

			const char* outfile{};
			if (d.IsString()) {
				// patch
				outfile = d.GetString();
			}
			else {
				throw std::runtime_error(std::format("Invalid file {} in {}: not a string", t.GetString(), tmpl.configFile.string()));
			}

			std::filesystem::path out{ outBase / mstch::render(d.GetString(), context) };

			std::string data;

			if (!utils::ReadFile(in, data)) {
				LOG_ERROR("Can't read file {} for {}", in.string(), tmpl.configFile.string());
				return tool::BASIC_ERROR;
			}

			data = mstch::render(data, context);

			std::filesystem::create_directories(out.parent_path());

			if (!utils::WriteFile(out, data)) {
				LOG_ERROR("Can't write file {} for {}", in.string(), tmpl.configFile.string());
				return tool::BASIC_ERROR;
			}

			LOG_INFO("create file {}", out.string());
		}

		LOG_INFO("template created in {}", outBase.string());

		return tool::OK;
	}

	ADD_TOOL(template, "common", "[name] (args?) (out)", "generate files from a template file", gen_template);
}