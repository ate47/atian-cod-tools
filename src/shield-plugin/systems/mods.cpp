#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/memory_allocator.hpp>
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
		typedef int DBFile;
		typedef int PMemStack;

		enum ZoneFlags : __int64 {
			DB_ZONE_PRE_GFX = 0x1,
			DB_ZONE_CORE_GFX = 0x2,
			DB_ZONE_POST_GFX = 0x4,
			DB_ZONE_UI = 0x8,
			DB_ZONE_DEV = 0x10,
			DB_ZONE_COMMON = 0x20,
			DB_ZONE_SESSIONMODE_COMMON = 0x40,
			DB_ZONE_LEVEL_LOW = 0x80,
			DB_ZONE_PRELOAD_LEVEL_LOW = 0x100,
			DB_ZONE_MISSION_LOW = 0x200,
			DB_ZONE_LOW_MARKER = 0x400,
			DB_ZONE_FFOTD = 0x800,
			DB_ZONE_SESSIONMODE_FFOTD = 0x1000,
			DB_ZONE_PLAYLISTS = 0x2000,
			DB_ZONE_MERGE = 0x4000,
			DB_ZONE_MYCHANGES = 0x8000,
			DB_ZONE_MID_MARKER = 0x10000,
			DB_ZONE_MISSION_HIGH = 0x200000,
			DB_ZONE_CORE_FRONTEND = 0x400000,
			DB_ZONE_PRELOAD_FRONTEND = 0x800000,
			DB_ZONE_LEVEL_HIGH = 0x1000000,
			DB_ZONE_PRELOAD_LEVEL_HIGH = 0x2000000,
			DB_ZONE_HIGH_MARKER = 0x4000000,
			DB_FLAG_LOC = 0x8000000,
			DB_FLAG_PATCH = 0x10000000,
			DB_FLAG_DONT_FREE_PMEM = 0x20000000,
			DB_FLAG_NOAUTOFREE = 0x40000000,
			DB_FLAG_IGNORE_MISSING = 0x80000000,
		};

		struct XZoneInfo {
			const char* name;
			int allocFlags;
			int freeFlags;
			int allocSlot;
			int freeSlot;
			XZoneBuffer fileBuffer;
		};

		struct {
			std::unordered_map<std::string, std::vector<XZoneInfo>> hooks{};
			core::memory_allocator::MemoryAllocator alloc{};
		} zoneHooks;

		hook::library::Detour DB_LoadXAssets_Detour;

		constexpr bool forceHook = true;

		void DB_LoadXAssets(XZoneInfo* zoneInfo, int zoneCount, bool sync) {
			std::vector<XZoneInfo> any;
			if (forceHook) {
				any.insert(any.end(), zoneInfo, zoneInfo + zoneCount);
			}

			for (size_t i = 0; i < zoneCount; i++) {
				if (!zoneInfo[i].name) continue;

				auto it{ zoneHooks.hooks.find(zoneInfo[i].name) };

				if (it == zoneHooks.hooks.end()) continue; // nothing to hook

				// match zone name
				if (any.empty()) {
					// add base zones
					any.insert(any.end(), zoneInfo, zoneInfo + zoneCount);
				}

				// add hook zones
				LOG_INFO("Hook {} -> {} fast file(s)", zoneInfo[i].name, it->second.size());
				for (XZoneInfo& nfo : it->second) {
					LOG_DEBUG("- {}", nfo.name ? nfo.name : "null");
				}
				any.insert(any.end(), it->second.begin(), it->second.end());
			}

			if (!any.empty()) {
				zoneInfo = any.data();
				zoneCount = (int)any.size();
			}


			LOG_TRACE("DB_LoadXAssets({}, {})", zoneCount, sync);

			for (size_t i = 0; i < zoneCount; i++) {
				XZoneInfo* nfo{ zoneInfo + i };
				LOG_TRACE("- {} 0x{:x}/0x{:x} 0x{:x}/0x{:x} ({}, 0x{:x})",
					nfo->name ? nfo->name : "null",
					nfo->allocFlags, nfo->freeFlags,
					nfo->allocSlot, nfo->freeSlot,
					(void*)nfo->fileBuffer.data, nfo->fileBuffer.dataSize
				);
			}
			DB_LoadXAssets_Detour.Call(zoneInfo, zoneCount, sync);
		}

		hook::library::Detour DB_LoadXFile_Detour;
		hook::library::Detour Stream_OpenFile_Detour;

		bool DB_LoadXFile(
			const char* path,
			DBFile f,
			XZoneBuffer* fileBuffer,
			const char* filename,
			void* assetList,
			void* blocks,
			void* interrupt,
			byte* buf,
			PMemStack side,
			int flags) {
			if (path) LOG_INFO("DB_LoadXFile({}, 0x{:x})", path, flags);

			return DB_LoadXFile_Detour.Call<bool>(path, f, fileBuffer, filename, assetList, blocks, interrupt, buf, side, flags);
		}

		int Stream_OpenFile(const char* filename, int flags) {
			if (filename) {
				LOG_INFO("Stream_OpenFile({}, 0x{:x})", filename, flags);
			}

			return Stream_OpenFile_Detour.Call<int>(filename, flags);
		}

		void ModsInit(uint64_t uid) {
			// hook the custom ff
			zoneHooks.hooks["core_common"].emplace_back(
				XZoneInfo{
					.name = "core_acts",
					.allocFlags = 0x20 | 0x800000,
					.freeFlags = 0x20 | 0x800000,
				}
			);
		}

		void ModsPostInit(uint64_t uid) {
			DB_LoadXAssets_Detour.Create(0x2EB8BC0_a, DB_LoadXAssets);
			DB_LoadXFile_Detour.Create(0x2E0CC10_a, DB_LoadXFile);
			Stream_OpenFile_Detour.Create(0x3C4CDE0_a, Stream_OpenFile);
		}
#ifndef CI_BUILD
		REGISTER_SYSTEM(mods, ModsInit, ModsPostInit);
#endif
	}
}