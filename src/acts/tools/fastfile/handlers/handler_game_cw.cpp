#include <includes.hpp>
#include <game_data.hpp>
#include <utils/enumlist.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/scan_container.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>


namespace fastfile::handlers::cw {
	namespace {
		using namespace fastfile::handlers::cw;

		class BOCWFFHandler;

		struct XBlock {
			byte* data;
			uint64_t size;
			uint64_t unk10;
		};

		struct XAssetRef {
			void* ptr;
		};

		struct {
			BOCWFFHandler* handler{};
			void (*Load_XAsset)(bool atStreamStart, XAsset_0* asset) {};
			int (*DB_GetXAssetTypeSize)(XAssetType type) {};
			void* (*DB_AllocStreamPos)(int alignement) {};
			void (*DB_PushStreamPos)(XFileBlock type) {};
			void (*DB_IncStreamPos)(size_t size) {};
			void (*DB_PopStreamPos)() {};
			bool (*Load_Stream)(bool atStreamStart, void* ptr, size_t size) {};
			void (*DB_ConvertOffsetToPointer)(void** data) {};
			void (*DB_AllocXBlocks)(size_t* blockSize, const char* filename, const char* filename2, XBlock* blocks, int side, int* loaded);
			void (*DB_InitStreams)(XBlock* blocks);
			XBlock loadBlocks[XFileBlock::XFILE_BLOCK_COUNT];
			utils::OutFileCE* outAsset{};

			fastfile::FastFileOption* opt{};
			std::vector<char*> xstrings{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[XAssetType::ASSET_TYPE_COUNT]{};
			XAssetList_0 assetList{};
			utils::EnumList<XAssetType, XAssetType::ASSET_TYPE_COUNT> handleList{ cw::PoolId };
		} gcx{};
	}



	const char* GetScrString(ScrString_t id) {
		if ((int)id >= gcx.assetList.stringsCount) {
			LOG_WARNING("Can't get scr string: {} >= {}", id.id, gcx.assetList.stringsCount);
			return utils::va("<invalid:0x%x>", id);
		}
		char* c{ gcx.assetList.strings[id] };
		if (!c) return "";
		if (!IsValidHandle(c)) return GetValidString(c);
		return acts::decryptutils::DecryptString(c);
	}

	const char* XFileBlockName(XFileBlock id) {
		static const char* names[]{
			"TEMP",
			"TEMP_PRELOAD",
			"RUNTIME_VIRTUAL",
			"RUNTIME_PHYSICAL",
			"VIRTUAL",
			"CPU_GPU_SHARE",
			"PHYSICAL",
			"STREAMER",
			"STREAMER_CPU",
			"MEMMAPPED",
			"LOAD_TIME",
			"TEMP_PRELOAD_SAVED_PTR",
			"MESH",
		};

		if (id >= ARRAYSIZE(names) || id < 0) {
			return "unknown";
		}
		return names[id];
	}

	bool IsValidHandle(const void* handle) {
		uintptr_t h{ (uintptr_t)handle };

		if (h < 0x1000000) return false;
		if (h >> 60) return false;
		return true; // maybe ok
	}

	const char* GetValidString(const char* handle, const char* defaultVal) {
		if (IsValidHandle(handle)) return handle;
		return defaultVal ? defaultVal : utils::va("<invalid:0x%llx>", (uintptr_t)handle);
	}

	std::unordered_map<XAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<XAssetType, Worker*> map{};
		return map;
	}

	std::ostream& operator<<(std::ostream& os, const fastfile::handlers::cw::ScrString_t& scr) {
		return os << scr.id;
	}

	namespace {

		const char* XBlockLocPtr(void* ptr) {
			byte* p{ (byte*)ptr };
			for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
				if (p >= gcx.ctx->blockSizes[i].data && p < gcx.ctx->blockSizes[i].data + gcx.ctx->blockSizes[i].size) {
					size_t rloc{ (size_t)(p - gcx.ctx->blockSizes[i].data) };
					return utils::va("%s+0x%llx/0x%llx", XFileBlockName((XFileBlock)i), rloc, gcx.ctx->blockSizes[i].size);
				}
			}
			return utils::va("%p", ptr);
		}
		void AssertCanWrite(void* ptr, size_t len) {
			byte* p{ (byte*)ptr };
			for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
				if (p >= gcx.ctx->blockSizes[i].data && p < gcx.ctx->blockSizes[i].data + gcx.ctx->blockSizes[i].size) {
					size_t rloc{ (size_t)(p - gcx.ctx->blockSizes[i].data) };
					if (rloc + len > gcx.ctx->blockSizes[i].size) {
						hook::error::DumpStackTraceFrom();
						throw std::runtime_error(std::format("AssertCanWrite: Can't read 0x{:x}, remaining 0x{:x} at {} ({} rloc:0x{:x})", len, gcx.ctx->blockSizes[i].size - rloc, ptr, XFileBlockName((XFileBlock)i), rloc));
					}
					break;
				}
			}
		}

		template<typename T = void, size_t align = sizeof(T)>
		T* AllocStreamPos() {
			return (T*)gcx.DB_AllocStreamPos(align);
		}

		void DB_LoadXFileData(void* pos, size_t size) {
			LOG_TRACE("{} DB_LoadXFileData({}, 0x{:x})", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr(pos), size);
			if (!gcx.reader->CanRead(size)) {
				hook::error::DumpStackTraceFrom();
			}
			byte* p;
			if (pos) {
				AssertCanWrite(pos, size);
				gcx.reader->Read(pos, size);
				p = (byte*)pos;
			}
			else {
				p = gcx.reader->ReadPtr<byte>(size);
			}

			size_t s{ std::min<size_t>(size, 0x40 * 8) };
			while (s > 0) {
				size_t ss{ std::min<size_t>(s, 0x40) };
				LOG_TRACE("{:03x} : {}", p - (byte*)pos, utils::data::AsHex(p, ss));
				p += ss;
				s -= ss;
			}
		}

		void Load_XStringCustom(char** str) {
			size_t skip;
			char* ptr{ acts::decryptutils::DecryptString(gcx.reader->ReadString(&skip)) };
			//LOG_TRACE("{} Load_XStringCustom({}, 0x{:x}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr((void*)str), size, gcx.reader->Loc());
			//LOG_TRACE("- {}", utils::data::AsHex(ptr, size + 1));
			// write decrypted string
			size_t size{ std::strlen(ptr) };
			AssertCanWrite(*str, size + 1);
			std::memcpy(*str, ptr, size + 1);
			if (gcx.opt->dumpXStrings) {
				gcx.xstrings.push_back(*str);
			}
			hashutils::AddPrecomputed(hash::Hash64(ptr), ptr, true);
			gcx.DB_IncStreamPos(skip + 1);
		}

		XAssetRef* DB_LinkXAssetEntry(XAssetType type, void* header) {
			gcx.loaded++;

			if (gcx.opt->assertContainer) {
				// todo: add header name
				//bo4FFHandlerContext.pool.AddAssetHeader(xasset->type, 0, xasset->header);
			}

			int assetSize{ gcx.DB_GetXAssetTypeSize(type) };
			CWXHash* phash{ header ? cw::GetAssetName(type, header, assetSize) : nullptr };
			CWXHash hash{ phash ? *phash : 0 };
			*(gcx.outAsset) << "\n" << cw::PoolName(type) << ",#" << hashutils::ExtractTmp("hash", hash);

			//const char* assetName{ hash ? hashutils::ExtractTmp("hash", hash->hash) : "<unknown>" };
			//*bo4FFHandlerContext.osassets << "\n" << XAssetNameFromId(xasset->type) << "," << assetName;
			void* baseHeader{ header };
			XAssetRef* ref{ gcx.allocator.Alloc<XAssetRef>() };
			ref->ptr = gcx.allocator.Alloc(assetSize);

			if (header) {
				std::memcpy(ref->ptr, header, assetSize);

				header = ref->ptr;
			}

			//if (hash && hash->name) {
			//	gcx.opt->AddAssetName(xasset->type, hash->name);
			//
			//	gcx.linkedAssets[xasset->type][hash->name] = xasset->header;
			//}
			LOG_DEBUG("Loading asset {}/{} -> {}/{}", 
				(int)type, 
				hashutils::ExtractTmp("hash", hash),
				header, 
				XBlockLocPtr(baseHeader)
			);

			if (gcx.opt->noAssetDump || (!gcx.handleList.Empty() && !gcx.handleList[type])) return ref; // ignore
			if (header) {
				auto& workers{ GetWorkers() };
				auto it{ workers.find(type) };
				if (it != workers.end()) {
					try {
						if (!it->second->ignoreFull || !gcx.handleList.Empty()) {
							it->second->Unlink(*gcx.opt, header);
						}
					}
					catch (std::runtime_error& e) {
						LOG_ERROR("Can't dump asset asset {}/{}: {}", PoolName(type), (void*)header, e.what());
					}
				}
			}
			return ref;
		}

		void* DB_FindXAssetHeader(XAssetType type, CWXHash name, bool errorIfMissing, int waitTime) {
			if (!name) return nullptr;
			auto it{ gcx.linkedAssets[type].find(name) };

			if (it != gcx.linkedAssets[type].end()) {
				return it->second;
			}

			LOG_WARNING("{} MISSING XASSET {} : {}", hook::library::CodePointer{ _ReturnAddress() }, PoolName(type), hashutils::ExtractTmp("hash", name));

			return nullptr;
		}

		template<size_t offset = 0>
		void EmptyStub() {
			LOG_TRACE("{} EmptyStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
		}
		template<size_t offset = 0>
		void ErrorStub() {
			hook::error::DumpStackTraceFrom();
			throw std::runtime_error(std::format("{} ErrorStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset));
		}

		int Return3Stub() {
			return 3;
		}

		void* PMem_Alloc(size_t size, size_t alignment, void* loaded, int pool, int stack) {
			static byte empty[1];
			if (!size) return &empty;
			LOG_TRACE("PMem_Alloc(0x{:x}, 0x{:x}, {}, {}, {})", size, alignment, loaded, pool, stack);
			if (pool == 2) return &empty; // no gpu streamer pool
			return utils::Aligned(gcx.allocator.Alloc<byte>(size + alignment - 1), alignment);
		}

		template<XAssetType type>
		void DB_LinkCustomAsset(void** asset) {
			*asset = DB_LinkXAssetEntry(type, *asset)->ptr;
		}

		class BOCWFFHandler : public fastfile::FFHandler {
		public:
			BOCWFFHandler() : fastfile::FFHandler("cw", "Black Ops Cold War") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				acts::game_data::GameData game{ "cw" };
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, game.GetModuleName(), "cw")};
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				game.SetScanContainer(&scan);

				gcx.handleList.Clear();
				if (opt.assetTypes) {
					gcx.handleList.LoadConfig(opt.assetTypes);
				}

				if (!acts::decryptutils::LoadDecryptModule(mod)) {
					throw std::runtime_error("Can't load decrypt module");
				}

				gcx.opt = &opt;

				scan.ignoreMissing = true;

				game.Get("Load_XAsset", &gcx.Load_XAsset);
				game.Get("DB_GetXAssetTypeSize", &gcx.DB_GetXAssetTypeSize);
				game.Get("Load_Stream", &gcx.Load_Stream);
				game.Get("DB_PopStreamPos", &gcx.DB_PopStreamPos);
				game.Get("DB_PushStreamPos", &gcx.DB_PushStreamPos);
				game.Get("DB_IncStreamPos", &gcx.DB_IncStreamPos);
				game.Get("DB_AllocStreamPos", &gcx.DB_AllocStreamPos);
				game.Get("DB_ConvertOffsetToPointer", &gcx.DB_ConvertOffsetToPointer);
				game.Get("DB_AllocXBlocks", &gcx.DB_AllocXBlocks);
				game.Get("DB_InitStreams", &gcx.DB_InitStreams);

				game.Redirect("DB_LoadXFileData", DB_LoadXFileData);
				game.Redirect("Load_XStringCustom", Load_XStringCustom);
				game.Redirect("DB_LinkXAssetEntry", DB_LinkXAssetEntry);
				game.Redirect("DB_FindXAssetHeader", DB_FindXAssetHeader);
				game.Redirect("PMem_Alloc", PMem_Alloc);
				game.Redirect("$UnkNavMesh", Return3Stub);

				// the link functions are inlined for these assets, we need to patch them
				game.Redirect("DB_LinkWeapon", DB_LinkCustomAsset<ASSET_TYPE_WEAPON>);
				game.Redirect("DB_LinkAttachmentUnique", DB_LinkCustomAsset<ASSET_TYPE_ATTACHMENTUNIQUE>);
				game.Redirect("DB_LinkSoundBank", DB_LinkCustomAsset<ASSET_TYPE_SOUND_BANK>);
				game.Redirect("DB_LinkDestructibleDef", DB_LinkCustomAsset<ASSET_TYPE_DESTRUCTIBLEDEF>);
				game.Redirect("DB_LinkDynModel", DB_LinkCustomAsset<ASSET_TYPE_DYNMODEL>);

				game.ApplyNullScans("fastfile");

				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				std::filesystem::path out{ opt.m_output / "cw" / "data" };
				std::filesystem::create_directories(out);

				if (!reader.CanRead(sizeof(gcx.assetList))) {
					LOG_OPT_INFO("empty, ignored");
					return;
				}

				gcx.loaded = 0;
				gcx.allocator.FreeAll();
				gcx.linkedAssets->clear();

				size_t blockSizes[XFILE_BLOCK_COUNT]{};

				// alloc the blocks
				for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
					blockSizes[i] = ctx.blockSizes[i].size;
					LOG_DEBUG("Block size {} = 0x{:x}", XFileBlockName((XFileBlock)i), blockSizes[i]);
				}

				gcx.DB_AllocXBlocks(blockSizes, ctx.ffname, ctx.ffname, gcx.loadBlocks, 0, nullptr);
				gcx.DB_InitStreams(gcx.loadBlocks);

				gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
				gcx.xstrings.clear();

				XAssetList_0& assetList{ gcx.assetList };
				DB_LoadXFileData(&assetList, sizeof(assetList));

				gcx.opt->assetNames.clear();

				hashutils::ReadDefaultFile(); // fix hashutils::Add in Load_String

				if (assetList.strings) {
					assetList.strings = AllocStreamPos<char*>();

					LOG_TRACE("Load {} (0x{:x})  strings... {}", assetList.stringsCount, assetList.stringsCount, (void*)assetList.strings);
					std::filesystem::path outStrings{ gcx.opt->m_output / "cw" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE os{ outStrings };
					if (!os) {
						throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
					}

					if (gcx.Load_Stream(true, assetList.strings, sizeof(*assetList.strings) * assetList.stringsCount)) {
						for (size_t i = 0; i < assetList.stringsCount; i++) {
							if (assetList.strings[i]) {
								if (assetList.strings[i] == (const char*)0xFFFFFFFFFFFFFFFF) {
									assetList.strings[i] = AllocStreamPos<char>();
									Load_XStringCustom(&assetList.strings[i]);
									LOG_TRACE("- {}/{} \"{}\"", i, assetList.stringsCount, assetList.strings[i]);
								}
								else {
									gcx.DB_ConvertOffsetToPointer((void**)&assetList.strings[i]);
									LOG_TRACE("- {}/{} \"{}\" (offset)", i, assetList.stringsCount, assetList.strings[i]);
								}

								char* scrstr{ assetList.strings[i] };
								hashutils::AddPrecomputed(hash::Hash64(scrstr), scrstr, true);
								os << scrstr << "\n";
							}
						}
						LOG_OPT_INFO("Dump {} (0x{:x}) strings into {}", assetList.stringsCount, assetList.stringsCount, outStrings.string());
					}
				}



				gcx.DB_PushStreamPos(XFILE_BLOCK_RUNTIME_VIRTUAL);
				if (assetList.unk10) {
					assetList.unk10 = AllocStreamPos<int>();
					gcx.Load_Stream(true, assetList.unk10, sizeof(*assetList.unk10) * assetList.stringsCount);
					LOG_DEBUG("Loaded {} ids", assetList.stringsCount);
				}

				gcx.DB_PopStreamPos();


				std::filesystem::path outAssets{ gcx.opt->m_output / "cw" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname) };
				std::filesystem::create_directories(outAssets.parent_path());
				utils::OutFileCE assetsOs{ outAssets };
				gcx.outAsset = &assetsOs;
				assetsOs << "type,name";

				if (assetList.assets) {
					assetList.assets = AllocStreamPos<XAsset_0, 8>();
					gcx.Load_Stream(true, assetList.assets, sizeof(*assetList.assets) * assetList.assetCount);

					if (!opt.noAssetDump) {
						for (auto& [t, w] : GetWorkers()) {
							w->PreXFileLoading(opt, ctx);
						}
					}

					for (size_t i = 0; i < assetList.assetCount; i++) {
						const char* assType{ cw::PoolName(assetList.assets[i].type) };
						LOG_DEBUG("{}/{} Load asset {} (0x{:x}) {}", i, assetList.assetCount, assType, (int)assetList.assets[i].type, assetList.assets[i].header);
						gcx.Load_XAsset(false, &assetList.assets[i]);
					}

					LOG_OPT_INFO("Dumped assets into {}", outAssets.string());
					if (!opt.noAssetDump) {
						for (auto& [t, w] : GetWorkers()) {
							w->PostXFileLoading(opt, ctx);
						}
					}

					// xstrings
					if (opt.dumpXStrings) {
						std::filesystem::path outStrings{ gcx.opt->m_output / "cw" / "source" / "tables" / "data" / "xstrings" / std::format("{}.txt", ctx.ffname) };
						std::filesystem::create_directories(outStrings.parent_path());
						utils::OutFileCE os{ outStrings, true };


						for (char* xstr : gcx.xstrings) {
							os << xstr << "\n";
						}
						LOG_OPT_INFO("Dump xstrings into {}", outStrings.string());
					}
				}

				gcx.DB_PopStreamPos();
				LOG_OPT_INFO("Loaded {} asset(s)", gcx.loaded);
			}
		};

		utils::ArrayAdder<BOCWFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
}