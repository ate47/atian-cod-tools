#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include "data/cw.hpp"

namespace core {
	cw::XAssetPool* xassetpools;
	byte* (*DecryptString)(byte* str) {};
}

namespace {
	void Init(uint64_t id) {
		LOG_INFO("Init");
	}

	void PostInit(uint64_t id) {
		LOG_INFO("Post init");

		auto pool = hook::library::QueryScanContainerSingle("xassetpool", "48 8D 05 ? ? ? ? 48 C1 E2 ? 48 03 D0");
		core::xassetpools = pool.GetRelative<int32_t, cw::XAssetPool*>(3);
		core::DecryptString = hook::library::QueryScanContainerSinglePtr<decltype(core::DecryptString)>("DecryptString", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6");
	}
}
REGISTER_SYSTEM(core, Init, PostInit);