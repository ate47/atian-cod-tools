#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace fastfile::handlers::bo4 {
	struct XHash {
		uint64_t hash;
		uint64_t pad;
	};
	class Worker {
	public:
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
	};

	std::unordered_map<games::bo4::pool::XAssetType, Worker*>& GetWorkers();
}