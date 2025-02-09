#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <games/bo4/pool.hpp>
#include <decryptutils.hpp>

namespace {
	using namespace games::bo4::pool;

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

	void Load_SimpleAsset_Internal(void** header, XAssetType assetType) {
		bo4FFHandlerContext.loaded++;

		if (bo4FFHandlerContext.opt->assertContainer) {
			// todo: add header name
			bo4FFHandlerContext.pool.AddAssetHeader(assetType, 0, *header);
		}

		if (bo4FFHandlerContext.opt->noAssetDump) return; // ignore
		switch (assetType) {
		case ASSET_TYPE_SCRIPTPARSETREE: {
			struct ScriptParseTree {
				const char* name;
				int32_t len;
				int32_t pad;
				byte* buffer;
			}; static_assert(sizeof(ScriptParseTree) == 0x18);
			ScriptParseTree* spt{ (ScriptParseTree*)*header };


			std::filesystem::path outDir{ bo4FFHandlerContext.opt->m_output / "bo4" / "spt" };

			if (!FixupName(spt->name)) {
				break;
			}

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}c", *(uint64_t*)spt->buffer, spt->name) };
			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump scriptparsetree {} {} 0x{:x}", outFile.string(), (void*)spt->buffer, spt->len);
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
										  break;
		case ASSET_TYPE_RAWFILE: {
			struct RawFile {
				const char* name;
				int32_t len;
				byte* buffer;
			}; static_assert(sizeof(RawFile) == 0x18);
			RawFile* asset{ (RawFile*)*header };

			if (!FixupName(asset->name)) {
				break;
			}

			std::filesystem::path outFile{ bo4FFHandlerContext.opt->m_output / "bo4" / "source" / asset->name };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_INFO("Dump raw file {} {} 0x{:x}", outFile.string(), (void*)asset->buffer, asset->len);
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			break;
		}
		default: {
			LOG_TRACE("Ignoring asset {}/{}", XAssetNameFromId(assetType), (void*)header);
			break;
		}
		}
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

			hook::memory::RedirectJmp(lib[0x2EBC050], Load_StreamStub);
			hook::memory::RedirectJmp(lib[0x2EBBBA0], DB_AllocStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBBFB0], DB_ConvertOffsetToAlias);
			hook::memory::RedirectJmp(lib[0x2EBB6F0], Load_SimpleAsset_Internal);
			hook::memory::RedirectJmp(lib[0x2EBBFF0], DB_ConvertOffsetToPointer);
			hook::memory::RedirectJmp(lib[0x2EBBCC0], DB_InsertPointer);
			hook::memory::RedirectJmp(lib[0x2EBBBE0], DB_IncStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBC110], Load_XStringCustom);
			hook::memory::RedirectJmp(lib[0x2EBBE20], DB_PopStreamPos);
			hook::memory::RedirectJmp(lib[0x2EBBEA0], DB_PushStreamPos);
			bo4FFHandlerContext.Load_XAsset = reinterpret_cast<decltype(bo4FFHandlerContext.Load_XAsset)>(lib[0x2E35D10]);

			/*

			bo4FFHandlerContext.DB_GetXAssetName = reinterpret_cast<decltype(bo4FFHandlerContext.DB_GetXAssetName)>(lib[0x13E9D80]);

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

			if (assetList.stringsCount) {
				//reader.Align<void*>();
				assetList.strings = reader.ReadPtr<char*>(assetList.stringsCount);

				for (size_t i = 0; i < assetList.stringsCount; i++) {
					if (fastfile::IsSame(assetList.strings[i], -1)) {
						assetList.strings[i] = reader.ReadString();
					}
					else {

						assetList.strings[i] = emptyStr;
					}
				}
			}

			{
				std::filesystem::path outStrings{ out / std::format("{}_strings.txt", ctx.ffname) };
				utils::OutFileCE os{ outStrings };
				if (!os) {
					throw std::runtime_error(std::format("Can't open {}", outStrings.string()));
				}
				for (size_t i = 0; i < assetList.stringsCount; i++) {
					os << acts::decryptutils::DecryptStringT8(assetList.strings[i]) << "\n";
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
			bo4FFHandlerContext.readers.emplace_back(reader, XFILE_BLOCK_TEMP);
			bo4FFHandlerContext.opt = &opt;
			bo4FFHandlerContext.osassets = &osa;
			bo4FFHandlerContext.loaded = 0;

			for (size_t i = 0; i < assetList.assetCount; i++) {
				XAsset_0& asset{ assetList.assets[i] };

				LOG_DEBUG("Load asset {} (0x{:x})", XAssetNameFromId(asset.type), (int)asset.type);

				bo4FFHandlerContext.Load_XAsset(false, &asset);
			}

			LOG_INFO("{} asset(s) loaded (0x{:x})", bo4FFHandlerContext.loaded, bo4FFHandlerContext.loaded);

		}
	};

	utils::ArrayAdder<BO4FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
