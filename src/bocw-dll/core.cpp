#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include "cw.hpp"

namespace core {
	cw::XAssetPool* xassetpools;
}

namespace {
	void Init(uint64_t id) {
		LOG_INFO("Init");
	}

	void PostInit(uint64_t id) {
		LOG_INFO("Post init");

		auto pool = hook::library::QueryScanContainerSingle("xassetpool", "48 8D 05 ? ? ? ? 48 C1 E2 ? 48 03 D0");
		core::xassetpools = pool.GetRelative<int32_t, cw::XAssetPool*>(3);
	}
}
REGISTER_SYSTEM(core, Init, PostInit);