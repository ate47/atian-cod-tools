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

	static core::config::ConfigEnumData PrestigeTypeOfOriginNamesEnum[] {
		{ PrestigeTypeOfOriginNames[ORIGIN_TITLE_T4], ORIGIN_TITLE_T4},
		{ PrestigeTypeOfOriginNames[ORIGIN_TITLE_T5], ORIGIN_TITLE_T5},
		{ PrestigeTypeOfOriginNames[ORIGIN_TITLE_T6], ORIGIN_TITLE_T6},
		{ PrestigeTypeOfOriginNames[ORIGIN_TITLE_T7], ORIGIN_TITLE_T7},
		{ PrestigeTypeOfOriginNames[ORIGIN_TITLE_T8], ORIGIN_TITLE_T8},
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

	class RankInfoImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
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
			RankInfo& rankinfo{ ff.data.AllocStreamRef<RankInfo>(ref) };

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			rankinfo.name.name = ctx.HashXHash(assetName, true);
			rankinfo.name.name = ctx.HashXHash(objCfg.GetCString("name"), true);

			rankinfo.shortNameRef.name = ctx.HashXHash(objCfg.GetCString("shortNameRef"), true);
			rankinfo.fullNameRef.name = ctx.HashXHash(objCfg.GetCString("fullNameRef"), true);
			rankinfo.ingameNameRef.name = ctx.HashXHash(objCfg.GetCString("ingameNameRef"), true);

			rankinfo.level = (int32_t)objCfg.GetInteger("level");
			rankinfo.minXp = (uint32_t)objCfg.GetInteger("minXp");
			rankinfo.maxXp = (uint32_t)objCfg.GetInteger("maxXp");

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			ctx.LinkAsset(XAssetType::ASSET_TYPE_IMAGE, objCfg.GetCString("icon"), rankinfo.icon, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_IMAGE, objCfg.GetCString("iconLarge"), rankinfo.iconLarge, false, &ff);

			rapidjson::Value& orewards{ objCfg.GetVal("rewards", 0, objCfg.main) };
			if (!orewards.IsNull()) {
				if (!orewards.IsArray()) {
					LOG_ERROR("Invalid zbarrier boards: not an array");
					ctx.error = true;
					return;
				}

				auto rewards{ orewards.GetArray() };
				rankinfo.rewards = (RankInfoReward*)fastfile::linker::memory::POINTER_NEXT;
				rankinfo.rewardsCount = (uint64_t)rewards.Size();

				ff.data.Align(8);
				RankInfoReward* prewards{ ff.data.AllocStreamPtr<RankInfoReward>(rewards.Size()) };

				for (auto& reward : rewards) {
					core::config::ConfigGeneric creward{ objCfg.GetSub(reward) };

					prewards->prestigeTarget.name = ctx.HashXHash(creward.GetCString("prestigeTarget"), true);
					prewards->rewardName.name = ctx.HashXHash(creward.GetCString("rewardName"), true);
				}
			}

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset rank {} (hash_{:x})", assetName, rankinfo.name.name);
		}
	};

	class RankTableImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();

			utils::InFileCE input{ path };

			if (!input) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}
			std::vector<std::string> ranks{};

			std::string line{};
			while (*input && std::getline(*input, line, '\n')) {
				if (line.empty() || line[0] == '#') {
					continue;
				}
				ranks.emplace_back(line);
			}


			ff.data.PushStream(XFILE_BLOCK_TEMP);
			RankTable& table{ ff.data.AllocStreamRef<RankTable>(ref) };

			std::string rfpathStr{ rfpath.string() };
			table.name.name = ctx.HashXHash(rfpathStr, true);
			table.ranksCount = (uint32_t)ranks.size();

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			if (!ranks.empty()) {
				table.ranks = (RankInfo**)fastfile::linker::memory::POINTER_NEXT;
				ff.data.Align(8);
				RankInfo** list{ ff.data.AllocStreamPtr<RankInfo*>(ranks.size()) };

				for (size_t i = 0; i < ranks.size(); i++) {
					ctx.LinkAsset(XAssetType::ASSET_TYPE_RANK, ranks[i].data(), list[i], false, &ff);
				}
			}

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset ranktable {} (hash_{:x})", rfpathStr, table.name.name);
		}
	};

	class PrestigeInfoImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
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
			PrestigeInfo& prestigeInfo{ ff.data.AllocStreamRef<PrestigeInfo>(ref) };

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			prestigeInfo.name.name = ctx.HashXHash(assetName, true);
			prestigeInfo.name.name = ctx.HashXHash(objCfg.GetCString("name"), true);
			prestigeInfo.displayName.name = ctx.HashXHash(objCfg.GetCString("displayName"), true);
			prestigeInfo.unlockLevel = (int32_t)objCfg.GetInteger("unlockLevel");
			prestigeInfo.winsRequired = (uint32_t)objCfg.GetInteger("winsRequired");
			prestigeInfo.unk2c = (uint32_t)objCfg.GetInteger("unk2c");
			prestigeInfo.titleOfOrigin = objCfg.GetEnumVal<PrestigeTypeOfOrigin>("titleOfOrigin", PrestigeTypeOfOriginNamesEnum, ACTS_ARRAYSIZE(PrestigeTypeOfOriginNamesEnum), ORIGIN_TITLE_T8);

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			ctx.LinkAsset(XAssetType::ASSET_TYPE_IMAGE, objCfg.GetCString("iconName"), prestigeInfo.iconName, false, &ff);
			ctx.LinkAsset(XAssetType::ASSET_TYPE_IMAGE, objCfg.GetCString("iconNameLarge"), prestigeInfo.iconNameLarge, false, &ff);

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset prestige {} (hash_{:x})", assetName, prestigeInfo.name.name);
		}
	};


	class PrestigeTableImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, fastfile::linker::memory::LinkerDataChunk** ref, BO4FFContext& ff) override {
			std::filesystem::path path{ ctx.linkCtx.input / id };
			std::filesystem::path rfpath{ path.filename() };
			rfpath.replace_extension();

			utils::InFileCE input{ path };

			if (!input) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}
			std::vector<std::string> ranks{};

			std::string line{};
			while (*input && std::getline(*input, line, '\n')) {
				if (line.empty() || line[0] == '#') {
					continue;
				}
				ranks.emplace_back(line);
			}


			ff.data.PushStream(XFILE_BLOCK_TEMP);
			PrestigeTable& table{ ff.data.AllocStreamRef<PrestigeTable>(ref) };

			std::string rfpathStr{ rfpath.string() };
			table.name.name = ctx.HashXHash(rfpathStr, true);
			table.prestigesCount = (uint32_t)ranks.size();

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			if (!ranks.empty()) {
				table.prestiges = (PrestigeInfo**)fastfile::linker::memory::POINTER_NEXT;
				ff.data.Align(8);
				PrestigeInfo** list{ ff.data.AllocStreamPtr<PrestigeInfo*>(ranks.size()) };

				for (size_t i = 0; i < ranks.size(); i++) {
					ctx.LinkAsset(XAssetType::ASSET_TYPE_PRESTIGE, ranks[i].data(), list[i], false, &ff);
				}
			}

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset ranktable {} (hash_{:x})", rfpathStr, table.name.name);
		}
	};

	utils::MapAdder<RankInfoImpl, XAssetType, XAssetLinker> implri{ GetWorkers(), XAssetType::ASSET_TYPE_RANK };
	utils::MapAdder<RankTableImpl, XAssetType, XAssetLinker> implrt{ GetWorkers(), XAssetType::ASSET_TYPE_RANKTABLE };
	utils::MapAdder<PrestigeInfoImpl, XAssetType, XAssetLinker> implpi{ GetWorkers(), XAssetType::ASSET_TYPE_PRESTIGE };
	utils::MapAdder<PrestigeTableImpl, XAssetType, XAssetLinker> implpt{ GetWorkers(), XAssetType::ASSET_TYPE_PRESTIGETABLE };
}