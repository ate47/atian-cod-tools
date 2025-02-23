#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class LocalizeWorker : public LinkerWorker {
	public:
		LocalizeWorker() : LinkerWorker("Localize") {}

		void Compute(BO4LinkContext& ctx) override {

			std::filesystem::path localizePath{ ctx.linkCtx.input / "localize.json" };
			core::config::Config localize{ localizePath };
			localize.SyncConfig(false);
			if (localize.main.Empty()) return; // nothing to force

			struct LocalizeEntry {
				uintptr_t value; // const char*
				XHash name;
			};

			size_t added{};
			for (auto& [k, v] : localize.main.GetObj()) {
				if (!k.IsString() || !v.IsString()) {
					LOG_WARNING("Invalid localize: {} -> {}", k.GetString(), v.GetString());
					continue;
				}
				LocalizeEntry& header{ utils::Allocate<LocalizeEntry>(ctx.assetData) };
				header.name.name = hash::Hash64Pattern(k.GetString());
				header.value = fastfile::ALLOC_PTR;
				utils::WriteString(ctx.assetData, v.GetString());
				ctx.assets.emplace_back(games::bo4::pool::ASSET_TYPE_LOCALIZE_ENTRY, fastfile::ALLOC_PTR);
				added++;
			}
			LOG_INFO("Added asset localizeentry {}: {} entry(ies)", localizePath.string(), added);
		}
	};

	utils::ArrayAdder<LocalizeWorker, LinkerWorker> impl{ GetWorkers() };
}