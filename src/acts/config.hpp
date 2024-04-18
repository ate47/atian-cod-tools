#pragma once

/*
 * Config utilities, search for a path inside the config file, a path is delimited by '.',
 * example for:
 * { "test": { "test2": 42 } }
 * 42 is at the path "test.test2"
 */
namespace acts::config {
	constexpr const char* CONFIG_FILE = "acts.json";

	int64_t GetInteger(const char* path, int64_t defaultValue = 0);
	double GetDouble(const char* path, double defaultValue = 0);
	std::string GetString(const char* path, const char* defaultValue = "");
	bool GetBool(const char* path, bool defaultValue = false);

	void SetInteger(const char* path, int64_t defaultValue);
	void SetDouble(const char* path, double defaultValue);
	void SetString(const char* path, const std::string& defaultValue);
	void SetBool(const char* path, bool defaultValue);

	/*
	 * Load and save the config
	 */
	void SyncConfig();
	/*
	 * Save the config
	 */
	void SaveConfig();
}