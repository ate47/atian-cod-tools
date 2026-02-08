#include <includes.hpp>
#include <games/cod/asset_names.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <hook/scan_container.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_bo6.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/fastfile/fastfile_names_store.hpp>
#include <game_data.hpp>


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

		
		class FFHandlerImpl;

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
			fastfile::names_store::NamesStore namesStore{ [](const char* name) -> uint64_t { return hash::HashIWAsset(name); } };
			utils::OutFileCE* outAsset{};
		} gcx{};

		void AddBootsLimitAssetNames() {
			for (size_t i = 0; i < gcx.assetNames.TypesCount(); i++) {
				const char* n{ gcx.assetNames.GetTypeName((T10AssetType)i) };
				if (!n) continue;
				hashutils::AddPrecomputed(hash::Hash64A(n, 0xDA800D9CA00C6B4F), n, true);
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

		void* DB_AddAsset(DBLoadCtx* ctx, T10AssetType type, void** handle) {
			T10HashAssetType hashType{ GetHashType(type) };
			uint64_t hash{ GetXAssetName(hashType, handle ? *handle : 0) };
			const char* name{ hashutils::ExtractTmp("hash", hash) };
			const char* poolName{ PoolName(hashType) };
			LOG_DEBUG("DB_AddAsset({}, '{}') {}", poolName, name, hook::library::CodePointer{_ReturnAddress()});
			*(gcx.outAsset) << "\n" << poolName << ",#" << name;

			if (handle && *handle) {
				gcx.linkedAssets[hashType][hash] = *handle;
			}

			if (handle && !(gcx.opt->noAssetDump || !gcx.assetNames.ShouldHandle(type) || !gcx.namesStore.Contains(hash, true))) {

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

		void* DB_AddAssetRef(T10AssetType type, uint64_t name, void* handle) {
			LOG_DEBUG("DB_AddAssetRef({}, '{}') {}", PoolName(GetHashType(type)), hashutils::ExtractTmp("hash", name), hook::library::CodePointer{_ReturnAddress()});
			T10HashAssetType hashType{ GetHashType(type) };
			if (handle) {
				gcx.linkedAssets[hashType][name] = handle;
				return handle;
			}
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

		class FFHandlerImpl : public fastfile::FFHandler {
		public:
			// -w "((mp|zm)_t10|ingame|code|global).*"
			FFHandlerImpl() : fastfile::FFHandler(handlerId, handlerName) {
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, gameExe, gameDumpId) };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				acts::game_data::GameData game{ gameDumpId };
				game.SetScanContainer(&scan);

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
				dumpOpts.assetHashedName = "T10HashAssetType";
				dumpOpts.assetTypeName = "T10AssetType";
				dumpOpts.assetPrefix = "T10_ASSET_";
				dumpOpts.assetHashedPrefix = "T10H_ASSET_";
				gcx.assetNames.DumpFiles(opt.m_output / gameDumpId / "code", &dumpOpts);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				gcx.namesStore.LoadConfig(opt.assets);

				LoadStreamObject* loadStreamObj{ game.GetPointer<LoadStreamObject*>("loadStreamObj") };
				if (loadStreamObj) {
					loadStreamObj->__vtb = &dbLoadStreamVTable;
				}
				scan.ignoreMissing = true;

				gcx.Load_Asset = game.GetPointer<decltype(gcx.Load_Asset)>("Load_Asset");
				gcx.DB_HashScrStringName = game.GetPointer<decltype(gcx.DB_HashScrStringName)>("DB_HashScrStringName");

				gcx.poolInfo = game.GetPointer<decltype(gcx.poolInfo)>("poolInfo");

				game.Redirect("GetMappedTypeStub", GetMappedTypeStub);
				game.Redirect("LoadStreamTA", LoadStream);
				game.Redirect("Load_String", Load_String);
				game.Redirect("Load_StringName", Load_String); // str
				game.Redirect("DB_AddAsset", DB_AddAsset);
				game.Redirect("DB_AddAssetRef", DB_AddAssetRef);
				game.Redirect("Load_CustomScriptString", Load_CustomScriptString);
				game.Redirect("DB_LoadStreamOffset", DB_LoadStreamOffset); // 2E25100
				game.Redirect("DB_RegisterStreamOffset", DB_RegisterStreamOffset); //2E24F20
				game.Redirect("$Unk_RetFalse", ReturnStub<4, bool, false>);
				game.Redirect("$Unk_Align_Ret", Unk_Align_Ret); // 2DE3CC0
				game.ApplyNullScans("fastfile");


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
					worker->PreLoadWorker(nullptr);
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
				gcx.streamLocations.clear();

				if (!reader.CanRead(sizeof(gcx.assets))) {
					LOG_WARNING("empty fastfile, ignored");
					return;
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
									DB_RegisterStreamOffset(nullptr, stroff, (void*)str);
									//LOG_OPT_INFO("store offset {:x} -> {}", stroff, str);
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

				LOG_OPT_INFO("String dump into {}", outStrings.string());
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

				gcx.assets.unk10 = reader.ReadPtr<uint32_t>(gcx.assets.unk10_count);
				// idk
				//std::filesystem::path outUnk{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "unk" / std::format("{}.csv", ctx.ffname) };
				//{
				//	std::filesystem::create_directories(outUnk.parent_path());
				//	utils::OutFileCE stringsOs{ outUnk };
				//	for (size_t i = 0; i < gcx.assets.unk10_count; i++) {
				//		stringsOs << i << ",0x" << std::hex << gcx.assets.unk10[i] << "," << hashutils::ExtractTmp("hash", gcx.assets.unk10[i]) << "\n";
				//	}
				//}
				//
				//LOG_OPT_INFO("unk dump into {}", outUnk.string());

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

						sos
							<< utils::FormattedStringJson{ s }
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
					LOG_OPT_INFO("XStrings names dump into {}", ostr.string());
					gcx.xstringLocs = nullptr;
				}

				LOG_DEBUG("done reading {}", ctx.ffname);
			}

		};

		
		utils::ArrayAdder<FFHandlerImpl, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
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
		if (!handle) return 0;
		return gcx.poolInfo[type].GetAssetName(handle);
	}
}