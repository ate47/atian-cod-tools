#pragma once
#include <filesystem>
#include <vector>

namespace utils {
	bool ReadFile(const std::filesystem::path& path, LPVOID& buffer, size_t& size);
	bool WriteFile(const std::filesystem::path& path, LPCVOID ptr, size_t size);
}