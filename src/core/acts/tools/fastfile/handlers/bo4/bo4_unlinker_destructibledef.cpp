#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/utils/data_utils.hpp>

namespace {
	using namespace fastfile::handlers::bo4;
	struct PhysConstraints;
	struct PhysPreset;

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct DestructibleStage {
				XHash name;
				PhysPreset* physPreset;
				uint64_t unk18;
				FxEffectDefHandle baseEffect;
				ScrString_t unk28;
				ScrString_t unk2c[4];
				ScrString_t unk3c;
				uint64_t unk40;
				uint32_t unk48;
				uint32_t unk4c;
			};
			static_assert(sizeof(DestructibleStage) == 0x50);

			typedef int32_t XPartBits[12];
			struct DestructiblePiece {
				DestructibleStage stages[7];
				byte parentPiece;
				byte subtargetID;
				float parentDamagePercent;
				float bulletDamageScale;
				float explosiveDamageScale;
				float meleeDamageScale;
				float impactDamageScale;
				float entityDamageTransfer;
				vec3_t subtargetOffset;
				ScrString_t subtargetBone;
				PhysConstraints* physConstraints;
				uint64_t unk268;
				XHash damageSound;
				FxEffectDefHandle burnEffect;
				XHash burnSound;
				ScrString_t enableLabel;
				XPartBits hideBones;
			};
			static_assert(sizeof(DestructiblePiece) == 0x2d0);

			struct DestructibleDef {
				XHash name;
				XModel* model;
				XModel* pristineModel;
				XModel* unk20;
				int32_t numPieces;
				DestructiblePiece* pieces;
				bool clientOnly;
				bool syncBaseHealthWithEntity;
			};
			static_assert(sizeof(DestructibleDef) == 0x40);

			DestructibleDef* asset{ (DestructibleDef*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name) };
			std::filesystem::path outDir{ opt.m_output / "bo4" / "source" / "tables" / "destructibledef"};
			std::filesystem::path outFile{ outDir / std::format("{}.json", n) };
			std::filesystem::create_directories(outDir);
			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump destructibledef {}", outFile.string());

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueXAsset("model", games::bo4::pool::XAssetType::ASSET_TYPE_XMODEL, asset->model);
			json.WriteFieldValueXAsset("pristineModel", games::bo4::pool::XAssetType::ASSET_TYPE_XMODEL, asset->pristineModel);
			json.WriteFieldValueXAsset("unk20", games::bo4::pool::XAssetType::ASSET_TYPE_XMODEL, asset->unk20);
			json.WriteFieldValueBool("clientOnly", asset->clientOnly);
			json.WriteFieldValueBool("syncBaseHealthWithEntity", asset->syncBaseHealthWithEntity);

			json.WriteFieldNameString("pieces");
			json.BeginArray();
			for (size_t i = 0; i < asset->numPieces; i++) {
				DestructiblePiece* piece = asset->pieces + i;

				json.BeginObject();
				json.WriteFieldNameString("stages");
				json.BeginArray();
				for (size_t j = 0; j < ARRAYSIZE(piece->stages); j++) {
					DestructibleStage* stage{ piece->stages + j };

					json.BeginObject();
					json.WriteFieldValueXHash("name", stage->name);
					json.WriteFieldValueXAsset("physPreset", games::bo4::pool::XAssetType::ASSET_TYPE_PHYSPRESET, stage->physPreset);
					json.WriteFieldValueXAsset("baseEffect", games::bo4::pool::XAssetType::ASSET_TYPE_FX, stage->baseEffect);
					json.WriteFieldValueScrString("unk28", stage->unk28);
					json.WriteFieldValueScrStringArray("unk2c", ARRAYSIZE(stage->unk2c), stage->unk2c);
					json.WriteFieldValueScrString("unk3c", stage->unk3c);

					json.EndObject();

				}
				json.EndArray();

				json.WriteFieldValueNumber<int>("parentPiece", piece->parentPiece);
				json.WriteFieldValueNumber<int>("subtargetID", piece->subtargetID);
				json.WriteFieldValueNumber("parentDamagePercent", piece->parentDamagePercent);
				json.WriteFieldValueNumber("bulletDamageScale", piece->bulletDamageScale);
				json.WriteFieldValueNumber("explosiveDamageScale", piece->explosiveDamageScale);
				json.WriteFieldValueNumber("meleeDamageScale", piece->meleeDamageScale);
				json.WriteFieldValueNumber("impactDamageScale", piece->impactDamageScale);
				json.WriteFieldValueNumber("entityDamageTransfer", piece->entityDamageTransfer);
				json.WriteFieldValueString("subtargetOffset", std::format("({}, {}, {})", piece->subtargetOffset[0], piece->subtargetOffset[1], piece->subtargetOffset[2]));
				json.WriteFieldValueXHash("damageSound", piece->damageSound);
				json.WriteFieldValueXHash("burnSound", piece->burnSound);
				json.WriteFieldValueScrString("subtargetBone", piece->subtargetBone);
				json.WriteFieldValueScrString("enableLabel", piece->enableLabel);
				json.WriteFieldValueXAsset("burnEffect", games::bo4::pool::XAssetType::ASSET_TYPE_FX, piece->burnEffect);
				json.WriteFieldValueXAsset("physConstraints", games::bo4::pool::XAssetType::ASSET_TYPE_PHYSCONSTRAINTS, piece->physConstraints);
				json.WriteFieldValueNumber("unk268", piece->unk268);
				json.WriteFieldNameString("hideBones");
				json.BeginArray();
				constexpr size_t numbits = sizeof(piece->hideBones) * 8;
				constexpr size_t numbitsField = sizeof(piece->hideBones[0]) * 8;
				for (size_t j = 0; j < numbits; j++) {
					size_t field{ j / (sizeof(piece->hideBones[0]) * 8) };
					size_t fbit{ j % numbitsField };
					int bit{ (piece->hideBones[field] >> fbit) & 1 };
					if (bit) {
						json.WriteValueNumber(j);
					}
				}
				json.EndArray();

				json.EndObject();
			}
			json.EndArray();

			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_DESTRUCTIBLEDEF };
}