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
		rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> main{};
		Config(const std::filesystem::path& path);

		rapidjson::GenericValue<decltype(Config::main)::EncodingType, decltype(Config::main)::AllocatorType>& GetVal(const char* path, size_t off, rapidjson::GenericValue<decltype(Config::main)::EncodingType, decltype(Config::main)::AllocatorType>& loc);
		void SetVal(const char* path, rapidjson::Value& value, size_t off, rapidjson::GenericValue<decltype(Config::main)::EncodingType, decltype(Config::main)::AllocatorType>& loc);

		int64_t GetInteger(const char* path, int64_t defaultValue = 0);
		inline int64_t GetInteger(const std::string& path, int64_t defaultValue = 0) { return GetInteger(path.c_str(), defaultValue); }
		double GetDouble(const char* path, double defaultValue = 0);
		inline double GetDouble(const std::string& path, double defaultValue = 0) { return GetDouble(path.c_str(), defaultValue); }
		std::string GetString(const char* path, const char* defaultValue = "");
		inline std::string GetString(const std::string& path, const char* defaultValue = "") { return GetString(path.c_str(), defaultValue); }
		bool GetBool(const char* path, bool defaultValue = false);
		inline bool GetBool(const std::string& path, bool defaultValue = false) { return GetBool(path.c_str(), defaultValue); }
		int64_t GetEnum(const char* path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0);
		inline int64_t GetEnum(const std::string& path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) { return GetEnum(path.c_str(), data, dataCount, defaultEnumValue); }
		template<typename T>
		inline T GetEnumVal(const char* path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
			return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
		}
		template<typename T>
		inline T GetEnumVal(const std::string& path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
			return (T)GetEnum(path.c_str(), data, dataCount, (int64_t)defaultEnumValue);
		}

		void SetInteger(const char* path, int64_t defaultValue);
		inline void SetInteger(const std::string& path, int64_t defaultValue) { SetInteger(path.c_str(), defaultValue); }
		void SetDouble(const char* path, double defaultValue);
		inline void SetDouble(const std::string& path, double defaultValue) { SetDouble(path.c_str(), defaultValue); }
		void SetString(const char* path, const std::string& defaultValue);
		inline void SetString(const std::string& path, const std::string& defaultValue) { SetString(path.c_str(), defaultValue); }
		void SetBool(const char* path, bool defaultValue);
		inline void SetBool(const std::string& path, bool defaultValue) { SetBool(path.c_str(), defaultValue); }
		void SetEnum(const char* path, int64_t enumValue, ConfigEnumData* data, size_t dataCount);
		inline void SetEnum(const std::string& path, int64_t enumValue, ConfigEnumData* data, size_t dataCount) { SetEnum(path.c_str(), enumValue, data, dataCount); }

		void SyncConfig(bool save);
		void SaveConfig() const;
	};

	Config& GetMainConfig();

	inline int64_t GetInteger(const char* path, int64_t defaultValue = 0) {
		return GetMainConfig().GetInteger(path, defaultValue);
	}
	inline int64_t GetInteger(const std::string& path, int64_t defaultValue = 0) {
		return GetMainConfig().GetInteger(path, defaultValue);
	}
	inline double GetDouble(const char* path, double defaultValue = 0) {
		return GetMainConfig().GetDouble(path, defaultValue);
	}
	inline double GetDouble(const std::string& path, double defaultValue = 0) {
		return GetMainConfig().GetDouble(path, defaultValue);
	}
	inline std::string GetString(const char* path, const char* defaultValue = "") {
		return GetMainConfig().GetString(path, defaultValue);
	}
	inline std::string GetString(const std::string& path, const char* defaultValue = "") {
		return GetMainConfig().GetString(path, defaultValue);
	}
	inline bool GetBool(const char* path, bool defaultValue = false) {
		return GetMainConfig().GetBool(path, defaultValue);
	}
	inline bool GetBool(const std::string& path, bool defaultValue = false) {
		return GetMainConfig().GetBool(path, defaultValue);
	}
	inline int64_t GetEnum(const char* path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) {
		return GetMainConfig().GetEnum(path, data, dataCount, defaultEnumValue);
	}
	inline int64_t GetEnum(const std::string& path, ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue = 0) {
		return GetMainConfig().GetEnum(path, data, dataCount, defaultEnumValue);
	}
	template<typename T>
	inline T GetEnumVal(const char* path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
		return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
	}
	template<typename T>
	inline T GetEnumVal(const std::string& path, ConfigEnumData* data, size_t dataCount, T defaultEnumValue = 0) {
		return (T)GetEnum(path, data, dataCount, (int64_t)defaultEnumValue);
	}

	inline void SetInteger(const char* path, int64_t value) {
		GetMainConfig().SetInteger(path, value);
	}
	inline void SetInteger(const std::string& path, int64_t value) {
		GetMainConfig().SetInteger(path, value);
	}
	inline void SetDouble(const char* path, double value) {
		GetMainConfig().SetDouble(path, value);
	}
	inline void SetDouble(const std::string& path, double value) {
		GetMainConfig().SetDouble(path, value);
	}
	inline void SetString(const char* path, const std::string& value) {
		GetMainConfig().SetString(path, value);
	}
	inline void SetString(const std::string& path, const std::string& value) {
		GetMainConfig().SetString(path, value);
	}
	inline void SetBool(const char* path, bool value) {
		GetMainConfig().SetBool(path, value);
	}
	inline void SetBool(const std::string& path, bool value) {
		GetMainConfig().SetBool(path, value);
	}
	inline void SetEnum(const char* path, int64_t enumValue, ConfigEnumData* data, size_t dataCount) {
		GetMainConfig().SetEnum(path, enumValue, data, dataCount);
	}
	inline void SetEnum(const std::string& path, int64_t enumValue, ConfigEnumData* data, size_t dataCount) {
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