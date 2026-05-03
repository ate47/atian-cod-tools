#include <dll_includes.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <hook/error.hpp>
#include <hook/library.hpp>
#include <games/bo4/pool.hpp>
#include <systems/mods.hpp>
#include <core/hashes/hash_store.hpp>

namespace systems::mods {
	std::vector<ModLoadingHook*>& GetModLoadingHooks() {
		static std::vector<ModLoadingHook*> hooks{};
		return hooks;
	}

	namespace {
		std::filesystem::path moddir{ "project-bo4/acts/mods" };
		std::filesystem::path zonedir{ "project-bo4/acts/zone" };

		struct XZoneBuffer {
			void* data;
			size_t dataSize;
		};
		typedef int DBFile;

		enum PMemStack : int32_t {
			PMEM_STACK_DB_COMMON = 0x0,
			PMEM_STACK_DB_MAP_A = 0x1,
			PMEM_STACK_DB_MAP_B = 0x2,
			PMEM_STACK_GAME = 0x3,
			PMEM_STACK_SERVER = 0x4,
			PMEM_STACK_HOT = 0x5,
			PMEM_STACK_CINEMATICS = 0x6,
			PMEM_STACK_UI = 0x7,
			PMEM_STACK_DYNAMIC_IMAGES = 0x8,
			PMEM_STACK_LIGHTING = 0x9,
			PMEM_STACK_SOUND = 0xA,
			PMEM_STACK_BINARY_PATCH = 0xB,
			PMEM_STACK_DEMO_DOWNLOAD = 0xC,
			PMEM_STACK_PER_LEVEL = 0xD,
			PMEM_STACK_DEBUG = 0xE,
			PMEM_STACK_COUNT = 0xF,
			PMEM_STACK_INVALID = -1,
			PHYS_ALLOC_LOW = 0x0,
			PHYS_ALLOC_HIGH = 0x3,
			PHYS_ALLOC_COUNT = 0xF,
		};

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
			DB_ZONE_BOOTSTRAP = 0x1,
			DB_ZONE_UI = 0x2,
			DB_ZONE_DEV = 0x4,
			DB_ZONE_COMMON = 0x8,
			//DB_ZONE_UNK0x10 = 0x10,
			DB_ZONE_SESSIONMODE_COMMON = 0x20,
			//DB_ZONE_UNK0x40 = 0x40,
			//DB_ZONE_UNK0x80 = 0x80,
			DB_ZONE_CORE_FRONTEND = 0x100,
			DB_ZONE_LEVEL_UI = 0x200,
			DB_ZONE_LEVEL = 0x400,
			DB_ZONE_PRELOAD_LEVEL = 0x800,
			//DB_ZONE_UNK0x1000 = 0x1000,
			DB_ZONE_FFOTD = 0x2000,
			DB_ZONE_SESSIONMODE_FFOTD = 0x4000,
			DB_ZONE_PLAYLISTS = 0x8000,
			DB_ZONE_MERGE = 0x10000,
			DB_ZONE_MYCHANGES = 0x20000,
			DB_FLAG_LOC = 0x80000,
			DB_FLAG_RES = 0x400000,
			DB_FLAG_IGNORE_MISSING = 0x800000,
			DB_FLAG_NOAUTOFREE = 0x1000000,
			DB_FLAG_CUSTOM = 0x40000000
		};

		core::config::ConfigEnumData fastfileTypeConfig[]{
			{ "ui", DB_ZONE_UI },
			{ "dev", DB_ZONE_DEV },
			{ "common", DB_ZONE_COMMON },
			{ "level", DB_ZONE_LEVEL },
			{ "preload_level", DB_ZONE_PRELOAD_LEVEL },
			{ "ffotd", DB_ZONE_FFOTD },
			{ "playlists", DB_ZONE_PLAYLISTS },
		};

		struct XZoneInfo {
			const char* name;
			int32_t allocFlags;
			int32_t freeFlags;
			int32_t allocSlot;
			int32_t freeSlot;
			XZoneBuffer fileBuffer;
		};

		struct XAsset {
			bo4::XAssetType type;
			byte unk1;
			void* header;
		};

		typedef char str64_t[64];

		struct DB_ExpandZoneList_Data {
			str64_t** names;
			int32_t* count;
			XZoneInfo** output;
		};

		struct CustomZone {
			const char* name{ "invalid"};
			const char* defaultLanguage{};
			std::vector<XZoneInfo> info{};
		};

		struct {
			std::unordered_map<uint64_t, CustomZone> zones{};
			std::unordered_map<uint64_t, CustomZone*> hooks{};
			std::vector<const char*> zoneNames{};
			std::unordered_map<uint64_t, uint64_t> nameHooks[bo4::XAssetType::ASSET_TYPE_COUNT];
			core::memory_allocator::MemoryAllocator alloc{};
		} zoneHooks;


		hook::library::Detour DB_LoadXFile_Detour;
		hook::library::Detour DB_AllocXBlocks_Detour;
		hook::library::Detour DB_ValidateFileHeader_Detour;
		hook::library::Detour DB_AuthLoad_AnalyzeData_Detour;
		hook::library::Detour PMem_Free_Detour;
		hook::library::Detour DB_LoadXAssets_Detour;
		hook::library::Detour DB_LinkXAssetEntry_Detour;
		hook::library::Detour UnkCheckSum_Detour;
		hook::library::Detour DB_FindXAssetHeader_Detour;
		hook::library::Detour DB_DoesXAssetExist_Detour;
		hook::library::Detour Stream_OpenFileInternal_Detour;
		hook::library::Detour PMem_EndAlloc2_Detour;
		hook::library::Detour PMem_BeginAlloc_Detour;
		hook::library::Detour DB_ExpandZoneList_Detour;
		
		constexpr bool forceHook = true;

		void PMem_EndAlloc2_Stub(PMemStack stack, XHash* name, void* a3) {
			LOG_TRACE("PMem_EndAlloc2({}, {})  {}", (int)stack, core::hashes::ExtractTmp("hash", name ? name->name : 0), hook::library::CodePointer{ _ReturnAddress() });
			PMem_EndAlloc2_Detour.Call(stack, name, a3);
		}

		void PMem_BeginAlloc_Stub(XHash* name, PMemStack stack, void* a3) {
			LOG_TRACE("PMem_BeginAlloc({}, {})  {}", (int)stack, core::hashes::ExtractTmp("hash", name ? name->name : 0), hook::library::CodePointer{ _ReturnAddress() });
			PMem_BeginAlloc_Detour.Call(name, stack, a3);
		}

		bool IsCustomZone(const char* zoneName) {
			return std::find_if(
				zoneHooks.zoneNames.begin(), zoneHooks.zoneNames.end(),
				[zoneName](const char* zn) { return !_strcmpi(zoneName, zn); }
			) != zoneHooks.zoneNames.end();
		}

		const char* AddCustomZone(const char* zone) {
			auto it{ std::find_if(
				zoneHooks.zoneNames.begin(), zoneHooks.zoneNames.end(),
				[zone](const char* zn) { return !_strcmpi(zone, zn); }
			) };
			if (it != zoneHooks.zoneNames.end()) {
				return *it;
			}
			zone = zoneHooks.alloc.CloneStr(zone);
			zoneHooks.zoneNames.push_back(zone);
			return zone;
		}

		bool CustomFileExists(const char* prefix, const char* zone, const char* extension) {
			std::filesystem::path file{ zonedir / std::format("{}{}.{}", prefix, zone, extension) };
			return std::filesystem::exists(file);
		}

		bool DB_ValidateXZoneInfo(XZoneInfo* info) {
			if (!info->name || (info->allocFlags & DB_FLAG_CUSTOM) == 0 || CustomFileExists("", info->name, "ff")) {
				return true; // empty or not custom, we don't touch
			}
			// the custom asked fast file doesn't exist

			const char* savedName{ info->name };
			// only our zones
			bool res{ (info->allocFlags & DB_FLAG_RES) != 0 };

			std::string_view sv{ info->name };
			size_t div{ sv.find_first_of('_') };
			const char* zone{ info->name };
			if (div != std::string::npos) {
				zone = &zone[div] + 1;
			}

			auto zit{ zoneHooks.zones.find(hash::Hash64(zone)) };
			if (zit == zoneHooks.zones.end()) {
				LOG_WARNING("INVALID ZONE WITH CUSTOM FLAGS: {}/{}", info->name, zone);
			}
			else {
				CustomZone& cz{ zit->second };

				if (res) {
					if (bo4::DB_Is4KEnabled() && CustomFileExists("1080_", zone, "ff")) {
						info->name = AddCustomZone(utils::va("1080_%s", zone));
					}
					else {
						LOG_INFO("Missing custom res fastfile {}, ignored", info->name);
						return false; // no default
					}
				}
				else if ((info->allocFlags & DB_FLAG_LOC) != 0) {
					if (cz.defaultLanguage) {
						// if the default language doesn't exist, it is a normal error
						info->name = AddCustomZone(utils::va("%s_%s", cz.defaultLanguage, zone));
					}
					else {
						LOG_WARNING("Missing custom localized fastfile {}, ignored", info->name);
						return false;
					}
				}
				else {
					LOG_WARNING("Missing custom fastfile with unknown type {}, ignored", info->name);
					return false;
				}
			}
			if (savedName != info->name) {
				LOG_INFO("redirected missing zone {} -> {}", savedName, info->name);
			}
			return true; // redirected or valid
		}

		int DB_ExpandZoneList_Stub(XZoneInfo* input, int inputCount, str64_t* names, XZoneInfo* output, int maxOutputCount) {
			int count{ DB_ExpandZoneList_Detour.Call<int>(input, inputCount, names, output, maxOutputCount) };

			XZoneInfo* valid{ output };
			XZoneInfo* current{ output };

			int newCount{};
			LOG_TRACE("DB_ExpandZoneList({}) -> {} {}", inputCount, count, hook::library::CodePointer{ _ReturnAddress() });
			for (size_t i = 0; i < count; i++) {
				XZoneInfo* c{ current++ };
				bool zoneValid{ DB_ValidateXZoneInfo(c) };
				LOG_TRACE("- {}(allocFlags=0x{:x} freeFlags=0x{:x}) {}", (c->name ? c->name : "<null>"), c->allocFlags, c->freeFlags, (zoneValid ? "" : " -> removed"));
				if (!zoneValid) {
					continue;
				}

				if (valid != c) {
					std::memcpy(valid, c, sizeof(*c));
				}

				valid++;
				newCount++;
			}

			return newCount;
		}

		void DB_LoadXAssets(XZoneInfo* zoneInfo, int zoneCount, bool sync) {
			std::vector<XZoneInfo> any;
			if (forceHook) {
				any.insert(any.end(), zoneInfo, zoneInfo + zoneCount);
			}

			for (size_t i = 0; i < zoneCount; i++) {
				if (!zoneInfo[i].name) continue;

				auto it{ zoneHooks.hooks.find(hash::Hash64(zoneInfo[i].name)) };

				if (it == zoneHooks.hooks.end()) continue; // nothing to hook

				// match zone name
				if (any.empty()) {
					// add base zones
					any.insert(any.end(), zoneInfo, zoneInfo + zoneCount);
				}

				// add hook zones
				LOG_INFO("Hook {} -> {} fast file(s)", zoneInfo[i].name, it->second->info.size());

				bool isPreloadLevel{ (zoneInfo[i].allocFlags & DB_ZONE_PRELOAD_LEVEL) != 0 };

				for (XZoneInfo& nfo : it->second->info) {
					LOG_DEBUG("- {}", nfo.name ? nfo.name : "null");

					// ignore in preloading
					if (isPreloadLevel && (nfo.allocFlags & DB_ZONE_LEVEL) != 0) {
						continue;
					}
					any.emplace_back(nfo);
				}
			}

			if (!any.empty()) {
				zoneInfo = any.data();
				zoneCount = (int)any.size();
			}


			LOG_TRACE("DB_LoadXAssets({}, {}) {}", zoneCount, sync, hook::library::CodePointer{ _ReturnAddress() });

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
			if (path) LOG_INFO("DB_LoadXFile({}, 0x{:x}) {}", path, flags, hook::library::CodePointer{ _ReturnAddress() });
			loadFakeFastFile = flags & DB_FLAG_CUSTOM;

			bool v{ DB_LoadXFile_Detour.Call<bool>(path, f, fileBuffer, filename, assetList, blocks, interrupt, buf, side, flags) };
			loadFakeFastFile = false;
			return v;
		}

		void DB_AllocXBlocksStub(size_t* blockSize, const char* f1, const char* f2, void* blocks, int side, int* loaded) {
			LOG_INFO("DB_AllocXBlocks({},{},...) {}", f1 ? f1 : "null", f2 ? f2 : "null", hook::library::CodePointer{ _ReturnAddress() });
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


					// kept for backward compatibility
					auto itHooks{ cfg.main.FindMember("fastfile_hook") };

					if (itHooks != cfg.main.MemberEnd() && itHooks->value.IsArray()) {
						LOG_WARNING("fastfile_hook config is a deprecated feature, please consider moving to fastfiles instead");

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

							const char* ffname{ AddCustomZone(nameIt->value.GetString()) };
							CustomZone& customZone{ zoneHooks.zones[hash::Hash64(ffname)] };
							customZone.name = ffname;

							if (hookIt == obj.MemberEnd()) {
								LOG_INFO("Loaded fastfile ref {}", ffname);
								continue;
							}
							const char* hook{ zoneHooks.alloc.CloneStr(hookIt->value.GetString()) };


							// to test, not for use
							bool isCommon = isCommonIt == obj.MemberEnd() || !isCommonIt->value.IsBool() || isCommonIt->value.GetBool();

							XZoneInfo info{};
							info.name = ffname;
							info.allocFlags = DB_FLAG_CUSTOM;
							info.freeFlags = 0;
							if (isCommon) {
								info.allocFlags |= DB_ZONE_COMMON;
								info.freeFlags |= DB_ZONE_COMMON;
							}
							customZone.info.emplace_back(info);
							zoneHooks.hooks[hash::Hash64(hook)] = &customZone;

							LOG_INFO("Loaded fastfile hook {}->{} (iscommon={})", hook, ffname, isCommon ? "true" : "false");
						}
					}

					rapidjson::Value& fastfilesData{ cfg.GetVal("fastfiles", 0, cfg.main)};
					if (!fastfilesData.IsNull()) {
						if (!fastfilesData.IsObject()) {
							throw std::runtime_error(std::format("Invalid fastfiles type in {}, not an object", modid));
						}

						for (auto& [k, v] : fastfilesData.GetObj()) {
							const char* ffname{ AddCustomZone(k.GetString()) };
							CustomZone& customZone{ zoneHooks.zones[hash::Hash64(ffname)] };
							customZone.name = ffname;

							const char* hook{ cfg.GetCString(utils::va("fastfiles.%s.hook", ffname)) };
							const char* defaultLanguage{ cfg.GetCString(utils::va("fastfiles.%s.default_language", ffname)) };

							if (defaultLanguage) {
								customZone.defaultLanguage = zoneHooks.alloc.CloneStr(defaultLanguage);
							}

							rapidjson::Value& files{ cfg.GetVal(utils::va("fastfiles.%s.files", ffname), 0, cfg.main) };

							if (files.IsArray()) {
								for (auto& file : files.GetArray()) {
									if (!file.IsString()) {
										LOG_WARNING("Invalid file type for fastfile {}", ffname);
										continue;
									}
									const char* r{ AddCustomZone(file.GetString()) };
									LOG_INFO("Loaded fastfile sub '{}'", r);
								}
							}

							if (!hook) {
								LOG_INFO("Loaded fastfile ref {}", ffname);
								continue;
							}

							int32_t flags{ cfg.GetEnumVal<int32_t>(utils::va("fastfiles.%s.type", ffname), fastfileTypeConfig, ACTS_ARRAYSIZE(fastfileTypeConfig)) };
							if (!flags) {
								LOG_WARNING("Invalid or missing zone type for {}", ffname);
								continue;
							}

							XZoneInfo info{};
							info.name = ffname;
							info.allocFlags = flags | DB_FLAG_CUSTOM;
							info.freeFlags = flags;
							customZone.info.emplace_back(info);
							zoneHooks.hooks[hash::Hash64(hook)] = &customZone;

							LOG_INFO("Loaded fastfile hook {}->{} (allocFlags=0x{:x})", hook, ffname, info.allocFlags);

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

					for (mods::ModLoadingHook* hook : GetModLoadingHooks()) {
						hook->hook(modid.data(), zoneHooks.alloc, cfg);
					}
				}
				catch (...) {
					LOG_ERROR("Exception when loading mod {}", subPath.string());
				}
			}
		}

		int Stream_OpenFileInternal_Stub(const char* name, int flags, uint8_t* key, size_t keyLen, uint8_t* iv, size_t ivLen) {
			static std::filesystem::path bo4zoneDir{ std::filesystem::absolute("zone") };
			std::filesystem::path path{ std::filesystem::absolute(name) };
			if (path.has_extension() && utils::IsSubDir(bo4zoneDir, path)) {
				path = std::filesystem::relative(path, bo4zoneDir);

				std::filesystem::path fname{ path.filename() };
				fname.replace_extension();

				std::string zoneName{ fname.string() };

				if (IsCustomZone(zoneName.c_str())) {
					// hooked, we need to redirect them to the acts/zone dir
					fname = zonedir / path;
					zoneName = fname.string();
					LOG_INFO("[Stream_OpenFileInternal({}) {}] -> {} ({})", name, hook::library::CodePointer{ _ReturnAddress() }, zoneName, (void*)zoneName.data());
					return Stream_OpenFileInternal_Detour.Call<int>(zoneName.data(), flags, key, keyLen, iv, ivLen);
				}
			}

			// LOG_TRACE("[Stream_OpenFileInternal({}) {}]", name, hook::library::CodePointer{ _ReturnAddress() });
			int r{ Stream_OpenFileInternal_Detour.Call<int>(name, flags, key, keyLen, iv, ivLen) };
			if (r < 0 && path.extension() != ".xpak") {
				LOG_TRACE("[Stream_OpenFileInternal({}) {}] Not found", name, hook::library::CodePointer{ _ReturnAddress() });
			}
			return r;
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
			Stream_OpenFileInternal_Detour.Create(0x3C4CE00_a, Stream_OpenFileInternal_Stub);
			// PMem_EndAlloc2_Detour.Create(0x3C403D0_a, PMem_EndAlloc2_Stub);
			// PMem_BeginAlloc_Detour.Create(0x3C40150_a, PMem_BeginAlloc_Stub);
			DB_ExpandZoneList_Detour.Create(0x2EBC690_a, DB_ExpandZoneList_Stub);
		}

		REGISTER_SYSTEM(mods, ModsInit, ModsPostInit);
	}
}