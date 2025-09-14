#pragma once
#include <utils/utils.hpp>

namespace core::memory_allocator {

	struct MemoryAllocatorPtr {
		void(*deleteFunc)(void* ptr);
		void* ptr;
	};

	class MemoryAllocator {
		size_t totalLen{};
		std::vector<MemoryAllocatorPtr> ptrs{};

	public:
		MemoryAllocator() {}
		
		MemoryAllocator(MemoryAllocator&& other) noexcept
			: totalLen(other.totalLen), ptrs(std::move(other.ptrs)) {
			other.totalLen = 0;
			other.ptrs.clear();
		}

		~MemoryAllocator() {
			for (MemoryAllocatorPtr& ptr : ptrs) {
				if (ptr.ptr) {
					ptr.deleteFunc(ptr.ptr);
				}
			}
		}

		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(MemoryAllocator&& other) = delete;


		template<typename T = void, typename... Args>
		T* New(Args... args) {
			totalLen += sizeof(T);
			T* ptr = new T(std::forward<Args>(args)...);
			ptrs.emplace_back([](void* p) { delete static_cast<T*>(p); }, (void*)ptr);
			return ptr;
		}

		template<typename T = void>
		T* Alloc(size_t size = sizeof(T), size_t count = 1) {
			totalLen += size * count;
			void* ptr = new byte[size * count];
			ptrs.emplace_back([](void* p) { delete[] static_cast<byte*>(p); }, (void*)ptr);
			return (T*)ptr;
		}

		template<typename T = void>
		T* AllocAligned(size_t size, size_t align) {
			return (T*)utils::Aligned(Alloc<T>(size + align - 1), align);
		}

		template<typename T>
		T* Add(T* ptr) {
			ptrs.emplace_back([](void* p) { delete static_cast<T*>(p); }, (void*)ptr);
			return ptr;
		}

		template<typename T = void>
		T* Alloc(const std::string& str) {
			T* ptr = Alloc<T>(str.length() + 1);
			std::memcpy(ptr, str.data(), str.length() + 1);
			return ptr;
		}

		template<typename T = void>
		T* ClonePtr(const T* src, size_t len) {
			void* dst = Alloc<byte>(len);
			std::memcpy(dst, src, len);
			return (T*)dst;
		}

		const char* CloneStr(const char* str) {
			size_t len = std::strlen(str);
			return ClonePtr<const char>(str, len + 1);
		}
		const char* CloneStr(const std::string& str) {
			return CloneStr(str.data());
		}

		void FreeAll() {
			for (MemoryAllocatorPtr& ptr : ptrs) {
				ptr.deleteFunc(ptr.ptr);
			}
			ptrs.clear();
		}

		void Free(void* ptr) {
			if (!ptr) return;

			auto it = ptrs.begin();
			while (it != ptrs.end()) {
				if (it->ptr == ptr) {
					it->deleteFunc(it->ptr);
					ptrs.erase(it);
					break;
				}

				it++;
			}
		}

		size_t Count() const {
			return ptrs.size();
		}

		size_t DebugAllocSize() const {
			return totalLen;
		}
	};

}