#include <includes.hpp>
#include <regex>
#include <future>

namespace {

	int hashscan(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}


		std::vector<std::filesystem::path> files{};

		LOG_TRACE("Load file(s)...");
		utils::GetFileRecurse(argv[2], files, [](const std::filesystem::path& p) {
			auto name = p.string();
			return 
				name.ends_with(".gsc") || name.ends_with(".csc") 
				|| name.ends_with(".csv") || name.ends_with(".tsv")
				|| name.ends_with(".dec.lua") 
				|| name.ends_with(".json");
		});
		LOG_TRACE("{} file(s) loaded...", files.size());

		std::unordered_set<uint64_t> hashes{};

		std::regex pattern{ "(hash|file|var|function|namespace|event|script)_([0-9a-fA-F]{1,16})" };

		std::string buffer{};
		for (const auto& p : files) {
			if (!utils::ReadFile(p, buffer)) {
				LOG_ERROR("Can't read file.");
				return false;
			}
			auto rbegin = std::sregex_iterator(buffer.begin(), buffer.end(), pattern);
			auto rend = std::sregex_iterator();

			for (std::sregex_iterator it = rbegin; it != rend; ++it) {
				std::smatch match = *it;

				hashes.insert(std::stoull(match[2].str(), nullptr, 16));
			}
			// TODO: async
		}

		LOG_INFO("Find {} hash(es)", hashes.size());

		std::ofstream output{ argv[3] };

		if (!output) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		for (uint64_t h : hashes) {
			output << "hash_" << std::hex << h << "\n";
		}

		output.close();

		LOG_INFO("Dump into {}", argv[3]);

		return tool::OK;
	}
	
	ADD_TOOL("hashscan", " [dir] [output]", "scan hashes in a directory", nullptr, hashscan);

}