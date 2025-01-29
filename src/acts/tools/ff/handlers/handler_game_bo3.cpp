#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/bo3/pools.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>

namespace {
	using namespace bo3::pool;

	class BO3FFHandler;

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
		int index;
	};

	struct {
		BO3FFHandler* handler{};
		XAsset_0** varXAsset{};
		void (*Load_XAsset)(bool atStreamStart) {};
		const char*(*DB_GetXAssetName)(T7XAssetType type, void** header) {};
		fastfile::FastFileOption* opt{};
		//core::bytebuffer::ByteBuffer* reader{};

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
			//bo3FFHandlerContext.Reader().Align(align);
		}
	} bo3FFHandlerContext{};

	enum XFileBlock : int {
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_RUNTIME_VIRTUAL = 0x1,
		XFILE_BLOCK_RUNTIME_PHYSICAL = 0x2,
		XFILE_BLOCK_UNK3 = 0x3,
		XFILE_BLOCK_UNK4 = 0x4,
		XFILE_BLOCK_VIRTUAL = 0x5,
		XFILE_BLOCK_PHYSICAL = 0x6,
		XFILE_BLOCK_UNK7 = 0x7,
		XFILE_BLOCK_STREAMER = 0x8,
		XFILE_BLOCK_MEMMAPPED = 0x9,
		MAX_XFILE_COUNT = 0xA,
	};

	bool Load_StreamStub(bool atStreamStart, void* ptr, uint32_t size) {
		auto& top{ bo3FFHandlerContext.Top() };
		LOG_TRACE("{} Load_Stream({}, {}, 0x{:x}) from 0x{:x}/{}", hook::library::CodePointer{ _ReturnAddress() }, atStreamStart, ptr, size, bo3FFHandlerContext.Loc(), top.index);
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
				if (!top.buff.CanRead(size)) {
					hook::error::DumpStackTraceFrom();
				}
				std::memset(ptr, 0, size);
				top.buff.Skip(size);
				ret = false;
				break;
			case XFILE_BLOCK_UNK7:
				return false;
			case XFILE_BLOCK_UNK3:
			case XFILE_BLOCK_UNK4:
			case XFILE_BLOCK_STREAMER:
				if (!top.buff.CanRead(size)) {
					hook::error::DumpStackTraceFrom();
				}
				top.buff.Skip(size);
				ret = false;
				break;
			default:
				throw std::runtime_error(std::format("Invalid stream index {}", top.index));
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
		LOG_TRACE("{} DB_AllocStreamPos({}) -> 0x{:x} / {}", hook::library::CodePointer{ _ReturnAddress() }, alignment, bo3FFHandlerContext.Loc(), bo3FFHandlerContext.Reader().Ptr<void>());
		return bo3FFHandlerContext.Reader().Ptr();
	}

	void Load_SimpleAsset_Internal(void** header, T7XAssetType assetType) {
		goto ignore;
		switch (assetType) {
		case T7_ASSET_TYPE_SCRIPTPARSETREE: {
			struct ScriptParseTree {
				const char* name;
				int32_t len;
				int32_t pad;
				byte* buffer;
			}; static_assert(sizeof(ScriptParseTree) == 0x18);
			ScriptParseTree* spt{ (ScriptParseTree*)*header };


			std::filesystem::path outDir{ bo3FFHandlerContext.opt->m_output / "bo3" / "spt"};

			std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}c", *(uint64_t*)spt->buffer, spt->name) };
			std::filesystem::create_directories(outFile.parent_path());
			if (!utils::WriteFile(outFile, spt->buffer, spt->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			else {
				LOG_INFO("Dump scriptparsetree into {}", outFile.string());
			}
		}
			break;
		case T7_ASSET_TYPE_RAWFILE: {
			struct RawFile {
				const char* name;
				int32_t len;
				byte* buffer;
			}; static_assert(sizeof(RawFile) == 0x18);
			RawFile* asset{ (RawFile*)*header };

			std::filesystem::path outFile{ bo3FFHandlerContext.opt->m_output / "bo3" / "source" / asset->name };

			std::filesystem::create_directories(outFile.parent_path());
			if (!utils::WriteFile(outFile, asset->buffer, asset->len)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
			else {
				LOG_INFO("Dump rawfile into {}", outFile.string());
			}
			break;
		}
		default:
		ignore: {
			LOG_TRACE("Ignoring asset {}/{}", T7XAssetName(assetType), (void*)header);
			break;
		}
		}
	}
	void DB_PopStreamPos() {
		size_t delta{ bo3FFHandlerContext.Reader().Loc() };
		bo3FFHandlerContext.readers.pop_back();
		bo3FFHandlerContext.Reader().Skip(delta);
		LOG_TRACE("{} DB_PopStreamPos -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, bo3FFHandlerContext.Loc());
	}
	void DB_PushStreamPos(int index) {
		core::bytebuffer::ByteBuffer& top{ bo3FFHandlerContext.Reader() };
		int old{ bo3FFHandlerContext.Top().index };
		if (index == XFILE_BLOCK_MEMMAPPED) {
			index = old;
		}
		bo3FFHandlerContext.readers.emplace_back(core::bytebuffer::ByteBuffer{ top.Ptr(), top.Remaining() }, index);
		LOG_TRACE("{} DB_PushStreamPos({}) -> 0x{:x}", hook::library::CodePointer{ _ReturnAddress() }, index, bo3FFHandlerContext.Loc());
	}

	void DB_IncStreamPos(int size) {
		auto& top{ bo3FFHandlerContext.Reader() };
		if (!top.CanRead(size)) {
			hook::error::DumpStackTraceFrom();
		}
		top.Skip(size);
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

	void Load_XStringCustom(const char** str) {
		LOG_TRACE("{} Load_XStringCustom({})", hook::library::CodePointer{ _ReturnAddress() }, (void*)*str);
		LOG_DEBUG("str {}", *str);
		DB_IncStreamPos((int)std::strlen(*str) + 1);
	}

	class BO3FFHandler : public fastfile::FFHandler {
		hook::module_mapper::Module game{};
	public:
		BO3FFHandler() : fastfile::FFHandler("bo3", "Black Ops 3") {
			bo3FFHandlerContext.handler = this;
		}

		void Init(fastfile::FastFileOption& opt) override {
			const char* g{ opt.game ? opt.game : "BlackOps3_dump.exe" };
			if (!game.Load(g)) {
				throw std::runtime_error(std::format("Can't load {}", g));
			}

			hook::library::Library lib{ *game };

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
			hook::memory::RedirectJmp(lib[0x1426160], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1426090], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x22728B0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C050], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x14260C0], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C050], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C050], EmptyStub); // unk
			hook::memory::RedirectJmp(lib[0x1C8C050], EmptyStub); // unk
			
		}
		
		void Cleanup() override {
			game.Free();
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
			std::filesystem::path out{ opt.m_output / "bo3" / ctx.ffname };
			std::filesystem::create_directories(out);


			XAssetList_0& assetList{ *reader.ReadPtr<XAssetList_0>() };

			if (assetList.stringsCount) {
				reader.Align<void*>();
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
				std::filesystem::path outStrings{ out / "strings.txt" };
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
				LOG_TRACE("not asset");
				return;
			}

			LOG_DEBUG("assets: 0x{:x}", reader.Loc());
			// fixme: for some reasons, we have a bad alignment in the uncompressed buffer, this should be fixed before reading
			// it seems to be linked with the PushStreamPos types, a virtual/physical will set a particular align
			// I suggest to use a stack with custom buffers
			//reader.Align<void*>(); 
			assetList.assets = reader.ReadPtr<XAsset_0>(assetList.assetCount);
			LOG_DEBUG("assets data 0x{:x}", reader.Loc());

			std::filesystem::path outAssets{ out / "assets.csv" };
			utils::OutFileCE osa{ outAssets };
			if (!osa) {
				throw std::runtime_error(std::format("Can't open {}", outAssets.string()));
			}
			bo3FFHandlerContext.readers.emplace_back(reader, 0);
			bo3FFHandlerContext.opt = &opt;

			for (size_t i = 0; i < assetList.assetCount; i++) {
				XAsset_0& asset{ assetList.assets[i] };

				LOG_DEBUG("Load asset {} (0x{:x})", T7XAssetName(asset.type), (int)asset.type);

				*bo3FFHandlerContext.varXAsset = &asset;
				bo3FFHandlerContext.Load_XAsset(false);
			}
		}
	};

	utils::ArrayAdder<BO3FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}
