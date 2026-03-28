#pragma once
namespace core::actsinfo {
    // Do not write in this file, it is updated by cmake

    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;
    constexpr unsigned int BUILD_VERSION_ID = 0x3000000;

#ifdef CI_BUILD
#define __ACTSINFO_DEV
#define __ACTSINFO_LDEV
#else
#define __ACTSINFO_DEV "dev-"
#define __ACTSINFO_LDEV L"dev-"
#endif

#ifdef PRERELEASE_BUILD
#define __ACTSINFO_PRER "-pre"
#define __ACTSINFO_LPRER L"-pre"
#else
#define __ACTSINFO_PRER
#define __ACTSINFO_LPRER
#endif

#ifdef GPL_BUILD
#define __ACTSINFO_GPL "-gpl"
#define __ACTSINFO_LGPL L"-gpl"
#else
#define __ACTSINFO_GPL
#define __ACTSINFO_LGPL
#endif

    constexpr const char *VERSION = __ACTSINFO_DEV "3.0.0" __ACTSINFO_GPL __ACTSINFO_PRER;
    constexpr const wchar_t *VERSIONW = __ACTSINFO_LDEV L"3.0.0" __ACTSINFO_LGPL __ACTSINFO_LPRER;

#ifdef CI_BUILD
    constexpr unsigned int VERSION_ID = BUILD_VERSION_ID;
#else
    // Disable updater
    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;
#endif // CI_BUILD

}
