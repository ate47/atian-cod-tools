#include <includes.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	struct XModel;
	struct FxEffectDef;

	struct ZBarrierBoard {
		XModel* pBoardModel;
		XModel* pAlternateBoardModel;
		XModel* pUpgradedBoardModel;
		XHash tearAnim;
		XHash boardAnim;
		XHash idleAnim;
		XHash staticAnim;
		FxEffectDef* repairEffect1;
		FxEffectDef* repairEffect2;
		vec3_t repairEffect1Offset;
		vec3_t repairEffect2Offset;
		XHash boardRepairSound;
		XHash boardRepairHoverSound;
		XHash pauseAndRepeatRepSound;
		float minPause;
		float maxPause;
		uint32_t numRepsToPullProBoard;
	};
	static_assert(sizeof(ZBarrierBoard) == 0xc0);

	struct ZBarrierDef {
		XHash name;
		XHash generalRepairSound1;
		XHash generalRepairSound2;
		XHash upgradedGeneralRepairSound1;
		XHash upgradedGeneralRepairSound2;
		XHash delayBetweenRepSounds;
		float delayBetweenRepSoundsDuration;
		uint32_t earthquakeOnRepair;
		float earthquakeMinScale;
		float earthquakeMaxScale;
		float earthquakeMinDuration;
		float earthquakeMaxDuration;
		float earthquakeRadius;
		int32_t numBoardsInBarrier;
		uint32_t autoHideOpenPieces;
		uint32_t taunts;
		uint32_t reachThroughAttacks;
		XHash zombieTauntAnimState;
		XHash zombieReachThroughAnimState;
		int32_t numAttackSlots;
		float attackSpotHorzOffset;
		XModel* pCollisionModel;
		ZBarrierBoard boards[6];
	};
	static_assert(sizeof(ZBarrierDef) == 0x540);

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
			ZBarrierDef zbarrier{};

			std::string rfpathStr{ rfpath.string() };
			std::string assetName{ objCfg.GetString("name", rfpathStr.c_str()) };
			zbarrier.name.name = ctx.HashXHash(assetName, true);
			zbarrier.name.name = ctx.HashXHash(objCfg.GetCString("name"), true);
			if (hashOut) *hashOut = zbarrier.name;
			zbarrier.generalRepairSound1.name = ctx.HashXHash(objCfg.GetCString("generalRepairSound1"), true);
			zbarrier.generalRepairSound2.name = ctx.HashXHash(objCfg.GetCString("generalRepairSound2"), true);
			zbarrier.upgradedGeneralRepairSound1.name = ctx.HashXHash(objCfg.GetCString("upgradedGeneralRepairSound1"), true);
			zbarrier.upgradedGeneralRepairSound2.name = ctx.HashXHash(objCfg.GetCString("upgradedGeneralRepairSound2"), true);
			zbarrier.delayBetweenRepSounds.name = ctx.HashXHash(objCfg.GetCString("delayBetweenRepSounds"), true);
			zbarrier.zombieTauntAnimState.name = ctx.HashXHash(objCfg.GetCString("zombieTauntAnimState"), true);
			zbarrier.zombieReachThroughAnimState.name = ctx.HashXHash(objCfg.GetCString("zombieReachThroughAnimState"), true);

			zbarrier.delayBetweenRepSoundsDuration = (float)objCfg.GetDouble("delayBetweenRepSoundsDuration");
			zbarrier.earthquakeMinScale = (float)objCfg.GetDouble("earthquakeMinScale");
			zbarrier.earthquakeMaxScale = (float)objCfg.GetDouble("earthquakeMaxScale");
			zbarrier.earthquakeMinDuration = (float)objCfg.GetDouble("earthquakeMinDuration");
			zbarrier.earthquakeMaxDuration = (float)objCfg.GetDouble("earthquakeMaxDuration");
			zbarrier.earthquakeRadius = (float)objCfg.GetDouble("earthquakeRadius");
			zbarrier.attackSpotHorzOffset = (float)objCfg.GetDouble("attackSpotHorzOffset");

			zbarrier.numAttackSlots = (int32_t)objCfg.GetInteger("numAttackSlots");
			zbarrier.earthquakeOnRepair = (uint32_t)objCfg.GetInteger("earthquakeOnRepair");
			zbarrier.autoHideOpenPieces = (uint32_t)objCfg.GetInteger("autoHideOpenPieces");
			zbarrier.taunts = (uint32_t)objCfg.GetInteger("taunts");
			zbarrier.reachThroughAttacks = (uint32_t)objCfg.GetInteger("reachThroughAttacks");

			const char* pCollisionModel{ objCfg.GetCString("pCollisionModel") };

			if (pCollisionModel) {
				zbarrier.pCollisionModel = (XModel*)fastfile::linker::data::POINTER_NEXT;
			}

			size_t objData{ ff.data.WriteData(zbarrier) };
			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);


			if (pCollisionModel) LinkAsset(XAssetType::ASSET_TYPE_XMODEL, ctx, pCollisionModel, nullptr, false, &ff);

			rapidjson::Value& oboards{ objCfg.GetVal("boards", 0, objCfg.main) };
			if (!oboards.IsNull()) {
				if (!oboards.IsArray()) {
					LOG_ERROR("Invalid zbarrier boards: not an array");
					ctx.error = true;
					return;
				}

				auto boards{ oboards.GetArray() };

				if (boards.Size() > ACTS_ARRAYSIZE(zbarrier.boards)) {
					LOG_ERROR("Invalid zbarrier boards: too many boards, maximum is {}", ACTS_ARRAYSIZE(zbarrier.boards));
					ctx.error = true;
					return;
				}


				// link boards

				for (auto& board : boards) {
					ZBarrierDef* def{ ff.data.GetData<ZBarrierDef>(objData) };
					ZBarrierBoard& zboard{ def->boards[def->numBoardsInBarrier++] };
					core::config::ConfigGeneric cboard{ objCfg.GetSub(board) };

					zboard.tearAnim.name = ctx.HashXHash(cboard.GetCString("tearAnim"), true);
					zboard.boardAnim.name = ctx.HashXHash(cboard.GetCString("boardAnim"), true);
					zboard.idleAnim.name = ctx.HashXHash(cboard.GetCString("idleAnim"), true);
					zboard.staticAnim.name = ctx.HashXHash(cboard.GetCString("staticAnim"), true);
					zboard.boardRepairSound.name = ctx.HashXHash(cboard.GetCString("boardRepairSound"), true);
					zboard.boardRepairHoverSound.name = ctx.HashXHash(cboard.GetCString("boardRepairHoverSound"), true);
					zboard.pauseAndRepeatRepSound.name = ctx.HashXHash(cboard.GetCString("pauseAndRepeatRepSound"), true);

					zboard.minPause = (float)cboard.GetDouble("minPause");
					zboard.maxPause = (float)cboard.GetDouble("maxPause");
					zboard.numRepsToPullProBoard = (uint32_t)cboard.GetInteger("numRepsToPullProBoard");

					if (!cboard.ScanString("repairEffect1Offset", "%f, %f, %f", &zboard.repairEffect1Offset[0], &zboard.repairEffect1Offset[1], &zboard.repairEffect1Offset[2])) {
						LOG_ERROR("Bad format for repairEffect1Offset \"123, 123, 123\"");
						ctx.error = true;
						continue;
					}
					if (!cboard.ScanString("repairEffect2Offset", "%f, %f, %f", &zboard.repairEffect2Offset[0], &zboard.repairEffect2Offset[1], &zboard.repairEffect2Offset[2])) {
						LOG_ERROR("Bad format for repairEffect2Offset \"123, 123, 123\"");
						ctx.error = true;
						continue;
					}

					const char* pBoardModel{ cboard.GetCString("pBoardModel") };
					const char* pAlternateBoardModel{ cboard.GetCString("pAlternateBoardModel") };
					const char* pUpgradedBoardModel{ cboard.GetCString("pUpgradedBoardModel") };
					const char* repairEffect1{ cboard.GetCString("repairEffect1") };
					const char* repairEffect2{ cboard.GetCString("repairEffect2") };

					if (pBoardModel) zboard.pBoardModel = (XModel*)fastfile::linker::data::POINTER_NEXT;
					if (pAlternateBoardModel) zboard.pAlternateBoardModel = (XModel*)fastfile::linker::data::POINTER_NEXT;
					if (pUpgradedBoardModel) zboard.pUpgradedBoardModel = (XModel*)fastfile::linker::data::POINTER_NEXT;
					if (repairEffect1) zboard.repairEffect1 = (FxEffectDef*)fastfile::linker::data::POINTER_NEXT;
					if (repairEffect2) zboard.repairEffect2 = (FxEffectDef*)fastfile::linker::data::POINTER_NEXT;

					if (pBoardModel) LinkAsset(XAssetType::ASSET_TYPE_XMODEL, ctx, pBoardModel, nullptr, false, &ff);
					if (pAlternateBoardModel) LinkAsset(XAssetType::ASSET_TYPE_XMODEL, ctx, pAlternateBoardModel, nullptr, false, &ff);
					if (pUpgradedBoardModel) LinkAsset(XAssetType::ASSET_TYPE_XMODEL, ctx, pUpgradedBoardModel, nullptr, false, &ff);
					if (repairEffect1) LinkAsset(XAssetType::ASSET_TYPE_FX, ctx, repairEffect1, nullptr, false, &ff);
					if (repairEffect2) LinkAsset(XAssetType::ASSET_TYPE_FX, ctx, repairEffect2, nullptr, false, &ff);
				}

			}
			ff.data.PopStream();

			ff.data.PopStream();
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_ZBARRIER };
}