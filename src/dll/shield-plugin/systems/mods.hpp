#pragma once
#include <core/config.hpp>
#include <core/memory_allocator.hpp>

namespace systems::mods {
	struct ModLoadingHook {
		void (*hook)(const char* modid, core::memory_allocator::MemoryAllocator& alloc, core::config::Config& modCfg);
	};

	std::vector<ModLoadingHook*>& GetModLoadingHooks();
}
