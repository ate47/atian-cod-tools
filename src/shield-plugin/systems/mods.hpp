#pragma once
#include <core/memory_allocator.hpp>
#include <core/bytebuffer.hpp>

namespace mods {
	struct Mod {
		std::unordered_map<uint64_t, uint64_t> redirects[bo4::XAssetType::ASSET_TYPE_COUNT]{};
		std::unordered_map<uint64_t, void*> assets[bo4::XAssetType::ASSET_TYPE_COUNT]{};
		void* data{};
		size_t dataLen{};

		core::bytebuffer::ByteBuffer CreateReader() {
			return { (byte*)data, dataLen };
		}
	};
	class ModContainer {
		std::unordered_map<uint64_t, uint64_t> redirects[bo4::XAssetType::ASSET_TYPE_COUNT]{};
		std::unordered_map<uint64_t, void*> assets[bo4::XAssetType::ASSET_TYPE_COUNT]{};
		std::vector<Mod*> mods{};
		core::memory_allocator::MemoryAllocator commonAlloc{};

	public:
		ModContainer() {}
		// Redirect a name
		void ApplyRedirects(bo4::XAssetType type, bo4::XHash* name);
		// load the xmod files
		void LoadContainer(const std::filesystem::path& directory);
		// sync the container data
		void SyncContainer();
		// clean mod data
		void ClearContainer();
		// create a mod
		Mod* AllocMod();
		// free a mod
		void FreeMod(Mod* mod);
		// Find the asset in the registered
		void* FindAsset(bo4::XAssetType type, bo4::XHash* name);
		// Does the asset exists in the container
		bool DoesAssetExist(bo4::XAssetType type, bo4::XHash* name);
	};

}
