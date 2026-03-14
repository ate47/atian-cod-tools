#pragma once

namespace core::memory_allocator {
	template<typename DataRefType = uint32_t>
	struct MemoryAllocatorStaticHeader {
		DataRefType prev;
		DataRefType next;
		DataRefType len;
		bool allocated;
	};

	template<size_t Len, typename DataRefType = uint32_t>
	class MemoryAllocatorStatic {
		byte data[Len + sizeof(MemoryAllocatorStaticHeader<DataRefType>)]{};
		static_assert(sizeof(data) <= (1ull << (sizeof(DataRefType) << 3)) && "DataRefType max size smaller than Len with header");

	public:
		MemoryAllocatorStatic() {
			MemoryAllocatorStaticHeader<DataRefType>& header{ *(MemoryAllocatorStaticHeader<DataRefType>*)&data[0] };
			header.prev = 0;
			header.next = 0;
			header.len = Len;
			header.allocated = false;
		}
		void* DataByRef(DataRefType ref) {
			return &data[ref];
		}
	private:
		MemoryAllocatorStaticHeader<DataRefType>* HeaderByRef(DataRefType ref) {
			return (MemoryAllocatorStaticHeader<DataRefType>*)DataByRef(ref);
		}
	public:

		DataRefType RefByData(void* ptr) {
			return (DataRefType)((byte*)ptr - (byte*)&data[0]);
		}
		DataRefType AllocRef(size_t len) {
			MemoryAllocatorStaticHeader<DataRefType>* header{ HeaderByRef(0) };

			while (header->allocated || header->len < len) {
				if (!header->next) throw std::runtime_error("Not enough data in the allocator");
				header = HeaderByRef(header->next);
			}

			header->allocated = true;
			void* ptr = header + 1;

			if (len + sizeof(MemoryAllocatorStaticHeader<DataRefType>) < header->len) {
				// we can create a block after this block
				MemoryAllocatorStaticHeader<DataRefType>* next = (MemoryAllocatorStaticHeader<DataRefType>*)((byte*)ptr + len);

				next->prev = RefByData(header);
				next->next = header->next;
				next->len = (DataRefType)(header->len - len - sizeof(MemoryAllocatorStaticHeader<DataRefType>));
				header->next = RefByData(next);
				header->len = (DataRefType)len;

			}
			return RefByData(ptr);
		}

		void* Alloc(size_t len) {
			return DataByRef(AllocRef(len));
		}

		template<typename T = void>
		T* Alloc(size_t len) {
			return (T*)DataByRef(AllocRef(sizeof(T) * len));
		}
		
		void FreeRef(DataRefType ref) {
			MemoryAllocatorStaticHeader<DataRefType>& header{ *((HeaderByRef(ref)) - 1) };
			header.allocated = false;
			if (header.next) {
				MemoryAllocatorStaticHeader<DataRefType>* nh{ HeaderByRef(header.next) };
				// the next isn't allocated, we can merge the 2 blocks
				if (!nh->allocated) {
					if (nh->next) {
						HeaderByRef(nh->next)->prev = RefByData(&header);
					}
					header.len += nh->len;
					header.next = nh->next;
				}
			}
			if (header.prev) {
				MemoryAllocatorStaticHeader<DataRefType>* ph{ HeaderByRef(header.prev) };
				if (!ph->allocated) {
					// we can remove this block
					ph->next = header.next;
					ph->len += header.len;
				}
			}
		}
		void Free(void* ptr) {
			FreeRef(RefByData(ptr));
		}

		const char* Alloc(const std::string& str) {
			const char* cstr = Alloc<const char>(str.length() + 1);
			std::memcpy(cstr, str.data(), str.length());
			return cstr;
		}
	};

}