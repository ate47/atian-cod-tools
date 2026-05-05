#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <hook/error.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	struct ZBarrierBoard {
		XModel* pBoardModel;
		XModel* pAlternateBoardModel;
		XModel* pUpgradedBoardModel;
		XHash tearAnim;
		XHash boardAnim;
		XHash idleAnim;
		XHash staticAnim;
		FxEffectDefHandle repairEffect1;
		FxEffectDefHandle repairEffect2;
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
		int numAttackSlots;
		float attackSpotHorzOffset;
		XModel* pCollisionModel;
		ZBarrierBoard boards[6];
	};
	static_assert(sizeof(ZBarrierDef) == 0x540);


	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			ZBarrierDef* asset{ (ZBarrierDef*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "zbarrier" / fastfile::GetCurrentContext().ffname / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump zbarrier {}", outFile.string());

			json.BeginObject();


			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXHash("generalRepairSound1", asset->generalRepairSound1);
			json.WriteFieldValueXHash("generalRepairSound2", asset->generalRepairSound2);
			json.WriteFieldValueXHash("upgradedGeneralRepairSound1", asset->upgradedGeneralRepairSound1);
			json.WriteFieldValueXHash("upgradedGeneralRepairSound2", asset->upgradedGeneralRepairSound2);
			json.WriteFieldValueXHash("delayBetweenRepSounds", asset->delayBetweenRepSounds);
			json.WriteFieldValueXHash("zombieTauntAnimState", asset->zombieTauntAnimState);
			json.WriteFieldValueXHash("zombieReachThroughAnimState", asset->zombieReachThroughAnimState);
			json.WriteFieldValueXAsset("collisionModel", games::bo4::pool::ASSET_TYPE_XMODEL, asset->pCollisionModel);
			json.WriteFieldValueNumber("delayBetweenRepSoundsDuration", asset->delayBetweenRepSoundsDuration);
			json.WriteFieldValueNumber("earthquakeOnRepair", asset->earthquakeOnRepair);
			json.WriteFieldValueNumber("earthquakeMinScale", asset->earthquakeMinScale);
			json.WriteFieldValueNumber("earthquakeMaxScale", asset->earthquakeMaxScale);
			json.WriteFieldValueNumber("earthquakeMinDuration", asset->earthquakeMinDuration);
			json.WriteFieldValueNumber("earthquakeMaxDuration", asset->earthquakeMaxDuration);
			json.WriteFieldValueNumber("earthquakeRadius", asset->earthquakeRadius);
			json.WriteFieldValueNumber("numBoardsInBarrier", asset->numBoardsInBarrier);
			json.WriteFieldValueNumber("autoHideOpenPieces", asset->autoHideOpenPieces);
			json.WriteFieldValueNumber("taunts", asset->taunts);
			json.WriteFieldValueNumber("reachThroughAttacks", asset->reachThroughAttacks);
			json.WriteFieldValueNumber("numAttackSlots", asset->numAttackSlots);
			json.WriteFieldValueNumber("attackSpotHorzOffset", asset->attackSpotHorzOffset);

			if (asset->numBoardsInBarrier) {
				json.WriteFieldNameString("boards");
				json.BeginArray();
				for (size_t i = 0; i < asset->numBoardsInBarrier; i++) {
					ZBarrierBoard* board{ &asset->boards[i] };

					json.BeginObject();
					json.WriteFieldValueXAsset("boardModel", games::bo4::pool::ASSET_TYPE_XMODEL, board->pBoardModel);
					json.WriteFieldValueXAsset("alternateBoardModel", games::bo4::pool::ASSET_TYPE_XMODEL, board->pAlternateBoardModel);
					json.WriteFieldValueXAsset("upgradedBoardModel", games::bo4::pool::ASSET_TYPE_XMODEL, board->pUpgradedBoardModel);
					json.WriteFieldValueXHash("tearAnim", board->tearAnim);
					json.WriteFieldValueXHash("boardAnim", board->boardAnim);
					json.WriteFieldValueXHash("idleAnim", board->idleAnim);
					json.WriteFieldValueXHash("staticAnim", board->staticAnim);
					json.WriteFieldValueXHash("boardRepairSound", board->boardRepairSound);
					json.WriteFieldValueXHash("boardRepairHoverSound", board->boardRepairHoverSound);
					json.WriteFieldValueXHash("pauseAndRepeatRepSound", board->pauseAndRepeatRepSound);
					json.WriteFieldValueXAsset("repairEffect1", games::bo4::pool::ASSET_TYPE_FX, board->repairEffect1);
					json.WriteFieldValueVector("repairEffect1Offset", board->repairEffect1Offset);
					json.WriteFieldValueXAsset("repairEffect2", games::bo4::pool::ASSET_TYPE_FX, board->repairEffect2);
					json.WriteFieldValueVector("repairEffect2Offset", board->repairEffect2Offset);
					json.WriteFieldValueNumber("minPause", board->minPause);
					json.WriteFieldValueNumber("maxPause", board->maxPause);
					json.WriteFieldValueNumber("numRepsToPullProBoard", board->numRepsToPullProBoard);

					json.EndObject();
				}
				json.EndArray();
			}



			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_ZBARRIER };
}