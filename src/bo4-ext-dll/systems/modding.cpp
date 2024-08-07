#include <dll_includes.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <pool.hpp>
#include <data/bo4.hpp>

namespace {
	hook::library::Detour GetAssetHeaderDetour;

	void* GetAssetHeaderStub(bo4::XAssetType type, bo4::XHash* name, bool errorIfMissing, int32_t waitTime) {


		return GetAssetHeaderDetour.Call<void*>(type, name, errorIfMissing, waitTime);
	}

	void PostInit(uint64_t id) {
		//GetAssetHeaderDetour.Create(0x2EB75B0_a, GetAssetHeaderStub);
	}

	REGISTER_SYSTEM(modding, nullptr, PostInit);
}