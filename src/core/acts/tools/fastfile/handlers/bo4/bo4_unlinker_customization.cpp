#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/utils/data_utils.hpp>

namespace fastfile::handlers::bo4::map {
	using namespace games::bo4::pool;
	struct PlayerRoleLevels {
		bool enabled;
		PlayerRoleTemplate* levels;
	};
	static_assert(sizeof(PlayerRoleLevels) == 0x10);

	enum CharacterGenderTypes : uint32_t {
		CHARACTER_GENDER_TYPE_MALE = 0x0,
		CHARACTER_GENDER_TYPE_MP = 0x0,
		CHARACTER_GENDER_TYPE_FIRST = 0x0,
		CHARACTER_GENDER_TYPE_FEMALE = 0x1,
		CHARACTER_GENDER_TYPE_COUNT = 0x2,
		CHARACTER_GENDER_TYPE_INVALID = 0x2,
		CHARACTER_GENDER_TYPE_ANY = 0x3,
	};

	struct CharacterHead {
		XHash assetName;
		XHash displayName;
		GfxImage* icon;
		XModel* xmodel;
		CharacterGenderTypes gender;
		uint32_t unk34;
		CharMixer* charMixer;
		scriptbundle::SB_ObjectsArray bundle;
	};
	static_assert(sizeof(CharacterHead) == 0x60);

	struct CustomizationTable {
		XHash name;
		uint32_t numPlayerRoles;
		PlayerRoleLevels** playerRoles;
		uint32_t numHeads;
		CharacterHead* heads;
	};
	static_assert(sizeof(CustomizationTable) == 0x30);

	struct CustomizationTable_FEImages {
		XHash name;
		uint32_t count;
		const char* bodyTypes;
	};
	static_assert(sizeof(CustomizationTable_FEImages) == 0x20);

	class CustomizationTableWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			CustomizationTable& asset{ *(CustomizationTable*)ptr };


			BO4JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueNumber("numHeads", asset.numHeads);
			json.WriteFieldValueNumber("numPlayerRoles", asset.numPlayerRoles);
			if (asset.numHeads) {
				json.WriteFieldNameString("heads");
				json.BeginArray();
				for (size_t i = 0; i < asset.numHeads; i++) {
					CharacterHead* head{ &asset.heads[i] };
					json.BeginObject();
					json.WriteFieldValueXHash("name", head->assetName);
					json.WriteFieldValueXHash("displayName", head->displayName);
					json.WriteFieldValueXAsset("icon", ASSET_TYPE_IMAGE, head->icon);
					json.WriteFieldValueXAsset("xmodel", ASSET_TYPE_XMODEL, head->xmodel);
					json.WriteFieldValueXAsset("charMixer", ASSET_TYPE_charMIXER, head->charMixer);
					json.WriteFieldValueNumber("gender", (int)head->gender);
					json.WriteFieldValueUnknown("unk34", head->unk34);
					scriptbundle::WriteObject(json, "bundle", head->bundle);
					json.EndObject();
				}

				json.EndArray();
			}
			if (asset.numPlayerRoles) {
				json.WriteFieldNameString("playerRoles");
				json.BeginArray();
				for (size_t i = 0; i < asset.numPlayerRoles; i++) {
					PlayerRoleLevels* role{ asset.playerRoles[i] };
					if (!role) {
						json.WriteValueNull();
						continue;
					}
					json.BeginObject();
					json.WriteFieldValueBool("enabled", role->enabled);
					json.WriteFieldValueXAsset("template", ASSET_TYPE_PLAYER_ROLE_TEMPLATE, role->levels);
					json.EndObject();
				}
				json.EndArray();
			}
			json.EndObject();


			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "customization" / std::format("{}.json", hashutils::ExtractTmp("file", asset.name)) };

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump customizationtable {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	class CustomizationFEImagesTableWorker : public Worker {

		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			CustomizationTable_FEImages& asset{ *(CustomizationTable_FEImages*)ptr };


			BO4JsonWriter json{};

			json.BeginObject();
			json.WriteFieldValueXHash("name", asset.name);
			json.WriteFieldValueNumber("count", asset.count);

			if (asset.bodyTypes) {
				json.WriteFieldNameString("bodyTypes");
				json.BeginArray();
				for (size_t i = 0; i < asset.count; i++) {
					json.WriteValueNumber((int)asset.bodyTypes[i]);
				}
				json.EndArray();
			}

			json.EndObject();


			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "customization" / "feimages" / std::format("{}.json", hashutils::ExtractTmp("file", asset.name))};

			std::filesystem::create_directories(outFile.parent_path());
			LOG_OPT_INFO("Dump customizationtable {}", outFile.string());

			if (!json.WriteToFile(outFile)) {
				LOG_ERROR("Error when dumping {}", outFile.string());
			}
		}
	};

	utils::MapAdder<CustomizationTableWorker, games::bo4::pool::XAssetType, Worker> implcom{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_CUSTOMIZATION_TABLE };
	utils::MapAdder<CustomizationFEImagesTableWorker, games::bo4::pool::XAssetType, Worker> implcomimg{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES };
}