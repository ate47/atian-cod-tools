#include <includes.hpp>
#include <tools/fastfile/handlers/handler_assets.hpp>
#include <decryptutils.hpp>

namespace fastfile::handlers::assets {
	struct Asset {
		uint32_t type;
		void* handle;
	};

	// default
	struct AssetListOld {
		uint32_t stringsCount;
		bool stringsLoaded;
		const char** strings;
		uint32_t assetsCount;
		uint32_t assetsLoaded;
		Asset* assets;
	};
	static_assert(sizeof(AssetListOld) == 0x20);

	// cw
	struct AssetListCW {
		int stringsCount;
		const char** strings;
		int* unk10;
		int assetCount;
		Asset* assets;
	};
	static_assert(sizeof(AssetListCW) == 0x28);

	// bo6 latest bo7
	struct AssetListCer {
		uint32_t stringsCount;
		bool stringsLoaded;
		const char** strings;
		uint32_t assetsCount;
		uint32_t assetsLoaded;
		uint32_t* unk10;
		uint32_t unk10_count;
		Asset* assets;
	};
	static_assert(sizeof(AssetListCer) == 0x30);

	namespace {

		struct {
			AssetListCer list{};
		} gcx;

		enum StreamPointerFlag : uint64_t {
			SPF_CREATE_REF = 1ull << 61, // create a offset with the data as name
			SPF_NEXT = 2ull << 61, // load the data after, otherwise data is name of offset
			SPF_UNK = 4ull << 61, // unk

			SPF_DATA_MASK = ~(SPF_CREATE_REF | SPF_NEXT | SPF_UNK)
		};

		class AssetListFFHandler : public FFHandler {
		public:
			AssetListFFHandler() : fastfile::FFHandler("assets", "Extract assetlist data", true) {
			}

			void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx) override {
				opt.GetGame(false, nullptr, true);
				if (!opt.m_gameId) {
					throw std::runtime_error("Missing gameId, set it with --gameId [g]");
				}


				switch (opt.m_gameId) {
				case GID_BO3:
				case GID_BO4:
				case GID_IW:
				case GID_WWII:
				case GID_MW19:
				case GID_VG:
				case GID_MWII:
				case GID_MWIII: {
					AssetListOld* header{ reader.ReadPtr<AssetListOld>() };
					gcx.list = {};
					gcx.list.assets = header->assets;
					gcx.list.assetsCount = header->assetsCount;
					gcx.list.strings = header->strings;
					gcx.list.stringsCount = header->stringsCount;
					break;
				}

				case GID_CW: {
					AssetListCW* header{ reader.ReadPtr<AssetListCW>() };
					gcx.list = {};
					gcx.list.assets = header->assets;
					gcx.list.assetsCount = header->assetCount;
					gcx.list.strings = header->strings;
					gcx.list.stringsCount = header->stringsCount;
					// unk10 is virtual
					break;
				}

				case GID_BO6:
				case GID_BO7: {
					gcx.list = reader.Read<AssetListCer>();
					break;
				}

				default:
					throw std::runtime_error(std::format("Invalid gameid {}", GetGameIdName(opt.m_gameId)));
				}

				const char* fftype{ ctx.GetFFType() };
				std::filesystem::path outStrings{ opt.m_output / "assetlist" / "strings" / fftype / std::format("{}.txt", ctx.ffname) };

				if (gcx.list.strings) {
					std::filesystem::create_directories(outStrings.parent_path());
					utils::OutFileCE stringsOs{ outStrings };
					LOG_OPT_INFO("dump {}", outStrings.string());
					gcx.list.strings = reader.ReadPtr<const char*>(gcx.list.stringsCount);

					for (size_t i = 0; i < gcx.list.stringsCount; i++) {
						const char* str;
						uint64_t stroff{ (uint64_t)gcx.list.strings[i] };
						if ((stroff & StreamPointerFlag::SPF_NEXT) != 0) {
							str = acts::decryptutils::DecryptString(reader.ReadString());
						}
						else {
							str = "";
						}
						gcx.list.strings[i] = str;
						stringsOs << str << "\n";
					}
				}
				if (gcx.list.unk10) {
					gcx.list.unk10 = reader.ReadPtr<uint32_t>(gcx.list.unk10_count);
				}

				std::filesystem::path outPreAssets{ opt.m_output / "assetlist" / "pre_assets" / fftype / std::format("{}.csv", ctx.ffname) };
				std::filesystem::path outAssets{ opt.m_output / "assetlist" / "assets" / fftype / std::format("{}.csv", ctx.ffname) };
				if (gcx.list.assets) {
					gcx.list.assets = reader.ReadPtr<Asset>(gcx.list.assetsCount);

					{
						// dump ids
						std::filesystem::create_directories(outPreAssets.parent_path());
						utils::OutFileCE assetsOs{ outPreAssets };
						LOG_OPT_INFO("dump {}", outPreAssets.string());
						assetsOs << "type,ref";

						for (size_t i = 0; i < gcx.list.assetsCount; i++) {
							Asset* asset{ &gcx.list.assets[i] };
							assetsOs << "\n";

							if (asset->type >= 0x400) {
								// probably a hash
								assetsOs << hashutils::ExtractTmp("hash", asset->type);
							}
							else {
								assetsOs << std::hex << "0x" << asset->type;
							}

							assetsOs << "," << asset->handle;
						}
					}
					if (opt.m_gameRevId) {
						std::unordered_map<uint64_t, Worker*>& workers{ GetWorkers() };
						
						for (auto& [k, v] : workers) {
							v->PreXFileLoading(opt, ctx);
						}

						for (size_t i = 0; i < gcx.list.assetsCount; i++) {
							Asset* asset{ &gcx.list.assets[i] };

							UnlinkType(AssetGameRef(opt.m_gameId, opt.m_gameRevId, asset->type), opt, reader, &asset->handle);
						}

						for (auto& [k, v] : workers) {
							v->PostXFileLoading(opt, ctx);
						}
					}
				}
			}
		};
	}

	bool UnlinkTypeRef(uint64_t ref, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle) {
		if (!*handle) {
			return true;
		}
		uint64_t v{ (uint64_t)*handle };
		if ((v & 0x7FFFFFFFFFFFFFFFi64) == 0x3FFFFFFFFFFFFFFFULL) {
			reader.ReadString();
			return true;
		}
		if ((v & 0x7FFFFFFFFFFFFFFFi64) != 0x7FFFFFFFFFFFFFFFULL) {
			// todo: link
			return true;
		}
		return false;
	}

	void UnlinkType(uint64_t ref, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle) {
		if (UnlinkTypeRef(ref, opt, reader, handle)) {
			return;
		}

		std::unordered_map<uint64_t, Worker*>& workers{ GetWorkers() };

		auto it{ workers.find(ref) };

		if (it == workers.end()) {
			throw std::runtime_error(std::format("Missing handler for type {}", ref & 0xffffffff));
		}

		it->second->Unlink(opt, reader, handle);
	}

	std::unordered_map<uint64_t, Worker*>& GetWorkers() {
		static std::unordered_map<uint64_t, Worker*> map{};
		return map;
	}
	const char* GetScrString(ScrString_t id) {
		if ((int)id >= gcx.list.stringsCount) {
			LOG_WARNING("Can't get scr string: {} >= {}", id.id, gcx.list.stringsCount);
			return utils::va("<invalid:0x%x>", id);
		}
		const char* c{ gcx.list.strings[id] };
		if (!c) return "";
		return c;
	}

	utils::ArrayAdder<AssetListFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}