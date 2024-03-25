#pragma once
namespace actsinfo {
    // Do not write in this file, it is updated by premake
#ifdef DEBUG

    constexpr const char* VERSION = "Dev";
    constexpr const wchar_t* VERSIONW = L"Dev";
    constexpr unsigned int VERSION_ID = 0xFFFFFFFF;

#else

    // Version used for the release
    constexpr const char* VERSION = "1.5.3";
    constexpr const wchar_t* VERSIONW = L"1.5.3";
    constexpr unsigned int VERSION_ID = 0x10503000;

#endif
}
