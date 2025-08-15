#include <includes.hpp>
#include <tools/ff/handlers/handler_game_bo4.hpp>
#include <tools/ff/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <hook/error.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	enum ObjectiveVAlign : uint32_t {
		OBVA_TOP = 0,
		OBVA_CENTER = 1,
		OBVA_BOTTOM = 2,
		OBVA_COUNT = 3,
	};
	enum ObjectiveHAlign : uint32_t {
		OBHA_LEFT = 0,
		OBHA_CENTER = 1,
		OBHA_RIGHT = 2,
		OBHA_COUNT = 3,
	};

	const char* VALIGN[]{
		"TOP", "CENTER", "BOTTOM"
	};

	const char* HALIGN[]{
		"LEFT", "CENTER", "RIGHT"
	};

	class ImplWorker : public Worker {
		using Worker::Worker;

		enum ObjectiveCullingGroupType : int32_t {
			OBJ_CULL_GROUP_AMMO = 0x0,
			OBJ_CULL_GROUP_LOW_AMMO = 0x1,
			OBJ_CULL_GROUP_UNIQUE = 0x2,
			OBJ_CULL_GROUP_COUNT = 0x3,
		};

		struct ObjectiveCullGroup {
			ObjectiveCullingGroupType type;
			int32_t keepClosestCount;
		};

		enum ObjectiveAmmoTypes : int32_t {
			OBJECTIVE_SHOWN_AMMO_STATE_ALL = 0x0,
			OBJECTIVE_SHOWN_AMMO_STATE_LOW = 0x1,
			OBJECTIVE_SHOWN_AMMO_STATE_NOT_LOW = 0x2,
			OBJECTIVE_SHOWN_AMMO_STATE_COUNT = 0x3,
		};

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			struct Objective {
				XHash name;
				ObjectiveHAlign hAlign;
				ObjectiveVAlign vAlign;
				vec3_t waypointOffset;
				byte unk24;
				byte unk25;
				bool waypointShowDistance;
				uint32_t unk28;
				bool waypointHideArrow;
				bool waypointClamp;
				uint32_t unk30;
				float unk34;
				bool waypointFadeWhenTargeted;
				byte unk39;
				byte unk3a;
				byte unk3b;
				byte unk3c;
				byte unk3d;
				byte unk3e;
				byte unk3f;
				byte unk40;
				byte unk41;
				byte unk42;
				byte unk43;
				bool waypointFadeWhenInCombat;
				byte unk49;
				byte unk4a;
				bool waypointShowDistanceWhenFar;
				float waypointShowDistanceWhenFarDist;
				bool waypointForceOffscreenStyle;
				bool waypointHideWhenNotVisible;
				bool shouldCullDistance;
				byte unk4f;
				float cullDistance;
				bool unk54;
				byte unk55;
				byte unk56;
				byte unk57;
				float unk58;
				uint32_t unk5c;
				float unk60;
				float unk64;
				GfxImage* waypointImage;
				XHash waypointText;
				GfxImage* objectiveImage;
				XHash description;
				XHash friendlyDescription;
				XHash enemyDescription;
				XHash subObjectiveDescription;
				GfxImage* subObjectiveImage;
				XHash notifyString;
				uint32_t unke0;
				uint32_t unke4;
				bool hasLowAmmoState;
				scriptbundle::SB_ObjectsArray bundle;
			};

			static_assert(sizeof(Objective) == 0x110);

			Objective* asset{ (Objective*)ptr };

			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "objective" / GetFFContext()->ffname / std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
			std::filesystem::create_directories(outFile.parent_path());
			BO4JsonWriter json{};

			LOG_INFO("Dump objective {}", outFile.string());

			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldValueString("hAlign", asset->hAlign >= ARRAYSIZE(HALIGN) ? "invalid" : HALIGN[asset->hAlign]);
			json.WriteFieldValueString("vAlign", asset->vAlign >= ARRAYSIZE(VALIGN) ? "invalid" : VALIGN[asset->vAlign]);
			json.WriteFieldValueString("waypointOffset", std::format("{}, {}, {}", asset->waypointOffset[0], asset->waypointOffset[1], asset->waypointOffset[2]));
			json.WriteFieldValueBool("waypointShowDistance", asset->waypointShowDistance);
			json.WriteFieldValueBool("waypointHideArrow", asset->waypointHideArrow);
			json.WriteFieldValueBool("waypointClamp", asset->waypointClamp);
			json.WriteFieldValueBool("waypointFadeWhenTargeted", asset->waypointFadeWhenTargeted);
			json.WriteFieldValueBool("waypointFadeWhenInCombat", asset->waypointFadeWhenTargeted);
			json.WriteFieldValueBool("waypointShowDistanceWhenFar", asset->waypointShowDistanceWhenFar);
			json.WriteFieldValueNumber("waypointShowDistanceWhenFarDist", asset->waypointShowDistanceWhenFarDist);
			json.WriteFieldValueBool("waypointForceOffscreenStyle", asset->waypointForceOffscreenStyle);
			json.WriteFieldValueBool("waypointHideWhenNotVisible", asset->waypointHideWhenNotVisible);
			json.WriteFieldValueBool("shouldCullDistance", asset->shouldCullDistance);
			json.WriteFieldValueNumber("cullDistance", asset->cullDistance);
			json.WriteFieldValueBool("hasLowAmmoState", asset->hasLowAmmoState);
			json.WriteFieldValueXHash("waypointText", asset->waypointText);
			json.WriteFieldValueXHash("description", asset->description);
			json.WriteFieldValueXHash("friendlyDescription", asset->friendlyDescription);
			json.WriteFieldValueXHash("enemyDescription", asset->enemyDescription);
			json.WriteFieldValueXHash("subObjectiveDescription", asset->subObjectiveDescription);
			json.WriteFieldValueXHash("notifyString", asset->notifyString);
			json.WriteFieldValueXAsset("objectiveImage", games::bo4::pool::ASSET_TYPE_IMAGE, asset->objectiveImage);
			json.WriteFieldValueXAsset("waypointImage", games::bo4::pool::ASSET_TYPE_IMAGE, asset->waypointImage);
			json.WriteFieldValueXAsset("subObjectiveImage", games::bo4::pool::ASSET_TYPE_IMAGE, asset->subObjectiveImage);


			if (asset->unk24) json.WriteFieldValueNumber("unk24", (int)asset->unk24); // unused?
			if (asset->unk25) json.WriteFieldValueNumber("unk25", (int)asset->unk25);
			if (asset->unk28) json.WriteFieldValueUnknown("unk28", asset->unk28); // unused?
			if (asset->unk30) json.WriteFieldValueNumber("unk30", asset->unk30);
			if (asset->unk34) json.WriteFieldValueNumber("unk34", asset->unk34); // 0.08?
			if (asset->unk4f) json.WriteFieldValueNumber("unk4f", (int)asset->unk4f); // pad?
			if (asset->unk54) json.WriteFieldValueNumber("unk54", (int)asset->unk54);
			if (asset->unk55) json.WriteFieldValueNumber("unk55", (int)asset->unk55); // unused?
			if (asset->unk56) json.WriteFieldValueNumber("unk56", (int)asset->unk56); // unused?
			if (asset->unk57) json.WriteFieldValueNumber("unk57", (int)asset->unk57); // unused?
			if (asset->unk58) json.WriteFieldValueNumber("unk58", asset->unk58);
			if (asset->unk5c)json.WriteFieldValueUnknown("unk5c", asset->unk5c); // pad?
			if (asset->unk60) json.WriteFieldValueNumber("unk60", asset->unk60); //70?
			if (asset->unk64) json.WriteFieldValueNumber("unk64", asset->unk64); //40?

			bool error{};
			json.WriteFieldNameString("bundle");
			fastfile::handlers::bo4::scriptbundle::WriteObject(json, asset->bundle, error);
			if (error) {
				return;
			}
			json.EndObject();


			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_OBJECTIVE };
}