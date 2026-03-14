#include <includes_shared.hpp>
#include <core/strings.hpp>
#include <utils/utils.hpp>

namespace core::strings {
	const char* GetCppIdentifier(const char* name) {
		return utils::MapString(utils::CloneString(name), [](char c) -> char {
			c = std::toupper(c);

			if (!(c == '_' || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
				return '_'; // remap bad char
			}

			return c;
		});
	}
}