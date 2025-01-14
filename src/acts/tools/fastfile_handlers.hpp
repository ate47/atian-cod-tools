#pragma once

namespace fastfile {

	class FFAssetPool {
		std::vector<std::unordered_map<uint64_t, void*>> pool{};
	public:
		FFAssetPool() {};

		void* FindAssetHeader(size_t type, uint64_t name);
		void AddAssetHeader(size_t type, uint64_t name, void* header);
		void ClearRef();
	};
}