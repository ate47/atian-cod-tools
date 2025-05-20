#pragma once

namespace deps::scobalula::wni {
	constexpr const char* packageIndexDir = "package_index";
	constexpr uint32_t WNI_MAGIC = 0x20494E57;
	constexpr uint16_t WNI_VERSION = 1;
	constexpr size_t WNI_HEADER_SIZE = sizeof(WNI_MAGIC) + sizeof(WNI_VERSION) + sizeof(uint32_t) * 3;

	bool ReadWNIFiles(std::filesystem::path p, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len) = nullptr, std::mutex* loadMutex = nullptr);
	bool ReadWNIFile(std::filesystem::path p, std::function<void(uint64_t hash, const char* str)> each, void* (*allocMemory)(size_t len) = nullptr, std::mutex* loadMutex = nullptr);
	void LoadHashFile(std::filesystem::path p);
}