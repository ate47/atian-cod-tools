#include <includes_shared.hpp>

namespace hash {

	bool TryHashPattern(const char* str, uint64_t& outVal) {
		 std::string_view v{ str };

		 if (!v.rfind("var_", 0)) {
			 outVal = std::strtoull(&str[4], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("event_", 0)) {
			 outVal = std::strtoull(&str[6], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("function_", 0)) {
			 outVal = std::strtoull(&str[9], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("namespace_", 0)) {
			 outVal = std::strtoull(&str[10], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("script_", 0)) {
			 outVal = std::strtoull(&str[7], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("hash_", 0)) {
			 outVal = std::strtoull(&str[5], nullptr, 16);
			 return true;
		 }
		 if (!v.rfind("file_", 0)) {
			 outVal = std::strtoull(&str[5], nullptr, 16);
			 return true;
		 }
		 return false;
	}
}