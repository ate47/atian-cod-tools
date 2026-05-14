#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	struct RankInfoReward {
		XHash prestigeTarget;
		XHash rewardName;
	};
	struct RankInfo {
		XHash name;
		uint32_t level;
		uint32_t minXp;
		uint32_t maxXp;
		XHash shortNameRef;
		XHash fullNameRef;
		XHash ingameNameRef;
		GfxImage* icon;
		GfxImage* iconLarge;
		uint64_t rewardsCount;
		RankInfoReward* rewards;
	};

	enum PrestigeTypeOfOrigin : uint32_t {
		ORIGIN_TITLE_T4 = 0,
		ORIGIN_TITLE_T5 = 1,
		ORIGIN_TITLE_T6 = 2,
		ORIGIN_TITLE_T7 = 3,
		ORIGIN_TITLE_T8 = 4,
	};

	const char* PrestigeTypeOfOriginNames[]{
		"t4", "t5", "t6", "t7", "t8"
	};

	struct PrestigeInfo {
		XHash name;
		XHash displayName;
		uint32_t unlockLevel;
		uint32_t winsRequired;
		PrestigeTypeOfOrigin titleOfOrigin;
		uint32_t unk2c;
		GfxImage* iconName;
		GfxImage* iconNameLarge;
	};

	struct RankTable {
		XHash name;
		uint64_t ranksCount;
		RankInfo** ranks;
	};

	struct PrestigeTable {
		XHash name;
		int64_t prestigesCount;
		PrestigeInfo** prestiges;
	};

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, uint64_t* hashOut, BO4FFContext& ff) override {
			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();

			core::config::Config objCfg{ path };

			if (!objCfg.SyncConfig(false)) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}

			ff.data.PushStream(XFILE_BLOCK_TEMP);
			RankInfo rankinfo{};

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			rankinfo.name.name = ctx.HashXHash(assetName, true);
			rankinfo.name.name = ctx.HashXHash(objCfg.GetCString("name"), true);
			if (hashOut) *hashOut = rankinfo.name;

			rankinfo.shortNameRef.name = ctx.HashXHash(objCfg.GetCString("shortNameRef"), true);
			rankinfo.fullNameRef.name = ctx.HashXHash(objCfg.GetCString("fullNameRef"), true);
			rankinfo.ingameNameRef.name = ctx.HashXHash(objCfg.GetCString("ingameNameRef"), true);

			rankinfo.level = (int32_t)objCfg.GetInteger("level");
			rankinfo.minXp = (uint32_t)objCfg.GetInteger("minXp");
			rankinfo.maxXp = (uint32_t)objCfg.GetInteger("maxXp");

			const char* icon{ objCfg.GetCString("icon") };
			const char* iconLarge{ objCfg.GetCString("iconLarge") };

			if (icon) rankinfo.icon = (GfxImage*)fastfile::linker::data::POINTER_NEXT;
			if (iconLarge) rankinfo.iconLarge = (GfxImage*)fastfile::linker::data::POINTER_NEXT;

			size_t objData{ ff.data.WriteData(rankinfo) };
			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			if (icon) LinkAsset(XAssetType::ASSET_TYPE_IMAGE, ctx, icon, nullptr, false, &ff);
			if (iconLarge) LinkAsset(XAssetType::ASSET_TYPE_IMAGE, ctx, iconLarge, nullptr, false, &ff);

			rapidjson::Value& orewards{ objCfg.GetVal("rewards", 0, objCfg.main) };
			if (!orewards.IsNull()) {
				if (!orewards.IsArray()) {
					LOG_ERROR("Invalid zbarrier boards: not an array");
					ctx.error = true;
					return;
				}

				auto rewards{ orewards.GetArray() };
				RankInfo& prankInfo{ *ff.data.GetData<RankInfo>(objData) };
				prankInfo.rewards = (RankInfoReward*)fastfile::linker::data::POINTER_NEXT;
				prankInfo.rewardsCount = (uint64_t)rewards.Size();

				ff.data.Align(8);
				RankInfoReward* prewards{ ff.data.AllocDataPtr<RankInfoReward>(sizeof(RankInfoReward) * rewards.Size()) };

				for (auto& reward : rewards) {
					core::config::ConfigGeneric creward{ objCfg.GetSub(reward) };

					prewards->prestigeTarget.name = ctx.HashXHash(creward.GetCString("prestigeTarget"), true);
					prewards->rewardName.name = ctx.HashXHash(creward.GetCString("rewardName"), true);
				}
			}

			ff.data.PopStream();

			ff.data.PopStream();
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_RANK };
}