#include <dll_includes.hpp>
#include <core/system.hpp>
#include <core/config.hpp>
#include <hook/library.hpp>
#include <hook/process.hpp>
#include <Windows.h>
#include <winternl.h>
#include <conio.h>
#include <TlHelp32.h>

namespace {
    // Helper struct: one pattern byte, with wildcard flag
    struct PatternByte
    {
        std::uint8_t value;
        bool isWildcard;
    };

    // Parse a signature string like:
    std::vector<PatternByte> parse_signature(const char* signature)
    {
        if (!signature) {
            throw std::invalid_argument("signature is null");
        }

        std::vector<PatternByte> pattern;
        std::istringstream iss(signature);
        std::string token;

        while (iss >> token) {
            // Treat "??" or "?" as wildcard
            if (token == "??" || token == "?") {
                pattern.push_back({ 0u, true });
                continue;
            }

            // Basic validation: must be two hex chars
            if (token.size() != 2 ||
                !std::isxdigit(static_cast<unsigned char>(token[0])) ||
                !std::isxdigit(static_cast<unsigned char>(token[1]))) {
                throw std::invalid_argument("Invalid token in signature: " + token);
            }

            // Convert hex byte
            unsigned long val = std::stoul(token, nullptr, 16);
            pattern.push_back({ static_cast<std::uint8_t>(val & 0xFFu), false });
        }

        if (pattern.empty()) {
            throw std::invalid_argument("Empty pattern");
        }

        return pattern;
    }

    // Scan a memory range [base, base + size) for the pattern (with wildcards)
    void scan_range_for_pattern(std::uint8_t* base,
        std::size_t size,
        const std::vector<PatternByte>& pattern,
        std::vector<std::uint8_t*>& outMatches)
    {
        const std::size_t patSize = pattern.size();
        if (size < patSize) {
            return;
        }

        std::uint8_t* end = base + (size - patSize);

        for (std::uint8_t* p = base; p <= end; ++p) {
            bool match = true;
            for (std::size_t i = 0; i < patSize; ++i) {
                const PatternByte& pb = pattern[i];
                if (!pb.isWildcard && p[i] != pb.value) {
                    match = false;
                    break;
                }
            }
            if (match) {
                outMatches.push_back(p);
            }
        }
    }

    // Main function: scan all executable memory in the *current* process
    // for the given signature string, returning all match addresses.
    std::vector<std::uint8_t*> scan_executable_memory_for_signature(const char* signature)
    {
        std::vector<PatternByte> pattern = parse_signature(signature);
        std::vector<std::uint8_t*> matches;

        SYSTEM_INFO sysInfo{};
        GetSystemInfo(&sysInfo);

        auto minAddr = static_cast<std::uint8_t*>(sysInfo.lpMinimumApplicationAddress);
        auto maxAddr = static_cast<std::uint8_t*>(sysInfo.lpMaximumApplicationAddress);

        MEMORY_BASIC_INFORMATION mbi{};
        std::uint8_t* addr = minAddr;

        while (addr < maxAddr) {
            if (!VirtualQuery(addr, &mbi, sizeof(mbi))) {
                break; // can't query further
            }

            // Ensure non-zero region size to avoid infinite loops in weird cases
            if (mbi.RegionSize == 0) {
                addr += 0x1000; // advance by one page as a fallback
                continue;
            }

            bool isCommitted = (mbi.State == MEM_COMMIT);
            bool isGuard = (mbi.Protect & PAGE_GUARD) != 0;
            bool isNoAccess = (mbi.Protect & PAGE_NOACCESS) != 0;

            bool isExecutable =
                (mbi.Protect & PAGE_EXECUTE) ||
                (mbi.Protect & PAGE_EXECUTE_READ) ||
                (mbi.Protect & PAGE_EXECUTE_READWRITE) ||
                (mbi.Protect & PAGE_EXECUTE_WRITECOPY);

            if (isCommitted && !isGuard && !isNoAccess && isExecutable) {
                auto* regionStart = static_cast<std::uint8_t*>(mbi.BaseAddress);
                std::size_t regionSize = static_cast<std::size_t>(mbi.RegionSize);

                // Only scan regions that can possibly contain the whole pattern
                if (regionSize >= pattern.size()) {
                    scan_range_for_pattern(regionStart, regionSize, pattern, matches);
                }
            }

            addr += mbi.RegionSize;
        }

        return matches;
    }

    // Scan all executable memory in the current process for `searchSignature`,
    // and replace ONLY the first N bytes of each match, where N is the length
    // of `replacementSignature`. Any remaining bytes in the match (if the search
    // pattern is longer) are left untouched.
    //
    // - `searchSignature` may contain wildcards ("?" / "??").
    // - `replacementSignature` may be shorter than the search pattern, but
    //   NOT longer, and must NOT contain wildcards.
    // - Example: scan_and_replace_pattern("CC ?? CC", "90 90");  // only first 2 bytes changed
    std::vector<std::uint8_t*> scan_and_replace_pattern(const char* searchSignature,
        const char* replacementSignature)
    {
        if (!searchSignature || !replacementSignature) {
            throw std::invalid_argument("scan_and_replace_pattern: null signature argument");
        }

        // Parse both signatures.
        std::vector<PatternByte> searchPattern = parse_signature(searchSignature);
        std::vector<PatternByte> replacementPattern = parse_signature(replacementSignature);

        if (replacementPattern.empty()) {
            throw std::invalid_argument("scan_and_replace_pattern: replacement pattern must not be empty");
        }

        if (replacementPattern.size() > searchPattern.size()) {
            throw std::invalid_argument(
                "scan_and_replace_pattern: replacement pattern cannot be longer than search pattern");
        }

        // Require that the replacement has no wildcards.
        for (const auto& pb : replacementPattern) {
            if (pb.isWildcard) {
                throw std::invalid_argument(
                    "scan_and_replace_pattern: replacement pattern must not contain wildcards");
            }
        }

        const std::size_t bytesToPatch = replacementPattern.size();

        // Find all locations that match the full search signature.
        std::vector<std::uint8_t*> matches = scan_executable_memory_for_signature(searchSignature);

        std::vector<std::uint8_t*> patched; // addresses we actually modified

        for (auto* p : matches) {
            if (!p) {
                continue;
            }

            DWORD oldProtect = 0;
            // Make just the bytes we are going to overwrite RWX.
            if (!VirtualProtect(static_cast<LPVOID>(p),
                static_cast<SIZE_T>(bytesToPatch),
                PAGE_EXECUTE_READWRITE,
                &oldProtect))
            {
                // Couldn't change protection; skip this match.
                continue;
            }

            // Apply the replacement bytes over the first N bytes.
            for (std::size_t i = 0; i < bytesToPatch; ++i) {
                p[i] = replacementPattern[i].value;
            }

            // Make sure the CPU sees the modified code.
            FlushInstructionCache(GetCurrentProcess(), p, bytesToPatch);

            // Restore original protection.
            DWORD dummy = 0;
            VirtualProtect(static_cast<LPVOID>(p),
                static_cast<SIZE_T>(bytesToPatch),
                oldProtect,
                &dummy);

            patched.push_back(p);
        }

        return patched;
    }

    static const wchar_t* badWords[] = {
        L"IDA",
        L"Pro 7.7.220118",
        L"ida",
        L"HxD",
        L"cheatengine",
        L"Cheat Engine",
        L"x96dbg",
        L"x32dbg",
        L"x64dbg",
        L"Wireshark",
        L"Debug",
        L"DEBUG",
        L"msvsmon",
    };

    void EraseBadWords(wchar_t* str) {
        if (!str || !*str) {
            return;
        }
        std::wstring_view strw{ str };

        for (const wchar_t* keyword : badWords) {
            size_t pos;
            while ((pos = strw.find(keyword)) != std::string::npos) {

                for (const wchar_t* i = keyword; *i; i++) {
                    str[pos++] = L'a';
                }
            }
        }
    }

    hook::library::Detour GetWindowTextW_Detour;
    hook::library::Detour NtQuerySystemInformation_Detour;
    hook::library::Detour NtQueryInformationProcess_Detour;

    int WINAPI GetWindowTextW_Stub(HWND hWnd, LPWSTR lpString, int nMaxCount) {
        int r{ GetWindowTextW_Detour.Call<int>(hWnd, lpString, nMaxCount) };
        if (r) {
			EraseBadWords(lpString);
        }
        return r;
    }

    int WINAPI GetWindowTextA_Stub(HWND hWnd, LPSTR lpString, int nMaxCount) {
        //GetWindowTextW()
        std::wstring wbuff{};
        wbuff.resize(nMaxCount);

        int r{ GetWindowTextW(hWnd, wbuff.data(), nMaxCount) };

        if (r) {
            std::string buff{ utils::WStrToStr(wbuff) };
            std::memset(lpString, 0, nMaxCount);
			std::memcpy(lpString, buff.data(), buff.size());
        }

        return r;
    }

    NTSTATUS NTAPI NtQuerySystemInformation_Stub(
        SYSTEM_INFORMATION_CLASS SystemInformationClass,
        PVOID SystemInformation,
        ULONG SystemInformationLength,
        PULONG ReturnLength
    ) {
        NTSTATUS status{ NtQuerySystemInformation_Detour.Call<NTSTATUS>(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength) };

        if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessInformation) {
            PSYSTEM_PROCESS_INFORMATION spi{ (PSYSTEM_PROCESS_INFORMATION)SystemInformation };

            while (spi) {
                if (spi->ImageName.Length) {
                    EraseBadWords(spi->ImageName.Buffer);
                }

                spi = spi->NextEntryOffset ? (PSYSTEM_PROCESS_INFORMATION)((byte*)spi +spi->NextEntryOffset) : nullptr;
            }
        }

        return status;
    }

    NTSTATUS NTAPI NtQueryInformationProcess_Stub(
            HANDLE ProcessHandle,
            PROCESSINFOCLASS ProcessInformationClass,
            PVOID ProcessInformation,
            ULONG ProcessInformationLength,
            PULONG ReturnLength
        ) {
		NTSTATUS status{ NtQueryInformationProcess_Detour.Call<NTSTATUS>(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength) };

        constexpr DWORD ProcessDebugPort = 7;
        constexpr DWORD ProcessDebugObjectHandle = 30;
        constexpr DWORD ProcessDebugFlags = 31;
        constexpr DWORD ProcessImageFileNameWin32 = 43;

        if (NT_SUCCESS(status) && (ProcessInformationClass == ProcessImageFileName || ProcessInformationClass == ProcessImageFileNameWin32)) {
            UNICODE_STRING& str{ *(UNICODE_STRING*)ProcessInformation };

            if (str.Length) {
                EraseBadWords(str.Buffer);
            }
        }

        return status;
    }

    void PatchArxan(uint64_t uid) {
        scan_and_replace_pattern("8B 0C 8B 33 0C 82", "48 31 C9 90 90 90");
        scan_and_replace_pattern("8B 0C 8B F7 D9 03 0C 82", "48 31 C9 90 90 90 90 90");
        scan_and_replace_pattern("8B 04 82 8B 14 8B 3B C2", "48 31 C0 48 31 D2");
        LOG_TRACE("patched arxan");
    }

    void PatchArxanPre(uint64_t uid) {
        // patch dev stuff
        hook::library::Library ntdll{ GetModuleHandleA("ntdll.dll") };

        hook::memory::RedirectJmp(GetWindowTextA, GetWindowTextA_Stub, true);
        GetWindowTextW_Detour.Create(GetWindowTextW, GetWindowTextW_Stub);

        NtQuerySystemInformation_Detour.Create(ntdll["NtQuerySystemInformation"], NtQuerySystemInformation_Stub);
        NtQueryInformationProcess_Detour.Create(ntdll["NtQueryInformationProcess"], NtQueryInformationProcess_Stub);
        LOG_TRACE("patched pre-arxan");

    }
}


REGISTER_SYSTEM(arxan, PatchArxanPre, PatchArxan);