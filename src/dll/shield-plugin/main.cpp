#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE; // Nothing by default
}

EXPORT const char* PBO4_GetPluginName() {
    return "acts-shield";
}

EXPORT void PBO4_PreStart() {
    LOG_INFO("prestart acts plugin");
    // default config for now
    core::logs::setfile("project-bo4/acts/acts-plugin.log");
    core::config::SetMainConfig(std::format("project-bo4/acts/{}", core::config::MAIN_CONFIG_FILE));
    core::config::SyncConfig(true);

    core::config::ConfigEnumData logNames[]{
        { "trace", core::logs::LVL_TRACE_PATH},
        { "debug", core::logs::LVL_DEBUG},
        { "info", core::logs::LVL_INFO},
        { "warning", core::logs::LVL_WARNING},
        { "error", core::logs::LVL_ERROR}
    };

    core::logs::setlevel(core::config::GetEnumVal<core::logs::loglevel>("logger.level", logNames, ARRAYSIZE(logNames), core::logs::LVL_INFO));
    hook::error::EnableHeavyDump();
    hook::error::InstallErrorHooks(true);

    hook::library::Library main{};
    LOG_INFO("init acts dll pid={} name={}", GetCurrentProcessId(), main.GetName());
    core::system::Init();
}

EXPORT void PBO4_PostUnpack() {
    LOG_INFO("post unpack acts plugin");
    core::system::PostInit();
}

EXPORT void PBO4_PreDestroy() {} // nothing
