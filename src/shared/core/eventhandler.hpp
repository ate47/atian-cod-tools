#pragma once

namespace core::eventhandler {
	typedef std::function<void(void* data)> EventCallback;
	void RegisterEventCallback(uint64_t name, EventCallback callback);
	void RunEvent(uint64_t name, void* data = nullptr);
}