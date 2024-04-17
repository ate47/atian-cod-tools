#include <includes.hpp>
#include "tools/gsc_formatter.hpp"


#pragma once

namespace tool::gsc::formatter {
	namespace {
		FormatterInfo defaultFormatter{ "default", FFL_DEFAULT };
		FormatterInfo seriousFormatter{ "serious", FFL_NEWLINE_AFTER_BLOCK_START | FFL_NO_SPACE_AFTER_CONTROL | FFL_ONE_LINE_HEADER_COMMENTS | FFL_NOERROR_STR };
		FormatterInfo xenFormatter{ "xen", FFL_NEWLINE_AFTER_BLOCK_START | FFL_SPACE_BEFOREAFTER_PARAMS | FFL_NOERROR_STR };

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