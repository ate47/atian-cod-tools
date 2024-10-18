#pragma once

namespace core::strings {
	template<typename T>
	void TrimIdx(const T& str, size_t& start, size_t& end) {
		while (start < end && std::isspace(str[start])) {
			start++;
		}
		while (end > start && std::isspace(str[end - 1])) {
			end--;
		}
	}

	template<typename T>
	std::string Trim(const T& str, size_t start, size_t end) {
		TrimIdx<T>(str, start, end);
		return { &str[start], &str[end] };
	}
}