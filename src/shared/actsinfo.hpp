#pragma once
namespace actsinfo {
    // Do not write in this file, it is updated by premake
#ifndef CI_BUILD

    constexpr const char* VERSION = "Dev";
    constexpr const wchar_t* VERSIONW = L"Dev";
    constexpr unsigned int VERSION_ID = 0xFFFFFFFF;

#else

    // Version used for the release
    constexpr const char* VERSION = "1.13.1";
    constexpr const wchar_t* VERSIONW = L"1.13.1";
    constexpr unsigned int VERSION_ID = 0x1130100;

#endif
}
