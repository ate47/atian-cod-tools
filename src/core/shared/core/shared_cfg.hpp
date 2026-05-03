#pragma once
#include <core/logs.hpp>
#include <core/memory_allocator.hpp>
#include <hook/library.hpp>

namespace core::shared_cfg {

	struct SharedCfgLogs  {
		core::logs::loglevel loglevel = core::logs::LVL_INFO;
		const char* logfile{};
		bool basiclog{};
		bool noStdLogs{};
		std::ostream* outStream{};
		std::ostream* cout{};
		std::ostream* cerr{};
		core::logs::logcallback callback{};

		std::vector<std::string> paths{};
	};
	struct SharedCfgHashes {
		core::memory_allocator::MemoryAllocator alloc{};
		std::unordered_map<uint64_t, const char*> defstrings{};
	};

	struct SharedCfgPlugins{
		std::unordered_map<std::string, hook::library::Library> loaded{};
		std::filesystem::path pluginsDir{};
	};

	struct SharedCfg {
		SharedCfgLogs log;
		SharedCfgHashes hashes;
		SharedCfgPlugins plugins;
	};

	SharedCfg& GetSharedConfig();
	void SetSharedConfig(SharedCfg* cfg);
}