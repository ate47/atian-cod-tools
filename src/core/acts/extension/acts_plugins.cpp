#include <includes.hpp>
#include <core/actsinfo.hpp>


namespace acts::plugins {
	using LoadPluginFunc = void(*)();
	using GetActsVersion = uint32_t(*)();

	void LoadPlugins() {
		std::filesystem::path pluginsDir{ utils::GetProgDir() / "plugins" };
		std::filesystem::create_directories(pluginsDir);
		std::vector<std::filesystem::path> pluginFiles{};
		utils::GetFileRecurseExt(pluginsDir, pluginFiles, ".dll\0");

		for (const std::filesystem::path& plugin : pluginFiles) {
			std::string name{ plugin.string() };
			HMODULE mod{ LoadLibraryA(name.data()) };
			if (!mod) {
				LOG_ERROR("Failed to load plugin {}: {}", name, GetLastError());
				continue;
			}
			GetActsVersion pGetActsVersion{ (GetActsVersion)GetProcAddress(mod, "GetActsVersion") };
			if (!pGetActsVersion) {
				LOG_ERROR("Failed to find GetActsVersion in plugin {}: {}", name, GetLastError());
				FreeLibrary(mod);
				continue;
			};

			uint32_t pluginVersion{ pGetActsVersion() };

			if (pluginVersion != core::actsinfo::BUILD_VERSION_ID) {
				LOG_ERROR("Version mismatch in plugin {}: plugin version is 0x{:x}, but ACTS common version is 0x{:x}!", name, pluginVersion, core::actsinfo::BUILD_VERSION_ID);
				FreeLibrary(mod);
				continue;
			}

			LoadPluginFunc pLoadActsPlugin{ (LoadPluginFunc)GetProcAddress(mod, "LoadActsPlugin") };

			if (pLoadActsPlugin) {
				pLoadActsPlugin();
			}
			LOG_TRACE("Loaded plugin {}", name);
		}

	}
}