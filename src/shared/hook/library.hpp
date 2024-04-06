#pragma once
#include <detours.h>

namespace hook::library {
	/*
	 * @return module from an address
	 */
	HMODULE GetLibraryInfo(const void* address);
	/*
	 * Get the name of a library
	 * @param hmod module
	 * @return name without the path
	 */
	const char* GetLibraryName(HMODULE hmod);

	/*
	 * Get the path of a library
	 * @param hmod module
	 * @return path
	 */
	const char* GetLibraryPath(HMODULE hmod);

	/*
	 * Locate the PDB of a module
	 * @return pdb path or null
	 */
	const char* LocatePDB(HMODULE hmod);

	/*
	 * Detour structure
	 */
	class Detour {
		void* origin{};
		void* base{};
	public:
		inline Detour() {}

		/*
		 * Create a detour from a base to a location
		 * @param base origin
		 * @param to destination
		 */
		void Create(void* base, void* to) {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			this->base = origin = base;

			DetourAttach(&(PVOID&)this->base, to);

			LONG error = DetourTransactionCommit();

			if (error != NO_ERROR) {
				throw std::runtime_error("Can't commit detour");
			}
		}

		/*
		 * @return origin pointer
		 */
		constexpr void* GetOrigin() const {
			return origin;
		}

		/*
		 * @return base pointer
		 */
		template<typename Type>
		constexpr Type* GetBase() const {
			return reinterpret_cast<Type*>(base);
		}

		/*
		 * call the base pointer
		 */
		template<typename Out = void, typename... Args>
		inline Out Call(Args ... args) const {
			return reinterpret_cast<Out(*)(Args ...)>(base)(args...);
		}

	};
}