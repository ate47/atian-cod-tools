#pragma once

namespace compatibility::scobalula::wnigen{
	int CompressWNIFile(std::map<std::string, std::unordered_set<uint64_t>>& dataMap, const std::filesystem::path& out);
	int CompressWNIFile(std::unordered_map<uint64_t, const char*>& dataMap, const std::filesystem::path& out);
}