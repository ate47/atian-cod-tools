#pragma once
namespace core::actsinfo {
    // Do not write in this file, it is updated by cmake

    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;
    constexpr unsigned int BUILD_VERSION_ID = 0x3000000;

    constexpr const char *VERSION = "dev-3.0.0";
    constexpr const wchar_t *VERSIONW = L"dev-3.0.0";

#ifdef CI_BUILD
    constexpr unsigned int VERSION_ID = BUILD_VERSION_ID;
#else
    // Disable updater
    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;
#endif // CI_BUILD

}
