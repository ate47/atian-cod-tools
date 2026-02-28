#pragma once

namespace deps::dzporter::cdb {
	constexpr uint32_t CDB_MAGIC = 0x42444E50;

	struct NameDatabaseHeader {
		uint32_t magic;
		uint32_t entries;
		uint32_t compressedSize;
		uint32_t decompressedSize;
	};

	bool ReadCDBFiles(std::filesystem::path p, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len) = nullptr, std::mutex* loadMutex = nullptr);
	bool ReadCDBFile(std::filesystem::path p, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len) = nullptr, std::mutex* loadMutex = nullptr);
	void CompressCDBFile(std::unordered_map<uint64_t, const char*>& dataMap, const std::filesystem::path& out);
	void CompressCDBFile(std::map<std::string, std::unordered_set<uint64_t>>& dataMap, const std::filesystem::path& out);
	void LoadHashFile(std::filesystem::path p);
}