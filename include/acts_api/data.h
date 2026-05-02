#ifndef __ACTS_API_DATA_H__
#define __ACTS_API_DATA_H__
#include "api.h"
/*
 * Atian Tools Data API
 */

// default game data parent
#define GAME_DATA_BASE_PARENT "scans"

// game data

/*
 * Create a new game data handle, the handle should be freed with ActsAPICloseHandle
 * @param dirname game data directory name
 * @param loadGameModule load the game module (if available)
 * @param customGameModule custom game module path, if not null, it will be used instead of the default one in the game data
 * @param loadDecrypt also load the game module for decryption, if loadGameModule is false, this option will be ignored
 * @return game data handle, INVALID_ACTS_HANDLE_VALUE if it fails with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsHandle ActsAPIData_NewGameData(const char* dirname, bool loadGameModule, const char* customGameModule, bool loadDecrypt);

/*
 * Validate all scans in the config
 * @param gameData game data handle
 * @return true if the scans are valid, false otherwise
 */
ACTS_COMMON_API bool ActsAPIData_ValidateScans(ActsHandle gameData);
/*
 * Apply all the null scans
 * @param gameData game data handle
 * @param id null scan group
 * @return ACTS_STATUS_OK if the null scans are applied, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPIData_ApplyNullScans(ActsHandle gameData, const char* id);
/*
 * Get the module name (if available)
 * @param gameData game data handle
 * @return module name or null if unavailable
 */
ACTS_COMMON_API const char* ActsAPIData_GetModuleName(ActsHandle gameData);
/*
 * Get a config value
 * @param gameData game data handle
 * @param path config path, for example "scans.my_scan.ctype"
 * @return config value or null if unavailable
 */
ACTS_COMMON_API const char* ActsAPIData_GetConfigCString(ActsHandle gameData, const char* path);
/*
 * Redirect a scan or a null scan to a new pointer
 * @param gameData game data handle
 * @param id scan id
 * @param to new pointer
 * @param parent scan parent, by default "scans", can be "nullscans" for null scans
 * @return ACTS_STATUS_OK if the null scans are applied, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPIData_Redirect(ActsHandle gameData, const char* id, void* to, const char* ACTS_DEFAULT(parent, GAME_DATA_BASE_PARENT));
/*
 * Null a scan or a null scan
 * @param gameData game data handle
 * @param id scan id
 * @param parent scan parent, by default "scans", can be "nullscans" for null scans
 * @return ACTS_STATUS_OK if the null scans are applied, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPIData_Nulled(ActsHandle gameData, const char* id, const char* ACTS_DEFAULT(parent, GAME_DATA_BASE_PARENT));
/*
 * Get a scan value
 * @param gameData game data handle
 * @param id scan id
 * @param to output value
 * @param parent scan parent, by default "scans", can be "nullscans" for null scans
 * @return ACTS_STATUS_OK if the null scans are applied, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPIData_Get(ActsHandle gameData, const char* id, void** to, const char* ACTS_DEFAULT(parent, GAME_DATA_BASE_PARENT));

// decryption

/*
 * Load a decryption module to be able to decrypt strings with ActsAPIData_DecryptString
 * @param path decryption module path
 * @return if the module was loaded successfully
 */
ACTS_COMMON_API bool ActsAPIData_LoadDecryptModule(char* path);
/*
 * Decrypt a string with the loaded decryption module, the decryption module should be loaded with ActsAPIData_LoadDecryptModule before using this function
 * @param str string to decrypt
 * @return the decrypted string, if no decryption module was loaded, str is returned
 */
ACTS_COMMON_API char* ActsAPIData_DecryptString(char* str);

#endif // __ACTS_API_DATA_H__