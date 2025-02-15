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
		T* Alloc(size_t size = sizeof(T), size_t count = 1) {
			totalLen += size * count;
			void* ptr = new byte[size * count];
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
		const char* CloneStr(const std::string& str) {
			return CloneStr(str.data());
		}

		void FreeAll() {
			for (void* ptr : ptrs) {
				delete[] ptr;
			}
			ptrs.clear();
		}

		void Free(void* ptr) {
			if (!ptr) return;

			auto it = ptrs.begin();
			while (it != ptrs.end()) {
				if (*it == ptr) {
					delete[] ptr;
					it = ptrs.erase(it);
				}
				else {
					it++;
				}
			}
		}
	};

}