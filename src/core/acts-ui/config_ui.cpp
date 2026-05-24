#include <ui_includes.hpp>
#include <config_ui.hpp>
#include <acts_api/config.h>

namespace ui3::config {
	std::unordered_map<uint64_t, PropertyEntry>& GetPropertyMap() {
		static std::unordered_map<uint64_t, PropertyEntry> map;
		return map;
	}

	std::vector<PropertyEntry*>& GetPropertyList() {
		static std::vector<PropertyEntry*> list;
		return list;
	}

	void SyncConfigVals() {
		for (auto& [k, entry] : GetPropertyMap()) {
			entry.Load(entry.ptr);
		}
		std::vector<PropertyEntry*>& list{ GetPropertyList() };

		std::sort(list.begin(), list.end(), [](PropertyEntry* a, PropertyEntry* b) {
			return _strcmpi(a->path ? a->path : "", b->path ? b->path : "") < 0;
		});
	}
	void SaveConfig() {
		ActsAPIConfig_SaveConfig();
	}
}