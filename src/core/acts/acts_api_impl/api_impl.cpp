#include <includes.hpp>
#include <acts_api_impl/api_impl.hpp>
#include <acts_api/api.h>
#include <acts_api/version.h>
#include <core/actsinfo.hpp>

namespace {
	thread_local char actsApiLastMessageBuffer[0x400]{};
	core::memory_allocator::MemoryAllocator allocator;
}

core::memory_allocator::MemoryAllocator& ActsAPIImpl_GetAllocator() {
	return allocator;
}

unsigned int ActsAPIVersion_GetBuildVersion() {
	return core::actsinfo::BUILD_VERSION_ID;
}
const char* ActsAPIVersion_GetVersion() {
	return core::actsinfo::VERSION;
}
unsigned int ActsAPIVersion_GetVersionId() {
	return core::actsinfo::VERSION_ID;
}
const char* ActsGetAPILastMessage() {
	return actsApiLastMessageBuffer;
}

void ActsAPISetLastMessage(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(actsApiLastMessageBuffer, sizeof(actsApiLastMessageBuffer), fmt, args);
	va_end(args);
}

void ActsAPICloseHandle(ActsHandle handle) {
	if (IS_HANDLE_VALID(handle)) {
		allocator.Free(handle);
	}
}

ActsStatus ActsAPIVersion_ValidateVersion2(uint32_t buildVersion) {
	if (ACTS_API_BUILD_VERSION_ID == buildVersion) {
		return ACTS_STATUS_OK;
	}
	ActsAPISetLastMessage("Unmatching build 0x%x != 0x%x", ACTS_API_BUILD_VERSION_ID, buildVersion);
	return  ACTS_STATUS_ERROR;
}
