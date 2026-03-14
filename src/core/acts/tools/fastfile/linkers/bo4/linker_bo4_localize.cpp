#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class LocalizeWorker : public LinkerWorker {
	public:
		LocalizeWorker() : LinkerWorker("Localize") {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["localizelist"]) {
				std::filesystem::path localizePath{ ctx.linkCtx.input / assval.value };
				assval.handled = true;
				core::config::Config localize{ localizePath };
				localize.SyncConfig(false);
				if (localize.main.Empty()) continue; // nothing to force

				struct LocalizeEntry {
					const char* value;
					XHash name;
				};
				static_assert(sizeof(LocalizeEntry) == 0x18);

				std::string lang{ localize.GetString("language", "all")};

				BO4FFContext* pffctx;
				if (lang == "all") {
					pffctx = &ctx.mainFF;
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
					ffctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_LOCALIZE_ENTRY, fastfile::linker::data::POINTER_NEXT);

					ffctx.data.PushStream(XFILE_BLOCK_TEMP);
					LocalizeEntry header{};

					header.name.name = ctx.HashXHash(key);
					header.value = (const char*)fastfile::linker::data::POINTER_NEXT;
					ffctx.data.WriteData(header);

					ffctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
					ffctx.data.Align<char>();
					ffctx.data.WriteData(v.GetString());
					ffctx.data.PopStream();

					ffctx.data.PopStream();
					added++;
				}
				LOG_INFO("Added asset localizeentry {}: {} entry(ies) into {}", assval.value, added, ffctx.ffname);
			}
		}
	};

	utils::ArrayAdder<LocalizeWorker, LinkerWorker> impl{ GetWorkers() };
}