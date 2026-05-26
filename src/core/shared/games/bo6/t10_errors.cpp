#include <includes_shared.hpp>
#include <games/bo6/t10_errors.hpp>
#include <games/bo6/t10_errors_generated.hpp>
#include <utils/utils.hpp>

namespace games::bo6::errors {
	const char* TranslateError(uint32_t code, bool defaultMessage) {
		for (auto& err : ERRORS) {
			if (err.code == code) {
				return err.msg;
			}
		}
		return defaultMessage ? utils::va("Bo6 Error %u", code) : nullptr;

	}
}