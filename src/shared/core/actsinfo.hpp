#pragma once
namespace core::actsinfo {
    // Do not write in this file, it is updated by premake
    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;
#ifdef CI_BUILD

    // Version used for the release
#ifdef GPL_BUILD
    constexpr const char* VERSION = "2.21.0-gpl";
    constexpr const wchar_t* VERSIONW = L"2.21.0-gpl";
#else
    constexpr const char* VERSION = "2.21.0";
    constexpr const wchar_t* VERSIONW = L"2.21.0";
#endif // GPL_BUILD
    constexpr unsigned int VERSION_ID = 0x2015000;

#else


#ifdef PRERELEASE_BUILD

    // prerelease
#ifdef GPL_BUILD
    constexpr const char* VERSION = "2.21.0-gpl-pre";
    constexpr const wchar_t* VERSIONW = L"2.21.0-gpl-pre";
#else
    constexpr const char* VERSION = "2.21.0-pre";
    constexpr const wchar_t* VERSIONW = L"2.21.0-pre";
#endif // GPL_BUILD

#else

#ifdef GPL_BUILD
    constexpr const char* VERSION = "Dev-gpl";
    constexpr const wchar_t* VERSIONW = L"Dev-gpl";
#else
    constexpr const char* VERSION = "Dev";
    constexpr const wchar_t* VERSIONW = L"Dev";
#endif // GPL_BUILD

#endif // PRERELEASE_BUILD
    // Disable updater
    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;

#endif // CI_BUILD
}
