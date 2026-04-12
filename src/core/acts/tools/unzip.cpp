#include <includes.hpp>
#include <deps/miniz.hpp>
#include <core/memory_allocator.hpp>

namespace {
	
	void* MINIZAlloc(void* opaque, size_t items, size_t size) {
		core::memory_allocator::MemoryAllocator& alloc{ *(core::memory_allocator::MemoryAllocator*)opaque };
		size_t len{ size * items };
		byte* d{ alloc.Alloc<byte>(len + sizeof(len)) };
		*(uint64_t*)d = len;
		d += sizeof(len);
		LOG_TRACE("MINIZAlloc({}, {}) -> {}", items, size, (void*)d);
		return d;
	}
	void MINIZFree(void* opaque, void* address) {
		core::memory_allocator::MemoryAllocator& alloc{ *(core::memory_allocator::MemoryAllocator*)opaque };
		LOG_TRACE("MINIZFree({})", address);
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
		LOG_TRACE("MINIZRealloc({}, {}, {}) -> {}", address, items, size, p);
		return p;
	}

	int zip_test(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) {
			return tool::BAD_USAGE;
		}
		core::memory_allocator::MemoryAllocator alloc{};
		mz_zip_archive zip;
		std::memset(&zip, 0, sizeof(zip));

		zip.m_pAlloc_opaque = &alloc;
		zip.m_pAlloc = MINIZAlloc;
		zip.m_pFree = MINIZFree;
		zip.m_pRealloc = MINIZRealloc;

		if (!mz_zip_reader_init_file(&zip, argv[2], 0)) {
			LOG_ERROR("mz_zip_reader_init_file fail");

			return tool::BASIC_ERROR;
		}

		LOG_INFO("file loaded");
		

		return tool::OK;
	}
	int zlib_extract(int argc, const char* argv[]) {
		if (argc == 2) {
			LOG_ERROR("Missing config file");
			return tool::BAD_USAGE;
		}

		std::filesystem::path f(argv[2]);

		void* buff = NULL;
		size_t size = 0;

		if (!utils::ReadFileNotAlign(f, buff, size, false)) {
			LOG_ERROR("Can't read file '{}'", f.string());
			return tool::BASIC_ERROR;
		}

		byte unzippedData[5000];

		uLongf sizef = (uLongf)size;
		uLongf sizef2;


		if (uncompress2(unzippedData, &sizef, reinterpret_cast<const Bytef*>(buff), &sizef2) < 0) {
			LOG_ERROR("Too big");
			std::free(buff);
			return tool::BASIC_ERROR;
		}


		auto output = f.parent_path() / (f.filename().string() + "e");
		utils::WriteFile(output, &unzippedData[0], sizef2);

		LOG_INFO("extract info '{}'", output.string());

		std::free(buff);

		return tool::OK;
	}

}


ADD_TOOL(zip_test, "common", " [file]", "zip test", zip_test);
ADD_TOOL(zlib, "common", " [file]", "zlib extract", zlib_extract);