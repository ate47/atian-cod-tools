#include <includes.hpp>
#include <game_data.hpp>
#include <games/cod/asset_names.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <hook/scan_container.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <tools/bo6/bo6.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_bo6_sp.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/fastfile/fastfile_names_store.hpp>


namespace fastfile::handlers::bo6sp {
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
			T10AssetType type;
			void* handle;
		};

		struct AssetList {
			uint32_t stringsCount;
			bool stringsLoaded;
			const char** strings;
			uint32_t assetsCount;
			uint32_t assetsLoaded;
			Asset* assets;
		};

		struct HashedType {
			T10HashAssetType hash;
			T10AssetType type;
			const char* name;
		};

		union LoadStreamObjectData;

		struct LoadStreamObjectVtable {
			bool(__fastcall* LoadStream)(LoadStreamObjectData* that, bool* atStreamStart, void** data, int64_t* len);
			void(__fastcall* copy2)(LoadStreamObjectData* that);
			void(__fastcall* copy3)(LoadStreamObjectData* that);
			void(__fastcall* f4)(LoadStreamObjectData* that);
			void(__fastcall* nulled)(LoadStreamObjectData* that);
		};

		union LoadStreamObjectData {
			struct {
				bool(*LoadStream)(bool atStreamStart, void* ptr, size_t len);
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


		bool LoadStreamImpl(LoadStreamObjectData* that, bool* atStreamStart, void** data, int64_t* len);
		void ErrorStub(LoadStreamObjectData* that) {
			throw std::runtime_error(std::format("Error loadstream {}", hook::library::CodePointer{ _ReturnAddress() }));
		}
		void AllocStreamPos(int align);
		void PushStreamPos(int type);
		void PopStreamPos();
		void PreAssetRead(T10AssetType type);
		void PostAssetRead();


		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };

		struct XStringOutCTX {
			std::filesystem::path path;
			std::map<std::string, std::unordered_set<uint64_t>> map{};
		};

		constexpr T10AssetType MAX_ASSET_COUNT = (T10AssetType)512;

		struct {
			BO6FFHandler* handler{};
			void (*Load_Asset)(bool atStreamStart, Asset* asset) {};
			AssetPoolInfo* poolInfo{};
			byte** db_streamData{};

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
			games::cod::asset_names::AssetNames<T10HashAssetType, T10AssetType> assetNames{ bo6::PoolId };
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


		void LoadXFileData(void* ptr, int64_t len) {
			LOG_TRACE("LoadXFileData({}, 0x{:x}/0x{:x}) {}", ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{_ReturnAddress()});
			if (!gcx.reader->CanRead(len)) {
				hook::error::DumpStackTraceFrom();
			}
			gcx.reader->Read(ptr, len);
			*gcx.db_streamData = gcx.reader->Ptr();

			if constexpr (traceData) {
				size_t s{ std::min<size_t>(len, 0x40 * 2) };
				byte* p{ (byte*)ptr };
				while (s > 0) {
					size_t ss{ std::min<size_t>(s, 0x40) };
					LOG_TRACE("{:03x} : {}", p - (byte*)ptr, utils::data::AsHex(p, ss));
					p += ss;
					s -= ss;
				}
			}
		}

		bool LoadStream(bool atStreamStart, void* ptr, int64_t len) {
			LOG_TRACE("LoadStream({}, {}, 0x{:x}/0x{:x}) {}", atStreamStart, ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{_ReturnAddress()});
			if (!atStreamStart || !len) {
				return true;
			}
			if (!gcx.streamPosStackIndex) {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error("empty streampos stack");
			}

			int block{ gcx.streamPosStack[gcx.streamPosStackIndex - 1] };

			if (block == 11) {
				return false;
			}
			else {
				LoadXFileData(ptr, len);
				return true;
			}

		}

		void Load_String(char** pstr) {
			LOG_TRACE("Load_String({}/0x{:x}:0x{:x}) {}", (void*)pstr, gcx.reader->Loc(), gcx.reader->Remaining(), hook::library::CodePointer{_ReturnAddress()});
			char* str{ *pstr };
			do {
				if (!gcx.reader->CanRead(1)) {
					LOG_ERROR("Load_String error with size 0x{:x}", str - *pstr);
					hook::error::DumpStackTraceFrom();
				}
				gcx.reader->Read(str, 1);
			} while (*str++);
			*gcx.db_streamData = gcx.reader->Ptr();
			if (gcx.xstringLocs) gcx.xstringLocs->push_back(*pstr);
			hashutils::Add(*pstr, true, true);
		}

		void Load_CustomScriptString(uint32_t* pstr) {
			//LOG_TRACE("Load_CustomScriptString({}) {}", (void*)pstr, hook::library::CodePointer{ _ReturnAddress() });
			// nothing to do
		}

		void AllocStreamPos(int align) {
			LOG_TRACE("AllocStreamPos({}) {}", align, hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.reader->CanRead(1)) {
				*gcx.db_streamData = gcx.reader->Ptr();
			}
			else {
				static byte empty[1];
				*gcx.db_streamData = empty;
			}
		}

		void PushStreamPos(int type) {
			LOG_TRACE("PushStreamPos({}) {}", type, hook::library::CodePointer{ _ReturnAddress() });
			gcx.streamPosStack[gcx.streamPosStackIndex++] = type;
			*gcx.db_streamData = gcx.reader->Ptr();
		}

		void PopStreamPos() {
			LOG_TRACE("PopStreamPos() {}", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.streamPosStackIndex) throw std::runtime_error("Can't pop empty stack");
			gcx.streamPosStackIndex--;
		}

		void PreAssetRead(T10AssetType type) {
			LOG_TRACE("PreAssetRead({}) {}", PoolName(GetHashType(type)), hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.assetLoadStackTop == ARRAYSIZE(gcx.assetLoadStack)) {
				throw std::runtime_error("PreAssetRead stack overflow");
			}
			gcx.assetLoadStack[gcx.assetLoadStackTop++] = GetHashType(type);
		}

		void PostAssetRead() {
			LOG_TRACE("PostAssetRead({}) {}", gcx.assetLoadStackTop ? PoolName(gcx.assetLoadStack[gcx.assetLoadStackTop - 1]) : "invalid", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.assetLoadStackTop) {
				throw std::runtime_error("PostAssetRead stack underflow");
			}
			gcx.assetLoadStackTop--;
		}


		bool LoadStreamImpl(LoadStreamObjectData* that, bool* atStreamStart, void** data, int64_t* len) {
			// redirect to custom version
			return LoadStream(*atStreamStart, *data, *len);
		}

		void* DB_AddAsset(T10AssetType type, void** handle) {
			T10HashAssetType hashType{ GetHashType(type) };
			uint64_t hash{ GetXAssetName(hashType, handle ? *handle : 0) };
			const char* name{ hashutils::ExtractTmp("hash", hash) };
			const char* poolName{ PoolName(hashType) };
			LOG_DEBUG("DB_AddAsset({}, '{}') {}", poolName, name, hook::library::CodePointer{ _ReturnAddress() });
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
			LOG_DEBUG("DB_AddAssetRef({}, '{}') {}", PoolName(GetHashType(type)), hashutils::ExtractTmp("hash", name), hook::library::CodePointer{ _ReturnAddress() });
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

		template<T10HashAssetType type>
		void* DB_AddAssetCustom(void** handle) {
			return DB_AddAsset(GetExePoolId(type), handle);
		}

		uint32_t* Unk_Align_Ret() {
			static uint32_t data{};
			return &data;
		}

		void DB_LoadStreamOffset(uint64_t val, void** pptr) {
			void*& ref{ gcx.streamLocations[val & StreamPointerFlag::SPF_DATA_MASK] };
			if (ref) {
				*pptr = ref;
			}
			else {
				LOG_WARNING("missing val for DB_LoadStreamOffset {:x}", val);
				*pptr = nullptr;
			}
		}

		void DB_RegisterStreamOffset(uint64_t val, void* ptr) {
			gcx.streamLocations[val & StreamPointerFlag::SPF_DATA_MASK] = ptr;
		}

		class BO6FFHandler : public fastfile::FFHandler {
		public:
			// -w "((mp|zm)_t10|ingame|code|global).*"
			BO6FFHandler() : fastfile::FFHandler("bo6sp", "Black Ops 6 Campaign") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				acts::game_data::GameData game{ "bo6sp"};
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, game.GetModuleName(), "bo6sp") };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				game.SetScanContainer(&scan);
				scan.Sync();

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(mod)) {
					throw std::runtime_error("Can't load decryption module");
				}

#ifdef CI_BUILD
				LOG_WARNING("You are using the bo6 sp handler, for now Black Ops 6 is still under");
				LOG_WARNING("Development and it might not work after a game update.");
				LOG_WARNING("This handler was developed for the Steam version and might not");
				LOG_WARNING("work for the BattleNet or the Gamepass version.");
#endif

				// should be done before the handleList to have the hashes loaded
				game.InitAssetNames(gcx.assetNames);
				AddBootsLimitAssetNames();
				games::cod::asset_names::AssetDumpFileOptions dumpOpts{};
				dumpOpts.baseFileName = "bo6";
				dumpOpts.assetHashedName = "T10HashAssetType";
				dumpOpts.assetTypeName = "T10AssetType";
				dumpOpts.assetPrefix = "T10_ASSET_";
				dumpOpts.assetHashedPrefix = "T10H_ASSET_";
				gcx.assetNames.DumpFiles(opt.m_output / "bo6sp" / "code", &dumpOpts);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				gcx.namesStore.LoadConfig(opt.assets);

				scan.ignoreMissing = true;

				LoadStreamObject* loadStreamObj{ game.GetPointer<LoadStreamObject*>("loadStreamObj") };

				if (loadStreamObj) {
					loadStreamObj->__vtb = &dbLoadStreamVTable;
				}

				gcx.Load_Asset = game.GetPointer<decltype(gcx.Load_Asset)>("Load_Asset");
				gcx.db_streamData = game.GetPointer<decltype(gcx.db_streamData)>("db_streamData");
				gcx.poolInfo = game.GetPointer<decltype(gcx.poolInfo)>("poolInfo");

				game.Redirect("LoadStreamTA", LoadStream);
				game.Redirect("Load_String", Load_String);
				game.Redirect("Load_StringName", Load_String);
				game.Redirect("DB_AddXAsset", DB_AddAsset);
				game.Redirect("DB_AddXAssetRef", DB_AddAssetRef);
				game.Redirect("Load_CustomScriptString", Load_CustomScriptString);
				game.Redirect("AllocStreamPos", AllocStreamPos);
				game.Redirect("PreAssetRead", PreAssetRead);
				game.Redirect("PostAssetRead", PostAssetRead);
				game.Redirect("PushStreamPos", PushStreamPos);
				game.Redirect("PopStreamPos", PopStreamPos);
				game.Redirect("DB_RegisterStreamOffset", DB_RegisterStreamOffset);
				game.Redirect("DB_LoadStreamOffset", DB_LoadStreamOffset);

				game.Redirect("DB_LinkSoundBank", DB_AddAssetCustom<bo6::T10H_ASSET_SOUNDBANK>);
				game.Redirect("DB_LinkSoundBankTransient", DB_AddAssetCustom<bo6::T10H_ASSET_SOUNDBANKTRANSIENT>);
				game.Redirect("DB_LinkStreamingInfo", DB_AddAssetCustom<bo6::T10H_ASSET_STREAMINGINFO>);
				game.Redirect("DB_LinkComputeshader", DB_AddAssetCustom<bo6::T10H_ASSET_COMPUTESHADER>);
				game.Redirect("DB_LinkLibShader", DB_AddAssetCustom<bo6::T10H_ASSET_LIBSHADER>);
				game.Redirect("DB_LinkDLogSchema", DB_AddAssetCustom<bo6::T10H_ASSET_DLOGSCHEMA>);
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
					std::filesystem::path path{ opt.m_output / "bo6sp" / "xstrings_all.wni" };
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

				LOG_DEBUG("assets: {}, strings: {}", gcx.assets.assetsCount, gcx.assets.stringsCount);


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
				std::filesystem::path outStrings{ gcx.opt->m_output / "bo6sp" / "source" / "tables" / "data" / "strings" / fftype / std::format("{}.txt", ctx.ffname) };

				{
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
									DB_RegisterStreamOffset(stroff, (void*)str);
									//LOG_OPT_INFO("store offset {:x} -> {}", stroff, str);
								}
								hashutils::Add(str, true, true);
								if (gcx.xstringLocs) gcx.xstringLocs->push_back(str);
							}
							else {
								DB_LoadStreamOffset(stroff, (void**)&str);
							}
						}
						else {
							str = "";
						}
						gcx.assets.strings[i] = str;
						stringsOs << std::dec << std::setfill(' ') << std::setw(utils::Log<10>(gcx.assets.stringsCount) + 1) << i << "\t";
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

				if (!gcx.assets.assetsCount) {
					LOG_OPT_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / "bo6sp" / "source" / "tables" / "data" / "assets" / fftype / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					gcx.outAsset = &assetsOs;
					assetsOs << "type,name";
					gcx.assets.assets = reader.ReadPtr<Asset>(gcx.assets.assetsCount);

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

							LOG_DEBUG("load #{} -> {}({})", assetId, PoolName(bo6sp::GetHashType(asset->type)), (int)asset->type);
							gcx.Load_Asset(false, asset);
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
						if constexpr (!hasRelativeLoads) {
							if (v->requiresRelativeLoads) continue;
						}
						v->PostXFileLoading(*gcx.opt, ctx);
					}
				}
				LOG_OPT_INFO("Asset names dump into {}", outAssets.string());
				if (gcx.xstringLocs) {
					std::filesystem::path ostr{ gcx.opt->m_output / "bo6sp" / "source" / "tables" / "data" / "xstrings" / fftype / std::format("{}.txt", ctx.ffname) };
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