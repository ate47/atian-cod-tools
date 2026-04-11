#ifndef __ACTS_API_API_H__
#define __ACTS_API_API_H__
#include <stdint.h>
/*
 * Atian Tools Common API commons
 */


#ifndef ACTS_COMMON_API
#define ACTS_EXTERN_C extern "C"
#ifdef ACTS_COMMON_DLL
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllexport)
#else
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllimport)
#endif
#endif

enum ActsStatus {
	ACTS_STATUS_OK = 0,
	ACTS_STATUS_ERROR = -1,
};

// acts handle, should be closed with ActsAPICloseHandle
typedef void* ActsHandle;

#define INVALID_ACTS_HANDLE_VALUE ((ActsHandle)INVALID_HANDLE_VALUE)
// test if an ActsHandle is valid
#define IS_HANDLE_VALID(handle) ((handle) && (handle) != INVALID_ACTS_HANDLE_VALUE)

// test if an ActsStatus value is OK
#define ACTS_OK(status) ((status) >= ACTS_STATUS_OK)
// test if an ActsStatus value is not OK (error)
#define ACTS_NOT_OK(status) ((status) < ACTS_STATUS_OK)

/*
 * @return Get last API message for this thread
 */
ACTS_COMMON_API const char* ActsGetAPILastMessage();

/*
 * Get the last message for ActsGetAPILastMessage()
 * @param fmt format
 * @param ... format params
 */
ACTS_COMMON_API void ActsAPISetLastMessage(const char* fmt, ...);

/*
 * Close an ActsHandle
 * @param handle handle
 */
ACTS_COMMON_API void ActsAPICloseHandle(ActsHandle handle);


#endif // __ACTS_API_API_H__