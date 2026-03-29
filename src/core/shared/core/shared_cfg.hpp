#pragma once
#include <core/logs.hpp>

namespace core::shared_cfg {

	struct SharedCfg {
		struct {
			core::logs::loglevel loglevel = core::logs::LVL_INFO;
			const char* logfile{};
			bool basiclog{};
			std::ostream* outStream{};
			std::ostream* cout{};
			std::ostream* cerr{};

			std::vector<std::string> paths{};
		} log;
	};

	SharedCfg& GetSharedConfig();
	void SetSharedConfig(SharedCfg* cfg);
}