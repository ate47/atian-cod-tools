#include <includes.hpp>
#include <tools/ff/fastfile_zone.hpp>
#include <compiler/preprocessor.hpp>

namespace fastfile::zone {

	void Zone::ParseFile(const std::string& file, const char* filename) {
		if (filename) {
			ParseFile(file, [filename](core::logs::loglevel lvl, size_t line, const std::string& message) { LOG_LVLF(lvl, "[{}:{}] {}", filename, line, message); });
		}
		else {
			ParseFile(file, [](core::logs::loglevel lvl, size_t line, const std::string& message) { LOG_LVLF(lvl, "[line:{}] {}", line, message); });
		}
	}
	void Zone::ParseFile(const std::string& file, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler) {
		std::istringstream is{ file };
		
		size_t lineIdx{};
		std::string line;
		// load each lines
		while (is.good() && std::getline(is, line)) {
			lineIdx++;
			size_t start{};
			while (start < line.size() && std::isspace(line[start])) {
				start++;
			}

			if (start == line.size()) continue; // blank line
			
			size_t end{ line.size() };

			while (std::isspace(line[end - 1])) {
				end--;
			}

			std::string_view sw{ &line.data()[start], &line.data()[end - start] };

			if (sw[0] == '>') {
				// config data
				size_t split{ sw.find_first_of('=') };

				if (split == std::string::npos) {
					errorHandler(core::logs::LVL_WARNING, lineIdx, "Invalid item, missing '='");
					continue;
				}

				std::string key{ sw.substr(1, split - 1) };
				const char*& val{ cfgs[key] };
				
				if (val) {
					errorHandler(core::logs::LVL_WARNING, lineIdx, std::format("Config item {} registered twice", key));
					continue;
				}

				size_t valLen{ sw.length() - split - 1 + 1 };
				char* keyStr{ alloc.ClonePtr<char>(&line.data()[start + split + 1], valLen) };
				keyStr[valLen - 1] = 0;
				val = keyStr;
			} else {
				// asset data
				size_t split{ sw.find_first_of(',') };

				if (split == std::string::npos) {
					errorHandler(core::logs::LVL_WARNING, lineIdx, std::format("Invalid asset item: missing ',' for {}", sw));
					continue;
				}

				std::string assettype{ sw.substr(0, split) };
				std::vector<const char*>& vals{ assets[assettype] };

				size_t valLen{ sw.length() - split - 1 + 1 };
				char* keyStr{ alloc.ClonePtr<char>(&line.data()[start + split + 1], valLen) };
				keyStr[valLen - 1] = 0;
				vals.push_back(keyStr);
			}
		}
	}

	const char* Zone::GetConfig(const char* name, const char* defaultVal) {
		const char* v{ cfgs[name] };
		return v ? v : defaultVal;
	}

	int64_t Zone::GetConfigInt(const char* name, int64_t defaultVal) {
		const char* v{ cfgs[name] };
		if (!v) return defaultVal;
		try {
			return utils::ParseFormatInt(v);
		}
		catch (...) {
			LOG_WARNING("Invalid int value for {}: {}", name, v);
			return defaultVal;
		}
	}
	bool Zone::GetConfigBool(const char* name, bool defaultVal) {
		const char* v{ cfgs[name] };
		if (!v) return defaultVal;
		if (utils::EqualIgnoreCase("true", v)) return true;
		if (utils::EqualIgnoreCase("false", v)) return true;
		LOG_WARNING("Invalid bool value for {}: {}", name, v);
		return defaultVal;
	}
	namespace {

		int fastfilezonetest(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
			Zone zone{};

			std::string zoneFile{ utils::ReadFile<std::string>(argv[2]) };

			acts::compiler::preprocessor::PreProcessorOption prep{};
			
			if (!prep.ApplyPreProcessor(zoneFile, argv[2])) {
				LOG_ERROR("Preprocessor error");
				return tool::BASIC_ERROR;
			}

			zone.ParseFile(zoneFile);
			LOG_INFO("Configs: {}", zone.cfgs.size());
			for (auto& [k, c] : zone.cfgs) {
				LOG_INFO("- {} -> \"{}\"", k, c);
			}
			
			LOG_INFO("Assets: {}", zone.assets.size());
			for (auto& [k, vec] : zone.assets) {
				for (const char* asset : vec) {
					LOG_INFO("- {} -> \"{}\"", k, asset);
				}
			}

			return tool::OK;
		}

		ADD_TOOL(fastfilezonetest, "dev", "", "read fastfile zone", fastfilezonetest);
	}
	


}