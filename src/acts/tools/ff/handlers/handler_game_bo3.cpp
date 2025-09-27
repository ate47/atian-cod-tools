#include <includes.hpp>
#include <utils/enumlist.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/bo3/pools.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <tools/ff/handlers/handler_game_bo3.hpp>

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
	struct XFile
	{
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		fastfile::FastFileCompression compression;
		fastfile::FastFilePlatform platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
		uint32_t metaVersion;
		char mergeFastfile[64];
		uint64_t size;
		uint64_t externalSize;
		uint64_t memMappedOffset;
		uint64_t blockSize[10];
		char fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
	};

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

		if (id >= ARRAYSIZE(names) || id < 0) {
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
		BO3FFHandler* handler{};
		XAsset_0** varXAsset{};
		void (*Load_XAsset)(bool atStreamStart) {};
		const char*(*DB_GetXAssetName)(T7XAssetType type, void** header) {};
		fastfile::FastFileOption* opt{};
		size_t loaded{};
		utils::OutFileCE* osassets{};
		//core::bytebuffer::ByteBuffer* reader{};
		fastfile::pool::FFAssetPool pool{};
		utils::EnumList<T7XAssetType, T7XAssetType::T7_ASSET_TYPE_COUNT> handleList{ bo3::pool::T7XAssetIdFromName };
		std::vector<StreamRead> readers{};
		std::vector<char*> xstrings{};

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
			//bo3FFHandlerContext.Reader().Align(align);
		}
	} bo3FFHandlerContext{};

	bool Load_StreamStub(bool atStreamStart, void* ptr, uint32_t size) {
		auto& top{ bo3FFHandlerContext.Top() };
		LOG_TRACE("{} Load_Stream({}, {}, 0x{:x}) from 0x{:x}/{}", hook::library::CodePointer{ _ReturnAddress() }, atStreamStart, ptr, size, bo3FFHandlerContext.Loc(), XFileBlockName(top.index));
		bool ret;
		if (atStreamStart && size) {
			switch (top.index) {
			case XFILE_BLOCK_TEMP:
			case XFILE_BLOCK_VIRTUAL:
			case XFILE_BLOCK_PHYSICAL:
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
			case XFILE_BLOCK_STREAMER_RESERVE:
				return false;
			case XFILE_BLOCK_DELAY_VIRTUAL:
			case XFILE_BLOCK_DELAY_PHYSICAL:
			case XFILE_BLOCK_STREAMER:
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
		bo3FFHandlerContext.Align((size_t)alignment + 1);
		LOG_TRACE("{} DB_AllocStreamPos(0x{:x}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, alignment + 1, bo3FFHandlerContext.Loc(), bo3FFHandlerContext.Reader().Ptr<void>());
		return bo3FFHandlerContext.Reader().Ptr();
	}

	void Load_SimpleAsset_Internal(void** header, T7XAssetType assetType) {
		bo3FFHandlerContext.loaded++;

		if (bo3FFHandlerContext.opt->assertContainer) {
			// todo: add header name
			bo3FFHandlerContext.pool.AddAssetHeader(assetType, 0, *header);
		}

		if (bo3FFHandlerContext.opt->noAssetDump || (!bo3FFHandlerContext.handleList.Empty() && !bo3FFHandlerContext.handleList[assetType])) return; // ignore

		auto& workers{ fastfile::handlers::bo3::GetWorkers() };

		auto it{ workers.find(assetType) };
		if (it != workers.end()) {
			it->second->Unlink(*bo3FFHandlerContext.opt, *header);
		}
		else {
			LOG_DEBUG("Ignoring asset {}/{}", T7XAssetName(assetType), (void*)header);
		}
	}
	void DB_PopStreamPos() {
		size_t delta{ bo3FFHandlerContext.Reader().Loc() };
		bo3FFHandlerContext.readers.pop_back();
		bo3FFHandlerContext.Reader().Skip(delta);
		LOG_TRACE("{} DB_PopStreamPos -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, bo3FFHandlerContext.Loc());
	}
	void DB_PushStreamPos(XFileBlock index) {
		core::bytebuffer::ByteBuffer& top{ bo3FFHandlerContext.Reader() };
		XFileBlock old{ bo3FFHandlerContext.Top().index };
		if (index == XFILE_BLOCK_MEMMAPPED) {
			index = old;
		}
		bo3FFHandlerContext.readers.emplace_back(core::bytebuffer::ByteBuffer{ top.Ptr(), top.Remaining() }, index);
		LOG_TRACE("{} DB_PushStreamPos({}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, XFileBlockName(index), bo3FFHandlerContext.Loc());
	}

	void DB_IncStreamPos(int size) {
		//auto& top{ bo3FFHandlerContext.Reader() };
		//if (!top.CanRead(size)) {
		//	hook::error::DumpStackTraceFrom();
		//}
		//top.Skip(size);
		LOG_TRACE("{} DB_IncStreamPos(0x{:x}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, size, bo3FFHandlerContext.Loc());
	}

	void DB_ConvertOffsetToAlias(void* data) {
		LOG_TRACE("{} DB_ConvertOffsetToAlias", hook::library::CodePointer{ _ReturnAddress() });
	}
	void DB_ConvertOffsetToPointer(void* data) {
		LOG_TRACE("{} DB_ConvertOffsetToPointer", hook::library::CodePointer{ _ReturnAddress() });
	}

	void** DB_InsertPointer() {
		bo3FFHandlerContext.Align(8);
		void** ptr{ bo3FFHandlerContext.Reader().Ptr<void*>() };

		LOG_TRACE("{} DB_InsertPointer {} -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)ptr, bo3FFHandlerContext.Loc());
		return ptr;
	}

	void EmptyStub() {
		LOG_TRACE("{} EmptyStub", hook::library::CodePointer{ _ReturnAddress() });
	}

	void Load_XStringCustom(char** str) {
		LOG_TRACE("{} Load_XStringCustom({}) 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, (void*)*str, bo3FFHandlerContext.Loc());
		auto& top{ bo3FFHandlerContext.Reader() };
		size_t size;
		const char* ptr{ top.ReadString(&size) };
		if (ptr != *str) {
			std::memcpy(*str, ptr, size + 1);
		}
		if (bo3FFHandlerContext.opt->dumpXStrings) {
			bo3FFHandlerContext.xstrings.push_back(*str);
		}
		LOG_DEBUG("str {}", *str);
		DB_IncStreamPos((int)size);
	}

	class BO3FFHandler : public fastfile::FFHandler {
	public:
		BO3FFHandler() : fastfile::FFHandler("bo3", "Black Ops 3") {
			bo3FFHandlerContext.handler = this;
		}

		void Init(fastfile::FastFileOption& opt) override {
			hook::library::Library lib{ opt.GetGame(true, nullptr, false, "BlackOps3_dump.exe") };

			bo3FFHandlerContext.handleList.Clear();
			if (opt.assetTypes) {
				bo3FFHandlerContext.handleList.LoadConfig(opt.assetTypes);
			}

			bo3FFHandlerContext.varXAsset = lib.Get<XAsset_0*>(0x940E838);
			bo3FFHandlerContext.Load_XAsset = reinterpret_cast<decltype(bo3FFHandlerContext.Load_XAsset)>(lib[0x140E200]);
			bo3FFHandlerContext.DB_GetXAssetName = reinterpret_cast<decltype(bo3FFHandlerContext.DB_GetXAssetName)>(lib[0x13E9D80]);

			hook::memory::RedirectJmp(lib[0x14266D0], Load_StreamStub);
			hook::memory::RedirectJmp(lib[0x1426390], DB_AllocStreamPos);
			hook::memory::RedirectJmp(lib[0x1426190], Load_SimpleAsset_Internal);
			hook::memory::RedirectJmp(lib[0x1426620], DB_ConvertOffsetToAlias);
			hook::memory::RedirectJmp(lib[0x1426650], DB_ConvertOffsetToPointer);
			hook::memory::RedirectJmp(lib[0x14263B0], DB_IncStreamPos);
			hook::memory::RedirectJmp(lib[0x1426460], DB_InsertPointer);
			hook::memory::RedirectJmp(lib[0x14267E0], Load_XStringCustom);
			hook::memory::RedirectJmp(lib[0x1426500], DB_PopStreamPos);
			hook::memory::RedirectJmp(lib[0x1426580], DB_PushStreamPos);

			hook::memory::RedirectJmp(lib[0x141F4C0], EmptyStub); // link image
			hook::memory::RedirectJmp(lib[0x1CB6F20], EmptyStub); // load texture
			hook::memory::RedirectJmp(lib[0x1426800], EmptyStub); // Load_ScriptStringCustom, use idx
			hook::memory::RedirectJmp(lib[0x14261B0], EmptyStub); // Load_SndBankAsset
			hook::memory::RedirectJmp(lib[0x1CD4190], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD45B0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD4550], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD4380], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD4430], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD47C0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1D27810], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1D17660], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x22728B0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x22728F0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C050], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x14260C0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C190], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1426160], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1426090], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x14260F0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8BFB0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1CD4610], EmptyStub); // unk
			
			// write pool data to disk
			if (opt.assertContainer) {
				bo3FFHandlerContext.pool.csiHeader.gameId = compatibility::scobalula::csi::CG_BO3;
				bo3FFHandlerContext.pool.csiHeader.WriteCsi(compatibility::scobalula::csi::ActsCsiLocation());
			}
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
			std::filesystem::path out{ opt.m_output / "bo3" / "data"};
			std::filesystem::create_directories(out);


			XAssetList_0& assetList{ *reader.ReadPtr<XAssetList_0>() };

			if (assetList.stringsCount) {
				//reader.Align<void*>();
				assetList.strings = reader.ReadPtr<const char*>(assetList.stringsCount);

				for (size_t i = 0; i < assetList.stringsCount; i++) {
					if (fastfile::IsSame(assetList.strings[i], -1)) {
						assetList.strings[i] = reader.ReadString();
					}
					else {

						assetList.strings[i] = "";
					}
				}
			}

			{
				std::filesystem::path outStrings{ opt.m_output / "bo3" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };
				std::filesystem::create_directories(outStrings.parent_path());
				utils::OutFileCE os{ outStrings };
				if (!os) {
					throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
				}
				for (size_t i = 0; i < assetList.stringsCount; i++) {
					os << assetList.strings[i] << "\n";
				}
				LOG_INFO("Dump strings into {}", outStrings.string());
			}

			if (assetList.unk18) {
				throw std::runtime_error("assetList.unk18");
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
			bo3FFHandlerContext.readers.emplace_back(reader, XFILE_BLOCK_TEMP);
			bo3FFHandlerContext.opt = &opt;
			bo3FFHandlerContext.osassets = &osa;
			bo3FFHandlerContext.loaded = 0;
			bo3FFHandlerContext.xstrings.clear();


			for (size_t i = 0; i < assetList.assetCount; i++) {
				XAsset_0& asset{ assetList.assets[i] };

				const char* assType{ T7XAssetName(asset.type) };
				LOG_DEBUG("Load asset {} (0x{:x})", assType, (int)asset.type);
				std::filesystem::path binout{ opt.m_output / "bo3" / "binary" / ctx.ffname };
				if (opt.dumpBinaryAssets) {
					std::filesystem::create_directories(binout);
				}

				*bo3FFHandlerContext.varXAsset = &asset;
				size_t originLoc{ bo3FFHandlerContext.Loc() };
				bo3FFHandlerContext.Load_XAsset(false);
				size_t endLoc{ bo3FFHandlerContext.Loc() };

				if (opt.dumpBinaryAssets) {
					std::vector<byte> rawAsset{};

					utils::WriteValue(rawAsset, &asset, sizeof(asset));
					reader.Goto(originLoc);
					size_t len{ endLoc - originLoc };
					utils::WriteValue(rawAsset, reader.ReadPtr<byte>(len), len);

					std::filesystem::path assetOut{ binout / std::format("{:04}_{}.bin", i, assType) };

					if (utils::WriteFile(assetOut, rawAsset)) {
						LOG_INFO("Dump asset {}", assetOut.string());
					}
					else {
						LOG_ERROR("Error when dumping {}", assetOut.string());
					}
				}
			}

			LOG_INFO("{} asset(s) loaded (0x{:x})", bo3FFHandlerContext.loaded, bo3FFHandlerContext.loaded);

			// xstrings
			if (opt.dumpXStrings) {
				std::filesystem::path outStrings{ bo3FFHandlerContext.opt->m_output / "bo3" / "source" / "tables" / "data" / "xstrings" / std::format("{}.txt", ctx.ffname) };
				std::filesystem::create_directories(outStrings.parent_path());
				utils::OutFileCE os{ outStrings, true };


				for (char* xstr : bo3FFHandlerContext.xstrings) {
					os << xstr << "\n";
				}
				LOG_INFO("Dump xstrings into {}", outStrings.string());
			}

		}
	};

	utils::ArrayAdder<BO3FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
