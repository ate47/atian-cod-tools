#pragma once

namespace tool::gsc::formatter {
	enum FormatterFlags : uint64_t {
		FFL_DEFAULT = 0,
		FFL_NO_SPACE_AFTER_CONTROL = 1,
		FFL_NEWLINE_AFTER_BLOCK_START = 1 << 1,
		FFL_SPACE_BEFOREAFTER_PARAMS = 1 << 2,
		FFL_ONE_LINE_HEADER_COMMENTS = 1 << 3,
		FFL_NOERROR_STR = 1 << 4,
		FFL_NO_FUNCTION_TITLE = 1 << 5,
		FFL_BLOCK_INLINE = 1 << 6,
		FFL_NEWLINES_BETWEEN_BLOCKS = 1 << 7,
		FFL_LINE_AFTER_COPYRIGHT = 1 << 8,
		FFL_NO_BOOL_ANALYSIS = 1 << 9,
		FFL_SWITCH_PAD_CASES = 1 << 10,
		FFL_SWITCH_FORCE_BLOCKS = 1 << 11,
		FFL_FUNC_HEADER_FORMAT1 = 1 << 12,
		FFL_FUNC_HEADER_FORMAT2 = 1 << 13,
		FFL_FUNC_HEADER_FORMAT_MASK = FFL_FUNC_HEADER_FORMAT1 | FFL_FUNC_HEADER_FORMAT2,
		FFL_FUNC_HEADER_FORMAT_SERIOUS = FFL_FUNC_HEADER_FORMAT1,
		FFL_FUNC_HEADER_FORMAT_NONE = FFL_FUNC_HEADER_FORMAT2,
		FFL_FUNC_HEADER_FORMAT_TYPE_4 = FFL_FUNC_HEADER_FORMAT1 | FFL_FUNC_HEADER_FORMAT2,
	};
	struct FormatterInfo {
		const char* name;
		uint64_t flags{};

		// features:
		// before/after param space
		// new line before block start
		// space between if/for/while/switch and (
		// space between () and { (if inline)
		// one line header comments
	};

	constexpr FormatterFlags GetHeaderFormat(uint64_t flags) {
		return (FormatterFlags)(flags & FFL_FUNC_HEADER_FORMAT_MASK);
	}

	const FormatterInfo& GetFromName(const char* name = nullptr);
	const std::vector<FormatterInfo*>& GetFormatters();

}