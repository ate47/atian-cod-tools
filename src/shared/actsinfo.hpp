#pragma once
namespace actsinfo {
    // Do not write in this file, it is updated by premake
#ifdef DEBUG

    constexpr const char* VERSION = "Dev";
    constexpr unsigned int VERSION_ID = 0xFFFFFFFF;

#else

    // Version used for the release
    constexpr const char* VERSION = "1.2.1";
    constexpr unsigned int VERSION_ID = 0x10201000;

#endif
}
