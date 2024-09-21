#include <includes_shared.hpp>
#include "eventhandler.hpp"

namespace core::eventhandler {
	static std::unordered_map<uint64_t, std::vector<EventCallback>>& GetHandlers() {
		static std::unordered_map<uint64_t, std::vector<EventCallback>> handlers{};
		return handlers;
	}
	
	void RegisterEventCallback(uint64_t name, EventCallback callback) {
		GetHandlers()[name].push_back(callback);
	}

	void RunEvent(uint64_t name, void* data) {
		auto& handlers = GetHandlers();
		auto it = handlers.find(name);
		if (it == handlers.end()) {
			return; // nothing to run
		}

		for (auto& handler : it->second) {
			handler(data);
		}
	}

}