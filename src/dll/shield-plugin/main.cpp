#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>
#include <core/config_shared.hpp>
#include <core/system.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <data/bo4_generated.hpp>

namespace {
    struct ModConfig {
        bool callsys{};
    } cfg;

    HANDLE mainThread{};
    DWORD mainThreadId{};
    hook::library::Detour GetSystemMetricsDetour;

    int GetSystemMetricsStub(int nIndex);

    void PostUnpack() {
        LOG_INFO("post unpack acts");
        try {
            {
                hook::scan_container::ScanContainer scan{ {}, true };
                bo4::LoadScans(scan);
            }
            if (!cfg.callsys) {
                core::system::PostInit();
            }
        } catch (std::exception& e) {
            LOG_ERROR("Error at ACTS DLL post init {}", e.what());
            MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL post init", MB_ICONERROR);
            *reinterpret_cast<byte*>(0x123456789) = 2;
        }
    }

    void InitActs() {
        LOG_INFO("init acts");
        // default config for now
        core::logs::setfile("acts-bo4.log");
        core::config::SetMainConfig(std::format("acts/{}", core::config::MAIN_CONFIG_FILE));
        core::config::SyncConfig(true);
        core::config::shared::LoadConfig();
        cfg.callsys = core::config::Get("acts.callsys", cfg.callsys);

        hook::error::EnableHeavyDump();
        hook::error::InstallErrorHooks(true);

        hook::library::Library main{};
        LOG_INFO(
            "init acts {}(0x{:x}) dll pid={} name={}",
            core::actsinfo::VERSION,
            core::actsinfo::BUILD_VERSION_ID,
            GetCurrentProcessId(),
            main.GetName()
        );
    }

    int InitDll() {
        hook::library::Library main{};

        // clear error
        std::filesystem::path exepath{ main.GetPath() };
        std::filesystem::remove(exepath.replace_extension(".start"));

        mainThread = GetCurrentThread();
        mainThreadId = GetCurrentThreadId();

        hook::library::Library user32{ "user32.dll" };

        if (!user32) {
            throw std::runtime_error("Can't find user32.dll");
        }

        GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);

        try {
            if (!cfg.callsys) {
                core::system::Init();
            }
        } catch (std::exception& e) {
            LOG_ERROR("Error at ACTS DLL init {}", e.what());
            MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL init", MB_ICONERROR);
            return false;
        }
        return true;
    }

    class PatchedStart {
        void* ref{};
        byte data[14]{}; // r64 jump

      public:
        PatchedStart() = default;

        PatchedStart(void* ref, void* to) : ref(ref) {
            LOG_TRACE("patch {}->{}", hook::library::CodePointer{ ref }, hook::library::CodePointer{ to });
            std::memcpy(data, ref, sizeof(data));
            if (to) {
                hook::memory::RedirectJmp(ref, to, true);
            } else {
                hook::memory::Nulled(ref);
            }
        }

        PatchedStart(PatchedStart&& other) noexcept : ref(other.ref) {
            std::memcpy(data, other.data, sizeof(data));
            other.ref = nullptr;
        }

        PatchedStart(const PatchedStart&) = delete;
        PatchedStart& operator=(const PatchedStart&) = delete;

        ~PatchedStart() {
            if (ref) {
                hook::process::WriteMemSafe(ref, data, sizeof(data));
            }
        }
    };

    std::vector<PatchedStart> starts{};

    int NewStart() {
        if (!InitDll()) {
            return -1;
        }

        starts.clear(); // cleanup the existing ones
        LOG_TRACE("start game...");
        return ((int (*)())platform::GetAddressOfEntryPoint())();
    }

    void PatchStart() {
        InitActs();
        starts.emplace_back(platform::GetAddressOfEntryPoint(), NewStart);
        for (void* tls : platform::GetTLSAddresses()) {
            starts.emplace_back(tls, nullptr);
        }
    }

    int GetSystemMetricsStub(int nIndex) {
        static std::once_flag of{};

        if (mainThreadId == GetCurrentThreadId()) {
            std::call_once(of, [] { PostUnpack(); });
        }
        return GetSystemMetricsDetour.Call<int>(nIndex);
    }
} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        PatchStart();
    }
    return TRUE;
}

// hook powrprof.dll for auto injection
EXPORT NTSTATUS CallNtPowerInformation(
    POWER_INFORMATION_LEVEL InformationLevel, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer,
    ULONG OutputBufferLength
) {
    static auto func = [] {
        hook::library::Library powrprof{ "powrprof.dll", true };

        if (!powrprof)
            throw std::runtime_error(utils::va("can't find system powrprof.dll"));

        return reinterpret_cast<decltype(&CallNtPowerInformation)>(powrprof["CallNtPowerInformation"]);
    }();

    return func(InformationLevel, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
}

// XInput auto inject
EXPORT DWORD XInputGetState(DWORD dwUserIndex, void* pState) {
    static auto func = [] {
        hook::library::Library xinput9_1_0{ "xinput9_1_0.dll", true };

        if (!xinput9_1_0)
            throw std::runtime_error(utils::va("can't find system xinput9_1_0.dll"));

        return reinterpret_cast<decltype(&XInputGetState)>(xinput9_1_0["XInputGetState"]);
    }();

    return func(dwUserIndex, pState);
}
EXPORT DWORD __stdcall XInputSetState(DWORD dwUserIndex, void* pVibration) {
    static auto func = [] {
        hook::library::Library xinput9_1_0{ "xinput9_1_0.dll", true };

        if (!xinput9_1_0)
            throw std::runtime_error(utils::va("can't find system xinput9_1_0.dll"));

        return reinterpret_cast<decltype(&XInputSetState)>(xinput9_1_0["XInputSetState"]);
    }();

    return func(dwUserIndex, pVibration);
}