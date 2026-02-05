#pragma once
#include <hook/library.hpp>
#include <hook/scan_container.hpp>

namespace hook::module_mapper {
	class Module {
		bool freeOnExit{};
		hook::library::Library lib{ (HMODULE)0 };
		hook::scan_container::ScanContainer scanContainer{ true };
		hook::library::ScanLogger logger{};
	public:
		Module(bool freeOnExit = false);
		Module(const Module& other) = delete;
		Module(Module&& other) = delete;
		~Module();

		bool Load(const std::filesystem::path& path, bool patchIAT = true, bool absolute = true);
		void Free();

		constexpr operator bool() {
			return lib;
		}

		constexpr const hook::library::Library* operator->() const {
			return &lib;
		}

		constexpr const hook::library::Library& operator*() const {
			return lib;
		}

		constexpr const hook::library::Library& GetLibrary() const {
			return lib;
		}

		hook::scan_container::ScanContainer& GetScanContainer();
		hook::library::ScanLogger& GetScanLogger();
	};
}