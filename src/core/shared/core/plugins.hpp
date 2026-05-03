#pragma once
#include <hook/library.hpp>

namespace core::plugins {
	void LoadPlugins();
	void SetPluginsPath(std::filesystem::path path);
	void ForEachPlugin(std::function<void(hook::library::Library plugin)> callback);

	template<typename... Args>
	void CallForEachPlugin(const char* procName, Args... args) {
		using ProcType = void(*)(Args... args);
		ForEachPlugin([procName, args...](hook::library::Library plugin) {
			ProcType proc{ plugin.GetProc<ProcType>(procName) };
			if (proc) {
				LOG_TRACE("call {}::{}", plugin, procName);
				proc(args...);
			}
		});
	}
}