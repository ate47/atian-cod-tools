#include <includes.hpp>
#include <game_data.hpp>
#include <utils/enumlist.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_dump.hpp>
#include <tools/fastfile/fastfile_compiled_zone.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <games/bo4/pool.hpp>
#include <decryptutils.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/handler_game_bo4_load.hpp>


namespace fastfile::handlers::bo4 {
	namespace {
		using namespace games::bo4::pool;
		using namespace fastfile::handlers::bo4;

		class BO4FFHandler;

		struct StreamPosInfo {
			byte* pos;
			XFileBlock index;
		};

		struct SafeLoc {
			const char* name;
			byte* buffer;
			size_t len;
		};

		struct {
			BO4FFHandler* handler{};
			void (*Load_XAsset)(bool atStreamStart, XAsset_0* asset) {};

			fastfile::FastFileOption* opt{};
			std::vector<char*> xstrings{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			byte* data{};
			size_t dataLen{};
			utils::OutFileCE* outAssetNames{};
			utils::OutFileCE* outXHash{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unique_ptr<fastfile::compiled_zone::CompiledZone> compiledZone{};
			std::unordered_map<uint64_t, void*> linkedAssets[XAssetType::ASSET_TYPE_COUNT]{};
			XAssetList_0 assetList{};
			bool isRunning{};
			utils::EnumList<XAssetType, XAssetType::ASSET_TYPE_COUNT> handleList{ games::bo4::pool::XAssetIdFromName };

			// replace data
			XBlock* g_streamBlocks{};
			byte* g_streamPos{};
			XFileBlock g_streamPosIndex{};
			byte* g_streamRead[XFileBlock::XFILE_BLOCK_COUNT2]{};
			byte* g_streamPosArray[XFileBlock::XFILE_BLOCK_COUNT2]{};
			int g_streamPosStackIdx{};
			XBlock g_loadblocks[XFileBlock::XFILE_BLOCK_COUNT2]{};
			std::vector<SafeLoc> safeLocs{};
			StreamPosInfo g_streamPosStack[64]{};
		} gcx{};

		struct {
			XAsset_0* lastAsset{};
			size_t assetIdx{};
		} errorCtx;
		void PrintErrorContext() {
			if (!gcx.isRunning) {
				return;
			}

			LOG_ERROR("ERROR WHEN DUMPING DATA");
			if (errorCtx.lastAsset) {
				LOG_ERROR("last asset: {} / {}", XAssetNameFromId(errorCtx.lastAsset->type), errorCtx.lastAsset->header);
				LOG_ERROR("index: {} / {}", errorCtx.assetIdx, gcx.assetList.assetCount);
				LOG_ERROR("gcx.g_streamPosStackIdx = {}", gcx.g_streamPosStackIdx);
			}
		}

		inline void DECLSPEC_NORETURN ThrowFastFileError_(const std::string& msg) {
			LOG_LVL(core::logs::LVL_ERROR, msg);
			PrintErrorContext();
			hook::error::DumpStackTraceFrom();
			throw std::runtime_error("fastfile error");
		}

#define ThrowFastFileError(...) ThrowFastFileError_(std::format(__VA_ARGS__))

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

		byte* Mem_AllocAligned(const char* id, size_t size, size_t alignment, bool streamed) {
			static byte empty{};
			if (!size || streamed) {
				return &empty;
			}
			byte* d{ gcx.allocator.AllocAligned<byte>(size, alignment) };
			gcx.safeLocs.emplace_back(id, d, size);
			return d;
		}
	}

	void DB_AllocXBlocks(size_t* blockSize, XBlock* blocks) {
		gcx.safeLocs.clear();
		gcx.safeLocs.emplace_back("assetList", (byte*)&gcx.assetList, sizeof(gcx.assetList));
		//gcx.safeLocs.emplace_back("decompressed", gcx.data, gcx.dataLen);
		for (size_t i = 0; i < XFILE_BLOCK_COUNT; i++) {
			XFileBlock block{ (XFileBlock)i };

			blocks[block].data = nullptr;
			blocks[block].size = blockSize[block];

			if (!blocks[block].size) {
				continue;
			}

			switch (GetXFileBlockMemLocation(block)) {
			case XFILE_BLOCK_LOC_VIRTUAL:
				blocks[block].size += 0xF;
				blocks[block].data = Mem_AllocAligned(XFileBlockName(block), blocks[block].size, 0x10000, false);
				break;
			case XFILE_BLOCK_LOC_PHYSICAL:
			case XFILE_BLOCK_LOC_STREAM_CPU:
			case XFILE_BLOCK_LOC_STREAM_MMAP:
				blocks[block].data = Mem_AllocAligned(XFileBlockName(block), blocks[block].size, 0x10000, false);
				break;
			case XFILE_BLOCK_LOC_STREAM_DEV:
				blocks[block].data = Mem_AllocAligned(XFileBlockName(block), blocks[block].size, 0x10000, true);
				break;
			}
		}
	}

	void DB_InitStreams(XBlock* blocks) {
		gcx.g_streamBlocks = blocks;
		gcx.g_streamPos = blocks[XFILE_BLOCK_TEMP].data;
		gcx.g_streamPosIndex = XFILE_BLOCK_TEMP;
		gcx.g_streamPosArray[XFILE_BLOCK_TEMP] = nullptr;
		gcx.g_streamRead[XFILE_BLOCK_TEMP] = 0;

		for (size_t i = 1; i < XFILE_BLOCK_COUNT; i++) {
			gcx.g_streamRead[i] = 0;
			gcx.g_streamPosArray[i] = blocks[i].data;
		}
	}

	static void AssertCanWrite(void* ptr, size_t len) {
		if (!len) {
			return;
		}
		byte* p{ (byte*)ptr };
		for (const SafeLoc& loc : gcx.safeLocs) {
			if (p >= loc.buffer && p < loc.buffer + loc.len) {
				size_t rloc{ (size_t)(p - loc.buffer) };
				if (rloc + len > loc.len) {
					ThrowFastFileError("AssertCanWrite: Can't read {}:0x{:x}+0x{:x}, remaining 0x{:x} at {}", loc.name, rloc, len, (int64_t)loc.len - rloc, ptr);
				}
				return;
			}
		}
		ThrowFastFileError("AssertCanWrite: Can't read 0x{:x} at {} (outside data)", len, ptr);
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

	void DB_IncStreamPos(size_t size) {
		gcx.g_streamPos += size;
	}

	bool Load_Stream(bool atStreamStart, void* ptr, size_t size) {
		LOG_TRACE("Load_Stream({}, {}, {}) {}", atStreamStart ? "true" : "false", ptr, size, hook::library::CodePointer{_ReturnAddress()});
		if (!atStreamStart || !size) {
			return true;
		}

		switch (gcx.g_streamPosIndex) {
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
			memset(ptr, 0, size);
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

	void* DB_AllocStreamPos(int align) {
		LOG_TRACE("DB_AllocStreamPos({}) {}", align, hook::library::CodePointer{ _ReturnAddress() });
		byte* streamPos{ gcx.g_streamPos };
		byte* newLoc{ utils::Aligned(gcx.g_streamPos, (size_t)align + 1) };
		gcx.g_streamPos = newLoc;
		// add align padding
		gcx.g_streamRead[gcx.g_streamPosIndex] += newLoc - streamPos;
		return newLoc;
	}

	void DB_PopStreamPos() {
		LOG_TRACE("DB_PopStreamPos() {}", hook::library::CodePointer{ _ReturnAddress() });
		if (!gcx.g_streamPosStackIdx) {
			ThrowFastFileError("DB_PopStreamPos error: Empty pos stack");
		}

		int newidx{ --gcx.g_streamPosStackIdx };
		byte* pos;
		switch (gcx.g_streamPosIndex) {
		case XFILE_BLOCK_TEMP:
			pos = gcx.g_streamPosStack[newidx].pos;
			gcx.g_streamPos = pos;
			break;
		case XFILE_BLOCK_TEMP_PRELOAD:
			pos = utils::Aligned(gcx.g_streamPos, 0x10);
			gcx.g_streamPos = pos;
			break;
		default:
			pos = gcx.g_streamPos;
			break;
		}

		XFileBlock index{ gcx.g_streamPosStack[newidx].index };
		if (index != gcx.g_streamPosIndex) {
			gcx.g_streamPosArray[gcx.g_streamPosIndex] = pos;
			gcx.g_streamPos = gcx.g_streamPosArray[index];
			gcx.g_streamPosIndex = index;
		}
	}

	bool DB_EnableMemMappedData() {
		return false;
	}

	void DB_PushStreamPos(XFileBlock index) {
		LOG_TRACE("DB_PushStreamPos({}) {}", XFileBlockName(index), hook::library::CodePointer{_ReturnAddress()});
		XFileBlock prevIndex{ gcx.g_streamPosIndex };
		XFileBlock newIndex{ index };

		if (index == XFILE_BLOCK_MEMMAPPED) {
			if (!DB_EnableMemMappedData()) {
				newIndex = gcx.g_streamPosIndex;
			}
		}

		if (newIndex == XFILE_BLOCK_MEMMAPPED2) {
			newIndex = XFILE_BLOCK_MEMMAPPED;
		}

		int stackIndex{ gcx.g_streamPosStackIdx++ };
		if (stackIndex == ARRAYSIZE(gcx.g_streamPosStack)) {
			ThrowFastFileError("DB_PushStreamPos error: Full pos stack");
		}
		gcx.g_streamPosStack[stackIndex].index = prevIndex;

		byte* pos;
		if (newIndex == prevIndex) {
			pos = gcx.g_streamPos;
		}
		else {
			gcx.g_streamPosIndex = newIndex;
			gcx.g_streamPosArray[prevIndex] = gcx.g_streamPos;
			pos = gcx.g_streamPosArray[newIndex];
			gcx.g_streamPos = pos;
		}
		gcx.g_streamPosStack[stackIndex].pos = pos;
	}

	const void** DB_InsertPointer() {
		LOG_TRACE("DB_InsertPointer() {}", hook::library::CodePointer{ _ReturnAddress() });
		XFileBlock index{ gcx.g_streamPosIndex };
		int posStackLoc{ gcx.g_streamPosStackIdx };
		gcx.g_streamPosStack[gcx.g_streamPosStackIdx].index = gcx.g_streamPosIndex;

		byte* pos;
		if (index == XFILE_BLOCK_VIRTUAL) {
			pos = gcx.g_streamPos;
		}
		else {
			// we swap to BLOCK_VIRTUAL to create the pointer
			gcx.g_streamPosArray[index] = gcx.g_streamPos;
			index = XFILE_BLOCK_VIRTUAL;
			pos = gcx.g_streamPosArray[XFILE_BLOCK_VIRTUAL];
			gcx.g_streamPosIndex = XFILE_BLOCK_VIRTUAL;
		}

		gcx.g_streamPosStackIdx = posStackLoc;
		gcx.g_streamPosStack[posStackLoc].pos = pos;

		// add alignment, but not the pointer data, maybe a mistake from them
		byte* oldPos{ pos };
		pos = utils::Aligned<void*>(pos);
		gcx.g_streamRead[XFILE_BLOCK_VIRTUAL] += pos - oldPos;
		gcx.g_streamPos = pos + sizeof(void*);

		XFileBlock currentIndex{ gcx.g_streamPosStack[posStackLoc].index };
		if (currentIndex != index) {
			gcx.g_streamPosArray[index] = gcx.g_streamPos;
			gcx.g_streamPosIndex = currentIndex;
			gcx.g_streamPos = gcx.g_streamPosArray[currentIndex];
		}
		return (const void**)pos;
	}


	void* DB_GetOffsetData(uint64_t ref) {
		XFileBlock block{ (XFileBlock)((ref - 1) >> 60) };
		uint64_t offset{ (ref - 1) & ~0xF000000000000000 };

		void* loc{ &gcx.g_streamBlocks[block].data[offset] };
		LOG_TRACE("DB_GetOffsetData(0x{:x}, {}, 0x{:x}) -> {} {}", ref, XFileBlockName(block), offset, loc, hook::library::CodePointer{ _ReturnAddress() });
		return loc;
	}

	void DB_ConvertOffsetToPointer(void** data) {
		LOG_TRACE("DB_ConvertOffsetToPointer() {}", hook::library::CodePointer{ _ReturnAddress() });
		*data = DB_GetOffsetData(*(uint64_t*)data);
	}

	void DB_ConvertOffsetToAlias(void** data) {
		LOG_TRACE("DB_ConvertOffsetToAlias() {}", hook::library::CodePointer{ _ReturnAddress() });
		*data = *(void**)DB_GetOffsetData(*(uint64_t*)data);
	}

	void Load_XStringCustom(char** str) {
		size_t size;
		char* ptr{ gcx.reader->ReadString(&size) };
		LOG_TRACE("{} Load_XStringCustom({}, 0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, XBlockLocPtr((void*)str), size, gcx.reader->Loc(), acts::decryptutils::DecryptStringT8(ptr));
		AssertCanWrite(*str, size + 1);
		std::memcpy(*str, ptr, size + 1);
		const char* ds{ acts::decryptutils::DecryptStringT8(*str) };
		std::memmove(*str, ds, std::strlen(ds) + 1);
		hashutils::AddPrecomputed(hash::Hash64(ds), ds, true);
		if (gcx.opt->dumpXStrings) {
			gcx.xstrings.push_back(*str);
		}
		DB_IncStreamPos(size + 1);
	}

	void Load_String(bool atStreamStart, char** handle) {
		if (*handle) {
			if (*handle == (char*)0xFFFFFFFFFFFFFFFF) {
				*handle = AllocStreamPos<char>();
				Load_XStringCustom(handle);
			}
			else {
				DB_ConvertOffsetToPointer((void**)handle);
			}
		}
	}

	void Load_SimpleAsset_Internal(void** header, games::bo4::pool::XAssetType type) {
		XAsset_0 newEntry{};
		newEntry.type = type;
		newEntry.header = *header;
		*header = DB_LinkXAssetEntry(&newEntry, false)->header;
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
		size_t assetSize{ games::bo4::pool::GetAssetSize(xasset->type) };
		if (!assetSize) {
			LOG_ERROR("Can't link {}: empty asset type", games::bo4::pool::XAssetNameFromId(xasset->type));
			return xasset;
		}
		void* baseHeader{ xasset->header };
		void* linkedHeader{ gcx.allocator.Alloc(assetSize) };

		if (xasset->header) {
			std::memcpy(linkedHeader, xasset->header, assetSize);

			xasset->header = linkedHeader;
		}

		if (hash && hash->name) {
			bool defaultAsset{ (hash->name & ~hash::MASK63) != 0 };
			if (defaultAsset) hash->name &= hash::MASK63;
			*gcx.outAssetNames << XAssetNameFromId(xasset->type) << "," << (defaultAsset ? "d" : "") << "#" << hashutils::ExtractTmp("hash", hash->name);
			**gcx.outAssetNames << std::endl;

			gcx.linkedAssets[xasset->type][hash->name] = xasset->header;

			LOG_DEBUG("Loading asset {}({})/{}({:x}) -> {}/{}", 
				XAssetNameFromId(xasset->type), (int)xasset->type, hashutils::ExtractTmp("hash", hash->name), hash->name,
				xasset->header, XBlockLocPtr(baseHeader)
			);
		}

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
	namespace {

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
			ThrowFastFileError("{} ErrorStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
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

		void Load_XHash(XHash* hash) {
			if (gcx.outXHash && *hash) {
				*gcx.outXHash << hashutils::ExtractTmp("hash", hash->name) << "\n";
			}
		}

		class BO4FFHandler : public fastfile::FFHandler {
		public:
			BO4FFHandler() : fastfile::FFHandler("bo4", "Black Ops 4") {
				gcx.handler = this;

				hook::error::AddErrorDumper(PrintErrorContext);
			}

			void Init(fastfile::FastFileOption& opt) override {
				gcx.opt = &opt;

				gcx.handleList.Clear();
				if (opt.assetTypes) {
					gcx.handleList.LoadConfig(opt.assetTypes);
				}

				acts::game_data::GameData game{ "bo4" };
				hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, game.GetModuleName(), "bo4") };
				hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
				game.SetScanContainer(&scan);
				scan.Sync();

				scan.ignoreMissing = true;

				game.Get("Load_XAsset", &gcx.Load_XAsset);
				void* Load_XHash{};
				if (opt.dumpXHash) {
					game.Get("Load_XHash", &Load_XHash);
				}

				game.Redirect("Load_Stream", Load_Stream);
				game.Redirect("DB_PopStreamPos", DB_PopStreamPos);
				game.Redirect("DB_PushStreamPos", DB_PushStreamPos);
				game.Redirect("DB_IncStreamPos", DB_IncStreamPos);
				game.Redirect("DB_AllocStreamPos", DB_AllocStreamPos);
				game.Redirect("DB_ConvertOffsetToPointer", DB_ConvertOffsetToPointer);
				game.Redirect("DB_ConvertOffsetToAlias", DB_ConvertOffsetToAlias);
				game.Redirect("DB_GetOffsetData", DB_GetOffsetData);
				game.Redirect("DB_InsertPointer", DB_InsertPointer);
				game.Redirect("DB_AllocXBlocks", DB_AllocXBlocks);
				game.Redirect("DB_InitStreams", DB_InitStreams);
				game.Redirect("DB_LoadXFileData", DB_LoadXFileData);
				game.Redirect("Load_XStringCustom", Load_XStringCustom);
				game.Redirect("DB_LinkXAssetEntry", DB_LinkXAssetEntry);
				game.Redirect("DB_FindXAssetHeader", DB_FindXAssetHeader);

				game.ApplyNullScans("fastfile");


				if (scan.foundMissing) {
					throw std::runtime_error("Can't find some patterns");
				}

				if (opt.dumpXHash) {
					// this one is quite heavy, so only when required
					// Load_XHash is empty, so we can't jump from it
					void* trampoline{ hook::memory::AllocateNear(Load_XHash, 0x10) };
					hook::memory::RedirectJmp(trampoline, fastfile::handlers::bo4::Load_XHash, true);

					size_t patchedXHash{};
					std::vector<hook::library::ScanResult> calls{ scan.Scan("E8 ?? ?? ?? ??") };

					for (hook::library::ScanResult& res : calls) {
						if (res.GetRelative<int32_t>(1) == (byte*)Load_XHash) {
							void* call{ res.GetPtr(1) };
							int32_t delta{ (int32_t)((byte*)trampoline - (byte*)call - sizeof(delta)) };
							hook::process::WriteMemSafe(call, &delta, sizeof(delta));
							patchedXHash++;
						}
					}
					LOG_TRACE("patched {}/{} Load_XHash call(s)", patchedXHash, calls.size());
				}

			}
			void Cleanup() override {
				gcx.isRunning = false;
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				SetFFContext(&ctx);
				gcx.reader = &reader;
				gcx.data = reader.Ptr();
				gcx.dataLen = reader.Remaining();
				std::filesystem::path out{ opt.m_output / "bo4" / "data" };
				std::filesystem::create_directories(out);
				gcx.isRunning = true;
				utils::CloseEnd end{ [] {
					gcx.isRunning = false;
					gcx.compiledZone = nullptr;
					errorCtx.lastAsset = nullptr;
				} };

				if (opt.dumpCompiledZone) {
					gcx.compiledZone = std::make_unique<fastfile::compiled_zone::CompiledZone>();
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

				DB_AllocXBlocks(blockSizes, gcx.g_loadblocks);
				DB_InitStreams(gcx.g_loadblocks);

				DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
				gcx.xstrings.clear();

				XAssetList_0& assetList{ gcx.assetList };
				DB_LoadXFileData(&assetList, sizeof(assetList));
				SetAssetList(&assetList);
				DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);

				gcx.opt->assetNames.clear();

				for (auto& [hashType, worker] : GetWorkers()) {
					worker->PreLoadWorker(&ctx);
				}

				if (assetList.strings) {
					assetList.strings = AllocStreamPos<char*>();

					LOG_TRACE("Log strings... {}", (void*)assetList.strings);
					std::filesystem::path outStrings{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE os{ outStrings };
					if (!os) {
						ThrowFastFileError("Can't open {}", outStrings.string());
					}

					if (Load_Stream(true, assetList.strings, sizeof(*assetList.strings) * assetList.stringsCount)) {
						for (size_t i = 0; i < assetList.stringsCount; i++) {
							Load_String(false, &assetList.strings[i]);
							if (assetList.strings[i]) {
								os << assetList.strings[i] << "\n";
							}
						}
						LOG_OPT_INFO("Dump strings into {}", outStrings.string());
					}
				}

				utils::OutFileCE osxhash{};
				if (opt.dumpXHash) {
					std::filesystem::path outXHash{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "xhash" / std::format("{}.txt", ctx.ffname) };
					std::filesystem::create_directories(outXHash.parent_path());
					osxhash->open(outXHash.string());
					if (!osxhash) {
						ThrowFastFileError("Can't open {}", outXHash.string());
					}
					gcx.outXHash = &osxhash;
					LOG_OPT_INFO("Dump xhashes into {}", outXHash.string());
				}
				else {
					gcx.outXHash = {};
				}

				DB_PopStreamPos();

				if (assetList.assets) {
					assetList.assets = AllocStreamPos<XAsset_0, 7>();
					Load_Stream(true, assetList.assets, sizeof(*assetList.assets) * assetList.assetCount);

					if (!opt.noAssetDump) {
						for (auto& [t, w] : GetWorkers()) {
							w->PreXFileLoading(opt, ctx);
						}
					}
					std::filesystem::path outAssets{ gcx.opt->m_output / "bo4" / "source" / "tables" / "data" / "assets" / std::format("{}{}.csv", (opt.noAssetDump ? "unloaded_" : ""), ctx.ffname)};
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE osa{ outAssets };
					osa << "type,name\n";
					gcx.outAssetNames = &osa;

					for (size_t i = 0; i < assetList.assetCount; i++) {
						XAsset_0* ass{ &assetList.assets[i] };
						errorCtx.lastAsset = ass;
						errorCtx.assetIdx = i;
						const char* assType{ XAssetNameFromId(ass->type) };
						LOG_DEBUG("{}/{} Load asset {} (0x{:x}) {}", i, assetList.assetCount, assType, (int)ass->type, ass->header);
						if (!opt.noAssetDump || opt.testDump || opt.dumpCompiledZone) {
#ifndef CI_BUILD
							//if (i >= 110100) core::logs::setlevel(core::logs::LVL_TRACE_PATH);
#endif
							gcx.Load_XAsset(false, ass);
							continue;
						}

						osa << assType << ",0x" << ass->header << "\n";
					}
					errorCtx.lastAsset = nullptr;

					LOG_OPT_INFO("Dumped assets into {}", outAssets.string());
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
						LOG_OPT_INFO("Dump xstrings into {}", outStrings.string());
					}
				}

				DB_PopStreamPos();
				LOG_OPT_INFO("Loaded {} asset(s)", gcx.loaded);

				if (gcx.compiledZone) {
					std::filesystem::path outCZ{ gcx.opt->m_output / "bo4" / "compiled" / std::format("{}.zonec", ctx.ffname) };
					gcx.compiledZone->DumpDebug(outCZ);
				}
			}
		};

		utils::ArrayAdder<BO4FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
}