#ifndef __ACTS_API_CONFIG_H__
#define __ACTS_API_CONFIG_H__
#include "api.h"
/*
 * Atian Tools Config API
 */

// config data for enum
typedef struct {
	// str name
	const char* name;
	// value
	int64_t enumValue;
} ActsAPIConfig_ConfigEnumData;

//#include <core/config.hpp>
// ActsAPIConfig_


/*
 * Set the config file
 * @param path config file path
 */
ACTS_COMMON_API void ActsAPIConfig_SetMainConfig(const char* path);
/*
 * Load and save the config
 * @param save save the config
 * @return if the config were loaded
 */
ACTS_COMMON_API bool ActsAPIConfig_SyncConfig(bool save);

/*
 * Save the config
 */
ACTS_COMMON_API void ActsAPIConfig_SaveConfig();

// get functions
ACTS_COMMON_API int64_t ActsAPIConfig_GetInteger(const char* path, int64_t ACTS_DEFAULT(defaultValue, 0));
ACTS_COMMON_API double ActsAPIConfig_GetDouble(const char* path, double ACTS_DEFAULT(defaultValue, 0));
ACTS_COMMON_API const char* ActsAPIConfig_GetString(const char* path, const char* ACTS_DEFAULT(defaultValue, ""));
ACTS_COMMON_API bool ActsAPIConfig_GetBool(const char* path, bool ACTS_DEFAULT(defaultValue, false));
ACTS_COMMON_API int64_t ActsAPIConfig_GetEnum(const char* path, ActsAPIConfig_ConfigEnumData* data, size_t dataCount, int64_t ACTS_DEFAULT(defaultEnumValue, 0));

// set functions
ACTS_COMMON_API void ActsAPIConfig_SetInteger(const char* path, int64_t value);
ACTS_COMMON_API void ActsAPIConfig_SetDouble(const char* path, double value);
ACTS_COMMON_API void ActsAPIConfig_SetString(const char* path, const char* value);
ACTS_COMMON_API void ActsAPIConfig_SetBool(const char* path, bool value);
ACTS_COMMON_API void ActsAPIConfig_SetEnum(const char* path, int64_t enumValue, ActsAPIConfig_ConfigEnumData* data, size_t dataCount);


#endif // __ACTS_API_CONFIG_H__