#pragma once
#include <hook/library.hpp>

namespace deps::ntdll {

    class NtDll {
        hook::library::Library ntdll;
        NtDll() : ntdll("ntdll.dll", true) {
            if (!ntdll) throw std::runtime_error("Can't load ntdll.dll");
            NtSuspendProcess = GetProc<decltype(NtSuspendProcess)>("NtSuspendProcess");
            NtResumeProcess = GetProc<decltype(NtResumeProcess)>("NtResumeProcess");
            NtCreateThreadEx = GetProc<decltype(NtCreateThreadEx)>("NtCreateThreadEx");
        }
    public:

        template<typename ProcType>
        ProcType GetProc(const char* name) {
            ProcType proc{ ntdll.GetProc<ProcType>(name) };
            if (!proc) throw std::runtime_error(std::format("Can't load {}::{}", ntdll, name));
            LOG_TRACE("loaded {}::{}", ntdll, name);
            return proc;
        }

        LONG(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
        LONG(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);
        NTSTATUS(NTAPI* NtCreateThreadEx)(
            _Out_ PHANDLE ThreadHandle,
            _In_ ACCESS_MASK DesiredAccess,
            _In_opt_ LPVOID  ObjectAttributes,
            _In_ HANDLE ProcessHandle,
            _In_ LPVOID StartRoutine,
            _In_opt_ uintptr_t Argument,
            _In_ ULONG CreateFlags,
            _In_ SIZE_T ZeroBits,
            _In_ SIZE_T StackSize,
            _In_ SIZE_T MaximumStackSize,
            _In_opt_ LPVOID AttributeList
            );

        /*
         * Get the ndll instance
         */
        static NtDll& GetInstance();
    };

    inline LONG NtSuspendProcess(
        IN HANDLE ProcessHandle
    ) {
        return NtDll::GetInstance().NtSuspendProcess(ProcessHandle);
    }

    inline LONG NtResumeProcess(
        IN HANDLE ProcessHandle
    ) {
        return NtDll::GetInstance().NtResumeProcess(ProcessHandle);
    }

    inline NTSTATUS NtCreateThreadEx(
        _Out_ PHANDLE ThreadHandle,
        _In_ ACCESS_MASK DesiredAccess,
        _In_opt_ LPVOID  ObjectAttributes,
        _In_ HANDLE ProcessHandle,
        _In_ LPVOID StartRoutine,
        _In_opt_ uintptr_t Argument,
        _In_ ULONG CreateFlags,
        _In_ SIZE_T ZeroBits,
        _In_ SIZE_T StackSize,
        _In_ SIZE_T MaximumStackSize,
        _In_opt_ LPVOID AttributeList
    ) {
        return NtDll::GetInstance().NtCreateThreadEx(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, StartRoutine, Argument, CreateFlags, ZeroBits, StackSize, MaximumStackSize, AttributeList);
    }
}