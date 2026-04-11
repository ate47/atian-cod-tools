#ifndef __ACTS_API_VERSION_H__
#define __ACTS_API_VERSION_H__
#include "api.h"
#include "version_info.h"

/*
 * Atian Tools Version API
 */

/*
 * @return build version
 */
ACTS_COMMON_API unsigned int ActsAPIVersion_GetBuildVersion();

/*
 * @return version string
 */
ACTS_COMMON_API const char* ActsAPIVersion_GetVersion();

/*
 * @return version id
 */
ACTS_COMMON_API unsigned int ActsAPIVersion_GetVersionId();

/*
 * check that a API version is the same as the built version, the macro ActsAPIVersion_ValidateVersion
 * @param buildVersion build version
 * @return status, if not ok ActsGetAPILastMessage is set
 */
ACTS_COMMON_API ActsStatus ActsAPIVersion_ValidateVersion2(uint32_t buildVersion);

/*
 * check that the API version is the same as the built version
 * @return status, if not ok ActsGetAPILastMessage is set
 */
#define ActsAPIVersion_ValidateVersion() ActsAPIVersion_ValidateVersion2(ACTS_API_BUILD_VERSION_ID)

#endif //__ACTS_API_VERSION_H__