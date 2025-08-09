#include <includes.hpp>
#include <utils/enumlist.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/scan_container.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <decryptutils.hpp>
#include <tools/ff/handlers/handler_game_cw.hpp>


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
			//CWXHash (*DB_GetXAssetName)(XAssetType type, const void* header) {};
			char* (*DecryptString)(char*) {};
			void* (*DB_AllocStreamPos)(int alignement) {};
			void (*DB_PushStreamPos)(XFileBlock type) {};
			void (*DB_IncStreamPos)(size_t size) {};
			void (*DB_PopStreamPos)() {};
			bool (*Load_Stream)(bool atStreamStart, void* ptr, size_t size) {};
			void (*DB_ConvertOffsetToPointer)(void** data) {};
			void (*DB_AllocXBlocks)(size_t* blockSize, const char* filename, const char* filename2, XBlock* blocks, int side, int* loaded);
			void (*DB_InitStreams)(XBlock* blocks);
			XBlock loadBlocks[XFileBlock::XFILE_BLOCK_COUNT];

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
		return gcx.DecryptString(c);
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
			size_t size;
			char* ptr{ gcx.reader->ReadString(&size) };
			//LOG_TRACE("{} Load_XStringCustom({}, 0x{:x}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr((void*)str), size, gcx.reader->Loc());
			//LOG_TRACE("- {}", utils::data::AsHex(ptr, size + 1));
			AssertCanWrite(*str, size + 1);
			std::memcpy(*str, ptr, size + 1);
			if (gcx.opt->dumpXStrings) {
				gcx.xstrings.push_back(*str);
			}
			hashutils::AddPrecomputed(hash::Hash64(*str), *str);
			gcx.DB_IncStreamPos(size + 1);
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
			if (hash && gcx.opt->dumpAssetNames) {
				if (gcx.opt->assetNames.size() < type) {
					gcx.opt->assetNames.resize(type + 10);
				}
				gcx.opt->assetNames[type].insert((uint64_t)hash);
			}

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

		template<size_t off, bool error = false>
		void RemoveStub(hook::library::Library& lib) {
			if constexpr (error) {
				hook::memory::RedirectJmp(lib[off], &ErrorStub<off>);
			}
			else {
				hook::memory::RedirectJmp(lib[off], &EmptyStub<off>);
			}
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
				hook::library::Library lib{ opt.GetGame(true) };
				hook::scan_container::ScanContainer scan{ lib, true };
				scan.Sync();

				gcx.handleList.Clear();
				if (opt.assetTypes) {
					gcx.handleList.LoadConfig(opt.assetTypes);
				}

				//if (!acts::decryptutils::LoadDecryptModule(lib)) {
				//	throw std::runtime_error("Can't load decrypt module");
				//}

				gcx.opt = &opt;

				bool err{};
#define __GCX_LOAD_PATTERN(_func, _pattern) try { _func = scan.ScanSingle(_pattern, #_func).GetPtr<decltype(_func)>(); } catch (std::exception& e) { LOG_ERROR("{}", e.what()); err = true; }
				gcx.Load_XAsset = scan.ScanSingle("E8 ? ? ? ? 48 83 C3 ? 8D 4E ? E8 ? ? ? ? 8B F1 84 C0 75 06", "gcx.Load_XAsset").GetRelative<int32_t, decltype(gcx.Load_XAsset)>(1);
				__GCX_LOAD_PATTERN(gcx.DB_GetXAssetTypeSize, "0F B6 C1 48 8D 0C 80 48 8D 05 ? ? ? ? 8B");
				//__GCX_LOAD_PATTERN(gcx.DB_GetXAssetName, "0F B6 C1 48 8D 0C 80 48 8D 05 ? ? ? ? 48 8B");
				
				__GCX_LOAD_PATTERN(gcx.DecryptString, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6");

				__GCX_LOAD_PATTERN(gcx.Load_Stream, "48 89 5C 24 ? 57 48 83 EC ? 49 8B D8 4C 8B CA 40 B7 ? 84 C9 74 46 48 85 DB 74 41 48 63 05 ? ? ? ? 83 F8 ? 77 2D 48 8D 15 71");
				__GCX_LOAD_PATTERN(gcx.DB_PopStreamPos, "44 8B 05 ? ? ? ? 4C 8D 0D ? ? ? ? 48 63");
				__GCX_LOAD_PATTERN(gcx.DB_PushStreamPos, "40 53 48 83 EC 20 8B D9 83 F9 09");
				__GCX_LOAD_PATTERN(gcx.DB_IncStreamPos, "48 01 0D ? ? ? ? C3");
				__GCX_LOAD_PATTERN(gcx.DB_AllocStreamPos, "4C 8B 05 ? ? ? ? 48 63 D1 48");
				__GCX_LOAD_PATTERN(gcx.DB_ConvertOffsetToPointer, "4C 8B 01 49 FF C8 49 8B C0 48 C1 E8 ? 48 8D 14 40 48 B8 FF FF FF FF FF FF FF 0F 4C 23 C0 48 8B 05 ? ? ? ? 4C");
				__GCX_LOAD_PATTERN(gcx.DB_AllocXBlocks, "40 55 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B");
				__GCX_LOAD_PATTERN(gcx.DB_InitStreams, "33 D2 48 89 0D ? ? ? ? 48");

				hook::memory::RedirectJmp(scan.ScanSingle("85 D2 0F 8E 45 01 00 00 41", "DB_LoadXFileData").location, DB_LoadXFileData);
				hook::memory::RedirectJmp(scan.ScanSingle("E8 ? ? ? ? EB 08 48 8B CF E8 ? ? ? ? 33 C9", "Load_XStringCustom").GetRelative<int32_t>(1), Load_XStringCustom);
				hook::memory::RedirectJmp(scan.ScanSingle("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B EA", "DB_LinkXAssetEntry").location, DB_LinkXAssetEntry);
				hook::memory::RedirectJmp(scan.ScanSingle("48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D AC 24 80 D0", "DB_FindXAssetHeader").location, DB_FindXAssetHeader);
				hook::memory::RedirectJmp(scan.ScanSingle("44 89 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 57", "PMem_Alloc").location, PMem_Alloc);

				hook::memory::Nulled(scan.ScanSingle("E8 ? ? ? ? 48 8B 03 48 85 C0 74 2B", "Load_ScriptStringCustom1").GetRelative<int32_t, void*>(1));
				hook::memory::Nulled(scan.ScanSingle("E8 ? ? ? ? 48 8B 43 ? 48 85 C0 74 54", "Load_ScriptStringCustom2").GetRelative<int32_t, void*>(1));
				hook::memory::Nulled(scan.ScanSingle("E8 ? ? ? ? EB 09 48 8D 4B ? E8 ? ? ? ? 48 83 7B ? ? 0F 84 DA 00 00 00").GetRelative<int32_t, void*>(1));

				hook::memory::Nulled(scan.ScanSingle("48 83 EC ? 48 8B 01 4C 8B D2").location);
				hook::memory::Nulled(scan.ScanSingle("40 53 48 83 EC ? 48 8D 4A").location);
				hook::memory::Nulled(scan.ScanSingle("40 53 48 83 EC ? 48 8B 19 41").location);
				hook::memory::Nulled(scan.ScanSingle("48 83 EC ? 48 85 D2 74 30").location);
				hook::memory::Nulled(scan.ScanSingle("48 85 D2 0F 84 81 00 00 00 48 89").location);
				hook::memory::Nulled(scan.ScanSingle("48 85 D2 0F 84 9A 00").location);
				hook::memory::Nulled(scan.ScanSingle("48 85 C9 0F 84 26 02 00 00 48").location);
				hook::memory::Nulled(scan.ScanSingle("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 35 ? ? ? ? 33").location);
				hook::memory::Nulled(scan.ScanSingle("48 8B 81 ? ? ? ? 48 85 C0 74 10 48").location);
				hook::memory::Nulled(scan.ScanSingle("48 85 C9 0F 84 84 00 00 00 53").location);
				hook::memory::Nulled(scan.ScanSingle("40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 84 C0 74 1F BA").location);
				hook::memory::Nulled(scan.ScanSingle("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 4C 8B 29").location);
				hook::memory::Nulled(scan.ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 83 EC ? 4C 8B 35").location);
				hook::memory::Nulled(scan.ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 49").location);
				hook::memory::Nulled(scan.ScanSingle("E8 ? ? ? ? 48 83 C6 ? 48 83 C3 ? 49 83 EE ? 75 BA").GetRelative<int32_t, void*>(1));
				hook::memory::Nulled(scan.ScanSingle("E8 ? ? ? ? 48 83 EF 80 48 83 EB 80 48 83 EE ? 75 BA").GetRelative<int32_t, void*>(1));


				// the link functions are inlined for these assets, we need to patch them
				hook::memory::RedirectJmp(scan.ScanSingle("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 84 D2 75 0D 48 8B 11 B1 ? E8 ? ? ? ? 48 89 07 48 8B 3F 48 8D 9F ? ? ? ?", "DB_LinkWeapon").location, DB_LinkCustomAsset<ASSET_TYPE_WEAPON>);
				hook::memory::RedirectJmp(scan.ScanSingle("48 89 5C 24 ? 57 48 83 EC ? 48 89 6C 24 ? 48 8B D9", "DB_LinkAttachmentUnique").location, DB_LinkCustomAsset<ASSET_TYPE_ATTACHMENTUNIQUE>);
				hook::memory::RedirectJmp(scan.ScanSingle("48 89 5C 24 ? 57 48 83 EC ? 48 8B 11 48 8B F9 B1", "DB_LinkSoundBank").location, DB_LinkCustomAsset<ASSET_TYPE_SOUND_BANK>);
				hook::memory::RedirectJmp(scan.ScanSingle("E8 ? ? ? ? E8 ? ? ? ? 48 83 7B ? ? 74 37", "DB_LinkDestructibleDef").GetRelative<int32_t, void*>(1), DB_LinkCustomAsset<ASSET_TYPE_DESTRUCTIBLEDEF>);
				hook::memory::RedirectJmp(scan.ScanSingle("E8 ? ? ? ? E8 ? ? ? ? 48 8D 4B ? E8 ? ? ? ? 48 83 C3", "DB_LinkDynModel").GetRelative<int32_t, void*>(1), DB_LinkCustomAsset<ASSET_TYPE_DYNMODEL>);
				// fixme: find scan
				hook::memory::RedirectJmp(lib[0x9E1A420], DB_LinkCustomAsset<ASSET_TYPE_VEHICLE>);
				hook::memory::RedirectJmp(lib[0xBF4C710], DB_LinkCustomAsset<ASSET_TYPE_NAVMESH>);
				hook::memory::RedirectJmp(lib[0xC9695E0], DB_LinkCustomAsset<ASSET_TYPE_VEHICLEASSEMBLY>);
				hook::memory::RedirectJmp(lib[0xC7BBA30], DB_LinkCustomAsset<ASSET_TYPE_CLIP_MAP>);
				hook::memory::RedirectJmp(lib[0xC559210], DB_LinkCustomAsset<ASSET_TYPE_GFX_MAP>);
				
				
#undef __GCX_LOAD_PATTERN

				if (err) throw std::runtime_error("can't init");
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				std::filesystem::path out{ opt.m_output / "cw" / "data" };
				std::filesystem::create_directories(out);

				if (!reader.CanRead(sizeof(gcx.assetList))) {
					LOG_INFO("empty, ignored");
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
				//SetAssetList(&assetList);

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
									LOG_TRACE("- {}/{} \"{}\"", i, assetList.stringsCount, gcx.DecryptString(assetList.strings[i]));
								}
								else {
									gcx.DB_ConvertOffsetToPointer((void**)&assetList.strings[i]);
									LOG_TRACE("- {}/{} \"{}\" (offset)", i, assetList.stringsCount, gcx.DecryptString(assetList.strings[i]));
								}

								char* scrstr{ gcx.DecryptString(assetList.strings[i]) };
								hashutils::AddPrecomputed(hash::Hash64(scrstr), scrstr, true);
								os << scrstr << "\n";
							}
						}
						LOG_INFO("Dump {} (0x{:x}) strings into {}", assetList.stringsCount, assetList.stringsCount, outStrings.string());
					}
				}



				gcx.DB_PushStreamPos(XFILE_BLOCK_RUNTIME_VIRTUAL);
				if (assetList.unk10) {
					assetList.unk10 = AllocStreamPos<int>();
					gcx.Load_Stream(true, assetList.unk10, sizeof(*assetList.unk10) * assetList.stringsCount);
					LOG_DEBUG("Loaded {} ids", assetList.stringsCount);
				}

				gcx.DB_PopStreamPos();

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

					if (gcx.opt->dumpAssetNames) {
						std::filesystem::path outAssets{ gcx.opt->m_output / "cw" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname) };
						{
							std::filesystem::create_directories(outAssets.parent_path());
							utils::OutFileCE osa{ outAssets };
							osa << "type,name";

							size_t n{ std::min<size_t>(gcx.opt->assetNames.size(), XAssetType::ASSET_TYPE_COUNT) };

							for (size_t i = 0; i < n; i++) {
								for (uint64_t h : gcx.opt->assetNames[i]) {
									osa << "\n" << cw::PoolName((XAssetType)i) << ",#" << hashutils::ExtractTmp("hash", h);
								}
							}
						}
						LOG_INFO("Dumped assets into {}", outAssets.string());
					}
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
							os << gcx.DecryptString(xstr) << "\n";
						}
						LOG_INFO("Dump xstrings into {}", outStrings.string());
					}
				}

				gcx.DB_PopStreamPos();
				LOG_INFO("Loaded {} asset(s)", gcx.loaded);
			}
		};

		utils::ArrayAdder<BOCWFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
}