#ifndef __ACTS_API_STRUCTS_H__
#define __ACTS_API_STRUCTS_H__
#include "api.h"
/*
 * Atian Tools Data Structure API
 */


// std::vector<byte> equivalent
ACTS_COMMON_API ActsHandle ActsAPIStructs_VectorData();
ACTS_COMMON_API uint8_t* ActsAPIStructs_VectorData_GetData(ActsHandle handle);
ACTS_COMMON_API size_t ActsAPIStructs_VectorData_Size(ActsHandle handle);
ACTS_COMMON_API void ActsAPIStructs_VectorData_Resize(ActsHandle handle, size_t len);
ACTS_COMMON_API void ActsAPIStructs_VectorData_AddData(ActsHandle handle, void* data, size_t len);




#ifdef __cplusplus
// c++ helpers

template<typename T>
inline void ActsAPIStructs_VectorData_AddData(ActsHandle handle, T data) {
	ActsAPIStructs_VectorData_AddData(handle, &data, sizeof(T));
}

#endif // __cplusplus

#endif // __ACTS_API_STRUCTS_H__