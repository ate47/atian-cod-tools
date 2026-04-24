#include <includes.hpp>
#include <format>
#include <acts_api/logs.h>

ActsAPILog_Level ActsAPILog_GetLevel() {
	return (ActsAPILog_Level)core::logs::getlevel();
}

void ActsAPILog_SetLevel(ActsAPILog_Level level) {
	core::logs::setlevel((core::logs::loglevel)level);
}

void ActsAPILog_Log2(ActsAPILog_Level level, const char* header, const char* file, size_t line, const char* str, bool endl) {
	core::logs::log((core::logs::loglevel)level, header, file, line, str, endl);
}

void ActsAPILog_Log(ActsAPILog_Level level, const char* file, size_t line, const char* str, bool endl) {
	core::logs::log((core::logs::loglevel)level, file, line, str, endl);
}

void ActsAPILog_Logf(ActsAPILog_Level level, const char* file, size_t line, bool endl, const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	ActsAPILog_Log(level, file, line, utils::vap(fmt, va), endl);
	va_end(va);
}

void ActsAPILog_Log2f(ActsAPILog_Level level, const char* header, const char* file, size_t line, bool endl, const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	ActsAPILog_Log(level, file, line, utils::vap(fmt, va), endl);
	va_end(va);
}