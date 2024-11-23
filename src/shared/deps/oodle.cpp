#include <includes_shared.hpp>
#include "oodle.hpp"
#include <hook/library.hpp>

namespace deps::oodle {
    namespace {
        hook::library::Library oodle{};

        size_t(*OodleLZ_DecompressPtr)(
            void* src, size_t srcLen, void* dest, size_t destLen,
            int a5,
            int a6,
            int a7,
            __int64 a8,
            __int64 a9,
            unsigned int(*a10)(__int64, __int64, __int64, __int64),
            __int64 a11,
            __int64 a12,
            __int64 a13,
            int a14) {};
        void (*Oodle_GetConfigValuesPtr)(int32_t* cfg){};
    }

    bool LoadOodle(const char* libname) {
        FreeOodle();
        oodle.SetModule(libname, false, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

        if (!oodle) {
            LOG_ERROR("Can't load oodle lib {}", libname);
            return false;
        }

        OodleLZ_DecompressPtr = oodle.GetProc<decltype(OodleLZ_DecompressPtr)>("OodleLZ_Decompress");
        if (!OodleLZ_DecompressPtr) {
            LOG_ERROR("Can't load find OodleLZ_Decompress in lib {}, is this a oodle dll?", libname);
            FreeOodle();
            return false;
        }
        if (Oodle_GetConfigValuesPtr = oodle.GetProc<decltype(Oodle_GetConfigValuesPtr)>("Oodle_GetConfigValues")) {
            LOG_TRACE("Loaded oodle version 0x{:x}", GetVersion());
        }

        return true;
    }
    bool LoadOodleFromGame(const hook::library::Library& game) {
        for (const char* depNameCS : game.GetIATModules()) {
            std::string_view depName{ depNameCS };

            if (depName.starts_with(OO2CORE_PREFIX)) {
                return LoadOodle(depNameCS);
            }
        }

        LOG_ERROR("Can't find Oodle core library prefix in {}", game);
        return false;
    }

    void FreeOodle() {
        oodle.Free();
        OodleLZ_DecompressPtr = nullptr;
    }

    void GetConfigValues(int32_t* cfg) {
        if (!Oodle_GetConfigValuesPtr) {
            throw std::runtime_error("Oodle_GetConfigValues not available or oodle not loaded");
        }
        Oodle_GetConfigValuesPtr(cfg);
    }

    int32_t GetVersion() {
        int32_t cfg[7];
        GetConfigValues(cfg);
        return cfg[6];
    }

    const hook::library::Library& GetOodleLib() {
        return oodle;
    }

    size_t Decompress(void* src, size_t srcLen, void* dest, size_t destLen) {
        if (!OodleLZ_DecompressPtr) {
            throw std::runtime_error("Oodle not loaded");
        }

        return OodleLZ_DecompressPtr(src, srcLen, dest, destLen, 0, 0, 0, 0, 0, nullptr, 0, 0, 0, 0);
    }
}