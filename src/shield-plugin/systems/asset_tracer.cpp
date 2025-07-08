#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <core/config.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <hook/error.hpp>
#include <utils/enumlist.hpp>
#include <games/bo4/pool.hpp>
#include <utils/data_utils.hpp>

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
	hook::library::Detour DB_SHA256_Init_Detour;
	hook::library::Detour DB_SHA256_Process_Detour;
	hook::library::Detour DB_SHA256_Final_Detour;
	static std::mutex strf{};
	//0x7ff623e317d0
	void DB_SHA256_Init_Stub(void* str) {
		{
			std::lock_guard lg{ strf };
			LOG_TRACE("DB_SHA256_Init({})", str);
			hook::error::DumpStackTraceFrom(core::logs::LVL_TRACE);
		}
		DB_SHA256_Init_Detour.Call(str);
	}
	void DB_SHA256_Process_Stub(void* str, void* sign, uint32_t size) {
		{
			std::lock_guard lg{ strf };
			LOG_TRACE("DB_SHA256_Process({}, {}, {:x})", str, sign, size);
			hook::error::DumpStackTraceFrom(core::logs::LVL_TRACE);
		}
		DB_SHA256_Process_Detour.Call(str, sign, size);
	}
	int DB_SHA256_Final_Stub(void* str, byte* end) {
		std::string h{ utils::data::AsHex(end, 32) };
		int r{ DB_SHA256_Final_Detour.Call<int>(str, end) };
		{
			std::lock_guard lg{ strf };
			LOG_TRACE("DB_SHA256_Final({}, {})", str, h);
			LOG_TRACE("           POST({}, {})", str, utils::data::AsHex(end, 32));
			hook::error::DumpStackTraceFrom(core::logs::LVL_TRACE);
		}
		return r;
	}

	void PostInit(uint64_t uid) {
		std::string assetTraceCfg{ core::config::GetString("test.assettrace")};
		assetTrace.LoadConfig(assetTraceCfg.data());
		if (!assetTrace.Empty()) {
			DB_FindXAssetHeader_Detour.Create(0x2EB75B0_a, DB_FindXAssetHeader_Stub);
			DB_GetXAssetName_Detour.Create(0x28AAB60_a, DB_GetXAssetName_Stub);
			LOG_INFO("added hook for tracer {}", assetTraceCfg);
		}
		if (core::config::GetBool("test.sha256trace", false)) {
			DB_SHA256_Init_Detour.Create(0x452E1F0_a, DB_SHA256_Init_Stub);
			DB_SHA256_Process_Detour.Create(0x452E230_a, DB_SHA256_Process_Stub);
			DB_SHA256_Final_Detour.Create(0x452E330_a, DB_SHA256_Final_Stub);
		}
	}


	REGISTER_SYSTEM(asset_tracer, nullptr, PostInit);
}