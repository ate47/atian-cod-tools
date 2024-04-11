#pragma once

namespace core::eventhandler {
	typedef void (*EventCallback)(void* data);
	void RegisterEventCallback(uint64_t name, EventCallback callback);
	void RunEvent(uint64_t name, void* data = nullptr);
}