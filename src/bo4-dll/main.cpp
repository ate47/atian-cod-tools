#include <dll_includes.hpp>
clisync::CliSyncData inject::g_cliData{};

std::vector<inject::DetourRegistryData*>& detourRegistryData() {
    static std::vector<inject::DetourRegistryData*> vec{};
    return vec;
};

void inject::RegisterDetour(DetourRegistryData* data) {
    detourRegistryData().emplace_back(data);
}

void inject::InjectedSystem::InjectSystem(InjectedSystem* sys) {
    if (sys) {
        GetSystems().push_back(sys);
    }
}

std::vector<inject::InjectedSystem*>& inject::InjectedSystem::GetSystems() {
    static std::vector<InjectedSystem*> sys{};
    return sys;
}

inject::DllType inject::g_dllType = inject::DLLTYPE_UNKNOWN;

namespace {
    std::mutex dllStart;

    bool CanStart(inject::DllType type) {
        std::lock_guard lg{ dllStart };

        if (inject::g_dllType) {
            return false;
        }

        inject::g_dllType = type;

        return true;
    }

    using inject::g_cliData;

    void PreInitDll() {
        auto& systems = inject::InjectedSystem::GetSystems();
        std::sort(systems.begin(), systems.end(), [](const inject::InjectedSystem* e1, const inject::InjectedSystem* e2) { return e1->level < e2->level; });

        HMODULE modStart = process::BaseHandle();

        LOG_INFO("Init ACTS BO4 DLL with {} system(s) using type {}", systems.size(), (char)('0' + inject::g_dllType));
        LOG_INFO("workdir: {0}", g_cliData.workDir);
        LOG_INFO("modstrt: {:#08x}", reinterpret_cast<uintptr_t>(modStart));

        LOG_DEBUG("Preinit systems");
        // pre init systems
        for (auto* sys : systems) {
            if (sys->preinit) {
                LOG_DEBUG("- system {}", sys->name);
                sys->preinit();
            }
        }
        LOG_DEBUG("Preinit systems done");
    }

    void PostInitDll() {
        auto& systems = inject::InjectedSystem::GetSystems();
        std::sort(systems.begin(), systems.end(), [](const inject::InjectedSystem* e1, const inject::InjectedSystem* e2) { return e1->level < e2->level; });
        LOG_DEBUG("Postinit ACTS BO4 DLL with {} system(s)", systems.size());

        // post init systems
        LOG_DEBUG("Post init systems");
        for (auto* sys : systems) {
            if (sys->postinit) {
                LOG_DEBUG("postinit system {}", sys->name);
                sys->postinit();
            }
        }
        LOG_DEBUG("Post init systems done");
    }

    void SyncCLIOnce(clisync::CliSyncData* data) {
        g_cliData = *data;

        auto logpath = (std::filesystem::path{ g_cliData.workDir } / "acts-bo4.log").string();
        alogs::setfile(logpath.c_str());
        if (g_cliData.HasFeature(clisync::FEATURE_LOG)) {
            alogs::setlevel(alogs::LVL_DEBUG);
        }

        PreInitDll();
        LOG_INFO("Sync CLI - Postinit");
        PostInitDll();
        LOG_INFO("Sync CLI - Done");
    }

    void PatchStart() {
        CanStart(inject::DLLTYPE_CLIENT); // set client type, ignore the rest

        GetCurrentDirectoryA(sizeof(g_cliData.execDir), g_cliData.execDir);
        GetCurrentDirectoryA(sizeof(g_cliData.workDir), g_cliData.workDir);

#ifdef DEBUG
        g_cliData.features = alogs::LVL_DEBUG;
#endif

        auto logpath = (std::filesystem::path{ g_cliData.workDir } / "acts-bo4.log").string();
        alogs::setfile(logpath.c_str());
        if (g_cliData.HasFeature(clisync::FEATURE_LOG)) {
            alogs::setlevel(alogs::LVL_DEBUG);
        }

        // clear start
        auto start = process::PImageOptHeader()->AddressOfEntryPoint;
        auto modStart = process::BaseHandle();

        if (!modStart) {
            LOG_ERROR("BAD MODULE START, ABORT");
            return;
        }

        inject::DetourInfo<void> dStart{ "Start", start, []() {}, inject::PRE_INIT };

        PreInitDll();

        LOG_INFO("Start client");

        // now we can start the game
        dStart();
    }

    void PathDetours(inject::DetourTime time) {
        LOG_INFO("Install {} detours...", (time == inject::DetourTime::POST_INIT ? "POST" : "PRE"));

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        auto error = false;
        LONG err;
        for (auto& detourData : detourRegistryData()) {
            if (detourData->time != time) {
                continue;
            }
            LOG_DEBUG("detouring {}@{:x}->{:x}", detourData->title, reinterpret_cast<uintptr_t>(detourData->pointer), reinterpret_cast<uintptr_t>(detourData->detour));
            err = DetourAttach(reinterpret_cast<PVOID*>(detourData->pointer), detourData->detour);

            if (err != NO_ERROR) {
                error = true;
                LPSTR messageBuffer = nullptr;
                FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                LOG_ERROR("Error when attaching detour {}: {} ({:x})", detourData->title, messageBuffer, err);
                LocalFree(messageBuffer);
                break;
            }
        }

        if (!error) {
            err = DetourTransactionCommit();
            if (err != NO_ERROR) {
                error = true;
                LPSTR messageBuffer = nullptr;
                FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                LOG_ERROR("Error when commit transaction: {} ({:x})", messageBuffer, err);
                LocalFree(messageBuffer);
            }
        }

        if (error) {
            DetourTransactionAbort();
            return;
        }

        LOG_INFO("Detours installed... ({})", (time == inject::DetourTime::POST_INIT ? "POST" : "PRE"));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
#ifdef CLIENT_TEST
        PatchStart();
#endif
        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#ifdef CLIENT_TEST
// call d3d11 base implementation
EXPORT HRESULT D3D11CreateDevice(void* adapter, const uint64_t driver_type,
	const HMODULE software, const UINT flags,
	const void* p_feature_levels, const UINT feature_levels,
	const UINT sdk_version, void** device, void* feature_level,
	void** immediate_context) {
	static auto func = [] {
			return reinterpret_cast<decltype(&D3D11CreateDevice)>(
                GetProcAddress(process::LoadSysLib("d3d11.dll"), "D3D11CreateDevice")
            );
	}();

	return func(adapter, driver_type, software, flags, p_feature_levels, feature_levels, 
        sdk_version, device, feature_level, immediate_context);
}
#endif


EXPORT void SyncCLI(clisync::CliSyncData* data) {
    if (!CanStart(inject::DLLTYPE_EXTENSION)) return;
    SyncCLIOnce(data);
}

ADD_INJECTED_SYSTEM(detours, "detours", inject::PRIORITY_DETOURS,
    []() { PathDetours(inject::PRE_INIT); },
    []() { PathDetours(inject::POST_INIT); }
);
