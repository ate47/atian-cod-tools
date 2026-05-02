#include <includes.hpp>
#include <core/actsinfo.hpp>

constexpr uint32_t GetNextMajorVersion(uint32_t version) {
	return (version & 0xFF000000) + 0x1000000;
}

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
			GetActsVersion pGetActsMinVersion{ (GetActsVersion)GetProcAddress(mod, "GetActsMinVersion") };
			GetActsVersion pGetActsMaxVersion{ (GetActsVersion)GetProcAddress(mod, "GetActsMaxVersion") };
			if (!pGetActsMinVersion) {
				LOG_ERROR("Failed to find <uint32_t GetActsMinVersion()> in plugin {}: {}", name, GetLastError());
				FreeLibrary(mod);
				continue;
			};

			uint32_t minPluginVersion{ pGetActsMinVersion() };
			uint32_t maxPluginVersion{ pGetActsMaxVersion ? pGetActsMaxVersion() : GetNextMajorVersion(minPluginVersion) };

			if (minPluginVersion > core::actsinfo::BUILD_VERSION_ID || maxPluginVersion < core::actsinfo::BUILD_VERSION_ID) {
				LOG_ERROR("Version mismatch in plugin {}: plugin version is 0x{:x}-0x{:x}, but ACTS common version is 0x{:x}!", name, minPluginVersion, maxPluginVersion, core::actsinfo::BUILD_VERSION_ID);
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