#pragma once
#include <core/eventhandler.hpp>
#include <utils/hash.hpp>

namespace callbacks {
	using namespace core::eventhandler;

	inline void RegisterThreadRegister(EventCallback callback) {
		RegisterEventCallback(hash::Hash64("ThreadRegister"), callback);
	}
	inline void ThreadRegister(void* data = nullptr) {
		RunEvent(hash::Hash64("ThreadRegister"), data);
	}
	inline void RegisterRenderImGui(EventCallback callback) {
		RegisterEventCallback(hash::Hash64("RenderImGui"), callback);
	}
	inline void RenderImGui(void* data = nullptr) {
		RunEvent(hash::Hash64("RenderImGui"), data);
	}
}