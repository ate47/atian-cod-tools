#include <includes_shared.hpp>
#include "shared_cfg.hpp"

namespace core::shared_cfg {
	namespace {
		SharedCfg* config{};
	}

	SharedCfg& GetSharedConfig() {
		static SharedCfg defaultCfg{};
		if (config) return *config;
		return defaultCfg;
	}

	void SetSharedConfig(SharedCfg* cfg) {
		config = cfg;
	}

	void* GetSharedConfigPtr() {
		return &GetSharedConfig();
	}

	void SetSharedConfigPtr(void* cfg) {
		SetSharedConfig((SharedCfg*)cfg);
	}
}