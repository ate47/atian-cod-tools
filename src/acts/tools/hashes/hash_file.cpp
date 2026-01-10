#include <includes.hpp>
#include <cli/cli_options.hpp>

namespace {
	struct HashEntry {
		uint64_t hash;
		std::string str;
	};

	bool CleanHash(char* string) {
		bool any{};
		for (; *string; string++) {
			if (*string == '\\') {
				*string = '/';
				any = true;
			}
		}

		return any;
	}

	int sort_file(int argc, const char* argv[]) {
		cli::options::CliOptions opts{};
		bool help{};
		bool validate{};
		opts
			.addOption(&help, "show help", "--help", "", "-h")
			.addOption(&validate, "validate files", "--validate", "", "-v")
			;

		if (!opts.ComputeOptions(2, argc, argv) || help) {
			opts.PrintOptions();
			return tool::OK;
		}
		if (opts.NotEnoughParam(1)) {
			return tool::BAD_USAGE;
		}

		int r{ tool::OK };

		std::vector<std::filesystem::path> files{};
		utils::GetFileRecurseExt(opts[0], files, ".csv\0");

		if (files.empty()) {
			LOG_ERROR("can't find any csv file in {}", opts[0]);
			return tool::BAD_USAGE;
		}

		for (std::filesystem::path& path : files) {
			size_t cleaned{};
			bool isSorted{ true };
			bool hasDup{};
			std::vector<HashEntry> entries{};
			{
				utils::InFileCE is{ path, true };

				uint64_t last{};
				std::string line{};
				while (is->good() && std::getline(*is, line)) {
					size_t cut{ line.find(',') };
					if (cut == std::string::npos) {
						continue; // bad line
					}

					char* l{ line.data() };
					l[cut] = 0;
					uint64_t hash{ std::strtoull(line.data(), nullptr, 16) };
					if (last >= hash) {
						if (last == hash) {
							hasDup = true;
						}
						else {
							isSorted = false;
						}
					}
					last = hash;

					char* data{ &l[cut + 1] };

					if (CleanHash(data)) {
						cleaned++;
					}

					entries.emplace_back(hash, data);
				}
			}

			LOG_INFO("loaded {} entry(ies) from {}", entries.size(), path.string());

			core::logs::loglevel nfo{ validate ? core::logs::LVL_ERROR : core::logs::LVL_INFO };
			bool needsUpdate{};

			if (!isSorted) {
				LOG_LVLF(nfo, "the file isn't sorted");
				std::sort(entries.begin(), entries.end(), [](HashEntry& a, HashEntry& b) { return a.hash < b.hash; });
				needsUpdate = true;
			}

			// count the duplicated elements

			if (hasDup) {
				LOG_LVLF(nfo, "the file contains duplicated elements");
				needsUpdate = true;
			}
			if (cleaned) {
				LOG_LVLF(nfo, "the file contains uncleaned elements");
				needsUpdate = true;
			}

			if (validate) {
				if (needsUpdate) {
					LOG_ERROR("the file isn't sorted correctly");
					r = tool::BASIC_ERROR;
				}
				else {
					LOG_INFO("the file is sorted correctly");
				}
				continue;
			}

			if (!needsUpdate) {
				LOG_INFO("nothing to update");
				continue;
			}

			{
				utils::OutFileCE os{ path, true };

				uint64_t last{};
				for (HashEntry& entry : entries) {
					if (entry.hash == last) {
						continue; // ignored
					}

					// write back the entry
					os << std::hex << entry.hash << "," << entry.str << "\n";

					last = entry.hash;
				}

				LOG_INFO("write {}", path.string());
			}
		}

		return r;
	}

	ADD_TOOL(sort_file, "hash", " [files]", "sort a csv hash file", sort_file);
}