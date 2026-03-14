#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <tools/utils/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_scriptbundle.hpp>

namespace {
	using namespace fastfile::handlers::bo4;

	union GfxColor {
		uint32_t packed;
		byte array[4];
	};

	struct CamoMaterialChannel {
		Material* camoMaterial;
		float translationX;
		float translationY;
		float scaleX;
		float scaleY;
		float rotation;
		float normalBlend;
		float glossBlend;
		GfxColor albedoTint;
		GfxImage* detailMap;
		float detailHeight;
		vec2_t detailScale;
	}; static_assert(sizeof(CamoMaterialChannel) == 0x40);

	struct CamoMaterial {
		Material** baseMaterials;
		GfxImage** baseMaterialMasks;
		CamoMaterialChannel* camoMaterialChannels;
		uint16_t numBaseMaterials;
		uint16_t unk1a;
		uint32_t unk1c;
	}; static_assert(sizeof(CamoMaterial) == 0x20);

	struct WeaponCamoMaterialSet {
		CamoMaterial* materials;
		uint32_t numMaterials;
	}; static_assert(sizeof(WeaponCamoMaterialSet) == 0x10);

	struct WeaponCamo {
		XHash name;
		WeaponCamoMaterialSet* camoMaterials;
		uint32_t numCamoMaterials;
	};static_assert(sizeof(WeaponCamo) == 0x20);

	class WeaponCamoWorker : public Worker {
		void Unlink(fastfile::FastFileOption& opt, void* ptr) {
			WeaponCamo* asset{ (WeaponCamo*)ptr };

			const char* n{ hashutils::ExtractTmp("file", asset->name.name) };
			std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "weaponcamo" / std::format("{}.json", n) };
			std::filesystem::create_directories(outFile.parent_path());

			BO4JsonWriter json{};

			LOG_OPT_INFO("Dump weaponcamo {}", outFile.string());
			json.BeginObject();

			json.WriteFieldValueXHash("name", asset->name);
			json.WriteFieldNameString("camoMaterials");
			json.BeginArray();

			for (size_t i = 0; i < asset->numCamoMaterials; i++) {
				WeaponCamoMaterialSet* set{ asset->camoMaterials + i };
				json.BeginObject();
				json.WriteFieldNameString("materials");
				json.BeginArray();
				for (size_t j = 0; j < set->numMaterials; j++) {
					CamoMaterial* material{ set->materials + j };
					json.BeginObject();
					json.WriteFieldNameString("baseMaterials");
					json.BeginArray();
					for (size_t k = 0; k < material->numBaseMaterials; k++) {
						if (!material->baseMaterials[k] && !material->baseMaterialMasks[k]) {
							continue;
						}

						json.BeginObject();
						json.WriteFieldValueXAsset("material", games::bo4::pool::ASSET_TYPE_MATERIAL, material->baseMaterials[k]);
						json.WriteFieldValueXAsset("mask", games::bo4::pool::ASSET_TYPE_IMAGE, material->baseMaterialMasks[k]);
						json.EndObject();
					}
					json.EndArray();

					//json.WriteFieldValueNumber<decltype(material->unk1a), false, true> ("unk1a", material->unk1a);
					//json.WriteFieldValueNumber<decltype(material->unk1c), false, true>("unk1c", material->unk1c);
					json.WriteFieldNameString("camoMaterialChannels");
					json.BeginArray();
					for (size_t k = 0; k < material->numBaseMaterials; k++) {
						CamoMaterialChannel* channel{ material->camoMaterialChannels + k };

						json.BeginObject();
						json.WriteFieldValueXAsset("camoMaterial", games::bo4::pool::ASSET_TYPE_MATERIAL, channel->camoMaterial);
						json.WriteFieldValueXAsset("detailMap", games::bo4::pool::ASSET_TYPE_IMAGE, channel->detailMap);
						json.WriteFieldValueNumber("translationX", channel->translationX);
						json.WriteFieldValueNumber("translationY", channel->translationY);
						json.WriteFieldValueNumber("scaleX", channel->scaleX);
						json.WriteFieldValueNumber("scaleY", channel->scaleY);
						json.WriteFieldValueNumber("rotation", channel->rotation);
						json.WriteFieldValueNumber("normalBlend", channel->normalBlend);
						json.WriteFieldValueNumber("glossBlend", channel->glossBlend);
						json.WriteFieldValueNumber("detailHeight", channel->detailHeight);
						json.WriteFieldValueString("detailScale", std::format("({}, {})", channel->detailScale[0], channel->detailScale[1]));
						json.WriteFieldNameString("albedoTint");
						json.WriteValueNumber(channel->albedoTint.packed);
						/*
						json.BeginObject();
						json.WriteFieldValueNumber("packed", channel->albedoTint.packed);
						json.WriteFieldNameString("array");
						{
							json.BeginArray();
							json.WriteValueNumber<int>(channel->albedoTint.array[0]);
							json.WriteValueNumber<int>(channel->albedoTint.array[1]);
							json.WriteValueNumber<int>(channel->albedoTint.array[2]);
							json.WriteValueNumber<int>(channel->albedoTint.array[3]);
							json.EndArray();
						}
						json.EndObject();
						*/

						json.EndObject();
					}
					json.EndArray();
					

					json.EndObject();
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

	utils::MapAdder<WeaponCamoWorker, games::bo4::pool::XAssetType, Worker> impl{ GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_WEAPON_CAMO };
}