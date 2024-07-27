#pragma once
#include <rapidjson/document.h>

/*
 * Config utilities, search for a path inside the config file, a path is delimited by '.',
 * example for:
 * { "test": { "test2": 42 } }
 * 42 is at the path "test.test2"
 */
namespace core::config {
	constexpr const char* MAIN_CONFIG_FILE = "acts.json";

	struct ConfigEnumData {
		const char* name;
		int64_t enumValue;
	};

	class Config {
	public:
		const std::filesystem::path configFile;
		rapidjson::Document main{};
		Config(const std::filesystem::path& path);

		int64_t GetInteger(const char* path, int64_t defaultValue = 0);
		double GetDouble(const char* path, double defaultValue = 0);
		std::string GetString(const char* path, const char* defaultValue = "");
		bool GetBool(const char* path, bool defaultValue = false);
		int64_t GetEnum(const char* path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0);
		template<typename T>
		inline T GetEnumVal(const char* path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
			return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
		}

		void SetInteger(const char* path, int64_t defaultValue);
		void SetDouble(const char* path, double defaultValue);
		void SetString(const char* path, const std::string& defaultValue);
		void SetBool(const char* path, bool defaultValue);
		void SetEnum(const char* path, int64_t enumValue, ConfigEnumData* data, size_t dataCount);

		void SyncConfig(bool save);
		void SaveConfig() const;
	};

	Config& GetMainConfig();

	inline int64_t GetInteger(const char* path, int64_t defaultValue = 0) {
		return GetMainConfig().GetInteger(path, defaultValue);
	}
	inline double GetDouble(const char* path, double defaultValue = 0) {
		return GetMainConfig().GetDouble(path, defaultValue);
	}
	inline std::string GetString(const char* path, const char* defaultValue = "") {
		return GetMainConfig().GetString(path, defaultValue);
	}
	inline bool GetBool(const char* path, bool defaultValue = false) {
		return GetMainConfig().GetBool(path, defaultValue);
	}
	inline int64_t GetEnum(const char* path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) {
		return GetMainConfig().GetEnum(path, data, dataCount, defaultEnumValue);
	}
	template<typename T>
	inline T GetEnumVal(const char* path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
		return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
	}

	inline void SetInteger(const char* path, int64_t value) {
		GetMainConfig().SetInteger(path, value);
	}
	inline void SetDouble(const char* path, double value) {
		GetMainConfig().SetDouble(path, value);
	}
	inline void SetString(const char* path, const std::string& value) {
		GetMainConfig().SetString(path, value);
	}
	inline void SetBool(const char* path, bool value) {
		GetMainConfig().SetBool(path, value);
	}
	inline void SetEnum(const char* path, int64_t enumValue, ConfigEnumData* data, size_t dataCount) {
		GetMainConfig().SetEnum(path, enumValue, data, dataCount);
	}

	/*
	 * Load and save the config
	 */
	inline void SyncConfig(bool save) {
		GetMainConfig().SyncConfig(save);
	}
	/*
	 * Save the config
	 */
	inline void SaveConfig() {
		GetMainConfig().SaveConfig();
	}
}