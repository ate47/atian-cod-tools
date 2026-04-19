#include <includes.hpp>
#include <game_data.hpp>
#include <utils/enumlist.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/bo3/pools.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <tools/fastfile/handlers/handler_game_bo3.hpp>

namespace fastfile::handlers::bo3 {
	std::unordered_map<::bo3::pool::T7XAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<::bo3::pool::T7XAssetType, Worker*> map{};
		return map;
	}

	bool FixupName(const char*& s) {
		int b;
		if (!s) return false;
		if (!hook::memory::ReadMemorySafe((void*)s, &b, sizeof(b))) {
			LOG_ERROR("Error when checking name {}", (void*)s);
			s = nullptr;
			return false;
		}
		if (*s == ',') s++;
		return true;
	}
}
namespace {
	using namespace fastfile::handlers::bo3;

	using namespace bo3::pool;

	class BO3FFHandler;

	enum XFileBlock : int {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x1,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x2,
		XFILE_BLOCK_DELAY_VIRTUAL = 0x3,
		XFILE_BLOCK_DELAY_PHYSICAL = 0x4,
		XFILE_BLOCK_VIRTUAL = 0x5,
		XFILE_BLOCK_PHYSICAL = 0x6,
		XFILE_BLOCK_STREAMER_RESERVE = 0x7,
		XFILE_BLOCK_STREAMER = 0x8,
		XFILE_BLOCK_MEMMAPPED = 0x9,
		MAX_XFILE_COUNT = 0xA,
	};

	struct XBlock {
		byte* data;
		uint64_t size;
	};

	const char* XFileBlockName(XFileBlock id) {
		static const char* names[]{
			"TEMP",
			"RUNTIME_VIRTUAL",
			"RUNTIME_PHYSICAL",
			"DELAY_VIRTUAL",
			"DELAY_PHYSICAL",
			"VIRTUAL",
			"PHYSICAL",
			"STREAMER_RESERVE",
			"STREAMER",
			"MEMMAPPED"
		};

		if (id >= ACTS_ARRAYSIZE(names) || id < 0) {
			return "unknown";
		}
		return names[id];
	}

	struct XAsset_0 {
		T7XAssetType type;
		void* header;
	}; static_assert(sizeof(XAsset_0) == 0x10);


	struct XAssetList_0 {
		int stringsCount;
		const char** strings;
		uint32_t unk10;
		void* unk18;
		int assetCount;
		XAsset_0* assets;
	}; static_assert(sizeof(XAssetList_0) == 0x30);

	struct StreamRead {
		core::bytebuffer::ByteBuffer buff;
		XFileBlock index;
	};

	struct {
		core::memory_allocator::MemoryAllocator allocator{};
		XAssetList_0** varScriptStringList{};
		XAsset_0** varXAsset{};
		XFileBlock* g_streamPosIndex{};
		void (*Load_XAsset)(bool atStreamStart) {};
		void (*DB_InitStreams)(XBlock* blocks) {};
		const char*(*DB_GetXAssetName)(XAsset_0* asset) {};
		void (*DB_IncStreamPos)(int len) {};
		void (*DB_PushStreamPos)(XFileBlock index) {};
		void (*DB_PopStreamPos)() {};
		void* (*DB_AllocStreamPos)(int alignment) {};
		void (*Load_ScriptStringList)(bool atStreamStart) {};

		XBlock blocks[XFileBlock::MAX_XFILE_COUNT]{};
		XAssetList_0 assetList{};
		fastfile::FastFileOption* opt{};
		size_t loaded{};
		utils::OutFileCE* osassets{};
		core::bytebuffer::ByteBuffer* reader{};
		fastfile::pool::FFAssetPool pool{};
		utils::EnumList<T7XAssetType, T7XAssetType::T7_ASSET_TYPE_COUNT> handleList{ bo3::pool::T7XAssetIdFromName };
		std::vector<char*> xstrings{};
	} gcx{};


	enum XFileBlockMemLocation : uint32_t {
		XFILE_BLOCK_LOC_VIRTUAL = 0,
		XFILE_BLOCK_LOC_PHYSICAL = 1,
		XFILE_BLOCK_LOC_STREAM = 2,
		XFILE_BLOCK_LOC_STREAM_CPU = 3,
		XFILE_BLOCK_LOC_STREAM_MMAP = 4,
		XFILE_BLOCK_LOC_TEMP = 5,
	};

	XFileBlockMemLocation GetXFileBlockMemLocation(XFileBlock id) {
		static XFileBlockMemLocation data[10]{
			XFILE_BLOCK_LOC_TEMP, // XFILE_BLOCK_TEMP
			XFILE_BLOCK_LOC_VIRTUAL, // XFILE_BLOCK_RUNTIME_VIRTUAL
			XFILE_BLOCK_LOC_PHYSICAL, // XFILE_BLOCK_RUNTIME_PHYSICAL
			XFILE_BLOCK_LOC_VIRTUAL, // XFILE_BLOCK_DELAY_VIRTUAL
			XFILE_BLOCK_LOC_PHYSICAL, // XFILE_BLOCK_DELAY_PHYSICAL
			XFILE_BLOCK_LOC_VIRTUAL, // XFILE_BLOCK_VIRTUAL
			XFILE_BLOCK_LOC_PHYSICAL, // XFILE_BLOCK_PHYSICAL
			XFILE_BLOCK_LOC_STREAM, // XFILE_BLOCK_STREAMER_RESERVE
			XFILE_BLOCK_LOC_STREAM_CPU, // XFILE_BLOCK_STREAMER
			XFILE_BLOCK_LOC_STREAM_MMAP, // XFILE_BLOCK_MEMMAPPED
		};
		return data[id];
	}

	byte* Mem_AllocAligned(const char* id, size_t size, size_t alignment, bool streamed) {
		static byte empty{};
		if (!size || streamed) {
			return &empty;
		}
		byte* d{ gcx.allocator.AllocAligned<byte>(size, alignment) };
		return d;
	}

	void DB_AllocXBlocks(size_t* blockSize, XBlock* blocks) {
		//gcx.safeLocs.emplace_back("decompressed", gcx.data, gcx.dataLen);
		for (size_t i = 0; i < MAX_XFILE_COUNT; i++) {
			XFileBlock block{ (XFileBlock)i };

			blocks[block].data = nullptr;
			blocks[block].size = blockSize[block];

			if (!blocks[block].size) {
				continue;
			}

			switch (GetXFileBlockMemLocation(block)) {
			case XFILE_BLOCK_LOC_TEMP:
			case XFILE_BLOCK_LOC_VIRTUAL:
				blocks[block].size += 0xF;
				blocks[block].data = Mem_AllocAligned(XFileBlockName(block), blocks[block].size, 0x10000, false);
				break;
			case XFILE_BLOCK_LOC_PHYSICAL:
			case XFILE_BLOCK_LOC_STREAM_CPU:
			case XFILE_BLOCK_LOC_STREAM_MMAP:
				blocks[block].data = Mem_AllocAligned(XFileBlockName(block), blocks[block].size, 0x10000, false);
				break;
			}
		}
	}

	inline void DECLSPEC_NORETURN ThrowFastFileError_(const std::string& msg) {
		LOG_LVL(core::logs::LVL_ERROR, msg);
		hook::error::DumpStackTraceFrom();
		throw std::runtime_error("fastfile error");
	}

#define ThrowFastFileError(...) ThrowFastFileError_(std::format(__VA_ARGS__))

	void DB_LoadXFileData(void* ptr, int64_t len) {
		LOG_TRACE("DB_LoadXFileData({}, 0x{:x}/0x{:x}) {}", ptr, len, gcx.reader->Remaining(), hook::library::CodePointer{ _ReturnAddress() });
		if (!ptr) {
			ThrowFastFileError("Can't read empty pointer idx:{}", (int)*gcx.g_streamPosIndex);
		}
		if (!gcx.reader->CanRead(len)) {
			hook::error::DumpStackTraceFrom();
		}
		gcx.reader->Read(ptr, len);
	}

	bool Load_Stream(bool atStreamStart, void* ptr, uint32_t size) {
		LOG_TRACE("{} Load_Stream({}, {}, 0x{:x}) {}", hook::library::CodePointer{ _ReturnAddress() }, atStreamStart ? "true" : "false", ptr, size, (int)*gcx.g_streamPosIndex);

		bool ret;
		if (atStreamStart && size) {
			switch (*gcx.g_streamPosIndex) {
			case XFILE_BLOCK_TEMP:
			case XFILE_BLOCK_VIRTUAL:
			case XFILE_BLOCK_PHYSICAL:
				DB_LoadXFileData(ptr, size);
				gcx.DB_IncStreamPos(size);
				ret = true;
				break;
			case XFILE_BLOCK_RUNTIME_VIRTUAL:
			case XFILE_BLOCK_RUNTIME_PHYSICAL:
				std::memset(ptr, 0, size);
				gcx.DB_IncStreamPos(size);
				ret = false;
				break;
			case XFILE_BLOCK_STREAMER_RESERVE:
				return false;
			case XFILE_BLOCK_DELAY_VIRTUAL:
			case XFILE_BLOCK_DELAY_PHYSICAL:
			case XFILE_BLOCK_STREAMER:
				gcx.DB_IncStreamPos(size);
				ret = false;
				break;
			default:
				throw std::runtime_error(std::format("Invalid stream index {}", (int)*gcx.g_streamPosIndex));
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

	void* DB_LinkXAssetEntry(T7XAssetType assetType, void* header) {
		gcx.loaded++;

		if (gcx.opt->assertContainer) {
			// todo: add header name
			gcx.pool.AddAssetHeader(assetType, 0, header);
		}

		const char* assType{ T7XAssetName(assetType) };
		XAsset_0 asset{};
		asset.type = assetType;
		asset.header = header;
		const char* n{ gcx.DB_GetXAssetName(&asset) };
		if (gcx.osassets) {
			if (*n == ',') n++;
			*gcx.osassets << assType << "," << n << "\n";
		}


		fastfile::AddAssetHeader(n, header, assetType, 0);


		if (gcx.opt->noAssetDump || (!gcx.handleList.Empty() && !gcx.handleList[assetType])) {
			return header; // ignore
		}

		auto& workers{ fastfile::handlers::bo3::GetWorkers() };

		auto it{ workers.find(assetType) };
		if (it != workers.end()) {
			it->second->Unlink(*gcx.opt, header);
		}

		return header;
	}

	void Load_XStringCustom(char** str) {
		LOG_TRACE("{} Load_XStringCustom({})", hook::library::CodePointer{ _ReturnAddress() }, (void*)str);
		size_t len;
		char* ptr{ gcx.reader->ReadString(&len) };
		LOG_TRACE("Load_XStringCustom() -> {}", ptr);
		std::memcpy(*str, ptr, len + 1);
		gcx.DB_IncStreamPos((int)(len + 1));
		hashutils::Add(ptr, true, true);
		if (gcx.opt->dumpXStrings) {
			gcx.xstrings.push_back(ptr);
		}
	}

	class BO3FFHandler : public fastfile::FFHandler {
	public:
		BO3FFHandler() : fastfile::FFHandler("bo3", "Black Ops 3", compatibility::scobalula::csi::CordycepGame::CG_BO3) {
		}

		void Init(fastfile::FastFileOption& opt) override {
			acts::game_data::GameData game{ "bo3" };
			commonFiles = game.GetCommonFastFiles();
			hook::module_mapper::Module& mod{ opt.GetGameModule(true, nullptr, false, game.GetModuleName(), "bo3") };
			hook::scan_container::ScanContainer& scan{ mod.GetScanContainer() };
			game.SetScanContainer(&scan);
			scan.Sync();

			scan.ignoreMissing = true;

			gcx.handleList.Clear();
			if (opt.assetTypes) {
				gcx.handleList.LoadConfig(opt.assetTypes);
			};

			game.Get("varXAsset", &gcx.varXAsset);
			game.Get("Load_XAsset", &gcx.Load_XAsset);
			game.Get("varScriptStringList", &gcx.varScriptStringList);
			game.Get("Load_ScriptStringList", &gcx.Load_ScriptStringList);
			game.Get("DB_GetXAssetName", &gcx.DB_GetXAssetName);
			game.Get("DB_InitStreams", &gcx.DB_InitStreams);
			game.Get("DB_IncStreamPos", &gcx.DB_IncStreamPos);
			game.Get("DB_PushStreamPos", &gcx.DB_PushStreamPos);
			game.Get("DB_PopStreamPos", &gcx.DB_PopStreamPos);
			game.Get("DB_AllocStreamPos", &gcx.DB_AllocStreamPos);
			game.Get("g_streamPosIndex", &gcx.g_streamPosIndex);

			game.Redirect("Load_Stream", Load_Stream);
			game.Redirect("DB_LinkXAssetEntry", DB_LinkXAssetEntry);
			game.Redirect("Load_XStringCustom", Load_XStringCustom);

			game.ApplyNullScans("fastfile");

			scan.Save();
			if (scan.foundMissing) {
				throw std::runtime_error("Can't find some patterns");
			}
			
			// write pool data to disk
			if (opt.assertContainer) {
				gcx.pool.csiHeader.gameId = compatibility::scobalula::csi::CG_BO3;
				gcx.pool.csiHeader.WriteCsi(compatibility::scobalula::csi::ActsCsiLocation());
			}
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
			gcx.reader = &reader;
			gcx.opt = &opt;

			gcx.opt->assetNames.clear();

			gcx.allocator.FreeAll();

			std::filesystem::path out{ opt.m_output / "bo3" / "data"};
			std::filesystem::create_directories(out);

			size_t blockSizes[MAX_XFILE_COUNT]{};
			if (ctx.blocksCount != MAX_XFILE_COUNT) {
				throw std::runtime_error(std::format("INVALID ctx.blocksCount != MAX_XFILE_COUNT ({} != {})", ctx.blocksCount, (int)MAX_XFILE_COUNT));
			}

			for (size_t i = 0; i < MAX_XFILE_COUNT; i++) {
				blockSizes[i] = ctx.blockSizes[i].size;
			}

			DB_AllocXBlocks(blockSizes, gcx.blocks);
			gcx.DB_InitStreams(gcx.blocks);
			gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
			DB_LoadXFileData(&gcx.assetList, sizeof(gcx.assetList));
			gcx.DB_PushStreamPos(XFILE_BLOCK_VIRTUAL);
			*gcx.varScriptStringList = &gcx.assetList;
			gcx.Load_ScriptStringList(false);
			gcx.DB_PopStreamPos();

			std::filesystem::path outStrings{ opt.m_output / "bo3" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };
			std::filesystem::path outAssets{ opt.m_output / "bo3" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname) };
			std::filesystem::path outXStrings{ gcx.opt->m_output / "bo3" / "source" / "tables" / "data" / "xstrings" / std::format("{}.txt", ctx.ffname) };
			std::filesystem::create_directories(outStrings.parent_path());
			std::filesystem::create_directories(outAssets.parent_path());
			{
				utils::OutFileCE os{ outStrings };
				if (!os) {
					throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
				}
				for (size_t i = 0; i < gcx.assetList.stringsCount; i++) {
					os << utils::PtrOrElse(gcx.assetList.strings[i], "") << "\n";
				}
				LOG_OPT_INFO("Dump strings into {}", outStrings.string());
			}

			if (gcx.assetList.unk18) {
				throw std::runtime_error("gcx.assetList.unk18 found: to fix");
			}



			utils::OutFileCE osa{ outAssets, true };
			gcx.osassets = &osa;
			gcx.loaded = 0;
			gcx.xstrings.clear();

			if (gcx.assetList.assets) {
				gcx.assetList.assets = (XAsset_0*)gcx.DB_AllocStreamPos(7);
				Load_Stream(true, gcx.assetList.assets, sizeof(gcx.assetList.assets[0]) * gcx.assetList.assetCount);

				for (size_t i = 0; i < gcx.assetList.assetCount; i++) {
					XAsset_0& asset{ gcx.assetList.assets[i] };

					const char* assType{ T7XAssetName(asset.type) };
					LOG_DEBUG("Load asset {} (0x{:x}) {} / {}", assType, (int)asset.type, i, gcx.assetList.assetCount);

					*gcx.varXAsset = &asset;
					gcx.Load_XAsset(false);
				}
			}
			gcx.DB_PopStreamPos();

			LOG_OPT_INFO("{} asset(s) loaded (0x{:x})", gcx.loaded, gcx.loaded);

			// xstrings
			if (opt.dumpXStrings) {
				std::filesystem::create_directories(outXStrings.parent_path());
				utils::OutFileCE os{ outXStrings, true };


				for (char* xstr : gcx.xstrings) {
					os << xstr << "\n";
				}
				LOG_OPT_INFO("Dump xstrings into {}", outXStrings.string());
			}

			gcx.allocator.FreeAll();
		}
	};

	utils::ArrayAdder<BO3FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
