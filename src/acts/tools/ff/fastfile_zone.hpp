#pragma once
#include <core/memory_allocator.hpp>

namespace fastfile::zone {
	class Zone {
		core::memory_allocator::MemoryAllocator alloc{};
	public:
		std::unordered_map<std::string, std::vector<const char*>> assets;
		std::unordered_map<std::string, const char*> cfgs;

		void ParseFile(const std::string& file, std::function<void(core::logs::loglevel lvl, size_t line, const std::string& message)> errorHandler);

		void ParseFile(const std::string& file, const char* filename = nullptr);

		const char* GetConfig(const char* name, const char* defaultVal = nullptr);
		int64_t GetConfigInt(const char* name, int64_t defaultVal = 0);
		bool GetConfigBool(const char* name, bool defaultVal = false);
	};
}