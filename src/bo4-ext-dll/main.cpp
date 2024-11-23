#include <dll_includes.hpp>
#include <utils/utils.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <core/config.hpp>

namespace bo4 {
    namespace {
        HANDLE mainThread{};
        DWORD mainThreadId{};
        hook::library::Detour GetSystemMetricsDetour;
        hook::library::Detour GetThreadContextDetour;

        int GetSystemMetricsStub(int nIndex) {
            // unpack?

            static std::once_flag of{};

            if (mainThreadId == GetCurrentThreadId()) {
                std::call_once(of, [] {
                    try {
                        LOG_DEBUG("Postinit systems");
                        core::system::PostInit();
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

        BOOL WINAPI GetThreadContextStub(HANDLE hThread, LPCONTEXT lpContext) {
            // clear debug flags if inside the game
            if (hook::library::GetLibraryInfo(_ReturnAddress()) == hook::process::BaseHandle()) {
                if (lpContext && lpContext->ContextFlags & (CONTEXT_DEBUG_REGISTERS & ~CONTEXT_AMD64)) {
                    lpContext->ContextFlags &= ~(CONTEXT_DEBUG_REGISTERS & ~CONTEXT_AMD64);
                }
                if (hThread && hThread != INVALID_HANDLE_VALUE) {
                    core::eventhandler::RunEvent(hash::Hash64("RegisterThread"), (void*)hThread);
                }
            }

            return GetThreadContextDetour.Call<BOOL>(hThread, lpContext);
        }

        void InitDll() {
            try {
                core::logs::setfile("acts-bo4.log");
                core::config::SyncConfig(true);

                core::config::ConfigEnumData logNames[]{
                    { "trace", core::logs::LVL_TRACE},
                    { "debug", core::logs::LVL_DEBUG},
                    { "info", core::logs::LVL_INFO},
                    { "warning", core::logs::LVL_WARNING},
                    { "error", core::logs::LVL_ERROR}
                };

                core::logs::setlevel(core::config::GetEnumVal<core::logs::loglevel>("logger.level", logNames, ARRAYSIZE(logNames), core::logs::LVL_INFO));

                hook::library::Library main{};
                LOG_INFO("init acts dll pid={} name={}", GetCurrentProcessId(), main.GetName());

                mainThread = GetCurrentThread();
                mainThreadId = GetCurrentThreadId();
                if (core::config::GetBool("logger.heavyDump", true)) {
                    hook::error::EnableHeavyDump();
                    hook::error::InstallErrorHooks(true);
                }

                // clear error
                std::filesystem::path exepath{ main.GetPath()};
                std::filesystem::remove(exepath.replace_extension(".start"));

                hook::library::Library user32 = "user32.dll";
                hook::library::Library kernel32 = "kernel32.dll";

                if (!user32) {
                    throw std::runtime_error("Can't find user32.dll");
                }
                if (!kernel32) {
                    throw std::runtime_error("Can't find kernel32.dll");
                }
                GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);
                GetThreadContextDetour.Create(kernel32["GetThreadContext"], GetThreadContextStub);

                LOG_DEBUG("Init systems");
                core::system::Init();
                core::config::SaveConfig();
            }
            catch (std::exception& e) {
                LOG_ERROR("Error at ACTS DLL startup {}", e.what());
                MessageBoxA(NULL, utils::va("%s", e.what()), "Error at ACTS DLL startup", MB_ICONERROR);
                *reinterpret_cast<byte*>(0x123456789) = 2;
            }
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        bo4::InitDll();
    }
    return TRUE;
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
