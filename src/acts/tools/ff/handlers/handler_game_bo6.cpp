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

	std::unordered_map<T10RAssetType, Worker*>& GetWorkers() {
		static std::unordered_map<T10RAssetType, Worker*> workers{};
		return workers;
	}

	namespace {
		class BO6FFHandler;

		struct Asset {
			T10RAssetType type;
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


		struct {
			BO6FFHandler* handler{};

			fastfile::FastFileOption* opt{};
			fastfile::FastFileContext* ctx{};
			core::bytebuffer::ByteBuffer* reader{};
			size_t loaded{};
			core::memory_allocator::MemoryAllocator allocator{};
			std::unordered_map<uint64_t, void*> linkedAssets[T10RAssetType::T10R_ASSET_COUNT]{};
			AssetList assets{};
		} gcx{};

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
					for (size_t i = 0; i < gcx.assets.assetsCount; i++) {
						Asset& asset{ gcx.assets.assets[i] };

						// todo:
						// asset.type = sub_88B85F0(asset.type)

						assetsOs << "\n" << std::hex << asset.type << ",<unk>";


					}
				}
				LOG_INFO("Asset names dump into {}", outAssets.string());
			}

		};

		
		utils::ArrayAdder<BO6FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
	}
}