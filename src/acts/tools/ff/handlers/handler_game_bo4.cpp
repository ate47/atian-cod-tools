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

		struct XAssetEntry {
			XAsset_0 asset;
			byte zoneIndex;
			bool inuse;
			uint32_t nextHash;
			uint32_t nextType;
		};

		struct StreamRead {
			core::bytebuffer::ByteBuffer buff;
			XFileBlock index;
		};

		struct {
			BO4FFHandler* handler{};
			XAsset_0** varXAsset{};
			byte* pos[XFILE_BLOCK_COUNT]{};
			size_t remaining[XFILE_BLOCK_COUNT]{};
			void (*Load_XAsset)(bool atStreamStart, XAsset_0* asset) {};
			const char* (*DB_GetXAssetName)(XAssetType type, void** header) {};
			fastfile::FastFileOption* opt{};
			size_t loaded{};
			utils::OutFileCE* osassets{};
			fastfile::FastFileContext* ctx{};
			//core::bytebuffer::ByteBuffer* reader{};
			fastfile::pool::FFAssetPool pool{};
			XAssetList_0* assetList{};

			std::vector<StreamRead> readers{};

			StreamRead& Top() {
				if (readers.empty()) throw std::runtime_error("No reader");
				return readers[readers.size() - 1];
			}
			core::bytebuffer::ByteBuffer& Reader() {
				return Top().buff;
			}

			size_t Loc() {
				size_t loc{};
				for (StreamRead& r : readers) {
					loc += r.buff.Loc();
				}
				return loc;
			}
		} bo4FFHandlerContext{};

		void DB_IncStreamPos(int size) {
			LOG_TRACE("{} DB_IncStreamPos(0x{:x}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, size, bo4FFHandlerContext.Loc());
			/*
			auto& top{ bo4FFHandlerContext.Top() };
			if (bo4FFHandlerContext.remaining[top.index] < size) {
				throw std::runtime_error(std::format("IncStreamPos after end for {}", XFileBlockName(top.index)));
			}

			bo4FFHandlerContext.remaining[top.index] -= size;
			bo4FFHandlerContext.pos[top.index] += size;
			*/
		}

		bool Load_Stream(bool atStreamStart, void* ptr, uint32_t size) {
			auto& top{ bo4FFHandlerContext.Top() };
			LOG_TRACE("{} Load_Stream({}, {}, 0x{:x}) from 0x{:x}/{}", hook::library::CodePointer{ _ReturnAddress() }, atStreamStart, ptr, size, bo4FFHandlerContext.Loc(), XFileBlockName(top.index));
			bool ret;
			if (atStreamStart && size) {
				switch (top.index) {
				case XFILE_BLOCK_TEMP:
				case XFILE_BLOCK_TEMP_PRELOAD:
				case XFILE_BLOCK_VIRTUAL:
				case XFILE_BLOCK_PHYSICAL:
				case XFILE_BLOCK_MEMMAPPED:
				case XFILE_BLOCK_MEMMAPPED2:
					if (!top.buff.CanRead(size)) {
						LOG_ERROR("Can't read for index {}", XFileBlockName(top.index));
						hook::error::DumpStackTraceFrom();
					}
					top.buff.Read(ptr, size);
					ret = true;
					break;
				case XFILE_BLOCK_RUNTIME_VIRTUAL:
				case XFILE_BLOCK_RUNTIME_PHYSICAL:
					std::memset(ptr, 0, size);
					DB_IncStreamPos(size);
					ret = false;
					break;
				case XFILE_BLOCK_STREAMER:
				case XFILE_BLOCK_STREAMER_CPU:
					DB_IncStreamPos(size);
					ret = false;
					break;
				default:
					throw std::runtime_error(std::format("Invalid stream index {}", (int)top.index));
				}
			}
			else {
				ret = true;
			}

			byte* p{ (byte*)ptr };
			size_t s{ std::min<size_t>(size, 0x40 * 8) };
			while (s > 0) {
				size_t ss{ std::min<size_t>(s, 0x40) };
				LOG_TRACE("{:03x} : {}", p - (byte*)ptr, utils::data::AsHex(p, ss));
				p += ss;
				s -= ss;
			}
			return ret;
		}

		void* DB_AllocStreamPos(int alignment) {
			/*
			LOG_TRACE("{} DB_AllocStreamPos(0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, alignment + 1, bo4FFHandlerContext.Loc(), bo4FFHandlerContext.Reader().Ptr<void>());
			auto& top{ bo4FFHandlerContext.Top() };

			byte* currentLoc{ bo4FFHandlerContext.pos[top.index] };
			byte* alignedLoc{ utils::Aligned(currentLoc, alignment + 1) };
			size_t size{ (size_t)(alignedLoc - currentLoc) };

			if (bo4FFHandlerContext.remaining[top.index] < size) {
				throw std::runtime_error(std::format("DB_AllocStreamPos aligned after end for {}", XFileBlockName(top.index)));
			}

			bo4FFHandlerContext.remaining[top.index] -= size;
			bo4FFHandlerContext.pos[top.index] += size;
			return alignedLoc;
			*/
			LOG_TRACE("{} DB_AllocStreamPos(0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, alignment + 1, bo4FFHandlerContext.Loc(), bo4FFHandlerContext.Reader().Ptr<void>());
			return bo4FFHandlerContext.Reader().Ptr();
		}

		XAsset_0* DB_LinkXAssetEntry(XAsset_0* xasset, bool allowOverride) {
			bo4FFHandlerContext.loaded++;

			if (bo4FFHandlerContext.opt->assertContainer) {
				// todo: add header name
				bo4FFHandlerContext.pool.AddAssetHeader(xasset->type, 0, xasset->header);
			}

			XHash* hash{ xasset->header ? games::bo4::pool::GetAssetName(xasset->type, xasset->header, 0) : nullptr };
			const char* assetName{ hash ? hashutils::ExtractTmp("hash", hash->name) : "<unknown>" };

			*bo4FFHandlerContext.osassets << "\n" << XAssetNameFromId(xasset->type) << "," << assetName;
			if (hash && hash->name) {
				bo4FFHandlerContext.opt->AddAssetName(xasset->type, hash->name);
			}

			LOG_DEBUG("Loading asset {}/{} -> {}", XAssetNameFromId(xasset->type), hashutils::ExtractTmp("hash", hash->name), xasset->header);

			if (bo4FFHandlerContext.opt->noAssetDump) return xasset; // ignore
			auto& workers{ fastfile::handlers::bo4::GetWorkers() };
			auto it{ workers.find(xasset->type) };
			if (it != workers.end()) {
				try {
					it->second->Unlink(*bo4FFHandlerContext.opt, xasset->header);
				}
				catch (std::runtime_error& e) {
					LOG_ERROR("Can't dump asset asset {}/{}: {}", XAssetNameFromId(xasset->type), (void*)xasset->header, e.what());
				}
			}
			return xasset;
		}
		void DB_PopStreamPos() {
			size_t delta{ bo4FFHandlerContext.Reader().Loc() };
			bo4FFHandlerContext.readers.pop_back();
			bo4FFHandlerContext.Reader().Skip(delta);
			LOG_TRACE("{} DB_PopStreamPos -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, bo4FFHandlerContext.Loc());
		}
		void DB_PushStreamPos(XFileBlock index) {
			core::bytebuffer::ByteBuffer& top{ bo4FFHandlerContext.Reader() };
			XFileBlock old{ bo4FFHandlerContext.Top().index };
			//if (index == XFILE_BLOCK_MEMMAPPED) {
			//	index = old;
			//}
			//if (index == XFILE_BLOCK_MEMMAPPED2) {
			//	index = XFILE_BLOCK_MEMMAPPED;
			//}
			bo4FFHandlerContext.readers.emplace_back(core::bytebuffer::ByteBuffer{ top.Ptr(), top.Remaining() }, index);
			LOG_TRACE("{} DB_PushStreamPos({}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(index), bo4FFHandlerContext.Loc());
		}

		void DB_ConvertOffsetToAlias(uint64_t* data) {
			XFileBlock block{ (XFileBlock)((*data - 1) >> 60) };
			uint64_t val{ (*data - 1) & ~0xF000000000000000ull };
			LOG_TRACE("{} DB_ConvertOffsetToAlias {}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(block), val, bo4FFHandlerContext.Loc());
			//*reinterpret_cast<void**>(data) = *(void**)(bo4FFHandlerContext.ctx->blockSizes[block].data + val);
		}

		void DB_ConvertOffsetToPointer(uint64_t* data) {
			XFileBlock block{ (XFileBlock)((*data - 1) >> 60) };
			uint64_t val{ (*data - 1) & ~0xF000000000000000ull };
			LOG_TRACE("{} DB_ConvertOffsetToPointer {}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(block), val, bo4FFHandlerContext.Loc());
			//*reinterpret_cast<void**>(data) = bo4FFHandlerContext.ctx->blockSizes[block].data + val;
		}

		uint64_t DB_ConvertOffsetToUnk(uint64_t data) {
			XFileBlock block{ (XFileBlock)((data - 1) >> 60) };
			uint64_t val{ (data - 1) & ~0xF000000000000000ull };

			LOG_TRACE("{} DB_ConvertOffsetToUnk {}/{:x} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(block), val, bo4FFHandlerContext.Loc());

			return data; // bo4FFHandlerContext.ctx->blockSizes[block].data + val;
		}

		void** DB_InsertPointer() {
			//void** ptr{ bo4FFHandlerContext.Reader().Ptr<void*>() };

			//LOG_TRACE("{} DB_InsertPointer {} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)ptr, bo4FFHandlerContext.Loc());
			return nullptr;
			/*
			auto& top{ bo4FFHandlerContext.Top() };

			byte* currentLoc{ bo4FFHandlerContext.pos[XFILE_BLOCK_VIRTUAL] };
			byte* alignedLoc{ utils::Aligned<void**>(currentLoc) };
			size_t size{ (size_t)(alignedLoc - currentLoc) };

			if (bo4FFHandlerContext.remaining[XFILE_BLOCK_VIRTUAL] < size) {
				throw std::runtime_error(std::format("DB_AllocStreamPos aligned after end for {}", XFileBlockName(top.index)));
			}

			bo4FFHandlerContext.remaining[XFILE_BLOCK_VIRTUAL] -= size;
			bo4FFHandlerContext.pos[XFILE_BLOCK_VIRTUAL] += size;

			//LOG_TRACE("{} DB_InsertPointer {} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)ptr, bo4FFHandlerContext.Loc());
			return (void**)alignedLoc;
			*/
		}

		void* DB_FindXAssetHeader(XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
			return nullptr;
		}

		void EmptyStub() {
			LOG_TRACE("{} EmptyStub", hook::library::CodePointer{ _ReturnAddress() });
		}

		void Load_XStringCustom(char** str) {
			LOG_TRACE("{} Load_XStringCustom({}) 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)*str, bo4FFHandlerContext.Loc());
			auto& top{ bo4FFHandlerContext.Reader() };
			size_t size;
			const char* ptr{ top.ReadString(&size) };
			if (ptr != *str) {
				std::memcpy(*str, ptr, size + 1);
			}
			LOG_DEBUG("str {}", *str);
			DB_IncStreamPos((int)size);
		}

		class BO4FFHandler : public fastfile::FFHandler {
		public:
			BO4FFHandler() : fastfile::FFHandler("bo4", "Black Ops 4 (old)") {
				bo4FFHandlerContext.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true) };

				bo4FFHandlerContext.opt = &opt;

				bo4FFHandlerContext.Load_XAsset = reinterpret_cast<decltype(bo4FFHandlerContext.Load_XAsset)>(lib[0x2E35D10]);

				hook::memory::RedirectJmp(lib[0x2EB75B0], DB_FindXAssetHeader);
				hook::memory::RedirectJmp(lib[0x2EB84F0], DB_LinkXAssetEntry);
				hook::memory::RedirectJmp(lib[0x2EBBBA0], DB_AllocStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBBE0], DB_IncStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBCC0], DB_InsertPointer);
				hook::memory::RedirectJmp(lib[0x2EBBE20], DB_PopStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBEA0], DB_PushStreamPos);
				hook::memory::RedirectJmp(lib[0x2EBBFB0], DB_ConvertOffsetToAlias);
				hook::memory::RedirectJmp(lib[0x2EBBFF0], DB_ConvertOffsetToPointer);
				hook::memory::RedirectJmp(lib[0x2EBC020], DB_ConvertOffsetToUnk);
				hook::memory::RedirectJmp(lib[0x2EBC050], Load_Stream);
				hook::memory::RedirectJmp(lib[0x2EBC110], Load_XStringCustom);

				hook::memory::RedirectJmp(lib[0x2EBC480], EmptyStub); //Load_ScrStringPtr
				hook::memory::RedirectJmp(lib[0x2EBC430], EmptyStub); //Load_ScriptStringCustom
				hook::memory::RedirectJmp(lib[0x35BA450], EmptyStub); // Load_GfxImageAdapter
				hook::memory::RedirectJmp(lib[0x3CA8870], EmptyStub); // Load_SndBankAsset
				hook::memory::RedirectJmp(lib[0x3CA88C0], EmptyStub); // Load_SndBankAsset
				hook::memory::RedirectJmp(lib[0x35FC100], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3733C90], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FB980], EmptyStub); // load texture
				hook::memory::RedirectJmp(lib[0x35FC060], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBFC0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBDD0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3600EA0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x370BD10], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x353AE00], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3700CE0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3DC46F0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3CBBE00], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x353AF10], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x353ADD0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x3729A70], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBC90], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBF20], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBE80], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FBD30], EmptyStub); // unk
			
				hook::memory::RedirectJmp(lib[0x22B7AF0], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x22B7B00], EmptyStub); // unk
				hook::memory::RedirectJmp(lib[0x35FCE50], EmptyStub); // unk

				
				// write pool data to disk
				if (opt.assertContainer) {
					bo4FFHandlerContext.pool.csiHeader.gameId = compatibility::scobalula::csi::CG_BO4;
					bo4FFHandlerContext.pool.csiHeader.WriteCsi(compatibility::scobalula::csi::ActsCsiLocation());
				}
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				std::filesystem::path out{ opt.m_output / "bo4" / "data" };
				std::filesystem::create_directories(out);


				XAssetList_0& assetList{ *reader.ReadPtr<XAssetList_0>() };

				static char emptyStr[1]{};

				std::vector<fastfile::dump::BinXAsset> assetMap{};
				size_t stringsOffsetStart{ reader.Loc() };

				hashutils::ReadDefaultFile();
				// add hashes
				hashutils::AddPrecomputed(hash::Hash64(ctx.ffname), ctx.ffname, true);

				if (assetList.stringsCount) {
					//reader.Align<void*>();
					assetList.strings = reader.ReadPtr<char*>(assetList.stringsCount);

					for (size_t i = 0; i < assetList.stringsCount; i++) {
						if (fastfile::IsSame(assetList.strings[i], -1)) {
							assetList.strings[i] = reader.ReadString();
							hashutils::AddPrecomputed(hash::Hash64(assetList.strings[i]), assetList.strings[i], true);
						}
						else {

							assetList.strings[i] = emptyStr;
						}
					}
				}

				// allocate the blocks
				/*
				*/
				// TODO: use the blocks, better to rewrite the handler itself


				SetAssetList(&assetList);
				size_t stringsOffsetEnd{ reader.Loc() };
				{
					std::filesystem::path outStrings{ out / std::format("{}_strings.txt", ctx.ffname) };
					utils::OutFileCE os{ outStrings };
					if (!os) {
						throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
					}
					for (size_t i = 0; i < assetList.stringsCount; i++) {
						char* scrstr{ acts::decryptutils::DecryptStringT8(assetList.strings[i]) };
						assetList.strings[i] = scrstr;
						os << scrstr << "\n";
					}
					LOG_INFO("Dump strings into {}", outStrings.string());
				}

				if (!assetList.assetCount) {
					LOG_INFO("no asset");
					return;
				}

				LOG_DEBUG("assets: 0x{:x}", reader.Loc());
				// fixme: for some reasons, we have a bad alignment in the uncompressed buffer, this should be fixed before reading
				// it seems to be linked with the PushStreamPos types, a virtual/physical will set a particular align
				// I suggest to use a stack with custom buffers
				//reader.Align<void*>(); 
				assetList.assets = reader.ReadPtr<XAsset_0>(assetList.assetCount);
				LOG_DEBUG("assets data 0x{:x}", reader.Loc());

				std::filesystem::path outAssets{ out / std::format("{}_assets.csv", ctx.ffname) };
				utils::OutFileCE osa{ outAssets };
				if (!osa) {
					throw std::runtime_error(std::format("Can't open {}", outAssets.string()));
				}
				osa << "type,name";
				bo4FFHandlerContext.readers.emplace_back(reader, XFILE_BLOCK_TEMP);
				bo4FFHandlerContext.osassets = &osa;
				bo4FFHandlerContext.loaded = 0;
				bo4FFHandlerContext.ctx = &ctx;

				std::filesystem::path bindir{ opt.m_output / "bo4" / "binary" };
				std::filesystem::path binout{ bindir / ctx.ffname };
				if (opt.dumpBinaryAssets) {
					std::filesystem::create_directories(binout);
				}
				else if (opt.dumpBinaryAssetsMap) {
					std::filesystem::create_directories(bindir);
				}

				for (size_t i = 0; i < assetList.assetCount; i++) {
					XAsset_0& asset{ assetList.assets[i] };

					const char* assType{ XAssetNameFromId(asset.type) };
					LOG_DEBUG("Load asset {} (0x{:x})", assType, (int)asset.type);


					size_t originLoc{ bo4FFHandlerContext.Loc() };
					bo4FFHandlerContext.Load_XAsset(false, &asset);
					size_t len{ bo4FFHandlerContext.Loc() - originLoc };

					LOG_DEBUG("asset {} loaded (0x{:x}:0x{:x})", assType, originLoc, len);

					if (opt.dumpBinaryAssets) {
						std::vector<byte> rawAsset{};

						utils::WriteValue(rawAsset, &asset, sizeof(asset));
						reader.Goto(originLoc);
						utils::WriteValue(rawAsset, reader.ReadPtr<byte>(len), len);

						std::filesystem::path assetOut{ binout / std::format("{:04}_{}.bin", i, assType) };

						if (utils::WriteFile(assetOut, rawAsset)) {
							LOG_INFO("Dump asset {}", assetOut.string());
						}
						else {
							LOG_ERROR("Error when dumping {}", assetOut.string());
						}
					}
					if (opt.dumpBinaryAssetsMap) {
						assetMap.emplace_back((uint32_t)asset.type, (uint32_t)i, originLoc, len);
					}
				}
				size_t eofLoc{ bo4FFHandlerContext.Loc() };
				if (eofLoc != reader.Length()) {
					LOG_WARNING("eof at 0x{:x} != 0x{:x}", eofLoc, reader.Length());
				}

				if (opt.dumpBinaryAssetsMap) {
					std::vector<byte> mapData{};

					fastfile::dump::BinXAssetListHeader& header{ utils::Allocate<fastfile::dump::BinXAssetListHeader>(mapData) };
					header.magic = fastfile::dump::BIN_MAGIC;
					header.stringsCount = assetList.stringsCount;
					header.stringsOffsetStart = stringsOffsetStart;
					header.stringsOffsetEnd = stringsOffsetEnd;
					header.assetOffset = mapData.size();
					header.assetCount = assetMap.size();

					utils::WriteValue(mapData, assetMap.data(), sizeof(assetMap[0]) * assetMap.size());

					std::filesystem::path binmapout{ bindir / std::format("{}.map", ctx.ffname) };

					if (utils::WriteFile(binmapout, mapData)) {
						LOG_INFO("Dump asset {}", binmapout.string());
					}
					else {
						LOG_ERROR("Error when dumping {}", binmapout.string());
					}
				}

				LOG_INFO("{} asset(s) loaded (0x{:x})", bo4FFHandlerContext.loaded, bo4FFHandlerContext.loaded);

			}

			void Cleanup() override {
				if (bo4FFHandlerContext.opt->dumpAssetNames) {
					std::filesystem::path outAssets{ bo4FFHandlerContext.opt->m_output / "bo4" / "assetnames.csv" };
					{
						utils::OutFileCE osa{ outAssets };
						osa << "type,name";

						size_t n{ std::min<size_t>(bo4FFHandlerContext.opt->assetNames.size(), games::bo4::pool::ASSET_TYPE_COUNT)};

						for (size_t i = 0; i < n; i++) {
							for (uint64_t h : bo4FFHandlerContext.opt->assetNames[i]) {
								osa << "\n" << XAssetNameFromId((games::bo4::pool::XAssetType)i) << "," << hashutils::ExtractTmp("hash", h);
							}
						}
					}
					LOG_INFO("Dumped assets into {}", outAssets.string());
				}
			}
		};

		utils::ArrayAdder<BO4FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
	std::unordered_map<games::bo4::pool::XAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<games::bo4::pool::XAssetType, Worker*> map{};
		return map;
	}

	const char* GetScrString(ScrString_t id) {
		if ((int)id >= bo4FFHandlerContext.assetList->stringsCount) {
			throw std::runtime_error(std::format("Can't get scr string: {} >= {}", id, bo4FFHandlerContext.assetList->stringsCount));
		}
		return acts::decryptutils::DecryptStringT8(bo4FFHandlerContext.assetList->strings[id]);
	}

	void SetAssetList(XAssetList_0* assetList) {
		bo4FFHandlerContext.assetList = assetList;
	}

	const char* XFileBlockName(XFileBlock id) {
		static const char* names[]{
			"TEMP",
			"TEMP_PRELOAD",
			"RUNTIME_VIRTUAL",
			"RUNTIME_PHYSICAL",
			"VIRTUAL",
			"PHYSICAL",
			"STREAMER",
			"STREAMER_CPU",
			"MEMMAPPED",
			"COUNT",
			"MEMMAPPED2"
		};

		if (id >= ARRAYSIZE(names) || id < 0) {
			return "unknown";
		}
		return names[id];
	}

	XFileBlockMemLocation GetXFileBlockMemLocation(XFileBlock id) {
		static XFileBlockMemLocation data[9]{
			XFILE_BLOCK_LOC_VIRTUAL,
			XFILE_BLOCK_LOC_VIRTUAL,
			XFILE_BLOCK_LOC_VIRTUAL,
			XFILE_BLOCK_LOC_PHYSICAL,
			XFILE_BLOCK_LOC_VIRTUAL,
			XFILE_BLOCK_LOC_PHYSICAL,
			XFILE_BLOCK_LOC_STREAM,
			XFILE_BLOCK_LOC_STREAM_CPU,
			XFILE_BLOCK_LOC_STREAM_MMAP,
		};
		return data[id];
	}
}
