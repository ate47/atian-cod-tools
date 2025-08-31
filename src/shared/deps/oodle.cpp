#include <includes_shared.hpp>
#include "oodle.hpp"


namespace deps::oodle {

	Oodle& GetInstance() {
		static Oodle oodle{};
		static std::once_flag of{};

		std::call_once(of, [] {
			if (!oodle.LoadAny()) {
				throw std::runtime_error("Can't load oodle");
			}
		});

		return oodle;
	}

    Oodle::Oodle(const char* libname) {
        LoadOodle(libname);
    }
    namespace {
        void DefaultPrintF(int unk, const char* filename, int len, const char* fmt, ...) {
            if (HAS_LOG_LEVEL(core::logs::LVL_INFO)) {
                va_list va{};
                va_start(va, fmt);

                core::logs::log(core::logs::LVL_INFO, filename, len, utils::vap(fmt, va), false);

                va_end(va);
            }
        }
    }

    bool Oodle::LoadOodle(const char* libname) {
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

        if ((OodleLZ_GetCompressedBufferSizeNeeded.v8 = oodle.GetProc<POodleLZ_GetCompressedBufferSizeNeededV8>("OodleLZ_GetCompressedBufferSizeNeeded"))
            && (OodleLZ_Compress = oodle.GetProc<POodleLZ_Compress>("OodleLZ_Compress"))) {
            LOG_TRACE("Loaded oodle compress");

            useV8 = OodleLZ_GetCompressedBufferSizeNeeded.v8(deps::oodle::OODLE_COMP_KRAKEN, 10000) > 10000;
        }

        if (OodleCore_Plugins_SetPrintf = oodle.GetProc<POodleCore_Plugins_SetPrintf>("OodleCore_Plugins_SetPrintf")) {
            OodleCore_Plugins_SetPrintf(DefaultPrintF);
        }

        return true;
    }

    bool Oodle::LoadAny() {
        FreeOodle();

        for (int i = 0; i < 10; i++) {
            std::string libname{ std::format(OO2CORE_PATTERN, i) };
            oodle.SetModule(libname, false, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

            if (!oodle) continue;

            if (!(OodleLZ_Decompress = oodle.GetProc<POodleLZ_Decompress>("OodleLZ_Decompress"))) {
                LOG_ERROR("Can't load find OodleLZ_Decompress in lib {}, is this an oodle dll?", libname);
                FreeOodle();
                return false;
            }
            if (Oodle_GetConfigValues = oodle.GetProc<POodle_GetConfigValues>("Oodle_GetConfigValues")) {
                LOG_TRACE("Loaded oodle version 0x{:x}", GetVersion());
            }

            if ((OodleLZ_GetCompressedBufferSizeNeeded.v8 = oodle.GetProc<POodleLZ_GetCompressedBufferSizeNeededV8>("OodleLZ_GetCompressedBufferSizeNeeded"))
                && (OodleLZ_Compress = oodle.GetProc<POodleLZ_Compress>("OodleLZ_Compress"))) {
                LOG_TRACE("Loaded oodle compress");

                useV8 = OodleLZ_GetCompressedBufferSizeNeeded.v8(deps::oodle::OODLE_COMP_KRAKEN, 10000) >= 10000;
            }

            if (OodleCore_Plugins_SetPrintf = oodle.GetProc<POodleCore_Plugins_SetPrintf>("OodleCore_Plugins_SetPrintf")) {
                OodleCore_Plugins_SetPrintf(DefaultPrintF);
            }

            return true;
        }
        return false;
    }

    bool Oodle::LoadOodleFromGame(const hook::library::Library& game) {
        for (const char* depNameCS : game.GetIATModules()) {
            std::string_view depName{ depNameCS };

            if (depName.starts_with(OO2CORE_PREFIX)) {
                return LoadOodle(depNameCS);
            }
        }

        LOG_ERROR("Can't find Oodle core library prefix in {}", game);
        return false;
    }

    void Oodle::FreeOodle() {
        OodleLZ_Decompress = nullptr;
        Oodle_GetConfigValues = nullptr;
        oodle.ClearModule();
    }

    void Oodle::GetConfigValues(int32_t* cfg) const {
        if (!Oodle_GetConfigValues) {
            throw std::runtime_error("Oodle_GetConfigValues not available or oodle not loaded");
        }
        Oodle_GetConfigValues(cfg);
    }

    int Oodle::Compress(OodleCompressor compressor, const void* src, int32_t srcLen, void* dest, OodleCompressionLevel level) const {
        if (!OodleLZ_Compress) {
            throw std::runtime_error("OodleLZ_Compress not available or oodle not loaded");
        }
        return OodleLZ_Compress(compressor, src, srcLen, dest, level, nullptr, nullptr, nullptr, nullptr, 0);
    }

    int32_t Oodle::GetCompressedBufferSizeNeeded(OodleCompressor compressor, int32_t rawSize) const {
        if (!OodleLZ_GetCompressedBufferSizeNeeded.v8) {
            throw std::runtime_error("OodleLZ_GetCompressedBufferSizeNeeded not available or oodle not loaded");
        }
        if (useV8) {
            return OodleLZ_GetCompressedBufferSizeNeeded.v8(compressor, rawSize);
        }
        else {
            return OodleLZ_GetCompressedBufferSizeNeeded.v67(rawSize);
        }
    }

    int32_t Oodle::GetVersion() const {
        int32_t cfg[7];
        GetConfigValues(cfg);
        return cfg[6];
    }

    void Oodle::SetPrintf(printffunc func) const {
        if (!OodleCore_Plugins_SetPrintf) {
            throw std::runtime_error("OodleCore_Plugins_SetPrintf not available or oodle not loaded");
        }
        OodleCore_Plugins_SetPrintf(func);
    }

    int Oodle::Decompress(
        const void* src, uint32_t srcLen, void* dest, uint32_t destLen, OodleFuzeSafe fuzeSafe, OodleCheckCrcValues checkCrc, OodleVerbosity verbosity, OodleThreadPhase threadPhase,
        byte* decBufBase, uint64_t decBufSize, OodleDecompressCallback fpCallback , void* callbackUserData, byte* decoderMemory, uint64_t decoderMemorySize) const {
        if (!OodleLZ_Decompress) {
            throw std::runtime_error("Oodle not loaded");
        }

        return OodleLZ_Decompress(src, srcLen, dest, destLen, fuzeSafe, checkCrc, verbosity, decBufBase, decBufSize, fpCallback, callbackUserData, decoderMemory, decoderMemorySize, threadPhase);
    }

}