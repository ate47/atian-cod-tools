#pragma once

namespace core::updater {
	constexpr const char* VERSION_ENDPOINT = "https://github.com/ate47/atian-cod-tools/releases/latest/download/version";
	constexpr const char* ZIP_ENDPOINT = "https://github.com/ate47/atian-cod-tools/releases/latest/download/acts.zip";
	constexpr const char* UPDATE_ZIP_NAME = "acts_update.zip";

    struct VersionData {
        unsigned int v;
        std::string name;

        bool Read(const std::string& input);

        bool ReadFile(const std::filesystem::path& path);

		bool ReadURL(const std::string& url);
    };
	/**
	 * Check for an update for this installation
	 * @return if the process should stop
	 */
	bool CheckUpdate();

	/**
	 * apply the update
	 */
	void ApplyUpdate();
}