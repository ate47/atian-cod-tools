#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE; // Nothing by default
}

EXPORT const char* PBO4_GetPluginName() {
    return "acts-shield";
}

EXPORT void PBO4_PreStart() {
    // default config for now
    core::logs::setfile("acts-plugin.log");
    core::logs::setlevel(core::logs::LVL_TRACE_PATH);
    core::config::SyncConfig(true);


    LOG_INFO("prestart acts plugin");
}

EXPORT void PBO4_PostUnpack() {
    LOG_INFO("post unpack acts plugin");
}

EXPORT void PBO4_PreDestroy() {} // nothing
