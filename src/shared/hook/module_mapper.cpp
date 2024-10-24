#include <includes_shared.hpp>
#include <utils.hpp>
#include "module_mapper.hpp"

namespace hook::module_mapper {

	Module::Module(bool freeOnExit) : freeOnExit(freeOnExit) {

	}
	Module::~Module() {
		if (freeOnExit) Free();
	}

	bool Module::Load(const std::filesystem::path& path) {
		Free(); // free module if required
		std::filesystem::path ap{ std::filesystem::absolute(path) };
		std::string p{ ap.string() };
		lib.SetModule(LoadLibraryExA(p.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | DONT_RESOLVE_DLL_REFERENCES));

		if (!lib) return false;
		
		// patch lib
		lib.PatchIAT();

		return true;
	}
	void Module::Free() {
		if (!lib) return; // nothing to free
		// free and set the module to null
		lib.Free();
		lib.SetModule(NULL);
	}


}