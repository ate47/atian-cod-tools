#include <includes.hpp>

class ResolverPattern;

class ResolverConfig {
public:
	std::unordered_set<UINT64> m_hashes{};
	std::vector<ResolverPattern> m_pattern{};
	std::string m_dict{};
	std::string m_output{};
	std::mutex m_writeMutex{};
};

class ResolverPattern {
public:
	const std::string m_startStr;
	const UINT64 m_start;
	std::string m_suffix;
	LPCCH m_suffixCStr;

	ResolverPattern(std::string&& startStr, std::string&& suffix)
		: m_startStr(startStr), m_start(hash::Hash64(startStr.c_str())), m_suffix(suffix) {
		for (size_t i = 0; i < m_suffix.length(); i++) {
			auto& c = m_suffix[i];

			if (c == '\\') {
				c = '/';
			}
			else if (c >= 'A' && c <= 'Z') {
				c = c - 'A' + 'z';
			}
		}
		m_suffixCStr = m_suffix.data();
	}

	inline void RunPass(ResolverConfig& cfg, size_t delta, LPCCH dict, size_t dictlen) const {
		UINT64 p = m_start;

		size_t d = delta;

		while (d >= dictlen) {
			p = (p ^ dict[d % dictlen]) * 0x100000001b3;
			d = (d / dictlen) - 1;
		}
		p = (p ^ dict[d]) * 0x100000001b3;

		for (LPCCH e = m_suffixCStr; *e; e++) {
			p = (p ^ *e) * 0x100000001b3;
		}

		p = p & 0x7FFFFFFFFFFFFFFF;

		auto f = cfg.m_hashes.find(p);

		if (f == cfg.m_hashes.end()) {
			return;
		}

		std::lock_guard<std::mutex> lg(cfg.m_writeMutex);

		std::cout << std::hex << p << "," << m_startStr;

		d = delta;

		while (d >= dictlen) {
			std::cout << dict[d % dictlen];
			d = (d / dictlen) - 1;
		}
		std::cout << dict[d];

		std::cout << m_suffix << "\n";

		if (!cfg.m_output.empty()) {
			std::ofstream b{ cfg.m_output, std::ios::app };

			if (!b) {
				LOG_ERROR("Can't open output file {}", cfg.m_output);
			}

			b << std::hex << p << "," << m_startStr;

			d = delta;

			while (d >= dictlen) {
				b << dict[d % dictlen];
				d = (d / dictlen) - 1;
			}
			b << dict[d];

			b << m_suffix << "\n";
		}
	}
};

static int resolver(const Process& unused, int argc, const char* argv[]) {
	if (argc == 2) {
		std::cerr << "Missing config file\n";
		return tool::BAD_USAGE;
	}

	std::filesystem::path cfgFile = std::filesystem::absolute(argv[2]);

	std::ifstream cfgIn{ cfgFile };

	if (!cfgIn) {
		std::cerr << "Can't load config file '" << cfgFile.string() << "'\n";
		return tool::BASIC_ERROR;
	}

	ResolverConfig cfg{};

	std::string line{};

	bool error = false;

	while (std::getline(cfgIn, line)) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		std::string_view sv = line;

		auto idx = sv.find_first_of(' ', 0);

		if (idx == std::string::npos) {
			continue;
		}
		auto action = sv.substr(0, idx);
		auto param = sv.substr(idx + 1);

		if (action == "hashes") {
			std::filesystem::path paramPath{ param };

			auto filename = paramPath.is_absolute() ? paramPath : (cfgFile.parent_path() / paramPath);
			
			LOG_INFO("loading hashes from {}...", filename.string());

			std::ifstream hashStream{ filename };

			if (!hashStream) {
				LOG_ERROR("Can't load file {}", filename.string());
				error = true;
				break;
			}

			UINT64 hash;
			auto oldSize = cfg.m_hashes.size();

			while (hashStream >> std::setbase(16) >> hash) {
				cfg.m_hashes.insert(hash & 0x7FFFFFFFFFFFFFFF);
			}

			hashStream.close();

			LOG_INFO("{} new hashes loaded", cfg.m_hashes.size() - oldSize);
		}
		else if (action == "dump") {
			if (param == "hash") {
				LOG_INFO("{} hash(s) loaded", cfg.m_hashes.size());
				for (const auto& e : cfg.m_hashes) {
					LOG_INFO("hash_{:x}", e);
				}
			} else if (param == "pattern") {
				LOG_INFO("- {} pattern(s) loaded", cfg.m_pattern.size());
				for (const auto& e : cfg.m_pattern) {
					LOG_INFO("- {}???{}", e.m_startStr, e.m_suffix);
				}
			}
			else if (param == "dict") {
				LOG_INFO("- dict[{}]: {} ", cfg.m_dict.length(), cfg.m_dict);
			}
			else {
				LOG_ERROR("can't find dump operation: '{}'", param);
				error = true;
				break;
			}
		}
		else if (action == "add") {
			cfg.m_hashes.insert(std::strtoll(param.data(), nullptr, 16));
		}
		else if (action == "dict") {
			cfg.m_dict += param;
		}
		else if (action == "output") {
			cfg.m_output = param;
		}
		else if (action == "pattern") {
			auto delta = param.find("???", 0, 3);

			if (delta == std::string::npos) {
				LOG_ERROR("can't find '???' location in pattern '{}'", param);
				error = true;
				break;
			}

			cfg.m_pattern.emplace_back(std::string(param.substr(0, delta)), std::string(param.substr(delta + 3)));
		}
		else {
			LOG_ERROR("can't find operation: '{}'", action);
			error = true;
			break;
		}
	}

	cfgIn.close();

	if (error) {
		return tool::BASIC_ERROR;
	}

	if (cfg.m_pattern.size() == 0) {
		cfg.m_pattern.emplace_back(std::string{}, std::string{});
		LOG_INFO("using default pattern");
	}

	LOG_INFO("start with {} hash(es) and {} pattern(s)", cfg.m_hashes.size(), cfg.m_pattern.size());

	CHAR namebuff[200] = {0};

	LPCCH dict = cfg.m_dict.data();
	size_t dictlen = cfg.m_dict.length();

	std::vector<std::thread> threads{};

	for (size_t i = 0; i < cfg.m_pattern.size() - 1; i++) {
		const auto& pattern = cfg.m_pattern[i + 1];
		threads.push_back(
			std::thread([&cfg, pattern, dict, dictlen]() {
				for (size_t delta = 0; ; delta++) {
					pattern.RunPass(cfg, delta, dict, dictlen);
				}
			})
		);
	}
	for (size_t delta = 0; ; delta++) {
		if (delta % 500000000 == 0) {
			auto d = delta;
			int shift = 0;

			while (d >= dictlen) {
				namebuff[shift++] = dict[d % dictlen];
				d = (d / dictlen) - 1;
			}
			namebuff[shift++] = dict[d];
			namebuff[shift] = 0;

			LOG_INFO("curr: {} {} -> {:x}", delta, namebuff, hash::Hash64(namebuff));
		}
		cfg.m_pattern[0].RunPass(cfg, delta, dict, dictlen);
	}

	return tool::OK;
}


ADD_TOOL("resolver", " [cfg_file]", "hash resolver", false, resolver);
