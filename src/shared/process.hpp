#pragma once
#include <includes_shared.hpp>

namespace process {
    asmjit::JitRuntime& GetJitRuntime();
    byte* BasePtr();
    HMODULE BaseHandle();
    PIMAGE_OPTIONAL_HEADER PImageOptHeader(HMODULE mod = 0);
    PIMAGE_NT_HEADERS PImageNtHeader(HMODULE mod = 0);
    PIMAGE_DOS_HEADER PImageDosHeader(HMODULE mod = 0);

    void WriteMemSafe(void* dest, void* src, size_t len);

    template<typename Type>
    void WriteMemSafe(void* dest, Type src) {
        WriteMemSafe(dest, &src, sizeof(src));
    }

    template<typename Out = byte>
    inline Out* Relativise(uintptr_t location) {
        return reinterpret_cast<Out*>(&BasePtr()[location]);
    }

    HMODULE LoadLib(const char* lib);
    inline HMODULE LoadLib(std::string lib) {
        return LoadLib(lib.data());
    }

    HMODULE LoadSysLib(const char* lib);
    inline HMODULE LoadSysLib(std::string lib) {
        return LoadSysLib(lib.data());
    }

    void** GetImportAddrTableEntry(const char* lib, const char* entry);
    /*
     * Pattern scan
     * @param pattern Pattern, ? for wildcard, [0-9A-F] for byte
     * @return matches
     */
    std::vector<byte*> Scan(std::string pattern);

    class AssemblerExp : public asmjit::x86::Assembler {
    public:
        using Assembler::Assembler;
        using Assembler::call;
        using Assembler::jmp;

        void pushregisters();
        void popregisters();

        void prepareStackForCall();
        void restoreStackAfterCall();

        template <typename T>
        void callAligned(T&& target) {
            this->prepareStackForCall();
            this->call(std::forward<T>(target));
            this->restoreStackAfterCall();
        }

        asmjit::Error call(void* target);
        asmjit::Error jmp(void* target);
    };

}