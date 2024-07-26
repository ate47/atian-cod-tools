#include <includes.hpp>
#include "tools/gsc_formatter.hpp"

namespace tool::gsc::formatter {
	namespace {
		FormatterInfo defaultFormatter{ "default", FFL_DEFAULT };
		FormatterInfo seriousFormatter{ "serious", FFL_NEWLINE_AFTER_BLOCK_START | FFL_NO_SPACE_AFTER_CONTROL | FFL_ONE_LINE_HEADER_COMMENTS | FFL_NOERROR_STR | FFL_SWITCH_PAD_CASES | FFL_SWITCH_FORCE_BLOCKS | FFL_FUNC_HEADER_FORMAT_SERIOUS };
		FormatterInfo xenFormatter{ "xen", FFL_NEWLINE_AFTER_BLOCK_START | FFL_SPACE_BEFOREAFTER_PARAMS | FFL_NOERROR_STR | FFL_NO_FUNCTION_TITLE | FFL_BLOCK_INLINE | FFL_NEWLINES_BETWEEN_BLOCKS | FFL_LINE_AFTER_COPYRIGHT | FFL_NO_BOOL_ANALYSIS | FFL_SWITCH_PAD_CASES | FFL_FUNC_HEADER_FORMAT_NONE };

		std::vector<FormatterInfo*> formatters{
			&defaultFormatter,
			&seriousFormatter,
			&xenFormatter,
		};

	}

	const FormatterInfo& GetFromName(const char* name) {
		if (!name) {
			return defaultFormatter;
		}

		for (const auto* fmt : formatters) {
			if (!_strcmpi(name, fmt->name)) {
				return *fmt;
			}
		}
		return defaultFormatter;
	}

	const std::vector<FormatterInfo*>& GetFormatters() {
		return formatters;
	}

}