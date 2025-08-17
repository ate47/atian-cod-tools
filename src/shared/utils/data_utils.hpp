#pragma once

namespace utils::data {

	void WriteHex(std::ostream& out, uintptr_t base, void* buff, size_t size);

	std::string AsHex(const void* buff, size_t size);

	template<typename T>
	std::string ArrayAsString(
		const T* array, size_t count, 
		const char* delim = ", ", const char* prefix = "[", const char* suffix = "]", 
		std::function<std::string(const T&)> format = [](const T& t) { return std::format("{}", t); }
	) {
		if (!array) {
			return "nullptr[]";
		}
		std::stringstream ss{};
		ss << prefix;
		for (size_t i = 0; i < count; i++) {
			if (i) ss << delim;
			ss << format(array[i]);
		}
		ss << suffix;

		return ss.str();
	}
	bool IsNulled(const void* buff, size_t size);
}