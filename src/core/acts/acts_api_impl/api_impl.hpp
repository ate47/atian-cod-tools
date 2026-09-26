#pragma once
#include <acts_api/api.h>
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

inline std::vector<byte>& ActsAPIImpl_VectorData(ActsHandle handle) {
    return *reinterpret_cast<std::vector<byte>*>(handle);
}
/*
 * Execute run or if an exception is thrown, fail with ActsAPISetLastMessage set to its message
 * @param run action
 * @return return of run or ACTS_STATUS_ERROR and ActsAPISetLastMessage
 */
ActsStatus ActsAPIImpl_ErrHandler(std::function<ActsStatus()> run);

/*
 * Execute run or if an exception is thrown, fail with ActsAPISetLastMessage set to its message
 * @param run action
 * @return return ok status or ACTS_STATUS_ERROR and ActsAPISetLastMessage
 */
ActsStatus ActsAPIImpl_ErrHandler(std::function<void()> run);

// check sized struct for member
// ptr*, member, defaultVal
#define SIZED_STRUCT_MEMBER(strct, member, defaultVal)                                                                 \
    ((offsetof(std::remove_pointer_t<decltype(strct)>, member) + sizeof((strct)->member) <= (strct)->structSize)       \
         ? (strct)->member                                                                                             \
         : (defaultVal))
