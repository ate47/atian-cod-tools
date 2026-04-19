#pragma once
#include <includes_shared.hpp>

namespace hook::process {
#ifdef ASMJIT_STATIC
    asmjit::JitRuntime& GetJitRuntime();
#endif
    byte* BasePtr();
    void* BaseHandle();

    void WriteMemSafe(void* dest, void* src, size_t len);

    template<typename Type>
    void WriteMemSafe(void* dest, Type src) {
        WriteMemSafe(dest, &src, sizeof(src));
    }

    template<typename Out = byte>
    inline Out* Relativise(uintptr_t location) {
        return reinterpret_cast<Out*>(&BasePtr()[location]);
    }

    void* LoadLib(const char* lib, int32_t flags = 0);
    inline void* LoadLib(std::string lib, int32_t flags = 0) {
        return LoadLib(lib.data(), flags);
    }

    void* LoadSysLib(const char* lib);
    inline void* LoadSysLib(std::string lib) {
        return LoadSysLib(lib.data());
    }

    void** GetImportAddrTableEntry(const char* lib, const char* entry);
    /*
     * Pattern scan
     * @param pattern Pattern, ? for wildcard, [0-9A-F] for byte
     * @return matches
     */
    std::vector<byte*> Scan(std::string pattern);

#ifdef ASMJIT_STATIC
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

#endif
}