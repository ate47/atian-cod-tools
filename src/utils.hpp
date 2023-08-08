#pragma once
#include <Windows.h>
#include <filesystem>
#include <vector>

namespace utils {
	bool ReadFileNotAlign(const std::filesystem::path& path, LPVOID& buffer, size_t& size);
	bool ReadFileAlign(const std::filesystem::path& path, LPVOID& buffer, LPVOID& bufferAligned, size_t& size, size_t& sizeAligned);
	bool WriteFile(const std::filesystem::path& path, LPCVOID ptr, size_t size);

	template<typename Type>
	inline BYTE* Aligned(BYTE* ptr) {
		return reinterpret_cast<BYTE*>((reinterpret_cast<uintptr_t>(ptr) + (sizeof(Type) - 1)) & ~(sizeof(Type) - 1));
	}
}