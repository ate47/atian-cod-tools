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
#include <tools/sp23/sp23.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_mwiii_sp.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/fastfile/fastfile_names_store.hpp>


namespace fastfile::handlers::mwiiisp {
	using namespace ::sp23;
	constexpr bool hasRelativeLoads = false;
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
			HandlerAssetType type;
			void* handle;
		};
		static_assert(sizeof(Asset) == 0x10);

		struct AssetList {
			uint32_t stringsCount;
			bool stringsLoaded;
			const char** strings;
			uint32_t assetsCount;
			uint32_t assetsLoaded;
			Asset* assets;
		};
		static_assert(sizeof(AssetList) == 0x20);

		struct HashedType {
			HandlerHashedAssetType hash;
			HandlerAssetType type;
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


		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };

		struct XStringOutCTX {
			std::filesystem::path path;
			std::map<std::string, std::unordered_set<uint64_t>> map{};
		};
		enum XFileBlock {
			XFILE_BLOCK_MEMMAPPED = 11,
			XFILE_BLOCK_VIRTUAL = 15,
			XFILE_BLOCK_COUNT = 16,
		};

		struct RewindData {
			byte data[0x6000];
			uint32_t unk6000;
			uint32_t unk6004;
			uint32_t unk6008;
			uint32_t unk600c;
			uint32_t unk6010;
			uint32_t unk6014;
			uint64_t unk6018;
		}; static_assert(sizeof(RewindData) == 0x6020);

		struct XBlock {
			byte* data;
			uint64_t size;
		}; static_assert(sizeof(XBlock) == 0x10);


		struct {
			void (*Load_Asset)(bool atStreamStart, Asset* asset) {};
			void (*DB_InitStreams)(XBlock* blocks) {};
			void (*Load_ScriptStringList)(bool atStreamStart, AssetList* list) {};
			void (*DB_PatchMem_FixStreamAlignment)(int align) {};
			void (*DB_PopStreamPos)() {};
			void (*DB_PushStreamPos)(XFileBlock type) {};

			size_t lastBlockCount{};
			AssetPoolInfo* poolInfo{};
			byte** streamPos{};
			RewindData** rewind{};
			XFileBlock* streamPosIndex{};
			byte* unkFixStreamAlign{};
			std::unique_ptr<RewindData[]> rewindBuffer{};
			std::unique_ptr<XBlock[]> blocks{};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<HandlerHashedAssetType, std::unordered_map<uint64_t, void*>> linkedAssets{};
			AssetList assets{};
			std::unordered_map<uint64_t, uint32_t> scrStringMap{};
			std::unordered_map<uint64_t, void*> streamLocations{};
			std::vector<const char*>* xstringLocs{};
			std::unique_ptr<XStringOutCTX> xstrOutGlb{};
			games::cod::asset_names::AssetNames<HandlerHashedAssetType, HandlerAssetType> assetNames{};
			fastfile::names_store::NamesStore namesStore{ [](const char* name) -> uint64_t { return hash::HashIWAsset(name); } };
			utils::OutFileCE* outAsset{};
		} gcx{};



		inline void DECLSPEC_NORETURN ThrowFastFileError_(const std::string& msg) {
			LOG_LVL(core::logs::LVL_ERROR, msg);
			hook::error::DumpStackTraceFrom();
			throw std::runtime_error("fastfile error");
		}

#define ThrowFastFileError(...) ThrowFastFileError_(std::format(__VA_ARGS__))

		void LoadXFileData(void* ptr, int64_t len) {
			LOG_TRACE("LoadXFileData({}, 0x{:x}/0x{:x}) {}", ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{ _ReturnAddress() });
			if (!ptr) {
				ThrowFastFileError("Can't read empty pointer idx:{}", (int)*gcx.streamPosIndex);
			}
			if (!gcx.reader->CanRead(len)) {
				hook::error::DumpStackTraceFrom();
			}
			gcx.reader->Read(ptr, len);

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

		void DB_IncStreamPos(size_t len) {
			*gcx.streamPos += len;
		}

		bool LoadStreamTA(bool atStreamStart, void* ptr, int64_t len) {
			LOG_TRACE("LoadStreamTA({}, {}, 0x{:x}/0x{:x}) {}", atStreamStart, ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{ _ReturnAddress() });
			if (!atStreamStart || !len) {
				return true;
			}

			bool notMapped{ *gcx.streamPosIndex != XFileBlock::XFILE_BLOCK_MEMMAPPED };

			if (notMapped) {
				LoadXFileData(ptr, len);
			}

			DB_IncStreamPos(len);

			return notMapped;

		}

		void Load_String(char** str) {
			LOG_TRACE("{} Load_String()", hook::library::CodePointer{ _ReturnAddress() });
			size_t size;
			char* ptr{ gcx.reader->ReadString(&size) };
			ptr = acts::decryptutils::DecryptString(ptr);
			std::memcpy(*str, ptr, std::strlen(ptr) + 1);
			hashutils::Add(*str, true, true);
			DB_IncStreamPos(size + 1);
			if (gcx.xstringLocs) gcx.xstringLocs->push_back(*str);
		}

		void Load_CustomScriptString(uint32_t* pstr) {
			//LOG_TRACE("Load_CustomScriptString({}) {}", (void*)pstr, hook::library::CodePointer{ _ReturnAddress() });
			// nothing to do
		}


		bool LoadStreamImpl(LoadStreamObjectData* that, bool* atStreamStart, void** data, int64_t* len) {
			// redirect to custom version
			return LoadStreamTA(*atStreamStart, *data, *len);
		}

		void* DB_AddAsset(HandlerAssetType type, void** handle) {
			HandlerHashedAssetType hashType{ GetHashType(type) };
			uint64_t hash{ GetXAssetName(hashType, handle ? *handle : 0) };
			const char* name{ hashutils::ExtractTmp("hash", hash) };
			const char* poolName{ GetPoolName(hashType) };
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

		template<HandlerHashedAssetType type>
		void* DB_AddAssetCustom(void** handle) {
			return DB_AddAsset(GetExePoolId(type), handle);
		}

		class FFHandlerImpl : public fastfile::FFHandler {
		public:
			FFHandlerImpl() : fastfile::FFHandler(handlerId, handlerName) {
			}

			void Init(fastfile::FastFileOption& opt) override {
				acts::game_data::GameData game{ gameDumpId };
				std::string gameExe{ game.Config().GetString("module") };
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, gameExe.data(), gameDumpId) };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				game.SetScanContainer(&scan);;
				scan.Sync();

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(mod)) {
					throw std::runtime_error("Can't load decryption module");
				}

#ifdef CI_BUILD
				LOG_WARNING("You are using the {} ({}) " " handler", handlerName, handlerId);
				LOG_WARNING("This handler was developed for the Steam version and might not");
				LOG_WARNING("work for the BattleNet, COD HQ or the Gamepass version.");
#endif

				// should be done before the handleList to have the hashes loaded
				gcx.assetNames.InitMap(mod, "physicslibrary", "string");
				games::cod::asset_names::AssetDumpFileOptions dumpOpts{};
				dumpOpts.baseFileName = "mwiii";
				dumpOpts.assetHashedName = "JupHashAssetType";
				dumpOpts.assetTypeName = "JupAssetType";
				dumpOpts.assetPrefix = "JUP_ASSET_";
				dumpOpts.assetHashedPrefix = "JUPH_ASSET_";
				gcx.assetNames.DumpFiles(opt.m_output / gamePath / "code", &dumpOpts);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				gcx.namesStore.LoadConfig(opt.assets);

				scan.ignoreMissing = true;

				LoadStreamObject* loadStreamObj{ game.GetPointer<decltype(loadStreamObj)>("loadStreamObj") };
				if (loadStreamObj) {
					loadStreamObj->__vtb = &dbLoadStreamVTable;
				}

				gcx.Load_Asset = game.GetPointer<decltype(gcx.Load_Asset)>("Load_Asset");
				gcx.DB_InitStreams = game.GetPointer<decltype(gcx.DB_InitStreams)>("DB_InitStreams");
				gcx.Load_ScriptStringList = game.GetPointer<decltype(gcx.Load_ScriptStringList)>("Load_ScriptStringList");
				gcx.DB_PatchMem_FixStreamAlignment = game.GetPointer<decltype(gcx.DB_PatchMem_FixStreamAlignment)>("DB_PatchMem_FixStreamAlignment");
				gcx.DB_PushStreamPos = game.GetPointer<decltype(gcx.DB_PushStreamPos)>("DB_PushStreamPos");
				gcx.DB_PopStreamPos = game.GetPointer<decltype(gcx.DB_PopStreamPos)>("DB_PopStreamPos");
				gcx.streamPos = game.GetPointer<decltype(gcx.streamPos)>("streamPos");
				gcx.rewind = game.GetPointer<decltype(gcx.rewind)>("rewind");
				gcx.streamPosIndex = game.GetPointer<decltype(gcx.streamPosIndex)>("streamPosIndex");
				gcx.unkFixStreamAlign = game.GetPointer<decltype(gcx.unkFixStreamAlign)>("$unkFixStreamAlign");
				gcx.poolInfo = game.GetPointer<decltype(gcx.poolInfo)>("poolInfo");

				game.Redirect("LoadStreamTA", LoadStreamTA);
				game.Redirect("Load_StringName", Load_String);
				game.Redirect("Load_String", Load_String);
				game.Redirect("DB_AddXAsset", DB_AddAsset);
				game.Redirect("Load_CustomScriptString", Load_CustomScriptString);

				game.Redirect("DB_LinkSoundBank", DB_AddAssetCustom<JUPH_ASSET_SOUNDBANK>);
				game.Redirect("DB_LinkSoundBankTransient", DB_AddAssetCustom<JUPH_ASSET_SOUNDBANKTRANSIENT>);
				game.Redirect("DB_LinkStreamingInfo", DB_AddAssetCustom<JUPH_ASSET_STREAMINGINFO>);
				game.Redirect("DB_LinkComputeshader", DB_AddAssetCustom<JUPH_ASSET_COMPUTESHADER>);
				game.Redirect("DB_LinkLibShader", DB_AddAssetCustom<JUPH_ASSET_LIBSHADER>);
				game.Redirect("DB_LinkDLogSchema", DB_AddAssetCustom<JUPH_ASSET_DLOGSCHEMA>);
				game.Redirect("DB_LinkXModel", DB_AddAssetCustom<JUPH_ASSET_XMODEL>);
				game.Redirect("DB_LinkSoundGlobalContext", DB_AddAssetCustom<JUPH_ASSET_SOUNDGLOBALCONTEXT>);
				game.Redirect("DB_LinkSoundGlobalVolMod", DB_AddAssetCustom<JUPH_ASSET_SOUNDGLOBALVOLMOD>);
				game.Redirect("DB_LinkSndTable", DB_AddAssetCustom<JUPH_ASSET_SNDTABLE>);
				game.Redirect("DB_LinkSoundSubmix", DB_AddAssetCustom<JUPH_ASSET_SOUNDSUBMIX>);
				game.Redirect("DB_LinkReverbPreset", DB_AddAssetCustom<JUPH_ASSET_REVERBPRESET>);
				game.Redirect("DB_LinkSndMasterPreset", DB_AddAssetCustom<JUPH_ASSET_SNDMASTERPRESET>);
				game.ApplyNullScans("fastfile");

				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}

				if (!opt.noWorkerPreload) {
					for (auto& [hashType, worker] : GetWorkers()) {
						worker->PreLoadWorker(nullptr);
					}
				}

				for (auto& [hashType, worker] : GetWorkers()) {
					HandlerAssetType type{ GetExePoolId(hashType) };
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

				// cleanup data
				gcx.rewindBuffer = nullptr;
				gcx.blocks = nullptr;
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				gcx.streamLocations.clear();

				gcx.opt->assetNames.clear();

				gcx.allocator.FreeAll();
				gcx.linkedAssets.clear();
				gcx.xstringLocs = nullptr;

				if (!ctx.blocksCount) {
					throw std::runtime_error("No block information");
				}

				if (!reader.CanRead(sizeof(gcx.assets))) {
					LOG_WARNING("empty fastfile, ignored");
					return;
				}

				// alloc data
				bool newSizes{ ctx.blocksCount != gcx.lastBlockCount };

				if (newSizes || !gcx.rewindBuffer) {
					gcx.rewindBuffer = std::make_unique<RewindData[]>(ctx.blocksCount);
				}
				if (newSizes || !gcx.blocks) {
					gcx.blocks = std::make_unique<XBlock[]>(ctx.blocksCount);
				}
				gcx.lastBlockCount = ctx.blocksCount;

				// setup rewind buffers
				std::memset(gcx.rewindBuffer.get(), 0, sizeof(gcx.rewindBuffer[0]) * ctx.blocksCount);
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					gcx.rewind[i] = &gcx.rewindBuffer[i];
				}

				// setup memory blocks
				for (size_t i = 0; i < ctx.blocksCount; i++) {
					size_t len{ ctx.blockSizes[i].size };
					gcx.blocks[i].size = len;
					if (len) {
						gcx.blocks[i].data = gcx.allocator.AllocAligned<byte>(len, 0x100000);
					}
					else {
						gcx.blocks[i].data = nullptr;
					}

					LOG_DEBUG("Block size {} = 0x{:x}", i, len);
				}
				gcx.DB_InitStreams(gcx.blocks.get());
				*gcx.unkFixStreamAlign = 1;

				LoadXFileData(&gcx.assets, sizeof(gcx.assets));

				LOG_DEBUG("assets: {}, strings: {}", gcx.assets.assetsCount, gcx.assets.stringsCount);


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
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE stringsOs{ outStrings };
					gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
					gcx.Load_ScriptStringList(false, &gcx.assets);
					gcx.DB_PopStreamPos();

					// decrypt and print the strings
					for (size_t i = 0; i < gcx.assets.stringsCount; i++) {
						const char* str{ gcx.assets.strings[i] };
						if (!str) {
							continue;
						}
						gcx.assets.strings[i] = acts::decryptutils::DecryptString((char*)str);
						stringsOs
							<< std::dec << std::setfill(' ') << std::setw(utils::Log<10>(gcx.assets.stringsCount) + 1) << i << "\t"
							<< gcx.assets.strings[i] << "\n";
					}
				}

				LOG_OPT_INFO("String dump into {}", outStrings.string());
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

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
					gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
					gcx.DB_PatchMem_FixStreamAlignment(7);
					gcx.assets.assets = (Asset*)*gcx.streamPos;
					LoadStreamTA(true, gcx.assets.assets, sizeof(gcx.assets.assets[0]) * gcx.assets.assetsCount);

					for (auto& [k, v] : GetWorkers()) {
						if constexpr (!hasRelativeLoads) {
							if (v->requiresRelativeLoads) continue;
						}
						v->PreXFileLoading(*gcx.opt, ctx);
					}

					bool err{};
					size_t assetId;
					try {
						for (assetId = 0; assetId < gcx.assets.assetsCount; assetId++) {
							Asset* asset{ &gcx.assets.assets[assetId] };

							LOG_DEBUG("load #{} -> {}({})", assetId, PoolName(GetHashType(asset->type)), (int)asset->type);
							gcx.Load_Asset(false, asset);
						}

					}
					catch (std::runtime_error& e) {
						LOG_ERROR("can't dump ff {}", e.what());
					}

					for (auto& [k, v] : GetWorkers()) {
						if constexpr (!hasRelativeLoads) {
							if (v->requiresRelativeLoads) continue;
						}
						v->PostXFileLoading(*gcx.opt, ctx);
					}
					gcx.DB_PopStreamPos();
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
							h.insert(hash::HashJupScr(s));
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

	const char* GetPoolName(uint32_t hash) {
		return gcx.assetNames.GetMappedType((HandlerHashedAssetType)hash)->name;
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

	HandlerHashedAssetType GetHashType(HandlerAssetType type) {
		return gcx.assetNames.GetHashType(type);
	}

	HandlerAssetType GetExePoolId(const char* name) {
		return gcx.assetNames.GetExePoolId(name);
	}
	HandlerAssetType GetExePoolId(HandlerHashedAssetType name) {
		return gcx.assetNames.GetExePoolId(name);
	}

	uint64_t GetXAssetName(HandlerHashedAssetType htype, void* handle) {
		HandlerAssetType type{ GetExePoolId(htype) };
		if (type == gcx.assetNames.InvalidId()) {
			throw std::runtime_error(std::format("INVALID HASH TYPE ID {}", PoolName(htype)));
		}
		return gcx.poolInfo[type].GetAssetName(handle);
	}
}
