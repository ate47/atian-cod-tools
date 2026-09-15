#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <winternl.h>

namespace {

    static const wchar_t* badWords[] = {
        L"IDA",    L"Pro 7.7.220118", L"ida",       L"HxD",   L"cheatengine", L"Cheat Engine", L"x96dbg",
        L"x32dbg", L"x64dbg",         L"Wireshark", L"Debug", L"DEBUG",       L"msvsmon",
    };

    void EraseBadWords(wchar_t* str) {
        if (!str || !*str) {
            return;
        }
        std::wstring_view strw{ str };

        for (const wchar_t* keyword : badWords) {
            size_t pos;
            while ((pos = strw.find(keyword)) != std::string::npos) {

                for (const wchar_t* i = keyword; *i; i++) {
                    str[pos++] = L'a';
                }
            }
        }
    }

    hook::library::Detour GetWindowTextW_Detour;
    hook::library::Detour NtQuerySystemInformation_Detour;
    hook::library::Detour NtQueryInformationProcess_Detour;
    hook::library::Detour GetThreadContext_Detour;

    int WINAPI GetWindowTextW_Stub(HWND hWnd, LPWSTR lpString, int nMaxCount) {
        int r{ GetWindowTextW_Detour.Call<int>(hWnd, lpString, nMaxCount) };
        if (r) {
            EraseBadWords(lpString);
        }
        return r;
    }

    int WINAPI GetWindowTextA_Stub(HWND hWnd, LPSTR lpString, int nMaxCount) {
        // GetWindowTextW()
        std::wstring wbuff{};
        wbuff.resize(nMaxCount);

        int r{ GetWindowTextW(hWnd, wbuff.data(), nMaxCount) };

        if (r) {
            std::string buff{ utils::WStrToStr(wbuff) };
            std::memset(lpString, 0, nMaxCount);
            std::memcpy(lpString, buff.data(), buff.size());
        }

        return r;
    }

    NTSTATUS NTAPI NtQuerySystemInformation_Stub(
        SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength,
        PULONG ReturnLength
    ) {
        NTSTATUS status{
            NtQuerySystemInformation_Detour
                .Call<NTSTATUS>(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength)
        };

        if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessInformation) {
            PSYSTEM_PROCESS_INFORMATION spi{ (PSYSTEM_PROCESS_INFORMATION)SystemInformation };

            while (spi) {
                if (spi->ImageName.Length) {
                    EraseBadWords(spi->ImageName.Buffer);
                }

                spi = spi->NextEntryOffset ? (PSYSTEM_PROCESS_INFORMATION)((byte*)spi + spi->NextEntryOffset) : nullptr;
            }
        }

        return status;
    }

    NTSTATUS NTAPI NtQueryInformationProcess_Stub(
        HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
        ULONG ProcessInformationLength, PULONG ReturnLength
    ) {
        NTSTATUS status{ NtQueryInformationProcess_Detour.Call<NTSTATUS>(
            ProcessHandle,
            ProcessInformationClass,
            ProcessInformation,
            ProcessInformationLength,
            ReturnLength
        ) };

        constexpr DWORD ProcessDebugPort = 7;
        constexpr DWORD ProcessDebugObjectHandle = 30;
        constexpr DWORD ProcessDebugFlags = 31;
        constexpr DWORD ProcessImageFileNameWin32 = 43;

        if (NT_SUCCESS(status) &&
            (ProcessInformationClass == ProcessImageFileName || ProcessInformationClass == ProcessImageFileNameWin32)) {
            UNICODE_STRING& str{ *(UNICODE_STRING*)ProcessInformation };

            if (str.Length) {
                EraseBadWords(str.Buffer);
            }
        }

        return status;
    }

    BOOL WINAPI GetThreadContext_Stub(HANDLE hThread, LPCONTEXT lpContext) {
        if (!GetThreadContext_Detour.Call<BOOL>(hThread, lpContext)) {
            return false;
        }
        if (lpContext) {
            lpContext->Dr0 = 0;
            lpContext->Dr1 = 0;
            lpContext->Dr2 = 0;
            lpContext->Dr3 = 0;
        }
        return true;
    }

    void PatchArxan(uint64_t uid) {
        // todo
    }

    void PatchArxanPre(uint64_t uid) {
        // patch dev stuff
        hook::library::Library ntdll{ GetModuleHandleA("ntdll.dll") };
        hook::library::Library kernel32{ GetModuleHandleA("kernel32.dll") };

        hook::memory::RedirectJmp(GetWindowTextA, GetWindowTextA_Stub, true);
        GetWindowTextW_Detour.Create(GetWindowTextW, GetWindowTextW_Stub);

        NtQuerySystemInformation_Detour.Create(ntdll["NtQuerySystemInformation"], NtQuerySystemInformation_Stub);
        NtQueryInformationProcess_Detour.Create(ntdll["NtQueryInformationProcess"], NtQueryInformationProcess_Stub);
        GetThreadContext_Detour.Create(kernel32["GetThreadContext"], GetThreadContext_Stub);
        LOG_TRACE("patched pre-arxan");
    }
} // namespace

REGISTER_SYSTEM(arxan, PatchArxanPre, PatchArxan);