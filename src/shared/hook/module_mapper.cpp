#include <includes_shared.hpp>
#include <utils/utils.hpp>
#include "module_mapper.hpp"

namespace hook::module_mapper {

	Module::Module(bool freeOnExit) : freeOnExit(freeOnExit) {

	}
	Module::~Module() {
		if (freeOnExit) Free();
	}

	bool Module::Load(const std::filesystem::path& path, bool patchIAT, bool absolute) {
		Free(); // free module if required
		std::filesystem::path ap{ absolute ? std::filesystem::absolute(path) : path };
		std::string p{ ap.string() };
		lib.SetModule(LoadLibraryExA(p.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | DONT_RESOLVE_DLL_REFERENCES));

		if (!lib) return false;
		
		// patch lib
		if (patchIAT) {
			lib.PatchIAT();
		}
		LOG_TRACE("Module loaded {}", lib);

		return true;
	}
	void Module::Free() {
		if (!lib) return; // nothing to free
		// free and set the module to null
		lib.Free();
		lib.ClearModule();
	}


}