#pragma once

namespace cli::options {
	class CliOption {
		const char* longName;
		const char* smallName;
		const char* usage;
		const char* description;
		core::logs::loglevel lvl;
		size_t len{};
	public:
		CliOption(const char* longName, const char* smallName, const char* usage, const char* description, core::logs::loglevel lvl)
			: longName(longName), smallName(smallName), usage(usage), description(description), lvl(lvl) {
			if (longName) len = std::strlen(longName);
			if (smallName) {
				if (len) {
					len++; // add space
				}
				len += std::strlen(smallName);
			}
			if (usage) len += std::strlen(usage);
		}

		bool Match(const char* arg) const {
			return (longName && !_strcmpi(longName, arg)) || (smallName && !strcmp(smallName, arg));
		}

		constexpr size_t Length() {
			return len;
		}

		constexpr core::logs::loglevel Level() {
			return lvl;
		}

		virtual bool ConsumeArg(int& offset, int argc, const char** argv) = 0;

		std::string StringLine(size_t namePadding) {
			std::stringstream ss{};

			if (longName) {
				ss << longName;
				if (smallName) {
					ss << " " << smallName;
				}
			}
			else if (smallName) {
				ss << smallName;
			}
			ss << usage;

			size_t delta{ namePadding - Length() + 1};
			ss 
				<< std::setw(delta) << std::setfill(' ') << std::left << " "
				<< ": " << description;

			return ss.str();
		}
	};
	template<typename Type>
	class CliOptionVal : public CliOption {
		Type* valPtr;
	public:
		CliOptionVal(Type* valPtr, const char* longName, const char* smallName, const char* usage, const char* description, core::logs::loglevel lvl)
			: valPtr(valPtr), CliOption(longName, smallName, usage, description, lvl) {}

		bool ConsumeArg(int& offset, int argc, const char** argv) override {
			if constexpr (std::is_same<Type, bool>()) {
				*valPtr = true;
				return true;
			}
			if (offset == argc) {
				LOG_ERROR("Missing param for {}", argv[offset - 1]);
				return false;
			}

			const char* param{ argv[offset++] };

			if constexpr (std::is_unsigned<Type>()) {
				*valPtr = (Type)std::strtoull(param, nullptr, 10);
			}
			else if constexpr (std::is_signed<Type>()) {
				*valPtr = (Type)std::strtoll(param, nullptr, 10);
			}
			else if constexpr (std::is_floating_point<Type>()) {
				*valPtr = (Type)std::strtod(param, nullptr);
			}
			else if constexpr (std::is_convertible<const char*, Type>()) {
				*valPtr = param;
			}
			else {
				LOG_WARNING("Ignored param: {}", param);
			}
			return true;
		}
	};

	class CliOptions {
		std::vector<std::unique_ptr<CliOption>> options{};
		std::vector<const char*> params{};
		size_t longestOption{};


	public:
		CliOptions() {}

		template<typename Type>
		CliOptions& addOption(Type* ptr, const char* description, const char* longName, const char* usage = "", const char* smallName = nullptr, core::logs::loglevel lvl = core::logs::loglevel::LVL_INFO) {
			auto opt{ std::make_unique<CliOptionVal<Type>>(ptr, longName, smallName, usage, description, lvl) };
			longestOption = std::max<size_t>(opt->Length(), longestOption);
			options.push_back(std::move(opt));
			return *this;
		}

		void PrintOptions() const {
			for (const auto& opt : options) {
				LOG_LVL(opt->Level(), opt->StringLine(longestOption));
			}	
		}

		bool NotEnoughParam(size_t count) const {
			return params.size() < count;
		}

		bool ComputeOptions(int startParam, int endParam, const char** argv) {
			params.clear();

			for (int i = startParam; i < endParam; ) {
				const char* arg{ argv[i++] };
				
				if (arg[0] == '-') {
					bool matchAny{};
					for (auto& opt : options) {
						if (opt->Match(arg)) {
							if (!opt->ConsumeArg(i, endParam, argv)) {
								return false;
							}
							matchAny = true;
							break;
						}
					}
					if (matchAny) {
						continue;
					}
					LOG_ERROR("Undefined option {}", arg);
					return false;
				}

				params.push_back(arg);
			}

			return true;
		}

		constexpr size_t ParamsCount() {
			return params.size();
		}

		const char* operator[](size_t idx) {
			return params[idx];
		}
	};

}