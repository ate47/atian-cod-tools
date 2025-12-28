#include <includes.hpp>
#include "fastfile_asset_pool.hpp"

namespace fastfile::pool {
	void* FFAssetPool::FindAssetHeader(size_t type, uint64_t name) {
		if (type >= pool.size()) return nullptr;
		auto it{ pool[type].find(name) };
		if (it == pool[type].end()) return nullptr;
		return it->second;
	}

	void FFAssetPool::AddAssetHeader(size_t type, uint64_t name, void* header) {
		if (type >= pool.size()) {
			pool.resize(type + 1);
		}
		pool[type][name] = header;
	}

	void FFAssetPool::ClearRef() {
		pool.clear();
		memory.clear();
		refsAllocator.FreeAll();
	}

	std::vector<byte>& FFAssetPool::CreateMemoryBuffer() {
		return *memory.emplace_back(std::make_unique<std::vector<byte>>()).get();
	}
}