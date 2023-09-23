#pragma once
namespace actsinfo {
    // Do not write in this file, it is updated by premake
#ifdef DEBUG

    constexpr const char* VERSION = "Debug";
    constexpr unsigned int VERSION_ID = 0xFFFFFFFF;

#else

    // Version used for the release
    constexpr const char* VERSION = "1.2.0";
    constexpr unsigned int VERSION_ID = 0x10200000;

#endif
}
