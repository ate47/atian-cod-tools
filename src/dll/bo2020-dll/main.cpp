#include <dll_includes.hpp>
#include <core/actsinfo.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <hook/error.hpp>


namespace {
    HANDLE mainThread{};
    DWORD mainThreadId{};
    hook::library::Detour GetSystemMetricsDetour;

    int GetSystemMetricsStub(int nIndex) {
        // unpack?

        static std::once_flag of{};

        if (mainThreadId == GetCurrentThreadId()) {
            std::call_once(of, [] {
                try {
                    core::system::PostInit();
                    hook::library::SaveScanContainer();
                }
                catch (std::exception& e) {
                    LOG_ERROR("Error at ACTS DLL post init {}", e.what());
                    MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL post init", MB_ICONERROR);
                    *reinterpret_cast<byte*>(0x123456789) = 2;
                }
            });
        }
        return GetSystemMetricsDetour.Call<int>(nIndex);
    }

    void InitDll() {
        try {
            // default config for now
            core::logs::setfile("acts-cod2020.log");
            core::config::SyncConfig(true);

            core::config::ConfigEnumData logNames[]{
                { "trace", core::logs::LVL_TRACE_PATH},
                { "debug", core::logs::LVL_DEBUG},
                { "info", core::logs::LVL_INFO},
                { "warning", core::logs::LVL_WARNING},
                { "error", core::logs::LVL_ERROR}
            };

            core::logs::setlevel(core::config::GetEnumVal<core::logs::loglevel>("logger.level", logNames, ARRAYSIZE(logNames), core::logs::LVL_INFO));

            hook::library::Library main{};
            std::string_view libname{ main.GetName() };
            if (libname.rfind("CrashHandler") != std::string::npos) {
                LOG_DEBUG("ignore loading crash handler {}", libname);
                return; // ignore crash handler
            }
            LOG_INFO("init acts dll pid={} name={}", GetCurrentProcessId(), main.GetName());

            mainThread = GetCurrentThread();
            mainThreadId = GetCurrentThreadId();
            hook::error::EnableHeavyDump();
            hook::error::InstallErrorHooks(true);

            // clear error
            std::filesystem::path exepath{ main.GetPath() };
            std::filesystem::remove(exepath.replace_extension(".start"));


            hook::library::Library user32 = "user32.dll";
            if (!user32) {
                throw std::runtime_error("Can't find user32.dll");
            }

            GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);

            core::system::Init();
        }
        catch (std::exception& e) {
            LOG_ERROR("Error at ACTS DLL startup {}", e.what());
            MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL startup", MB_ICONERROR);
            *reinterpret_cast<byte*>(0x123456789) = 2;
        }
    }

}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        InitDll();
    }
    return TRUE; // Nothing by default
}

// hook powrprof.dll for auto injection
EXPORT NTSTATUS CallNtPowerInformation(POWER_INFORMATION_LEVEL InformationLevel, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength) {
    static auto func = [] {
        hook::library::Library powrprof{ "powrprof.dll", true };

        if (!powrprof) throw std::runtime_error(utils::va("can't find system powrprof.dll"));

        return reinterpret_cast<decltype(&CallNtPowerInformation)>(powrprof["CallNtPowerInformation"]);
        }();

        return func(InformationLevel, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
}
