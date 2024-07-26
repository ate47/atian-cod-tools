#include <dll_includes.hpp>
#include <winternl.h>
#include <core/system.hpp>
#include <core/config.hpp>
#include <hook/library.hpp>
#include <hook/memory.hpp>

namespace {
    hook::library::Detour CreateThreadDetour{};
    bool drmEnabled{};
    void* tlsOriginal{};

    void** GetTlsLoc() {
        hook::library::Library main{};

        auto& tlsDir = main.GetOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

        if (tlsDir.VirtualAddress && tlsDir.Size) {
            IMAGE_TLS_DIRECTORY& tls = *reinterpret_cast<IMAGE_TLS_DIRECTORY*>(main[tlsDir.VirtualAddress]);
            return reinterpret_cast<void**>(main[tls.AddressOfCallBacks]);
        }
        return nullptr;
    }

    HANDLE WINAPI CreateThreadStub(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
        if (hook::library::GetLibraryInfo(lpStartAddress) == process::BaseHandle()) {
            void** c = GetTlsLoc();
            if (c) {
                process::WriteMemSafe(c, tlsOriginal);
            }
            CreateThreadDetour.Clear();
            return CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
        }
        return CreateThreadDetour.Call<HANDLE>(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    }

    hook::library::Detour NtQueryInformationProcessDetour;
    hook::library::Detour NtQuerySystemInformationDetour;

    void CleanupStr(UNICODE_STRING& str) {
        if (!str.Buffer || !str.Length) {
            return;
        }
        const std::wstring_view buff{ str.Buffer, str.Length };

        static const std::wstring_view bad[]{
            L"IDA",
            L"ida",
            L"HxD"
        };

        for (const std::wstring_view& b : bad) {
            size_t pos;

            while ((pos = buff.rfind(b)) != std::string::npos) {
                for (size_t i = 0; i < b.length(); i++) {
                    str.Buffer[pos + i] = L't';
                }
            }
        }
    }

    NTSTATUS NTAPI NtQueryInformationProcessStub(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) {
        NTSTATUS ret = NtQueryInformationProcessDetour.Call<NTSTATUS>(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

        if (NT_SUCCESS(ret)) {
            if (ProcessInformationClass == ProcessImageFileName || ProcessInformationClass == 43) {
                CleanupStr(*(UNICODE_STRING*)ProcessInformation);
            }
        }

        return ret;
    }

    NTSTATUS NTAPI NtQuerySystemInformationStub(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength) {
        NTSTATUS ret = NtQuerySystemInformationDetour.Call<NTSTATUS>(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

        if (NT_SUCCESS(ret)) {
            if (SystemInformationClass == SystemProcessInformation) {
                SYSTEM_PROCESS_INFORMATION* nfo = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(SystemInformation);
                while (true) {
                    CleanupStr(nfo->ImageName);
                    if (!nfo->NextEntryOffset) {
                        break;
                    }

                    nfo = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>((byte*)nfo + nfo->NextEntryOffset);
                }
            }
        }

        return ret;
    }

	void Init(uint64_t id) {
        if (!(drmEnabled = core::config::GetBool("core.drm.enabled", true))) {
            return;
        }
        // patch tls
        void** c = GetTlsLoc();
        if (c) {
            tlsOriginal = *c;

            process::WriteMemSafe(c, nullptr);

            CreateThreadDetour.Create(CreateThread, CreateThreadStub);
        }
        else {
            LOG_WARNING("Can't find TLS location");
        }

        hook::library::Library ntdll{ "ntdll.dll" };

        if (ntdll) {
            NtQueryInformationProcessDetour.Create(ntdll["NtQueryInformationProcess"], NtQueryInformationProcessStub);
            NtQuerySystemInformationDetour.Create(ntdll["NtQuerySystemInformation"], NtQuerySystemInformationStub);
        }
	}

	void PostInit(uint64_t id) {
        if (!drmEnabled) {
            return;
        }

	}

    REGISTER_SYSTEM(drm, Init, PostInit);
}