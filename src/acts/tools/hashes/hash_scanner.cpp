#include <includes.hpp>
#include "hash_scanner.hpp"
#include <regex>
#include <future>
#include <BS_thread_pool.hpp>
#include <actslib/profiler.hpp>

namespace tool::hash::scanner {

	void ScanHashes(const std::vector<std::filesystem::path>& files, std::unordered_set<uint64_t>& hashes) {
		static std::regex pattern{ "(hash|file|var|function|namespace|event|script)_([0-9a-fA-F]{1,16})" };

		BS::thread_pool pool{};
		std::mutex mtx{};

		actslib::profiler::Profiler profiler{ "scan hashes" };

		profiler.PushSection("Scan");
		for (const std::filesystem::path& p : files) {
			pool.detach_task([p, &mtx, &hashes] {
				std::string buffer{};
				if (!utils::ReadFile(p, buffer)) {
					LOG_ERROR("Can't read file {}.", p.string());
					return;
				}
				auto rbegin = std::sregex_iterator(buffer.begin(), buffer.end(), pattern);
				auto rend = std::sregex_iterator();
				std::unordered_set<uint64_t> read{};
				for (std::sregex_iterator it = rbegin; it != rend; ++it) {
					std::smatch match = *it;

					read.insert(std::stoull(match[2].str(), nullptr, 16));
				}

				{
					std::lock_guard lg{ mtx };
					hashes.insert(read.begin(), read.end());
				}
			});
		}
		profiler.PopSection();
		profiler.PushSection("Wait");
		pool.wait();
		profiler.PopSection();
		if (alogs::getlevel() == alogs::LVL_TRACE) {
			profiler.WriteToStr(std::cout);
		}
	}
	namespace {

		int hashscan(Process& proc, int argc, const char* argv[]) {
			if (argc < 4) {
				return tool::BAD_USAGE;
			}

			std::vector<std::filesystem::path> files{};

			LOG_TRACE("Load file(s)...");
			utils::GetFileRecurse(argv[2], files, [](const std::filesystem::path& p) {
				std::string name = p.string();
				return 
					name.ends_with(".gsc") || name.ends_with(".csc") 
					|| name.ends_with(".csv") || name.ends_with(".tsv")
					|| name.ends_with(".dec.lua") 
					|| name.ends_with(".json");
			});
			LOG_TRACE("{} file(s) loaded...", files.size());

			std::unordered_set<uint64_t> hashes{};
			ScanHashes(files, hashes);

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
	
		ADD_TOOL("hashscan", "hash", " [dir] [output]", "scan hashes in a directory", nullptr, hashscan);

	}


}