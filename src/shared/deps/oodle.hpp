#pragma once
#include <hook/library.hpp>

namespace deps::oodle {
	constexpr const char* OO2CORE_PREFIX = "oo2core_";
	constexpr const char* OO2CORE_6 = "oo2core_6_win64";
	constexpr const char* OO2CORE_7 = "oo2core_7_win64";
	constexpr const char* OO2CORE_8 = "oo2core_8_win64";

    enum OodleFuzeSafe : int {
        OODLE_FS_NO = 0,
        OODLE_FS_YES = 1
    };

    enum OodleCheckCrcValues : int {
        OODLE_CRC_NO = 0,
        OODLE_CRC_YES = 1
    };

    enum OodleVerbosity : int {
        OODLE_VERB_NONE = 0,
        OODLE_VERB_MIN = 1,
        OODLE_VERB_SOME = 2,
        OODLE_VERB_LOTS = 3
    };

    enum OodleThreadPhase : int {
        OODLE_TP_ThreadPhase1 = 1,
        OODLE_TP_ThreadPhase2 = 2,
        OODLE_TP_ThreadPhaseAll = 3,
        OODLE_TP_Unthreaded = OODLE_TP_ThreadPhaseAll
    };

    enum OodleCompressor : int {
        OODLE_COMP_NONE = 3,
        OODLE_COMP_KRAKEN = 8,
        OODLE_COMP_LEVIATHAN = 13,
        OODLE_COMP_MERMAID = 9,
        OODLE_COMP_SELKIE = 11,
        OODLE_COMP_HYDRA = 12,
    };
    enum OodleCompressionLevel : int {
        OODLE_COMPL_NONE = 0,
        OODLE_COMPL_SUPERFAST = 1,
        OODLE_COMPL_NORMAL = 4,
        OODLE_COMPL_OPTIMAL5 = 9,
    };

    typedef int(*POodleLZ_Compress)(
        OodleCompressor compressor,
        const void* src, 
        int32_t srcLen, 
        void* dest,
        OodleCompressionLevel level,
        const void* pOptions,
        const void* dictionaryBase,
        const void* lrm,
        void* scratchMem,
        int32_t scratchSize
    );

    typedef int32_t(*POodleLZ_Decompress)(
        void* src, 
        uint32_t srcLen, 
        void* dest, 
        uint32_t destLen,
        OodleFuzeSafe fuzzSafe,
        OodleCheckCrcValues checkCrc,
        OodleVerbosity verbosity,
        byte* decBufBase,
        uint64_t decBufSize,
        uint64_t fpCallback,
        void* callbackUserData,
        byte* decoderMemory,
        uint64_t decoderMemorySize,
        OodleThreadPhase threadPhase);
    typedef void(*POodle_GetConfigValues)(int32_t* cfg);

    typedef int32_t(*POodleLZ_GetCompressedBufferSizeNeeded)(OodleCompressor compressor, int32_t rawSize);


    class Oodle {
        hook::library::Library oodle{ (HMODULE)0 };
        POodleLZ_Decompress OodleLZ_Decompress{};
        POodle_GetConfigValues Oodle_GetConfigValues{};
        POodleLZ_GetCompressedBufferSizeNeeded OodleLZ_GetCompressedBufferSizeNeeded{};
        POodleLZ_Compress OodleLZ_Compress{};

    public:
        Oodle() {}
        Oodle(const char* libname) {
            LoadOodle(libname);
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

            if ((OodleLZ_GetCompressedBufferSizeNeeded = oodle.GetProc<POodleLZ_GetCompressedBufferSizeNeeded>("OodleLZ_GetCompressedBufferSizeNeeded"))
                && (OodleLZ_Compress = oodle.GetProc<POodleLZ_Compress>("OodleLZ_Compress"))) {
                LOG_TRACE("Loaded oodle compress");
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
            OodleLZ_Decompress = nullptr;
            Oodle_GetConfigValues = nullptr;
            oodle.ClearModule();
        }

        void GetConfigValues(int32_t* cfg) const {
            if (!Oodle_GetConfigValues) {
                throw std::runtime_error("Oodle_GetConfigValues not available or oodle not loaded");
            }
            Oodle_GetConfigValues(cfg);
        }

        void Compress(OodleCompressor compressor, const void* src, int32_t srcLen, void* dest, OodleCompressionLevel level = OODLE_COMPL_NORMAL) const {
            if (!OodleLZ_Compress) {
                throw std::runtime_error("OodleLZ_Compress not available or oodle not loaded");
            }
            OodleLZ_Compress(compressor, src, srcLen, dest, level, nullptr, nullptr, nullptr, nullptr, 0);
        }

        int32_t GetCompressedBufferSizeNeeded(OodleCompressor compressor, int32_t rawSize) const {
            if (!OodleLZ_GetCompressedBufferSizeNeeded) {
                throw std::runtime_error("OodleLZ_GetCompressedBufferSizeNeeded not available or oodle not loaded");
            }
            return OodleLZ_GetCompressedBufferSizeNeeded(compressor, rawSize);
        }

        int32_t GetVersion() const {
            int32_t cfg[7];
            GetConfigValues(cfg);
            return cfg[6];
        }

        constexpr const hook::library::Library& GetOodleLib() const {
            return oodle;
        }

        int Decompress(
            void* src, uint32_t srcLen, void* dest, uint32_t destLen, 
            OodleFuzeSafe fuzeSafe = OODLE_FS_NO, OodleCheckCrcValues checkCrc = OODLE_CRC_NO, OodleVerbosity verbosity = OODLE_VERB_NONE,
            OodleThreadPhase threadPhase = OODLE_TP_Unthreaded) const {
            if (!OodleLZ_Decompress) {
                throw std::runtime_error("Oodle not loaded");
            }

            return OodleLZ_Decompress(src, srcLen, dest, destLen, fuzeSafe, checkCrc, verbosity, nullptr, 0, 0, nullptr, nullptr, 0, threadPhase);
        }
    };
}