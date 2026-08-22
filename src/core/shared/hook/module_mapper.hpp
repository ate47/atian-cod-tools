#pragma once
#include <hook/library.hpp>
#include <hook/scan_container.hpp>

namespace hook::module_mapper {
    class Module {
        bool freeOnExit{};
        hook::library::Library lib{ (void*)0 };
        uintptr_t originBase{};
        hook::scan_container::ScanContainer scanContainer{ true };
        hook::library::ScanLogger logger{};

      public:
        Module(bool freeOnExit = false);
        Module(const Module& other) = delete;
        Module(Module&& other) = delete;
        ~Module();

        bool Load(const std::filesystem::path& path, bool patchIAT = true, bool absolute = true);
        void Free();

        constexpr operator bool() { return lib; }

        constexpr const hook::library::Library* operator->() const { return &lib; }

        constexpr const hook::library::Library& operator*() const { return lib; }

        constexpr const hook::library::Library& GetLibrary() const { return lib; }

        hook::scan_container::ScanContainer& GetScanContainer();
        hook::library::ScanLogger& GetScanLogger();

        void* RebasePtr(uintptr_t origin) const;

        template<typename T = void>
        T* Rebase(uintptr_t origin) const {
            return (T*)RebasePtr(origin);
        }

        template<typename T = void>
        T* Rebase(T* origin) const {
            return Rebase<T>(reinterpret_cast<uintptr_t>(origin));
        }

        friend std::ostream& operator<<(std::ostream& out, const Module& ptr);
    };
} // namespace hook::module_mapper
template<>
struct std::formatter<hook::module_mapper::Module, char> : utils::BasicFormatter<hook::module_mapper::Module> {};