#pragma once

#define S_ANY __declspec(selectany)
namespace hook::refs {
	template<typename T>
	class Ref {
	public:
		T* ptr;
		Ref(size_t loc) : ptr(reinterpret_cast<T*>(loc)) {}
		Ref(void* ptr) : ptr(reinterpret_cast<T*>(ptr)) {}

		constexpr operator T* () const {
			return ptr;
		}

		constexpr T* operator->() const {
			return ptr;
		}
	};
}
byte* operator"" _g(size_t val);
byte* operator"" _a(size_t val);