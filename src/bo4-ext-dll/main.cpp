#include <dll_includes.hpp>
#include <utils.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <core/system.hpp>
#include <core/config.hpp>

namespace bo4 {
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

        void InitDll() {
            try {
                core::config::SyncConfig();
                static std::string logFile{ "acts-bo4.log" };
                logFile = core::config::GetString("logger.output", logFile.c_str());

                core::config::ConfigEnumData logNames[]{
                    { "trace", alogs::LVL_TRACE},
                    { "debug", alogs::LVL_DEBUG},
                    { "info", alogs::LVL_INFO},
                    { "warning", alogs::LVL_WARNING},
                    { "error", alogs::LVL_ERROR}
                };

                alogs::setfile(logFile.data());
                alogs::setlevel(core::config::GetEnumVal<alogs::loglevel>("logger.level", logNames, ARRAYSIZE(logNames), alogs::LVL_INFO));

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

                if (!user32) {
                    throw std::runtime_error("Can't find user32.dll");
                }
                GetSystemMetricsDetour.Create(user32["GetSystemMetrics"], GetSystemMetricsStub);

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