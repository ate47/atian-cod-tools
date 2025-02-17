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

namespace {
	using namespace games::bo4::pool;

	struct XHash {
		uint64_t hash;
		uint64_t pad;
	};

	class BO4FFHandler;

	enum XFileBlock : int {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_TEMP_PRELOAD = 0x1,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x2,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x3,
		XFILE_BLOCK_VIRTUAL = 0x4,
		XFILE_BLOCK_PHYSICAL = 0x5,
		XFILE_BLOCK_STREAMER = 0x6,
		XFILE_BLOCK_STREAMER_CPU = 0x7,
		XFILE_BLOCK_MEMMAPPED = 0x8,
		XFILE_BLOCK_COUNT = 0x9,
		XFILE_BLOCK_MEMMAPPED2 = 0xA,
	};

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

	struct XAsset_0 {
		XAssetType type;
		void* header;
	}; static_assert(sizeof(XAsset_0) == 0x10);

	struct XAssetEntry {
		XAsset_0 asset;
		byte zoneIndex;
		bool inuse;
		uint32_t nextHash;
		uint32_t nextType;
	};

	struct XAssetList_0 {
		int stringsCount;
		char** strings;
		int assetCount;
		XAsset_0* assets;
	}; static_assert(sizeof(XAssetList_0) == 0x20);

	struct StreamRead {
		core::bytebuffer::ByteBuffer buff;
		XFileBlock index;
	};

	struct {
		BO4FFHandler* handler{};
		XAsset_0** varXAsset{};
		void (*Load_XAsset)(bool atStreamStart, XAsset_0* asset) {};
		const char* (*DB_GetXAssetName)(XAssetType type, void** header) {};
		fastfile::FastFileOption* opt{};
		size_t loaded{};
		utils::OutFileCE* osassets{};
		//core::bytebuffer::ByteBuffer* reader{};
		fastfile::pool::FFAssetPool pool{};
		std::vector<const char*> mtStrings{};

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
		void Align(size_t align) {
			//bo4FFHandlerContext.Reader().Align(align);
		}
	} bo4FFHandlerContext{};

	bool Load_StreamStub(bool atStreamStart, void* ptr, uint32_t size) {
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
					hook::error::DumpStackTraceFrom();
				}
				top.buff.Read(ptr, size);
				ret = true;
				break;
			case XFILE_BLOCK_RUNTIME_VIRTUAL:
			case XFILE_BLOCK_RUNTIME_PHYSICAL:
				//if (!top.buff.CanRead(size)) {
				//	hook::error::DumpStackTraceFrom();
				//}
				//std::memset(ptr, 0, size);
				//top.buff.Skip(size);
				ret = false;
				break;
			case XFILE_BLOCK_STREAMER:
			case XFILE_BLOCK_STREAMER_CPU:
				//if (!top.buff.CanRead(size)) {
				//	hook::error::DumpStackTraceFrom();
				//}
				//top.buff.Skip(size);
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
		bo4FFHandlerContext.Align((size_t)alignment + 1);
		LOG_TRACE("{} DB_AllocStreamPos(0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, alignment + 1, bo4FFHandlerContext.Loc(), bo4FFHandlerContext.Reader().Ptr<void>());
		return bo4FFHandlerContext.Reader().Ptr();
	}

	XAsset_0* DB_LinkXAssetEntry(XAsset_0* xasset, bool allowOverride) {
		bo4FFHandlerContext.loaded++;

		if (bo4FFHandlerContext.opt->assertContainer) {
			// todo: add header name
			bo4FFHandlerContext.pool.AddAssetHeader(xasset->type, 0, xasset->header);
		}

		games::bo4::pool::XHash* hash{ xasset->header ? games::bo4::pool::GetAssetName(xasset->type, xasset->header, 0) : nullptr };
		const char* assetName{ hash ? hashutils::ExtractTmp("hash", hash->hash) : "<unknown>" };

		*bo4FFHandlerContext.osassets << "\n" << XAssetNameFromId(xasset->type) << "," << assetName;
		if (hash && hash->hash) {
			bo4FFHandlerContext.opt->AddAssetName(xasset->type, hash->hash);
		}

		if (bo4FFHandlerContext.opt->noAssetDump) return xasset; // ignore
		switch (xasset->type) {
		case ASSET_TYPE_SCRIPTPARSETREE: {
			struct ScriptParseTree {
				XHash name;
				byte* buffer;
				uint32_t len;
			}; static_assert(sizeof(ScriptParseTree) == 0x20);
			ScriptParseTree* spt{ (ScriptParseTree*)xasset->header };


			std::filesystem::path outDir{ bo4FFHandlerContext.opt->m_output / "bo4" / "spt" };

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/script_{:x}.gscc", *(uint64_t*)spt->buffer, spt->name.hash) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump scriptparsetree {} 0x{:x} ({})", outFile.string(), spt->len, hashutils::ExtractTmpScript(spt->name.hash));
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			break;
		}
		case ASSET_TYPE_RAWFILE: {
			struct RawFile {
				XHash name;
				int32_t len;
				byte* buffer;
			}; static_assert(sizeof(RawFile) == 0x20);
			RawFile* asset{ (RawFile*)xasset->header };

			const char* n{ hashutils::ExtractPtr(asset->name.hash) };

			if (!n) {
				n = utils::va("hashed/rawfile/file_%llx.raw", asset->name.hash);
			}

			std::filesystem::path outFile{ bo4FFHandlerContext.opt->m_output / "bo4" / "source" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump raw file {} 0x{:x}", outFile.string(), asset->len);
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			break;
		}
		case ASSET_TYPE_LUAFILE: {
			struct LuaFile {
				XHash name;
				uint32_t len;
				byte* buffer;
			}; static_assert(sizeof(LuaFile) == 0x20);
			LuaFile* asset{ (LuaFile*)xasset->header };

			const char* n{ hashutils::ExtractPtr(asset->name.hash) };

			if (!n) {
				n = utils::va("hashed/%llx.lua", asset->name.hash);
			}

			std::filesystem::path outFile{ bo4FFHandlerContext.opt->m_output / "bo4" / "luafile" / n };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump lua file {} 0x{:x}", outFile.string(), asset->len);
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			break;
		}
		default: {
			LOG_TRACE("Ignoring asset {}/{}", XAssetNameFromId(xasset->type), (void*)xasset->header);
			break;
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
		bo4FFHandlerContext.readers.emplace_back(core::bytebuffer::ByteBuffer{ top.Ptr(), top.Remaining() }, index);
		LOG_TRACE("{} DB_PushStreamPos({}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(index), bo4FFHandlerContext.Loc());
	}

	void DB_IncStreamPos(int size) {
		//auto& top{ bo4FFHandlerContext.Reader() };
		//if (!top.CanRead(size)) {
		//	hook::error::DumpStackTraceFrom();
		//}
		//top.Skip(size);
		LOG_TRACE("{} DB_IncStreamPos(0x{:x}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, size, bo4FFHandlerContext.Loc());
	}

	void DB_ConvertOffsetToAlias(void* data) {
		LOG_TRACE("{} DB_ConvertOffsetToAlias", hook::library::CodePointer{ _ReturnAddress() });
	}
	void DB_ConvertOffsetToPointer(void* data) {
		LOG_TRACE("{} DB_ConvertOffsetToPointer", hook::library::CodePointer{ _ReturnAddress() });
	}
	uint64_t* DB_UnkPtr(uint64_t* data) {
		LOG_TRACE("{} DB_UnkPtr {}", hook::library::CodePointer{ _ReturnAddress() }, (void*)data);
		return data;
	}

	void* DB_FindXAssetHeader(XAssetType type, XHash* name, bool errorIfMissing, int waitTime) {
		return nullptr;
	}

	void** DB_InsertPointer() {
		bo4FFHandlerContext.Align(8);
		void** ptr{ bo4FFHandlerContext.Reader().Ptr<void*>() };

		LOG_TRACE("{} DB_InsertPointer {} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)ptr, bo4FFHandlerContext.Loc());
		return ptr;
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
		BO4FFHandler() : fastfile::FFHandler("bo4", "Black Ops 4") {
			bo4FFHandlerContext.handler = this;
		}

		void Init(fastfile::FastFileOption& opt) override {
			hook::library::Library lib{ opt.GetGame(true) };

			bo4FFHandlerContext.opt = &opt;

			bo4FFHandlerContext.Load_XAsset = reinterpret_cast<decltype(bo4FFHandlerContext.Load_XAsset)>(lib[0x2E35D10]);

			hook::memory::RedirectJmp(lib[0x2EBC050], Load_StreamStub);
			hook::memory::RedirectJmp(lib[0x2EBBBA0], DB_AllocStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBBFB0], DB_ConvertOffsetToAlias);
			hook::memory::RedirectJmp(lib[0x2EB84F0], DB_LinkXAssetEntry);
			hook::memory::RedirectJmp(lib[0x2EBBFF0], DB_ConvertOffsetToPointer);
			hook::memory::RedirectJmp(lib[0x2EBBCC0], DB_InsertPointer);
			hook::memory::RedirectJmp(lib[0x2EBBBE0], DB_IncStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBC110], Load_XStringCustom);
			hook::memory::RedirectJmp(lib[0x2EBBE20], DB_PopStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBBEA0], DB_PushStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBC020], DB_UnkPtr);
			hook::memory::RedirectJmp(lib[0x2EB75B0], DB_FindXAssetHeader);

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

			
			
			/*
			bo4FFHandlerContext.DB_GetXAssetName = reinterpret_cast<decltype(bo4FFHandlerContext.DB_GetXAssetName)>(lib[0x13E9D80]);

			hook::memory::RedirectJmp(lib[0x1426160], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1426090], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x14260F0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8BFB0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD4610], EmptyStub); // unk
			*/
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
			size_t stringsOffsetEnd{ reader.Loc() };

			bo4FFHandlerContext.mtStrings.clear();
			{
				std::filesystem::path outStrings{ out / std::format("{}_strings.txt", ctx.ffname) };
				utils::OutFileCE os{ outStrings };
				if (!os) {
					throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
				}
				for (size_t i = 0; i < assetList.stringsCount; i++) {
					char* scrstr{ acts::decryptutils::DecryptStringT8(assetList.strings[i]) };
					bo4FFHandlerContext.mtStrings.push_back(scrstr);
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
