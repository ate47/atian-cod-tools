#pragma once


namespace core::memory_allocator {

	class MemoryAllocator {
		size_t totalLen{};
		std::vector<void*> ptrs{};

	public:
		~MemoryAllocator() {
			for (void* ptr : ptrs) {
				delete[] ptr;
			}
		}

		template<typename T = void>
		T* Alloc(size_t len) {
			totalLen += len;
			void* ptr = new byte[len];
			ptrs.push_back(ptr);
			return (T*)ptr;
		}

		template<typename T = void>
		T* Alloc(const std::string& str) {
			T* ptr = Alloc<T>(str.length() + 1);
			std::memcpy(ptr, str.data(), str.length() + 1);
			return ptr;
		}

		const char* CloneStr(const char* str) {
			size_t len = std::strlen(str);
			char* ptr = Alloc<char>(len + 1);
			std::memcpy(ptr, str, len + 1);
			return ptr;
		}
	};

}