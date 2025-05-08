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
			Asset* assets;
		};

		struct HashedType {
			uint32_t hash;
			T10RAssetType type;
			const char* name;
		};

		struct DBLoadCtx {
			void* __vtb;
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
				bool(__fastcall * LoadStream)(DBLoadCtx * ctx, bool atStreamStart, void* ptr, size_t len);
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

		LoadStreamObjectVtable dbLoadStreamVTable{ LoadStreamImpl, ErrorStub, ErrorStub, ErrorStub , ErrorStub };

		struct {
			BO6FFHandler* handler{};
			void (*Load_Asset)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset) {};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[T10RAssetType::T10R_ASSET_COUNT]{};
			AssetList assets{};
			HashedType typeMaps[0x200];
			size_t typeMapsCount;

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


		void LoadXFileData(void* ptr, int64_t len) {
			throw std::runtime_error("not implemented LoadXFileData");
		}

		bool LoadStream(DBLoadCtx* context, bool atStreamStart, void* data, int64_t len) {
			throw std::runtime_error("not implemented LoadStream");
		}


		bool LoadStreamImpl(LoadStreamObjectData* that, DBLoadCtx* context, bool* atStreamStart, void** data, int64_t* len) {
			// redirect to custom version
			return LoadStream(context, *atStreamStart, *data, *len);
		}

		int32_t GetMappedTypeStub(uint32_t hash) {
			return gcx.GetMappedType(hash)->type;
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

				lib.Redirect("40 53 48 83 EC ? 49 8B D9 4D", LoadStream);
				lib.Redirect("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 48 8B DA 48 8B CA E8 ? ? ? ? 48 8B 0D", LoadXFileData);
				gcx.Load_Asset = lib.ScanSingle("4C 8B DC 49 89 5B ? 57 48 83 EC ? 49 8B D8 48 8B F9 84 D2 74 3C 48 8B 05 ? ? ? ? 4D 8D 4B E8 49 C7 43 ? ? ? ? ? 4D 8D 43 ? 49 89 5B E8 48 8B D1 C6 44 24 ? ? 48 8D 0D ? ? ? ? 4C 8B 10 49 8D 43 ? 49 89 43 D8 41 FF D2 84 C0 74 1C 48")
					.Get<void(*)(DBLoadCtx* ctx, bool atStreamStart, Asset* asset)>();
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				gcx.ctx = &ctx;
				gcx.reader = &reader;
				std::filesystem::path out{ opt.m_output / "bo6" / "data" };
				std::filesystem::create_directories(out);

				reader.Read(&gcx.assets, sizeof(gcx.assets));

				LOG_INFO("assets: {}, strings: {}", gcx.assets.assetsCount, gcx.assets.stringsCount);

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

				if (!gcx.assets.assetsCount) {
					LOG_INFO("no assets to load");
					return;
				}

				std::filesystem::path outAssets{ gcx.opt->m_output / "bo6" / "source" / "tables" / "data" / "assets" / std::format("{}.csv", ctx.ffname) };
				{
					std::filesystem::create_directories(outAssets.parent_path());
					utils::OutFileCE assetsOs{ outAssets };
					assetsOs << "type,name";
					gcx.assets.assets = reader.ReadPtr<Asset>(gcx.assets.assetsCount);

					hook::library::Library lib{ opt.GetGame(true) };

					DBLoadCtx loadCtx{};

					bool err{};
					for (size_t i = 0; i < gcx.assets.assetsCount; i++) {
						Asset* asset{ gcx.assets.assets + i };

						const HashedType* type{ gcx.GetMappedType(asset->type) };

						assetsOs << "\n" << std::hex << type->name << ",<unk>";

						asset->type = type->type;
						gcx.Load_Asset(&loadCtx, false, asset);
					}
				}
				LOG_INFO("Asset names dump into {}", outAssets.string());
			}

		};

		
		utils::ArrayAdder<BO6FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}

	std::unordered_map<uint32_t, Worker*>& GetWorkers() {
		static std::unordered_map<uint32_t, Worker*> workers{};
		return workers;
	}

	const char* GetPoolName(uint32_t hash) {
		return gcx.GetMappedType(hash)->name;
	}
}