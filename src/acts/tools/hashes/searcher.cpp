#include <includes.hpp>


namespace {

	int hashsearcher(Process& proc, int argc, const char* argv[]) {
		if (argc < 4) {
			return tool::BAD_USAGE;
		}

		std::unordered_set<uint64_t> hashes{};

		{
			std::ifstream input{ argv[2] };
			if (!input) {
				LOG_ERROR("Can't open {}", argv[2]);
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd ci{ [&input] { input.close(); } };

			std::string line{};

			while (input && std::getline(input, line, '\n')) {
				if (line.empty()) continue;
				try {
					hashes.insert(std::strtoull(line.c_str(), nullptr, 16));
				}
				catch (std::exception&) {}
			}
		}
		LOG_INFO("Loaded {} hash(es)", hashes.size());



		return tool::OK;
	}

	ADD_TOOL("hashsearcher", "hash", " [file] [output]", "search in hash file", nullptr, hashsearcher);

}