#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/memory_allocator.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <games/bo4/pool.hpp>
#include "mods.hpp"
#include <core/hashes/hash_store.hpp>

namespace systems::mods {
	namespace {

		struct XZoneBuffer {
			void* data;
			size_t dataSize;
		};
		typedef int DBFile;
		typedef int PMemStack;
		struct XFile {
			uint8_t magic[8];
			uint32_t version;
			uint8_t server;
			byte compression;
			uint8_t platform;
			uint8_t encrypted;
			uint64_t timestamp;
			uint32_t changelist;
			uint32_t archiveChecksum[4];
			char builder[32];
			uint32_t metaVersion;
			char mergeFastfile[64];
			char missionFastFiles[16][64];
			uint64_t size;
			uint64_t externalSize;
			uint64_t memMappedOffset;
			uint64_t blockSize[9];
			uint64_t unk4f0;
			uint64_t unk4f8;
			uint64_t unk500;
			uint64_t unk508;
			uint64_t unk510s;
			uint64_t unk518s;
			uint64_t unk520pa;
			byte pad0[392];
			char unkSign[64];
			char fastfileName[64];
			uint8_t signature[256];
			uint8_t aesIV[16];
		}; static_assert(sizeof(XFile) == 0x840);
		enum DBValidateResult : int {
			DBV_OK = 0,
			DBV_WARN = 1,
			DBV_ERROR = 2,
		};

		/*
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
		*/
		enum ZoneFlags : uint32_t {
			DB_ZONE_COMMON8 = 0x8,
			DB_ZONE_COMMON = 0x20,
			DB_FLAG_IGNORE_MISSING = 0x800000,
			DB_FLAG_CUSTOM = 0x40000000
		};

		struct XZoneInfo {
			const char* name;
			int allocFlags;
			int freeFlags;
			int allocSlot;
			int freeSlot;
			XZoneBuffer fileBuffer;
		};

		struct XAsset {
			bo4::XAssetType type;
			byte unk1;
			void* header;
		};


		struct {
			std::unordered_map<std::string, std::vector<XZoneInfo>> hooks{};
			std::unordered_map<uint64_t, uint64_t> nameHooks[bo4::XAssetType::ASSET_TYPE_COUNT];
			core::memory_allocator::MemoryAllocator alloc{};
		} zoneHooks;


		hook::library::Detour DB_LoadXFile_Detour;
		hook::library::Detour Stream_OpenFile_Detour;
		hook::library::Detour DB_AllocXBlocks_Detour;
		hook::library::Detour DB_ValidateFileHeader_Detour;
		hook::library::Detour DB_AuthLoad_AnalyzeData_Detour;
		hook::library::Detour PMem_Free_Detour;
		hook::library::Detour DB_LoadXAssets_Detour;
		hook::library::Detour DB_LinkXAssetEntry_Detour;
		hook::library::Detour UnkCheckSum_Detour;
		hook::library::Detour DB_FindXAssetHeader_Detour;
		hook::library::Detour DB_DoesXAssetExist_Detour;
		
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


		bool loadFakeFastFile{};

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
			loadFakeFastFile = flags & DB_FLAG_CUSTOM;

			bool v{ DB_LoadXFile_Detour.Call<bool>(path, f, fileBuffer, filename, assetList, blocks, interrupt, buf, side, flags) };
			loadFakeFastFile = false;
			return v;
		}

		int Stream_OpenFile(const char* filename, int flags) {
			if (filename) {
				LOG_INFO("Stream_OpenFile({}, 0x{:x})", filename, flags);
			}

			return Stream_OpenFile_Detour.Call<int>(filename, flags);
		}

		void DB_AllocXBlocksStub(size_t* blockSize, const char* f1, const char* f2, void* blocks, int side, int* loaded) {
			LOG_INFO("DB_AllocXBlocks({},{},...)", f1 ? f1 : "null", f2 ? f2 : "null");
			DB_AllocXBlocks_Detour.Call(blockSize, f1, f2, blocks, side, loaded);
		}

		DBValidateResult DB_ValidateFileHeader_Stub(XFile* xfile, char* reason, size_t size, bool patchdiff) {
			DBValidateResult r{ DB_ValidateFileHeader_Detour.Call<DBValidateResult>(xfile, reason, size, patchdiff) };
			switch (r) {
			case DBV_OK:
				break;
			case DBV_WARN:
				LOG_WARNING("DB_ValidateFileHeader({}) -> warning : {}", xfile ? xfile->fastfileName : "null", reason);
				break;
			case DBV_ERROR:
			default:
				LOG_ERROR("DB_ValidateFileHeader({}) -> error : {}", xfile ? xfile->fastfileName : "null", reason);
				break;
			}

			return r;
		}

		int DB_AuthLoad_AnalyzeData_Stub(uint8_t* data, uint32_t size, int activeStream, uint8_t* checksum) {
			if (!loadFakeFastFile) {
				return DB_AuthLoad_AnalyzeData_Detour.Call<int>(data, size, activeStream, checksum);
			}
			return 0;
		}

		void* DB_LinkXAssetEntry_Stub(XAsset* newEntry, bool allowOverride) {
			void* entry{ DB_LinkXAssetEntry_Detour.Call<void*>(newEntry, allowOverride) };
			if (loadFakeFastFile) {
				LOG_DEBUG(
					"DB_LinkXAssetEntry({}={}, {}, allowOverride={}) -> {}",
					games::bo4::pool::XAssetNameFromId(newEntry->type), (int)newEntry->type,
					core::hashes::ExtractTmp("hash", games::bo4::pool::GetAssetName(newEntry->type, newEntry->header)->name),
					allowOverride, entry
				);
			}
			return entry;
		}

		XHash RedirectName(bo4::XAssetType type, XHash* name) {
			auto it{ zoneHooks.nameHooks[type].find(name->name) };
			if (it != zoneHooks.nameHooks[type].end()) {
				return { it->second, 0 };
			}
			return *name;
		}

		void* DB_FindXAssetHeader_Stub(bo4::XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
			XHash other{ RedirectName(type, name) };
			return DB_FindXAssetHeader_Detour.Call<void*>(type, &other, errorIfMissing, waitTime);
		}

		bool DB_DoesXAssetExist_Stub(bo4::XAssetType type, XHash* name) {
			XHash other{ RedirectName(type, name) };
			return DB_DoesXAssetExist_Detour.Call<bool>(type, &other);
		}


		void UnkCheckSum_Stub() {
			if (!loadFakeFastFile) {
				UnkCheckSum_Detour.Call();
			}
		}

		void ModsInit(uint64_t uid) {
			std::vector<std::filesystem::path> modConfigs{};

			std::filesystem::path moddir{ "acts/mods" };
			std::filesystem::create_directories(moddir);
			for (const std::filesystem::directory_entry& sub : std::filesystem::directory_iterator{ moddir }) {
				if (!sub.is_directory()) continue; // ignore files

				std::filesystem::path subPath{ sub };
				try {
					std::filesystem::path cpath{ subPath / "config.json" };

					core::config::Config cfg{ cpath };
					cfg.SyncConfig(false);

					std::string modid{ cfg.GetString("modid") };

					LOG_INFO("Loading {}...", modid);

					auto itHooks{ cfg.main.FindMember("fastfile_hook") };

					if (itHooks != cfg.main.MemberEnd() && itHooks->value.IsArray()) {
						for (rapidjson::Value& hook : itHooks->value.GetArray()) {
							if (!hook.IsObject()) {
								LOG_WARNING("Found invalid fastfile hook: not an object");
								continue;
							}

							auto obj{ hook.GetObj() };
							
							auto nameIt{ obj.FindMember("name") };
							auto hookIt{ obj.FindMember("hook") };
							auto isCommonIt{ obj.FindMember("is_common") };

							if (nameIt == obj.MemberEnd()) {
								LOG_WARNING("Found invalid fastfile hook: missing name");
								continue;
							}

							const char* ffname{ nameIt->value.GetString() };
							if (hookIt == obj.MemberEnd()) {
								LOG_WARNING("Found invalid fastfile hook: missing hook for {}", ffname);
								continue;
							}
							const char* hook{ hookIt->value.GetString() };

							// to test, not for use
							bool isCommon = isCommonIt == obj.MemberEnd() || !isCommonIt->value.IsBool() || isCommonIt->value.GetBool();

							XZoneInfo info{};
							info.name = zoneHooks.alloc.CloneStr(ffname);
							info.allocFlags = DB_FLAG_IGNORE_MISSING | DB_FLAG_CUSTOM;
							info.freeFlags = DB_FLAG_IGNORE_MISSING;
							if (isCommon) {
								info.allocFlags |= DB_ZONE_COMMON8;
								info.freeFlags |= DB_ZONE_COMMON8;
							}
							zoneHooks.hooks[hook].emplace_back(info);

							LOG_INFO("Loaded fastfile hook {}->{} (iscommon={})", hook, ffname, isCommon ? "true" : "false");
						}
					}

					auto itRedirects{ cfg.main.FindMember("redirects") };

					if (itRedirects != cfg.main.MemberEnd() && itRedirects->value.IsArray()) {
						for (rapidjson::Value& redirect : itRedirects->value.GetArray()) {
							if (!redirect.IsObject()) {
								LOG_WARNING("Found invalid fastfile hook: not an object");
								continue;
							}
							auto obj{ redirect.GetObj() };

							auto originIt{ obj.FindMember("origin") };
							auto destIt{ obj.FindMember("dest") };
							auto typeIt{ obj.FindMember("type") };

							if (originIt == obj.MemberEnd()) {
								LOG_WARNING("Found invalid redirect: missing origin name");
								continue;
							}
							if (destIt == obj.MemberEnd()) {
								LOG_WARNING("Found invalid redirect: missing dest name");
								continue;
							}
							if (typeIt == obj.MemberEnd()) {
								LOG_WARNING("Found invalid redirect: missing type");
								continue;
							}


							const char* origin{ originIt->value.GetString() };
							const char* dest{ destIt->value.GetString() };
							const char* typeName{ typeIt->value.GetString() };

							bo4::XAssetType type{ games::bo4::pool::XAssetIdFromName(typeName) };

							if (type == bo4::XAssetType::ASSET_TYPE_COUNT) {
								LOG_WARNING("Found invalid redirect: bad type for {}->{}", origin, dest);
								continue;
							}

							uint64_t horigin{ hash::Hash64Pattern(origin) };
							uint64_t hdest{ hash::Hash64Pattern(dest) };

							zoneHooks.nameHooks[type][horigin] = hdest;
							LOG_INFO(
								"Loaded redirect {} {}(0x{:x})->{}(0x{:x})", 
								games::bo4::pool::XAssetNameFromId(type),
								origin, horigin,
								dest, hdest
							);
						}
					}
				}
				catch (...) {
					LOG_ERROR("Exception when loading mod {}", subPath.string());
				}
			}
		}

		void ModsPostInit(uint64_t uid) {
			DB_LoadXAssets_Detour.Create(0x2EB8BC0_a, DB_LoadXAssets);
			DB_LoadXFile_Detour.Create(0x2E0CC10_a, DB_LoadXFile);
			DB_AllocXBlocks_Detour.Create(0x2EB5870_a, DB_AllocXBlocksStub);
			DB_ValidateFileHeader_Detour.Create(0x2E0EBE0_a, DB_ValidateFileHeader_Stub);
			DB_AuthLoad_AnalyzeData_Detour.Create(0x28B5F40_a, DB_AuthLoad_AnalyzeData_Stub);
			DB_LinkXAssetEntry_Detour.Create(0x2EB84F0_a, DB_LinkXAssetEntry_Stub);
			UnkCheckSum_Detour.Create(0x28B5FA0_a, UnkCheckSum_Stub);
			DB_FindXAssetHeader_Detour.Create(0x2EB75B0_a, DB_FindXAssetHeader_Stub);
			DB_DoesXAssetExist_Detour.Create(0x2EB6C90_a, DB_DoesXAssetExist_Stub);
		}

		REGISTER_SYSTEM(mods, ModsInit, ModsPostInit);
	}
}