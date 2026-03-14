#include <includes.hpp>
#include <hksclua.hpp>

namespace {
	int lua_hksc(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		hksc_StateSettings settings;
		hksc_State* H;

		hksI_StateSettings(&settings);
		H = hksI_newstate(&settings);
		if (!H) {
			LOG_ERROR("Cannot create luafile state: not enough memory");
			return tool::BASIC_ERROR;
		}

		utils::CloseEnd ceH{ [H] { hksI_close(H); } };

		std::filesystem::path output;
		if (tool::NotEnoughParam(argc, 2)) {
			output = argv[2];
			output.replace_extension(".luac");
		}
		else {
			output = argv[3];
		}

		utils::OutFileCE os{ output, true, std::ios::binary };

		lua_setmode(H, HKSC_MODE_SOURCE);
		lua_setbytecodestrippinglevel(H, BYTECODE_STRIPPING_ALL);
		int status{
			hksI_parser_file(H, argv[2], [](hksc_State* H, void* ud) -> int {
				return lua_dump(H, [](hksc_State* H, const void* p, size_t size, void* u) -> int {
					utils::OutFileCE* outluac = (utils::OutFileCE*)u;

					utils::WriteValue(*outluac, p, size);

					return 0;
				}, ud);
			}, & os)
		};

		if (status) {
			LOG_ERROR("Can't compile lua {}: {}", argv[2], lua_geterror(H));
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Compiled into {}", output.string());

		return tool::OK;
	};

	ADD_TOOL(hksc, "lib", " [file] (out=file.luac)", "Compile bo4 lua using integrated hks compiler", lua_hksc);
}