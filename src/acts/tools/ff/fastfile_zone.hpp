#pragma once
#include <core/memory_allocator.hpp>

namespace fastfile::zone {
	class Zone {
		core::memory_allocator::MemoryAllocator alloc{};
	public:
		std::unordered_map<std::string, std::vector<const char*>> assets;
		std::unordered_map<std::string, const char*> cfgs;

		void ParseFile(const std::string& file);
	};
}