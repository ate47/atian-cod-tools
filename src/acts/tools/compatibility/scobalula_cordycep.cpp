#include <includes.hpp>
#include <hook/module_mapper.hpp>
#include <toml.hpp>

namespace {
	using namespace std::literals;
	struct CordycepHandlerPattern {
		std::string name;
		std::string sig;
		std::string type;
		int offset;
	};

	class CordycepHandlerInfo {
	public:
		std::string moduleName;
		std::vector<CordycepHandlerPattern> patterns{};
		std::unordered_map<uint64_t, size_t> namesLookup{};
		CordycepHandlerInfo(const std::filesystem::path& handlerToml) {
			std::string handlerTomlStr = handlerToml.string();
			LOG_TRACE("Loading {}", handlerTomlStr);
			auto cfg = toml::parse_file(handlerTomlStr);

			std::string_view ModuleName = cfg["ModuleName"].value_or(""sv);
			if (ModuleName.empty()) {
				throw std::runtime_error(utils::va("Can't load module %s: No module name", handlerTomlStr.data()));
			}
			moduleName = ModuleName;

			toml::array* cfgPatterns = cfg.get_as<toml::array>("Patterns");

			CordycepHandlerInfo& that{ *this };
			cfgPatterns->for_each([&that](toml::table& pattern) {
				std::string name{ pattern["PatternName"].value_or(""s) };
				std::string sig{ pattern["PatternSignature"].value_or(""s) };
				std::string type{ pattern["PatternType"].value_or(""s) };
				int offset{ pattern["Offset"].value_or(0) };

				//LOG_TRACE("{} : {} / {} / {}", name, sig, type, offset);
				that.namesLookup[hash::Hash64(name.data())] = that.patterns.size();
				that.patterns.emplace_back(name, sig, type, offset);
			});
		}

		CordycepHandlerPattern* operator[](const char* name) {
			auto it = namesLookup.find(hash::Hash64(name));
			if (it == namesLookup.end()) return nullptr;
			return &patterns[it->second];
		}
	};

	int cordycep_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		std::filesystem::path cordycep{ argv[2] };

		CordycepHandlerInfo cordycepInfo{ cordycep / "Data" / "Configs" / utils::va("%s.toml", argv[3]) };

		std::filesystem::path dumpFile{ cordycep / cordycepInfo.moduleName };

		if (!std::filesystem::exists(dumpFile)) {
			LOG_ERROR("Dump file {} doesn't exist, did you forget to dump the game?", dumpFile.string());
			return tool::BASIC_ERROR;
		}

		// wip

		return tool::OK;
	}

	ADD_TOOL(cordycep_mapper, "dev", "[cordycep install] [handler]", "Use cordycep config to do stuff", cordycep_mapper);

}