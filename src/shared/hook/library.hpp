#pragma once
#include <detours.h>
#include <hook/process.hpp>
#include <utils/utils.hpp>

namespace hook::library {
	struct ScanResult;
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
	 * Init a scan container
	 * @param name container name
	 */
	void InitScanContainer(const char* name);

	/*
	 * Save the scan container
	 */
	void SaveScanContainer();

	// Scan result
	struct ScanResult {
		byte* location;

		/*
		 * Get an element from this location
		 * @param Type element type
		 * @param offset location offset
		 * @return element
		 */
		template<typename Type>
		constexpr Type Get(size_t offset = 0) const {
			return *reinterpret_cast<Type*>(location + offset);
		}

		/*
		 * Get an element relative using offset to this location
		 * @param Type offset read type
		 * @param TypeOut return type
		 * @param offset location offset
		 * @param from relative start offset
		 * @return pointer
		 */
		template<typename Type, typename TypeOut = byte*>
		constexpr TypeOut GetRelativeFrom(size_t offset, size_t from) const {
			return reinterpret_cast<TypeOut>(location + from + Get<Type>(offset));
		}

		/*
		 * Get an element relative using offset to this location
		 * @param Type offset read type
		 * @param TypeOut return type
		 * @param offset location offset
		 * @return pointer
		 */
		template<typename Type, typename TypeOut = byte*>
		constexpr TypeOut GetRelative(size_t offset) const {
			return GetRelativeFrom<Type, TypeOut>(offset, offset + sizeof(Type));
		}

		/*
		 * Get a pointer from this this location
		 * @param Type return type
		 * @param offset location offset
		 * @return pointer
		 */
		template<typename Type = void*>
		constexpr Type GetPtr(size_t offset = 0) const {
			return reinterpret_cast<Type>(location + offset);
		}
	};

	/*
	 * Query the scan container for an unique result, otherwise throw an exception
	 * @param name pattern name (for logs)
	 * @param pattern pattern
	 * @return result
	 */
	ScanResult QueryScanContainerSingle(const char* name, const char* pattern);

	/*
	 * Query the scan container for multiple results
	 * @param name pattern name (for logs)
	 * @param pattern pattern
	 * @param single stop after one pattern
	 * @return results
	 */
	std::vector<ScanResult> QueryScanContainer(const char* name, const char* pattern, bool single = false);

	/*
	 * Query the scan container for an unique result and convert it to a pointer
	 * @param Type pointer type
	 * @param name pattern name (for logs)
	 * @param pattern pattern
	 * @return pointer
	 */
	template<typename Type>
	inline Type QueryScanContainerSinglePtr(const char* name, const char* pattern) {
		return QueryScanContainerSingle(name, pattern).GetPtr<Type>();
	}

	/*
	 * Scan the memory to find a pattern
	 * @param hmod module
	 * @param pattern pattern
	 * @param single single search
	 * @return matches
	 */
	std::vector<ScanResult> ScanLibrary(HMODULE hmod, const char* pattern, bool single = false);

	/*
	 * Scan the memory to find a string
	 * @param hmod module
	 * @param pattern pattern
	 * @param single single search
	 * @return matches
	 */
	std::vector<ScanResult> ScanLibraryString(HMODULE hmod, const char* pattern, bool single = false);

	/*
	 * Scan the memory to find data
	 * @param hmod module
	 * @param pattern pattern
	 * @param single single search
	 * @return matches
	 */
	template<typename T>
	std::vector<ScanResult> ScanLibraryNumber(HMODULE hmod, T val, bool single = false) {
		char buff[sizeof(T) * 3 + 1]{};

		byte* ptr{ (byte*)&val };
		char* patStrPtr{ buff };

		for (size_t i = 0; i < sizeof(T); i++) {
			patStrPtr += std::snprintf(patStrPtr, 4, "%02x ", (int)ptr[i]);
		}

		return ScanLibrary(hmod, buff, single);
	}
	class LibraryModule;

	// Library information
	class Library {
		HMODULE hmod{};
	public:
		Library() : hmod(process::BaseHandle()) {
		}

		Library(HMODULE hmod) : hmod(hmod) {
		}

		Library(const char* name) : hmod(process::LoadLib(name)) {
		}

		Library(const char* name, bool system, DWORD flags = 0) : hmod(system ? process::LoadSysLib(name) : process::LoadLib(name, flags)) {
		}

		Library(const std::string& name, bool system = false, DWORD flags = 0) : hmod(system ? process::LoadSysLib(name) : process::LoadLib(name, flags)) {
		}

		Library(const std::filesystem::path& name, bool system = false, DWORD flags = 0) : hmod(system ? process::LoadSysLib(name.string()) : process::LoadLib(name.string(), flags)) {
		}

		Library(const Library& other) : hmod(other.hmod) {
		}

		void SetModule(HMODULE hmod) {
			this->hmod = hmod;
		}

		void ClearModule() {
			this->hmod = 0;
		}

		void SetModule(const char* name, bool system = false, DWORD flags = 0) {
			HMODULE loaded{ GetModuleHandleA(name) };
			
			if (loaded) {
				SetModule(loaded);
				return;
			}

			SetModule(system ? process::LoadSysLib(name) : process::LoadLib(name, flags));
		}

		void SetModule(const std::string& name, bool system = false, DWORD flags = 0) {
			SetModule(name.data(), system, flags);
		}


		void SetModule(const std::filesystem::path& name, DWORD flags = 0) {
			SetModule(name.string(), false, flags);
		}

		bool Free() {
			if (!*this) return false;
			return FreeLibrary(hmod);
		}

		constexpr bool operator==(const Library& other) const {
			return hmod == other.hmod;
		}

		constexpr operator bool() const {
			return hmod != 0;
		}

		constexpr void* operator*() const {
			return hmod;
		}

		inline void* operator[](size_t offset) const {
			return reinterpret_cast<byte*>(hmod) + offset;
		}

		inline void* operator[](const char* name) const {
			return GetProcAddress(hmod, name);
		}

		inline const char* GetName() const {
			return GetLibraryName(hmod);
		}

		inline const char* GetPath() const {
			return GetLibraryPath(hmod);
		}

		inline const char* GetPDB() const {
			return LocatePDB(hmod);
		}

		inline PIMAGE_NT_HEADERS GetNTHeader() const {
			return process::PImageNtHeader(hmod);
		}

		inline PIMAGE_OPTIONAL_HEADER GetOptHeader() const {
			return process::PImageOptHeader(hmod);
		}

		inline PIMAGE_DOS_HEADER GetDosHeader() const {
			return process::PImageDosHeader(hmod);
		}

		inline std::vector<ScanResult> Scan(const char* pattern, bool single = false) const {
			return ScanLibrary(hmod, pattern, single);
		}

		ScanResult ScanSingle(const char* pattern, const char* name = nullptr) const {
			auto res = Scan(pattern);

			if (res.empty()) {
				throw std::runtime_error(utils::va("Can't find pattern %s", name ? name : pattern));
			}
			if (res.size() != 1) {
				throw std::runtime_error(utils::va("Too many finds for pattern %s", name ? name : pattern));
			}

			return res[0];
		}

		inline std::vector<ScanResult> ScanString(const char* str, bool single = false) const {
			return ScanLibraryString(hmod, str, single);
		}

		ScanResult ScanStringSingle(const char* str, const char* name = nullptr) const {
			auto res = ScanString(str);

			if (res.empty()) {
				throw std::runtime_error(utils::va("Can't find string %s", name ? name : str));
			}
			if (res.size() != 1) {
				throw std::runtime_error(utils::va("Too many finds for string %s", name ? name : str));
			}

			return res[0];
		}


		template<typename T>
		inline std::vector<ScanResult> ScanNumber(T val, bool single = false) const {
			return ScanLibraryNumber(hmod, val, single);
		}

		template<typename T>
		ScanResult ScanNumberSingle(T val, const char* name = nullptr) const {
			auto res = ScanNumber(val);

			if (res.empty()) {
				throw std::runtime_error(name ? utils::va("Can't find number %s", name) : utils::va("Can't find number %lld", val));
			}
			if (res.size() != 1) {
				throw std::runtime_error(name ? utils::va("Too many finds for number %s", name) : utils::va("Too many finds for number %lld", val));
			}

			return res[0];
		}

		template<typename T>
		const T* Get(size_t loc) const {
			return reinterpret_cast<T*>((*this)[loc]);
		}

		template<typename T>
		T* Get(size_t loc) {
			return reinterpret_cast<T*>((*this)[loc]);
		}

		template<typename T>
		T GetProc(const char* name) {
			return (T)GetProcAddress(hmod, name);
		}

		std::vector<const char*> GetIATModules() const;
		void PatchIAT();

		template<typename T = void>
		T* Rebase(uintptr_t origin) const {
			return reinterpret_cast<T*>(origin - GetOptHeader()->ImageBase + reinterpret_cast<uintptr_t>(hmod));
		}

		template<typename T = void>
		T* Rebase(T* origin) const {
			return Rebase<T>(reinterpret_cast<uintptr_t>(origin));
		}

		template<typename T = uintptr_t>
		T Rloc(uintptr_t origin) const {
			return (T)(origin - reinterpret_cast<uintptr_t>(hmod));
		}
		template<typename T = uintptr_t, typename T2 = void>
		T Rloc(T2* origin) const {
			return Rloc<T>(reinterpret_cast<uintptr_t>(origin));
		}

		template<typename T = uintptr_t>
		T RlocBased(uintptr_t origin) const {
			return (T)(origin - GetOptHeader()->ImageBase);
		}
		template<typename T = uintptr_t, typename T2 = void>
		T RlocBased(T2* origin) const {
			return RlocBased<T>(reinterpret_cast<uintptr_t>(origin));
		}

		friend std::ostream& operator<<(std::ostream& out, const Library& ptr);
	};

	// Detour structure
	class Detour {
		void* origin{};
		void* base{};
		void* to{};
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
			this->to = to;

			DetourAttach(&(PVOID&)this->base, to);

			LONG error = DetourTransactionCommit();

			if (error != NO_ERROR) {
				throw std::runtime_error(utils::va("Can't commit detour %p -> %p", base, to));
			}
		}

		void Clear() {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			DetourDetach(&(PVOID&)this->base, this->to);

			LONG error = DetourTransactionCommit();

			if (error != NO_ERROR) {
				throw std::runtime_error(utils::va("Can't commit clear detour %p -> %p", base));
			}
		}

		constexpr operator bool() const {
			return base;
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

	// Code pointer for logs
	class CodePointer {
		void* location;
	public:
		CodePointer(void* location);

		constexpr void* operator*() const {
			return location;
		}

		friend std::ostream& operator<<(std::ostream& out, const CodePointer& ptr);
	};
}

template<>
struct std::formatter<hook::library::CodePointer, char> : utils::BasicFormatter<hook::library::CodePointer> {};
template<>
struct std::formatter<hook::library::Library, char> : utils::BasicFormatter<hook::library::Library> {};