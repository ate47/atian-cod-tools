#pragma once
#include <hook/library.hpp>

namespace deps::oodle {
	constexpr const char* OO2CORE_PREFIX = "oo2core_";
	constexpr const char* OO2CORE_6 = "oo2core_6_win64";
	constexpr const char* OO2CORE_7 = "oo2core_7_win64";
	constexpr const char* OO2CORE_8 = "oo2core_8_win64";

    typedef size_t(*POodleLZ_Decompress)(void* src, size_t srcLen, void* dest, size_t destLen, int a5, int a6, int a7, __int64 a8, __int64 a9, unsigned int(*a10)(__int64, __int64, __int64, __int64), __int64 a11, __int64 a12, __int64 a13, int a14);
    typedef void(*POodle_GetConfigValues)(int32_t* cfg);

    class Oodle {
        hook::library::Library oodle{};
        POodleLZ_Decompress OodleLZ_Decompress{};
        POodle_GetConfigValues Oodle_GetConfigValues{};

    public:
        Oodle() {}
        Oodle(const char* libname) {
            LoadOodle(libname);
        }
        ~Oodle() {
            oodle.Free();
        }

        constexpr operator bool() const {
            return oodle;
        }

        bool LoadOodle(const char* libname) {
            FreeOodle();
            oodle.SetModule(libname, false, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

            if (!oodle) {
                LOG_ERROR("Can't load oodle lib {}", libname);
                return false;
            }

            if (!(OodleLZ_Decompress = oodle.GetProc<POodleLZ_Decompress>("OodleLZ_Decompress"))) {
                LOG_ERROR("Can't load find OodleLZ_Decompress in lib {}, is this an oodle dll?", libname);
                FreeOodle();
                return false;
            }
            if (Oodle_GetConfigValues = oodle.GetProc<POodle_GetConfigValues>("Oodle_GetConfigValues")) {
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
            OodleLZ_Decompress = nullptr;
            Oodle_GetConfigValues = nullptr;
        }

        void GetConfigValues(int32_t* cfg) const {
            if (!Oodle_GetConfigValues) {
                throw std::runtime_error("Oodle_GetConfigValues not available or oodle not loaded");
            }
            Oodle_GetConfigValues(cfg);
        }

        int32_t GetVersion() const {
            int32_t cfg[7];
            GetConfigValues(cfg);
            return cfg[6];
        }

        constexpr const hook::library::Library& GetOodleLib() const {
            return oodle;
        }

        size_t Decompress(void* src, size_t srcLen, void* dest, size_t destLen) const {
            if (!OodleLZ_Decompress) {
                throw std::runtime_error("Oodle not loaded");
            }

            return OodleLZ_Decompress(src, srcLen, dest, destLen, 0, 0, 0, 0, 0, nullptr, 0, 0, 0, 0);
        }
    };
}