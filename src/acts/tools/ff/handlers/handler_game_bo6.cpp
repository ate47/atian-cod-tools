#include <includes.hpp>
#include <games/cod/asset_names.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <hook/scan_container.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <tools/bo6/bo6.hpp>
#include <decryptutils.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>


namespace fastfile::handlers::bo6 {
	using namespace ::bo6;
	constexpr bool hasRelativeLoads = false;
	constexpr bool traceData = false;

	enum StreamPointerFlag : uint64_t {
		SPF_CREATE_REF = 1ull << 61, // create a offset with the data as name
		SPF_NEXT = 2ull << 61, // load the data after, otherwise data is name of offset
		SPF_UNK = 4ull << 61, // unk

		SPF_DATA_MASK = ~(SPF_CREATE_REF | SPF_NEXT | SPF_UNK)
	};

	namespace {
		template<size_t offset = 0>
		void EmptyStub() {
			LOG_TRACE("{} EmptyStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
		}
		template<size_t offset = 0>
		void ErrorStub() {
			hook::error::DumpStackTraceFrom();
			throw std::runtime_error(std::format("{} ErrorStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset));
		}
		template<size_t offset = 0, typename T, T value>
		T ReturnStub() {
			LOG_TRACE("{} ReturnStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
			return value;
		}

		
		class BO6FFHandler;

		struct Asset {
			union {
				T10HashAssetType hash;
				T10AssetType id;
			} type;
			void* handle;
		};

		struct AssetList {
			uint32_t stringsCount;
			bool stringsLoaded;
			const char** strings;
			uint32_t assetsCount;
			uint32_t assetsLoaded;
			uint32_t* unk10;
			uint32_t unk10_count;
			Asset* assets;
		};


		struct HashedType {
			T10HashAssetType hash;
			T10AssetType type;
			const char* name;
		};

		struct DBLoadCtx;
		struct DBLoadCtxData;
		struct DBLoadCtxVT {
			void*(__fastcall* AllocStreamPos)(DBLoadCtx* ctx, int align);
			void(__fastcall* PushStreamPos)(DBLoadCtx* ctx, int type);
			void(__fastcall* PopStreamPos)(DBLoadCtx* ctx);
			void(__fastcall* PreAssetRead)(DBLoadCtx* ctx, T10AssetType type);
			void(__fastcall* PostAssetRead)(DBLoadCtx* ctx);
		};

		struct StreamPosInfo {
			byte* pos;
			int index;
		};

		struct DBLoadCtxData {
			uint32_t streamRead[16];
			uint64_t unk40;
			uint64_t unk48;
			uint64_t unk50;
			uint64_t unk58;
			uint64_t unk60;
			uint64_t unk68;
			uint64_t unk70;
			uint64_t unk78;
			uint64_t unk80;
			uint64_t unk88;
			uint64_t unk90;
			uint64_t unk98;
			uint64_t unka0;
			uint64_t unka8;
			uint64_t unkb0;
			uint64_t unkb8;
			uint64_t unkc0;
			uint64_t unkc8;
			uint64_t unkd0;
			uint64_t unkd8;
			uint64_t unke0;
			uint64_t unke8;
			uint64_t unkf0;
			uint64_t unkf8;
			byte* streamPos;
			byte* streamPosArray[16];
			uint32_t streamPosIndex;
			uint32_t blockType;
			StreamPosInfo streamPosInfo[64];
			uint32_t g_streamPosStackIdx;
			uint32_t unk594;
			uint32_t unk598;
			uint32_t unk59c;
			uint64_t unk5a0;
			uint64_t unk5a8;
			uint64_t unk5b0;
			uint64_t unk5b8;
			uint64_t unk5c0;
			uint64_t loadData;
			uint64_t unk5d0;
			uint64_t unk5d8;
			uint64_t unk5e0;
			bool unk5e8;
			uint64_t unk5f0;
			uint32_t unk5f4;
			byte* streamPos2;
		};

		struct DBLoadCtx {
			DBLoadCtxVT* __vtb;
			DBLoadCtxData data;
		};


		union LoadStreamObjectData;

		struct LoadStreamObjectVtable {
			bool(__fastcall* LoadStream)(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len);
			void(__fastcall* copy2)(LoadStreamObjectData* that);
			void(__fastcall* copy3)(LoadStreamObjectData* that);
			void(__fastcall* f4)(LoadStreamObjectData* that);
			void(__fastcall* nulled)(LoadStreamObjectData* that);
		};

		union LoadStreamObjectData {
			struct {
				bool(*LoadStream)(DBLoadCtx* ctx, bool atStreamStart, void* ptr, size_t len);
				uint64_t unk08;
				uint64_t unk10;
				uint64_t unk18;
			} loadStream;
		};
		static_assert(sizeof(LoadStreamObjectData) == 0x20);
		struct LoadStreamObject {
			LoadStreamObjectVtable* __vtb;
			LoadStreamObjectData data;
		};

		struct AssetPoolInfo {
			uint32_t itemSize;
			uint32_t unk4;
			uint32_t unk8;
			uint32_t unkc;
			uint64_t(*GetAssetName)(void* header);
			uint64_t unk18;
			void(*SetAssetName)(void* header, uint64_t name, const char* strName);
			byte unk28;
			byte unk29;
		};


		bool LoadStreamImpl(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len);
		void ErrorStub(LoadStreamObjectData* that) {
			throw std::runtime_error(std::format("Error loadstream {}", hook::library::CodePointer{ _ReturnAddress() }));
		}
		void* AllocStreamPos(DBLoadCtx* ctx, int align);
		void PushStreamPos(DBLoadCtx* ctx, int type);
		void PopStreamPos(DBLoadCtx* ctx);
		void PreAssetRead(DBLoadCtx* ctx, T10AssetType type);
		void PostAssetRead(DBLoadCtx* ctx);


		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };
		DBLoadCtxVT dbLoadCtxVTable{ AllocStreamPos, PushStreamPos, PopStreamPos, PreAssetRead, PostAssetRead };

		struct XStringOutCTX {
			std::filesystem::path path;
			std::map<std::string, std::unordered_set<uint64_t>> map{};
		};

		constexpr T10AssetType MAX_ASSET_COUNT = (T10AssetType)512;

		struct {
			BO6FFHandler* handler{};
			void (*Load_Asset)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset) {};
			AssetPoolInfo* poolInfo{};
			uint64_t(*DB_HashScrStringName)(const char* str, size_t len, uint64_t iv) {};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			T10HashAssetType assetLoadStack[0x100];
			size_t assetLoadStackTop{};
			int streamPosStack[64]{};
			int streamPosStackIndex{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<T10HashAssetType, std::unordered_map<uint64_t, void*>> linkedAssets{};
			AssetList assets{};
			std::unordered_map<uint64_t, uint32_t> scrStringMap{};
			std::unordered_map<uint64_t, void*> streamLocations{};
			std::vector<const char*>* xstringLocs{};
			std::unique_ptr<XStringOutCTX> xstrOutGlb{};
			games::cod::asset_names::AssetNames<T10HashAssetType, T10AssetType> assetNames{ "physicslibrary", "string", bo6::PoolId };
			utils::OutFileCE* outAsset{};
		} gcx{};


		void LoadXFileData(DBLoadCtx* context, void* ptr, int64_t len) {
			LOG_TRACE("LoadXFileData({}, {}) {}", ptr, len, hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.reader->CanRead(len)) {
				hook::error::DumpStackTraceFrom();
			}
			gcx.reader->Read(ptr, len);

			if constexpr (traceData) {
				size_t s{ std::min<size_t>(len, 0x40 * 8) };
				byte* p{ (byte*)ptr };
				while (s > 0) {
					size_t ss{ std::min<size_t>(s, 0x40) };
					LOG_TRACE("{:03x} : {}", p - (byte*)ptr, utils::data::AsHex(p, ss));
					p += ss;
					s -= ss;
				}
			}
		}

		bool LoadStream(DBLoadCtx* context, bool atStreamStart, void* ptr, int64_t len) {
			LOG_TRACE("LoadStream({}, {}, {}) {}", atStreamStart, ptr, len, hook::library::CodePointer{ _ReturnAddress() });
			if (!atStreamStart || !len) {
				return true;
			}
			if (!gcx.streamPosStackIndex) throw std::runtime_error("empty streampos stack");

			int block{ gcx.streamPosStack[gcx.streamPosStackIndex - 1] };

			if (block == 11) {
				return false;
			}
			else {
				LoadXFileData(context, ptr, len);
				return true;
			}

		}

		void Load_String(DBLoadCtx* context, char** pstr) {
			char* str{ *pstr };
			do {
				gcx.reader->Read(str, 1);
			} while (*str++);
			if (gcx.xstringLocs) gcx.xstringLocs->push_back(*pstr);
			hashutils::Add(*pstr, true, true);
		}

		void Load_CustomScriptString(DBLoadCtx* context, uint32_t* pstr) {
			uint32_t low{ *pstr };
			uint32_t high;
			LoadXFileData(context, &high, sizeof(high));

			uint64_t strHash{ (uint64_t)low | ((uint64_t)high << 32) };
			if (!strHash) {
				return;
			}
			auto it{ gcx.scrStringMap.find(strHash) };
			if (it == gcx.scrStringMap.end()) {
				LOG_ERROR("Can't load scr string, bad hash value: {:x}/{}", strHash, hashutils::ExtractTmp("hash", strHash));
				*pstr = 0xFFFFFFFF;
				return;
			}
			else {
				*pstr = it->second;
			}
		}

		void* AllocStreamPos(DBLoadCtx* ctx, int align) {
			LOG_TRACE("AllocStreamPos({}) {}", align, hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.reader->CanRead(1)) {
				return gcx.reader->Ptr();
			}
			else {
				static char empty[1];
				return empty;
			}
		}

		void PushStreamPos(DBLoadCtx* ctx, int type) {
			LOG_TRACE("PushStreamPos({}) {}", type, hook::library::CodePointer{ _ReturnAddress() });
			gcx.streamPosStack[gcx.streamPosStackIndex++] = type;
		}

		void PopStreamPos(DBLoadCtx* ctx) {
			LOG_TRACE("PopStreamPos() {}", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.streamPosStackIndex) throw std::runtime_error("Can't pop empty stack");
			gcx.streamPosStackIndex--;
		}

		void PreAssetRead(DBLoadCtx* ctx, T10AssetType type) {
			LOG_TRACE("PreAssetRead({}) {}", PoolName(GetHashType(type)), hook::library::CodePointer{_ReturnAddress()});
			if (gcx.assetLoadStackTop == ARRAYSIZE(gcx.assetLoadStack)) {
				throw std::runtime_error("PreAssetRead stack overflow");
			}
			gcx.assetLoadStack[gcx.assetLoadStackTop++] = GetHashType(type);
		}

		void PostAssetRead(DBLoadCtx* ctx) {
			LOG_TRACE("PostAssetRead({}) {}", gcx.assetLoadStackTop ? PoolName(gcx.assetLoadStack[gcx.assetLoadStackTop - 1]) : "invalid", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.assetLoadStackTop) {
				throw std::runtime_error("PostAssetRead stack underflow");
			}
			gcx.assetLoadStackTop--;
		}


		bool LoadStreamImpl(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len) {
			// redirect to custom version
			return LoadStream(context, *atStreamStart, *data, *len);
		}

		int32_t GetMappedTypeStub(T10HashAssetType hash) {
			return gcx.assetNames.GetMappedType(hash)->type;
		}

		void* DB_LinkGenericXAsset(DBLoadCtx* ctx, T10AssetType type, void** handle) {
			T10HashAssetType hashType{ GetHashType(type) };
			uint64_t hash{ GetXAssetName(hashType, handle ? *handle : 0) };
			const char* name{ hashutils::ExtractTmp("hash", hash) };
			const char* poolName{ PoolName(hashType) };
			LOG_DEBUG("DB_LinkGenericXAsset({}, '{}') {}", poolName, name, hook::library::CodePointer{_ReturnAddress()});
			*(gcx.outAsset) << "\n" << poolName << ",#" << name;

			if (handle && *handle) {
				gcx.linkedAssets[hashType][hash] = *handle;
			}

			if (handle && !(gcx.opt->noAssetDump || !gcx.assetNames.ShouldHandle(type))) {

				std::unordered_map<bo6::T10HashAssetType, Worker*>& map{ GetWorkers() };
				auto it{ map.find(hashType) };
				if (it != map.end()) {
					size_t itemSize{ gcx.poolInfo[type].itemSize };
					if (it->second->assetSize != itemSize) {
						LOG_ERROR("Can't check size of asset entry {}({}): 0x{:x} != 0x{:x}", gcx.assetNames.GetTypeName(type), (int)type, it->second->assetSize, itemSize);
					}
					else {
						if constexpr (!hasRelativeLoads) {
							if (!it->second->requiresRelativeLoads) {
								it->second->Unlink(*gcx.opt, *gcx.ctx, *handle);
							}
						}
						else {
							it->second->Unlink(*gcx.opt, *gcx.ctx, *handle);
						}
					}
				}
			}

			return handle ? *handle : nullptr;
		}

		void* DB_LinkGenericXAssetEx(T10AssetType type, uint64_t name, void** handle) {
			LOG_DEBUG("DB_LinkGenericXAssetEx({}, '{}') {}", PoolName(GetHashType(type)), hashutils::ExtractTmp("hash", name), hook::library::CodePointer{_ReturnAddress()});
			if (handle) {
				return *handle;
			}
			T10HashAssetType hashType{ GetHashType(type) };
			auto it{ gcx.linkedAssets[hashType].find(name) };
			if (it != gcx.linkedAssets[hashType].end()) {
				return it->second;
			}
			// create a fake asset
			void* asset{ gcx.allocator.Alloc(gcx.poolInfo[type].itemSize) };
			gcx.poolInfo[type].SetAssetName(asset, name, hashutils::ExtractPtr(name));
			gcx.linkedAssets[hashType][name] = asset;
			return asset;
		}

		uint32_t* Unk_Align_Ret(DBLoadCtx* ctx) {
			static uint32_t data{};
			return &data;
		}

		void DB_LoadStreamOffset(DBLoadCtx* ctx, uint64_t val, void** pptr) {
			void*& ref{ gcx.streamLocations[val & StreamPointerFlag::SPF_DATA_MASK] };
			if (ref) {
				*pptr = ref;
			}
			else {
				LOG_WARNING("missing val for DB_LoadStreamOffset {:x}", val);
				*pptr = nullptr;
			}
		}

		void DB_RegisterStreamOffset(DBLoadCtx* ctx, uint64_t val, void* ptr) {
			gcx.streamLocations[val & StreamPointerFlag::SPF_DATA_MASK] = ptr;
		}

		class BO6FFHandler : public fastfile::FFHandler {
		public:
			// -w "((mp|zm)_t10|ingame|code|global).*"
			BO6FFHandler() : fastfile::FFHandler("bo6", "Black Ops 6") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true, nullptr, false, "cod_dump.exe") };
				hook::scan_container::ScanContainer scan{ lib, true };
				scan.Sync();

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(lib)) {
					throw std::runtime_error("Can't load decryption module");
				}

#ifdef CI_BUILD
				LOG_WARNING("You are using the bo6 handler, for now Black Ops 6 is still under");
				LOG_WARNING("Development and it might not work after a game update.");
				LOG_WARNING("This handler was developed for the Steam version and might not");
				LOG_WARNING("work for the BattleNet or the Gamepass version.");
#endif

				// should be done before the handleList to have the hashes loaded
				gcx.assetNames.InitMap(lib);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				games::cod::asset_names::AssetDumpFileOptions dumpOpts{};
				dumpOpts.baseFileName = "bo6";
				dumpOpts.assetHashedName = "T10HashAssetType";
				dumpOpts.assetTypeName = "T10AssetType";
				dumpOpts.assetPrefix = "T10_ASSET_";
				dumpOpts.assetHashedPrefix = "T10H_ASSET_";
				gcx.assetNames.DumpFiles(opt.m_output / "bo6" / "code", &dumpOpts);

				LoadStreamObject* loadStreamObj{ lib.ScanAny("48 8B 05 ? ? ? ? 4C 8D 4C 24 ? 48 C7 44 24 ? ? ? ? ? 4C 8D 44 24 ? 48 89 6C 24 ?", "loadStreamObj").GetRelative<int32_t, LoadStreamObject*>(3)};
				loadStreamObj->__vtb = &dbLoadStreamVTable;

				//E8 ? ? ? ? 80 3E 00 74 1E
				//lib.Redirect("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 48 8B DA 48 8B CA E8 ? ? ? ? 48 8B 0D", LoadXFileData);
				gcx.Load_Asset = scan.ScanSingle("4C 8B DC 49 89 5B ? 57 48 83 EC ? 49 8B D8 48 8B F9 84 D2 74 3C 48 8B 05 ? ? ? ? 4D 8D 4B E8 49 C7 43 ? ? ? ? ? 4D 8D 43 ? 49 89 5B E8 48 8B D1 C6 44 24 ? ? 48 8D 0D ? ? ? ? 4C 8B 10 49 8D 43 ? 49 89 43 D8 41 FF D2 84 C0 74 1C 48", "gcx.Load_Asset")
					.GetPtr<decltype(gcx.Load_Asset)>();
				gcx.DB_HashScrStringName = scan.ScanSingle("48 89 5C 24 ? 57 48 83 EC ? 49 8B F8 4C 8B DA 48 8B D9 48", "gcx.DB_HashScrStringName")
					.GetPtr<decltype(gcx.DB_HashScrStringName)>();

				gcx.poolInfo = (AssetPoolInfo*)(scan.ScanSingle("40 53 48 63 C9 48 8D 1D ? ? ? ?", "poolInfo")
					.GetRelative<int32_t>(8) - offsetof(AssetPoolInfo, SetAssetName));

				scan.ignoreMissing = true;

				auto Red = [](void* from, void* to) {
					if (from) {
						hook::memory::RedirectJmp(from, to);
					}
				};

				Red(scan.ScanSingle("40 56 41 56 48 83 EC ? 48 8B 15", "GetMappedTypeStub").location, GetMappedTypeStub);

				Red(scan.ScanSingle("48 89 5C 24 ? 57 48 83 EC ? 49 8B F9 4D 8B C8 48 8B D9", "LoadStream").location, LoadStream);
				Red(scan.ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 2A 48 8B F2", "Load_String").location, Load_String);
				Red(scan.ScanSingle("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 32 41", "Load_StringName").location, Load_String); // str
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B D8 8B EA", "DB_LinkGenericXAsset").location, DB_LinkGenericXAsset);
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B E8 48 8B DA 8B", "DB_LinkGenericXAssetEx").location, DB_LinkGenericXAssetEx);
				Red(scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B FA 41 B8", "Load_CustomScriptString").location, Load_CustomScriptString);

				// Stream delta, todo
				Red(scan.ScanSingle("4C 8B DC 48 83 EC ?? 8B 05 ?? ?? ?? ?? 4C 8B C1 85 C0 0F 84 1B", "EmptyStub<0>").location, EmptyStub<0>); // 2DD6730
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 56 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 48 8B DA", "DB_RegisterStreamOffset").location, DB_RegisterStreamOffset); //2E24F20
				Red(scan.ScanSingle("4C 8B DC 48 83 EC ?? 8B 05 ?? ?? ?? ?? 4C 8B C1 85 C0 0F 84 33", "EmptyStub<2>").location, EmptyStub<2>); // 2DD63E0
				Red(scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 4C 8B CA", "DB_LoadStreamOffset").location, DB_LoadStreamOffset); // 2E25100

				// idk
				Red(scan.ScanSingle("8B 81 ?? ?? ?? ?? 48 8D 14 40 83", "ReturnStub<4, bool, false>").location, ReturnStub<4, bool, false>);
				Red(scan.ScanSingle("C5 FB 10 02 44", "EmptyStub<5>").location, EmptyStub<5>); // 2DE3F00
				Red(scan.ScanSingle("8B 81 ?? ?? ?? ?? 48 8D 04 40 48", "Unk_Align_Ret").location, Unk_Align_Ret); // 2DE3CC0

				// remove
				Red(scan.ScanSingle("40 53 48 83 EC ?? 41 8B 40 ?? 49", "EmptyStub<7>").location, EmptyStub<7>); // image
				Red(scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 49 8B D8 48 8B FA B9", "EmptyStub<8>").location, EmptyStub<8>);
				Red(scan.ScanSingle("48 89 5C 24 ? 55 48 8D 6C 24 A9 48 81 EC ? ? ? ? 41", "EmptyStub<9>").location, EmptyStub<9>);
				Red(scan.ScanSingle("40 53 48 83 EC ?? 8B 42 ?? 49", "EmptyStub<10>").location, EmptyStub<10>);
				Red(scan.ScanSingle("48 8B 05 ?? ?? ?? ?? 0F B7 80", "EmptyStub<11>").location, EmptyStub<11>); // sound
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 41 0F B7 D8 0F", "EmptyStub<12>").location, EmptyStub<12>); // sound
				Red(scan.ScanSingle("40 53 48 83 EC ?? 81 61", "EmptyStub<13>").location, EmptyStub<13>); // model
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 41 56 41 57 48 83 EC ?? 0F B6 F2", "EmptyStub<14>").location, EmptyStub<14>); // streaminginfo
				Red(scan.ScanSingle("48 83 EC ?? E8 ?? ?? ?? ?? 83 F8 FF 75", "EmptyStub<15>").location, EmptyStub<15>); // computeshaders
				Red(scan.ScanSingle("48 89 5C 24 ? 55 56 57 41 57", "EmptyStub<16>").location, EmptyStub<16>); // computeshaders
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8 48 89 03 E8 ?? ?? ?? ?? 48 8B", "EmptyStub<17>").location, EmptyStub<17>); // computeshaders, TODO: better
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B D9 E8 ?? ?? ?? ?? 48 89 43 ?? 48 8B", "EmptyStub<18>").location, EmptyStub<18>); // libshared
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 89 03 E8 ?? ?? ?? ?? E8", "EmptyStub<19>").location, EmptyStub<19>); // libshared, TODO: better
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 4C 24 ?? 56 57 41 54 41 56 41 57 48 83 EC ?? 45 33", "EmptyStub<20>").location, EmptyStub<20>); // dlogschema
				Red(scan.ScanSingle("49 8B C0 4C 8B 02", "EmptyStub<7>").location, EmptyStub<21>); // model


				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}

				for (auto& [hashType, worker] : GetWorkers()) {
					T10AssetType type{ GetExePoolId(hashType) };
					if (type == gcx.assetNames.InvalidId()) {
						LOG_ERROR("type {} was removed", PoolName(hashType));
					}
					else {
						size_t trueLen{ gcx.poolInfo[type].itemSize };
						if (worker->assetSize != trueLen) {
							LOG_WARNING("type {} doesn't have the expected size: acts:0x{:x} != exe:0x{:x}", PoolName(hashType), worker->assetSize, trueLen);
						}
					}
				}

				if (opt.dumpXStrings) {
					std::filesystem::path path{ opt.m_output / "bo6" / "xstrings_all.wni" };
					gcx.xstrOutGlb = std::make_unique<XStringOutCTX>(path);
				}
			}
			void Cleanup() override {
				if (gcx.xstrOutGlb) {
					LOG_INFO("Dump xstrings to {}", gcx.xstrOutGlb->path.string());
					if (compatibility::scobalula::wnigen::CompressWNIFile(gcx.xstrOutGlb->map, gcx.xstrOutGlb->path)) {
						LOG_ERROR("Failed to create wni file");
					}
					gcx.xstrOutGlb = nullptr;
				}
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				gcx.streamLocations.clear();

				if (!reader.CanRead(sizeof(gcx.assets))) {
					LOG_WARNING("empty fastfile, ignored");
					return;
				}

				reader.Read(&gcx.assets, sizeof(gcx.assets));

				LOG_INFO("assets: {}, strings: {}, unk: {}", gcx.assets.assetsCount, gcx.assets.stringsCount, gcx.assets.unk10_count);


				gcx.opt->assetNames.clear();

				gcx.allocator.FreeAll();
				gcx.linkedAssets.clear();

				gcx.xstringLocs = nullptr;
				std::vector<const char*> xstringLocs{};
				if (opt.dumpXStrings) {
					gcx.xstringLocs = &xstringLocs;
				}

				std::string fftype{ ctx.ffname };
				size_t fdd{ fftype.find('_') };
				if (fdd != std::string::npos) {
					fftype.resize(fdd);
				}

				std::filesystem::path outStrings{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "strings" / fftype / std::format("{}.txt", ctx.ffname) };

				{
					gcx.scrStringMap.clear();
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE stringsOs{ outStrings };
					gcx.assets.strings = reader.ReadPtr<const char*>(gcx.assets.stringsCount);
					for (size_t i = 0; i < gcx.assets.stringsCount; i++) {
						const char* str;
						uint64_t stroff{ (uint64_t)gcx.assets.strings[i] };
						if (stroff) {
							if (stroff & StreamPointerFlag::SPF_NEXT) {
								str = acts::decryptutils::DecryptString(reader.ReadString());
								if (stroff & StreamPointerFlag::SPF_CREATE_REF) {
									DB_RegisterStreamOffset(nullptr, stroff, (void*)str);
									//LOG_INFO("store offset {:x} -> {}", stroff, str);
								}
								hashutils::Add(str, true, true);
								if (gcx.xstringLocs) gcx.xstringLocs->push_back(str);
							}
							else {
								DB_LoadStreamOffset(nullptr, stroff, (void**)&str);
							}
						}
						else {
							str = "";
						}
						gcx.assets.strings[i] = str;
						uint64_t hash{ gcx.DB_HashScrStringName(str, std::strlen(str), hash::IV_DEFAULT) };
						gcx.scrStringMap[hash] = (uint32_t)i;
						stringsOs << std::dec << std::setfill(' ') << std::setw(utils::Log<10>(gcx.assets.stringsCount) + 1) << i << "\t" << std::setw(16) << std::setfill('0') << std::hex << hash << "\t";
						if (stroff) {
							if (stroff & StreamPointerFlag::SPF_NEXT) {
								if (stroff & StreamPointerFlag::SPF_CREATE_REF) {
									stringsOs << "[ref:" << std::setw(16) << std::setfill('0') << std::hex << (stroff & StreamPointerFlag::SPF_DATA_MASK) << "]";
								}
								else {
									stringsOs << "[next]";
								}
							}
							else {
								stringsOs << "[off:" << std::setw(16) << std::setfill('0') << std::hex << (stroff & StreamPointerFlag::SPF_DATA_MASK) << "]";
							}
						}
						else {
							stringsOs << "[null]";
						}
						stringsOs << "\t" << str << "\n";
					}
				}

				LOG_INFO("String dump into {}", outStrings.string());
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

				gcx.assets.unk10 = reader.ReadPtr<uint32_t>(gcx.assets.unk10_count);
				// idk
				//std::filesystem::path outUnk{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "unk" / std::format("{}.csv", ctx.ffname) };
				//{
				//	std::filesystem::create_directories(outUnk.parent_path());
				//	utils::OutFileCE stringsOs{ outUnk };
				//	for (size_t i = 0; i < gcx.assets.unk10_count; i++) {
				//		stringsOs << i << ",0x" << std::hex << gcx.assets.unk10[i] << "," << hashutils::ExtractTmp("hash", gcx.assets.unk10[i]) << "\n";
				//	}
				//}
				//
				//LOG_INFO("unk dump into {}", outUnk.string());

				if (!gcx.assets.assetsCount) {
					LOG_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "assets" / fftype / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					gcx.outAsset = &assetsOs;
					assetsOs << "type,name";
					gcx.assets.assets = reader.ReadPtr<Asset>(gcx.assets.assetsCount);

					DBLoadCtx loadCtx{};
					DBLoadCtxVT* vt = &dbLoadCtxVTable;

					for (auto& [k, v] : GetWorkers()) {
						if constexpr (!hasRelativeLoads) {
							if (v->requiresRelativeLoads) continue;
						}
						v->PreXFileLoading(*gcx.opt, ctx);
					}

					//if (!gcx.handleList.Empty()) {
					//	// check that we match at least one required type
					//	bool anySee{};
					//	for (size_t i = 0; i < gcx.assets.assetsCount; i++) {
					//		Asset* asset{ gcx.assets.assets + i };
					//
					//		if (gcx.handleList[gcx.GetMappedType(asset->type)->type]) {
					//			anySee = true;
					//			break;
					//		}
					//	}
					//
					//	if (!anySee) return;
					//}

					bool err{};
					size_t assetId;
					try {
						for (assetId = 0; assetId < gcx.assets.assetsCount; assetId++) {
							Asset* asset{ gcx.assets.assets + assetId };

							const auto* type{ gcx.assetNames.GetMappedType(asset->type.hash) };

							LOG_DEBUG("load #{} -> {}({})", assetId, type->name, (int)type->type);
							gcx.Load_Asset((DBLoadCtx*)&vt, false, asset);
						}

					} catch (std::runtime_error& e) {
						LOG_ERROR("can't dump ff {}", e.what());
					}
					if (gcx.assetLoadStackTop && assetId != gcx.assets.assetsCount) {
						LOG_ERROR("Asset load stack ({}) - asset:#{}/{}", gcx.assetLoadStackTop, assetId + 1, gcx.assets.assetsCount);
						for (size_t i = gcx.assetLoadStackTop; i; i--) {
							LOG_ERROR("- {} - {}", i, PoolName(gcx.assetLoadStack[i - 1]));
						}
					}

					for (auto& [k, v] : GetWorkers()) {
						if constexpr (!hasRelativeLoads) {
							if (v->requiresRelativeLoads) continue;
						}
						v->PostXFileLoading(*gcx.opt, ctx);
					}
				}
				LOG_INFO("Asset names dump into {}", outAssets.string());
				if (gcx.xstringLocs) {
					std::filesystem::path ostr{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "xstrings" / fftype / std::format("{}.txt", ctx.ffname) };
					std::filesystem::create_directories(ostr.parent_path());
					utils::OutFileCE sos{ ostr };
					for (const char* s : *gcx.xstringLocs) {
						if (!s) continue;
						byte b;
						if (!hook::memory::ReadMemorySafe((void*)s, &b, 1)) {
							LOG_ERROR("Can't read string");
							continue;
						}

						sos
							<< utils::FormattedString{ s }
							<< "\n"
							;

						if (gcx.xstrOutGlb) {
							std::string lcs{ s };
							utils::LowerCase(lcs.data());
							std::unordered_set<uint64_t>& h{ gcx.xstrOutGlb->map[lcs] };
							// add our hashes
							h.insert(hash::Hash64A(s));
							h.insert(hash::HashIWAsset(s));
							h.insert(hash::HashT10OmnVar(s));
							h.insert(hash::HashT10Scr(s));
							h.insert(hash::HashT10ScrSP(s));
						}
					}
					LOG_INFO("XStrings names dump into {}", ostr.string());
					gcx.xstringLocs = nullptr;
				}

				LOG_DEBUG("done reading {}", ctx.ffname);
			}

		};

		
		utils::ArrayAdder<BO6FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}

	std::unordered_map<bo6::T10HashAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<bo6::T10HashAssetType, Worker*> workers{};
		return workers;
	}
	std::vector<const char*>* GetXStrings() {
		return gcx.xstringLocs;
	}

	const char* GetPoolName(uint32_t hash) {
		return gcx.assetNames.GetMappedType((T10HashAssetType)hash)->name;
	}

	const char* GetScrString(ScrString_t id) {
		if ((int)id >= gcx.assets.stringsCount) {
			LOG_WARNING("Can't get scr string: {} >= {}", id.id, gcx.assets.stringsCount);
			return utils::va("<invalid:0x%x>", id);
		}
		const char* c{ gcx.assets.strings[id] };
		if (!c) return "";
		return c;
	}

	T10HashAssetType GetHashType(T10AssetType type) {
		return gcx.assetNames.GetHashType(type);
	}

	T10AssetType GetExePoolId(const char* name) {
		return gcx.assetNames.GetExePoolId(name);
	}
	T10AssetType GetExePoolId(T10HashAssetType name) {
		return gcx.assetNames.GetExePoolId(name);
	}

	uint64_t GetXAssetName(T10HashAssetType htype, void* handle) {
		T10AssetType type{ GetExePoolId(htype) };
		if (type == gcx.assetNames.InvalidId()) {
			throw std::runtime_error(std::format("INVALID HASH TYPE ID {}", PoolName(htype)));
		}
		return gcx.poolInfo[type].GetAssetName(handle);
	}
}