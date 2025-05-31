#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/ff/fastfile_dump.hpp>
#include <tools/utils/data_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <hook/error.hpp>
#include <tools/ff/fastfile_asset_pool.hpp>
#include <tools/bo6/bo6.hpp>
#include <decryptutils.hpp>
#include <tools/ff/handlers/handler_game_bo6.hpp>


namespace fastfile::handlers::bo6 {
	using namespace ::bo6;

	namespace {
		template<size_t offset = 0>
		void EmptyStub() {
			LOG_TRACE("{} EmptyStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
		}
		template<size_t offset = 0>
		void ErrorStub() {
			hook::error::DumpStackTraceFrom();
			throw std::runtime_error(std::format("{} ErrorStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset));
		}
		template<size_t offset = 0, typename T, T value>
		T ReturnStub() {
			LOG_TRACE("{} ReturnStub<0x{:x}>", hook::library::CodePointer{ _ReturnAddress() }, offset);
			return value;
		}

		
		class BO6FFHandler;

		struct Asset {
			uint32_t type; // T10RAssetType
			void* handle;
		};

		struct AssetList {
			uint32_t stringsCount;
			bool stringsLoaded;
			const char** strings;
			uint32_t assetsCount;
			uint32_t assetsLoaded;
			uint32_t* unk10;
			uint32_t unk10_count;
			Asset* assets;
		};


		struct HashedType {
			uint32_t hash;
			T10RAssetType type;
			const char* name;
		};

		struct DBLoadCtx;
		struct DBLoadCtxData;
		struct DBLoadCtxVT {
			void*(__fastcall* AllocStreamPos)(DBLoadCtx* ctx, int align);
			void(__fastcall* PushStreamPos)(DBLoadCtx* ctx, int type);
			void(__fastcall* PopStreamPos)(DBLoadCtx* ctx);
			void(__fastcall* PreAssetRead)(DBLoadCtx* ctx, T10RAssetType type);
			void(__fastcall* PostAssetRead)(DBLoadCtx* ctx);
		};

		struct StreamPosInfo {
			byte* pos;
			int index;
		};

		struct DBLoadCtxData {
			uint32_t streamRead[16];
			uint64_t unk40;
			uint64_t unk48;
			uint64_t unk50;
			uint64_t unk58;
			uint64_t unk60;
			uint64_t unk68;
			uint64_t unk70;
			uint64_t unk78;
			uint64_t unk80;
			uint64_t unk88;
			uint64_t unk90;
			uint64_t unk98;
			uint64_t unka0;
			uint64_t unka8;
			uint64_t unkb0;
			uint64_t unkb8;
			uint64_t unkc0;
			uint64_t unkc8;
			uint64_t unkd0;
			uint64_t unkd8;
			uint64_t unke0;
			uint64_t unke8;
			uint64_t unkf0;
			uint64_t unkf8;
			byte* streamPos;
			byte* streamPosArray[16];
			uint32_t streamPosIndex;
			uint32_t blockType;
			StreamPosInfo streamPosInfo[64];
			uint32_t g_streamPosStackIdx;
			uint32_t unk594;
			uint32_t unk598;
			uint32_t unk59c;
			uint64_t unk5a0;
			uint64_t unk5a8;
			uint64_t unk5b0;
			uint64_t unk5b8;
			uint64_t unk5c0;
			uint64_t loadData;
			uint64_t unk5d0;
			uint64_t unk5d8;
			uint64_t unk5e0;
			bool unk5e8;
			uint64_t unk5f0;
			uint32_t unk5f4;
			byte* streamPos2;
		};

		struct DBLoadCtx {
			DBLoadCtxVT* __vtb;
			DBLoadCtxData data;
		};


		union LoadStreamObjectData;

		struct LoadStreamObjectVtable {
			bool(__fastcall* LoadStream)(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len);
			void(__fastcall* copy2)(LoadStreamObjectData* that);
			void(__fastcall* copy3)(LoadStreamObjectData* that);
			void(__fastcall* f4)(LoadStreamObjectData* that);
			void(__fastcall* nulled)(LoadStreamObjectData* that);
		};

		union LoadStreamObjectData {
			struct {
				bool(*LoadStream)(DBLoadCtx* ctx, bool atStreamStart, void* ptr, size_t len);
				uint64_t unk08;
				uint64_t unk10;
				uint64_t unk18;
			} loadStream;
		};
		static_assert(sizeof(LoadStreamObjectData) == 0x20);
		struct LoadStreamObject {
			LoadStreamObjectVtable* __vtb;
			LoadStreamObjectData data;
		};


		bool LoadStreamImpl(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len);
		void ErrorStub(LoadStreamObjectData* that) {
			throw std::runtime_error(std::format("Error loadstream {}", hook::library::CodePointer{ _ReturnAddress() }));
		}
		void* AllocStreamPos(DBLoadCtx* ctx, int align);
		void PushStreamPos(DBLoadCtx* ctx, int type);
		void PopStreamPos(DBLoadCtx* ctx);
		void PreAssetRead(DBLoadCtx* ctx, T10RAssetType type);
		void PostAssetRead(DBLoadCtx* ctx);


		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };
		DBLoadCtxVT dbLoadCtxVTable{ AllocStreamPos, PushStreamPos, PopStreamPos, PreAssetRead, PostAssetRead };

		struct {
			BO6FFHandler* handler{};
			void (*Load_Asset)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset) {};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			int streamPosStack[64]{};
			int streamPosStackIndex{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[T10RAssetType::T10R_ASSET_COUNT]{};
			AssetList assets{};
			HashedType typeMaps[0x200];
			size_t typeMapsCount;
			utils::OutFileCE* outAsset{};

			void InitTypeMaps(hook::library::Library& lib) {
				// that:
				// acts exe_pool_dumper .\output_bo6\Cordycep-2.5.8.0\Data\Dumps\cod_dump.exe  physicslibrary string
				constexpr const char* first = "physicslibrary";
				constexpr const char* last = "string";
				for (hook::library::ScanResult& firstStringOffsetRes : lib.ScanString(first)) {
					uintptr_t firstStringOffset{ firstStringOffsetRes.GetPtr<uintptr_t>() };
					LOG_TRACE("try string \"{}\" -> 0x{:x}", first, lib.Rloc(firstStringOffset));
					for (hook::library::ScanResult& poolNamesRes : lib.ScanNumber(firstStringOffset)) {
						uintptr_t poolNamesOffset{ poolNamesRes.GetPtr<uintptr_t>() };

						uintptr_t* poolNames{ reinterpret_cast<uintptr_t*>(poolNamesOffset) };
						LOG_TRACE("try poolNames -> 0x{:x}", lib.Rloc(poolNamesOffset));

						// we can try to see if the next one is a valid string

						void* next{ reinterpret_cast<void*>(poolNames[1]) };

						if (next < *lib || next > lib[0x10000000]) {
							LOG_TRACE("Not inside library");
							continue; // not inside the module
						}


						size_t count{};
						while (true) {
							if (!poolNames[count]) {
								LOG_TRACE("Can't find last pool name"); // cw?
								count = 0;
								break;
							}
							const char* cc = lib.Rebase<const char>(poolNames[count]);
							if (last && !_strcmpi(cc, last)) break;
							count++;
						}

						if (count <= 40) {
							LOG_TRACE("Not enough candidates: {}", count);
							continue;
						}

						if (count > ARRAYSIZE(typeMaps)) {
							LOG_TRACE("Too many candidates: {}", count);
							continue;
						}

						typeMapsCount = count;
						for (size_t i = 0; i < count; i++) {
							HashedType* type = &typeMaps[i];
							type->type = (T10RAssetType)i;
							type->name = lib.Rebase<const char>(poolNames[i]);
							type->hash = (uint32_t)hash::HashX32(type->name);
						}

						// sort it for better usage
						std::sort(typeMaps, typeMaps + typeMapsCount, [](auto& a, auto& b) { return a.hash < b.hash; });
						LOG_DEBUG("{} asset names loaded", typeMapsCount);
						return;
					}
					throw std::runtime_error("Can't scan asset pool names");
				}
			}

			const HashedType* GetMappedType(uint32_t val) const {
				size_t min{}, max{ typeMapsCount };

				while (min < max) {
					size_t mid{ (min + max) / 2 };
					if (typeMaps[mid].hash > val) {
						max = mid;
					}
					else if (typeMaps[mid].hash != val) {
						min = mid + 1;
					}
					else {
						return &typeMaps[mid];
					}
				}
				throw std::runtime_error(std::format("Invalid asset type name {}", hashutils::ExtractTmp("hash", val)));
			}

		} gcx{};


		void LoadXFileData(DBLoadCtx* context, void* ptr, int64_t len) {
			LOG_TRACE("LoadXFileData({}, {}) {}", ptr, len, hook::library::CodePointer{ _ReturnAddress() });
			gcx.reader->Read(ptr, len);
		}

		bool LoadStream(DBLoadCtx* context, bool atStreamStart, void* ptr, int64_t len) {
			LOG_TRACE("LoadStream({}, {}, {}) {}", atStreamStart, ptr, len, hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.streamPosStackIndex) throw std::runtime_error("empty streampos stack");

			int block{ gcx.streamPosStack[gcx.streamPosStackIndex - 1] };

			if (block == 11) {
				return false;
			}
			else {
				LoadXFileData(context, ptr, len);
				return true;
			}

		}

		void Load_String(DBLoadCtx* context, char** pstr) {
			char* str{ *pstr };
			do {
				gcx.reader->Read(str, 1);
			} while (*str++);
		}
		void Load_CustomScriptString(DBLoadCtx* context, uint32_t* pstr) {
			uint32_t k;
			LoadXFileData(context, &k, sizeof(k));
		}

		void* AllocStreamPos(DBLoadCtx* ctx, int align) {
			LOG_TRACE("AllocStreamPos({}) {}", align, hook::library::CodePointer{ _ReturnAddress() });
			if (gcx.reader->CanRead(1)) {
				return gcx.reader->Ptr();
			}
			else {
				static char empty[1];
				return empty;
			}
		}

		void PushStreamPos(DBLoadCtx* ctx, int type) {
			LOG_TRACE("PushStreamPos({}) {}", type, hook::library::CodePointer{ _ReturnAddress() });
			gcx.streamPosStack[gcx.streamPosStackIndex++] = type;
		}

		void PopStreamPos(DBLoadCtx* ctx) {
			LOG_TRACE("PopStreamPos() {}", hook::library::CodePointer{ _ReturnAddress() });
			if (!gcx.streamPosStackIndex) throw std::runtime_error("Can't pop empty stack");
			gcx.streamPosStackIndex--;
		}

		void PreAssetRead(DBLoadCtx* ctx, T10RAssetType type) {
			LOG_TRACE("PreAssetRead({}) {}", type, hook::library::CodePointer{ _ReturnAddress() });
		}

		void PostAssetRead(DBLoadCtx* ctx) {
			LOG_TRACE("PostAssetRead() {}", hook::library::CodePointer{ _ReturnAddress() });
		}


		bool LoadStreamImpl(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len) {
			// redirect to custom version
			return LoadStream(context, *atStreamStart, *data, *len);
		}

		int32_t GetMappedTypeStub(uint32_t hash) {
			return gcx.GetMappedType(hash)->type;
		}

		void* DB_LinkGenericXAsset(DBLoadCtx* ctx, T10RAssetType type, void** handle) {
			const char* name{ hashutils::ExtractTmp("hash", handle ? *(uint64_t*)*handle : 0) };
			*(gcx.outAsset) << "\n" << type << ",#" << name;
			LOG_DEBUG("DB_LinkGenericXAsset({}, '{}') {}", type, name, hook::library::CodePointer{_ReturnAddress()});

			if (handle) {
				std::unordered_map<bo6::T10RAssetType, Worker*>& map{ GetWorkers() };
				auto it{ map.find(type) };
				if (it != map.end()) {
					it->second->Unlink(*gcx.opt, *handle);
				}
			}

			return handle ? *handle : nullptr;
		}

		void* DB_LinkGenericXAssetEx(T10RAssetType type, uint64_t name, void** handle) {
			LOG_DEBUG("DB_LinkGenericXAssetEx({}, '{}') {}", type, hashutils::ExtractTmp("hash", name), hook::library::CodePointer{_ReturnAddress()});
			return handle ? *handle : nullptr;
		}

		uint32_t* Unk_Align_Ret(DBLoadCtx* ctx) {
			static uint32_t data{};
			return &data;
		}

		class BO6FFHandler : public fastfile::FFHandler {
		public:
			BO6FFHandler() : fastfile::FFHandler("bo6", "Black Ops 6") {
				gcx.handler = this;
			}

			void Init(fastfile::FastFileOption& opt) override {
				hook::library::Library lib{ opt.GetGame(true) };

				gcx.opt = &opt;

				if (!acts::decryptutils::LoadDecryptModule(lib)) {
					throw std::runtime_error("Can't load decryption module");
				}

				gcx.InitTypeMaps(lib);

				lib.Redirect("40 56 41 56 48 83 EC ? 48 8B 15", GetMappedTypeStub);

				LoadStreamObject* loadStreamObj{ lib.ScanAny("48 8B 05 ? ? ? ? 4C 8D 4C 24 ? 48 C7 44 24 ? ? ? ? ? 4C 8D 44 24 ? 48 89 6C 24 ?").GetRelative<int32_t, LoadStreamObject*>(3) };
				loadStreamObj->__vtb = &dbLoadStreamVTable;

				lib.Redirect("48 89 5C 24 ? 57 48 83 EC ? 49 8B F9 4D 8B C8 48 8B D9", LoadStream);
				lib.Redirect("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 2A 48 8B F2", Load_String); // scr
				lib.Redirect("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 32 41", Load_String); // str
				lib.Redirect("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B D8 8B EA", DB_LinkGenericXAsset);
				lib.Redirect("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 49 8B E8 48 8B DA 8B", DB_LinkGenericXAssetEx);
				lib.Redirect("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B FA 41 B8", Load_CustomScriptString);
				// Stream delta, todo
				lib.Redirect("4C 8B DC 48 83 EC ?? 8B 05 ?? ?? ?? ?? 4C 8B C1 85 C0 0F 84 1B", EmptyStub<0>); // 2DD6730
				lib.Redirect("48 89 5C 24 ?? 48 89 6C 24 ?? 56 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 48 8B DA", EmptyStub<1>); //2E24F20
				lib.Redirect("4C 8B DC 48 83 EC ?? 8B 05 ?? ?? ?? ?? 4C 8B C1 85 C0 0F 84 33", EmptyStub<2>);
				lib.Redirect("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 81 ?? ?? ?? ?? 4C 8B CA", EmptyStub<3>);

				// idk
				lib.Redirect("8B 81 ?? ?? ?? ?? 48 8D 14 40 83", ReturnStub<4, bool, false>);
				lib.Redirect("C5 FB 10 02 44", EmptyStub<5>); // 2DE3F00
				lib.Redirect("8B 81 ?? ?? ?? ?? 48 8D 04 40 48", Unk_Align_Ret); // 2DE3CC0
				// remove
				lib.Redirect("40 53 48 83 EC ?? 41 8B 40 ?? 49", EmptyStub<7>); // image
				lib.Redirect("48 89 5C 24 ?? 57 48 83 EC ?? 49 8B D8 48 8B FA B9", EmptyStub<8>);
				lib.Redirect("48 8B C4 53 48 81 EC ?? ?? ?? ?? 41 0F B7", EmptyStub<9>);
				lib.Redirect("40 53 48 83 EC ?? 8B 42 ?? 49", EmptyStub<10>);
				lib.Redirect("48 8B 05 ?? ?? ?? ?? 0F B7 80", EmptyStub<11>); // sound
				lib.Redirect("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 41 0F B7 D8 0F", EmptyStub<12>); // sound
				lib.Redirect("40 53 48 83 EC ?? 81 61", EmptyStub<13>); // model
				lib.Redirect("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 41 56 41 57 48 83 EC ?? 0F B6 F2", EmptyStub<14>); // streaminginfo
				lib.Redirect("48 83 EC ?? E8 ?? ?? ?? ?? 83 F8 FF 75", EmptyStub<15>); // computeshaders
				lib.Redirect("40 53 55 56 57 41 57 48 83 EC ?? 8B 1D", EmptyStub<16>); // computeshaders
				lib.Redirect("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8 48 89 03 E8 ?? ?? ?? ?? 48 8B", EmptyStub<17>); // computeshaders, TODO: better
				lib.Redirect("40 53 48 83 EC ?? 48 8B D9 E8 ?? ?? ?? ?? 48 89 43 ?? 48 8B", EmptyStub<18>); // libshared
				lib.Redirect("40 53 48 83 EC ?? 48 8B 02 4C 8D 44 24 ?? 48 8B DA 48 89 44 24 ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 89 03 E8 ?? ?? ?? ?? E8", EmptyStub<19>); // libshared, TODO: better
				lib.Redirect("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 4C 24 ?? 56 57 41 54 41 56 41 57 48 83 EC ?? 45 33", EmptyStub<20>); // dlogschema


				//E8 ? ? ? ? 80 3E 00 74 1E
				//lib.Redirect("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 48 8B DA 48 8B CA E8 ? ? ? ? 48 8B 0D", LoadXFileData);
				gcx.Load_Asset = lib.ScanSingle("4C 8B DC 49 89 5B ? 57 48 83 EC ? 49 8B D8 48 8B F9 84 D2 74 3C 48 8B 05 ? ? ? ? 4D 8D 4B E8 49 C7 43 ? ? ? ? ? 4D 8D 43 ? 49 89 5B E8 48 8B D1 C6 44 24 ? ? 48 8D 0D ? ? ? ? 4C 8B 10 49 8D 43 ? 49 89 43 D8 41 FF D2 84 C0 74 1C 48")
					.GetPtr<void(*)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset)>();
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				//HwBp::Set(&gcx.reader, 8, HwBp::When::Written);
				std::filesystem::path out{ opt.m_output / "bo6" / "data" };
				std::filesystem::create_directories(out);

				reader.Read(&gcx.assets, sizeof(gcx.assets));

				LOG_INFO("assets: {}, strings: {}, unk: {}", gcx.assets.assetsCount, gcx.assets.stringsCount, gcx.assets.unk10_count);

				std::filesystem::path outStrings{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "strings" / std::format("{}.txt", ctx.ffname) };

				{
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE stringsOs{ outStrings };
					gcx.assets.strings = reader.ReadPtr<const char*>(gcx.assets.stringsCount);
					for (size_t i = 0; i < gcx.assets.stringsCount; i++) {
						const char* str;
						if (gcx.assets.strings[i]) {
							str = acts::decryptutils::DecryptString(reader.ReadString());
						}
						else {
							str = "";
						}
						gcx.assets.strings[i] = str;
						stringsOs << i << "\t" << str << "\n";
					}
				}

				LOG_INFO("String dump into {}", outStrings.string());
				LOG_DEBUG("string end at 0x{:x}", reader.Loc());

				gcx.assets.unk10 = reader.ReadPtr<uint32_t>(gcx.assets.unk10_count);
				// idk
				//std::filesystem::path outUnk{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "unk" / std::format("{}.csv", ctx.ffname) };
				//{
				//	std::filesystem::create_directories(outUnk.parent_path());
				//	utils::OutFileCE stringsOs{ outUnk };
				//	for (size_t i = 0; i < gcx.assets.unk10_count; i++) {
				//		stringsOs << i << ",0x" << std::hex << gcx.assets.unk10[i] << "," << hashutils::ExtractTmp("hash", gcx.assets.unk10[i]) << "\n";
				//	}
				//}
				//
				//LOG_INFO("unk dump into {}", outUnk.string());

				if (!gcx.assets.assetsCount) {
					LOG_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					gcx.outAsset = &assetsOs;
					assetsOs << "type,name";
					gcx.assets.assets = reader.ReadPtr<Asset>(gcx.assets.assetsCount);

					hook::library::Library lib{ opt.GetGame(true) };

					DBLoadCtx loadCtx{};
					DBLoadCtxVT* vt = &dbLoadCtxVTable;

					for (auto& [k, v] : GetWorkers()) {
						v->PreXFileLoading(*gcx.opt, ctx);
					}

					bool err{};
					for (size_t i = 0; i < gcx.assets.assetsCount; i++) {
						Asset* asset{ gcx.assets.assets + i };

						const HashedType* type{ gcx.GetMappedType(asset->type) };

						LOG_DEBUG("load #{} -> {}", i, type->name);
						if (opt.noAssetDump) {
							assetsOs << "\n" << std::hex << type->name << ",<unk>";
						}
						else {
							gcx.Load_Asset((DBLoadCtx*)&vt, false, asset);
						}
					}

					for (auto& [k, v] : GetWorkers()) {
						v->PostXFileLoading(*gcx.opt, ctx);
					}
				}
				LOG_INFO("Asset names dump into {}", outAssets.string());
			}

		};

		
		utils::ArrayAdder<BO6FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}

	std::unordered_map<bo6::T10RAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<bo6::T10RAssetType, Worker*> workers{};
		return workers;
	}

	const char* GetPoolName(uint32_t hash) {
		return gcx.GetMappedType(hash)->name;
	}
}