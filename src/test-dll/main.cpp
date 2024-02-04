#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <psapi.h> // For access to GetModuleFileNameEx
#include <detours.h>
#include "includes_shared.hpp"

#define EXPORT extern "C" __declspec(dllexport)



// test DLL for memapi tests
namespace {
    BOOL(WINAPI* ReadProcessMemoryReal)(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    BOOL WINAPI ReadProcessMemoryHook(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead) {
        std::wofstream of{ "detours.txt", std::ios::app };

        of << "Reading " << std::hex << lpBaseAddress << "," << lpBuffer << "," << nSize << " from " << hProcess << "\n";

        of.close();

        return ReadProcessMemoryReal(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
    }

    void DetectInjections() {
        std::ofstream of{ "detours.txt", std::ios::app };

        of << "Injecting detours...\n";
        auto* hook = &SetWindowsHookExW;


        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        ReadProcessMemoryReal = ReadProcessMemory;

        DetourAttach((LPVOID*)&ReadProcessMemoryReal, (LPVOID)ReadProcessMemoryHook);

        auto det = DetourTransactionCommit();

        if (det != NO_ERROR) {
            MessageBoxA(0, "Error applying detours", "Error", MB_OK);
        }
        else {
            of << "Detours injected.\n";
        }

        Process bo4{ "BlackOps4.exe" };

        of << "bo4: " << bo4 << ":" << std::hex << bo4[(UINT64)0] << "\n";


        of.close();
    }
}


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DetectInjections();
        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

EXPORT void ProcessInjectionTest1() {
    std::cout << "Injection test 1" << "\n";
}

EXPORT void ProcessInjectionTest2(int value1, double value2, float value3, LPCWCH value4, bool value5) {
    std::wcout << L"Injection test 2:" << "\n"
        << value1 << ", " << value2 << ", " << value3 << ", " << value4 << ", " << value5 << "\n";
}

static BYTE TestFunctionScanData[2][10] = {
    { 0x01, 0x02, 0x03, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0x9, 0x10 },
    { 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x9, 0x10 }
};

EXPORT void TestFunctionScan() {
    std::cout << "Data: " << &TestFunctionScanData << "\n";

    for (size_t i = 0; i < sizeof(TestFunctionScanData[0]); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)TestFunctionScanData[0][i] << " ";
    }
    std::cout << "\n";
    for (size_t i = 0; i < sizeof(TestFunctionScanData[1]); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)TestFunctionScanData[1][i] << " ";
    }
    std::cout << "\n";

}