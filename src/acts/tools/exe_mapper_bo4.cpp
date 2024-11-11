#include <includes.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>

namespace {
	struct {

	} bo4;

	hook::library::Detour Com_Error_Detour;

	void Com_Error_Stub(uint32_t code) {
		throw std::runtime_error(utils::va("Com_Error(0x%x)", code));
	}

	int bo4_exe_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		hook::module_mapper::Module mod{};
		if (!mod.Load(argv[2])) {
			LOG_ERROR("Can't map module {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Module loaded");

		Com_Error_Detour.Create((*mod)[0x288B110], Com_Error_Stub);

		LOG_INFO("Hook loaded");



		return tool::OK;
	}

	ADD_TOOL(bo4_exe_mapper, "dev", "[exe]", "test bo4 mapping", bo4_exe_mapper);
}