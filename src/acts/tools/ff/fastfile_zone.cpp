#include <includes.hpp>
#include <tools/ff/fastfile_zone.hpp>
#include <compiler/preprocessor.hpp>

namespace fastfile::zone {
	
	void Zone::ParseFile(const std::string& file) {
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
					LOG_WARNING("[line:{}] Invalid config item: missing = for {}", lineIdx, sw);
					continue;
				}

				std::string key{ sw.substr(1, split - 1) };
				const char*& val{ cfgs[key] };
				
				if (val) {
					LOG_WARNING("[line:{}] Config item {} registered twice", lineIdx, key);
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
					LOG_WARNING("[line:{}] Invalid asset item: missing , for {}", lineIdx, sw);
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