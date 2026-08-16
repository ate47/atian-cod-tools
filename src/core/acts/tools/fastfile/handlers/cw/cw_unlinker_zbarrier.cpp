#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>
#include <tools/fastfile/handlers/cw/cw_unlinker_scriptbundle.hpp>

namespace {
    using namespace fastfile::handlers::cw;

    struct ZBarrierBoard {
        XModel* pBoardModel;
        XModel* pAlternateBoardModel;
        XModel* pUpgradedBoardModel;
        CWXHash tearAnim;
        CWXHash boardAnim;
        CWXHash idleAnim;
        CWXHash staticAnim;
        FxEffectDef* repairEffect1;
        FxEffectDef* repairEffect2;
        vec3_t repairEffect1Offset;
        vec3_t repairEffect2Offset;
        CWXHash boardRepairSound;
        CWXHash boardRepairHoverSound;
        CWXHash pauseAndRepeatRepSound;
        float minPause;
        float maxPause;
        uint32_t numRepsToPullProBoard;
    };
    static_assert(sizeof(ZBarrierBoard) == 0x88);

    struct ZBarrierDef {
        CWXHash name;
        CWXHash generalRepairSound1;
        CWXHash generalRepairSound2;
        CWXHash upgradedGeneralRepairSound1;
        CWXHash upgradedGeneralRepairSound2;
        CWXHash delayBetweenRepSounds;
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
        CWXHash zombieTauntAnimState;
        CWXHash zombieReachThroughAnimState;
        int numAttackSlots;
        float attackSpotHorzOffset;
        XModel* pCollisionModel;
        ZBarrierBoard boards[6];
    };
    static_assert(sizeof(ZBarrierDef) == 0x3b0);

    class ImplWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            ZBarrierDef* asset{ (ZBarrierDef*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "zbarrier" /
                                           fastfile::GetCurrentContext().ffname /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
            std::filesystem::create_directories(outFile.parent_path());
            BOCWJsonWriter json{};

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
            json.WriteFieldValueXAsset("collisionModel", XAssetType::ASSET_TYPE_XMODEL, asset->pCollisionModel);
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
                    json.WriteFieldValueXAsset("boardModel", XAssetType::ASSET_TYPE_XMODEL, board->pBoardModel);
                    json.WriteFieldValueXAsset(
                        "alternateBoardModel",
                        XAssetType::ASSET_TYPE_XMODEL,
                        board->pAlternateBoardModel
                    );
                    json.WriteFieldValueXAsset(
                        "upgradedBoardModel",
                        XAssetType::ASSET_TYPE_XMODEL,
                        board->pUpgradedBoardModel
                    );
                    json.WriteFieldValueXHash("tearAnim", board->tearAnim);
                    json.WriteFieldValueXHash("boardAnim", board->boardAnim);
                    json.WriteFieldValueXHash("idleAnim", board->idleAnim);
                    json.WriteFieldValueXHash("staticAnim", board->staticAnim);
                    json.WriteFieldValueXHash("boardRepairSound", board->boardRepairSound);
                    json.WriteFieldValueXHash("boardRepairHoverSound", board->boardRepairHoverSound);
                    json.WriteFieldValueXHash("pauseAndRepeatRepSound", board->pauseAndRepeatRepSound);
                    json.WriteFieldValueXAsset("repairEffect1", XAssetType::ASSET_TYPE_FX, board->repairEffect1);
                    json.WriteFieldValueVector("repairEffect1Offset", board->repairEffect1Offset);
                    json.WriteFieldValueXAsset("repairEffect2", XAssetType::ASSET_TYPE_FX, board->repairEffect2);
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

    utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_ZBARRIER };
} // namespace