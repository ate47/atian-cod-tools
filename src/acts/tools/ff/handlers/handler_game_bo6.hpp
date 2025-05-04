#pragma once
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/bo6/bo6.hpp>

namespace fastfile::handlers::bo6 {
	using namespace ::bo6;

	class Worker {
	public:
		virtual void Unlink(fastfile::FastFileOption& opt, void* ptr) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	std::unordered_map<T10RAssetType, Worker*>& GetWorkers();
}