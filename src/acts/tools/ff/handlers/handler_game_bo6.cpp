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

		struct {
			BO6FFHandler* handler{};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[T10RAssetType::T10R_ASSET_COUNT]{};
			AssetList assets{};
			HashedType typeMaps[0x200];
			size_t typeMapsCount;
			bool typeMapsLoaded{};

			void InitTypeMaps(hook::library::Library& lib) {
				if (!typeMapsLoaded) {
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
							typeMapsLoaded = true;
							LOG_DEBUG("{} asset names loaded", typeMapsCount);
							return;
						}
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

					bool err{};
					for (size_t i = 0; i < gcx.assets.assetsCount; i++) {
						Asset* asset{ gcx.assets.assets + i };

						const HashedType* type{ gcx.GetMappedType(asset->type) };

						assetsOs << "\n" << std::hex << type->name << ",<unk>";

						//asset->type = type->type;
						// load_asset
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