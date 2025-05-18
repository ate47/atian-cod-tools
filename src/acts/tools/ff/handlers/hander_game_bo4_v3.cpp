#include <includes.hpp>
#include <utils/enumlist.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <games/bo4/pool.hpp>
#include <decryptutils.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>


namespace fastfile::handlers::bo4 {
	namespace {
		using namespace games::bo4::pool;
		using namespace fastfile::handlers::bo4;

		class BO4FFHandler;

		struct XBlock {
			byte* data;
			uint64_t size;
			uint64_t unk10;
		};


		struct {
			BO4FFHandler* handler{};
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
			XBlock* g_load_blocks{};

			fastfile::FastFileOption* opt{};
			std::vector<char*> xstrings{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[XAssetType::ASSET_TYPE_COUNT]{};
			XAssetList_0 assetList{};
			utils::EnumList<XAssetType, XAssetType::ASSET_TYPE_COUNT> handleList{ games::bo4::pool::XAssetIdFromName };
		} gcx{};

		const char* XBlockLocPtr(void* ptr) {
			byte* p{ (byte*)ptr };
			for (size_t i = 0; i < XFILE_BLOCK_COUNT2; i++) {
				if (p >= gcx.ctx->blockSizes[i].data && p < gcx.ctx->blockSizes[i].data + gcx.ctx->blockSizes[i].size) {
					size_t rloc{ (size_t)(p - gcx.ctx->blockSizes[i].data) };
					return utils::va("%s+0x%llx/0x%llx", XFileBlockName((XFileBlock)i), rloc, gcx.ctx->blockSizes[i].size);
				}
			}
			return utils::va("%p", ptr);
		}
		void AssertCanWrite(void* ptr, size_t len) {
			byte* p{ (byte*)ptr };
			for (size_t i = 0; i < XFILE_BLOCK_COUNT2; i++) {
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

		constexpr bool IsTemp(XFileBlock blk) {
			return blk == XFILE_BLOCK_TEMP || blk == XFILE_BLOCK_TEMP_PRELOAD;
		}

		constexpr bool IsStreamOrMapped(XFileBlock blk) {
			return blk == XFILE_BLOCK_MEMMAPPED || blk == XFILE_BLOCK_MEMMAPPED2 || blk == XFILE_BLOCK_STREAMER || blk == XFILE_BLOCK_STREAMER_CPU;
		}


		template<typename T = void, size_t align = sizeof(T) - 1>
		T* AllocStreamPos() {
			return (T*)gcx.DB_AllocStreamPos(align);
		}

		void DB_LoadXFileData(void* pos, size_t size) {
			//LOG_TRACE("{} DB_LoadXFileData({}, 0x{:x}) -> {}/0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr(pos), size);
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
			LOG_TRACE("{} Load_XStringCustom({}, 0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr((void*)str), size, gcx.reader->Loc(), acts::decryptutils::DecryptStringT8(ptr));
			AssertCanWrite(*str, size + 1);
			std::memcpy(*str, ptr, size + 1);
			if (gcx.opt->dumpXStrings) {
				gcx.xstrings.push_back(*str);
			}
			gcx.DB_IncStreamPos(size + 1);
		}

		XAsset_0* DB_LinkXAssetEntry(XAsset_0* xasset, bool allowOverride) {
			gcx.loaded++;

			if (gcx.opt->assertContainer) {
				// todo: add header name
				//bo4FFHandlerContext.pool.AddAssetHeader(xasset->type, 0, xasset->header);
			}

			XHash* hash{ xasset->header ? games::bo4::pool::GetAssetName(xasset->type, xasset->header, 0) : nullptr };

			//const char* assetName{ hash ? hashutils::ExtractTmp("hash", hash->hash) : "<unknown>" };
			//*bo4FFHandlerContext.osassets << "\n" << XAssetNameFromId(xasset->type) << "," << assetName;
			int assetSize{ gcx.DB_GetXAssetTypeSize(xasset->type) };
			void* baseHeader{ xasset->header };
			void* linkedHeader{ gcx.allocator.Alloc(assetSize) };

			if (xasset->header) {
				std::memcpy(linkedHeader, xasset->header, assetSize);

				xasset->header = linkedHeader;
			}

			if (hash && hash->name) {
				gcx.opt->AddAssetName(xasset->type, hash->name);

				gcx.linkedAssets[xasset->type][hash->name] = xasset->header;
			}
			LOG_DEBUG("Loading asset {}/{} -> {}/{}", XAssetNameFromId(xasset->type), hashutils::ExtractTmp("hash", hash->name), xasset->header, XBlockLocPtr(baseHeader));

			if (gcx.opt->noAssetDump || (!gcx.handleList.Empty() && !gcx.handleList[xasset->type])) return xasset; // ignore
			if (xasset->header) {
				auto& workers{ fastfile::handlers::bo4::GetWorkers() };
				auto it{ workers.find(xasset->type) };
				if (it != workers.end()) {
					try {
						if (!it->second->ignoreFull || !gcx.handleList.Empty()) {
							it->second->Unlink(*gcx.opt, xasset->header);
						}
					}
					catch (std::runtime_error& e) {
						LOG_ERROR("Can't dump asset asset {}/{}: {}", XAssetNameFromId(xasset->type), (void*)xasset->header, e.what());
					}
				}
			}
			return xasset;
		}

		void* DB_FindXAssetHeader(XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
			if (!name || name->name) return nullptr;
			auto it{ gcx.linkedAssets[type].find(name->name) };

			if (it != gcx.linkedAssets[type].end()) {
				return it->second;
			}

			LOG_WARNING("MISSING XASSET {} : {}", XAssetNameFromId(type), hashutils::ExtractTmp("hash", name->name));

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

		class BO4FFHandler : public fastfile::FFHandler {
		public:
			BO4FFHandler() : fastfile::FFHandler("bo4", "Black Ops 4") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true) };

				gcx.handleList.Clear();
				if (opt.assetTypes) {
					gcx.handleList.LoadConfig(opt.assetTypes);
				}

				gcx.opt = &opt;

				gcx.Load_XAsset = reinterpret_cast<decltype(gcx.Load_XAsset)>(lib[0x2E35D10]);
				gcx.DB_GetXAssetTypeSize = reinterpret_cast<decltype(gcx.DB_GetXAssetTypeSize)>(lib[0x28AABA0]);

				gcx.Load_Stream = reinterpret_cast<decltype(gcx.Load_Stream)>(lib[0x2EBC050]);
				gcx.DB_PopStreamPos = reinterpret_cast<decltype(gcx.DB_PopStreamPos)>(lib[0x2EBBE20]);
				gcx.DB_PushStreamPos = reinterpret_cast<decltype(gcx.DB_PushStreamPos)>(lib[0x2EBBEA0]);
				gcx.DB_IncStreamPos = reinterpret_cast<decltype(gcx.DB_IncStreamPos)>(lib[0x2EBBBE0]);
				gcx.DB_AllocStreamPos = reinterpret_cast<decltype(gcx.DB_AllocStreamPos)>(lib[0x2EBBBA0]);
				gcx.DB_ConvertOffsetToPointer = reinterpret_cast<decltype(gcx.DB_ConvertOffsetToPointer)>(lib[0x2EBBFF0]);
				gcx.DB_AllocXBlocks = reinterpret_cast<decltype(gcx.DB_AllocXBlocks)>(lib[0x2EB5870]);
				gcx.DB_InitStreams = reinterpret_cast<decltype(gcx.DB_InitStreams)>(lib[0x2EBBBF0]);
				gcx.g_load_blocks = reinterpret_cast<decltype(gcx.g_load_blocks)>(lib[0xA0F3B10 + 0x20]);

				hook::memory::RedirectJmp(lib[0x2E0E130], DB_LoadXFileData);
				hook::memory::RedirectJmp(lib[0x2EBC110], Load_XStringCustom);
				hook::memory::RedirectJmp(lib[0x2EB84F0], DB_LinkXAssetEntry);
				hook::memory::RedirectJmp(lib[0x2EB75B0], DB_FindXAssetHeader);
				hook::memory::RedirectJmp(lib[0x3C41C30], PMem_Alloc);

				RemoveStub<0x2EBC480>(lib); // Load_ScriptStringCustom1
				RemoveStub<0x2EBC430>(lib); // Load_ScriptStringCustom2
				RemoveStub<0x2EBC540>(lib); // Load_ScriptStringCustom3
				RemoveStub<0x22B7AF0>(lib); // unk
				RemoveStub<0x22B7B00>(lib); // unk
				RemoveStub<0x2EBBDC0, true>(lib); // not sure
				RemoveStub<0x2EBBF40, true>(lib); // not sure
				RemoveStub<0x288B110, true>(lib); // Com_Error_
				RemoveStub<0x35BA450>(lib); // Load_GfxImageAdapter
				RemoveStub<0x3CA8870>(lib); // Load_SndBankAsset
				RemoveStub<0x3CA88C0>(lib); // Load_SndBankAsset
				RemoveStub<0x35FC100>(lib); // unk
				RemoveStub<0x3733C90>(lib); // unk
				RemoveStub<0x35FB980>(lib); // load texture
				RemoveStub<0x35FC060>(lib); // unk
				RemoveStub<0x35FBFC0>(lib); // unk
				RemoveStub<0x35FBDD0>(lib); // unk
				RemoveStub<0x3600EA0>(lib); // unk
				RemoveStub<0x370BD10>(lib); // unk
				RemoveStub<0x353AE00>(lib); // unk
				RemoveStub<0x3700CE0>(lib); // unk
				RemoveStub<0x3DC46F0>(lib); // unk
				RemoveStub<0x3CBBE00>(lib); // unk
				RemoveStub<0x353AF10>(lib); // unk
				RemoveStub<0x353ADD0>(lib); // unk
				RemoveStub<0x3729A70>(lib); // unk
				RemoveStub<0x35FBC90>(lib); // unk
				RemoveStub<0x35FBF20>(lib); // unk
				RemoveStub<0x35FBE80>(lib); // unk
				RemoveStub<0x35FBD30>(lib); // unk
				RemoveStub<0x35FCE50>(lib); // unk

			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				std::filesystem::path out{ opt.m_output / "bo4" / "data" };
				std::filesystem::create_directories(out);


				gcx.loaded = 0;
				gcx.allocator.FreeAll();
				gcx.linkedAssets->clear();

				size_t blockSizes[XFILE_BLOCK_COUNT]{};

				// alloc the blocks
				for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
					blockSizes[i] = ctx.blockSizes[i].size;
					LOG_DEBUG("Block size {} = 0x{:x}", XFileBlockName((XFileBlock)i), blockSizes[i]);
				}

				gcx.DB_AllocXBlocks(blockSizes, ctx.ffname, ctx.ffname, gcx.g_load_blocks, 0, nullptr);
				gcx.DB_InitStreams(gcx.g_load_blocks);

				gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
				gcx.xstrings.clear();

				XAssetList_0& assetList{ gcx.assetList };
				DB_LoadXFileData(&assetList, sizeof(assetList));
				SetAssetList(&assetList);
				gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);

				gcx.opt->assetNames.clear();

				if (assetList.strings) {
					assetList.strings = AllocStreamPos<char*>();

					LOG_TRACE("Log strings... {}", (void*)assetList.strings);
					std::filesystem::path outStrings{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };
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
								}
								else {
									gcx.DB_ConvertOffsetToPointer((void**)&assetList.strings[i]);
								}

								char* scrstr{ acts::decryptutils::DecryptStringT8(assetList.strings[i]) };
								hashutils::AddPrecomputed(hash::Hash64(scrstr), scrstr, true);
								os << scrstr << "\n";
							}
						}
						LOG_INFO("Dump strings into {}", outStrings.string());
					}
				}

				gcx.DB_PopStreamPos();

				if (assetList.assets) {
					assetList.assets = AllocStreamPos<XAsset_0, 7>();
					gcx.Load_Stream(true, assetList.assets, sizeof(*assetList.assets) * assetList.assetCount);

					if (!opt.noAssetDump) {
						for (auto& [t, w] : GetWorkers()) {
							w->PreXFileLoading(opt, ctx);
						}
					}

					for (size_t i = 0; i < assetList.assetCount; i++) {
						const char* assType{ XAssetNameFromId(assetList.assets[i].type) };
						LOG_DEBUG("{}/{} Load asset {} (0x{:x}) {}", i, assetList.assetCount, assType, (int)assetList.assets[i].type, assetList.assets[i].header);
						gcx.Load_XAsset(false, &assetList.assets[i]);
					}

					if (gcx.opt->dumpAssetNames) {
						std::filesystem::path outAssets{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname)};
						{
							std::filesystem::create_directories(outAssets.parent_path());
							utils::OutFileCE osa{ outAssets };
							osa << "type,name";

							size_t n{ std::min<size_t>(gcx.opt->assetNames.size(), games::bo4::pool::ASSET_TYPE_COUNT) };

							for (size_t i = 0; i < n; i++) {
								for (uint64_t h : gcx.opt->assetNames[i]) {
									osa << "\n" << XAssetNameFromId((games::bo4::pool::XAssetType)i) << ",#" << hashutils::ExtractTmp("hash", h);
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
						std::filesystem::path outStrings{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "xstrings" / std::format("{}.txt", ctx.ffname) };
						std::filesystem::create_directories(outStrings.parent_path());
						utils::OutFileCE os{ outStrings, true };


						for (char* xstr : gcx.xstrings) {
							os << acts::decryptutils::DecryptStringT8(xstr) << "\n";
						}
						LOG_INFO("Dump xstrings into {}", outStrings.string());
					}
				}

				gcx.DB_PopStreamPos();
				LOG_INFO("Loaded {} asset(s)", gcx.loaded);
			}
		};

		utils::ArrayAdder<BO4FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
}