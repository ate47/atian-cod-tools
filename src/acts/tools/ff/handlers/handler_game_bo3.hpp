#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/bo3/pools.hpp>

namespace fastfile::handlers::bo3 {
	class Worker {
	public:
		virtual void Unlink(fastfile::FastFileOption& opt, void** ptr) = 0;
	};

	std::unordered_map<::bo3::pool::T7XAssetType, Worker*>& GetWorkers();
}