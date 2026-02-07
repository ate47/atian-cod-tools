#include <includes.hpp>
#include <scans_dir.hpp>
#include <games/cod/asset_names.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <hook/scan_container.hpp>
#include <tools/fastfile/fastfile_names_store.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>
#include <tools/fastfile/handlers/handler_iw_common.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>


namespace fastfile::handlers::bo7 {
	using namespace tool::bo7;
	constexpr bool traceData = false;

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

		struct Asset {
			union {
				HandlerHashedAssetType hash;
				HandlerAssetType id;
			} type;
			void* handle;
		};

		struct XBlock {
			byte* data;
			uint64_t size;
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

		enum XFileBlock : int {
			XFILE_BLOCK_MEMMAPPED = 11,
			XFILE_BLOCK_VIRTUAL = 15,
			XFILE_BLOCK_COUNT = 16,
		};

		struct DBLoadCtx;
		struct DBLoadCtxVT {
			void (*unk0)(DBLoadCtx* ctx, byte a1);
			void* (__fastcall* AllocStreamPos)(DBLoadCtx* ctx, int align);
			void(__fastcall* PushStreamPos)(DBLoadCtx* ctx, XFileBlock type);
			void(__fastcall* PopStreamPos)(DBLoadCtx* ctx);
			void(__fastcall* PreAssetRead)(DBLoadCtx* ctx, HandlerAssetType type);
			void(__fastcall* PostAssetRead)(DBLoadCtx* ctx);
		};
		struct HashRef {
			HashRef* next;
			uint64_t name;
			void* value;
		};

		struct DBLoadCtxRewind {
			uint64_t unk0;
			uint64_t unk8;
			HashRef* freeHead;
			uint64_t unk18;
			uint64_t unk20;
			uint64_t unk28;
			uint64_t unk30;
			HashRef** hashedRefs;
			uint64_t numBlocks;
			uint64_t numRefs;
		};

		struct XZoneTemporaryLoadData {
			byte __pad[0x14ce8];
			XBlock xblocks[16];
			uint64_t unk14dc8;
			DBLoadCtxRewind* rewind;
			uint64_t unk14dd8;
			uint64_t unk14de0;
			uint64_t unk14de8;
		};
		static_assert(sizeof(XZoneTemporaryLoadData) == 0x14e10);


		constexpr size_t numHashBlocks = 0x100;
		constexpr size_t numHashBlocksAlloc = numHashBlocks * 0x400;

		struct DBLoadCtxData {
			byte __pad[392];
			XFileBlock streamPosIndex;
			uint32_t unk194;
			byte __pad198[1080];
			XZoneTemporaryLoadData* tempData;
			byte __pad5d8[56];
			void* unk608;
			byte unk610;
			byte __safepad0[0x100];
			XZoneTemporaryLoadData _allocTempData{};
			byte __safepad1[0x100];
			DBLoadCtxRewind _allocRewind{};
			byte __safepad2[0x100];
			HashRef* _allocHashRefsStarts[numHashBlocks];
			HashRef _allocHashRefs[numHashBlocksAlloc];
			byte __safepad3[0x100];
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
		void DBLoadCtxUnk0(DBLoadCtx* ctx, byte a1) {
			throw std::runtime_error(std::format("DBLoadCtxUnk0 {}", hook::library::CodePointer{ _ReturnAddress() }));
		}
		void* AllocStreamPos(DBLoadCtx* ctx, int align);
		void PushStreamPos(DBLoadCtx* ctx, XFileBlock type);
		void PopStreamPos(DBLoadCtx* ctx);
		void PreAssetRead(DBLoadCtx* ctx, HandlerAssetType type);
		void PostAssetRead(DBLoadCtx* ctx);


		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };
		DBLoadCtxVT dbLoadCtxVTable{ DBLoadCtxUnk0, AllocStreamPos, PushStreamPos, PopStreamPos, PreAssetRead, PostAssetRead };

		struct XStringOutCTX {
			std::filesystem::path path;
			std::map<std::string, std::unordered_set<uint64_t>> map{};
		};

		struct {
			void (*DB_LoadStreamOffset)(DBLoadCtx* ctx, uint64_t val, void** pptr);
			void (*DB_RegisterStreamOffset)(DBLoadCtx* ctx, uint64_t val, void* ptr);
			void (*Load_Asset)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset) {};
			AssetPoolInfo* poolInfo{};
			uint64_t(*DB_HashScrStringName)(const char* str, size_t len, uint64_t iv) {};
			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};

			size_t lastBlockCount{};
			HandlerHashedAssetType assetLoadStack[0x100];
			size_t assetLoadStackTop{};
			XFileBlock streamPosStack[64]{};
			int streamPosStackIndex{};
			size_t loaded{};
			std::unique_ptr<DBLoadCtx> loadCtx{};
			core::memory_allocator::MemoryAllocator allocator{};

			std::unordered_map<HandlerHashedAssetType, std::unordered_map<uint64_t, void*>> linkedAssets{};
			AssetList assets{};
			std::unordered_map<uint64_t, uint32_t> scrStringMap{};
			std::vector<const char*>* xstringLocs{};
			std::unique_ptr<XStringOutCTX> xstrOutGlb{};
			games::cod::asset_names::AssetNames<HandlerHashedAssetType, HandlerAssetType> assetNames{ "physicssfxeventasset", "string", bo7::PoolId };
			fastfile::names_store::NamesStore namesStore{ [](const char* name) -> uint64_t { return hash::HashIWAsset(name); } };
			utils::OutFileCE* outAsset{};
		} gcx{};

		void AddBootsLimitAssetNames() {
			for (size_t i = 0; i < gcx.assetNames.TypesCount(); i++) {
				const char* n{ gcx.assetNames.GetTypeName((HandlerAssetType)i) };
				if (!n) continue;
				uint64_t h{ hash::Hash64A(n, 0xDA800D9CA00C6B4FULL, 0x100000001B3) };
				n = hashutils::AddPrecomputed(h, n, true);
			}
		}

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

		bool LoadStreamTA(DBLoadCtx* context, bool atStreamStart, void* ptr, int64_t len) {
			LOG_TRACE("LoadStreamTA({}, {}, {}) {}", atStreamStart, ptr, len, hook::library::CodePointer{ _ReturnAddress() });
			if (!atStreamStart || !len) {
				return true;
			}
			if (!gcx.streamPosStackIndex) throw std::runtime_error("empty streampos stack");

			XFileBlock block{ gcx.streamPosStack[gcx.streamPosStackIndex - 1] };

			if (block == XFILE_BLOCK_MEMMAPPED) {
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
			char* decr{ acts::decryptutils::DecryptString(*pstr) };
			std::memcpy(*pstr, decr, std::strlen(decr) + 1);
			if (gcx.xstringLocs) gcx.xstringLocs->push_back(*pstr);
			if (**pstr == '#') {
				// used for localize dev entries
				hashutils::Add(&(*pstr)[1], true, true);
			}
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

		void PushStreamPos(DBLoadCtx* ctx, XFileBlock type) {
			LOG_TRACE("PushStreamPos({}) {}", (int)type, hook::library::CodePointer{ _ReturnAddress() });
			gcx.streamPosStack[gcx.streamPosStackIndex++] = type;
		}

		void PopStreamPos(DBLoadCtx* ctx) {
			LOG_TRACE("PopStreamPos() {}", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.streamPosStackIndex) throw std::runtime_error("Can't pop empty stack");
			gcx.streamPosStackIndex--;
		}

		void PreAssetRead(DBLoadCtx* ctx, HandlerAssetType type) {
			LOG_TRACE("PreAssetRead({}) {}", gcx.assetNames.GetTypeName(type), hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.assetLoadStackTop == ARRAYSIZE(gcx.assetLoadStack)) {
				throw std::runtime_error("PreAssetRead stack overflow");
			}
			gcx.assetLoadStack[gcx.assetLoadStackTop++] = gcx.assetNames.GetHashType(type);
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
			return LoadStreamTA(context, *atStreamStart, *data, *len);
		}

		HandlerAssetType GetMappedTypeStub(HandlerHashedAssetType hash) {
			return gcx.assetNames.GetMappedType(hash)->type;
		}

		void* DB_AddAsset(DBLoadCtx* ctx, HandlerAssetType type, void** handle) {
			HandlerHashedAssetType hashType{ gcx.assetNames.GetHashType(type) };
			uint64_t hash{ GetXAssetName(hashType, handle ? *handle : 0) };
			const char* name{ hashutils::ExtractTmp("hash", hash) };
			const char* poolName{ PoolName(hashType) };
			LOG_DEBUG("DB_AddAsset({}, '{}') {}", poolName, name, hook::library::CodePointer{ _ReturnAddress() });
			*(gcx.outAsset) << "\n" << poolName << ",#" << name;

			if (handle && *handle) {
				gcx.linkedAssets[hashType][hash] = *handle;
			}

			if (handle && !(gcx.opt->noAssetDump || !gcx.assetNames.ShouldHandle(type) || !gcx.namesStore.Contains(hash, true))) {

				std::unordered_map<HandlerHashedAssetType, Worker*>& map{ GetWorkers() };
				auto it{ map.find(hashType) };
				if (it != map.end()) {
					size_t itemSize{ gcx.poolInfo[type].itemSize };
					if (it->second->assetSize != itemSize) {
						LOG_ERROR("Can't check size of asset entry {}({}): 0x{:x} != 0x{:x}", gcx.assetNames.GetTypeName(type), (int)type, it->second->assetSize, itemSize);
					}
					else if (!it->second->graphic || gcx.opt->graphic) {
						it->second->Unlink(*gcx.opt, *gcx.ctx, *handle);
					}
				}
			}

			return handle ? *handle : nullptr;
		}

		void* DB_AddAssetRef(HandlerAssetType type, uint64_t name, void* strName) {
			LOG_DEBUG("DB_AddAssetRef({}, '{}') {}", gcx.assetNames.GetTypeName(type), strName ? strName : hashutils::ExtractTmp("hash", name), hook::library::CodePointer{ _ReturnAddress() });
			HandlerHashedAssetType hashType{ gcx.assetNames.GetHashType(type) };
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


		size_t GetRva(void* loc) {
			hook::library::Library library{ hook::library::GetLibraryInfo(loc) };
			if (!library) {
				return 0;
			}
			return (size_t)((byte*)loc - (byte*)*library);
		}

		class FFHandlerImpl : public fastfile::FFHandler {
		public:
			// -w "((mp|zm)_t10|ingame|code|global).*"
			FFHandlerImpl() : fastfile::FFHandler(handlerId, handlerName) {
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, gameExe, gameDumpId) };

				hook::library::ScanLogger& logger{ mod.GetScanLogger() };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				acts::scan_dir::ScanDir scanDir{ gameDumpId, scan };

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(mod)) {
					throw std::runtime_error("Can't load decryption module");
				}

#ifdef CI_BUILD
				LOG_WARNING("You are using the {} handler, for now {} is still under", handlerId, handlerName);
				LOG_WARNING("Development and it might not work after a game update.");
				LOG_WARNING("This handler was developed for the Steam version and might not");
				LOG_WARNING("work for the BattleNet or the Gamepass version.");
#endif

				// should be done before the handleList to have the hashes loaded
				gcx.assetNames.InitMap(mod);
				AddBootsLimitAssetNames();
				games::cod::asset_names::AssetDumpFileOptions dumpOpts{};
				dumpOpts.baseFileName = gameDumpId;
				dumpOpts.assetHashedName = hppHashedName;
				dumpOpts.assetTypeName = hppName;
				dumpOpts.assetPrefix = hppPrefix;
				dumpOpts.assetHashedPrefix = hppHashedPrefix;
				gcx.assetNames.DumpFiles(opt.m_output / gamePath / "code", &dumpOpts);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				gcx.namesStore.LoadConfig(opt.assets);

				gcx.loadCtx = std::make_unique<DBLoadCtx>();
				gcx.loadCtx->__vtb = &dbLoadCtxVTable;

				scan.ignoreMissing = true;

				LoadStreamObject* loadStreamObj{ scan.ScanAny("4C 8B DC 49 89 5B ? 57 48 83 EC ? 49 8B D8 48 8B F9 84 D2 74 3C 48 8B 05 ? ? ? ? 4D 8D 4B E8 49 C7 43 ? ? ? ? ? 4D 8D 43 ? 49 89 5B E8 48 8B D1 C6 44 24 ? ? 48 8D 0D ? ? ? ? 4C 8B 10 49 8D 43 ? 49 89 43 D8 41 FF D2 84 C0 74 1C 48", "loadStreamObj").GetRelative<int32_t, LoadStreamObject*>(25) };
				if (loadStreamObj) {
					loadStreamObj->__vtb = &dbLoadStreamVTable;
				}

				gcx.Load_Asset = scan.ScanSingle("4C 8B DC 49 89 5B ? 57 48 83 EC ? 49 8B D8 48 8B F9 84 D2 74 3C 48 8B 05 ? ? ? ? 4D 8D 4B E8 49 C7 43 ? ? ? ? ? 4D 8D 43 ? 49 89 5B E8 48 8B D1 C6 44 24 ? ? 48 8D 0D ? ? ? ? 4C 8B 10 49 8D 43 ? 49 89 43 D8 41 FF D2 84 C0 74 1C 48", "Load_Asset")
					.GetPtr<decltype(gcx.Load_Asset)>();
				gcx.DB_HashScrStringName = scan.ScanSingle("48 89 5C 24 ? 57 48 83 EC ? 49 8B F8 4C 8B DA 48 8B D9 48", "DB_HashScrStringName")
					.GetPtr<decltype(gcx.DB_HashScrStringName)>();

				gcx.poolInfo = (AssetPoolInfo*)(scan.ScanSingle("4C 8D 1D ? ? ? ? 49 8B C0 49 8B D1", "poolInfo")
					.GetRelative<int32_t>(3) - offsetof(AssetPoolInfo, SetAssetName));

				LOG_TRACE("poolInfo = {}", hook::library::CodePointer{ gcx.poolInfo });
				LOG_TRACE("Load_Asset = {}", hook::library::CodePointer{ gcx.Load_Asset });
				LOG_TRACE("DB_HashScrStringName = {}", hook::library::CodePointer{ gcx.DB_HashScrStringName });
				LOG_TRACE("loadStreamObj = {}", hook::library::CodePointer{ loadStreamObj });

				auto Red = [](void* from, void* to) {
					if (from) {
						hook::memory::RedirectJmp(from, to);
					}
				};

				Red(scan.ScanSingle("40 56 41 56 48 83 EC ? 48 8B 15", "GetMappedTypeStub").location, GetMappedTypeStub);

				Red(scan.ScanSingle("48 89 5C 24 10 57 48 83 EC 20 49 8B D9", "LoadStreamTA").location, LoadStreamTA);
				Red(scan.ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 2A 48 8B F2", "Load_String").location, Load_String);
				Red(scan.ScanSingle("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 32 41", "Load_StringName").location, Load_String); // str
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B D8 8B F2 48 8B F9 E8 ?? ?? ?? ?? 4C", "DB_AddAsset").location, DB_AddAsset);
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B E8 48 8B DA 8B", "DB_AddAssetRef").location, DB_AddAssetRef);
				Red(scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B FA 41 B8", "Load_CustomScriptString").location, Load_CustomScriptString);

				// Stream delta, todo
				Red(scan.ScanSingle("4C 8B DC 56 48 83 EC ?? 4C", "EmptyStub<0>").location, EmptyStub<0>); // 2DD6730
				gcx.DB_RegisterStreamOffset = scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 56 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 48 8B DA", "DB_RegisterStreamOffset")
					.GetPtr<decltype(gcx.DB_RegisterStreamOffset)>();
				gcx.DB_LoadStreamOffset = scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 4C 8B CA", "DB_LoadStreamOffset")
					.GetPtr<decltype(gcx.DB_LoadStreamOffset)>();
				Red(scan.ScanSingle("40 56 48 83 EC ?? 4C 8B D2", "EmptyStub<2>").location, EmptyStub<2>); // 2DD63E0

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
				//Red(scan.ScanSingle("48 89 5C 24 ? 55 56 57 41 57", "EmptyStub<16>").location, EmptyStub<16>); // computeshaders
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8 48 89 03 E8 ?? ?? ?? ?? 48 8B", "EmptyStub<17>").location, EmptyStub<17>); // computeshaders, TODO: better
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B D9 E8 ?? ?? ?? ?? 48 89 43 ?? 48 8B", "EmptyStub<18>").location, EmptyStub<18>); // libshared
				Red(scan.ScanSingle("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 89 03 E8 ?? ?? ?? ?? E8", "EmptyStub<19>").location, EmptyStub<19>); // libshared, TODO: better
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 4C 24 ?? 56 57 41 54 41 56 41 57 48 83 EC ?? 45 33", "EmptyStub<20>").location, EmptyStub<20>); // dlogschema
				Red(scan.ScanSingle("49 8B C0 4C 8B 02", "EmptyStub<7>").location, EmptyStub<21>); // model

				void (*ErrorStub)(uint32_t code, int a2, const char* msg, int a4) {
					[](uint32_t code, int a2, const char* msg, int a4) -> void {

						throw std::runtime_error(std::format("Error 0x{:x} : {}", code, msg ? msg : "no message"));
					}
				};

				Red(scan.ScanSingle("40 55 53 57 41 54 41 55 41 56 48 8D AC 24 58", "Error").location, ErrorStub);

				hook::library::ScanLoggerLogsOpt logsOpt{};
				logsOpt.base = gameDumpId;

				auto SaveScan = [&mod, &scan, &logger, &opt, &logsOpt] {
					scan.Save();
					logger.WriteLogs(opt.m_output / gamePath / "code", &logsOpt);
				};

				SaveScan(); // we might crash before the end of the context

				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}

				if (!opt.noWorkerPreload) {
					for (auto& [hashType, worker] : GetWorkers()) {
						worker->PreLoadWorker(nullptr);
					}
				}

				SaveScan();

				for (auto& [hashType, worker] : GetWorkers()) {
					HandlerAssetType type{ gcx.assetNames.GetExePoolId(hashType) };
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
					std::filesystem::path path{ opt.m_output / gamePath / "xstrings_all.wni" };
					gcx.xstrOutGlb = std::make_unique<XStringOutCTX>(path);
				}
			}
			void Cleanup() override {
				gcx.namesStore.WarnMissings();
				if (gcx.xstrOutGlb) {
					LOG_OPT_INFO("Dump xstrings to {}", gcx.xstrOutGlb->path.string());
					if (compatibility::scobalula::wnigen::CompressWNIFile(gcx.xstrOutGlb->map, gcx.xstrOutGlb->path)) {
						LOG_ERROR("Failed to create wni file");
					}
					gcx.xstrOutGlb = nullptr;
				}
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;

				if (!reader.CanRead(sizeof(gcx.assets))) {
					LOG_DEBUG("empty fastfile, ignored");
					return;
				}

				// setup load ctx data
				DBLoadCtx* loadCtx{ gcx.loadCtx.get() };
				std::memset(&loadCtx->data, 0, sizeof(loadCtx->data));
				XZoneTemporaryLoadData* tempData{ &loadCtx->data._allocTempData };
				loadCtx->data.tempData = tempData;
				DBLoadCtxRewind* rewind{ &loadCtx->data._allocRewind };
				loadCtx->data.tempData->rewind = rewind;
				rewind->numBlocks = numHashBlocks;
				rewind->hashedRefs = loadCtx->data._allocHashRefsStarts;

				// setup the hashmap, we need to do it because their compiler decided to inline some shits
				for (size_t i = 0; i < numHashBlocks; i++) {
					rewind->hashedRefs[i] = (HashRef*)&rewind->hashedRefs[i];
				}

				HashRef* freeHead{ loadCtx->data._allocHashRefs };
				rewind->freeHead = freeHead;
				rewind->numRefs = 0;
				for (size_t i = 1; i < numHashBlocksAlloc; i++) {
					freeHead[i - 1].next = &freeHead[i];
				}


				reader.Read(&gcx.assets, sizeof(gcx.assets));

				LOG_DEBUG("assets: {}, strings: {}, unk: {}", gcx.assets.assetsCount, gcx.assets.stringsCount, gcx.assets.unk10_count);


				gcx.opt->assetNames.clear();

				gcx.allocator.FreeAll();
				gcx.linkedAssets.clear();

				gcx.xstringLocs = nullptr;
				std::vector<const char*> xstringLocs{};
				if (opt.dumpXStrings) {
					gcx.xstringLocs = &xstringLocs;
				}

				if (!opt.noWorkerPreload) {
					for (auto& [hashType, worker] : GetWorkers()) {
						worker->PreLoadWorker(&ctx);
					}
				}

				const char* fftype{ ctx.GetFFType() };
				std::filesystem::path outStrings{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "strings" / fftype / std::format("{}.txt", ctx.ffname) };

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
									gcx.DB_RegisterStreamOffset(loadCtx, stroff, (void*)str);
									//LOG_OPT_INFO("store offset {:x} -> {}", stroff, str);
								}
								hashutils::Add(str, true, true);
								if (gcx.xstringLocs) gcx.xstringLocs->push_back(str);
							}
							else {
								gcx.DB_LoadStreamOffset(loadCtx, stroff, (void**)&str);
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

				LOG_OPT_INFO("String dump into {}", outStrings.string());
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

				// finally moved to a runtime memory block
				// gcx.assets.unk10 = reader.ReadPtr<uint32_t>(gcx.assets.unk10_count);

				if (!gcx.assets.assetsCount) {
					LOG_OPT_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "assets" / fftype / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					gcx.outAsset = &assetsOs;
					assetsOs << "type,name";
					gcx.assets.assets = reader.ReadPtr<Asset>(gcx.assets.assetsCount);


					for (auto& [k, v] : GetWorkers()) {
						if (!v->graphic || opt.graphic) {
							v->PreXFileLoading(*gcx.opt, ctx);
						}
					}

					size_t assetId;
					try {
						for (assetId = 0; assetId < gcx.assets.assetsCount; assetId++) {
							Asset* asset{ gcx.assets.assets + assetId };

							const auto* type{ gcx.assetNames.GetMappedType(asset->type.hash) };

							LOG_DEBUG("load #{} -> {}({})", assetId, type->name, (int)type->type);
							gcx.Load_Asset(loadCtx, false, asset);
						}

					}
					catch (std::runtime_error& e) {
						LOG_ERROR("can't dump ff {}", e.what());
					}
					if (gcx.assetLoadStackTop && assetId != gcx.assets.assetsCount) {
						LOG_ERROR("Asset load stack ({}) - asset:#{}/{}", gcx.assetLoadStackTop, assetId + 1, gcx.assets.assetsCount);
						for (size_t i = gcx.assetLoadStackTop; i; i--) {
							LOG_ERROR("- {} - {}", i, PoolName(gcx.assetLoadStack[i - 1]));
						}
					}

					for (auto& [k, v] : GetWorkers()) {
						if (!v->graphic || opt.graphic) {
							v->PostXFileLoading(*gcx.opt, ctx);
						}
					}
				}
				LOG_OPT_INFO("Asset names dump into {}", outAssets.string());
				if (gcx.xstringLocs) {
					std::filesystem::path ostr{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "xstrings" / fftype / std::format("{}.txt", ctx.ffname) };
					std::filesystem::create_directories(ostr.parent_path());
					utils::OutFileCE sos{ ostr };
					for (const char* s : *gcx.xstringLocs) {
						if (!s) continue;
						byte b;
						if (!hook::memory::ReadMemorySafe((void*)s, &b, 1)) {
							LOG_ERROR("Can't read string");
							continue;
						}

						sos << utils::FormattedStringJson{ s } << "\n";

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
					LOG_OPT_INFO("XStrings names dump into {}", ostr.string());
					gcx.xstringLocs = nullptr;
				}

				LOG_DEBUG("done reading {}", ctx.ffname);
			}

		};


		utils::ArrayAdder<FFHandlerImpl, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}

	std::unordered_map<HandlerHashedAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<HandlerHashedAssetType, Worker*> workers{};
		return workers;
	}
	std::vector<const char*>* GetXStrings() {
		return gcx.xstringLocs;
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

	uint64_t GetXAssetName(HandlerHashedAssetType htype, void* handle) {
		HandlerAssetType type{ gcx.assetNames.GetExePoolId(htype) };
		if (type == gcx.assetNames.InvalidId()) {
			throw std::runtime_error(std::format("INVALID HASH TYPE ID {}", PoolName(htype)));
		}
		return gcx.poolInfo[type].GetAssetName(handle);
	}

	games::cod::asset_names::AssetNames<HandlerHashedAssetType, HandlerAssetType>& GetAssetNames() {
		return gcx.assetNames;
	}
}