#ifndef CI_BUILD
#include <dll_includes.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <hook/library.hpp>
#include <HwBpLib.h>
#include "data/refs.hpp"

namespace {
    hook::library::Detour GetAssetHeaderDetour;

    void* GetAssetHeaderStub(bo4::XAssetType type, bo4::XHash* name, bool errorIfMissing, int32_t waitTime) {


        return GetAssetHeaderDetour.Call<void*>(type, name, errorIfMissing, waitTime);
    }

    HANDLE mainThread{};

    void Init(uint64_t id) {
        mainThread = GetCurrentThread();
    }

    void PostInit(uint64_t id) {
        core::eventhandler::RegisterEventCallback(hash::Hash64("RegisterThread"), [](void* hThread) {
            static thread_local bool init{};

            if (!init) {
                HwBp::Set(0x2EB75B0_a, 0x10, HwBp::When::ReadOrWritten);
                LOG_DEBUG("Add HWBP");
                init = true;
            }
        });
        //GetAssetHeaderDetour.Create(0x2EB75B0_a, GetAssetHeaderStub);
    }

    REGISTER_SYSTEM(drm, Init, PostInit);
}
#endif