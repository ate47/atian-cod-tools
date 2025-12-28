#include <includes.hpp>
#include <games/cod/asset_names.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <hook/scan_container.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_vg.hpp>
#include <tools/compatibility/scobalula_wnigen.hpp>
#include <tools/fastfile/fastfile_names_store.hpp>

namespace fastfile::handlers::vg {
	using namespace tool::mw19;
	constexpr bool hasRelativeLoads = false;
	constexpr bool traceData = false;

	namespace {
		template<size_t offset = 0>
		void EmptyStub() {
			LOG_TRACE("{} EmptyStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
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

		struct XStringOutCTX {
			std::filesystem::path path;
			std::map<std::string, std::unordered_set<uint64_t>> map{};
		};
		enum XFileBlock {
			XFILE_BLOCK_MEMMAPPED = 6,
			XFILE_BLOCK_VIRTUAL = 7,
			XFILE_BLOCK_COUNT = 10,
		};

		struct RewindStreamData {
			byte rewindHistory[0x6000];
			uint32_t rewindHistorySize;
			uint32_t currentRewind;
			uint32_t historyWalkIndex;
			uint32_t streamSizeCheck;
			uint32_t lastExpectedOffset;
			uint32_t lastExpectedAssetOffset;
			byte* lastActualAssetPos;
		}; static_assert(sizeof(RewindStreamData) == 0x6020);

		struct XBlock {
			byte* data;
			uint64_t size;
		}; static_assert(sizeof(XBlock) == 0x10);


		struct {
			void (*Load_Asset)(bool loaded) {};
			Asset** loadAsset{};
			void (*DB_InitStreams)(XBlock* blocks) {};
			void (*Load_ScriptStringList)(bool loaded) {};
			AssetList** loadStringList{};
			void (*DB_PatchMem_FixStreamAlignment)(int align) {};
			void (*DB_PopStreamPos)() {};
			void (*DB_PushStreamPos)(XFileBlock type) {};
			void (*DB_PatchMem_TryDiscard)(Asset* asset) {};
			void (*DB_PatchMem_BeginLoad)() {};
			uint32_t(*DB_GetAssetSize)(HandlerAssetType type) {};

			size_t lastBlockCount{};
			byte** streamPos{};
			RewindStreamData** rewind{};
			XFileBlock* streamPosIndex{};
			byte* s_rewindPostload{};
			std::unique_ptr<RewindStreamData[]> rewindBuffer{};
			std::unique_ptr<XBlock[]> blocks{};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			size_t currentAsset{};
			AssetList assets{};
			std::unordered_map<uint64_t, uint32_t> scrStringMap{};
			std::unordered_map<uint64_t, void*> streamLocations{};
			std::vector<const char*>* xstringLocs{};
			std::unique_ptr<XStringOutCTX> xstrOutGlb{};
			games::cod::asset_names::AssetNames<HandlerHashedAssetType, HandlerAssetType> assetNames{ "physicslibrary", "string", PoolId };
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

		void LoadStreamTA(bool loadData, void* ptr, int64_t len) {
			LOG_TRACE("LoadStreamTA({}, {}, 0x{:x}/0x{:x}) {}", loadData, ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{ _ReturnAddress() });

			if (!loadData && len) {
				if (*gcx.streamPosIndex != XFileBlock::XFILE_BLOCK_MEMMAPPED) {
					LoadXFileData(ptr, len);
				}
				else {
					std::memset(ptr, 0, len);
				}

				DB_IncStreamPos(len);
			}
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

		void* DB_AddAsset(HandlerAssetType type, void** handle) {
			HandlerHashedAssetType hashType{ GetHashType(type) };
			const char* poolName{ GetPoolName(hashType) };
			if (!handle || !*handle) {
				LOG_WARNING("DB_AddAsset({}, null) {}", poolName, hook::library::CodePointer{ _ReturnAddress() });
				return nullptr; // wtf?
			}
			XString name{ GetXAssetName(hashType, *handle) };

			size_t assetSize{ gcx.DB_GetAssetSize(type) };
			void* data{ gcx.allocator.Alloc(assetSize) };
			std::memcpy(data, *handle, assetSize);
			*handle = data;

			LOG_TRACE("DB_AddAsset({}, '{}') {}", poolName, name, hook::library::CodePointer{ _ReturnAddress() });
			*(gcx.outAsset) << "\n" << poolName << ",#" << name;

			if (!(gcx.opt->noAssetDump || !gcx.assetNames.ShouldHandle(type) || !gcx.namesStore.Contains(hash::HashIWAsset(name), true))) {

				std::unordered_map<HandlerHashedAssetType, Worker*>& map{ GetWorkers() };
				auto it{ map.find(hashType) };
				if (it != map.end()) {
					if (it->second->assetSize != assetSize) {
						LOG_ERROR("Can't check size of asset entry {}({}): 0x{:x} != 0x{:x}", gcx.assetNames.GetTypeName(type), (int)type, it->second->assetSize, assetSize);
					}
					else {
						it->second->Unlink(*gcx.opt, *gcx.ctx, *handle);
					}
				}
			}

			Asset linkedAsset{};
			linkedAsset.type = type;
			linkedAsset.handle = *handle;

			gcx.DB_PatchMem_TryDiscard(&linkedAsset);

			return *handle;
		}

		template<HandlerHashedAssetType type>
		void* DB_AddAssetCustom(void** handle) {
			return DB_AddAsset(GetExePoolId(type), handle);
		}

		class FFHandlerImpl : public fastfile::FFHandler {
		public:
			FFHandlerImpl() : fastfile::FFHandler(handlerId, handlerName) {
				forceNumXBlocks = XFILE_BLOCK_COUNT;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true, nullptr, false, gameExe, gameDumpId) };
				hook::scan_container::ScanContainer scan{ lib, true };
				scan.Sync();

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(lib)) {
					throw std::runtime_error("Can't load decryption module");
				}

#ifdef CI_BUILD
				LOG_WARNING("You are using the {} ({}) handler", handlerName, handlerId);
				LOG_WARNING("This handler was developed for the Steam version and might not");
				LOG_WARNING("work for the BattleNet, COD HQ or the Gamepass version.");
#endif

				// should be done before the handleList to have the hashes loaded
				gcx.assetNames.InitMap(lib);
				games::cod::asset_names::AssetDumpFileOptions dumpOpts{};
				dumpOpts.baseFileName = "vg";
				dumpOpts.assetHashedName = "S4HashAssetType";
				dumpOpts.assetTypeName = "S4AssetType";
				dumpOpts.assetPrefix = "S4_ASSET_";
				dumpOpts.assetHashedPrefix = "S4H_ASSET_";
				gcx.assetNames.DumpFiles(opt.m_output / gamePath / "code", &dumpOpts);
				gcx.assetNames.LoadAssetConfig(opt.assetTypes);
				gcx.namesStore.LoadConfig(opt.assets);

				scan.ignoreMissing = true;

				gcx.Load_Asset = scan.ScanSingle("48 89 0D ?? ?? ?? ?? B9 01 00 00 00 E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? FF C3 FF 40 14 48 8B 05 ?? ?? ?? ?? 3B 58 10 72 B4 33 DB", "gcx.Load_Asset").GetRelative<int32_t, decltype(gcx.Load_Asset)>(13);
				gcx.loadAsset = scan.ScanSingle("48 89 0D ?? ?? ?? ?? B9 01 00 00 00 E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? FF C3 FF 40 14 48 8B 05 ?? ?? ?? ?? 3B 58 10 72 B4 33 DB", "gcx.loadAsset").GetRelative<int32_t, Asset**>(3);
				gcx.DB_InitStreams = scan.ScanSingle("E8 ?? ?? ?? ?? 48 89 1D ?? ?? ?? ?? 48 8B 43 60", "gcx.DB_InitStreams").GetRelative<int32_t, decltype(gcx.DB_InitStreams)>(1);
				gcx.Load_ScriptStringList = scan.ScanSingle("48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? C6 40 04 01", "gcx.Load_ScriptStringList").GetRelative<int32_t, decltype(gcx.Load_ScriptStringList)>(8);
				gcx.loadStringList = scan.ScanSingle("48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? C6 40 04 01", "gcx.loadStringList").GetRelative<int32_t, AssetList**>(3);
				gcx.DB_PatchMem_FixStreamAlignment = scan.ScanSingle("40 53 80 3D ?? ?? ?? ?? ?? 4C", "gcx.DB_PatchMem_FixStreamAlignment").GetPtr<decltype(gcx.DB_PatchMem_FixStreamAlignment)>();
				gcx.DB_PushStreamPos = scan.ScanSingle("8B 15 ?? ?? ?? ?? 4C 8D 0D ?? ?? ?? ?? 8B 05", "gcx.DB_PushStreamPos").GetPtr<decltype(gcx.DB_PushStreamPos)>();
				gcx.DB_PopStreamPos = scan.ScanSingle("44 8B 05 ?? ?? ?? ?? 4C 8D 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 41 FF", "gcx.DB_PopStreamPos").GetPtr<decltype(gcx.DB_PopStreamPos)>();
				gcx.DB_PatchMem_TryDiscard = scan.ScanSingle("41 54 48 83 EC 40 80", "gcx.DB_PatchMem_TryDiscard").GetPtr<decltype(gcx.DB_PatchMem_TryDiscard)>();
				gcx.DB_GetAssetSize = scan.ScanSingle("E8 ?? ?? ?? ?? 49 8B 14 24 48 8B 0E", "gcx.DB_GetAssetSize").GetRelative<int32_t, decltype(gcx.DB_GetAssetSize)>(1);
				gcx.DB_PatchMem_BeginLoad = scan.ScanSingle("48 8B 05 ?? ?? ?? ?? 33 C9 48 85", "gcx.DB_PatchMem_BeginLoad").GetPtr<decltype(gcx.DB_PatchMem_BeginLoad)>();
				gcx.streamPos = scan.ScanSingle("48 89 15 ?? ?? ?? ?? 41 8B 80", "gcx.streamPos").GetRelative<int32_t, byte**>(3);
				gcx.rewind = scan.ScanSingle("8B C1 4C 8D 15 ?? ?? ?? ?? 4D 8B 14 C2", "gcx.rewind").GetRelative<int32_t, RewindStreamData**>(5);
				gcx.streamPosIndex = scan.ScanSingle("44 8B 05 ?? ?? ?? ?? 4C 8D 0D ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 41 FF", "gcx.streamPosIndex").GetRelative<int32_t, XFileBlock*>(16);
				gcx.s_rewindPostload = scan.ScanSingle("40 53 80 3D ?? ?? ?? ?? 00 4C 8D 1D ?? ?? ?? ?? 4C 8B C9", "gcx.s_rewindPostload").GetRelative<int32_t, byte*>(4) + 1;

				LOG_TRACE("gcx.Load_Asset = {}", hook::library::CodePointer{ gcx.Load_Asset });
				LOG_TRACE("gcx.loadAsset = {}", hook::library::CodePointer{ gcx.loadAsset });
				LOG_TRACE("gcx.DB_InitStreams = {}", hook::library::CodePointer{ gcx.DB_InitStreams });
				LOG_TRACE("gcx.Load_ScriptStringList = {}", hook::library::CodePointer{ gcx.Load_ScriptStringList });
				LOG_TRACE("gcx.loadStringList = {}", hook::library::CodePointer{ gcx.loadStringList });
				LOG_TRACE("gcx.DB_PatchMem_FixStreamAlignment = {}", hook::library::CodePointer{ gcx.DB_PatchMem_FixStreamAlignment });
				LOG_TRACE("gcx.DB_PushStreamPos = {}", hook::library::CodePointer{ gcx.DB_PushStreamPos });
				LOG_TRACE("gcx.DB_PopStreamPos = {}", hook::library::CodePointer{ gcx.DB_PopStreamPos });
				LOG_TRACE("gcx.DB_PatchMem_TryDiscard = {}", hook::library::CodePointer{ gcx.DB_PatchMem_TryDiscard });
				LOG_TRACE("gcx.DB_PatchMem_BeginLoad = {}", hook::library::CodePointer{ gcx.DB_PatchMem_BeginLoad });
				LOG_TRACE("gcx.streamPos = {}", hook::library::CodePointer{ gcx.streamPos });
				LOG_TRACE("gcx.rewind = {}", hook::library::CodePointer{ gcx.rewind });
				LOG_TRACE("gcx.streamPosIndex = {}", hook::library::CodePointer{ gcx.streamPosIndex });
				LOG_TRACE("gcx.s_rewindPostload = {}", hook::library::CodePointer{ gcx.s_rewindPostload });

				auto Red = [](void* from, void* to) {
					if (from) {
						hook::memory::RedirectJmp(from, to);
					}
					};

				Red(scan.ScanSingle("83 F9 01 74 44 53", "LoadStreamTA").location, LoadStreamTA);
				Red(scan.ScanSingle("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 39 BA", "Load_StringName").location, Load_String);
				Red(scan.ScanSingle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 48 8B 31 48 8B F9 48 8B CE", "Load_String").location, Load_String);
				Red(scan.ScanSingle("48 89 5C 24 ?? 56 57 41 56 48 83 EC 20 8B", "DB_AddAsset").location, DB_AddAsset);
				Red(scan.ScanSingle("48 8B 05 ?? ?? ?? ?? 4C 63 01", "Load_CustomScriptString").location, Load_CustomScriptString);
				Red(scan.ScanSingle("E8 ?? ?? ?? ?? 80 3F 00 74 23", "LoadXFileData").GetRelative<int32_t, void*>(1), LoadXFileData);

				Red(scan.ScanSingle("40 53 48 81 EC 90 00 00 00 48 8B 84", "EmptyStub<2>").location, EmptyStub<2>);
				Red(scan.ScanSingle("40 53 56 41 57 B8", "EmptyStub<5>").location, EmptyStub<5>);
				Red(scan.ScanSingle("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC B0 00 00 00 48", "EmptyStub<8>").location, EmptyStub<8>);
				Red(scan.ScanSingle("48 89 5C 24 08 57 48 83 EC 30 48 8B F9 48 8B DA B9", "EmptyStub<9>").location, EmptyStub<9>);
				Red(scan.ScanSingle("40 53 55 56 57 41 56 48 81 EC A0 00 00 00 48", "EmptyStub<14>").location, EmptyStub<14>);
				Red(scan.ScanSingle("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 40 49 8B F1 41", "EmptyStub<15>").location, EmptyStub<15>);
				Red(scan.ScanSingle("49 8B C0 4C 8B CA 4C 8B 01", "EmptyStub<16>").location, EmptyStub<16>);
				Red(scan.ScanSingle("48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 20 33 FF 48 8D 51", "EmptyStub<17>").location, EmptyStub<17>);
				Red(scan.ScanSingle("48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 B8", "EmptyStub<18>").location, EmptyStub<18>);
				Red(scan.ScanSingle("40 53 48 81 EC 90 00 00 00 48 8B 9C", "EmptyStub<19>").location, EmptyStub<19>);
				Red(scan.ScanSingle("48 8B C4 48 89 58 08 48 89 70 10 57 48 81 EC B0 00 00 00 48 8B 31", "EmptyStub<20>").location, EmptyStub<20>);
				Red(scan.ScanSingle("48 89 5C 24 08 57 48 81 EC C0 00 00 00 48 8B BC", "EmptyStub<21>").location, EmptyStub<21>);
				Red(scan.ScanSingle("4C 8B DC 57 41 56 48 81 EC 58", "EmptyStub<22>").location, EmptyStub<22>);
				Red(scan.ScanSingle("48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 40 49", "EmptyStub<23>").location, EmptyStub<23>);
				Red(scan.ScanSingle("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 81 EC A0 00 00 00 48 8B B4 24 D0", "EmptyStub<24>").location, EmptyStub<24>);
				Red(scan.ScanSingle("48 81 EC C8 00 00 00 4C 8B 9C", "EmptyStub<25>").location, EmptyStub<25>);


				Red(scan.ScanSingle("48 89 5C 24 10 57 48 83 EC 20 48 8B 11 48 8B F9 B9 16", "DB_AddAssetCustom<IW8H_ASSET_SOUNDBANK>").location, DB_AddAssetCustom<IW8H_ASSET_SOUNDBANK>);
				Red(scan.ScanSingle("48 89 5C 24 10 57 48 83 EC 20 48 8B 11 48 8B F9 B9 17", "DB_AddAssetCustom<IW8H_ASSET_SOUNDBANKTRANSIENT>").location, DB_AddAssetCustom<IW8H_ASSET_SOUNDBANKTRANSIENT>);
				Red(scan.ScanSingle("40 53 48 83 EC 20 48 8B 01 48 8D 54 24 30 48 8B D9 48 89 44 24 30 B9 09", "DB_AddAssetCustom<IW8H_ASSET_XMODEL>").location, DB_AddAssetCustom<IW8H_ASSET_XMODEL>);
				Red(scan.ScanSingle("40 53 48 83 EC 20 48 8B 01 48 8D 54 24 30 48 8B D9 48 89 44 24 30 B9 0C", "DB_AddAssetCustom<IW8H_ASSET_COMPUTESHADER>").location, DB_AddAssetCustom<IW8H_ASSET_COMPUTESHADER>);
				Red(scan.ScanSingle("40 53 48 83 EC 20 48 8B 01 48 8D 54 24 30 48 8B D9 48 89 44 24 30 B9 0D", "DB_AddAssetCustom<IW8H_ASSET_LIBSHADER>").location, DB_AddAssetCustom<IW8H_ASSET_LIBSHADER>);
				Red(scan.ScanSingle("40 53 48 83 EC 20 48 8B 01 48 8D 54 24 30 48 8B D9 48 89 44 24 30 B9 45", "DB_AddAssetCustom<IW8H_ASSET_STREAMINGINFO>").location, DB_AddAssetCustom<IW8H_ASSET_STREAMINGINFO>);
				Red(scan.ScanSingle("40 53 48 83 EC 20 48 8B 01 48 8D 54 24 30 48 8B D9 48 89 44 24 30 B9 7B", "DB_AddAssetCustom<IW8H_ASSET_DLOGSCHEMA>").location, DB_AddAssetCustom<IW8H_ASSET_DLOGSCHEMA>);


				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}

				for (auto& [hashType, worker] : GetWorkers()) {
					HandlerAssetType type{ GetExePoolId(hashType) };
					if (type == gcx.assetNames.InvalidId()) {
						LOG_ERROR("type {} was removed", PoolName(hashType));
					}
					else {
						size_t trueLen{ gcx.DB_GetAssetSize(type) };
						if (worker->assetSize != trueLen) {
							LOG_WARNING("type {} doesn't have the expected size: acts:0x{:x} != exe:0x{:x}", PoolName(hashType), worker->assetSize, trueLen);
						}
					}
					worker->GenDefaultXHashes(nullptr);
				}

				if (opt.dumpXStrings) {
					std::filesystem::path path{ opt.m_output / gamePath / "xstrings_all.wni" };
					gcx.xstrOutGlb = std::make_unique<XStringOutCTX>(path);
				}
				gcx.currentAsset = std::string::npos;
				static std::once_flag of{};
				std::call_once(of, [] {
					hook::error::AddErrorDumper([]() {
						if (gcx.currentAsset != std::string::npos) {
							LOG_ERROR("Current asset: {}/{} {}", gcx.currentAsset, gcx.assets.assetsCount, gcx.assetNames.GetTypeName(gcx.assets.assets[gcx.currentAsset].type));
							if (gcx.reader) {
								LOG_ERROR("Offset: 0x{:x} -> 0x{:x}", gcx.reader->Loc(), gcx.reader->Remaining());
							}
						}
					});

					});
			}
			void Cleanup() override {
				gcx.namesStore.WarnMissings();
				if (gcx.xstrOutGlb) {
					LOG_INFO("Dump xstrings to {}", gcx.xstrOutGlb->path.string());
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
					gcx.rewindBuffer = std::make_unique<RewindStreamData[]>(ctx.blocksCount);
				}
				if (newSizes || !gcx.blocks) {
					gcx.blocks = std::make_unique<XBlock[]>(ctx.blocksCount);
				}
				gcx.lastBlockCount = ctx.blocksCount;

				// setup rewind buffers
				std::memset(gcx.rewindBuffer.get(), 0, sizeof(gcx.rewindBuffer[0]) * ctx.blocksCount);
				LOG_DEBUG("rewing: {}", (void*)gcx.rewindBuffer.get());
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
				}

				gcx.DB_InitStreams(gcx.blocks.get());
				*gcx.s_rewindPostload = 1;
				gcx.DB_PatchMem_BeginLoad();

				LoadXFileData(&gcx.assets, sizeof(gcx.assets));

				LOG_DEBUG("assets: {}, strings: {}", gcx.assets.assetsCount, gcx.assets.stringsCount);


				std::vector<const char*> xstringLocs{};
				if (opt.dumpXStrings) {
					gcx.xstringLocs = &xstringLocs;
				}

				std::string fftype{ ctx.ffname };
				size_t fdd{ fftype.find('_') };
				if (fdd != std::string::npos) {
					fftype.resize(fdd);
				}

				for (auto& [hashType, worker] : GetWorkers()) {
					worker->GenDefaultXHashes(&ctx);
				}

				std::filesystem::path outStrings{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "strings" / fftype / std::format("{}.txt", ctx.ffname) };
				gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
				{
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE stringsOs{ outStrings };
					gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
					*gcx.loadStringList = &gcx.assets;
					gcx.Load_ScriptStringList(true);
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

				LOG_INFO("String dump into {} ({})", outStrings.string(), gcx.assets.stringsCount);
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

				if (!gcx.assets.assetsCount) {
					LOG_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / gamePath / "source" / "tables" / "data" / "assets" / fftype / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					gcx.outAsset = &assetsOs;
					assetsOs << "type,name";
					gcx.DB_PatchMem_FixStreamAlignment(7);
					gcx.assets.assets = (Asset*)*gcx.streamPos;
					LoadStreamTA(false, gcx.assets.assets, sizeof(gcx.assets.assets[0]) * gcx.assets.assetsCount);

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
							/*
							if (assetId == 13012) {
								core::logs::setlevel(core::logs::LVL_TRACE_PATH);
								core::logs::setbasiclog(false);
							}
							//*/
							LOG_TRACE("load #{} -> {}({}/0x{:x}) {}", assetId, PoolName(GetHashType(asset->type)), (int)asset->type, (int)asset->type, asset->handle);
							*gcx.loadAsset = asset;
							gcx.currentAsset = assetId;
							gcx.Load_Asset(true);
							gcx.currentAsset = std::string::npos;
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
				}
				gcx.DB_PopStreamPos();
				LOG_INFO("Asset names dump into {}", outAssets.string());
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
						}
					}
					LOG_INFO("XStrings names dump into {}", ostr.string());
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

	XString GetXAssetName(HandlerHashedAssetType htype, void* handle) {
		HandlerAssetType type{ GetExePoolId(htype) };
		if (type == gcx.assetNames.InvalidId()) {
			throw std::runtime_error(std::format("INVALID HASH TYPE ID {}", PoolName(htype)));
		}
		return ValidateName(*(XString*)handle);
	}

	XString ValidateName(XString name) {
		if (*name == ',') {
			name++;
		}
		return name;
	}
}
