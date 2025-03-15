#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/system.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include "mods.hpp"

namespace mods {
	namespace {
		struct XZoneBuffer {
			void* data;
			size_t dataSize;
		};

		struct XZoneInfo {
			const char* name;
			int allocFlags;
			int freeFlags;
			int allocSlot;
			int freeSlot;
			XZoneBuffer fileBuffer;
		};


		hook::library::Detour DB_LoadXAssets_Detour;

		void DB_LoadXAssets(XZoneInfo* nfo, int zoneCount, bool sync) {
			LOG_TRACE("DB_LoadXAssets({})", nfo->name ? nfo->name : "null");
			LOG_TRACE("Alloc free : 0x{:x}/0x{:x}", nfo->allocFlags, nfo->freeFlags);
			LOG_TRACE("Alloc slot : 0x{:x}/0x{:x}", nfo->allocSlot, nfo->freeSlot);
			DB_LoadXAssets_Detour.Call(nfo, zoneCount, sync);
		}

		void ModsPostInit(uint64_t uid) {
			DB_LoadXAssets_Detour.Create(0x2EB8BC0_a, DB_LoadXAssets);
		}

		REGISTER_SYSTEM(mods, nullptr, ModsPostInit);
	}
}