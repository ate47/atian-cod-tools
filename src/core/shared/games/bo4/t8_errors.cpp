#include <includes_shared.hpp>
#include <games/bo4/t8_errors.hpp>
#include <games/bo4/t8_errors_generated.hpp>
#include <core/memory_allocator.hpp>
#include <utils/utils.hpp>

namespace games::bo4::errors {


	const char* TranslateError(uint32_t code, bool defaultMessage) {
		for (auto& err : T89_ERRORS) {
			if (err.code == code) {
				return err.msg;
			}
		}
		return defaultMessage ? utils::va("Bo4 Error %u", code) : nullptr;

	}
}