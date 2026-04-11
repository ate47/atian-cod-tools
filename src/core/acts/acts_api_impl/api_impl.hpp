#pragma once
#include <core/memory_allocator.hpp>

/*
 * @return The allocator for the acts API returns, used after ActsAPICloseHandle()
 */
core::memory_allocator::MemoryAllocator& ActsAPIImpl_GetAllocator();

/*
 * Allocate a new pointer in the API memory, this pointer can be cleaned using ActsAPICloseHandle
 * @param args constructor params
 * @return object
 */
template<typename T = void, typename... Args>
inline T* ActsAPIImpl_New(Args... args) {
	return ActsAPIImpl_GetAllocator().New<T, Args...>(args...);
}