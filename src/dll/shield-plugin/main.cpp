#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <data/bo4_generated.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE; // Nothing by default
}

EXPORT const char* PBO4_GetPluginName() { return "acts-shield"; }

EXPORT void PBO4_PreStart() {
    LOG_INFO("prestart acts plugin");
    // default config for now
    core::logs::setfile("project-bo4/acts/acts-plugin.log");
    core::config::SetMainConfig(std::format("project-bo4/acts/{}", core::config::MAIN_CONFIG_FILE));
    core::config::SyncConfig(true);

    core::config::ConfigEnumData logNames[]{ { "trace", core::logs::LVL_TRACE_PATH },
                                             { "debug", core::logs::LVL_DEBUG },
                                             { "info", core::logs::LVL_INFO },
                                             { "warning", core::logs::LVL_WARNING },
                                             { "error", core::logs::LVL_ERROR } };

    core::logs::setlevel(
        core::config::GetEnumVal<core::logs::loglevel>(
            "logger.level",
            logNames,
            ACTS_ARRAYSIZE(logNames),
            core::logs::LVL_INFO
        )
    );
    hook::error::EnableHeavyDump();
    hook::error::InstallErrorHooks(true);

    hook::library::Library main{};
    LOG_INFO(
        "init acts {}(0x{:x}) dll pid={} name={}",
        core::actsinfo::VERSION,
        core::actsinfo::BUILD_VERSION_ID,
        GetCurrentProcessId(),
        main.GetName()
    );
    try {
        core::system::Init();
    } catch (std::exception& e) {
        LOG_ERROR("Error at ACTS DLL init {}", e.what());
        MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL init", MB_ICONERROR);
        *reinterpret_cast<byte*>(0x123456789) = 2;
    }
}

EXPORT void PBO4_PostUnpack() {
    LOG_INFO("post unpack acts plugin");
    try {
        {
            hook::scan_container::ScanContainer scan{ {}, true };
            bo4::LoadScans(scan);
        }
        core::system::PostInit();
    } catch (std::exception& e) {
        LOG_ERROR("Error at ACTS DLL post init {}", e.what());
        MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL post init", MB_ICONERROR);
        *reinterpret_cast<byte*>(0x123456789) = 2;
    }
}

EXPORT void PBO4_PreDestroy() {} // nothing
