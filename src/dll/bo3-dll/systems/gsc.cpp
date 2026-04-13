#include <dll_includes.hpp>
#include <core/system.hpp>
#include <core/memory_allocator.hpp>
#include <deps/miniz.hpp>

namespace {
	hook::library::Detour mz_zip_reader_init_file_Detour;
	core::memory_allocator::MemoryAllocator zipAllocator{};

	void* MINIZAlloc(void* opaque, size_t items, size_t size) {
		core::memory_allocator::MemoryAllocator& alloc{ *(core::memory_allocator::MemoryAllocator*)opaque };
		size_t len{ size * items };
		byte* d{ alloc.Alloc<byte>(len + sizeof(len)) };
		*(uint64_t*)d = len;
		d += sizeof(len);
		//LOG_TRACE("MINIZAlloc({}, {}) -> {}", items, size, (void*)d);
		return d;
	}

	void MINIZFree(void* opaque, void* address) {
		core::memory_allocator::MemoryAllocator& alloc{ *(core::memory_allocator::MemoryAllocator*)opaque };
		//LOG_TRACE("MINIZFree({})", address);
		alloc.Free(((byte*)address) - sizeof(size_t));
	}

	void* MINIZRealloc(void* opaque, void* address, size_t items, size_t size) {
		core::memory_allocator::MemoryAllocator& alloc{ *(core::memory_allocator::MemoryAllocator*)opaque };
		void* p{};
		if (size) {
			p = MINIZAlloc(opaque, size, items);
			if (address) {
				size_t oldLen{ *(size_t*)((byte*)address - sizeof(size_t)) };
				size_t len{ size * items };
				if (oldLen > len) {
					oldLen = len;
				}
				std::memcpy(p, address, oldLen);
			}
		}
		alloc.Free(address);
		//LOG_TRACE("MINIZRealloc({}, {}, {}) -> {}", address, items, size, p);
		return p;
	}


	mz_bool __fastcall mz_zip_reader_init_file_Stub(mz_zip_archive* pZip, const char* pFilename, mz_uint32 flags) {
		// the g_ScriptDebugHunk hunk user isn't a thing in bo3, so we need to replace the allocation
		pZip->m_pAlloc = MINIZAlloc;
		pZip->m_pFree = MINIZFree;
		pZip->m_pRealloc = MINIZRealloc;
		pZip->m_pAlloc_opaque = &zipAllocator;
		mz_bool r{ mz_zip_reader_init_file_Detour.Call<mz_bool>(pZip, pFilename, flags) };
		LOG_TRACE("mz_zip_reader_init_file_Stub({}, {}, {}) = {} ({})", (void*)pZip, pFilename, flags, r ? "true" : "false", hook::library::CodePointer{_ReturnAddress()});
		return r;
	}

	void PostInit(uint64_t id) {
		mz_zip_reader_init_file_Detour.Create(acts::Scan().ScanSingle("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B D9").GetPtr(), mz_zip_reader_init_file_Stub);
	}

	REGISTER_SYSTEM(gsc, nullptr, PostInit);
}