#pragma once

namespace core::updater {
	constexpr const char* UPDATE_ZIP_NAME = "acts_update.zip";

    struct VersionData {
		uint32_t v;
        std::string name;

        bool Read(const std::string& input);

        bool ReadFile(const std::filesystem::path& path);

		bool ReadURL(const std::string& url);
    };
	/**
	 * Check for an update for this installation
	 * @return if the process should stop
	 */
	bool CheckUpdate(bool forceUpdate, bool silent, bool ui);

	/**
	 * apply the update
	 */
	void ApplyUpdate(bool ui);

	/*
	 * Try to find an update
	 * @return if the process should stop
	 */
	bool FindUpdate(bool cli);

	/*
	 * Get a string of a version
	 * @param v version
	 * @return string
	 */
	const char* GetVersionName(uint32_t v);

	/*
	 * @return the url to check for update version
	 */
	const char* GetUpdateUrl();

	/*
	 * @return the url to download update zip
	 */
	const char* GetUpdateZip();
}