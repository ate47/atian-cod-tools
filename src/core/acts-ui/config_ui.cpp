#include <ui_includes.hpp>
#include <config_ui.hpp>
#include <acts_api/config.h>

namespace ui3::config {
	std::unordered_map<uint64_t, PropertyEntry>& GetPropertyMap() {
		static std::unordered_map<uint64_t, PropertyEntry> map;
		return map;
	}

	void SyncConfigVals() {
		for (auto& [k, entry] : GetPropertyMap()) {
			entry.Load(entry.ptr);
		}
	}
	void SaveConfig() {
		ActsAPIConfig_SaveConfig();
	}
}