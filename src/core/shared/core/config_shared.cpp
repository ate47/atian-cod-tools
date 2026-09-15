#include <includes_shared.hpp>
#include <hook/error.hpp>
#include <core/config.hpp>
#include <core/config_shared.hpp>
namespace core::config::shared {
    void LoadConfig() {
        core::config::Config& cfg{ core::config::GetMainConfig() };

        cfg.SyncConfig(false);

        core::config::ConfigEnumData logNames[]{ { "trace", core::logs::LVL_TRACE_PATH },
                                                 { "debug", core::logs::LVL_DEBUG },
                                                 { "info", core::logs::LVL_INFO },
                                                 { "warning", core::logs::LVL_WARNING },
                                                 { "error", core::logs::LVL_ERROR } };

        core::logs::setlevel(cfg.GetEnumVal<core::logs::loglevel>(
            "logger.level",
            logNames,
            ACTS_ARRAYSIZE(logNames),
            core::logs::LVL_INFO
        ));

        const char* logOut{ cfg.GetCString("logger.output") };
        if (logOut) {
            core::logs::setfile(logOut);
        }

#ifdef __ACTS_COMPRESS_HAS_RAPIDJSON

        core::config::RapidJsonGeneric& paths{ cfg.GetVal("logger.paths") };

        if (paths.IsString()) {
            core::logs::addlogpath(paths.GetString(), false);
        } else if (paths.IsArray()) {
            for (core::config::RapidJsonGeneric& val : paths.GetArray()) {
                core::logs::addlogpath(val.GetString(), false);
            }
        }

        core::config::RapidJsonGeneric& ignoredPaths{ cfg.GetVal("logger.ignoredPaths") };

        if (ignoredPaths.IsString()) {
            core::logs::addignoredlogpath(ignoredPaths.GetString(), false);
        } else if (ignoredPaths.IsArray()) {
            for (core::config::RapidJsonGeneric& val : ignoredPaths.GetArray()) {
                core::logs::addignoredlogpath(val.GetString(), false);
            }
        }

#endif // __ACTS_COMPRESS_HAS_RAPIDJSON

        if (cfg.GetBool("acts.debug")) {
            hook::error::EnableDebugDump();
        }

        LOG_TRACE("Loaded shared config from {}", cfg.configFile.string());
    }
} // namespace core::config::shared