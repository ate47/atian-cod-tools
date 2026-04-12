#include <includes.hpp>
#include <acts_api/config.h>
#include <core/config.hpp>


void ActsAPIConfig_SetMainConfig(const char* path) {
	core::config::SetMainConfig(path);
}
bool ActsAPIConfig_SyncConfig(bool save) {
	return core::config::SyncConfig(save);
}
void ActsAPIConfig_SaveConfig() {
	core::config::SaveConfig();
}
int64_t ActsAPIConfig_GetInteger(const char* path, int64_t defaultValue) {
	return core::config::GetInteger(path, defaultValue);
}
double ActsAPIConfig_GetDouble(const char* path, double defaultValue) {
	return core::config::GetDouble(path, defaultValue);
}
const char* ActsAPIConfig_GetString(const char* path, const char* defaultValue) {
	return core::config::GetCString(path, defaultValue);
}
bool ActsAPIConfig_GetBool(const char* path, bool defaultValue) {
	return core::config::GetBool(path, defaultValue);
}

void ActsAPIConfig_SetInteger(const char* path, int64_t value) {
	core::config::SetInteger(path, value);
}
void ActsAPIConfig_SetDouble(const char* path, double value) {
	core::config::SetDouble(path, value);
}
void ActsAPIConfig_SetString(const char* path, const char* value) {
	core::config::SetString(path, value);
}
void ActsAPIConfig_SetBool(const char* path, bool value) {
	core::config::SetBool(path, value);
}
int64_t ActsAPIConfig_GetEnum(const char* path, ActsAPIConfig_ConfigEnumData* data, size_t dataCount, int64_t defaultEnumValue) {
	return core::config::GetEnum(path, (core::config::ConfigEnumData*) data, dataCount, defaultEnumValue);
}
void ActsAPIConfig_SetEnum(const char* path, int64_t enumValue, ActsAPIConfig_ConfigEnumData* data, size_t dataCount) {
	core::config::SetEnum(path, enumValue,(core::config::ConfigEnumData*)data, dataCount);
}
