#include <includes_shared.hpp>
#include <utils/io_utils.hpp>
#include <utils/utils.hpp>
#include <hook/library.hpp>
#include <core/config.hpp>
#include <core/actsinfo.hpp>
#include "updater.hpp"
#include <deps/miniz.hpp>



namespace core::updater {
    
    static bool NeedCheck(bool silent) {
        if constexpr (core::actsinfo::DEV_VERSION_ID == core::actsinfo::VERSION_ID) {
            if (core::config::GetBool("updater.allowdev", false)) {
                LOG_WARNING("Updating DEV version");
            }
            else {
                if (!silent) {
                    LOG_ERROR("Updating DEV version");
                }
                return false; // can't update dev version
            }
        }
        return !core::config::GetBool("updater.disabled", false);
    }

	bool CheckUpdate(bool forceUpdate, bool silent, bool ui) {
        if (!forceUpdate && !NeedCheck(silent)) {
            return false;
        }

        auto LogInfo = [ui](const std::string& fmt, bool canCancel) {
            if (ui) {
                if (canCancel) {
                    if (MessageBoxA(NULL, fmt.data(), "Updater", MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL) {
                        return false;
                    }
                }
                else {
                    MessageBoxA(NULL, fmt.data(), "Updater", MB_ICONINFORMATION | MB_OK);
                }
            }
            else {
                LOG_INFO("{}", fmt);
            }
            return true;
        };

        static std::filesystem::path mainPath{ utils::GetProgDir() };
        static std::filesystem::path updaterExe{ mainPath / "acts-updater.exe" };
        static std::filesystem::path updaterOtherExe{ mainPath / "acts-updater-tmp.exe" };
        // remove tmp updater if it exists
        std::filesystem::remove(updaterOtherExe);
        if (!std::filesystem::exists(updaterExe)) {
            LOG_ERROR("Can't use updater without updater executable {}", updaterExe.string());
            return false;
        }

        core::logs::loglevel lvl{ silent ? core::logs::LVL_TRACE : core::logs::LVL_INFO };

        if (!forceUpdate) {
            // Check if we need an update
            std::string url = core::config::GetString("updater.versionUrl", VERSION_ENDPOINT);
            LOG_LVLF(lvl, "Fetching version from {}...", url);

            VersionData latestVersion{};
            if (!latestVersion.ReadURL(url)) {
                return false; // can't fetch latest update
            }

            if (latestVersion.v <= core::actsinfo::VERSION_ID) {
                LOG_LVLF(lvl, "Latest version {} <= {}", GetVersionName(latestVersion.v), GetVersionName(core::actsinfo::VERSION_ID));
                LOG_DEBUG("0x{:x} <= 0x{:x}", latestVersion.v, core::actsinfo::VERSION_ID);
                return false; // nothing to update
            }

            if (!LogInfo(std::format("Updating to {}", GetVersionName(latestVersion.v)), true)) {
                return false; // canceled by the user
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

        std::wstring updaterOtherExeStr{ std::format(L"{} {}", updaterOtherExe.wstring(), ui ? L"true" : L"false") };

        STARTUPINFOW si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        si.cb = sizeof(si);

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

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

		return true;
	}

    void ApplyUpdate(bool ui) {
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

        VersionData vd{};
        vd.ReadFile(mainPath / "version");

        std::string msg{ std::format("ACTS Updated to {}", vd.name) };

        LOG_INFO("{}", msg);

        if (ui) {
            MessageBoxA(NULL, msg.data(), "Updater", MB_ICONINFORMATION | MB_OK);
        }
    }

    bool FindUpdate(bool cli) {
        static std::filesystem::path mainPath{ utils::GetProgDir() };
        static std::filesystem::path updaterTest{ mainPath / "acts-updater.json" };


        if (!NeedCheck(true)) {
            return false;
        }

        LOG_TRACE("Check for update...");

        int64_t now{ utils::GetTimestamp() };


        core::config::Config cfg{ updaterTest };
        try {
            cfg.SyncConfig(false);
            bool disabled{ cfg.GetBool("disabled", false) };
            bool forced{ cfg.GetBool("forced", false) };
            int64_t lastCheck{ cfg.GetInteger("lastCheck", now) };
            int64_t timeDelta{ cfg.GetInteger("timeDelta", 1000LL * 3600 * 24) }; // by default one day

            if (!forced && (disabled || lastCheck + timeDelta > now)) {
                // nothing to check
                cfg.SaveConfig();
                LOG_TRACE("Nothing to update.");
                return false;
            }
            cfg.SetInteger("lastCheck", now);
            cfg.SetBool("forced", false);
            cfg.SaveConfig();
            return CheckUpdate(forced, true, !cli);
        }
        catch (std::runtime_error& err) {
            LOG_ERROR("Can't read {}: {}", updaterTest.string(), err.what());
        }
        return false;
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
    const char* GetVersionName(uint32_t v) {
        if (v == core::actsinfo::DEV_VERSION_ID) {
            return "DEV";
        }
        // 0x2008000 -> 2.8.0
        int patch{ v & 0xfff };
        int minor{ (v >> 12) & 0xfff };
        int major{ (v >> 24) & 0xfff };

        return utils::va("%d.%d.%d", major, minor, patch);
    }
}