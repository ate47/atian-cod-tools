#include <dll_includes.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <data/bo3.hpp>

namespace acts {
    namespace {
        hook::library::Detour GetSystemMetrics_Detour;
        hook::scan_container::ScanContainer scan{ false };

        void PostUnpack() {
            LOG_INFO("post init bo3 dll");

            try {
                scan.Load({});
                core::system::PostInit();
                scan.Save();
                LOG_INFO("ACTS Loaded");
            }
            catch (std::exception& e) {
                LOG_ERROR("Error at acts DLL post unpack: {}", e.what());
                MessageBoxA(NULL, utils::CloneString(e.what()), "Error at acts DLL post unpack", MB_ICONERROR);
                *reinterpret_cast<byte*>(0x123456789) = 2;
            }
        }


        int GetSystemMetrics_Stub(int nIndex) {
            static std::once_flag of{};
            std::call_once(of, PostUnpack);
            return GetSystemMetrics_Detour.Call<int>(nIndex);
        }

        bool IsGameLoaded() {
            hook::library::Library main{};
            std::vector<hook::library::ScanResult> r{ main.Scan("48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 15 80 3D") };
            if (r.size() != 1) {
                if (!r.empty()) {
                    LOG_WARNING("TOO MANY FIND FOR ui_level_sv");
                }
                return false;
            }
        
            bo3::dvar_t** ui_level_sv{ r[0].GetRelative<int32_t, bo3::dvar_t**>(3) };
            return *ui_level_sv != nullptr;
        }

	    void Main() {
            try {
                core::logs::setfile("acts-bo3.log");
                hook::library::Library main{};
                LOG_INFO("init bo3 dll pid={} name={}", GetCurrentProcessId(), main.GetName());
                core::config::SyncConfig(false);

                core::config::ConfigEnumData logNames[]{
                    { "trace", core::logs::LVL_TRACE_PATH },
                    { "debug", core::logs::LVL_DEBUG },
                    { "info", core::logs::LVL_INFO },
                    { "warning", core::logs::LVL_WARNING },
                    { "error", core::logs::LVL_ERROR} };

                core::logs::setlevel(core::config::GetEnumVal<core::logs::loglevel>("logger.level", logNames, ARRAYSIZE(logNames), core::logs::LVL_INFO));

                core::config::SaveConfig();

                hook::error::EnableHeavyDump();
                hook::error::InstallErrorHooks(true);

                if (IsGameLoaded()) {
                    LOG_ERROR("Game already loaded, can't patch");
                    return;
                }

                hook::library::Library User32{ GetModuleHandleA("User32.dll") };
                GetSystemMetrics_Detour.Create(User32["GetSystemMetrics"], GetSystemMetrics_Stub);

                std::filesystem::path start{ main.GetName() };
                start.replace_extension(".start");
                if (std::filesystem::exists(start)) {
                    LOG_DEBUG("remove {}", start.string());
                    std::filesystem::remove(start);
                }


                core::system::Init();
            }
            catch (std::exception& e) {
                LOG_ERROR("Error at acts DLL startup: {}", e.what());
                MessageBoxA(NULL, utils::CloneString(e.what()), "Error at acts DLL startup", MB_ICONERROR);
                *reinterpret_cast<byte*>(0x123456789) = 2;
            }
        }
    }


    hook::scan_container::ScanContainer& Scan() {
        return scan;
    }
}
BOOL WINAPI DllMain(HMODULE hModule, DWORD Reason, LPVOID lpVoid) {
	if (Reason == DLL_PROCESS_ATTACH) {
		acts::Main();
	}
	return TRUE;
}