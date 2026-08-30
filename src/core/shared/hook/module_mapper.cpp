#include <includes_shared.hpp>
#include <utils/utils.hpp>
#include "module_mapper.hpp"

namespace hook::module_mapper {

    Module::Module(bool freeOnExit) : freeOnExit(freeOnExit) {}
    Module::~Module() {
        if (freeOnExit)
            Free();
    }

    bool Module::Load(const std::filesystem::path& path, bool patchIAT, bool absolute) {
        Free(); // free module if required
        std::filesystem::path ap{ absolute ? std::filesystem::absolute(path) : path };
        std::string p{ ap.string() };
        int t = platform::LSF_SEARCH_DEFAULT_DIRS | platform::LSF_DONT_RESOLVE_REFERENCES;
        lib.SetModule(
            platform::LoadShared(p.c_str(), platform::LSF_SEARCH_DEFAULT_DIRS | platform::LSF_DONT_RESOLVE_REFERENCES)
        );
        scanContainer.Load(lib);
        scanContainer.logger = &logger;
        logger.Clean();

        if (!lib)
            return false;

        originBase = (uintptr_t)platform::GetLibBase(p.c_str());

        // patch lib
        if (patchIAT) {
            lib.PatchIAT();
        }

        LOG_TRACE("Module loaded {} [originBase={:x}]", *this, originBase);

        return true;
    }
    void Module::Free() {
        if (!lib)
            return; // nothing to free
        // free and set the module to null
        lib.Free();
        lib.ClearModule();
        scanContainer.Load(lib);
        logger.Clean();
        originBase = 0;
    }

    hook::scan_container::ScanContainer& Module::GetScanContainer(bool forceReload) {
        if (forceReload) {
            scanContainer.ForceReload();
        }
        scanContainer.Sync(false);
        return scanContainer;
    }

    hook::library::ScanLogger& Module::GetScanLogger() { return logger; }

    void* Module::RebasePtr(uintptr_t origin) const {
        if (originBase > origin || originBase + lib.ModuleInformation().SizeOfImage() < origin) {
            return (void*)origin; // not in modue
        }
        return lib[origin - originBase];
    }
    std::ostream& operator<<(std::ostream& out, const Module& ptr) { return out << ptr.lib; }
} // namespace hook::module_mapper