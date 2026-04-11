#ifndef __ACTS_API_API_H__
#define __ACTS_API_API_H__
#include <stdint.h>
#include <stdbool.h>
/*
 * Atian Tools Common API commons
 */


#if __cplusplus
#define ACTS_DEFAULT(name, value) name = (value)
#else // !__cplusplus
#define ACTS_DEFAULT(name, value) name
#endif // __cplusplus

#ifndef ACTS_COMMON_API
#if __cplusplus
#define ACTS_EXTERN_C extern "C"
#else // !__cplusplus
#define ACTS_EXTERN_C extern
#endif // __cplusplus
#ifdef ACTS_COMMON_DLL
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllexport)
#else // !ACTS_COMMON_DLL
# define ACTS_COMMON_API ACTS_EXTERN_C __declspec(dllimport)
#endif // ACTS_COMMON_DLL
#endif

typedef enum {
	ACTS_STATUS_OK = 0,
	ACTS_STATUS_ERROR = -1,
	ACTS_STATUS_ASSERTION_ERROR = -2,
} ActsStatus;

// acts handle, should be closed with ActsAPICloseHandle
typedef void* ActsHandle;

#define INVALID_ACTS_HANDLE_VALUE ((ActsHandle)0)
// test if an ActsHandle is valid
#define IS_ACTS_HANDLE_VALID(handle) ((handle) != INVALID_ACTS_HANDLE_VALUE)

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


// assert assertion or set message using ActsAPISetLastMessage and return ACTS_STATUS_ASSERTION_ERROR
#define ACTS_API_ASSERT_MESSAGE(assertion, message) \
do { \
	if (!(assertion)) {\
		ActsAPISetLastMessage("%s", message); \
		return ACTS_STATUS_ASSERTION_ERROR; \
	} \
} while (0)
// assert that val isn't null
#define ACTS_API_ASSERT_NOT_NULL(val) ACTS_API_ASSERT_MESSAGE((val), #val " can't be null")
// assert val, use #val as assertion message
#define ACTS_API_ASSERT(val) ACTS_API_ASSERT_MESSAGE((val), #val)
// assert that handle is a valid handle
#define ACTS_API_ASSERT_VALID_HANDLE(handle) ACTS_API_ASSERT_MESSAGE(IS_ACTS_HANDLE_VALID(handle), #handle " isn't a valid handle")

#endif // __ACTS_API_API_H__