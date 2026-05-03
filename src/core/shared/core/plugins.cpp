#include <includes_shared.hpp>
#include <core/plugins.hpp>
#include <core/actsinfo.hpp>
#include <hook/library.hpp>
#include <core/shared_cfg.hpp>

constexpr uint32_t GetNextMajorVersion(uint32_t version) {
	return (version & 0xFF000000) + 0x1000000;
}

namespace core::plugins {
	using GetActsVersion = uint32_t(*)();

	void LoadPlugins() {
		core::shared_cfg::SharedCfgPlugins& plugins{ core::shared_cfg::GetSharedConfig().plugins };
		std::filesystem::path pluginsDir{ plugins.pluginsDir.empty() ? utils::GetProgDir() / "plugins" : plugins.pluginsDir };
		std::filesystem::create_directories(pluginsDir);
		std::vector<std::filesystem::path> pluginFiles{};
		utils::GetFileRecurseExt(pluginsDir, pluginFiles, ".dll\0");

		for (const std::filesystem::path& plugin : pluginFiles) {
			std::filesystem::path namePath{ plugin.filename() };
			namePath.replace_extension();
			std::string name{ namePath.string() };
			hook::library::Library lib{ plugin };
			if (!lib) {
				LOG_ERROR("Failed to load plugin {}/{}", plugin.string(), name);
				continue;
			}
			GetActsVersion pGetActsMinVersion{ lib.GetProc<GetActsVersion>("GetActsMinVersion") };
			if (!pGetActsMinVersion) {
				LOG_ERROR("Failed to find <uint32_t GetActsMinVersion()> in plugin {}", plugin.string());
				lib.Free();
				continue;
			};

			GetActsVersion pGetActsMaxVersion{ lib.GetProc<GetActsVersion>("GetActsMaxVersion") };
			uint32_t minPluginVersion{ pGetActsMinVersion() };
			uint32_t maxPluginVersion{ pGetActsMaxVersion ? pGetActsMaxVersion() : GetNextMajorVersion(minPluginVersion) };

			if (minPluginVersion > core::actsinfo::BUILD_VERSION_ID || maxPluginVersion < core::actsinfo::BUILD_VERSION_ID) {
				LOG_ERROR("Version mismatch in plugin {}: plugin version is 0x{:x}-0x{:x}, but ACTS common version is 0x{:x}!", plugin.string(), minPluginVersion, maxPluginVersion, core::actsinfo::BUILD_VERSION_ID);
				lib.Free();
				continue;
			}

#ifdef _MSC_VER
			// if the plugin wants to use ABI unsafe functions, check for MSVC version mismatch
			GetActsVersion pGetActsMSVCVersion{ lib.GetProc<GetActsVersion>("GetActsMSVCVersion") };
			if (pGetActsMSVCVersion && pGetActsMSVCVersion() != _MSC_VER) {
				LOG_ERROR("MSVC version mismatch in plugin {}: plugin MSVC version is 0x{:x}, but ACTS common MSVC version is 0x{:x}!", plugin.string(), pGetActsMSVCVersion(), _MSC_VER);
				lib.Free();
				continue;
			}
#endif

			LOG_TRACE("Loaded plugin {}", name);
			plugins.loaded[name] = lib;
		}
	}

	void SetPluginsPath(std::filesystem::path path) {
		core::shared_cfg::SharedCfgPlugins& plugins{ core::shared_cfg::GetSharedConfig().plugins };
		plugins.pluginsDir = path;
	}

	void ForEachPlugin(std::function<void(hook::library::Library plugin)> callback) {
		core::shared_cfg::SharedCfgPlugins& plugins{ core::shared_cfg::GetSharedConfig().plugins };
		for (auto& [_, plugin] : plugins.loaded) {
			callback(plugin);
		}
	}
}