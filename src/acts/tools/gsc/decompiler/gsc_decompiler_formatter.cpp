#include <includes.hpp>
#include <core/strings.hpp>
#include <cli/clicolor.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_formatter.hpp>

namespace tool::gsc::formatter {
	namespace {
		constexpr const char* CUSTOM_FORMAT_FILE = "custom_format.txt";

		class CustomFormatterInfo : public FormatterInfo {
			bool loaded{};
		public:
			CustomFormatterInfo(const char* name) : FormatterInfo(name, FFL_DEFAULT) {}

			void InitFormatter() override {
				if (loaded) {
					return;
				}
				loaded = true;

				std::filesystem::path file{ utils::GetProgDir() / CUSTOM_FORMAT_FILE };
				utils::InFileCE input{ file };

				bool anyError{};
				if (!input) {
					LOG_ERROR("Can't load custom format file");
					anyError = true;
				}
				else {
					std::string line{};

					flags = 0;
					while (*input && std::getline(*input, line, '\n')) {
						line = core::strings::Trim(line, 0, line.length());

						if (line.empty() || line[0] == '#') {
							continue; // comment/empty
						}


						const FormatterFlagInfo* flag{ FlagInfoFromName(line.data()) };
						if (!flag) {
							LOG_ERROR("Invalid format name: {}, use gscdf to get the format names list", line);
							anyError = true;
							continue;
						}

						if (flag->mask && (flag->mask & flags)) {
							const char* nameOther{ FlagName((FormatterFlags)(flag->mask & flags)) };
							LOG_ERROR("Can't use format {}, not compatible with {}", flag->name, nameOther ? nameOther : "<error>" );
							anyError = true;
							continue;
						}

						flags |= flag->value;
					}
				}


				if (anyError) {
					throw std::runtime_error(std::format("Error in custom file data: {}", file.string()));
				}

			}
		};


		CustomFormatterInfo customFormatter{ "custom" };
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
			&customFormatter,
		};

		FormatterFlagInfo flagInfo[]{
			{ "NO_SPACE_AFTER_CONTROL", FFL_NO_SPACE_AFTER_CONTROL, FFL_EMPTY_MASK, "no space after controls (if, for, etc.)"},
			{ "NEWLINE_AFTER_BLOCK_START", FFL_NEWLINE_AFTER_BLOCK_START, FFL_EMPTY_MASK, "new line after block start" },
			{ "SPACE_BEFOREAFTER_PARAMS", FFL_SPACE_BEFOREAFTER_PARAMS, FFL_EMPTY_MASK, "use space between params" },
			{ "ONE_LINE_HEADER_COMMENTS", FFL_ONE_LINE_HEADER_COMMENTS, FFL_EMPTY_MASK, "use /**/ instead of // for decompiler comments" },
			{ "NOERROR_STR", FFL_NOERROR_STR, FFL_EMPTY_MASK, "use \"\" in missing strings (error/dev)"},
			{ "NO_FUNCTION_TITLE", FFL_NO_FUNCTION_TITLE, FFL_EMPTY_MASK, "do not add the \"function\" prefix for function definition" },
			{ "BLOCK_INLINE", FFL_BLOCK_INLINE, FFL_EMPTY_MASK, "inline block when too small" },
			{ "NEWLINES_BETWEEN_BLOCKS", FFL_NEWLINES_BETWEEN_BLOCKS, FFL_EMPTY_MASK, "add empty line between instruction blocks and controls" },
			{ "LINE_AFTER_COPYRIGHT", FFL_LINE_AFTER_COPYRIGHT, FFL_EMPTY_MASK, "add empty line between copyright and the script"},
			{ "NO_BOOL_ANALYSIS", FFL_NO_BOOL_ANALYSIS, FFL_EMPTY_MASK, "disable boolean analysis" },
			{ "SWITCH_PAD_CASES", FFL_SWITCH_PAD_CASES, FFL_EMPTY_MASK, "add padding in switch cases" },
			{ "SWITCH_FORCE_BLOCKS", FFL_SWITCH_FORCE_BLOCKS, FFL_EMPTY_MASK, "use block for switch cases"},
			{ "FUNC_HEADER_FORMAT_SERIOUS", FFL_FUNC_HEADER_FORMAT_SERIOUS, FFL_FUNC_HEADER_FORMAT_MASK, "use serious function header format" },
			{ "FUNC_HEADER_FORMAT_NONE", FFL_FUNC_HEADER_FORMAT_NONE, FFL_FUNC_HEADER_FORMAT_MASK, "remove function header" },
			// { "FUNC_HEADER_FORMAT_TYPE_4", FFL_FUNC_HEADER_FORMAT_TYPE_4, "unused" },
			{ "SWITCH_FORCE_BLOCKS_PADDING", FFL_SWITCH_FORCE_BLOCKS_PADDING, FFL_EMPTY_MASK, "add new line between switch blocks" },
			{ "ANIM_REAL", FFL_ANIM_REAL, FFL_EMPTY_MASK, "render #animtree and %anim instead of %animtree::anim" },
		};
	}

	const FormatterFlagInfo* FlagInfoFromName(const char* name) {
		for (FormatterFlagInfo& info : flagInfo) {
			if (!_strcmpi(name, info.name)) {
				return &info;
			}
		}

		return nullptr;
	}

	const char* FlagName(FormatterFlags flag) {
		for (FormatterFlagInfo& info : flagInfo) {
			if (info.value = flag) {
				return info.name;
			}
		}

		return nullptr;
	}

	const FormatterInfo& GetDefaultFormatter() {
		return treyarchFormatter.InitAndGet();
	}

	const FormatterInfo* GetFromName(const char* name) {
		if (!name) {
			return &GetDefaultFormatter();
		}

		for (FormatterInfo* fmt : formatters) {
			if (!_strcmpi(name, fmt->name)) {
				return &fmt->InitAndGet();
			}
		}
		return nullptr;
	}

	const std::vector<FormatterInfo*>& GetFormatters() {
		return formatters;
	}

	int gscdf(int argc, const char* argv[]) {
		std::filesystem::path file{ utils::GetProgDir() / CUSTOM_FORMAT_FILE };

		LOG_INFO("custom format names: ({})", ARRAYSIZE(flagInfo));

		for (FormatterFlagInfo& info : flagInfo) {
			LOG_INFO("{}{}{} : {}", cli::clicolor::COLOR_RED, info.name, cli::clicolor::CD_RESET, info.desc);
		}

		return tool::OK;
	}

	int gscdfp(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) {
			LOG_INFO("formatters list ():", formatters.size());

			for (const FormatterInfo* fmt : formatters) {
				LOG_INFO("{}", fmt->name);
			}

			return tool::OK;
		}

		const FormatterInfo* formatter{ GetFromName(argv[2]) };

		if (!formatter) {
			LOG_ERROR("Invalid formatter name: {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Format(s) for formatter {}", formatter->name);
		uint64_t flags{ formatter->flags };

		if (!flags) {
			LOG_INFO("default format");
		}

		for (const FormatterFlagInfo& flag : flagInfo) {
			if (!flag.value) {
				continue; // default
			}
			if (flag.mask) {
				if ((flags & flag.mask) != flag.value) continue;
			}
			else {
				if ((flags & flag.value) != flag.value) continue;
			}
			LOG_INFO("{}", flag.name);
		}

		return tool::OK;
	}

	ADD_TOOL(gscdf, "gsc", "", "GSC decompiler formatter info", gscdf);
	ADD_TOOL(gscdfp, "gsc", " [format]", "Print GSC decompiler formatter info", gscdfp);
}