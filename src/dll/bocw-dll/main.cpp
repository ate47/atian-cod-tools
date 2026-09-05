#include <dll_includes.hpp>
#include <utils/utils.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <hook/process.hpp>
#include <core/system.hpp>
#include <core/actsinfo.hpp>
#include <data/cw_generated.hpp>

namespace bocw {
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
                        {
                            hook::scan_container::ScanContainer scan{ {}, true };
                            LoadScans(scan);
                        }
                        core::system::PostInit();
                    } catch (std::exception& e) {
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
                core::logs::setfile("acts-bocw.log");
                core::logs::setlevel(core::logs::LVL_TRACE);
                hook::library::Library main{};
                std::string_view libname{ main.GetName() };
                if (libname.rfind("CrashHandler") != std::string::npos) {
                    LOG_DEBUG("ignore loading crash handler {}", libname);
                    return; // ignore crash handler
                }
                LOG_INFO("init acts dll pid={} name={}", GetCurrentProcessId(), libname);

                mainThread = GetCurrentThread();
                mainThreadId = GetCurrentThreadId();
                hook::error::EnableHeavyDump();
                hook::error::InstallErrorHooks(true);

                // clear error
                std::filesystem::path exepath{ main.GetPath() };
                std::filesystem::remove(exepath.replace_extension(".start"));
                // patch tls

                auto& tlsDir = platform::PImageOptHeader(*main)->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

                if (tlsDir.VirtualAddress && tlsDir.Size) {
                    IMAGE_TLS_DIRECTORY& tls = *reinterpret_cast<IMAGE_TLS_DIRECTORY*>(main[tlsDir.VirtualAddress]);
                    void** c = reinterpret_cast<void**>(main[tls.AddressOfCallBacks]);

                    while (c && *c) {
                        hook::memory::Nulled(*c);
                        c++;
                    }
                }

                // kernel hooks
                hook::library::Library kernel32 = "kernel32.dll";
                hook::library::Library dxgi = "dxgi.dll";
                hook::library::Library user32 = "user32.dll";

                if (!kernel32) {
                    throw std::runtime_error("Can't find kernel32.dll");
                }
                if (!dxgi) {
                    throw std::runtime_error("Can't find dxgi.dll");
                }
                if (!user32) {
                    throw std::runtime_error("Can't find user32.dll");
                }

                GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);

                core::system::Init();
            } catch (std::exception& e) {
                LOG_ERROR("Error at ACTS DLL startup {}", e.what());
                MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL startup", MB_ICONERROR);
                *reinterpret_cast<byte*>(0x123456789) = 2;
            }
        }
    } // namespace
} // namespace bocw

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        bocw::InitDll();
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
