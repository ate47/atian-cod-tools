#ifndef __ACTS_API_VERSION_H__
#define __ACTS_API_VERSION_H__
#include "api.h"
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

#endif //__ACTS_API_VERSION_H__