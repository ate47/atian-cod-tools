#include <includes.hpp>
#include "tools/gsc_formatter.hpp"

namespace tool::gsc::formatter {
	namespace {
		FormatterInfo actsOldFormatter{ "acts_old", FFL_DEFAULT };
		FormatterInfo seriousFormatter{ "serious", FFL_NEWLINE_AFTER_BLOCK_START | FFL_NO_SPACE_AFTER_CONTROL | FFL_ONE_LINE_HEADER_COMMENTS | FFL_NOERROR_STR | FFL_SWITCH_PAD_CASES | FFL_SWITCH_FORCE_BLOCKS | FFL_FUNC_HEADER_FORMAT_SERIOUS };
		FormatterInfo xenFormatter{ "xen", FFL_NEWLINE_AFTER_BLOCK_START | FFL_SPACE_BEFOREAFTER_PARAMS | FFL_NOERROR_STR | FFL_NO_FUNCTION_TITLE | FFL_BLOCK_INLINE | FFL_NEWLINES_BETWEEN_BLOCKS | FFL_LINE_AFTER_COPYRIGHT | FFL_NO_BOOL_ANALYSIS | FFL_SWITCH_PAD_CASES | FFL_FUNC_HEADER_FORMAT_NONE };
		FormatterInfo treyarchFormatter{ "treyarch", FFL_NEWLINE_AFTER_BLOCK_START | FFL_SPACE_BEFOREAFTER_PARAMS | FFL_NEWLINES_BETWEEN_BLOCKS | FFL_LINE_AFTER_COPYRIGHT | FFL_SWITCH_PAD_CASES | FFL_ANIM_REAL };
		FormatterInfo iwFormatter{ "iw", FFL_NEWLINE_AFTER_BLOCK_START | FFL_SPACE_BEFOREAFTER_PARAMS | FFL_NO_FUNCTION_TITLE | FFL_NEWLINES_BETWEEN_BLOCKS | FFL_LINE_AFTER_COPYRIGHT | FFL_SWITCH_PAD_CASES };

		std::vector<FormatterInfo*> formatters{
			&treyarchFormatter,
			&seriousFormatter,
			&xenFormatter,
			&iwFormatter,
			&actsOldFormatter,
		};

	}

	const FormatterInfo& GetDefaultFormatter() {
		return treyarchFormatter;
	}

	const FormatterInfo& GetFromName(const char* name) {
		if (!name) {
			return GetDefaultFormatter();
		}

		for (const FormatterInfo* fmt : formatters) {
			if (!_strcmpi(name, fmt->name)) {
				return *fmt;
			}
		}
		return GetDefaultFormatter();
	}

	const std::vector<FormatterInfo*>& GetFormatters() {
		return formatters;
	}

}