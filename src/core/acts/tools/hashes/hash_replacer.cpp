#include <includes.hpp>
#include "text_expand.hpp"
#include <regex>
#include <actslib/profiler.hpp>

namespace {
	static std::regex pattern{ "(hash|file|var|function|namespace|event|script|class|_id|x64)[:_]([0-9a-fA-F]{1,16})" };


	int hashreplacer(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;
		std::vector<std::filesystem::path> files{};
		utils::GetFileRecurseExt(argv[2], files, ".csv\0.gsc\0.csc\0.txt\0.json\0.dec.lua\0.ddl\0");

		utils::OutFileCE os{ argv[3], true, std::ios::app };

		std::string file{};
		size_t allMatchCount{};
		for (const std::filesystem::path& path : files) {
			LOG_INFO("Lookup {}...", path.string());
			if (!utils::ReadFile(path, file)) {
				LOG_ERROR("Can't read {}", path.string());
				continue;
			}

			auto nrbegin = std::sregex_iterator(file.begin(), file.end(), pattern);
			auto nrend = std::sregex_iterator();

			const char* strstart{ file.data() };
			std::stringstream ssout{};
			size_t matchCount{};

			size_t lastMatch{};
			for (std::sregex_iterator it = nrbegin; it != nrend; ++it) {
				std::smatch match = *it;
				try {
					uint64_t hash{ std::stoull(match[2].str(), nullptr, 16) };


					const char* out{ hashutils::ExtractPtr(hash) };

					if (!out) continue;

					matchCount++;

					os << std::hex << hash << "," << out << "\n";

					size_t start{ (size_t)match.position() };

					if (lastMatch != start) {
						// write pre str
						ssout.write(strstart + lastMatch, start - lastMatch);
					}

					ssout << out;
					lastMatch = start + match.length(); // put the cursor after the match because we replaced the string
				}
				catch (...) {
					// ignore
				}
			}

			if (!matchCount) {
				LOG_INFO("Nothing to replace");
				continue; // nothing to replace
			}

			if (lastMatch < file.length()) {
				ssout.write(strstart + lastMatch, file.length() - lastMatch);
			}

			if (!utils::WriteFile(path, ssout.str())) {
				LOG_ERROR("Error when writing back {}", path.string());
			}
			else {
				LOG_INFO("Replaced {} hashes", matchCount);
			}

			allMatchCount += matchCount;
		}
		LOG_INFO("Replaced {} hash(es) in {} file(s)", allMatchCount, files.size());

		return tool::OK;
	}

	ADD_TOOL(hashreplacer, "hash", " [input] [hashout]", "replace all the hashes of a file", hashreplacer);
}