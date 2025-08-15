#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>
#include <tools/ff/linkers/bo4/linker_bo4_scriptbundle.hpp>
#include <tools/ff/linkers/bo4/linker_bo4_gfx_image.hpp>

namespace {
	using namespace fastfile::linker::bo4;
	class ImplWorker : public LinkerWorker {
	public:
		ImplWorker() : LinkerWorker("Objective") {}
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

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["objective"]) {
				assval.handled = true;
				std::filesystem::path path{ ctx.linkCtx.input / assval.value };
				std::filesystem::path rfpath{ path.filename() };
				rfpath.replace_extension();

				core::config::Config objCfg{ path };
				
				if (!objCfg.SyncConfig(false)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				ctx.data.AddAsset(games::bo4::pool::ASSET_TYPE_OBJECTIVE, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				Objective obj{};


				static core::config::ConfigEnumData obvaCfg[]{
					{ "BOTTOM", ObjectiveVAlign::OBVA_BOTTOM },
					{ "CENTER", ObjectiveVAlign::OBVA_CENTER },
					{ "TOP", ObjectiveVAlign::OBVA_TOP },
				};
				static core::config::ConfigEnumData obhaCfg[]{
					{ "LEFT", ObjectiveHAlign::OBHA_LEFT },
					{ "CENTER", ObjectiveHAlign::OBHA_CENTER },
					{ "RIGHT", ObjectiveHAlign::OBHA_RIGHT },
				};
				obj.hAlign = objCfg.GetEnumVal<ObjectiveHAlign>("hAlign", obvaCfg, ARRAYSIZE(obvaCfg), ObjectiveHAlign::OBHA_LEFT);
				obj.vAlign = objCfg.GetEnumVal<ObjectiveVAlign>("vAlign", obhaCfg, ARRAYSIZE(obhaCfg), ObjectiveVAlign::OBVA_TOP);

				std::string rfpathStr{ rfpath.string() };
				obj.name.name = ctx.HashXHash(objCfg.GetString("name", rfpathStr.c_str()), true);
				obj.waypointShowDistance = objCfg.GetBool("waypointShowDistance");
				obj.waypointHideArrow = objCfg.GetBool("waypointHideArrow");
				obj.waypointClamp = objCfg.GetBool("waypointClamp");
				obj.waypointFadeWhenTargeted = objCfg.GetBool("waypointFadeWhenTargeted");
				obj.waypointFadeWhenInCombat = objCfg.GetBool("waypointFadeWhenInCombat");
				obj.waypointShowDistanceWhenFar = objCfg.GetBool("waypointShowDistanceWhenFar");
				obj.waypointForceOffscreenStyle = objCfg.GetBool("waypointForceOffscreenStyle");
				obj.waypointHideWhenNotVisible = objCfg.GetBool("waypointHideWhenNotVisible");
				obj.shouldCullDistance = objCfg.GetBool("shouldCullDistance");
				obj.hasLowAmmoState = objCfg.GetBool("hasLowAmmoState");

				obj.waypointText.name = ctx.HashXHash(objCfg.GetString("waypointText"), true);
				obj.description.name = ctx.HashXHash(objCfg.GetString("description"), true);
				obj.friendlyDescription.name = ctx.HashXHash(objCfg.GetString("friendlyDescription"), true);
				obj.enemyDescription.name = ctx.HashXHash(objCfg.GetString("enemyDescription"), true);
				obj.subObjectiveDescription.name = ctx.HashXHash(objCfg.GetString("subObjectiveDescription"), true);
				obj.notifyString.name = ctx.HashXHash(objCfg.GetString("notifyString"), true);
				
				obj.cullDistance = (float)objCfg.GetDouble("cullDistance");
				obj.waypointShowDistanceWhenFarDist = (float)objCfg.GetDouble("waypointShowDistanceWhenFarDist");

				// unk fields
				obj.unk24 = (byte)objCfg.GetInteger("unk24", 0);
				obj.unk25 = (byte)objCfg.GetInteger("unk25", 0);
				obj.unk28 = (uint32_t)objCfg.GetInteger("unk28", 0);
				obj.unk30 = (uint32_t)objCfg.GetInteger("unk30", 0);
				obj.unk34 = (float)objCfg.GetDouble("unk34", 0.08);
				obj.unk4f = (byte)objCfg.GetInteger("unk4f", 0);
				obj.unk54 = (byte)objCfg.GetInteger("unk54", 0);
				obj.unk55 = (byte)objCfg.GetInteger("unk55", 0);
				obj.unk56 = (byte)objCfg.GetInteger("unk56", 0);
				obj.unk57 = (byte)objCfg.GetInteger("unk57", 0);
				obj.unk58 = (float)objCfg.GetDouble("unk58", 0);
				obj.unk5c = (uint32_t)objCfg.GetInteger("unk5c", 0);
				obj.unk60 = (float)objCfg.GetDouble("unk60", 70);
				obj.unk64 = (float)objCfg.GetDouble("unk64", 40);
				if (!objCfg.ScanString("waypointOffset", "%f, %f, %f", &obj.waypointOffset[0], &obj.waypointOffset[1], &obj.waypointOffset[2])) {
					LOG_ERROR("Bad format for waypointOffset \"123, 123, 123\"");
					ctx.error = true;
					continue;
				}

				std::string waypointImage{ objCfg.GetString("waypointImage") };
				std::string objectiveImage{ objCfg.GetString("objectiveImage") };
				std::string subObjectiveImage{ objCfg.GetString("subObjectiveImage") };
				if (!waypointImage.empty()) obj.waypointImage = (GfxImage*)fastfile::ALLOC_PTR;
				if (!objectiveImage.empty()) obj.objectiveImage = (GfxImage*)fastfile::ALLOC_PTR;
				if (!subObjectiveImage.empty()) obj.subObjectiveImage = (GfxImage*)fastfile::ALLOC_PTR;

				size_t objData{ ctx.data.WriteData(obj) };
				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);

				// link images
				if (!waypointImage.empty() && !image::LinkGfxImagePtr(ctx, waypointImage.data())) continue;
				if (!objectiveImage.empty() && !image::LinkGfxImagePtr(ctx, objectiveImage.data())) continue;
				if (!subObjectiveImage.empty() && !image::LinkGfxImagePtr(ctx, subObjectiveImage.data())) continue;

				// link bundle
				auto bundle = objCfg.main.FindMember("bundle");
				if (bundle != objCfg.main.MemberEnd() && !scriptbundle::WriteArray(ctx, bundle->value, objData + offsetof(Objective, bundle))) {
					continue;
				}
				ctx.data.PopStream();

				ctx.data.PopStream();

				LOG_INFO("Added asset objective {} (hash_{:x})", rfpath.string(), obj.name.name);
			}
		}
	};

	utils::ArrayAdder<ImplWorker, LinkerWorker> impl{ GetWorkers() };
}