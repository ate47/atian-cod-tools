#pragma once
#include <core/memory_allocator.hpp>
#include <compatibility/scobalula_csi.hpp>

namespace fastfile::pool {
	class FFAssetPool {
		std::vector<std::unordered_map<uint64_t, void*>> pool{};
		std::vector<std::unique_ptr<std::vector<byte>>> memory{};
		core::memory_allocator::MemoryAllocator refsAllocator{};

	public:
		compatibility::scobalula::csi::CordycepProc csiHeader{};
		compatibility::scobalula::csi::XAssetPool64 cordPool{};

		FFAssetPool() {
			csiHeader.poolsAddress = reinterpret_cast<uintptr_t>(&cordPool);
			csiHeader.stringSize = 0;
		};

		FFAssetPool(const FFAssetPool&) = delete;
		FFAssetPool(FFAssetPool&&) = delete;

		void* FindAssetHeader(size_t type, uint64_t name);
		void AddAssetHeader(size_t type, uint64_t name, void* header);
		void ClearRef();
		std::vector<byte>& CreateMemoryBuffer();
	};
}