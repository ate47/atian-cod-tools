#include <includes.hpp>
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
			size_t remaining{};
			byte* pos{};
		};

		struct XBlockStackMember {
			XFileBlock type;
			byte* pos;
			size_t remaining;
		};

		struct {
			BO4FFHandler* handler{};
			void (*Load_XAsset)(bool atStreamStart, XAsset_0* asset) {};
			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			XBlock blocks[XFILE_BLOCK_COUNT2]{};
			size_t loaded{};
			std::stack<XBlockStackMember> blocksStack{};
			XAssetList_0 assetList{};
		} gcx{};

		void PrintIndexStack(core::logs::loglevel lvl) {
			if (HAS_LOG_LEVEL(lvl)) {
				std::stringstream ss{};

				for (const XBlockStackMember& blk : gcx.blocksStack._Get_container()) {
					ss << " -> " << XFileBlockName(blk.type) << "(0x" << std::hex << blk.remaining << ")";
				}

				LOG_LVLF(lvl, "Stack: {}", ss.str());
			}
		}

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

		void DB_PushStreamPos(XFileBlock type) {
			LOG_TRACE("{} DB_PushStreamPos({})", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(type));
			if (type > XFILE_BLOCK_MEMMAPPED) {
				type = XFILE_BLOCK_MEMMAPPED;
			}
			gcx.blocksStack.emplace(type, gcx.blocks[type].pos, gcx.blocks[type].remaining);
		}

		void DB_IncStreamPos(size_t size) {
			LOG_TRACE("{} DB_IncStreamPos(0x{:x})", hook::library::CodePointer{ _ReturnAddress() }, size);
			XFileBlock type{ gcx.blocksStack.top().type };
			if (IsStreamOrMapped(type)) return; // todo: handle xpak
			if (size <= gcx.blocks[type].remaining) {
				gcx.blocks[type].remaining -= size;
				gcx.blocks[type].pos += size;
			}
			else {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error(std::format("Can't align IncStreamPos(0x{:x}): Not enough data in {}, remaining 0x{:x}", size, XFileBlockName(type), gcx.blocks[type].remaining));
			}
		}

		void DB_PopStreamPos() {
			LOG_TRACE("{} DB_PopStreamPos()", hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.blocksStack.empty()) {
				throw std::runtime_error("Stream stack underflow");
			}
			XFileBlock idx{ gcx.blocksStack.top().type };

			if (IsTemp(idx)) {
				XBlockStackMember& mb{ gcx.blocksStack.top() };
				// reset the temp buffer
				gcx.blocks[idx].pos = mb.pos;
				gcx.blocks[idx].remaining = mb.remaining;
				gcx.blocksStack.pop();
			}
			else {
				gcx.blocksStack.pop();
				byte* ap{ utils::Aligned(gcx.blocks[idx].pos, 0x10) };
				size_t delta{ (size_t)(ap - gcx.blocks[idx].pos) };
				if (delta > gcx.blocks[idx].remaining) {
					gcx.blocks[idx].remaining = 0;
				}
				else {
					gcx.blocks[idx].remaining -= delta;
				}

				gcx.blocks[idx].pos = ap;
			}
		}

		void* DB_AllocStreamPos(int alignement) {
			LOG_TRACE("{} DB_AllocStreamPos(0x{:x})", hook::library::CodePointer{ _ReturnAddress() }, alignement + 1);
			XFileBlock type{ gcx.blocksStack.top().type };
			if (type == XFILE_BLOCK_MEMMAPPED) return nullptr;
			byte* pos{ gcx.blocks[type].pos };

			byte* npos{ utils::Aligned(pos, (size_t)alignement + 1) };

			size_t delta{ (size_t)(npos - pos) };

			if (delta <= gcx.blocks[type].remaining) {
				gcx.blocks[type].remaining -= delta;
			}
			else {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error(std::format("Can't align AllocStreamPos(0x{:x}): Not enough data in {}", alignement + 1, XFileBlockName(type)));
			}

			// update ptr
			gcx.blocks[type].pos = npos;

			LOG_TRACE("{} -> {} (max: 0x{:x})", hook::library::CodePointer{ _ReturnAddress() }, (void*)npos, gcx.blocks[type].remaining);
			return npos;
		}
		template<typename T = void, size_t align = sizeof(T) - 1>
		T* AllocStreamPos() {
			return (T*)DB_AllocStreamPos(align);
		}

		void DB_LoadXFileData(void* pos, size_t size) {
			LOG_TRACE("{} DB_LoadXFileData({}, 0x{:x}) -> {}/0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr(pos), size, XFileBlockName(gcx.blocksStack.top().type), gcx.reader->Loc());
			PrintIndexStack(core::logs::LVL_TRACE);
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

		bool Load_Stream(bool atStreamStart, void* ptr, size_t size) {
			XFileBlock type{ gcx.blocksStack.top().type };
			LOG_TRACE("{} Load_Stream({}, {}, 0x{:x}) -> {}/0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, atStreamStart, XBlockLocPtr(ptr), size, XFileBlockName(type), gcx.reader->Loc());
			if (!atStreamStart || !size) return true;
			switch (type) {
			case XFILE_BLOCK_TEMP:
			case XFILE_BLOCK_TEMP_PRELOAD:
			case XFILE_BLOCK_VIRTUAL:
			case XFILE_BLOCK_PHYSICAL:
			case XFILE_BLOCK_MEMMAPPED:
			case XFILE_BLOCK_MEMMAPPED2:
				DB_LoadXFileData(ptr, size);
				DB_IncStreamPos(size);
				return true;
			case XFILE_BLOCK_RUNTIME_VIRTUAL:
			case XFILE_BLOCK_RUNTIME_PHYSICAL:
				std::memset(ptr, 0, size);
				DB_IncStreamPos(size);
				return false;
			case XFILE_BLOCK_STREAMER:
			case XFILE_BLOCK_STREAMER_CPU:
				DB_IncStreamPos(size);
				return false;
			default:
				DB_IncStreamPos(size);
				return true;
			}
		}

		void Load_XStringCustom(char** str) {
			size_t size;
			char* ptr{ gcx.reader->ReadString(&size) };
			LOG_TRACE("{} Load_XStringCustom({}, 0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr((void*)str), size, gcx.reader->Loc(), acts::decryptutils::DecryptStringT8(ptr));
			AssertCanWrite(*str, size + 1);
			std::memcpy(*str, ptr, size + 1);
			DB_IncStreamPos(size + 1);
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
			if (hash && hash->name) {
				gcx.opt->AddAssetName(xasset->type, hash->name);
			}
			LOG_DEBUG("Loading asset {}/{} -> {}", XAssetNameFromId(xasset->type), hashutils::ExtractTmp("hash", hash->name), xasset->header);

			if (gcx.opt->noAssetDump) return xasset; // ignore
			if (xasset->header) {
				auto& workers{ fastfile::handlers::bo4::GetWorkers() };
				auto it{ workers.find(xasset->type) };
				if (it != workers.end()) {
					try {
						it->second->Unlink(*gcx.opt, xasset->header);
					}
					catch (std::runtime_error& e) {
						LOG_ERROR("Can't dump asset asset {}/{}: {}", XAssetNameFromId(xasset->type), (void*)xasset->header, e.what());
					}
				}
			}
			return xasset;
		}

		void* DB_FindXAssetHeader(XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
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

		void DB_ConvertOffsetToAlias(void** data) {
			uint64_t* p{ (uint64_t*)data };
			uint64_t block{ (*p - 1) >> 60 };
			uint64_t val{ (*p - 1) & ~0xF000000000000000ull };
			LOG_TRACE("{} DB_ConvertOffsetToAlias {}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName((XFileBlock)block), val, gcx.reader->Loc());
			return;
			if (gcx.ctx->blockSizes[block].size < val) {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error(std::format("Invalid ConvertOffsetToPointer with val {}/{:x}/{:x}", XFileBlockName((XFileBlock)block), val, *p));
			}
			*data = *(void**)&gcx.ctx->blockSizes[block].data[val];
		}

		void DB_ConvertOffsetToPointer(void** data) {
			uint64_t* p{ (uint64_t*)data };
			uint64_t block{ (*p - 1) >> 60 };
			uint64_t val{ (*p - 1) & ~0xF000000000000000ull };
			LOG_TRACE("{} DB_ConvertOffsetToPointer {}/{:x}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName((XFileBlock)block), val, *p, gcx.reader->Loc());
			return;
			if (gcx.ctx->blockSizes[block].size < val) {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error(std::format("Invalid ConvertOffsetToPointer with val {}/{:x}/{:x}", XFileBlockName((XFileBlock)block), val, *p));
			}
			*data = &gcx.ctx->blockSizes[block].data[val];
		}

		void* DB_ConvertOffsetToUnk(void* data) {
			uint64_t p{ reinterpret_cast<uint64_t>(data) };
			uint64_t block{ (p - 1) >> 60 };
			uint64_t val{ (p - 1) & ~0xF000000000000000ull };
			LOG_TRACE("{} DB_ConvertOffsetToUnk {}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName((XFileBlock)block), val, gcx.reader->Loc());
			return data;
			if (gcx.ctx->blockSizes[block].size < val) {
				hook::error::DumpStackTraceFrom();
				throw std::runtime_error(std::format("Invalid ConvertOffsetToPointer with val {}/{:x}/{:x}", XFileBlockName((XFileBlock)block), val, p));
			}
			return &gcx.ctx->blockSizes[block].data[val];
		}

		void** DB_InsertPointer() {
			LOG_TRACE("{} DB_InsertPointer -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, gcx.reader->Loc());
			DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
			void** ptr{ AllocStreamPos<void*>() };

			DB_PopStreamPos();

			return ptr;
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

		class BO4FFHandler : public fastfile::FFHandler {
		public:
			BO4FFHandler() : fastfile::FFHandler("bo4v2", "Black Ops 4") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true) };

				gcx.opt = &opt;

				gcx.Load_XAsset = reinterpret_cast<decltype(gcx.Load_XAsset)>(lib[0x2E35D10]);

				hook::memory::RedirectJmp(lib[0x2E0E130], DB_LoadXFileData);
				hook::memory::RedirectJmp(lib[0x2EBC050], Load_Stream);
				hook::memory::RedirectJmp(lib[0x2EBBBA0], DB_AllocStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBE20], DB_PopStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBEA0], DB_PushStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBBE0], DB_IncStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBFB0], DB_ConvertOffsetToAlias);
				hook::memory::RedirectJmp(lib[0x2EBBFF0], DB_ConvertOffsetToPointer);
				hook::memory::RedirectJmp(lib[0x2EBBCC0], DB_InsertPointer);
				hook::memory::RedirectJmp(lib[0x2EBC020], DB_ConvertOffsetToUnk);
				hook::memory::RedirectJmp(lib[0x2EBC110], Load_XStringCustom);
				hook::memory::RedirectJmp(lib[0x2EB84F0], DB_LinkXAssetEntry);
				hook::memory::RedirectJmp(lib[0x2EB75B0], DB_FindXAssetHeader);

				RemoveStub<0x2EBC480>(lib); //Load_ScrStringPtr
				RemoveStub<0x2EBC430>(lib); //Load_ScriptStringCustom
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
				RemoveStub<0x22B7AF0>(lib); // unk
				RemoveStub<0x22B7B00>(lib); // unk
				RemoveStub<0x35FCE50>(lib); // unk

				RemoveStub<0x2EBBDC0, true>(lib); // not sure
				RemoveStub<0x2EBBF40, true>(lib); // not sure
				
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				std::filesystem::path out{ opt.m_output / "bo4" / "data" };
				std::filesystem::create_directories(out);

				core::memory_allocator::MemoryAllocator blockAlloc{};



				// alloc the blocks
				for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
					size_t toAlloc{ ctx.blockSizes[i].size + 0xFFFF };
					LOG_DEBUG("Allocating blocks {} -> {}B (0x{:x})", XFileBlockName((XFileBlock)i), utils::FancyNumber(ctx.blockSizes[i].size), ctx.blockSizes[i].size);
					ctx.blockSizes[i].data = utils::Aligned((byte*)blockAlloc.Alloc(toAlloc), 0x10000);

					gcx.blocks[i].pos = ctx.blockSizes[i].data;
					gcx.blocks[i].remaining = ctx.blockSizes[i].size;
				}

				gcx.loaded = 0;
				gcx.blocksStack = {}; // cleanup
				gcx.blocksStack.emplace(XFILE_BLOCK_TEMP, gcx.blocks[XFILE_BLOCK_TEMP].pos, gcx.blocks[XFILE_BLOCK_TEMP].remaining);

				DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);

				XAssetList_0& assetList{ gcx.assetList };
				DB_LoadXFileData(&assetList, sizeof(assetList));
				SetAssetList(&assetList);
				DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);

				// Load_ScriptStringList(false, scriptStrings);
				if (assetList.strings) {
					assetList.strings = AllocStreamPos<char*>();

					LOG_TRACE("Log strings...");
					std::filesystem::path outStrings{ out / std::format("{}_strings.txt", ctx.ffname) };
					utils::OutFileCE os{ outStrings };
					if (!os) {
						throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
					}

					if (Load_Stream(true, assetList.strings, sizeof(*assetList.strings) * assetList.stringsCount)) {
						for (size_t i = 0; i < assetList.stringsCount; i++) {
							if (assetList.strings[i]) {
								if (assetList.strings[i] == (const char*)0xFFFFFFFFFFFFFFFF) {
									assetList.strings[i] = AllocStreamPos<char>();
									Load_XStringCustom(&assetList.strings[i]);
									char* scrstr{ acts::decryptutils::DecryptStringT8(assetList.strings[i]) };
									os << scrstr << "\n";
								}
								else {
									DB_ConvertOffsetToPointer((void**)&assetList.strings[i]);
								}
							}
						}
						LOG_INFO("Dump strings into {}", outStrings.string());
					}
				}

				DB_PopStreamPos();

				if (assetList.assets) {
					assetList.assets = AllocStreamPos<XAsset_0, 7>();
					Load_Stream(true, assetList.assets, sizeof(*assetList.assets) * assetList.assetCount);

					for (size_t i = 0; i < assetList.assetCount; i++) {
						const char* assType{ XAssetNameFromId(assetList.assets[i].type) };
						LOG_DEBUG("{}/{} Load asset {} (0x{:x})", i, assetList.assetCount, assType, (int)assetList.assets[i].type);
						gcx.Load_XAsset(false, &assetList.assets[i]);
					}
				}

				if (HAS_LOG_LEVEL(core::logs::LVL_DEBUG)) {
					LOG_DEBUG("Remaining:");
					LOG_DEBUG("stream: 0x{:x}", reader.Remaining());
					LOG_DEBUG("blocks:");
					for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
						size_t perc{ ctx.blockSizes[i].size ? 100 * gcx.blocks[i].remaining / ctx.blockSizes[i].size : 0 };
						LOG_DEBUG("- {}% 0x{:x}/0x{:x} {}", perc, gcx.blocks[i].remaining, ctx.blockSizes[i].size, XFileBlockName((XFileBlock)i));
					}

				}

				DB_PopStreamPos();
				LOG_INFO("Loaded {} asset(s)", gcx.loaded);
			}
		};
#ifndef CI_BUILD
		utils::ArrayAdder<BO4FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
#endif
	}
}