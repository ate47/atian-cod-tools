#pragma once
#include <data/bo4.hpp>
#include <core/config.hpp>
#include <core/memory_allocator.hpp>

namespace systems::mods {
	struct ModLoadingHook {
		void (*hook)(const char* modid, core::memory_allocator::MemoryAllocator& alloc, core::config::Config& modCfg);
	};
	typedef void (ModAssetLinkHook)(bo4::XAsset* asset);

	std::vector<ModLoadingHook*>& GetModLoadingHooks();
	std::vector<ModAssetLinkHook*>& GetModAssetLinkHooks();
}
