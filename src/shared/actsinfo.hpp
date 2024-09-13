#pragma once
namespace actsinfo {
    // Do not write in this file, it is updated by premake
    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;
#ifdef CI_BUILD

    // Version used for the release
    constexpr const char* VERSION = "1.13.1";
    constexpr const wchar_t* VERSIONW = L"1.13.1";
    constexpr unsigned int VERSION_ID = 0x1130100;

#else


#ifdef PRERELEASE_BUILD

    // prerelease
    constexpr const char* VERSION = "1.13.1-pre";
    constexpr const wchar_t* VERSIONW = L"1.13.1-pre";

#else

    constexpr const char* VERSION = "Dev";
    constexpr const wchar_t* VERSIONW = L"Dev";

#endif // PRERELEASE_BUILD
    // Disable updater
    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;

#endif // CI_BUILD
}
