#pragma once
#include <hook/memory.hpp>

#define S_ANY __declspec(selectany)
namespace hook::refs {
    template<typename T = void>
    class Ref {
      public:
        T* ptr;
        Ref() : ptr(nullptr) {}
        Ref(size_t loc) : ptr(reinterpret_cast<T*>(loc)) {}
        Ref(void* ptr) : ptr(reinterpret_cast<T*>(ptr)) {}

        constexpr operator T*() const { return ptr; }

        constexpr T* operator->() const { return ptr; }

        constexpr T* Get() { return ptr; }
    };

    template<typename T = void>
    class MultiRef {
      public:
        std::vector<void*> ptrs;
        MultiRef() : ptrs() {};

        constexpr T** Get() { return (T**)ptrs.data(); }
        constexpr size_t Count() const { return ptrs.size(); }

        void ForEach(std::function<void(T* v)> callback) {
            for (void* ptr : ptrs) {
                callback((T*)ptr);
            }
        }

        void Nulled() {
            for (void* ptr : ptrs) {
                hook::memory::Nulled(ptr);
            }
        }

        void ReturnVal(bool val) {
            for (void* ptr : ptrs) {
                hook::memory::ReturnVal(ptr, val);
            }
        }

        template<size_t _Count>
        void Patch(const byte (&patchData)[_Count]) {
            for (void* ptr : ptrs) {
                hook::process::WriteMemSafe(ptr, patchData, _Count);
            }
        }
    };
} // namespace hook::refs
byte* operator"" _g(size_t val);
byte* operator"" _a(size_t val);