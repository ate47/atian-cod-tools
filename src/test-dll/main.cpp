#include <Windows.h>
#include <iostream>

#define EXPORT extern "C" __declspec(dllexport)

// test DLL for memapi tests

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        std::cout << "DLL Injected\n";
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
