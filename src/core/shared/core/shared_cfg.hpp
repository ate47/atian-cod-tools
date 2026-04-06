#pragma once
#include <core/logs.hpp>

namespace core::shared_cfg {

	struct SharedCfg {
		struct {
			core::logs::loglevel loglevel = core::logs::LVL_INFO;
			const char* logfile{};
			bool basiclog{};
			bool noStdLogs{};
			std::ostream* outStream{};
			std::ostream* cout{};
			std::ostream* cerr{};
			core::logs::logcallback callback{};

			std::vector<std::string> paths{};
		} log;
	};

	SharedCfg& GetSharedConfig();
	void SetSharedConfig(SharedCfg* cfg);
}