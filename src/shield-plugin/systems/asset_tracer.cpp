#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <core/config.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <hook/error.hpp>
#include <utils/enumlist.hpp>
#include <games/bo4/pool.hpp>

namespace {
	hook::library::Detour DB_FindXAssetHeader_Detour;
	hook::library::Detour DB_GetXAssetName_Detour;
	utils::EnumList<games::bo4::pool::XAssetType, games::bo4::pool::XAssetType::ASSET_TYPE_COUNT> assetTrace{ &games::bo4::pool::XAssetIdFromName };


	void* DB_FindXAssetHeader_Stub(bo4::XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
		if (assetTrace[type]) {
			LOG_INFO("MATCH asset type {}:{}", games::bo4::pool::XAssetNameFromId(type), core::hashes::ExtractTmp("hash", *name));
			hook::error::DumpStackTraceFrom(core::logs::LVL_INFO);
		}
		return DB_FindXAssetHeader_Detour.Call<void*>(type, name, errorIfMissing, waitTime);
	}

	XHash* DB_GetXAssetName_Stub(XHash* retstr, const bo4::XAsset* asset) {
		XHash* name{ DB_GetXAssetName_Detour.Call<XHash*>(retstr, asset) };

		if (assetTrace[asset->type]) {
			LOG_INFO("MATCH asset type {}:{}", games::bo4::pool::XAssetNameFromId(asset->type), core::hashes::ExtractTmp("hash", *name));
			hook::error::DumpStackTraceFrom(core::logs::LVL_INFO);
		}

		return name;
	}

	void PostInit(uint64_t uid) {
		std::string assetTraceCfg{ core::config::GetString("test.assettrace")};
		assetTrace.LoadConfig(assetTraceCfg.data());
		if (!assetTrace.Empty()) {
			DB_FindXAssetHeader_Detour.Create(0x2EB75B0_a, DB_FindXAssetHeader_Stub);
			//DB_GetXAssetName_Detour.Create(0x28AAB60_a, DB_GetXAssetName_Stub);
			LOG_INFO("added hook for tracer {}", assetTraceCfg);
		}
	}


	REGISTER_SYSTEM(asset_tracer, nullptr, PostInit);
}