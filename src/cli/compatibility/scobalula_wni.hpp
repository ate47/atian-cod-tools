#pragma once

#include <includes_shared.hpp>

namespace compatibility::scobalula::wni {
	constexpr const char* packageIndexDir = "package_index";
	constexpr uint32_t WNI_MAGIC = 0x20494E57;
	constexpr uint16_t WNI_VERSION = 1;

	bool ReadWNIFiles(std::filesystem::path p, std::function<void(uint64_t hash, const char* str)> each);
}