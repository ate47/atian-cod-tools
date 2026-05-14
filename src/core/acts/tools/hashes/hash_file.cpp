#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <tools/hashes/hash_scanner.hpp>

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

	int mach_hashes(int argc, const char* argv[]) {
		using namespace tool::hash::scanner;
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		utils::InFileCE is{ argv[2], true };
		uint64_t alg{ tool::hash::scanner::ReadVmHashes(argv[3]) };

		uint64_t valids{};
		uint64_t checked{};
		std::string line{};
		while (is->good() && std::getline(*is, line)) {
			size_t cut{ line.find(',') };
			if (cut == std::string::npos) {
				continue; // bad line
			}

			char* l{ line.data() };
			l[cut] = 0;
			uint64_t hash{ std::strtoull(line.data(), nullptr, 16) & ::hash::MASK60 };

			char* str{ &l[cut + 1] };
			checked++;

			if (!hash && !*str) {
				valids++;
				continue; // ok
			}

			size_t t{};
			if ((alg & HASH_FNVA) && (::hash::Hash64A(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_FNVA32) && (::hash::HashX32(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_PRIME) && (::hash::HashPrime(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_DJB2) && (::hash::HashDJB2(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_SCR_T10) && (::hash::HashT10Scr(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_SCR_T10_SP) && (::hash::HashT10ScrSP(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_OMNVAR) && (::hash::HashT10OmnVar(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_SCR_T89) && (::hash::HashT89Scr(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_SCR_JUP) && (::hash::HashJupScr(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_RES) && (::hash::HashIWAsset(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_DVAR) && (::hash::HashIWDVar(str) & ::hash::MASK60) == hash) t++;
			if ((alg & HASH_T7) && (::hash::HashT7(str) & ::hash::MASK60) == hash) t++;

			if (!t) {
				LOG_WARNING("MISSING .. {:x},{}", hash, str);
			}
			else if (t != 1) {
				LOG_WARNING("TOO MANY . {:x},{}", hash, str);
			} else {
				valids++;
			}
		}
		LOG_INFO("validate strings: {}/{} ({:.4}%)", valids, checked, 100.0 * valids / checked);

		return valids == checked ? tool::OK : tool::BASIC_ERROR;
	}

	ADD_TOOL(sort_file, "hash", " [files]", "sort a csv hash file", sort_file);
	ADD_TOOL(mach_hashes, "hash", " [files]", "match hashes in hash file", mach_hashes);
}