#include <includes_shared.hpp>
#include <utils/io_utils.hpp>
#include <utils/utils.hpp>
#include <hook/library.hpp>
#include <core/config.hpp>
#include <core/actsinfo.hpp>
#include "updater.hpp"
#include <deps/miniz.hpp>



namespace core::updater {
#ifdef CI_BUILD
	constexpr auto ACTS_VERSION = core::actsinfo::VERSION_ID;
#else
#define DEV_UPDATER
	// dev, force a version for the updater
	constexpr auto ACTS_VERSION = core::actsinfo::VERSION_ID;
#endif
    
	bool CheckUpdate(bool forceUpdate) {
        if (!forceUpdate) {
#ifndef DEV_UPDATER
            return false; // not available yet
#endif
            if constexpr (core::actsinfo::DEV_VERSION_ID == ACTS_VERSION) {
                return false; // can't update dev version
            }

            //if (!core::config::GetBool("updater.enabled", true)) {
            //    return false;
            //}
        }

        static std::filesystem::path mainPath{ utils::GetProgDir() };
        static std::filesystem::path updaterExe{ mainPath / "acts-updater.exe" };
        static std::filesystem::path updaterOtherExe{ mainPath / "acts-updater-tmp.exe" };
        // remove tmp updater if it exists
        std::filesystem::remove(updaterOtherExe);
        if (!std::filesystem::exists(updaterExe)) {
            LOG_ERROR("Can't use updater without updater executable {}", updaterExe.string());
            return false;
        }

        if (!forceUpdate) {
            // Check if we need an update
            std::string url = core::config::GetString("updater.versionUrl", VERSION_ENDPOINT);
            LOG_TRACE("Fetching version from {}...", url);

            VersionData latestVersion{};
            if (!latestVersion.ReadURL(url)) {
                // todo: maybe we can check for the last time we updated acts to reduce impacts of timeouts/bad co
                return false; // can't fetch latest update
            }

            if (latestVersion.v <= ACTS_VERSION) {
                LOG_TRACE("Latest version 0x{:x} <= 0x{:x}", latestVersion.v, ACTS_VERSION);
                return false; // nothing to update
            }
        }

        std::string zip = core::config::GetString("updater.zipUrl", ZIP_ENDPOINT);
        LOG_INFO("Download latest version '{}'...", zip);

        std::string latest{};

        if (!utils::io::DownloadFile(zip, latest)) {
            LOG_ERROR("Error when downloading '{}'", zip);
            return false;
        }


        std::filesystem::path zipOut{ std::filesystem::absolute(mainPath / UPDATE_ZIP_NAME) };
        if (!utils::WriteFile(zipOut, latest)) {
            LOG_ERROR("Error when writing '{}'", zipOut.string());
            return false;
        }
        // Create update runable
        std::filesystem::copy(updaterExe, updaterOtherExe);

        LOG_INFO("Start updater with {}", zipOut.string());

        std::wstring updaterOtherExeStr{ updaterOtherExe.wstring() };

        STARTUPINFOW si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));


        if (!CreateProcessW(
            NULL,
            updaterOtherExeStr.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
            )) {
            LOG_ERROR("Can't create process {}", updaterOtherExe.string());
            return false;
        }
        
        AttachConsole(pi.dwThreadId);

		return true;
	}

    void ApplyUpdate() {
        static hook::library::Library main{};
        static std::filesystem::path mainPath{ utils::GetProgDir() };
        std::filesystem::path zipOut{ std::filesystem::absolute(mainPath / UPDATE_ZIP_NAME) };

        if (!std::filesystem::exists(zipOut)) {
            LOG_ERROR("Can't find {}", zipOut.string());
            return;
        }

        std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(zipOut) };

        miniz_cpp::zip_file zf{ buff };

        std::filesystem::path bindir{ "acts/bin" };

        for (miniz_cpp::zip_info& member : zf.infolist()) {
            std::filesystem::path rdir{ member.filename };
            std::filesystem::path out;
            if (utils::IsSubDir(bindir, rdir)) {
                out = mainPath / std::filesystem::relative(rdir, bindir);
            }
            else {
                out = mainPath / member.filename;
            }

            std::filesystem::create_directories(out.parent_path());
            LOG_INFO("Extracting into {}", out.string());

            std::string wni{ zf.read(member.filename) };

            if (!utils::WriteFile(out, wni)) {
                LOG_ERROR("Can't write file");
                continue;
            }
        }

        LOG_INFO("ACTS Updated");
    }

    bool VersionData::Read(const std::string& input) {
        std::stringstream ss{ input };

        if (!std::getline(ss, name, '\n')) {
            return false;
        }

        std::string line;
        if (!std::getline(ss, line, '\n')) {
            return false;
        }
        try {
            v = std::stoul(line, nullptr, 16);
        }
        catch (std::exception) {
            return false;
        }
        return true;
    }

    bool VersionData::ReadFile(const std::filesystem::path& path) {
        std::string buffer{};

        return utils::ReadFile(path, buffer) && Read(buffer);
    }

    bool VersionData::ReadURL(const std::string& url) {
        std::string buffer{};
        try {
            return utils::io::DownloadFile(url, buffer) && Read(buffer);
        }
        catch (std::exception) {
            return false;
        }
    }
}