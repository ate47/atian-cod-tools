#pragma once
#include <core/memory_allocator.hpp>
#include <compiler/preprocessor.hpp>

namespace fastfile::zone {
	class Zone {
		core::memory_allocator::MemoryAllocator alloc{};
	public:
		acts::compiler::preprocessor::PreProcessorOption preProcOpt{};
		std::unordered_map<std::string, std::vector<const char*>> assets;
		std::unordered_map<std::string, const char*> cfgs;

		void ParseFile(const std::filesystem::path& path, std::string& file, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler, size_t depth = 0);

		void ParseFile(const std::filesystem::path& path, std::string& file, size_t depth = 0);

		const char* GetConfig(const char* name, const char* defaultVal = nullptr);
		int64_t GetConfigInt(const char* name, int64_t defaultVal = 0);
		bool GetConfigBool(const char* name, bool defaultVal = false);
	};
}