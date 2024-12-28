#pragma once

namespace tool::hash::text_expand {
	constexpr const char dict_number[] = "0123456789";
	constexpr const char dict_alpha[] = "abcdefghijklmnopqrstuvwxyz";
	constexpr const char dict_alpha_number[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	constexpr const char cdict[] = "abcdefghijklmnopqrstuvwxyz0123456789_/";
	void GetDynamicPtr(size_t max, void(*func)(const char* str, void* data), void* data = nullptr, const char* dict = cdict, size_t n = sizeof(cdict) - 1);

	template<typename T = void>
	void GetDynamic(size_t max, void(*func)(const char* str, T* data), T* data = nullptr) {
		GetDynamicPtr(max, reinterpret_cast<void(*)(const char* str, void* data)>(func), (void*)data);
	}
	void GetDynamicAsyncPtr(size_t max, void(*func)(const char* str, void* data), void* data = nullptr, const char* dict = cdict, size_t n = sizeof(cdict) - 1);

	template<typename T = void>
	void GetDynamicAsync(size_t max, void(*func)(const char* str, T* data), T* data = nullptr, const char* dict = cdict, size_t n = sizeof(cdict) - 1) {
		GetDynamicAsyncPtr(max, reinterpret_cast<void(*)(const char* str, void* data)>(func), (void*)data, dict, n);
	}

	void GetDynamicAsyncDictPtr(size_t max, void(*func)(const char** str, void* data), const char** dict, void* data = nullptr);

	template<typename T = void>
	void GetDynamicAsyncDict(size_t max, void(*func)(const char** str, T* data), const char** dict, T* data = nullptr) {
		GetDynamicAsyncDictPtr(max, reinterpret_cast<void(*)(const char** str, void* data)>(func), dict, (void*)data);
	}
}