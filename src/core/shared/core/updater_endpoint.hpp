#pragma once
// Do not write in this file, it is updated by cmake

namespace core::updater {
    /* Version endpoint, match release\version format */
    constexpr const char* VERSION_ENDPOINT = "https://github.com/ate47/atian-cod-tools/releases/latest/download/version";
    /* ZIP endpoint, the archive to download */
    constexpr const char* ZIP_ENDPOINT = "https://github.com/ate47/atian-cod-tools/releases/latest/download/acts.zip";
    /* Project name */
    constexpr const char* PROJECT = "ate47/atian-cod-tools";
    /* Project URL */
    constexpr const char* PROJECT_URL = "https://github.com/ate47/atian-cod-tools";
} // namespace core::updater
