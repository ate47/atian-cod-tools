#pragma once
#include <core/memory_allocator.hpp>

namespace fastfile::compiled_zone {
	class CompiledZone {
		core::memory_allocator::MemoryAllocator alloc{};
		std::vector<size_t> offsets{};
	public:


		void DumpDebug(const std::filesystem::path& out);
	};

}