#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;

	struct LocalizeEntry {
		const char* value;
		XHash name;
	};
	static_assert(sizeof(LocalizeEntry) == 0x18);

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, uint64_t* hashOut, BO4FFContext& ff) override {
			std::filesystem::path localizePath{ ctx.linkCtx.input / id };
			core::config::Config localize{ localizePath };
			localize.SyncConfig(false);
			if (localize.main.Empty()) return; // nothing to force

			std::string lang{ localize.GetString("language", "all")};

			BO4FFContext* pffctx;
			if (lang == "all") {
				pffctx = &ff;
			}
			else {
				pffctx = &ctx.GetFFContext(utils::va("%s_", lang.c_str()));
			}

			BO4FFContext& ffctx{ *pffctx };

			size_t added{};
			for (auto& [k, v] : localize.main.GetObj()) {
				if (!k.IsString() || !v.IsString()) {
					LOG_WARNING("Invalid localize: {} -> {}", k.GetString(), v.GetString());
					continue;
				}
				const char* key{ k.GetString() };
				if (!std::strcmp(key, "language")) continue; // ignore language key

				// todo: better localize format to support multilang
				ffctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_LOCALIZE_ENTRY);

				ffctx.data.PushStream(XFILE_BLOCK_TEMP);
				LocalizeEntry& header{ ffctx.data.AllocStreamRef<LocalizeEntry>() };

				header.name.name = ctx.HashXHash(key);
				header.value = (const char*)fastfile::linker::memory::POINTER_NEXT;

				ffctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ffctx.data.Align<char>();
				ffctx.data.WriteStream(v.GetString());
				ffctx.data.PopStream();

				ffctx.data.PopStream();
				added++;
			}
			LOG_INFO("Added asset localizeentry {}: {} entry(ies) into {}", id, added, ffctx.ffname);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_LOCALIZE_LIST, true };
}