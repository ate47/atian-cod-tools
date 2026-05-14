#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	struct BGCacheInfoDef {
		games::bo4::pool::BGCacheTypes type;
		XHash name;
		uint64_t asset;
	}; static_assert(sizeof(BGCacheInfoDef) == 0x20);

	struct BGCacheInfo {
		XHash name;
		BGCacheInfoDef* def;
		int defCount;
	}; static_assert(sizeof(BGCacheInfo) == 0x20);

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void ComputeFinal(BO4LinkContext& ctx, BO4FFContext& ff) override {
			size_t entries{};
			for (auto& [_, v] : ff.bgcache) {
				for (auto& vv : v) {
					entries++;
				}
			}

			if (!entries) {
				return; // nothing to build
			}

			ff.data.AddAsset(games::bo4::pool::ASSET_TYPE_BG_CACHE, fastfile::linker::data::POINTER_NEXT);

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			BGCacheInfo bg{};

			bg.name.name = ff.ffnameHash;
			bg.def = (BGCacheInfoDef*)fastfile::linker::data::POINTER_NEXT;
			bg.defCount = (uint32_t)entries;
			ff.data.WriteData(bg);

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
			ff.data.Align(8);
			BGCacheInfoDef* defs{ ff.data.AllocDataPtr<BGCacheInfoDef>(sizeof(BGCacheInfoDef) * entries) };

			for (auto& [type, v] : ff.bgcache) {
				for (auto& name : v) {
					defs->name.name = name;
					defs->type = type;
					defs++;
				}
			}
			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset bgcache {} (hash_{:x})", ff.ffname, ff.ffnameHash);
		}

		void Compute(BO4LinkContext& ctx, const char* id, uint64_t* hashOut, BO4FFContext& ff) override {
			std::filesystem::path path{ ctx.linkCtx.input / id };
			utils::InFileCE is{ path };
			if (!is) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}

			rapidcsv::Document doc{};

			doc.Load(is, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

			LOG_INFO("Loaded bgcache data {}", id);

			if (!doc.GetRowCount()) return; // empty

			if (doc.GetColumnCount() < 2) {
				LOG_ERROR("Can't read {}: Invalid BGCache structure", path.string());
				ctx.error = true;
				return;
			}

			for (size_t i = 0; i < doc.GetRowCount(); i++) {
				const std::string typeStr{ doc.GetCell<std::string>(0, i) };
				const std::string nameStr{ doc.GetCell<std::string>(1, i) };

				games::bo4::pool::BGCacheTypes type{ games::bo4::pool::BGCacheIdFromName(typeStr.data()) };

				if (type == games::bo4::pool::BG_CACHE_TYPE_INVALID) {

					LOG_ERROR("Can't read {}: Unknown bgcache type {}", path.string(), typeStr);
					ctx.error = true;
					continue;
				}

				ff.bgcache[type].insert(ctx.HashXHash(nameStr.c_str()));
			}
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_BG_CACHE, true };
}