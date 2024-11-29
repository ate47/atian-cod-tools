#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>
#include <shield_sdk.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE; // Nothing by default
}

EXPORT const char* PBO4_GetPluginName() {
    return "acts-shield";
}

EXPORT bool PBO4_PreStart() {
    shield_sdk::log(shield_sdk::LOG_TYPE_INFO, "prestart acts plugin");
    return true;
}

EXPORT void PBO4_PostUnpack() {
    shield_sdk::log(shield_sdk::LOG_TYPE_INFO, "post unpack acts plugin");
}

EXPORT void PBO4_PreDestroy() {} // nothing
EXPORT void PBO4_Clean() {} // nothing
